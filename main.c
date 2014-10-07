#include <stdio.h>
#include <string.h>
#include "TFB_parser.h"
#include "BPPblackListRecord.h"
#include "DSC_converter.h"
#include "TLV_serializer.h"

typedef int uint32;
extern void print8L(char *buffer, int bufferLength);
extern void printCode(char *buffer, int bufferLength);

///////////// outside jurisdiction ///////////

#define PARAM_VERSION_TAG		14
#define PARAM_DATA_TAG			15
#define BL_VERSION_TAG			9
#define VALID_NODE_TAG			11
#define BLANK_NODE_TAG			7
#define HEADER_MAX_BYTE			8

uch check[]="\x03\x0C\x09\x01\x31\x0E\x01\x31\x0F\x01\x31\x0B\x01\x30";

uint8 blackListClear()
{
	VF_FILE fhd;
	VF_OFFSET sz;
	VF_FOLDER fold;
	uint8 r;
	
	r = VF_folderDefaultOpen(&fold,"HOST");
	r = VF_open(fold,"BNIPP_BL",&fhd,&sz);
	r = VF_deleteFile(fhd);
	
	return !r;
}

uint8 blackListOpenFile_OK(TFB_PARSER *parser)
{
	VF_FOLDER fold;
	TFB_PARSER prs;
	
	VF_folderDefaultOpen(&fold,"HOST");
	prs = TFB_openFile("BNIPP_BL", fold);
	
	if(!TFB_isCoherence(prs,check))
		return 0;
	
	memcpy(parser, &prs, sizeof(TFB_PARSER));
	return 1;
}

uint32 blackListGetVersion()
{
	uint8 bl[BNI_PPC_NUMBER_BYTE_LEN], found = 0;
	TFB_TAG el;
	TFB_PARSER prs;

	if(!blackListOpenFile_OK(&prs))
		return 0;
		 
	while(!TFB_isEmpty(prs)){
		TFB_nextTag(prs, &el);
		if(el.tag == BL_VERSION_TAG){
			TFB_getValue(prs, &el, bl, sizeof(bl));
			found = 1;
			break;
		}
	}
	
	TFB_close(prs);	
	if(found){
		printf("version :");
		print8L((char*)bl,sizeof(bl));
		return dscBcdToBinary32((BCD_T*)bl,BNI_PPC_NUMBER_BYTE_LEN <<1);
	}
	return 0;
}

uint8 blackListSetVersion(uint32 version)
{
	TFB_PARSER blf;
	uch verbuf[BNI_PPC_NUMBER_BYTE_LEN];
	TFB_TAG pr, now;
	uint8 wr = 0;
	
	if(blackListOpenFile_OK(&blf))
		return 0;
		
	dscBinary32ToBcd(version,(BCD_T*)verbuf,BNI_PPC_NUMBER_BYTE_LEN << 1);
	while(!TFB_isEmpty(blf)){
		TFB_nextTag(blf, &now);
		if(now.tag == TAG_PROLOG)
			pr = now;
		if(now.tag == BL_VERSION_TAG){
			TFB_setValue(blf, &now,verbuf,BNI_PPC_NUMBER_BYTE_LEN);
			wr = 1;
			break;
		}
	}
	
	if(!wr)
		TFB_setAfter(blf, &pr,BL_VERSION_TAG, BNI_PPC_NUMBER_BYTE_LEN, verbuf);
	
	TFB_close(blf);		
	return !wr;		
}

uint8 blackListIsElementExist(uch cardAppNumber[BNI_PPC_CAN_LEN])
{
	uch record[BLS_RECORD_LENGTH];
	uint8 found = 0;
	TFB_PARSER prs;
	TFB_TAG el;
	if(!blackListOpenFile_OK(&prs))
		return 0;
		
	while(!TFB_isEmpty(prs)){
		TFB_nextTag(prs, &el);
		if(el.tag == VALID_NODE_TAG){
			TFB_getValue(prs, &el,record,sizeof(record));
			if(isRecordedCan((char*)record,(char*)cardAppNumber)){
				found = 1;
				break;
			}
		}
	}
	TFB_close(prs);
	return found;
}

uint8 blackListAddRecord(TFB_PARSER p, uch rec[BLS_RECORD_LENGTH])
{
	TFB_TAG t;
	uch r[BLS_RECORD_LENGTH];
	uint8 iden = 0;
	
	while(!TFB_isEmpty(p)){
		TFB_nextTag(p, &t);
		if(t.tag == VALID_NODE_TAG){
			iden = 1;
			TFB_getValue(p, &t,r,sizeof(r));
			if(isRecordOverlap((char*)r, (char*)rec))
				return 1;
			continue;
		}
		if((t.tag != VALID_NODE_TAG)&&(iden == 1)){
			TFB_setBefore(p, &t,VALID_NODE_TAG, BLS_RECORD_LENGTH,rec);
			return 1;
		}
	}
	
	if(t.tag == BL_VERSION_TAG){
		TFB_setAfter(p, &t,VALID_NODE_TAG, BLS_RECORD_LENGTH,rec);
		return 1;
	}
	return 0;
}

uint8 blackListDeleteRecord(TFB_PARSER p, uch rec[BLS_RECORD_LENGTH])
{
	TFB_TAG t;
	uch r[BLS_RECORD_LENGTH];
	while(!TFB_isEmpty(p)){
		TFB_nextTag(p, &t);
		if(t.tag == VALID_NODE_TAG){
			TFB_getValue(p, &t,r,sizeof(r));
			if(isRecordOverlap((char*)r,(char*)rec)){
				return TFB_clearTag(p, &t);
			}
		}
	}
	return 1;
}

uint8 blackListParseLine(uch raw[BLACK_LIST_RECORD_LENGTH])
{
	uch record[BLS_RECORD_LENGTH];
	TFB_PARSER prs;
	uint8 r;
	
	if(!blackListOpenFile_OK(&prs))
		return 0;
	formatRecord((char*)record,(char*)raw);
	if(!isDeleteCommand((char*)raw))
		r = blackListAddRecord(prs,record);
	else
		r = blackListDeleteRecord(prs, record);
	
	TFB_close(prs);
	return r;
}

void testing()
{	
	// todo complete testing here
	blackListGetVersion();
	blackListSetVersion((int)30);
	blackListIsElementExist((uch*)"\x01\x02\x03\x04\x05\x06\x07\x08");
}

void pControlLine(){
	unsigned char lv0[50], *d, lv1[25];
	int len;
	
	// tag without child, must have length == 0
	d = lv1;
	d += TLV_writeTlv(0,BL_VERSION_TAG,0,(unsigned char*)" ",d);
	d += TLV_writeTlv(0,PARAM_VERSION_TAG,0,(unsigned char*)" ",d);
	d += TLV_writeTlv(0,PARAM_DATA_TAG, 0,0,d);
	d += TLV_writeTlv(0,VALID_NODE_TAG,0,0,d);
	len =  d - lv1;
	d = lv0;
	d += TLV_writeTlv(0,TAG_CHECKER,len,lv1,lv0);
	len = d - lv0;
	print8L((char*)lv0,len);
	printCode((char*)lv0, len);
}

int main(int argc, char **argv)
{
	pControlLine();
	//testing();
	return 0;
}
