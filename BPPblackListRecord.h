#ifndef BNI_BLACKLIST_RECORD
#define BNI_BLACKLIST_RECORD

#define BLACK_LIST_RECORD_LENGTH			13
#define BLACK_LIST_COUNTER_OFFSET			0
#define BLACK_LIST_CAN_OFFSET				2
#define BLACK_LIST_FUNCTION_OFFSET		10
#define BLACK_LIST_BDC_OFFSET				11
#define BLACK_LIST_BDC_CHECK_OFFSET		12

#define BL_FORMATTED_LENGTH				12

char BL_isRecordContainCan(const unsigned char recordA[BL_FORMATTED_LENGTH],
		const unsigned char *can);

char BL_isBdcMatch(const unsigned char fRec[BL_FORMATTED_LENGTH], unsigned char bdc);

void BL_formatRecord(unsigned char formatted[BL_FORMATTED_LENGTH],
		const unsigned char rawRecord[BLACK_LIST_RECORD_LENGTH]);

char BL_isDeleteCommand(const unsigned char rawRecord[BLACK_LIST_RECORD_LENGTH]);

char BL_isRecordOverlap(const unsigned char recordA[BL_FORMATTED_LENGTH],
		const unsigned char recordB[BL_FORMATTED_LENGTH]);

#endif
