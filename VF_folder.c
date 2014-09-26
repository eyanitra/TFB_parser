#include "VF_folder.h"

int VF_folderDefaultOpen(VF_FOLDER *folder, str folderName)
{
	return 1;
}

int VF_folderActivate(VF_FOLDER folder)
{
	return 1;
}

int VF_folderCreate(const str folderName, const VF_FOLDER_INFO *folderAtribute)
{
	return 1;
}

int VF_folderOpen(VF_FOLDER folder, const str folderName)
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