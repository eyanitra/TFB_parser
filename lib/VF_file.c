#include "VF_file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct rsc_vf{
	FILE *hdl;
	const char fileName[64];
};

typedef struct rsc_vf R_VF;

VF_OFFSET fsize(FILE* hdl){
	long size;
	fpos_t p;
	
	fgetpos(hdl, &p);
	
	fseek(hdl,0, SEEK_END);
	size = ftell(hdl);
	
	fsetpos(hdl, &p);
	return (VF_OFFSET)size;
}

/////////////////////////////

int VF_open(VF_FOLDER folder, const char *fileName, VF_FILE *fileHandle, VF_OFFSET *size)
{
	R_VF *c = (R_VF*)malloc(sizeof(R_VF));
	c->hdl = fopen(fileName, "a+b");
	if(c->hdl){
		strcpy((char*)c->fileName,fileName);
		fileHandle->rsc = c;
		fseek(c->hdl,0, SEEK_SET);
		if(size)
			*size = fsize(c->hdl);
		return 0;
	}
	
	free(c);
	return 1;
}

int VF_write(const uch *byteIn, int inputByteSize, VF_OFFSET offset,VF_FILE file)
{
	R_VF *c = (R_VF*)file.rsc;
	
	if(!c)
		return 1;
		
	if(!c->hdl){
		c->hdl = fopen(c->fileName,"r+b");
		if(!c->hdl)
			return 1;
	}
	
	if(fsize(c->hdl) < offset)
		return 1;
	fseek(c->hdl, offset, SEEK_SET);
	fwrite(byteIn, sizeof(uch),inputByteSize, c->hdl);
	return 0;
}

void VF_copySegmentFile(int srcOffset, int length, FILE *trg, FILE *src)
{
	unsigned char buf[64];
	int c, l;
	
	fseek(src,(long int)srcOffset, SEEK_SET);
	for(c= 0; c < length; c+= l){
		l = sizeof(buf);
		if(l > (length - c))
			l = length - c;
		fread(buf, sizeof(unsigned char), l,src);
		fwrite(buf, sizeof(unsigned char), l, trg);
	}
}

#define VF_TEMP_NAME "xxxtemp"

int VF_insert(const uch *byteIn, int inputByteSize, VF_OFFSET offset, int initialLength, VF_FILE file)
{
	R_VF *c = (R_VF*)file.rsc;
	FILE *temp;
	VF_OFFSET size;
	
	if(!c)
		return 1;
		
	if(!c->hdl){
		c->hdl = fopen(c->fileName,"r+b");
		if(!c->hdl)
			return 1;
	}

	size = fsize(c->hdl);
	if(size < offset)
		return 1;
	temp = tmpfile();
	if(!temp)
		return 1;
	VF_copySegmentFile(0,offset,temp, c->hdl);
	fwrite(byteIn, sizeof(uch), inputByteSize, temp);
	if(size > (offset + initialLength))
		VF_copySegmentFile(offset + initialLength,size - (offset+initialLength),temp, c->hdl);
	
	freopen(c->fileName,"wb+",c->hdl);
	rewind(c->hdl);
	rewind(temp);
	size = size - initialLength + inputByteSize;
	VF_copySegmentFile(0, size, c->hdl,temp);
	fclose(temp);
	fclose(c->hdl);
	c->hdl = 0;
	
	return 0;
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
	fseek(c->hdl, offset, SEEK_SET);
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
