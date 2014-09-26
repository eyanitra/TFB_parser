#ifndef VF_TFB_FOLDER
#define VF_TFB_FOLDER

typedef unsigned char *str;
typedef unsigned char hex;

#define VF_FOLDER_TOKEN_LENGTH		32

typedef struct{
	hex token[VF_FOLDER_TOKEN_LENGTH];
}VF_FOLDER;

// append file if exist, not erased on reboot, non transferable
#define PM_DEFAULT			0
// equal to "-w" if exist will be rewrite
#define PM_REWRITE			1
// file will erased on first call on second execution
#define PM_BOOTCLR			2
// file transferable and equipt with lock
#define PM_TRANSFER			4
// folder from other application
#define PM_IMPORT			8

typedef struct{
	unsigned long size;
	unsigned int maxFile;
	hex mode;
}VF_FOLDER_INFO;

int VF_folderDefaultOpen(VF_FOLDER *folder, str folderName);

int VF_folderActivate(VF_FOLDER folder);

int VF_folderCreate(const str folderName, const VF_FOLDER_INFO *folderAtribute);

int VF_folderOpen(VF_FOLDER folder, const str folderName);

int VF_getFolderInfo(VF_FOLDER_INFO *folderAttribute, VF_FOLDER folder);

int VF_folderDelete(VF_FOLDER folder);

// TODO add error definition

#endif