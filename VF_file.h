#ifdef VF_TFB_FILE
#define VF_TFB_FILE

#include "VF_folder.h"

typedef struct{
	void *rsc;
}VF_FILE;

typedef long VF_OFFSET;

int VF_open(VF_FOLDER folder, str fileName, VF_FILE *fileHandle, VF_OFFSET *size);

int VF_write(const hex *byteIn, int inputByteSize, VF_OFFSET offset,VF_FILE file);

int VF_insert(const hex *byteIn, int inputByteSize, VF_OFFSET offset,VF_FILE file);

int VF_read(hex *byteIn, int *ioByteSize, VF_OFFSET offset, VF_FILE file);

int VF_close(VF_FILE file);

int VF_deleteFile(VF_FILE file);

int VF_deleteClosed(VF_FOLDER folder, str fileName);

// TODO add error definition

#endif