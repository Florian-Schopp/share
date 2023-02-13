#include "ClipX.h"
#include <time.h>
#include <stdio.h>
#include <chrono>
#include <thread>
#include <mutex>
#include <fstream>
#include "Types.h"
#include "Structs.h"
#include <iostream>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>

#define closesocket close
#define sprintf_s sprintf
#define sscanf_s sscanf
char *strcpy_s(char *dest, size_t n, const char *src)
{
	size_t i;

	for (i = 0; i < n && src[i] != '\0'; i++)
		dest[i] = src[i];
	for (; i < n; i++)
		dest[i] = '\0';

	return dest;
}
#endif

using namespace std;

std::mutex g_buffer_mutex;
std::mutex g_socket_mutex;

std::thread MeasureThread, keepAliveThread;
// std::ofstream fifoLog;

ClipX::ClipX()
{
}

int ClipX::WaitForLineFeed()
{
	int rcvd = 0;
	char buf[100];
	do
	{
		rcvd = recv(sock, buf, rcvd + 1, MSG_PEEK);
		if (rcvd < 0)
			return -1;
	} while (!(rcvd >= 2 && buf[rcvd - 2] == '\r' && buf[rcvd - 1] == '\n'));
	return rcvd;
}

bool ClipX::GetOverFlowFlag()
{
	return overFlow;
}

int CLIPX_GetSignalTypeByID(int ID, char *data, int length)
{
	if ((ID < 2) || (ID > 31))
	{
		return -1;
	}
	else
	{
		if ((size_t)length < strlen(signalNames[ID]))
		{
			return -2;
		}
		else
		{
			strcpy_s(data, 40, signalNames[ID]);
			return 0;
		}
	}
}

int CLIPX_GetSignalIDByType(char *type)
{
	for (int i = 2; i < 32; i++)
	{
		if (strncmp(type, signalNames[i], strlen(type)) == 0)
		{
			return i;
		}
	}
	return -1;
}

int CLIPX_GetTransducerTypeByID(int ID, char *data, int length)
{
	if ((ID < 0) || (ID > 14))
	{
		return -1;
	}
	else
	{
		if ((size_t)length < strlen(transducerTypes[ID]))
		{
			return -2;
		}
		else
		{
			strcpy_s(data, 40, transducerTypes[ID]);
			return 0;
		}
	}
}

void ClipX::ClearBuffer()
{
	readptr = writeptr;
}

int ClipX::SDORead(int idx, int subidx, char *res, int size)
{
	if (!connected)
		return -1;
	std::cout << "Preparing request" << std::endl;
	char req[100];
	sprintf_s(req, "SDO? %d,%d \r\n", idx, subidx);
	g_socket_mutex.lock();
	send(sock, req, strlen(req), 0);
	int rcvd = recv(sock, res, WaitForLineFeed(), 0);

	res[rcvd] = '\0';
	g_socket_mutex.unlock();
	return rcvd;
}

int ClipX::SDOWrite(int idx, int subidx, const char *value)
{
	if (!connected)
		return -1;
	if (strlen(value) > 100)
		return -1;
	char req[150];
	sprintf_s(req, "SDO %d,%d,%s \r\n", idx, subidx, value);
	g_socket_mutex.lock();
	send(sock, req, strlen(req), 0);
	recv(sock, req, WaitForLineFeed(), 0);
	g_socket_mutex.unlock();
	if (req[0] == '0')
		return 0;
	else
		return -1;
}

int ClipX::startMeasurement()
{
	if (!connected)
		return -1;
	// Entries = Entries_empty;
	// Header = Header_empty;
	// if (Measuring)return -1;
	keepAlive = false;
	SDOWrite(0x4428, 1, "0");
	if (keepAliveThread.joinable())
		keepAliveThread.join();

	// fifoLog.open("C:/temp/FifoLog.csv");
	const char *req = "RMB?\r\n";
	overFlow = false;
	g_socket_mutex.lock();
	send(sock, req, strlen(req), 0);
	recv(sock, Header.Chararray, sizeof(Header), MSG_WAITALL);
	if (Header.res.dataBytes > 0)
		recv(sock, Entries.Chararray, Header.res.dataBytes, MSG_WAITALL);
	g_socket_mutex.unlock();
	SDOWrite(0x4428, 1, "1");
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	char res[100];
	SDORead(0x4428, 9, res, 100);
	struct tm tm;
	sscanf_s(res, "%d.%d.%d %d:%d:%d", &tm.tm_mday, &tm.tm_mon, &tm.tm_year, &tm.tm_hour, &tm.tm_min, &tm.tm_sec);
	tm.tm_year += 100;
	tm.tm_mon -= 1;
	timeStartEpoch = mktime(&tm);
	Measuring = true;
	MeasureThread = std::thread(&ClipX::MeasureFun, this);
	return 0;
}

void ClipX::MeasureFun()
{
	ReadFifo();
	while (Measuring)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		ReadFifo();
	}
}

