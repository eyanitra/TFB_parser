/******************************************************************************
 * 		DSCbigNumber.h
 *
 * 		Description:
 *
 * 		DSC is name id for data string conversion, one of them is bno
 * 		(big number operation), all of binary is in Big Endian style.
 *
 * 		Author : Evan
 * 		Date   : 1 August 2013
 *****************************************************************************/
#ifndef DSC_BIG_NUMBER
#define DSC_BIG_NUMBER

#include "DSC_converter.h"
typedef unsigned char HEX;


BIN_T dscBinaryOperatorCarry_AplusB(BIN_T *Result, BIN_T *OperandA, BIN_T *OperandB, unsigned int ByteLength);
BIN_T dscBinaryOperatorBorrow_BminA(BIN_T *Result, BIN_T *OperandB, BIN_T *OperandA, unsigned int ByteLength);

BCD_T dscBcdOperatorCarry_APlusB(BCD_T *Result, BCD_T *Bcd_A, BCD_T *Bcd_B, int Digit);
BCD_T dscBcdOperatorBorrow_BMinA(BCD_T *Result, BCD_T *Bcd_A, BCD_T *Bcd_B, int Digit);
BCD_T dscBcdOperatorModulus_DivBy2(BCD_T *Out_Quotient, BCD_T *In_Divident, int Digit);

HEX dscNibbleOperator_RightShift(HEX *bufInput, HEX *bufOutput, int ByteLength);

#endif	// end guard DSC_BIG_NUMBER
