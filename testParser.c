#include <stdio.h>
#include <string.h>
#include "TFB_parser.h"


#define PARSER_TFILE	"ulalalo"

static void printTagStat(TFB_TAG *dc)
{
	printf("tag :%d\n",dc->tag);
	printf("len :%d\n",dc->length);
	printf("ref :%d\n",dc->reff);
}

uch *roll(char base[3], int loop)
{
	static uch inner[3];
	int i;
	for(i = 0; i < 3;++i)
		inner[i] = base[i]+loop;
	return &inner[0];
}

void testWriting()
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
		printf("loop :%d\n", i);
		TFB_nextTag(hdl,&tag);
		printf("next tag \n");
		printTagStat(&tag);
		if(tag.tag != 0){
			tag = TFB_setAfter(hdl,&tag,100,3,(uch*)roll("000",i));
			printf("after tag \n");
			printTagStat(&tag);
			
			tag = TFB_setAfter(hdl,&tag,101,3,(uch*)roll("AAA",i));
			printf("after tag \n");
			printTagStat(&tag);
			
			tag = TFB_setAfter(hdl,&tag,102,3,(uch*)roll("KKK",i));
			printf("after tag \n");
			printTagStat(&tag);
			
			tag = TFB_setAfter(hdl,&tag,103,3,(uch*)roll("UUU",i));
			printf("after tag \n");
			printTagStat(&tag);
			printf("\n");
		}else
			break;
	}
	TFB_close(hdl);
}

void testPars_1()
{
	VF_FOLDER dummy;
	VF_deleteClosed(dummy, PARSER_TFILE);
	testWriting();
	testWriting();
}





