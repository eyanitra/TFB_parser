#ifndef VF_TFB_FOLDER
#define VF_TFB_FOLDER

#include "DSC_converter.h"

#define VF_FOLDER_TOKEN_LENGTH		32

struct vffolder_t{
	uch token[VF_FOLDER_TOKEN_LENGTH];
}; 

struct vffolderinfo_t{
	unsigned long size;
	unsigned int maxFile;
	uch mode;
};

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

typedef struct vffolderinfo_t 	VF_FOLDER_INFO;
typedef struct vffolder_t 		VF_FOLDER;

#define DEFAULT_ROM_FOLDER_NAME		"HOST"
#define DEFAULT_SWAP_FOLDER_NAME		"SYSTEM"

int VF_folderDefaultOpen(VF_FOLDER *folder, const char *folderName);

int VF_folderActivate(VF_FOLDER folder);

int VF_folderCreate(const char *folderName, const VF_FOLDER_INFO *folderAtribute);

int VF_folderOpen(VF_FOLDER folder, const char *folderName);

int VF_getFolderInfo(VF_FOLDER_INFO *folderAttribute, VF_FOLDER folder);

int VF_folderDelete(VF_FOLDER folder);

// TODO add error definition

#endif
