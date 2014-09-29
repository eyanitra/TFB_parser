/*******************************************************************************
 * DSC_hash.h
 *
 *	Description:
 * 		Data conversion function, this is module for hashing text into
 * 		certain fixed formatted data.
 * 		Currently supported algorithm is Fowler-Noll-Vo.
 *
 *  Author : Evan
 *  Date   : 29 July 2013
 ******************************************************************************/
#ifndef DSC_HASH_FNV_H
#define DSC_HASH_FNV_H


typedef char HASH_BYTE;

typedef enum
{
	fnvOne8,	// fnv algorithm output 1 byte, XOR folded from 32bit
	fnvOne16,	// fnv algorithm output 2 byte, XOR folded from 32bit
	fnvOne32,	// fnv algorithm output 4 byte,
}HASH_TYPE;

/*********************************************************
 * Macro for flatten type into byte Big-Endian Style
 ********************************************************/

#define BIGEND_2BYTE(ptrByte, Number) \
	ptrByte[0] = (Number >> 8)& 0xFF; \
	ptrByte[1] = (Number)& 0xFF; \

#define BIGEND_4BYTE(ptrByte, Number) \
	ptrByte[0] = (Number >> 24)& 0xFF; \
	ptrByte[1] = (Number >> 16)& 0xFF; \
	ptrByte[2] = (Number >> 8)& 0xFF; \
	ptrByte[3] = (Number)& 0xFF; \

#define BYTE_2BIGEND(ptrByte, Number) \
	Number = ptrByte[0]; \
	Number <<= 8;	\
	Number += ptrByte[1];\


#define BYTE_4BIGEND(ptrByte, Number) \
	Number = ptrByte[0]; \
	Number <<= 8;	\
	Number += ptrByte[1];\
	Number <<= 8;	\
	Number += ptrByte[2];\
	Number <<= 8;	\
	Number += ptrByte[3];\


// this function do hashing to strInput, stop hashing upon encounter 0 token
int cryDSChashString(HASH_TYPE flgAlgorithm, const HASH_BYTE *strInput, HASH_BYTE *strOut);


#endif // end function guard DSC_HASH_FNV_H
