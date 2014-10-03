#include "TFB_parser.h"
#include "TLV_serializer.h"

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


struct tfb_info{
	int tag;
	int length;
	VF_OFFSET off;
};

#define ANCESTRY_SIZE	8
typedef struct tfb_verify ANCESTRY;
struct tfb_verify{
	uch *check;				// must copy from file/get from outside
	VF_OFFSET dSeg;			// offset of data segment
	unsigned int sibling; 	// last tag
};

#define EL_ARRY_SIZE	16 
typedef struct tfb_page BLOCK;
struct tfb_page{
	int top;	// last valid element 0,1,... 
	struct tfb_info array[EL_ARRY_SIZE];
	BLOCK *next;
};

struct tfb_fifo{
	int top;
	int size;
	BLOCK *block;
};
typedef struct tfb_fifo XFIFO;

struct tfb_store{
	VF_FILE hdl;
	VF_OFFSET size;
	VF_OFFSET cur;
};
typedef struct tfb_store XFILE;

struct tfb_resource{
	XFILE file;
	XFIFO fifo;
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
		
		st->fifo.size = 0;
		st->fifo.top = 0;
		st->fifo.block =(BLOCK*) Z_MALLOX(sizeof(BLOCK));
		memset(&st->fifo.block, 0, sizeof(BLOCK));
		
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
	else
		chkB = (uch*)checker;

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
	
	st->ancor.sibling = TAG_PROLOG;
	st->ancor.check = chkB;
	return 1;
}

static uint8 TFB_isTagFound(XRES *st, unsigned int tag)
{
	uint8 found = 0, *lookup;
	int sk, limit, fTag, cdata, fLen, fVal;

	lookup = st->ancor.check;
	sk = TLV_valueOffset(lookup, 5);
	limit = TLV_readLengthFix(lookup, 5);
	while(limit > sk){
		fTag = TLV_readTag(&lookup[sk],limit -sk);
		if(fTag == tag){
			found = 1;
			break;
		}
		fLen = TLV_readLengthFix(&lookup[sk], limit - sk);
		if(fLen > 0){
			fVal = TLV_valueOffset(&lookup[sk], limit - sk);
			cdata = lookup[fVal];
			if(fLen > (cdata + 1)){
				sk += (cdata + fVal + 1);
				continue;
			}
		}
		sk += TLV_nextTlvOffset(&lookup[sk], limit -sk);
	}
	return found;
}

static uint8 TFB_isTagHasChild(XRES *st, unsigned int tag)
{
	// todo redesing control string
	return 0;
}

uint8 TFB_isCoherence(TFB_PARSER handle, const uch *checker)
{
	uch snoop[8], nice;
	int sz;
	VF_OFFSET off;
	unsigned int tag;
	XRES *st = (XRES*)handle.rsc;
	
	if(!TFB_checkerOk(st, checker))
		return 0;
		
	nice = 1;
	for(off = st->file.cur; off < st->file.size; ){
		sz = sizeof(snoop);
		if(VF_read(snoop,&sz, off,st->file.hdl))
			break;
		if(sz > 0){		
			tag = TLV_readTag(snoop,sz);
			if(TFB_isTagFound(st, tag)){
				if(TFB_isTagHasChild(st, tag))
					off += TLV_valueOffset(snoop, sz);
				else
					off += TLV_nextTlvOffset(snoop, sz);
			}
			else{
				nice = 0;
				break;
			}
		}
		else break;
	} 
	return nice;
}

static BLOCK *TFB_fillBuffer(XRES *resource, XFIFO *buffer)
{
	// if file handle blank return 0
	// read first tag from file offset
	// search it in checker
	// if not found -- return 0
	// if found compare checker stack vs current stack
	return 0;
}

void TFB_nextTag(TFB_PARSER handle, TFB_TAG *nT)
{
	BLOCK *blk;
	int cap;
	XRES *st = (XRES*)handle.rsc;
	
	if((!st)||(st->ancor.check == 0))
		return;
		
	// if buffer is not empty return tag
	// read file and fill buffer 
	// 
	
	if(blk == 0)
		return;
	++st->fifo.top;
	
	nT->reff = cap;
	cap -= blk->top;
	nT->length = blk->array[cap].length;
	nT->tag = blk->array[cap].tag;
}

uint8 TFB_isEmpty(TFB_PARSER handle)
{
	XRES *st = (XRES*)handle.rsc;
	if(!st)
		return 1;	// no handle
	if(st->ancor.check == 0)
		return 1;	// no lookup
	if(st->file.size <= 0)
		return 1;	// file blank
	if(st->file.cur >= st->file.size)
		return 1;   // file need to be reparsed
		
	// todo if buffer empty reread file to fill it
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
