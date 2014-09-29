#include "VF_folder.h"

int VF_folderDefaultOpen(VF_FOLDER *folder,const char *folderName)
{
	return 1;
}

int VF_folderActivate(VF_FOLDER folder)
{
	return 1;
}

int VF_folderCreate(const char *folderName, const VF_FOLDER_INFO *folderAtribute)
{
	return 1;
}

int VF_folderOpen(VF_FOLDER folder, const char *folderName)
{
	return 1;
}

int VF_getFolderInfo(VF_FOLDER_INFO *folderAttribute, VF_FOLDER folder)
{
	return 1;
}

int VF_folderDelete(VF_FOLDER folder)
{
	return 1;
}