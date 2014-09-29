#ifndef TAGGED_FILE_PARSER
#define TAGGED_FILE_PARSER

#include "VF_file.h"

typedef unsigned char uint8;
typedef void* TFB_REFF;

#define TAG_CLOSING		1
// TFB_nextTag produce this if one of super tag is closed
#define TAG_PROLOG		2
// TFB_nextTag produce this if entering file TLV section, no TLV shall accepted before this tag/inside

struct tfbstack_t{
	int tag;		
	int length;		
	TFB_REFF reff;	// reverence number for read value, or write to file
};

typedef VF_FILE TFB_PARSER;
typedef struct tfbstack_t TFB_TAG;

TFB_PARSER TFB_openFile(const char *fileName,VF_FOLDER folder);

uint8 TFB_isCoherence(TFB_PARSER handle, uch *checker);	// checker optional

TFB_TAG *TFB_nextTag(TFB_PARSER handle);

uint8 TFB_isEmpty(TFB_PARSER handle);

uint8 TFB_clearTag(TFB_TAG *reff);

uint8 TFB_getValue(TFB_TAG *reff, uch *outBuffer, int bufferLength);

uint8 TFB_setValue(TFB_TAG *reff, uch *outBuffer, int bufferLength);

TFB_TAG *TFB_setInside(TFB_TAG *reff, int tag, int length, uch *value);

TFB_TAG *TFB_setBefore(TFB_TAG *reff, int tag, int length, uch *value);

TFB_TAG *TFB_setAfter (TFB_TAG *reff, int tag, int length, uch *value);

void TFB_close(TFB_PARSER usedHandle);



#endif