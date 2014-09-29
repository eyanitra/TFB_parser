/*******************************************************************************
 * DSChashString.h
 *
 *	Description:
 * 		Data conversion function, this is module for hashing text into
 * 		certain fixed formatted data.
 * 		Currently supported algorithm is Fowler-Noll-Vo.
 *
 *  Author : Evan
 *  Date   : 29 July 2013
 ******************************************************************************/

#include "DSC_hash.h"

typedef unsigned int HASH_FNV_4BYTE;

#ifdef DSC_LIB_COMPILE		// guard for direct debugging

int cryDSChashString(HASH_TYPE flgAlgorithm, const HASH_BYTE *strInput, HASH_BYTE *strOut)
{
	// FNV64 offset basis 14695981039346656037
	// binary form: 1100101111110010100111001110010010000100001000100010001100100101
	// hexadecimal form: 0xCBF29CD484222325
	// FNV64 prime = 2^40 + 2^8 0xb3

	// FNV32 offset basis 2166136261
	// binary form: 10000001000111001001110111000101
	// hexadecimal form: 0x811C9DC5
	// FNV32 prime = 2^24 + 2^8 + 0x93 = 2^24 + 2^8 + 2^7 + 2^4 + 2^1 + 2^0
	HASH_BYTE ctrBasis32[4] = {0x81,0x1C,0x9D,0xC5};
	HASH_FNV_4BYTE ctrAccumulator32, ctrInputCast;

	// algorithm switch
	switch(flgAlgorithm)
	{
	case fnvOne32:
	case fnvOne16:
	case fnvOne8:
		// we doing this for consistencty of Big-Endian form
		BYTE_4BIGEND(ctrBasis32,ctrAccumulator32);
		while(*strInput != 0)
		{
			// accumulator *= FNV32 prime
			ctrAccumulator32 += (ctrAccumulator32 << 24) + \
					(ctrAccumulator32 << 8) + \
					(ctrAccumulator32 << 7) + \
					(ctrAccumulator32 << 4) + \
					(ctrAccumulator32 << 1);

			// accumulator ^= *strInput
			ctrInputCast = *strInput;
			ctrAccumulator32 *= ctrInputCast;
			strInput++;
		};
		break;
	default:
		return 1;
	}

	// result switch with XOR folding
	// result: 	123456789ABCD (13 BIT)
	// wanted: 	xxxx123456789 (9  BIT)
	// folding way
	// push :	0000000001234 (result >> 9 BIT)
	// xor  :	000056789ABCD (result masked 9 BIT)
	switch(flgAlgorithm)
	{
	case fnvOne32:
		BIGEND_4BYTE(strOut, ctrAccumulator32);
		break;
	case fnvOne16:
		BIGEND_4BYTE(ctrBasis32, ctrAccumulator32);
		strOut[0] = ctrBasis32[0] ^ ctrBasis32[2];
		strOut[1] = ctrBasis32[1] ^ ctrBasis32[3];
		break;
	case fnvOne8:
		BIGEND_4BYTE(ctrBasis32, ctrAccumulator32);
		strOut[0] = ctrBasis32[0] ^ ctrBasis32[1] ^ ctrBasis32[2] ^ ctrBasis32[3];
		break;
	}
	return 0;
}

#endif // end guard DSC_LIB_COMPILE
