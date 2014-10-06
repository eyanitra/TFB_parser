#include "TFB_parser.h"
#include "TLV_serializer.h"
#include "CB_cbuffer.h"

#ifdef _EFT30_
#	include <SDK30.h>
#	include "DSCtraceDebug.h"
#	define Z_MALLOX(x)		umalloc(x)
#	define Z_FREE(x)		ufree(x)
#else
#	include <stdlib.h>
#	include <string.h>
#	define Z_MALLOX(x)		malloc(x)
#	define Z_FREE(x)		free(x)
#endif



// do not implement yet
#define ANCESTRY_SIZE	8

struct tfb_verify{
	uch *check;				// must copy from file/get from outside
	VF_OFFSET dSeg;			// offset of data segment
	//do not implement yet
	unsigned int sibling; 	// last tag with same parent
	unsigned int parent;	// last tag as parent
};
typedef struct tfb_verify ANCESTRY;

#define FIFO_SIZE	16 
struct tfb_info{
	int tag;
	int length;
	VF_OFFSET off;
};
typedef struct tfb_info L_FIFO;

struct tfb_store{
	VF_FILE hdl;
	VF_OFFSET size;
	VF_OFFSET cur;
};
typedef struct tfb_store XFILE;

struct tfb_resource{
	XFILE file;
	CB_HDL fifo;
	ANCESTRY ancor;
};

typedef struct tfb_resource XRES;

TFB_PARSER TFB_openFile(const char *fileName,VF_FOLDER folder)
{
	TFB_PARSER prs;
	VF_FILE file;
	VF_OFFSET sz;
	XRES *st;
	
	prs.rsc = 0;
	if(VF_open(folder,fileName, &file, &sz) == 0){
		st = (XRES*)Z_MALLOX(sizeof(XRES));
		st->file.hdl = file;
		st->file.size = sz;
		st->file.cur = 0;
		
		memset(&st->ancor, 0, sizeof(ANCESTRY));
		st->fifo = CB_create(FIFO_SIZE, sizeof(L_FIFO));
		
		prs.rsc = st;
	}
	
	return prs;
}

static uint8 TFB_checkerOk(XRES *st, const uch *checker){

	uch *chkB, snoop[8];
	int sz;
	unsigned int tag, len, tot;

	if(!checker){
		chkB = snoop;
		sz = sizeof(snoop);
		VF_read(chkB,&sz,0,st->file.hdl);
	}
	else{
		chkB = (uch*)checker;
		st->ancor.dSeg = 0;
	}

	tag = TLV_readTag(chkB,sz);
	if(tag != TAG_CHECKER)
		return 0;
	chkB += TLV_tagByte(chkB,sz);
	len = TLV_readLengthFix(chkB, sz);
	if(!len)
		return 0;
	
	if(!checker){
		tot = TLV_tlvByte(tag, len);
		chkB = (uch*) Z_MALLOX(tot);
		sz = tot;
		VF_read(chkB,&sz,0,st->file.hdl);
		if(sz != tot)
			return 0;
		
		st->ancor.dSeg = TLV_nextTlvOffset(chkB,tot);
	}
	
	st->ancor.check = chkB;
	return 1;
}

static int TFB_findTag(XRES *st, unsigned int tag)
{
	uint8 found = 0, *lookup;
	int sk, limit, fTag; // cdata, fLen, fVal;

	lookup = st->ancor.check;
	sk = TLV_valueOffset(lookup, 5);
	limit = TLV_readLengthFix(lookup, 5);
	while(limit > sk){
		fTag = TLV_readTag(&lookup[sk],limit -sk);
		if(fTag == tag){
			found = 1;
			break;
		}
		/* 
		// disable reading CDATA
		fLen = TLV_readLengthFix(&lookup[sk], limit - sk);
		if(fLen > 0){
			fVal = TLV_valueOffset(&lookup[sk], limit - sk);
			cdata = lookup[fVal];
			if(fLen > (cdata + 1)){
				sk += (cdata + fVal + 1);
				continue;
			}
		}*/
		sk += TLV_nextTlvOffset(&lookup[sk], limit -sk);
	}
	if(found)
		return sk;
	return 0;
}

