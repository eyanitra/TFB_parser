#ifndef BPP_BLACK_BLC
#define BPP_BLACK_BLC

#include <stdio.h>
#include <string.h>
#include "TFB_parser.h"
#include "BPPblackListRecord.h"

typedef int uint32;

#define PARAM_VERSION_TAG		14
#define PARAM_DATA_TAG			15
#define BL_VERSION_TAG			9
#define VALID_NODE_TAG			11

uint8 blackListClear();
uint8 blackListOpenFile_OK(TFB_PARSER *parser);
uint32 blackListGetVersion();
uint8 blackListSetVersion(uint32 version);
uint8 blackListIsElementExist(uch cardAppNumber[BNI_PPC_CAN_LEN]);
uint8 blackListAddRecord(TFB_PARSER p, uch rec[BLS_RECORD_LENGTH]);
uint8 blackListDeleteRecord(TFB_PARSER p, uch rec[BLS_RECORD_LENGTH]);
uint8 blackListParseLine(uch raw[BLACK_LIST_RECORD_LENGTH]);


#endif
