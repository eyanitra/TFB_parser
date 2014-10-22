#include <stdio.h>
#include <string.h>
#include "TFB_parser.h"
#include "BPPblackListRecord.h"
#include "DSC_converter.h"
#include "TLV_serializer.h"
#include "BPP_blacklistControl.h"


#define PARSER_TFILE	"ulalalo"

static void printTagStat(TFB_TAG *dc)
{
	printf("tag :%d\n",dc->tag);
	printf("len :%d\n",dc->length);
	printf("ref :%d\n",dc->reff);
}

void testPars_1()
{
	VF_FOLDER dummy;
	TFB_PARSER hdl;
	int ret, i=0;
	TFB_TAG tag;
	// todo writing after tag
	hdl = TFB_openFile(PARSER_TFILE, dummy);
	ret = TFB_isCoherence(hdl, 0);
	printf("tfb coherence:%s\n",ret?"yes":"no");
	while(i++ < 3){
		ret = TFB_isEmpty(hdl);
		printf("tfb empty:%s\n",ret?"yes":"no");
		if(ret)
			break;
		TFB_nextTag(hdl,&tag);
		printf("next tag \n");
		printTagStat(&tag);
		if(tag.tag != 0){
			tag = TFB_setAfter(hdl,&tag,100,3,(uch*)"mmm");
			printf("after tag \n");
			printTagStat(&tag);
			
			tag = TFB_setBefore(hdl,&tag,101,3,(uch*)"xxx");
			printf("after tag \n");
			printTagStat(&tag);
			
			tag = TFB_setAfter(hdl,&tag,102,3,(uch*)"yyy");
			printf("after tag \n");
			printTagStat(&tag);
			
			tag = TFB_setBefore(hdl,&tag,103,3,(uch*)"zzz");
			printf("after tag \n");
			printTagStat(&tag);
		}else
			break;
	}
	TFB_close(hdl);
}


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
