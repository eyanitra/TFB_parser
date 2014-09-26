/*******************************************************
 * 	BPPblackList.c
 *
 * 		description: module for maintaining Card black-list
 * 			table. using file bs-tree.
 *
 *
 * Author : Evan
 * Date   : 27 Jan 2014
 ******************************************************/
#include "BPPblackList.h"
#include "proto.h"
#include "ToSerial.h"
#include "FASfileAlternative.h"
#include "FASfolderAlternative.h"
#include "DSCconversion.h"
#include "BPPblackListStorage.h"
#include "BSTree.h"

#define BLACK_LIST_FILE	"BNIPP_BL"
#define BL_VERSION_TAG			9
#define VALID_NODE_TAG			11
#define BLANK_NODE_TAG			7
#define HEADER_MAX_BYTE			8


typedef struct{
	FAS_FILE_HANDLE File;
	FAS_FILE_OFFSET TableOffset;
}START_UP;


////////////////////////




static FAS_FILE_OFFSET OpenBlackListFile(FAS_FILE_HANDLE *File)
{
	FAS_FOLDER_DESC System;
	FAS_FILE_OFFSET FileSize = 0;

	fasFOLDERdefaultOpen(&System, DEFAULT_ROM_FOLDER_NAME);
	fasFILEopen(System, BLACK_LIST_FILE,File,&FileSize);

	return FileSize;
}

static int GetTableOffsetRoot(FAS_FILE_HANDLE *OpenedFh)
{
	unsigned char VerBuffer[32], *Data;
	READ_OUT Tag, Length;
	int VerBufSize = sizeof(VerBuffer);

	memset(VerBuffer, 0, sizeof(VerBuffer));
	Data = VerBuffer;
	if(fasFILEread(VerBuffer, &VerBufSize, 0, *OpenedFh) != SFW_OK)
		return 0;

	Tag = ReadTagFromMemory(Data,VerBufSize);
	if((Tag.ReadByte == 0)||(Tag.Value != BL_VERSION_TAG))
		return 0;

	Data += Tag.ReadByte;
	VerBufSize -= Tag.ReadByte;
	Length = ReadLengthFromMemory(Data, VerBufSize);
	if((Length.ReadByte == 0)||(Length.Value == 0))
		return 0;

	Data += (Length.ReadByte + Length.Value);
	return Tag.ReadByte + Length.ReadByte + Length.Value;
}

static uint8 InitializeBlackListOK(uint32 version, FAS_FILE_HANDLE *handle)
{
	unsigned char VerBuffer[BNI_PPC_NUMBER_BYTE_LEN << 1];
	int fileError;
	BUFFER_T Version, firstRecord;

	dscBinary32ToBcd(version, VerBuffer, BNI_PPC_NUMBER_BYTE_LEN << 1);
	Version = CreateDynTlvBuffer(BL_VERSION_TAG, BNI_PPC_NUMBER_BYTE_LEN, VerBuffer);
	firstRecord = CreateDynBlankTlvBuffer(BLANK_NODE_TAG, BL_RECORD_LENGTH);

	fileError = fasFILEwrite(Version.Data, Version.Length, 0, *handle);
	if(fileError == SFW_OK){
		fileError = fasFILEwrite(firstRecord.Data, firstRecord.Length,
				Version.Length, *handle);
	}
	DeleteDynBuffer(&Version);
	DeleteDynBuffer(&firstRecord);

	if(fileError != SFW_OK)
		return FALSE;
	return TRUE;
}

#define TOGGLE_ON		1
#define TOGGLE_OFF		0

#define BlackListToggleOnFile(ptrStartUp)	WriteBlackListStartup(ptrStartUp, TOGGLE_ON)
#define BlackListToggleOffFile()			WriteBlackListStartup(0, TOGGLE_OFF)