int ClipX::ReadFifo()
{
	const char *req = "RMB?\r\n";
	g_socket_mutex.lock();
	if (send(sock, req, strlen(req), 0) < 0)
		return -1;
	if (recv(sock, Header.Chararray, sizeof(Header), MSG_WAITALL) < 0)
		return -1;
	if (recv(sock, Entries.Chararray, Header.res.dataBytes, MSG_WAITALL) != Header.res.dataBytes)
		return -1;
	g_socket_mutex.unlock();
	g_buffer_mutex.lock();

	if ((writeptr + Header.res.dataBytes / 28) < buffersize)
		memcpy((fifo + writeptr), Entries.res, Header.res.dataBytes);
	else
	{
		memcpy((fifo + writeptr), Entries.res, (buffersize - writeptr) * 28);
		int pos = buffersize - writeptr;
		int size = (Header.res.dataBytes / 28 - (buffersize - writeptr)) * 28;
		memcpy((fifo), Entries.res + pos, size);
	}

	int writeptrold = writeptr;
	writeptr += (Header.res.dataBytes / 28);
	writeptr %= buffersize;
	if (BETWEEN(readptr, writeptrold, writeptr))
	{
		overFlow = true;
		readptr = (writeptr + 1) % buffersize;
	}
	g_buffer_mutex.unlock();
	// sprintf_s(buf, "%i;  %i ; %i ; %i", 0, readptr, writeptr, (int)(Header.res.dataBytes / 28));
	// fifoLog << buf << std::endl;
	return Header.res.dataBytes / 28;
}

int ClipX::AvailableLines()
{
	int nrLines = writeptr - readptr;
	if (nrLines < 0)
		nrLines += buffersize;
	return nrLines;
}

int ClipX::ReadNextLine(double *MVLine)
{
	if (readptr == writeptr)
		return -1;
	g_buffer_mutex.lock();
	EntryStruct resultLine = fifo[readptr];
	readptr = (readptr + 1) % buffersize;
	g_buffer_mutex.unlock();
	long double t = resultLine.tick;
	MVLine[0] = timeStartEpoch + (t / 1000);
	MVLine[1] = resultLine.value1;
	MVLine[2] = resultLine.value2;
	MVLine[3] = resultLine.value3;
	MVLine[4] = resultLine.value4;
	MVLine[5] = resultLine.value5;
	MVLine[6] = resultLine.value6;
	// char buf[2048];
	// sprintf_s(buf, "%f;  %i ; %i ; %i ; %f; %f; %f; %f; %f; %f; %f", t, readptr, writeptr, (int)(Header.res.dataBytes / 28),MVLine[0], MVLine[1], MVLine[2], MVLine[3], MVLine[4], MVLine[5], MVLine[6]);
	// fifoLog << buf << std::endl;
	return AvailableLines();
}

int ClipX::ReadNextBlock(int maxreads, double *time, double *value1, double *value2, double *value3, double *value4, double *value5, double *value6)
{
	if (readptr == writeptr)
		return -1;
	int reads = AvailableLines();

	// input reads more than avaialable reads?
	if (maxreads < reads)
		reads = maxreads;

	for (int i = 0; i < reads; i++)
	{
		g_buffer_mutex.lock();
		EntryStruct resultLine = fifo[readptr];
		readptr = (readptr + 1) % buffersize;
		g_buffer_mutex.unlock();
		long double t = resultLine.tick;
		time[0 + i] = timeStartEpoch + (t / 1000);
		value1[0 + i] = resultLine.value1;
		value2[0 + i] = resultLine.value2;
		value3[0 + i] = resultLine.value3;
		value4[0 + i] = resultLine.value4;
		value5[0 + i] = resultLine.value5;
		value6[0 + i] = resultLine.value6;
	}

	return reads;
}

void ClipX::SetReadToWrite()
{
	readptr = writeptr;
}

int ClipX::stopMeasurement()
{
	if (!connected)
		return -1;
	if (!Measuring)
		return -1;
	Measuring = false;
	SDOWrite(0x4428, 1, "0");
	MeasureThread.join();
	keepAlive = true;
	keepAliveThread = std::thread(&ClipX::KeepAlive, this);
	return 0;
}

int ClipX::KeepAlive()
{
	char res[100];
	while (keepAlive)
	{
		SDORead(0x4280, 2, res, 100);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	return 0;
}

bool ClipX::isConnected()
{
	return connected;
}

const int ClipX::Connect(const char *IpAddr)
{
	std::cout << "Connecting" << std::endl;
	struct sockaddr_in server;

#ifdef _WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(1, 1);
	if (WSAStartup(wVersionRequested, &wsaData) != 0)
		return -1;

	if ((sock = WSASocketW(AF_INET, SOCK_STREAM, 6, NULL, 0, 0)) < 0)
		return -1;
#else
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return -1;
#endif

	server.sin_family = AF_INET;	// IP4
	server.sin_port = htons(55000); // Port

	if (inet_pton(AF_INET, IpAddr, &server.sin_addr) <= 0)
		return -1;

	if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
		return -2;

	keepAlive = true;
	Measuring = false;
	keepAliveThread = std::thread(&ClipX::KeepAlive, this);
	connected = true;

	return 0;
}
int ClipX::Disconnect()
{
	keepAlive = false;
	keepAliveThread.join();
	int result = 1;
	result = closesocket(sock);
	connected = false;
	return result;
}

ClipX::~ClipX()
{
}