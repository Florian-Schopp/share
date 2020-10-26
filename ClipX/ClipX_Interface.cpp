#include "stdafx.h"
#include <stdlib.h>
#include "ClipX_Interface.h"
#include "ClipX.h"




sClipX* __stdcall ClipX_Connect(const char * IP)
{
	sClipX *m;
	ClipX *obj;

	m = (sClipX*)malloc(sizeof(*m));
	obj = new ClipX();
	if (obj->Connect(IP) < 0)return nullptr;
	m->obj = obj;
	return m;
}

void __stdcall ClipX_Disconnect(MHandle m)
{
	if (m == nullptr)
		return;
	ClipX *obj;
	obj = static_cast<ClipX *>(m->obj);
	obj->Disconnect();
	delete static_cast<ClipX *>(m->obj);
	free(m);
}

void __stdcall ClipX_SDORead(MHandle m, int idx,int subidx,char* val,int size)
{
	ClipX *obj;

	if (m == nullptr)
		return;

	obj = static_cast<ClipX *>(m->obj);
	obj->SDORead(idx,subidx,val,size);
}

void __stdcall ClipX_SDOWrite(MHandle m, int idx, int subidx, char* val)
{
	ClipX *obj;

	if (m == nullptr)
		return ;

	obj = static_cast<ClipX *>(m->obj);
	obj->SDOWrite(idx, subidx, val);
}

bool __stdcall ClipX_isConnected(MHandle m) {
	ClipX* obj;
	obj = static_cast<ClipX*>(m->obj);
	return obj->isConnected();
}


int __stdcall ClipX_startMeasurement(MHandle m)
{
	ClipX *obj;

	if (m == nullptr)
		return -1;

	obj = static_cast<ClipX *>(m->obj);
	return obj->startMeasurement();
}

int __stdcall ClipX_stopMeasurement(MHandle m)
{
	ClipX *obj;

	if (m == nullptr)
		return -1;

	obj = static_cast<ClipX *>(m->obj);
	return obj->stopMeasurement();
}

int __stdcall ClipX_AvailableLines(MHandle m)
{
	ClipX *obj;

	if (m == nullptr)
		return -1;

	obj = static_cast<ClipX *>(m->obj);
	return obj->AvailableLines();
}

int __stdcall ClipX_ReadNextLine(MHandle m, double* MVLine) {
	ClipX *obj;

	if (m == nullptr)
		return -1;

	obj = static_cast<ClipX *>(m->obj);
	return obj->ReadNextLine(MVLine);
}

void __stdcall ClipX_SetReadToWrite(MHandle m) {
	ClipX* obj;

	if (m == nullptr)
		return;

	obj = static_cast<ClipX*>(m->obj);
	obj->SetReadToWrite();
}

int __stdcall ClipX_ReadNextBlock(MHandle m, int maxreads, double* time, double* value1, double* value2, double* value3, double* value4, double* value5, double* value6) {
	ClipX* obj;

	if (m == nullptr)
		return -1;

	obj = static_cast<ClipX *>(m->obj);
	return obj->ReadNextBlock(maxreads, time, value1, value2, value3, value4, value5, value6);
}