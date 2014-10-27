#ifndef VF_TFB_FILE
#define VF_TFB_FILE

#include "VF_folder.h"

struct opaquehdl_t{
	void *rsc;
};

typedef struct opaquehdl_t VF_FILE;
typedef long VF_OFFSET;

// on success return 0 otherwise error

int VF_isNameUsed(VF_FOLDER folder, const char *fileName);

int VF_open(VF_FOLDER folder,const char *fileName, VF_FILE *fileHandle, VF_OFFSET *size);

int VF_write(const uch *byteIn, int inputByteSize, VF_OFFSET offset,VF_FILE file);

int VF_insert(const uch *byteIn, int inputByteSize, VF_OFFSET offset,int initialSegmentLen, VF_FILE file);

int VF_read(uch *byteIn, int *ioByteSize, VF_OFFSET offset, VF_FILE file);

int VF_close(VF_FILE file);

int VF_deleteFile(VF_FILE file);

int VF_deleteClosed(VF_FOLDER folder, const char *fileName);

// TODO add error definition

#endif
