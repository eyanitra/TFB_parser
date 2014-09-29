#include "VF_file.h"
#include <stdio.h>

int VF_open(VF_FOLDER folder, const char *fileName, VF_FILE *fileHandle, VF_OFFSET *size)
{
	FILE *hdl = fopen(fileName, "+r");
	if(hdl)
		fileHandle->rsc = hdl;
	else
		return 1;
	return 0;
}

int VF_write(const uch *byteIn, int inputByteSize, VF_OFFSET offset,VF_FILE file)
{
	
	return 1;
}

int VF_insert(const uch *byteIn, int inputByteSize, VF_OFFSET offset,VF_FILE file)
{
	return 1;
}

int VF_read(uch *byteIn, int *ioByteSize, VF_OFFSET offset, VF_FILE file)
{
	return 1;
}

int VF_close(VF_FILE file)
{
	return 1;
}

int VF_deleteFile(VF_FILE file)
{
	return 1;
}

int VF_deleteClosed(VF_FOLDER folder,const char *fileName)
{
	return 1;
}