static void WriteBlackListStartup(START_UP *StartUp, uint8 TStat)
{
	static START_UP Init;
	static uint8 FlgFileOpened = FALSE;
	if(TStat == TOGGLE_ON){
		if(FlgFileOpened == FALSE){
			OpenBlackListFile(&Init.File);
			Init.TableOffset = GetTableOffsetRoot(&Init.File);
			FlgFileOpened = TRUE;
		}
		else if(Init.TableOffset == 0)
			Init.TableOffset = GetTableOffsetRoot(&Init.File);

		memcpy(StartUp, &Init, sizeof(START_UP));
	}
	else{
		fasFILEclose(&Init.File);
		FlgFileOpened = FALSE;
	}
}

static void WriteToRecord(unsigned int RecordTag,
		FAS_FILE_HANDLE hFile, FAS_FILE_OFFSET Offset,
		int RecordLength,
		unsigned char *Record)
{
	BUFFER_T NewNodeData = CreateDynTlvBuffer(RecordTag, RecordLength, Record);
	fasFILEwrite(NewNodeData.Data, NewNodeData.Length, Offset, hFile);
	DeleteDynBuffer(&NewNodeData);
}

#define WriteRecord(hFile, Offset, RecordLength, RecordData)\
		WriteToRecord(VALID_NODE_TAG, hFile, Offset, RecordLength, RecordData)

#define DeleteRecord(hFile, Offset, RecordLength, RecordData)\
		WriteToRecord(BLANK_NODE_TAG, hFile, Offset, RecordLength, RecordData)

void InsertNode(FAS_FILE_HANDLE hFile,
		FAS_FILE_OFFSET Root,
		FAS_FILE_OFFSET EndFile,
		int nodeByteSize,
		unsigned char *SerializedNode)
{
	BOOLEAN_SET RecordStatus;
	BUFFER_T NodeData, RawBuffer;
	RECORD_LINE Inset, Current;
	FAS_FILE_OFFSET EmptySpace = 0;

	RawBuffer.Length = nodeByteSize + HEADER_MAX_BYTE;
	RawBuffer.Data = umalloc(RawBuffer.Length);
	Inset = GetRecordLine(SerializedNode);

	while(EndFile > Root){
		fasFILEread(RawBuffer.Data,&RawBuffer.Length,Root,hFile);
		NodeData = GetTaggedValueFromMemory(RawBuffer.Data,RawBuffer.Length,VALID_NODE_TAG);
		if(NodeData.Data == 0){
			NodeData = GetTaggedValueFromMemory(RawBuffer.Data,RawBuffer.Length, BLANK_NODE_TAG);
			if(NodeData.Data == 0)
				break;
			else if(NodeData.Length == nodeByteSize)
				EmptySpace = Root;
		}
		else if(!IsRecordIdDiffer(NodeData.Data, SerializedNode)){
			Current = GetRecordLine(NodeData.Data);
			RecordStatus = CompareRecordLine(&Current, &Inset);
			if((RecordStatus == bs_AequalB)||(RecordStatus == bs_BinA)){
				ufree(RawBuffer.Data);
				return;
			}
			if(RecordStatus == bs_AinB){
				ufree(RawBuffer.Data);
				WriteRecord(hFile,Root,nodeByteSize, SerializedNode);
				return;
			}
			if(RecordStatus == bs_AoverLapB){
				trimOverlapRecordLine(&Current, &Inset);
				SetRecordLine(NodeData.Data, &Current);
				WriteRecord(hFile, Root, NodeData.Length, NodeData.Data);
			}
		}
		Root += (NodeData.Data - RawBuffer.Data) + NodeData.Length;
		RawBuffer.Length = nodeByteSize + HEADER_MAX_BYTE;
	}

	ufree(RawBuffer.Data);
	if(EmptySpace == 0)
		WriteRecord(hFile,Root,nodeByteSize, SerializedNode);
	else
		WriteRecord(hFile,EmptySpace,nodeByteSize, SerializedNode);
}

