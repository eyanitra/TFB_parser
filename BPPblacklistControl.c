#include "BPPblacklistControl.h"
#include "DSC_converter.h"

#ifdef _EFT30_
#	include "SDK30.h"
#	include "BPPsecurity.h"
#else
#	include <stdio.h>
#	include <string.h>
#endif

///////////// outside jurisdiction ///////////


#define BLANK_NODE_TAG			7
#define HEADER_MAX_BYTE			8

uch check[]="\x03\x0C\x09\x01\x31\x0E\x01\x31\x0F\x01\x31\x0B\x01\x30";

uint8 BL_ClearFile()
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

uint8 BL_openFileOK(TFB_PARSER *parser)
{
	VF_FOLDER fold;
	TFB_PARSER prs;
	
	VF_folderDefaultOpen(&fold,"HOST");
	prs = TFB_openFile("BNIPP_BL", fold);

	// don't check for now untill stable version
	//if(!TFB_isCoherence(prs,check))
	//	return 0;
	
	memcpy(parser, &prs, sizeof(TFB_PARSER));
	return 1;
}

// for testing purpose
extern void print8L(char *buffer, int bufferLength);

uint32 BL_getVersion()
{
	uint8 bl[NUMBER_BYTE_LEN], found = 0;
	TFB_TAG el;
	TFB_PARSER prs;

	if(!BL_openFileOK(&prs))
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
	if(found)
		return dscBcdToBinary32((BCD_T*)bl,NUMBER_BYTE_LEN <<1);
	return 0;
}

uint8 BL_setVersionOK(uint32 version)
{
	TFB_PARSER blf;
	uch verbuf[NUMBER_BYTE_LEN];
	TFB_TAG prolog, now;
	uint8 exist = 0;
	
	if(!BL_openFileOK(&blf))
		return 0;
		
	dscBinary32ToBcd(version,(BCD_T*)verbuf,NUMBER_BYTE_LEN << 1);
	while(!TFB_isEmpty(blf)){
		TFB_nextTag(blf, &now);
		if(now.tag == TAG_PROLOG)
			prolog = now;
		if(now.tag == BL_VERSION_TAG){
			TFB_setValue(blf, &now,verbuf, NUMBER_BYTE_LEN);
			exist = 1;
			break;
		}
	}
	
	if(!exist)
		TFB_setAfter(blf, &prolog,BL_VERSION_TAG, NUMBER_BYTE_LEN, verbuf);
	
	TFB_close(blf);		
	return 1;		
}

uint8 BL_isCardBlackListed(uch can[BNI_PPC_CAN_LEN], uch bdc)
{
	unsigned char record[BL_FORMATTED_LENGTH];
	uint8 found = 0;
	TFB_PARSER prs;
	TFB_TAG el;
	
	if(!BL_openFileOK(&prs))
		return 0;
		
	while(!TFB_isEmpty(prs)){
		TFB_nextTag(prs, &el);
		if(el.tag == VALID_NODE_TAG){
			TFB_getValue(prs, &el,record,sizeof(record));
			if(BL_isRecordContainCan(record,(char*)can) && BL_isBdcMatch(record, bdc)){
				found = 1;
				break;
			}
		}
	}
	TFB_close(prs);
	return found;
}

uint8 BL_addRecordOK(uch rec[BL_FORMATTED_LENGTH])
{
	TFB_PARSER p;
	TFB_TAG t, last;

	if(!BL_openFileOK(&p))
		return 0;
	
	last.tag = 0; // last initially not a valid tag
	while(!TFB_isEmpty(p)){
		TFB_nextTag(p, &t);
		if((t.tag == VALID_NODE_TAG)||(t.tag == BL_VERSION_TAG)){
			last = t;
			continue;
		}
		if((t.tag != VALID_NODE_TAG)&&(last.tag == VALID_NODE_TAG)){
			break;
		}
	}
	
	if((last.tag == BL_VERSION_TAG)||(last.tag == VALID_NODE_TAG)){
		TFB_setAfter(p, &t,VALID_NODE_TAG, BL_FORMATTED_LENGTH,rec);
		TFB_close(p);
		return 1;
	}
	return 0;
}

uint8 BL_deleteRecordOK(uch rec[BL_FORMATTED_LENGTH])
{
	TFB_PARSER p;
	TFB_TAG t;
	uch recIn[BL_FORMATTED_LENGTH];

	if(!BL_openFileOK(&p))
		return 0;
	
	while(!TFB_isEmpty(p)){
		TFB_nextTag(p, &t);
		if(t.tag == VALID_NODE_TAG){
			TFB_getValue(p, &t,recIn,sizeof(recIn));
			if(BL_isRecordOverlap((char*)rec,(char*)recIn)){
				TFB_clearTag(p, &t);
				break;
			}
		}
	}

	TFB_close(p);
	return 1;
}

uint8 BL_parseRawRecord(const uch raw[BLACK_LIST_RECORD_LENGTH])
{
	uch record[BL_FORMATTED_LENGTH];
	uint8 r;
	
	BL_formatRecord((char*)record,(char*)raw);
	if(!BL_isDeleteCommand((char*)raw))
		r = BL_addRecordOK(record);
	else
		r = BL_deleteRecordOK(record);
	
	return r;
}
