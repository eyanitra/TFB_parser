#include "VF_file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct rsc_vf{
	FILE *hdl;
	const char fileName[64];
};

typedef struct rsc_vf R_VF;

int VF_open(VF_FOLDER folder, const char *fileName, VF_FILE *fileHandle, VF_OFFSET *size)
{
	R_VF *c = (R_VF*)malloc(sizeof(R_VF));
	c->hdl = fopen(fileName, "+r");
	if(c->hdl){
		strcpy((char*)c->fileName,fileName);
		fileHandle->rsc = c;
		return 0;
	}
	
	free(c);
	return 1;
}

long int fsize(FILE* hdl){
	long int size, now;
	
	now = ftell(hdl);
	fseek(hdl,0, SEEK_END);
	size = ftell(hdl);
	
	fseek(hdl, now, SEEK_SET);
	return size;
}

int VF_write(const uch *byteIn, int inputByteSize, VF_OFFSET offset,VF_FILE file)
{
	R_VF *c = (R_VF*)file.rsc;
	
	if(!c)
		return 1;
		
	if(!c->hdl){
		c->hdl = fopen(c->fileName,"r+");
		if(!c->hdl)
			return 1;
	}
	
	if(fsize(c->hdl) < offset)
		return 1;
	fseek(c->hdl, offset, SEEK_SET);
	fwrite(byteIn, sizeof(uch),inputByteSize, c->hdl);
	return 0;
}

int VF_insert(const uch *byteIn, int inputByteSize, VF_OFFSET offset, int initialLength, VF_FILE file)
{
	R_VF *c = (R_VF*)file.rsc;
	
	if(!c)
		return 1;
		
	if(!c->hdl){
		c->hdl = fopen(c->fileName,"r+");
		if(!c->hdl)
			return 1;
	}

	if(fsize(c->hdl) < offset){
		
		return 1;
	}
		// todo insert 
	return 1;
}

int VF_read(uch *byteIn, int *ioByteSize, VF_OFFSET offset, VF_FILE file)
{
	int readSize;
	R_VF *c = (R_VF*)file.rsc;
	
	if(!c)
		return 1;
		
	if(!c->hdl){
		c->hdl = fopen(c->fileName,"r+");
		if(!c->hdl)
			return 1;
	}

	if(fsize(c->hdl) < offset)
		return 1;
	readSize = *ioByteSize;
	readSize = fread(byteIn, sizeof(uch), readSize,c->hdl);
	*ioByteSize = readSize;
	return 0;

}

int VF_close(VF_FILE file)
{
	R_VF *c = (R_VF*)file.rsc;
	if(c->hdl){
		fclose(c->hdl);
		c->hdl = 0;
		return 0;
	}
	return 1;
}

int VF_deleteFile(VF_FILE file)
{
	R_VF *c = (R_VF*)file.rsc;
	if(c->hdl){
		fclose(c->hdl);
		c->hdl = 0;
		remove(c->fileName);
		free(c);
		file.rsc = 0;
		return 0;
	}
	return 1;
}

int VF_deleteClosed(VF_FOLDER folder,const char *fileName)
{
	remove(fileName);
	return 0;
}
