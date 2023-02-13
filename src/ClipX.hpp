#pragma once
#ifdef _WIN32

#ifdef ClipXApi_EXPORTS
#define ClipX_API __declspec(dllexport)
#else
#define ClipX_API __declspec(dllimport)
#endif
#else
#define ClipX_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C"
{
#endif
	ClipX_API int CLIPX_GetSignalTypeByID(int ID, char *data, int length);
	ClipX_API int CLIPX_GetSignalIDByType(char *type);
	ClipX_API int CLIPX_GetTransducerTypeByID(int ID, char *data, int length);
#ifdef __cplusplus
}
#endif

class ClipX
{
public:
	ClipX();
	const int Connect(const char *IP);
	int Disconnect();
	int SDORead(int idx, int subidx, char *res, int size);
	int SDOWrite(int idx, int subidx, const char *val);
	int startMeasurement();
	int AvailableLines();
	int ReadNextLine(double *MVLine);
	int ReadNextBlock(int maxreads, double *time, double *value1, double *value2, double *value3, double *value4, double *value5, double *value6);
	int stopMeasurement();
	bool GetOverFlowFlag();
	void SetReadToWrite();
	bool isConnected();
	void ClearBuffer();
	~ClipX();
	bool keepAlive = true;

private:
	ClipX_API int KeepAlive();
	int WaitForLineFeed();
	int sock = 0, readptr = 0, writeptr = 0;
	unsigned long timeStartEpoch = 0;
	bool overFlow = false, connected = false;
	void MeasureFun();
	int ReadFifo();
	bool Measuring = false;
};

#define BETWEEN(value, min, max) ((max > min && value < max && value > min) || (max < min && (value < max || value > min)))
