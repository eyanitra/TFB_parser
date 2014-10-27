#include "BPP_blacklistControl.h"
#include "DSC_converter.h"

#ifdef _EFT30_
#	include "SDK30.h"
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
	
	if(!TFB_isCoherence(prs,check))
		return 0;
	
	memcpy(parser, &prs, sizeof(TFB_PARSER));
	return 1;
}

// for testing purpose
extern void print8L(char *buffer, int bufferLength);

uint32 BL_getVersion()
{
	uint8 bl[BNI_PPC_NUMBER_BYTE_LEN], found = 0;
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
	if(found){
		printf("version :");
		print8L((char*)bl,sizeof(bl));
		return dscBcdToBinary32((BCD_T*)bl,BNI_PPC_NUMBER_BYTE_LEN <<1);
	}
	return 0;
}

uint8 BL_setVersionOK(uint32 version)
{
	TFB_PARSER blf;
	uch verbuf[BNI_PPC_NUMBER_BYTE_LEN];
	TFB_TAG pr, now;
	uint8 wr = 0;
	
	if(BL_openFileOK(&blf))
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

uint8 BL_isCardBlackListed(uch can[BNI_PPC_CAN_LEN], uch bdc)
{
	uch record[BLS_RECORD_LENGTH];
	uint8 found = 0;
	TFB_PARSER prs;
	TFB_TAG el;
	if(!BL_openFileOK(&prs))
		return 0;
		
	while(!TFB_isEmpty(prs)){
		TFB_nextTag(prs, &el);
		if(el.tag == VALID_NODE_TAG){
			TFB_getValue(prs, &el,record,sizeof(record));
			if(isRecordedCan((char*)record,(char*)can)){
				found = 1;
				break;
			}
		}
	}
	TFB_close(prs);
	return found;
}

uint8 BL_addRecordOK(TFB_PARSER p, uch rec[BLS_RECORD_LENGTH])
{
	TFB_TAG t, f;
	uch r[BLS_RECORD_LENGTH];
	
	f.tag = 0; // f initially not a valid tag
	while(!TFB_isEmpty(p)){
		TFB_nextTag(p, &t);
		if(t.tag == VALID_NODE_TAG){
			TFB_getValue(p, &t,r,sizeof(r));
			if(isRecordOverlap((char*)r, (char*)rec))
				return 1;
			f = t;
			continue;
		}
		if((t.tag != VALID_NODE_TAG)&&(f.tag == VALID_NODE_TAG)){
			TFB_setAfter(p, &f,VALID_NODE_TAG, BLS_RECORD_LENGTH,rec);
			return 1;
		}
	}
	
	if(t.tag == BL_VERSION_TAG){
		TFB_setAfter(p, &t,VALID_NODE_TAG, BLS_RECORD_LENGTH,rec);
		return 1;
	}
	return 0;
}

uint8 BL_deleteRecord(TFB_PARSER p, uch rec[BLS_RECORD_LENGTH])
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

uint8 BL_parseRawRecord(uch raw[BLACK_LIST_RECORD_LENGTH])
{
	uch record[BLS_RECORD_LENGTH];
	TFB_PARSER prs;
	uint8 r;
	
	if(!BL_openFileOK(&prs))
		return 0;
	formatRecord((char*)record,(char*)raw);
	if(!isDeleteCommand((char*)raw))
		r = BL_addRecordOK(prs,record);
	else
		r = BL_deleteRecord(prs, record);
	
	TFB_close(prs);
	return r;
}
