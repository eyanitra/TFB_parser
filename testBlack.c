#include <stdio.h>
#include <string.h>
#include "BPPblacklistControl.h"

void testBl_1()
{
	TFB_PARSER parser;
	uch cardAppNumber[BNI_PPC_CAN_LEN];
	uch rec[BLS_RECORD_LENGTH];
	
	BL_openFileOK(&parser);
	//set value here
	BL_isCardBlackListed(cardAppNumber, 0);
	BL_getVersion();
	
	BL_setVersionOK(3);
	BL_getVersion();
	
	BL_addRecordOK(parser, rec);
	BL_isCardBlackListed(cardAppNumber, 0);
	
	BL_deleteRecord(parser, rec);
	BL_isCardBlackListed(cardAppNumber, 0);
}
