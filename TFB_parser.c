#include "TFB_parser.h"

TFB_PARSER TFB_openFile(const char *fileName,VF_FOLDER folder)
{
	TFB_PARSER prs;
	
	return prs;
}

uint8 TFB_isCoherence(TFB_PARSER handle, uch *checker)
{
	return 0;
}

TFB_TAG *TFB_nextTag(TFB_PARSER handle)
{
	return 0;
}

uint8 TFB_isEmpty(TFB_PARSER handle)
{
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