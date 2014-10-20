#include <stdio.h>
#include <string.h>
#include "TFB_parser.h"
#include "BPPblackListRecord.h"
#include "DSC_converter.h"
#include "TLV_serializer.h"
#include "BPP_blacklistControl.h"


#define PARSER_TFILE	"ulalalo"

void testPars_1()
{
	VF_FOLDER dummy;
	TFB_PARSER hdl;
	int ret;
	TFB_TAG tag;
	//empty file behavior
	
	hdl = TFB_openFile(PARSER_TFILE, dummy);
	ret = TFB_isCoherence(hdl, 0);
	printf("tfb coherence:%s\n",ret?"yes":"no");
	ret = TFB_isEmpty(hdl);
	printf("tfb empty:%s\n",ret?"yes":"no");
	TFB_nextTag(hdl,&tag);
}

// todo writing

// todo search

// todo delete

void testBl_1()
{
	TFB_PARSER parser;
	uch cardAppNumber[BNI_PPC_CAN_LEN];
	uch rec[BLS_RECORD_LENGTH];
	
	blackListOpenFile_OK(&parser);
	//set value here
	blackListIsElementExist(cardAppNumber);
	blackListGetVersion();
	
	blackListSetVersion(3);
	blackListGetVersion();
	
	blackListAddRecord(parser,	rec);
	blackListIsElementExist(cardAppNumber);
	
	blackListDeleteRecord(parser, rec);
	blackListIsElementExist(cardAppNumber);
}
