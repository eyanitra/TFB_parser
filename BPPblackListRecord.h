#ifndef BNI_BLACKLIST_RECORD
#define BNI_BLACKLIST_RECORD


#define BNI_PPC_NUMBER_BYTE_LEN		3
#define BNI_PPC_CAN_LEN				8

#define BLACK_LIST_RECORD_LENGTH			13
#define BLACK_LIST_COUNTER_OFFSET			0
#define BLACK_LIST_CAN_OFFSET				2
#define BLACK_LIST_CAN_ID_OFFSET			7
#define BLACK_LIST_FUNCTION_OFFSET			10
#define BLACK_LIST_DC_OFFSET				11
#define BLACK_LIST_DC_INDICATOR_OFFSET		12

#define BLS_RECORD_LENGTH					12

char isRecordedCan(const char recordA[BLS_RECORD_LENGTH], const char can[BNI_PPC_CAN_LEN]);

void formatRecord(char formatted[BLS_RECORD_LENGTH], const char rawRecord[BLACK_LIST_RECORD_LENGTH]);

char isDeleteCommand(const char rawRecord[BLACK_LIST_RECORD_LENGTH]);

char isRecordOverlap(const char recordA[BLS_RECORD_LENGTH], const char recordB[BLS_RECORD_LENGTH]);

#endif