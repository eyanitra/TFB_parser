/******************************************************************************
 * 		DSCbigNumber.c
 *
 * 		Description:
 * 			big number operation, Big endian style
 *
 * 		module name bno = big number operation
 *
 * 		Author : Evan
 * 		Date   : 1 August 2013
 *****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif


// Compile selector for compile on TELIUM or standard GCC
#ifdef _EFT30_
#	define TRACE(arg...)
#	define TRHEX(buffer, bufferLength)
#else
#	define TRACE(arg...)
#	define TRHEX(buffer, bufferLength)
#	define COMPILE_CONVERSION
#endif


#include "DSC_bignum.h"

BIN_T dscBinaryOperatorCarry_AplusB(BIN_T *Result, BIN_T *OperandA, BIN_T *OperandB, unsigned int ByteLength)
{
	BIN_T TCarrierR = 0;
	BIN_T Acc;
	int counter = ByteLength -1;
	while(counter >= 0){
		Acc = OperandB[counter] + TCarrierR;
		if(Acc >= OperandB[counter]){
			Acc += OperandA[counter];
			if(Acc >= OperandA[counter])
				TCarrierR = 0;
			else
				TCarrierR = 1;
		}
		else{
			TCarrierR = 1;
			Acc += OperandA[counter];
		}
		Result[counter] = Acc;
		counter--;
	}
	return TCarrierR;
}

BIN_T dscBinaryOperatorBorrow_BminA(BIN_T *Result, BIN_T *OperandB, BIN_T *OperandA, unsigned int ByteLength)
{
	BIN_T TCarrierR = 1, ctrAcc;
	//invert A to 2Complement
	int counter = ByteLength-1;
	while(counter >= 0){
		Result[counter] = 0;
		ctrAcc = ~OperandA[counter];
		Result[counter] = ctrAcc + TCarrierR;
		if(Result[counter]< ctrAcc){
			TCarrierR = 1;
			TRACE("carry");
		}
		else{
			TCarrierR = 0;
			TRACE("no carry");
		}
		--counter;
	}
	TRHEX(Result,ByteLength);
	// addition
	if(dscBinaryOperatorCarry_AplusB(Result, OperandB, Result, ByteLength))
		return 0;
	return 1;
}

// add big number A and B in ASCII form
// strOpA and strOpB string must:
// - have length equal to flgStrLenA
// - not contain non numerical ASCII character
// strRslt length must equal to flgStrLenA
// return :
// 0x30 (no carry), 0x31 (carry),
// 0xFF (non numerical token), 0xFE(negative length)
HEX bnoDSCasciiAPlusB(HEX *strRslt, HEX *strOpA, HEX *strOpB, int flgStrLenA)
{
	unsigned char flgACarry, ctrAccm, ctrAccmB;
	int ctrLoop;

	if(flgStrLenA < 0)return 0xFE;

	flgACarry = 0;
	//strRslt[flgStrLenA] = 0;// 24/11/2013 check its result to decimalStrToBinary
	for(ctrLoop = flgStrLenA-1; ctrLoop >= 0; ctrLoop--){
		//inspect token
		ctrAccm = strOpA[ctrLoop];
		ctrAccmB = strOpB[ctrLoop];

		ctrAccm = ctrAccm + ctrAccmB + flgACarry;
		ctrAccmB = (0xF0 & ctrAccm);// detect carry
		ctrAccm &= 0xF;				// detect last nibble
		switch(ctrAccmB){
		case 0x60:
			if(ctrAccm >= 0xA){
				// number - 0x0A + 0x30
				strRslt[ctrLoop] = ctrAccm + 0x26;
				flgACarry = 1;
			}
			else{
				strRslt[ctrLoop] = ctrAccm + 0x30;
				flgACarry = 0;
			}
			break;
		case 0x70:
			strRslt[ctrLoop] = ctrAccm + 0x36;
			flgACarry = 1;
			break;
		}
	}
	return flgACarry + 0x30;
}

// Subtract big number A from B in ASCII form
// strOpA and strOpB string must:
// - have length equal to flgStrLenA
// - not contain non numerical ASCII character
// strRslt length must equal to flgStrLenA
// return :
// 0x30 (no borrow/positive), 0x31 (borrow/negative),
// 0xFE(negative length)
HEX bnoDSCasciiBMinA(HEX *strRslt, HEX *strOpA, HEX *strOpB, int flgStrLenA)
{
	unsigned char flgAbrw, ctrAccm, ctrAccmB;
	int ctrLoop;

	if(flgStrLenA < 0)return 0xFE;
	strRslt[flgStrLenA] = 0;
	flgAbrw = 0;

	for(ctrLoop = flgStrLenA-1; ctrLoop >= 0; ctrLoop--)
	{
		ctrAccm = strOpA[ctrLoop];
		ctrAccmB = strOpB[ctrLoop];

		ctrAccmB -= flgAbrw;
		ctrAccmB -= ctrAccm;

		if(ctrAccmB > 0x39)	// negative overflow: (-1)0xFF-->0x39, (-2)0xFE-->0x38
		{
			strRslt[ctrLoop] = (ctrAccmB & 0xFF) - 0xC6;
			flgAbrw = 1;
		}
		else
		{
			strRslt[ctrLoop] = (ctrAccmB & 0xF) + 0x30;
			flgAbrw = 0;
		}
	}
	return 0x30 + flgAbrw;
}

// divide strDivident in ASCII form with 2
// put quotient to strQuotient
// this function will not check token validity
// both strQuotient and strDivident's length must equal to flgStrLen
// if bufDivident can not divided by 2 return 1
// otherwise return 0
HEX bnoDSCasciiDiv(HEX *strQuotient, HEX *strDivident, int flgStrLen)
{
	int ctrByte;
	HEX ctrAccQt, flgComp;
	// set register
	flgComp = 0;
	for(ctrByte = 0; ctrByte < flgStrLen; ctrByte++)
	{	// extract byte
		ctrAccQt = strDivident[ctrByte] - 0x30;
		// compute byte/2
		if(flgComp)
			ctrAccQt += 10;
		flgComp = ctrAccQt & 1;
		ctrAccQt >>= 1;
		// save byte
		strQuotient[ctrByte] = ctrAccQt + 0x30;
	}

	return flgComp;
}

#define FALSE 					0
#define TRUE					1

#define BCD_MASK_HIGH_DIGIT		0xF0
#define BCD_MASK_LOW_DIGIT		0xF
#define BCD_MAX_HIGH_DIGIT		0x90
#define BCD_NORM_HIGH_DIGIT		0xA0
#define BCD_CARRY_HIGH			0x10
#define BCD_MAX_LOW_DIGIT		0x9
#define BCD_NORM_LOW_DIGIT		0xA

BCD_T dscBcdOperatorCarry_APlusB(BCD_T *Result, BCD_T *Bcd_A, BCD_T *Bcd_B, int Digit)
{
	char FlgLoad;
	unsigned short AccHi, RegA, RegB, AccLow, Carry;
	int ctrByte;

	ctrByte = ((Digit +1)>> 1)-1;
	AccLow = Result[ctrByte] & BCD_MASK_LOW_DIGIT;
	for(Carry = 0, FlgLoad = FALSE; Digit > 0; Digit--)
	{
		if(FlgLoad == FALSE){
			RegA = Bcd_A[ctrByte];
			RegB = Bcd_B[ctrByte];
			FlgLoad = TRUE;
		}
		if(Digit & 1){
			AccHi = (RegA & BCD_MASK_HIGH_DIGIT) + (RegB & BCD_MASK_HIGH_DIGIT) + Carry;
			if(AccHi > BCD_MAX_HIGH_DIGIT){
				AccHi -= BCD_NORM_HIGH_DIGIT;
				Carry = 1;
			}
			else
				Carry = 0;

			AccHi += AccLow;
			Result[ctrByte] = (BCD_T)AccHi;
			--ctrByte;
			FlgLoad = FALSE;
		}
		else{
			AccLow = (RegA & BCD_MASK_LOW_DIGIT) + (RegB & BCD_MASK_LOW_DIGIT) + Carry;
			if(AccLow > BCD_MAX_LOW_DIGIT){
				AccLow -= BCD_NORM_LOW_DIGIT;
				Carry = BCD_CARRY_HIGH;
			}
			else
				Carry = 0;
		}

	}

	return (BCD_T)Carry;
}

BCD_T dscBcdOperatorBorrow_BMinA(BCD_T *Result, BCD_T *Bcd_A, BCD_T *Bcd_B, int Digit)
{
	unsigned char  FlgLoad;
	short Borrow, RegA, RegB, AccHi, AccLow;
	int ctrByte;

	ctrByte = ((Digit +1)>> 1)-1;
	AccLow = Result[ctrByte] & BCD_MASK_LOW_DIGIT;
	for(FlgLoad = FALSE, Borrow = 0; Digit > 0; Digit--)
	{
		if(FlgLoad == FALSE){
			RegA = Bcd_A[ctrByte];
			RegB = Bcd_B[ctrByte];
			FlgLoad = TRUE;
		}
		if(Digit & 1){
			AccHi = (RegB & BCD_MASK_HIGH_DIGIT) - (RegA & BCD_MASK_HIGH_DIGIT) - Borrow;
			if(AccHi < 0){
				AccHi += BCD_NORM_HIGH_DIGIT;
				Borrow = 1;
			}
			else Borrow = 0;

			AccHi += AccLow;
			Result[ctrByte] = (BCD_T)AccHi;
			--ctrByte;
			FlgLoad = FALSE;
		}
		else{
			AccLow = (RegB & BCD_MASK_LOW_DIGIT) - (RegA & BCD_MASK_LOW_DIGIT) - Borrow;
			if(AccLow < 0){
				AccLow += BCD_NORM_LOW_DIGIT;
				Borrow = BCD_CARRY_HIGH;
			}
			else Borrow = 0;
		}
	}

	return (BCD_T)Borrow;
}


BCD_T dscBcdOperatorModulus_DivBy2(BCD_T *Out_Quotient, BCD_T *In_Divident, int Digit)
{

	int ctrDigit, ctrAccm;
	unsigned short Reg, Carry, AccHi;

	for(ctrDigit = 0, Carry = 0; ctrDigit < Digit; ++ctrDigit){
		if(ctrDigit & 1){
			ctrAccm = Reg & 0xF;
			if(Carry)
				ctrAccm += BCD_NORM_LOW_DIGIT;
			Carry = ctrAccm & 1;
			AccHi += ctrAccm >> 1;
			Out_Quotient[ctrDigit >>1] = (BCD_T)AccHi;
		}
		else{
			Reg = In_Divident[ctrDigit >>1];
			ctrAccm = Reg >> 4;
			if(Carry)
				ctrAccm += BCD_NORM_LOW_DIGIT;
			Carry = ctrAccm & 1;
			ctrAccm >>= 1;
			AccHi = ctrAccm << 4;
		}
	}

	if((ctrDigit & 1) == 0){
		AccHi += Reg & 0xF;
		Out_Quotient[ctrDigit >>1] = (BCD_T)AccHi;
	}

	return Carry;
}

HEX dscNibbleOperator_RightShift(HEX *bufInput, HEX *bufOutput, int ByteLength)
{
	// todo buat fungsi ini beroperasi berdasarkan digit length
	unsigned char Temporary, Carry;
	int i;

	for(i = 0, Carry = 0; i < ByteLength; ++i){
		Temporary = bufInput[i];
		bufOutput[i] = (Carry) + (Temporary >> 4);
		Carry = Temporary & 0xF;
		Carry <<= 4;
	}
	return Carry;
}

#ifdef __cplusplus
}
#endif
