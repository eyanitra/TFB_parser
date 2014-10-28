#include "BPPblackListRecord.h"
#include "DSC_converter.h"

#ifdef _EFT30_
#	include  "SDK30.h"
#	include "BPPsecurity.h"
#else
#	define NUMBER_BYTE_LEN	3
#	define BNI_PPC_CAN_LEN				8
#	include <string.h>
#endif

#define BL_CTR_BYTE					2
#define BL_DEBT_COUNTER_BYTE		1
#define BL_DC_INDICATOR_BYTE		1
#define BL_UNIQUE_CAN_BYTE			5

#define BL_BDC_CHECK_OFFSET		0
#define BL_BDC_OFFSET				1
#define BL_CTR_OFFSET				2
#define BL_CAN_UNIQUE_OFFSET		4
#define BL_CAN_RUN_OFFSET			9


char BL_isRecordContainCan(const unsigned char fRec[BL_FORMATTED_LENGTH], const unsigned char can[BNI_PPC_CAN_LEN])
{
	unsigned int start, end, cs;
	if(memcmp(&fRec[BL_CAN_UNIQUE_OFFSET],can,BL_UNIQUE_CAN_BYTE) == 0){
		start = dscBcdToBinary32((BCD_T*)&fRec[BL_CAN_RUN_OFFSET], 6);
		end = start + dscBcdToBinary32((BCD_T*)&fRec[BL_CTR_OFFSET],(BL_CTR_BYTE << 1)) - 1;
		cs = dscBcdToBinary32((BCD_T*)&can[BL_UNIQUE_CAN_BYTE], 6);
		if((cs <= end)&&(cs >= start))
			return 1;
	}
	return 0;
}

char BL_isBdcMatch(const unsigned char fRec[BL_FORMATTED_LENGTH], unsigned char bdc)
{
	if(fRec[BL_BDC_CHECK_OFFSET] == 1){
		if(bdc == fRec[BL_BDC_OFFSET])
			return 1;
	}
	return 0;
}

void BL_formatRecord(unsigned char formatted[BL_FORMATTED_LENGTH], 
	const unsigned char rawRecord[BLACK_LIST_RECORD_LENGTH])
{
	formatted[BL_BDC_CHECK_OFFSET] = rawRecord[BLACK_LIST_BDC_CHECK_OFFSET];
	formatted[BL_BDC_OFFSET] = rawRecord[BLACK_LIST_BDC_OFFSET];
	memcpy(&formatted[BL_CTR_OFFSET], &rawRecord[BLACK_LIST_COUNTER_OFFSET], BL_CTR_BYTE);
	memcpy(&formatted[BL_CAN_UNIQUE_OFFSET], &rawRecord[BLACK_LIST_CAN_OFFSET], BNI_PPC_CAN_LEN);
}

char BL_isDeleteCommand(const unsigned char rawRecord[BLACK_LIST_RECORD_LENGTH])
{
	if(rawRecord[BLACK_LIST_FUNCTION_OFFSET] == 1)
		return 1;
	return 0;
}

char BL_isRecordOverlap(const unsigned char recordA[BL_FORMATTED_LENGTH], 
	const unsigned char recordB[BL_FORMATTED_LENGTH])
{
	unsigned int stA, fnA, stB, fnB;
	if(memcmp(&recordA[BL_CAN_UNIQUE_OFFSET], &recordB[BL_CAN_UNIQUE_OFFSET], BL_UNIQUE_CAN_BYTE) == 0)
	{
		stA = dscBcdToBinary32(&recordA[BL_CAN_RUN_OFFSET], 6);
		fnA = stA + dscBcdToBinary32(&recordA[BL_CTR_OFFSET], BL_CTR_BYTE << 1) - 1;
		stB = dscBcdToBinary32(&recordB[BL_CAN_RUN_OFFSET], 6);
		fnB = stB + dscBcdToBinary32(&recordB[BL_CTR_OFFSET],BL_CTR_BYTE << 1) - 1;

		if(stA == stB)
			return 1;
		if((stA > stB)&&(stA < fnB)){
			return 1;
		}
		else if(fnA >= stB)
			return 1;
	}
	return 0;
}
