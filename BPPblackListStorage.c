#include "BPPblackList.h"
#include "BPPblackListStorage.h"
#include "FASfileAlternative.h"
#include "FASfolderAlternative.h"
#include "DSCconversion.h"

/*
typedef struct{
	FAS_FILE_HANDLE File;
	FAS_FILE_OFFSET TableOffset;
}START_UP;
*/

#define BL_CAN_COUNTER_BYTE			2
#define BL_CAN_BYTE					8
#define BL_UNIQUE_CAN_BYTE			(BLACK_LIST_CAN_ID_OFFSET - BLACK_LIST_CAN_OFFSET)
#define BL_DEBT_COUNTER_BYTE		1
#define BL_DC_INDICATOR_BYTE		1

#define BL_CAN_COUNTER_DIGIT		(BL_CAN_COUNTER_BYTE << 1)
#define BL_CAN_ID_DIGIT				((BL_CAN_BYTE - BL_UNIQUE_CAN_BYTE) << 1)


BOOLEAN_SET CompareRecordLine(RECORD_LINE *RecA, RECORD_LINE *RecB)
{
	uint32 EndA = RecA->Start + RecA->Count - 1;
	uint32 EndB = RecB->Start + RecB->Count - 1;

	if(RecA->Start > RecB->Start){
		if(EndA > RecB->Start)
			return bs_AfreeB;
		if(EndA <= EndB)
			return bs_BinA;
		return bs_AoverLapB;
	}
	else if(RecB->Start > RecA->Start){
		if(EndB > RecA->Start )
			return bs_AfreeB;
		if(EndB <= EndA)
			return bs_AinB;
		return bs_AoverLapB;
	}

	if(EndA > EndB)
		return bs_BinA;
	else if(EndB > EndA)
		return bs_AinB;
	return bs_AequalB;
}

uint8 IsRecordIdDiffer(const char RecordA[BL_RECORD_LENGTH], const char RecordB[BL_RECORD_LENGTH])
{
	return (uint8)memcmp(&RecordA[BLACK_LIST_CAN_OFFSET],
			&RecordB[BLACK_LIST_CAN_OFFSET],BL_UNIQUE_CAN_BYTE);
}

void SetRecordLine(char BlackListRecord[BL_RECORD_LENGTH], RECORD_LINE *InLine)
{
	dscBinary32ToBcd(InLine->Count,&BlackListRecord[BLACK_LIST_COUNTER_OFFSET],BL_CAN_COUNTER_DIGIT);
	dscBinary32ToBcd(InLine->Start, &BlackListRecord[BLACK_LIST_CAN_ID_OFFSET],BL_CAN_ID_DIGIT);
}

RECORD_LINE GetRecordLine(const char BlackListRecord[BL_RECORD_LENGTH])
{
	RECORD_LINE RecId;
	RecId.Start = dscBcdToBinary32(&BlackListRecord[BLACK_LIST_CAN_ID_OFFSET],BL_CAN_ID_DIGIT);
	RecId.Count = dscBcdToBinary32(&BlackListRecord[BLACK_LIST_COUNTER_OFFSET], BL_CAN_COUNTER_DIGIT);
	return RecId;
}

void trimOverlapRecordLine(RECORD_LINE *Target,const RECORD_LINE *Trimmer)
{
	if(Trimmer->Start > Target->Start){
		Target->Count = (Trimmer->Start - Target->Start);
	}
	else{
		Target->Count -= (Trimmer->Count + Trimmer->Start - Target->Start);
		Target->Start = Trimmer->Count + Trimmer->Start;
	}
}

RECORD_LINE RemovePartialRecordLine(RECORD_LINE *Target, const RECORD_LINE *Part)
{
	RECORD_LINE Residu;
	Target->Count = Target->Start - Part->Start;
	Residu.Start = Part->Start + Part->Count;
	Residu.Count = Target->Start + Target->Count - Residu.Start;
	return Residu;
}


