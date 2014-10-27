#include "BPPblackListRecord.h"
#include "DSC_converter.h"

#ifdef _EFT30_
#	include  "SDK30.h"
#else
#	include <string.h>
#endif

#define BL_CAN_COUNTER_BYTE			2
#define BL_CAN_BYTE					8
#define BL_DEBT_COUNTER_BYTE		1
#define BL_DC_INDICATOR_BYTE		1
#define UNIQUE_CAN (BLACK_LIST_CAN_ID_OFFSET - BLACK_LIST_CAN_OFFSET)

#define BL_CAN_COUNTER_DIGIT		(BL_CAN_COUNTER_BYTE << 1)
#define BL_CAN_ID_DIGIT				((BL_CAN_BYTE - UNIQUE_CAN) << 1)

char isRecordedCan(const char recordA[BLS_RECORD_LENGTH], const char can[BNI_PPC_CAN_LEN])
{
	unsigned int start, end, cs;
	if(memcmp(&recordA[BLACK_LIST_CAN_OFFSET],can,UNIQUE_CAN) == 0){
		start = dscBcdToBinary32((BCD_T*)&recordA[BLACK_LIST_CAN_ID_OFFSET],BL_CAN_ID_DIGIT);
		end = start + dscBcdToBinary32((BCD_T*)&recordA[BLACK_LIST_COUNTER_OFFSET],BL_CAN_COUNTER_DIGIT);
		cs = dscBcdToBinary32((BCD_T*)&can[UNIQUE_CAN],BL_CAN_ID_DIGIT);
		if((cs <= end)&&(cs >= start))
			return 1;
	}
	return 0;
}

void formatRecord(char formatted[BLS_RECORD_LENGTH], const char rawRecord[BLACK_LIST_RECORD_LENGTH])
{
	char *f = formatted;
	memcpy(f,rawRecord, BL_CAN_COUNTER_BYTE + BL_CAN_BYTE);
	f += BL_CAN_COUNTER_BYTE + BL_CAN_BYTE;
	memcpy(f,&rawRecord[BLACK_LIST_DC_OFFSET], BL_DEBT_COUNTER_BYTE + BL_DC_INDICATOR_BYTE);
}

char isDeleteCommand(const char rawRecord[BLACK_LIST_RECORD_LENGTH])
{
	if(rawRecord[BLACK_LIST_FUNCTION_OFFSET] == 1)
		return 1;
	return 0;
}

char isRecordOverlap(const char recordA[BLS_RECORD_LENGTH], const char recordB[BLS_RECORD_LENGTH])
{
	return 0;
}
