#include "TLV_serializer.h"
#include <string.h>

#define LENGTH_BYTE_BER_1	0x80
#define LENGTH_BYTE_BER_2	0x100
#define LENGTH_BYTE_BER_3	0x10000
#define LENGTH_BYTE_BER_4	0x1000000
#define FIRST_LENGTH		0x80


int TLV_writeLength(unsigned int length, unsigned char *lengthBuffer)
{
	if(length < LENGTH_BYTE_BER_1){
		lengthBuffer[0] = length;
		return 1;
	}
	if(length < LENGTH_BYTE_BER_2){
		lengthBuffer[0] = FIRST_LENGTH + 1;
		lengthBuffer[1] = length;
		return 2;
	}
	if(length < LENGTH_BYTE_BER_3){
		lengthBuffer[0] = FIRST_LENGTH + 2;
		lengthBuffer[1] = length;
		lengthBuffer[2] = (length >> 8)& 0xFF;
		return 3;
	}
	if(length < LENGTH_BYTE_BER_4){
		lengthBuffer[0] = FIRST_LENGTH + 3;
		lengthBuffer[1] = length;
		lengthBuffer[2] = (length >> 8)& 0xFF;
		return 4;
	}
	lengthBuffer[0] = FIRST_LENGTH + 4;
	lengthBuffer[1] = (length >> 8)& 0xFF;
	lengthBuffer[2] = (length >> 16)& 0xFF;
	lengthBuffer[3] = (length >> 24)& 0xFF;
	return 5;
}

#define TAG_BYTE_BER_1		16
#define TAG_BYTE_BER_2		128
#define TAG_BYTE_BER_3      1024
#define TAG_BYTE_BER_4		2097152L
#define TAG_BYTE_BER_5		268435456L
#define TAG_STOP			0x80
#define SHORT_FORM			0x10
#define CLASS_MASK			0xE0
#define FIRST_MASK			(SHORT_FORM | 0x0F)
#define EXT_MASK			0x7F

int TLV_writeTag(unsigned char tagClass, unsigned int tag, unsigned char *tagBuffer)
{
	tagClass &= CLASS_MASK;
	if(tag < TAG_BYTE_BER_1){
		tagBuffer[0] = tag + tagClass;
		return 1;
	}
	if(tag < TAG_BYTE_BER_2){
		tagBuffer[0] = FIRST_MASK + tagClass;
		tagBuffer[1] = tag + TAG_STOP;
		return 2;
	}
	if(tag < TAG_BYTE_BER_3){
		tagBuffer[0] = FIRST_MASK + tagClass;
		tagBuffer[1] = (tag >> 7) & 0x7F;
		tagBuffer[2] = (tag & 0x7F) + TAG_STOP; 
		return 3;
	}
	if(tag < TAG_BYTE_BER_4){
		tagBuffer[0] = FIRST_MASK + tagClass;
		tagBuffer[1] = (tag >> 14) & 0x7F;
		tagBuffer[2] = (tag >> 7)  & 0x7F;
		tagBuffer[3] = (tag & 0x7F) + TAG_STOP;
		return 4;
	}
	
	tagBuffer[0] = FIRST_MASK + tagClass;
	tagBuffer[1] = (tag >> 21) & 0x7F;
	tagBuffer[2] = (tag >> 14) & 0x7F;
	tagBuffer[3] = (tag >> 7)  & 0x7F;
	tagBuffer[4] = (tag & 0x7F) + TAG_STOP;
	return 5;
}

int TLV_tlvByte(unsigned int tag, unsigned int length)
{
	int len = length;
	if(tag < TAG_BYTE_BER_1)
		len += 1;
	else if(tag < TAG_BYTE_BER_2)
		len += 2;
	else if(tag < TAG_BYTE_BER_3)
		len +=3;
	else if(tag < TAG_BYTE_BER_4)
		len += 4;
	else
		len += 5;
	
	if(length < LENGTH_BYTE_BER_1)
		len += 1;
	else if(length < LENGTH_BYTE_BER_2)
		len += 2;
	else if(length < LENGTH_BYTE_BER_3)
		len += 3;
	else if(length < LENGTH_BYTE_BER_4)
		len += 4;
	else 
		len += 5;
	
	return len;
}

int TLV_writeTlv(unsigned char tagClass, unsigned int tag, unsigned int length, 
	const unsigned char *value, 
	unsigned char *tlvBuffer)
{
	int sh;
	unsigned char *out;
	
	out = tlvBuffer;
	sh = TLV_writeTag(tagClass, tag,out);
	if(!sh)
		return 0;
	out += sh;
	sh = TLV_writeLength(length, out);
	if(!sh)
		return 0;
	out += sh;
	memcpy(out, value, length);
	return (int)(out - tlvBuffer)+ length;
}

unsigned int TLV_readLength(const unsigned char *lengthBuffer, int bufLen)
{
	unsigned int j = 0, i;
	if(lengthBuffer[0] < LENGTH_BYTE_BER_1){
		if(bufLen > 0)
			j = lengthBuffer[0];
	}
	else{
		for(i = 1; i < (lengthBuffer[0] & 0x7F); ++i){
			if(i > bufLen)
				return 0;
			j <<= 8;
			j += lengthBuffer[i];
		};
	}
	return j;
}

unsigned int TLV_readLengthFix(const unsigned char *tlvBuffer, int bufLen)
{
	int skip;
	skip = TLV_tagByte(tlvBuffer, bufLen);
	if(skip == 0)
		return 0;
	return TLV_readLength(&tlvBuffer[skip], bufLen- skip);
}

unsigned int TLV_readTag(const unsigned char *tagBuffer, int bufLen)
{
	unsigned int j = 0;
	int i;
	if((tagBuffer[0] & SHORT_FORM)== 0)
		return (tagBuffer[0] & 0xF);
	i = 1;	
	do{
		j <<= 7;
		j += (tagBuffer[i] & 0x7F);
	}while((tagBuffer[i++] & TAG_STOP) == 0);
	return j;
}

int TLV_lengthByte(const unsigned char *lengthBuffer, int bufLen)
{
	if((lengthBuffer[0] & FIRST_LENGTH) == 0)
		return 1;
	return (int)(lengthBuffer[0]& 0x7F);
}

int TLV_tagByte(const unsigned char *tagBuffer, int bufLen)
{
	int i = 1;
	if((tagBuffer[0] &  SHORT_FORM) == 0)
		return i;
	while((tagBuffer[i++] & TAG_STOP) == 0){
		if(i >= bufLen)
			return 0;
	}
	return i;
}

int TLV_valueOffset(const unsigned char *tlvBuffer, int bufLen)
{
	int l;
	l = TLV_tagByte(tlvBuffer, bufLen);
	if(l == 0)
		return 0;
	l += TLV_lengthByte(&tlvBuffer[l],bufLen - l);
	return l;
}

int TLV_nextTlvOffset(const unsigned char *tlvBuffer, int bufLen){

	int skip, l;
	skip = TLV_tagByte(tlvBuffer, bufLen);
	if(skip == 0)
		return 0;
	l = TLV_readLength(&tlvBuffer[skip], bufLen- skip);
	skip += TLV_lengthByte(&tlvBuffer[skip], bufLen - skip);
	return l + skip;
}