void DeleteNode(FAS_FILE_HANDLE hFile,
		FAS_FILE_OFFSET Root,
		FAS_FILE_OFFSET EndFile,
		int nodeByteSize,
		unsigned char *SerializedNode)
{
	BUFFER_T Buffer, Record;
	RECORD_LINE InsetLine, RecordLine,PartialLine;
	FAS_FILE_OFFSET PartialSpace;

	Buffer.Length = nodeByteSize + HEADER_MAX_BYTE;
	Buffer.Data = umalloc(Buffer.Length);
	InsetLine = GetRecordLine(SerializedNode);
	PartialSpace = EndFile;

	while(Root < EndFile){
		fasFILEread(Buffer.Data, &Buffer.Length, Root, hFile);
		Record = GetTaggedValueFromMemory(Buffer.Data, Buffer.Length, VALID_NODE_TAG);
		if(Record.Data == 0){
			Record = GetTaggedValueFromMemory(Buffer.Data, Buffer.Length, BLANK_NODE_TAG);
			if(Record.Data == 0){
				ufree(Buffer.Data);
				return;
			}else
				PartialSpace = Root;
		}
		else if(!IsRecordIdDiffer(Record.Data, SerializedNode)){
			RecordLine = GetRecordLine(Record.Data);
			switch(CompareRecordLine(&RecordLine, &InsetLine))
			{
			case bs_AequalB:
				DeleteRecord(hFile, Root, Record.Length, Record.Data);
				ufree(Buffer.Data);
				return;
			case bs_BinA:
				PartialLine = RemovePartialRecordLine(&RecordLine, &InsetLine);
				if(RecordLine.Count){
					SetRecordLine(Record.Data,&RecordLine);
					WriteRecord(hFile, Root, Record.Length, Record.Data);
				}
				else
					DeleteRecord(hFile, Root, Record.Length, Record.Data);
				if(PartialLine.Count){
					SetRecordLine(Record.Data,&PartialLine);
					WriteRecord(hFile, PartialSpace, Record.Length, Record.Data);
				}
				ufree(Buffer.Data);
				return;
			case bs_AoverLapB:
				trimOverlapRecordLine(&RecordLine, &InsetLine);
				SetRecordLine(Record.Data, &RecordLine);
				WriteRecord(hFile, Root, Record.Length, Record.Data);
				break;
			case bs_AinB:
				DeleteRecord(hFile, Root, Record.Length, Record.Data);
			case bs_AfreeB:
				break;
			}
		}
		Root += (Buffer.Data - Record.Data)+ Record.Length;
		Buffer.Length = nodeByteSize + HEADER_MAX_BYTE;
	}
}

FAS_FILE_OFFSET FindNode(FAS_FILE_HANDLE hFile,
		FAS_FILE_OFFSET Root,
		FAS_FILE_OFFSET EndFile,
		int nodeByteSize,
		unsigned char *SerializedNode)
{
	BUFFER_T Buffer, Record;
	RECORD_LINE RecordLine, InsetLine;

	Buffer.Length = nodeByteSize + HEADER_MAX_BYTE;
	Buffer.Data = umalloc(Buffer.Length);
	InsetLine = GetRecordLine(SerializedNode);

	while(Root < EndFile){
		fasFILEread(Buffer.Data, &Buffer.Length, Root, hFile);
		Record = GetTaggedValueFromMemory(Buffer.Data, Buffer.Length, VALID_NODE_TAG);
		if(Record.Data == 0){
			Record = GetTaggedValueFromMemory(Buffer.Data, Buffer.Length, BLANK_NODE_TAG);
			if(Record.Data == 0){
				break;
			}
		}
		else if(!IsRecordIdDiffer(Record.Data, SerializedNode)){
			RecordLine = GetRecordLine(Record.Data);
			switch(CompareRecordLine(&RecordLine, &InsetLine)){
			case bs_AequalB:
			case bs_BinA:
				ufree(Buffer.Data);
				return Root;
			default:
				break;
			}
		}
		Root +=(Record.Length + (Buffer.Data - Record.Data));
		Buffer.Length = nodeByteSize + HEADER_MAX_BYTE;
	}

	ufree(Buffer.Data);
	return 0;
}

///////////////////////////