static uint8 TFB_isTagHasChild(XRES *st, unsigned int tag, int findInfo)
{
	unsigned int tE, lE;
	lE =  TLV_readLengthFix(st->ancor.check,findInfo);
	tE = TLV_readTag(&st->ancor.check[findInfo],lE);
	if(tE != tag)
		return 0;
	if(TLV_readLengthFix(&st->ancor.check[findInfo], lE - findInfo) > 0)
		return 1;
	return 0;
}

static uint8 TFB_isParseable(XRES *st)
{
	uch snoop[8], nice;
	int sz, tagInfo;
	VF_OFFSET off;
	unsigned int tag;
	
	sz = sizeof(snoop);
	if(VF_read(snoop,&sz, 0,st->file.hdl))
		return 0;	// unreadable
	
	if(TLV_readTag(snoop, sz) == TAG_CHECKER)
		off = TLV_nextTlvOffset(snoop, sz);
	else
		off = 0;
		
		nice = 1;
	for(st->file.cur = off; off < st->file.size; ){
		sz = sizeof(snoop);
		if(VF_read(snoop,&sz, off,st->file.hdl))
			break;	// file end
		if(sz > 0){		
			tag = TLV_readTag(snoop,sz);
			tagInfo = TFB_findTag(st, tag);
			if(tagInfo > 0){
				if(TFB_isTagHasChild(st, tag, tagInfo)){
					off += TLV_valueOffset(snoop, sz);
					// todo put inspection tree implementation here
				}
				else
					off += TLV_nextTlvOffset(snoop, sz);
			}
			else{
				nice = 0; // tag not registered
				break;
			}
		}
		else break;
	}
	
	return nice;
}

uint8 TFB_isCoherence(TFB_PARSER handle, const uch *checker)
{
	XRES *st = (XRES*)handle.rsc;
	
	if(!TFB_checkerOk(st, checker))
		return 0;
		
	return TFB_isParseable(st);
}

static uint8 TFB_fillBuffOk(XRES *st)
{
	// check file offset if fresh, put TAG_PROLOG -> TAG_CHECKER offset
	// read file successive 
	// fill buffer to its full ness
	return 0;
}

void TFB_nextTag(TFB_PARSER handle, TFB_TAG *nT)
{
	XRES *st = (XRES*)handle.rsc;
	L_FIFO dTag;
	
	if(!st)
		return;
	memset(nT, 0, sizeof(TFB_TAG));
	if(CB_isEmpty(&st->fifo)){
		if(!TFB_fillBuffOk(st))
			return;
	}
	CB_getElement(&st->fifo, &dTag);
	nT->length = dTag.length;
	nT->reff   = dTag.off;
	nT->tag    = dTag.tag;
}

uint8 TFB_isEmpty(TFB_PARSER handle)
{
	XRES *st = (XRES*)handle.rsc;
	if(!st)
		return 1;	// no handle
	if(CB_isEmpty(&st->fifo)){
		if(!TFB_fillBuffOk(st))
			return 1;
	}
	return 0;
}

uint8 TFB_clearTag(TFB_TAG *reff)
{
	return 0;
}

uint8 TFB_getValue(TFB_TAG *reff, uch *outBuffer, int bufferLength)
{
	return 0;
}

uint8 TFB_setValue(TFB_TAG *reff, uch *outBuffer, int bufferLength)
{
	return 0;
}

TFB_TAG *TFB_setInside(TFB_TAG *reff, int tag, int length, uch *value)
{
	return 0;
}

TFB_TAG *TFB_setBefore(TFB_TAG *reff, int tag, int length, uch *value)
{
	return 0;
}

TFB_TAG *TFB_setAfter (TFB_TAG *reff, int tag, int length, uch *value)
{
	return 0;
}

void TFB_close(TFB_PARSER usedHandle){
	
}
