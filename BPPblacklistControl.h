#ifndef BPP_BLACK_BLC
#define BPP_BLACK_BLC

#include "TFB_parser.h"
#include "BPPblackListRecord.h"

#ifndef _EFT30_
	typedef int uint32;
#else
#	include "BPPsecurity.h"
#	include "proto.h"
#endif


#define PARAM_VERSION_TAG		14
#define PARAM_DATA_TAG			15
#define BL_VERSION_TAG			9
#define VALID_NODE_TAG			11

uint8   BL_ClearFile();
uint32 BL_getVersion();
uint8   BL_setVersionOK(uint32 version);
uint8   BL_isCardBlackListed(uch cardAppNumber[BNI_PPC_CAN_LEN], uch bdc);
uint8   BL_parseRawRecord(uch raw[BLACK_LIST_RECORD_LENGTH]);

#endif
