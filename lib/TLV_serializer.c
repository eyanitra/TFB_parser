#include "TLV_serializer.h"

#define LENGTH_BYTE_BER_1	128
#define LENGTH_BYTE_BER_2	1024
#define LENGTH_BYTE_BER_3	2097152L
#define LENGTH_BYTE_BER_4	268435456L
#define LAST_LENGTH			0x80

int TLV_writeLength(unsigned int length, char *lengthBuffer)
{
	if(length < LENGTH_BYTE_BER_1){
		lengthBuffer[0] = length| LAST_LENGTH;
		return 1;
	}
	if(length < LENGTH_BYTE_BER_2){
		lengthBuffer[0] = (length >> 7)& 0xFF;
		lengthBuffer[1] = (length & 0x7F)| LAST_LENGTH;
		return 2;
	}
	if(length < LENGTH_BYTE_BER_3){
		lengthBuffer[0] = (length >> 14)& 0xFF;
		lengthBuffer[1] = (length >> 7) & 0xFF;
		lengthBuffer[2] = (length & 0x7F)| LAST_LENGTH;
		return 3;
	}
	
	lengthBuffer[0] = (length >> 21)& 0xFF;
	lengthBuffer[1] = (length >> 14)& 0xFF;
	lengthBuffer[2] = (length >> 7) & 0xFF;
	lengthBuffer[3] = (length & 0x7F)| LAST_LENGTH;
	return 4;
}

#define TAG_BYTE_BER_1		16
#define TAG_BYTE_BER_2		LENGTH_BYTE_BER_1
#define TAG_BYTE_BER_3      LENGTH_BYTE_BER_2
#define TAG_BYTE_BER_4		LENGTH_BYTE_BER_3
#define TAG_BYTE_BER_5		LENGTH_BYTE_BER_4
#define FIRST_MASK			0x0F

int TLV_writeTag(unsigned int tag, char *tagBuffer)
{
	if(tag < TAG_BYTE_BER_1){
		tagBuffer[0] = tag;
		return 1;
	}
	if(tag < TAG_BYTE_BER_2){
		tagBuffer[0] = FIRST_MASK;
		tagBuffer[1] = tag + 0x80;
		return 2;
	}
	if(tag < TAG_BYTE_BER_3){
		tagBuffer[0] = FIRST_MASK;
		tagBuffer[1] = (tag >> 7) & 0x7F;
		tagBuffer[2] = (tag & 0x7F) + 0x80; 
		return 3;
	}
	if(tag < TAG_BYTE_BER_4){
		tagBuffer[0] = FIRST_MASK;
		tagBuffer[1] = (tag >> 14) & 0x7F;
		tagBuffer[2] = (tag >> 7)  & 0x7F;
		tagBuffer[3] = (tag & 0x7F) + 0x80;
		return 4;
	}
	
	tagBuffer[0] = FIRST_MASK;
	tagBuffer[1] = (tag >> 21) & 0x7F;
	tagBuffer[2] = (tag >> 14) & 0x7F;
	tagBuffer[3] = (tag >> 7)  & 0x7F;
	tagBuffer[4] = (tag & 0x7F) + 0x80;
	return 5;
}

int TLV_writeTlv(unsigned int tag, unsigned int length, char *value)
{
	return 0;
}

int TLV_readLength(char *lengthBuffer, int bufLen)
{
	return 0;
}

int TLV_readTag(char *tagBuffer, int bufLen)
{
	return 0;
}

int TLV_lengthByte(char *lengthBuffer, int bufLen)
{
	return 0;
}

int TLV_tagByte(char *tagBuffer, int bufLen)
{
	return 0;
}

int TVL_byte(unsigned int tag, unsigned int length)
{
	return 0;
}

int TLV_valueOffset(char *tlvBuffer, int bufLen)
{
	return 0;
}