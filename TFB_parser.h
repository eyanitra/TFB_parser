#ifndef TFB_PARSER
#define TFB_PARSER

#include "VF_file.h"

typedef unsigned char uint8;

typedef struct{
	int tag;
	int length;
	str value;
}STACK_LMN;

typedef struct{
	void *src;
}TFB_PARSER;

TFB_PARSER TFB_openFile(str fileName,VF_FOLDER folder);

uint8 TFB_IsCoherence(TFB_PARSER handle, hex *checker);	// checker optional

STACK_LMN *TFB_nextTag(TFB_PARSER handle);

uint8 TFB_IsEmpty(TFB_PARSER handle);



#endif