uint8 OpenBlackListOK()
{
	START_UP StartUp;

	BlackListToggleOnFile(&StartUp);
	if(StartUp.TableOffset == 0){
		if(InitializeBlackListOK(0,&StartUp.File)){
			if(GetTableOffsetRoot(&StartUp.File) > 0)
				return TRUE;
		}
		BlackListToggleOffFile();
		return FALSE;
	}
	return TRUE;

}

void CloseBlackList()
{
	BlackListToggleOffFile();
}

#define BLV_BUFFER_LENGTH		(32)

uint32 BlackListGetVersion()
{
	START_UP Start;
	BUFFER_T Version;
	unsigned char VerBuffer[BLV_BUFFER_LENGTH];
	int size;

	BlackListToggleOnFile(&Start);
	size = BLV_BUFFER_LENGTH;
	if(fasFILEread(VerBuffer, &size, 0, Start.File) != SFW_OK)
		return 0;

	Version = GetTaggedValueFromMemory(VerBuffer, size, BL_VERSION_TAG);
	if(Version.Length != BNI_PPC_NUMBER_BYTE_LEN)
		return 0;

	return dscBcdToBinary32(Version.Data, Version.Length << 1);
}

uint8 BlackListSetVersionOK(uint32 version)
{
	START_UP Start;
	FAS_FILE_OFFSET ValueOffset;
	BUFFER_T Version;
	int size;
	unsigned char VerBuffer[BLV_BUFFER_LENGTH], VerBcd[BNI_PPC_NUMBER_BYTE_LEN];

	BlackListToggleOnFile(&Start);
	dscBinary32ToBcd(version, VerBcd,(BNI_PPC_NUMBER_BYTE_LEN << 1));
	size = sizeof(VerBuffer);
	if(fasFILEread(VerBuffer, &size, 0, Start.File) == SFW_OK){
		if(size > 0){
			Version = GetTaggedValueFromMemory(VerBuffer,BLV_BUFFER_LENGTH,BL_VERSION_TAG);
			if(Version.Data != 0){
				ValueOffset = Version.Data - VerBuffer;
				fasFILEwrite(VerBcd, BNI_PPC_NUMBER_BYTE_LEN, ValueOffset, Start.File);
				return TRUE;
			}
		}
	}
	return FALSE;
}

void BlackListClear()
{
	FAS_FOLDER_DESC folder;
	BlackListToggleOffFile();
	fasFOLDERdefaultOpen(&folder, DEFAULT_ROM_FOLDER_NAME);
	fasFILEdelete(folder, BLACK_LIST_FILE);
}


uint8 BlackListParseRecordOK(unsigned char BlackListRecord[BLACK_LIST_RECORD_LENGTH])
{
	START_UP St;
	BUFFER_T Serialized;

	BlackListToggleOnFile(&St);
	if(St.TableOffset == 0)
		return FALSE;	// todo reinit table
	Serialized = AllocateSerializedData(BlackListRecord);
	if(BlackListRecord[BLACK_LIST_FUNCTION_OFFSET] == 0x1){
		DeleteNode(St.File,St.TableOffset,fasFILEbyteLength(St.File),
				Serialized.Length, Serialized.Data);
	}
	else if(BlackListRecord[BLACK_LIST_FUNCTION_OFFSET] == 0){
		InsertNode(St.File,St.TableOffset,fasFILEbyteLength(St.File),
				Serialized.Length, Serialized.Data);
	}
	FreeAllocatedRV(&Serialized);
	return TRUE;
}

uint8 BlackListElementExist(unsigned char CardAppNumber[BNI_PPC_CAN_LEN])
{
	START_UP St;
	FAS_FILE_OFFSET ElementOffset;
	BUFFER_T Serialized;

	BlackListToggleOnFile(&St);
	if(St.TableOffset == 0)
		return FALSE;

	Serialized = AllocateSearchData(CardAppNumber);
	ElementOffset = FindNode(St.File,St.TableOffset,
			fasFILEbyteLength(St.File),
			Serialized.Length, Serialized.Data);
	FreeAllocatedRV(&Serialized);
	if(ElementOffset < 0)
		return FALSE;
	return TRUE;
}


