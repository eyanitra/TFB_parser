#include <stdio.h>
#include "TFB_parser.h"

typedef int uint32;
#define BNI_PPC_NUMBER_BYTE_LEN		3
#define BNI_PPC_CAN_LEN			8

///////////// outside jurisdiction ///////////

const char blackList[] = "BNIPP_BL";

#define BL_VERSION_TAG			9
#define VALID_NODE_TAG			11
#define BLANK_NODE_TAG			7
#define HEADER_MAX_BYTE			8


void blackListGetVersion()
{
	uint8 bl[BNI_PPC_NUMBER_BYTE_LEN];
	VF_FOLDER fold;
	TFB_TAG *el;
	TFB_PARSER prs;
	
	
	VF_folderDefaultOpen(&fold,"HOST");
	prs = TFB_openFile(blackList, fold);
	
	while(!TFB_isEmpty(prs)){
		el = TFB_nextTag(prs);
		if(el->tag == BL_VERSION_TAG){
			TFB_getValue(el->reff, bl, sizeof(bl));
			break;
		}
	}
	printf("version :");
	print8L(bl,sizeof(bl));
	TFB_close(prs);
}

uint8 blackListIsElementExist(hex CardAppNumber[])

void testing()
{
	blackListGetVersion();
}


int main(int argc, char **argv)
{
	testing();
	return 0;
}
