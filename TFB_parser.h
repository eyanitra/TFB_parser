#ifndef TFB_PARSER
#define TFB_PARSER

#include "VF_file.h"

typedef unsigned char uint8;

#define TAG_CLOSING		1
// TFB_nextTag produce this if one of super tag is closed
// TAG_CLOSING length always 0
// TAG_CLOSING reff always 0

struct tfbstack_t{
	int tag;		// if you find closing tag from TFB_nextTag, one of super tag is closed
	int length;		// TAG_CLOSING length always 0
	long reff;		
};

typedef opaquehdl_t TFB_PARSER;
typedef tfbstack_t TFB_TAG;

TFB_PARSER TFB_openFile(str fileName,VF_FOLDER folder);

uint8 TFB_IsCoherence(TFB_PARSER handle, hex *checker);	// checker optional

TFB_TAG *TFB_nextTag(TFB_PARSER handle);

uint8 TFB_isEmpty(TFB_PARSER handle);

uint8 TFB_getValue(long reff, hex *outBuffer, int bufferLength);

void TFB_close(TFB_PARSER usedHandle);



#endif