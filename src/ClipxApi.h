#pragma once
#include <stdbool.h>
#ifdef _WIN32
#ifdef ClipXApi_EXPORTS
#define ClipX_API __declspec(dllexport)
#else
#define ClipX_API __declspec(dllimport)
#endif
#else
#define ClipX_API __attribute__((visibility("default")))
#endif
struct sClipX
{
	void *obj;
};

#ifdef __cplusplus
extern "C"
{
#endif

	typedef struct sClipX *MHandle;
	MHandle ClipX_Connect(const char *);
	void ClipX_SDORead(MHandle m, int idx, int subidx, char *val, int size);
	void ClipX_SDOWrite(MHandle m, int idx, int subidx, char *val);
	int ClipX_startMeasurement(MHandle m);
	int ClipX_AvailableLines(MHandle m);
	int ClipX_ReadNextLine(MHandle m, double *MVLine);
	int ClipX_ReadNextBlock(MHandle m, int maxreads, double *time, double *value1, double *value2, double *value3, double *value4, double *value5, double *value6);
	void ClipX_SetReadToWrite(MHandle m);
	int ClipX_stopMeasurement(MHandle m);
	void ClipX_Disconnect(MHandle m);
	bool ClipX_isConnected(MHandle m);

#ifdef __cplusplus
}
#endif
