#include "stdafx.h"
#include "CppUnitTest.h"
#include "ClipX.h"
#include "ClipX_Interface.h"
#include <iostream>
#include <fstream>
#include <windows.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ClipXApiTest
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		//const char * IP = "172.21.104.66" ;
		const char * IP = "172.21.104.143";
		TEST_METHOD(TestKeepAlive)
		{
			MHandle h = ClipX_Connect(IP);
			Assert::IsNotNull(h);
			char res[10];
			ClipX_SDOWrite(h, 0x4428, 8, "100");
			ClipX_SDORead(h, 0x4428, 8, res, 10);
			Sleep(100);
			Assert::AreEqual(0,strcmp("100\r\n", res));
		}
		TEST_METHOD(TestConnect)
		{
			MHandle h = ClipX_Connect(IP);
			Assert::IsNotNull(h);
			ClipX_Disconnect(h);
		}
		TEST_METHOD(GetSignalTypeByID)
		{
			char data[40];
			int state = CLIPX_GetSignalTypeByID(2, data, 40);
			Assert::IsFalse(strncmp("Field Value", data, strlen(data)));
		}

		TEST_METHOD(GetSignalIDByType) {
			char data[25] = "Calculated Value 1";
			int ID = CLIPX_GetSignalIDByType(data);
			Assert::IsTrue(ID == 21);
		}

		TEST_METHOD(GetTransducerTypeByID) {
			char data[40];
			CLIPX_GetTransducerTypeByID(13, data, 40);
			Assert::IsFalse(strncmp("Full bridge DC 100mV/V", data, strlen(data)));
		}

		TEST_METHOD(TestSDO)
		{
			char buf[100]="1000";
			MHandle h = ClipX_Connect(IP);
			Assert::IsNotNull(h);
			ClipX_SDOWrite(h, 0x4428, 8, buf);
			ClipX_SDORead(h, 0x4428, 8, buf,100);
			ClipX_Disconnect(h);
			
		}

		TEST_METHOD(TestSlowRead)
		{
			std::ofstream logFile;
			logFile.open("C:/temp/TestLogSlow.txt");
			ClipX dev = ClipX();
			dev.Connect(IP);
			logFile << "Connected" << std::endl;                  \
				char buf[2048] = { 0 };
			dev.startMeasurement();
			dev.SDORead(4428,8,buf,2048);
			logFile << "Started Measurement" << std::endl;
			for (int i = 0; i < 100; i++) {
				//std::cout << dev.AvailableLines() ;
				double values[7];
				if (dev.ReadNextLine(values) > 0)
				{
					sprintf_s(buf, "  Time: \t %f \t %f \t %f \t %f \t %f \t %f \t %f", values[0], values[1], values[2], values[3], values[4], values[5], values[6]);
					logFile << buf << std::endl;
				}
				\
					Sleep(100);

			}
			dev.stopMeasurement();
			logFile << "Stopped Measurement" << std::endl;
			dev.AvailableLines();
			dev.Disconnect();
			logFile.close();

		}
		TEST_METHOD(TestFastRead)
		{
			std::ofstream myfile;
			myfile.open("C:/temp/TestLogFast.csv");
			ClipX dev = ClipX();
			dev.Connect(IP);
			myfile << "Connected" << std::endl;                  \
				char buf[4096] = { 0 };
			myfile << "Started Measurement at " << dev.startMeasurement() << std::endl;
			int pointers[2] = { 0, 0 };
			for (int i = 0; i < 90000; i++) {
				double values[7];
				while (dev.AvailableLines()> 0) {
					dev.ReadNextLine(values);
					sprintf_s(buf, "  Time:; %f ; %f ; %f ; %f ; %f ; %f ; %f ; %d ; %d", values[0], values[1], values[2], values[3], values[4], values[5], values[6]);
					myfile << buf << std::endl;
				}
				Sleep(1);
			}

			dev.stopMeasurement();
			std::cout << "Stopped Measurement" << std::endl;
			dev.Disconnect();
			myfile.close();
		}

		TEST_METHOD(BlockRead)
		{
			std::ofstream myfile;
			myfile.open("C:/temp/TestBlockRead.csv");
			ClipX dev = ClipX();
			dev.Connect(IP);
			myfile << "Connected" << std::endl;                  
				char buf[4096] = { 0 };
			myfile << "Started Measurement at " << dev.startMeasurement() << std::endl;
			for (int i = 0; i < 100; i++) {
				double values[7][100];
				int count = dev.ReadNextBlock(100, values[0], values[1], values[2], values[3], values[4], values[5], values[6]);
				for (int j = 0; j < count; j++) {
					sprintf_s(buf, "  Time:; %f ; %f ; %f ; %f ; %f ; %f ; %f", values[0][j], values[1][j], values[2][j], values[3][j], values[4][j], values[5][j], values[6][j]);
					myfile << buf << std::endl;
				}
				
				Sleep(50);
				
			}

			dev.stopMeasurement();
			std::cout << "Stopped Measurement" << std::endl;
			dev.Disconnect();
			myfile.close();
		}

		TEST_METHOD(TestCWrapper) {
			MHandle h =  ClipX_Connect(IP);
			char res[10];
			ClipX_SDORead(h,0x4428,8,res,10);
			ClipX_Disconnect(h);
		}

	};
}