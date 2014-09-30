#include "TFB_parser.h"

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
	uch *check;
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
		memset(&st->fifo, 0, sizeof(XFIFO));
		prs.rsc = st;
	}
	
	return prs;
}

uint8 TFB_isCoherence(TFB_PARSER handle,const uch *checker)
{
	XRES *st = (XRES*)handle.rsc;
	
	// if there is checker save to handle
	// 	skip checker if present
	// else
	// 	no checker present return 0
	
	// parse file 
	return 0;
	
}

static BLOCK *TFB_parseFile(XRES *resource, BLOCK *lastBlock)
{
	return 0;
}

void TFB_nextTag(TFB_PARSER handle, TFB_TAG *nT)
{
	BLOCK *blk;
	int cap;
	XRES *st = (XRES*)handle.rsc;
	if((!st)||(!st->fifo.block))
		return;
		
	cap = st->fifo.top;
	for(blk = st->fifo.block ; blk&&(blk->top < cap); blk = blk->next){
		if(blk->next == 0)
			blk->next = TFB_parseFile(st, blk);
	}
	
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
	if((!st)||(!st->fifo.block))
		return 1;
	if(st->fifo.top < st->fifo.size)
		return 0;
	return 1;
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