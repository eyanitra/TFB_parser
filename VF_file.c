#include "VF_file.h"
#include <stdio.h>

int VF_open(VF_FOLDER folder, str fileName, VF_FILE *fileHandle, VF_OFFSET *size)
{
	FILE *hdl = fopen(fileName, "+r");
	if(hdl)
		fileHandle->rsc = hdl;
	else
		return 1;
	return 0;
}

int VF_write(const hex *byteIn, int inputByteSize, VF_OFFSET offset,VF_FILE file)
{
	return 1;
}

int VF_insert(const hex *byteIn, int inputByteSize, VF_OFFSET offset,VF_FILE file)
{
	return 1;
}

int VF_read(hex *byteIn, int *ioByteSize, VF_OFFSET offset, VF_FILE file)
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

int VF_deleteClosed(VF_FOLDER folder, str fileName)
{
	return 1;
}