BUFFER_T AllocateSerializedData( unsigned char BlackListRecord[13])
{
	BUFFER_T Serial;
	unsigned char *Data;

	Serial.Data = Data = umalloc(BL_RECORD_LENGTH);
	memcpy(Data, &BlackListRecord[BLACK_LIST_COUNTER_OFFSET],BL_CAN_COUNTER_BYTE );
	Data += BL_CAN_COUNTER_BYTE;
	memcpy(Data, &BlackListRecord[BLACK_LIST_CAN_OFFSET],BL_CAN_BYTE);
	Data += BL_CAN_BYTE;
	*Data = BlackListRecord[BLACK_LIST_DC_OFFSET];
	Data += BL_DEBT_COUNTER_BYTE;
	*Data = BlackListRecord[BLACK_LIST_DC_INDICATOR_OFFSET];
	Data += BL_DC_INDICATOR_BYTE;

	Serial.Length = Data - Serial.Data;
	return Serial;
}

BUFFER_T AllocateSearchData( unsigned char CardAppNumber[BNI_PPC_CAN_LEN])
{
	BUFFER_T Serial;
	unsigned char *Data;

	Serial.Data = Data = umalloc(BL_RECORD_LENGTH);
	memset(Data, 0, BL_RECORD_LENGTH);
	BE_INT_TO_BIN2(1, Data, BLACK_LIST_COUNTER_OFFSET);
	Data += BL_CAN_COUNTER_BYTE;
	memcpy(Data, CardAppNumber, BL_CAN_BYTE);
	Data += BL_CAN_BYTE;
	Serial.Length = BL_RECORD_LENGTH;

	return Serial;
}

void FreeAllocatedRV(BUFFER_T *Serialized)
{
	ufree(Serialized->Data);
	Serialized->Data = 0;
	Serialized->Length = 0;
}

/*
//////////////////////////////

#define VALID_NODE_TAG		9
#define BLANK_NODE_TAG		7
#define HEADER_MAX_BYTE		8


FAS_FILE_OFFSET FindNodeInTable(BOOLEAN_SET comparator(void *, void*),
		FAS_FILE_HANDLE hFile,
		FAS_FILE_OFFSET root,
		FAS_FILE_OFFSET EndFile,
		int nodeByteSize,
		unsigned char *SerializedNodeId)
{
	unsigned char *ElBuffer;
	int BufSize;
	BOOLEAN_SET comparation;

	BufSize = nodeByteSize + HEADER_MAX_BYTE;
	ElBuffer = umalloc(BufSize);

	BUFFER_T NodeData;
	while(EndFile > root){
		fasFILEread(ElBuffer,&BufSize,root,hFile);
		NodeData = GetTagBufferFromMemory(ElBuffer,BufSize,VALID_NODE_TAG);
		if(NodeData.Data == 0){
			NodeData = GetTagBufferFromMemory(ElBuffer,BufSize, BLANK_NODE_TAG);
			if(NodeData.Data == 0)
					break;
		}
		else{
			comparation = comparator(NodeData.Data, SerializedNodeId);
			if((comparation == bs_BinA)||(comparation == bs_AequalB)){
				ufree(ElBuffer);
				return root;
			}
		}
		root += (NodeData.Data - ElBuffer) + NodeData.Length;
	}
	ufree(ElBuffer);
	return NODE_NOT_FOUND;
}

void UpdateNodeInTable(BOOLEAN_SET comparator(void *, void*),
		FAS_FILE_HANDLE hFile,
		FAS_FILE_OFFSET root,
		FAS_FILE_OFFSET EndFile,
		int nodeByteSize,
		unsigned char *SerializedNodeId)
{
	FAS_FILE_OFFSET LOffset = FindNodeInTable(comparator,hFile,root,EndFile,nodeByteSize,SerializedNodeId);
	if(LOffset == NODE_NOT_FOUND)
		LOffset = EndFile;
	BUFFER_T NewElement = CreateDynTaggedBuffer(VALID_NODE_TAG, nodeByteSize, SerializedNodeId);
	fasFILEwrite(NewElement.Data, NewElement.Length, LOffset, hFile);
	DeleteDynBuffer(&NewElement);
}
*/

