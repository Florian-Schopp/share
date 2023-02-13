#pragma once
const int buffersize = 50000;

struct EntryStruct
{
	float value1;
	float value2;
	float value3;
	float value4;
	float value5;
	float value6;
	uint32_t tick;
} fifo[buffersize];

struct HeaderStruct
{
	unsigned int prot;
	unsigned int systemstatus;
	unsigned int digFlagsLow;
	unsigned int digFlagsHigh;
	unsigned int fifoFlags;
	unsigned int dataBytes;
	unsigned int reserved1;
	unsigned int reserved2;
};

union HeaderUnion
{
	char Chararray[sizeof(HeaderStruct)];
	struct HeaderStruct res;
} Header_empty, Header;

union EntryUnion
{
	char Chararray[sizeof(EntryStruct)];
	struct EntryStruct res[4000];
} Entries_empty, Entries;
