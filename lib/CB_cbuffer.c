#include "CB_cbuffer.h"
#ifdef _EFT30_
#	include <SDK30.h>
#	include "DSCtraceDebug.h"
#   define Z_CPY(a,b,c)		memcpy(a, b, c)
#	define Z_MALLOX(x)		umalloc(x)
#	define Z_FREE(x)		ufree(x)
#else
#	include <stdlib.h>
#	include <string.h>

#   define Z_CPY(a, b, c)	copycat(a, b, c)	
#	define Z_MALLOX(x)		malloc(x)
#	define Z_FREE(x)		free(x)

// since memcpy always end with segmentation fault
static void copycat(void *trg, const void* src, int len){
	unsigned char *trgx, *srcx;
	int i;
	trgx =(unsigned char*)trg;
	srcx =(unsigned char*)src;
	for(i = 0; i < len; ++i){
		*trgx = *srcx;
		trgx++;
		srcx++;
	}
}
#endif


struct cb_hidden{
	long bSize;
	int lSize;
	unsigned char *buffer;
	unsigned char isEmpty;
	int next;
	int first;
};

typedef struct cb_hidden handle;

CB_HDL CB_create(int bSize, int elSize)
{
	CB_HDL out;
	handle *hdl = (handle*)Z_MALLOX(sizeof(handle));
	hdl->bSize = elSize *bSize;
	hdl->lSize = elSize;
	hdl->first = 0;
	hdl->next = 0;
	hdl->isEmpty = 1;
	hdl->buffer = (unsigned char*)Z_MALLOX(elSize *bSize);
	out.rsc = hdl;
	return out;
}

unsigned char CB_isFull(CB_HDL *hd)
{
	handle *hdl = (handle*)hd->rsc;
	if(hdl->isEmpty)
		return 0;
	if(hdl->next == hdl->first)
		return 1;
	return 0;
}

unsigned char CB_isEmpty(CB_HDL *hd)
{
	handle *hdl = (handle*)hd->rsc;
	return hdl->isEmpty;
}

unsigned char CB_addElement(CB_HDL *hd,const void *element)
{
	handle *hdl = (handle*)hd->rsc;
	void *trg;
	if(hdl == 0)
		return 0;
	if(hdl->first == hdl->next){
		if(hdl->isEmpty == 0)
			return 0;
	}
	trg = hdl->buffer+hdl->next;
	Z_CPY(trg, element, hdl->lSize);
	if(hdl->next == (hdl->bSize - hdl->lSize))
		hdl->next = 0;
	else
		hdl->next += hdl->lSize;
	hdl->isEmpty = 0;
	return 1;
}

unsigned char CB_getElement(CB_HDL *hd, void *element)
{
	handle *hdl = (handle*)hd->rsc;
	if(hdl == 0)
	return 0;
		
	if(hdl->first == hdl->next){
		if(hdl->isEmpty)
			return 0;
	}
	Z_CPY(element,(hdl->buffer +hdl->first), hdl->lSize);
	if(hdl->first == (hdl->bSize - hdl->lSize))
		hdl->first = 0;
	else
		hdl->first += hdl->lSize;
	if(hdl->first == hdl->next)
		hdl->isEmpty = 1;
	return 1;
}

void CB_removeElements(CB_HDL *hdlBuffer)
{
	handle *hdl = (handle *)hdlBuffer->rsc;
	hdl->next = hdl->first;
	hdl->isEmpty = 1;
}

int CB_curSize(CB_HDL *hd)
{
	int c; 
	long t;
	
	handle *hdl = (handle*)hd->rsc;
	if(!hdl->isEmpty){
		if(hdl->next > hdl->first)
			t = hdl->next - hdl->first;
		else
			t = hdl->bSize - hdl->first + hdl->next;
	}
	else t = 0;
		
	for(c = 0; t > 0; ++c){
		t -= hdl->lSize;
	}
	return c;
}

void CB_clear(CB_HDL *hd)
{
	handle *hdl = (handle*)hd->rsc;
	Z_FREE(hdl->buffer);
	Z_FREE(hdl);
	hd->rsc = 0;
}

void CB_peek(CB_HDL *hd,  int eNuma, void *element)
{
	handle *hdl = (handle*)hd->rsc;
	eNuma *= hdl->lSize;
	eNuma += hdl->first;
	while(eNuma >= hdl->bSize)
		eNuma -= hdl->bSize;
	Z_CPY(element,(hdl->buffer +eNuma), hdl->lSize);
}
