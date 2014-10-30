/******************************************************************************
 * 		DSC_converter.c
 *
 * 		Description: numerical conversion utility and operation
 *
 * 		modul name cnv = convert
 *
 * 		Author : Evan
 * 		Date   : 17 Feb 2013
 *****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

// Compile selector for compile on TELIUM or standard GCC
#ifdef _EFT30_
#	include <SDK30.h>
#	define Z_MALLOX(x)		umalloc(x)
#	define Z_FREE(x)		ufree(x)
#	define TRACE(arg...)
#	define ASSERT(condition,arg...)
#else
#	define COMPILE_CONVERSION
#	include <stdlib.h>
#	include <string.h>
#	define Z_MALLOX(x)		malloc(x)
#	define Z_FREE(x)		free(x)
#	define TRACE(arg...)	//doTrace( __FUNCTION__ , arg)
#	define ASSERT(condition,arg...)
#endif

#include "DSC_converter.h"				//data type definition
#include "DSC_bignum.h"


typedef unsigned char BCD_EL;
extern HEX bnoDSCasciiAPlusB(HEX *strRslt, HEX *strOpA, HEX *strOpB, int flgStrLenA);
// convert bufInput into decimal ASCII numerical string
// bufInput must be in bigEndian format (MSB on lower address)
// strOutput or ctrOutByte must large enough to contain null pointer
// since length strOutput computed with biggest bit possible
// there is chance that output number will padded with preceding zero
// if strOutput length or ctrOutByte number less than bytes required
// function will return ERR_CNV_MORE_BUFFER
// on successful conversion function return 0
int dscBinaryToDecimalStr(BIN_T *bufBinary, STR_DEC *strDecimal, int ctrBinaryBufferLen, int ctrDecimalBufferLen)
{
	int ctrMinByte, ctrByte, ctrXcp, ctrBit;
	HEX *bufRoot, flgBitMask, *bufOut;

	// clear out output buffer
	memset(strDecimal ,0x30, ctrDecimalBufferLen);

	// find out number of bit
	for(ctrByte = 0; ctrByte < ctrBinaryBufferLen; ++ctrByte)
	{
		if(bufBinary[ctrByte] != 0)
		{
			flgBitMask = bufBinary[ctrByte];
			break;
		}
	}

	ctrBit = (ctrBinaryBufferLen - ctrByte) << 3;
	if(ctrBit)
	{	// ctrMinByte 8 or bigger
		ctrBit -= 8;
		while(flgBitMask)
		{
			ctrBit++;
			flgBitMask >>= 1;
		}
		TRACE("ctrBit %d",ctrBit);
		// using approximation to count byte needed X/3 ~ X/2 - X/4 + X/16 + X/32 == 0,34375
		ctrBit += 4092;// push about 1*2^10
		ctrMinByte = (ctrBit >> 1) - (ctrBit >> 2) + (ctrBit >> 4) + (ctrBit >> 6) + (ctrBit >> 9);
		ctrMinByte -= 1347;	//(1364 = 4092/3) - 1347 = 7
	}
	else
	{ // ctrMinByte == 0, input 0
		if(ctrDecimalBufferLen < 2)return ERR_CNV_MORE_BUFFER;
		strDecimal[--ctrDecimalBufferLen] = 0;
		return 0;
	}

	// prepare buf root twice of minimum buffer needed
	TRACE("ctrMinByte %d", ctrMinByte);
	bufRoot = Z_MALLOX(ctrMinByte << 1);
	memset(bufRoot,0x30,(ctrMinByte << 1));
	bufRoot[ctrMinByte-1] += 1;
	bufOut = &bufRoot[ctrMinByte];
	TRACE("bufOut %02x, bufRoot %02x",*bufOut, *bufRoot);

	// set register
	bufBinary += ctrBinaryBufferLen;
	bufBinary--;		// bufInput to last byte
	for(ctrByte = 0; ctrByte < ctrBinaryBufferLen; ctrByte++)
	{	// assumption HEX or flgBitMask is 8bit
		for(flgBitMask = 1; flgBitMask != 0; flgBitMask <<=1)
		{
			if(*bufBinary & flgBitMask)
				bnoDSCasciiAPlusB(bufOut,bufOut,bufRoot,ctrMinByte);

			bnoDSCasciiAPlusB(bufRoot,bufRoot,bufRoot,ctrMinByte);

		}
		bufBinary--;
	}

	// find real digit in result need to be copied
	TRACE("bufOut %d, bufRoot %d",*bufOut, *bufRoot);
	for(ctrByte = 0; ctrByte < ctrMinByte; ++ctrByte)
	{
		if(bufOut[ctrByte] > 0x30)
			break;
	}

	TRACE("ctrByte %d, ctrMinByte %d",ctrByte, ctrMinByte);
	ctrXcp = ctrMinByte - ctrByte;
	// check output space
	--ctrDecimalBufferLen;
	if(ctrDecimalBufferLen < ctrXcp )
		return ERR_CNV_MORE_BUFFER;
	//copy result to buffer
	TRACE("ctrXcp %d",ctrXcp);
	memcpy(&strDecimal[ctrDecimalBufferLen - ctrXcp], &bufOut[ctrByte], ctrXcp);
	strDecimal[ctrDecimalBufferLen] = 0;
	Z_FREE(bufRoot);
	return CNV_OK;
}


void dscBinary32ToDecimalStr(unsigned int bin, STR_DEC *strDec, int strDecLen)
{
	HEX *bufRoot, *bufOut;
	unsigned int bitMask, valMask;

	--strDecLen;
	bufRoot = (HEX*) Z_MALLOX((strDecLen) << 1);
	bufOut = &bufRoot[strDecLen];
	memset(bufRoot, 0x30,  strDecLen << 1);
	if(bin > 0){
		bufRoot[strDecLen - 1] += 1;
		for(bitMask = 1, valMask = (-1); bitMask != 0; bitMask <<= 1){
			if(bin & bitMask)
				bnoDSCasciiAPlusB(bufOut, bufOut, bufRoot, strDecLen);
				
			valMask ^= bitMask;
			if((bnoDSCasciiAPlusB(bufRoot, bufRoot, bufRoot, strDecLen) != 0x30)||((valMask & bin) == 0))
				break;	
		}
	}
	memcpy(strDec,bufOut, strDecLen);
	Z_FREE(bufRoot);
 	strDec[strDecLen] = 0;
 	 
}

// convert bufInput binary form to BCD numerical representation
// bufInput must be in bigEndian format (MSB on lower address)
// ctrOutByte initially tells size of bufOutput
// since length bufOutput computed with biggest bit possible
// there is chance that output number will padded with preceding zero
// if bufOutput length or ctrOutByte number less than bytes required
// function will return 1
// on successful conversion function return 0
int dscBinaryToBcd(BIN_T *bufInput, BCD_T *bufOutput, int ctrInByte, int *ctrOutByte)
{
	int ctrMinByte, ctrByte;
	HEX *bufRoot, flgMaskBit;
	// count minimum byte output required (((X3/10*8)+1)+1)/2 ~	((X154/64)+2)/2
	ctrMinByte = ctrInByte <<7;		// X128
	ctrMinByte += ctrInByte <<5;	//  X32
	ctrMinByte -= ((ctrInByte << 1) + (ctrInByte << 2)); // -(X4 + X2)
	ctrMinByte >>= 6;	// X/64
	ctrMinByte >>= 1;	// /2
	ctrMinByte++;
	if(*ctrOutByte < ctrMinByte)
	{
		*ctrOutByte = ctrMinByte;
		return ERR_CNV_MORE_BUFFER;
	}
	*ctrOutByte = ctrMinByte;
	// prepare root buffer
	bufRoot = Z_MALLOX(ctrMinByte);
	memset(bufRoot, 0, ctrMinByte);
	bufRoot[ctrMinByte-1] += 1;
	// prepare output buffer
	memset(bufOutput, 0, ctrMinByte);

	// set buf input to last byte
	bufInput += ctrInByte;
	bufInput --;
	for(ctrByte = 0; ctrByte < ctrInByte; ctrByte++)
	{	// assumption HEX or flgBitMask is 8bit
		for(flgMaskBit = 1; flgMaskBit != 0; flgMaskBit <<= 1)
		{
			if(*bufInput & flgMaskBit)
				dscBcdOperatorCarry_APlusB(bufOutput,bufOutput,bufRoot,ctrMinByte << 1);
			dscBcdOperatorCarry_APlusB(bufRoot,bufRoot,bufRoot,ctrMinByte << 1);
		}
		bufInput--;
	};

	Z_FREE(bufRoot);
	return 0;
}

// convert hexadecimal number to ASCII representation
// if configuration error return 0
STR_HEX dscCNVbinToAsciiToken(BIN_T flgInput, BTH_CFG flgConf)
{
	if(flgInput < 10)
		return flgInput + 0x30;
	switch (flgConf)
	{
	case cnv_uppercase:
		return flgInput +55;
	case cnv_lowercase:
		return flgInput +87;
	default:
		break;
	}
	return 0;
}

// convert bufBinary form to ASCII hexadecimal string
// bufBinary must be in bigEndian format (MSB on lower address)
// strOutput byte length and ctrOutByte at least (strInput length*2)+1
// return:
// CNV_OK
// ERR_CNV_MORE_BUFFER
// ERR_CNV_CONFIG
int dscBinaryToHexStr(BIN_T *bufBinary, STR_HEX *strHexadecimal, int ctrBinaryLen, int *ctrHexLen, BTH_CFG flgConf)
{
	int ctrByte;
	STR_HEX ctrLowNibble, ctrHighNibble;
	// calculate minimum output
	if(((ctrBinaryLen >> 1)+ 1) > *ctrHexLen)
		return ERR_CNV_MORE_BUFFER;
	// check configuration
	switch(flgConf)
	{
	case cnv_lowercase:
	case cnv_uppercase:
		break;
	default :
		return ERR_CNV_CONFIG;
	}
	// conversion
	for(ctrByte = 0; ctrByte < ctrBinaryLen; ctrByte++, bufBinary++)
	{
		ctrLowNibble = *bufBinary & 0xF;
		ctrHighNibble = (*bufBinary & 0xF0) >> 4;

		*strHexadecimal = dscCNVbinToAsciiToken(ctrHighNibble,flgConf);
		strHexadecimal++;

		*strHexadecimal = dscCNVbinToAsciiToken(ctrLowNibble, flgConf);
		strHexadecimal++;
	}

	*strHexadecimal = 0;
	return CNV_OK;
}

// bufBinary must be in bigEndian format (MSB on lower address)
void dscBinaryToHexStrX(const BIN_T *bufBinary, STR_HEX *strOut,  int strLenPlusOne)
{
	int ctrByte;
	STR_HEX ctrLowNibble, ctrHighNibble;

	if(strLenPlusOne <= 1)
		return;
	--strLenPlusOne;
	strLenPlusOne >>= 1;
	for(ctrByte = 0; ctrByte < strLenPlusOne; ctrByte++, bufBinary++)
	{
		ctrLowNibble = *bufBinary & 0xF;
		ctrHighNibble = (*bufBinary & 0xF0) >> 4;

		*strOut = dscCNVbinToAsciiToken(ctrHighNibble,cnv_uppercase);
		strOut++;

		*strOut = dscCNVbinToAsciiToken(ctrLowNibble, cnv_uppercase);
		strOut++;
	}

	*strOut = 0;
}

// number must be in bigEndian format (MSB on lower address)
void dscBinary32ToHexStrX(unsigned int number, STR_HEX *strOut, int strLenPlusOne)
{
	int i;
	STR_HEX token;
	unsigned int res;

	if(strLenPlusOne <= 1)
		return;
	strLenPlusOne -= 2;

	for(i = strLenPlusOne; ((number > 0)&&( i >= 0 )); --i ){
		res = number & 0xF;
		number >>= 4;
		token = res;
		strOut[i] = dscCNVbinToAsciiToken(token,cnv_uppercase);
	}

	while(i >= 0){
		strOut[i--] = 0x30;
	}

	strOut[strLenPlusOne +1] = 0;
}


// convert ASCII numerical representation to BCD representation
// ctrOutSize is length of output buffer,
// in pad_none configuration, it will be changed to output size after conversion
// bufInput is null terminated all number string
// in case of padding configuration, all of output buffer will padded
// with zero in BCD format, and function will not change ctrOutSize
// return:
// CNV_OK
// ERR_CNV_MORE_BUFFER
// ERR_CNV_INPUT
// ERR_CNV_BCD_CONF
int dscDecimalStrToBcd(STR_DEC *strInput, BCD_T *bufOutput, int *ctrOutSize, ATC_CFG flgConf)
{
	BCD_EL ctrTemp;
	int ctrIn, ctrOut, flgMsb;
	// find out length
	ctrIn = strlen((char*)strInput);
	ctrOut = ctrIn + 1;
	ctrOut >>= 1;
	if(*ctrOutSize < ctrOut)return ERR_CNV_MORE_BUFFER; // insufficient ctrOutSize

	// configure offset
	switch(flgConf)
	{
	case pad_back:
		memset(bufOutput, 0, *ctrOutSize);
		flgMsb = 1;
		break;
	case pad_none:
		*ctrOutSize = ctrOut;
		flgMsb = 1;
		// conversion on assumption of preserved buffer
		while(1)
		{
			ctrTemp = *strInput;
			if(ctrTemp == 0)break;	// null token this is the end
			ctrTemp -= 0x30;
			if(ctrTemp > 9)return ERR_CNV_INPUT;	// invalid input value

			if(flgMsb)
			{
				*bufOutput &= 0xF;
				*bufOutput += (ctrTemp << 4);
				flgMsb = 0;
			}
			else
			{
				*bufOutput &= 0xF0;
				*bufOutput += ctrTemp;
				bufOutput++;
				flgMsb = 1;
			}
			strInput++;
		}
		return CNV_OK;
	case pad_face:
		memset(bufOutput, 0, *ctrOutSize);
		bufOutput +=  (*ctrOutSize - ctrOut);
		if(ctrIn & 1)
			flgMsb = 0;
		break;
	default:
		return ERR_CNV_CONFIG;
	}
	// start conversion with blank target buffer
	while(1)
	{
		ctrTemp = *strInput;
		if(ctrTemp == 0)break;	// null token this is the end
		ctrTemp -= 0x30;
		if(ctrTemp > 9)return ERR_CNV_INPUT;	// invalid input value

		if(flgMsb)
		{
			*bufOutput = (ctrTemp << 4);
			flgMsb = 0;
		}
		else
		{
			*bufOutput += ctrTemp;
			bufOutput++;
			flgMsb = 1;
		}
		strInput++;
	}

	return CNV_OK;
}

// buffer size must at least half length of outDigit
// if input string shorter than expected digit
// output will be padded on front
int dscDecimalStrToBcdX(const STR_DEC *strInput, BCD_T *out, int outDigit)
{
	int i;
	BCD_T t, *h;

	h = out;
	for(i = 0; i < outDigit; ++i){
		if(strInput[i] == 0)
			break;
		if((strInput[i] > 0x39)||(strInput[i] < 0x30))
			return 1;

		if(i & 1){
			*h += (strInput[i] - 0x30);
			++h;
		}
		else{
			t = (strInput[i] - 0x30);
			t <<= 4;
			*h = t;
		}
	}
	while(i < outDigit){
		dscNibbleOperator_RightShift(out,out, outDigit << 1);
		++i;
	}
	return 0;
}

// convert BCD form into ASCII decimal string
// BCD with bigEndiand style, MSB on lower address
// strOutput size at least 2*ctrDigitToRead + 1 bytes
// null character is added on the end of strOutput
// function will not check size of strOutput !!!
// there is potential error if you not supply enough byte on strOutput
// return :
// CNV_OK
// ERR_CNV_BCD_READ
// ERR_CNV_BCD_CONF
// ERR_CNV_INPUT
int dscBcdToDecimalStr(BCD_T *bufInput, STR_DEC *strOutput, int ctrDigitToRead, CTA_CFG flgConf)
{
	int ctrOut, flgSkip;
	unsigned char defMask, defTemp;

	if(ctrDigitToRead < 0)return ERR_CNV_BCD_READ;

	switch(flgConf)	// do configuration
	{
	case fnibble_skip: // read second nibble
		defMask = 0xF;
		flgSkip = 0;
		break;
	case fnibble_read:	// read first nibble
		defMask = 0xF0;
		flgSkip = 0;
		break;
	case fzeros_skip:	// skip first zeros
		defMask = 0xF0;
		flgSkip = 1;
		break;
	default:
		return ERR_CNV_CONFIG;
	}
	// start conversion BCD to ASCII
	for(ctrOut=0; ctrOut < ctrDigitToRead; ctrOut++)
	{
		defTemp = *bufInput & defMask;	// take BCD value with mask
		// update mask
		if(defMask == 0x0F)
		{	// after read second nibble -- switch to another byte
			defMask = 0xF0;
			bufInput++;
		}
		else
		{	// after read first nibble move to second
			defMask = 0x0F;
			defTemp >>= 4;
		}

		if(defTemp > 9)return ERR_CNV_INPUT;	// check for faulty input
		if(flgSkip)
		{	// skip zeros
			if(defTemp == 0)
				continue;
			else
				flgSkip = 0;
		}

		*strOutput = defTemp + 0x30; // put on output string
		strOutput++;
	}	// end for
	// seal out with null token
	*strOutput = 0;
	return CNV_OK;
}

extern HEX bnoDSCasciiBMinA(HEX *strRslt, HEX *strOpA, HEX *strOpB, int flgStrLenA);
extern HEX bnoDSCasciiDiv(HEX *strQuotient, HEX *strDivident, int flgStrLen);

// convert ASCII numerical form to Binary form
// Binary with bigEndian format, MSB on lower address
// strInput must be valid ASCII numerical ended with null token
// bufOutput byte length and ctrOutSize at least (strInput length/2)+1
// function will check minimum size of ctrOutSize
// return:
// CNV_OK
// ERR_CNV_INPUT
// ERR_CNV_MORE_BUFFER
int dscDecimalStrToBinary(STR_DEC *strInput, BIN_T *bufOutput, int *ctrOutSize)
{
	// bufOutput size at least 1byte/2bytes strInput
	// the longer strInput, ratio become close to asymptotic value(1bytes out/4 bytes in)
	// but we use fix requirement to avoid complexity
	int ctrInLen, ctrInByte, ctrOutByte, ctrBiggestBit;
	HEX *bufRoot, *bufAccm, *bufPadIn, *bufTemp;
	HEX ctrBorrow, flgMaskBit;

	// find string length and test string token
	flgMaskBit = 0;
	ctrInByte = 0;
	for(ctrInLen = 0; strInput[ctrInLen] != 0; ctrInLen++)
	{
		if((strInput[ctrInLen] > 0x39)||(strInput[ctrInLen] < 0x30))
			return ERR_CNV_INPUT;
		if(flgMaskBit == 0)
		{	// find first non zero leading number
			if((strInput[ctrInLen] != 0x30))
			{
				flgMaskBit = 1;
				bufAccm = &strInput[ctrInLen];
			}
			else
				continue;
		}
		ctrInByte++;
	}
	// return if no need conversion
	if((!ctrInLen)||(!ctrInByte))
	{
		*ctrOutSize = 0;
		return CNV_OK;
	}

	// prepare Accm, Root, PadIn with leading zero
	// create zero padded input in bufPadIn
	// create ...001 in bufAccm
	// create ...0000 in bufRoot
	// create ...0000 in bufTemp
	ctrInByte++;
	TRACE("create buffer 4x:%d", ctrInByte);
	bufRoot = Z_MALLOX(ctrInByte << 2);
	memset(bufRoot, 0x30,(ctrInByte << 2));
	bufPadIn = bufRoot + ctrInByte;
	memcpy(bufPadIn+1, bufAccm, ctrInByte-1);
	bufAccm = bufPadIn + ctrInByte;
	bufAccm[ctrInByte-1] += 1;
	bufTemp = bufAccm + ctrInByte;

	// get biggest bit
	ctrBiggestBit = 0;
	while(1)
	{
		ctrBorrow = bnoDSCasciiBMinA(bufTemp,bufAccm,bufPadIn,ctrInByte);
		if(ctrBorrow != 0x30)
			break;
		ctrBiggestBit++;
		memcpy(bufRoot, bufAccm, ctrInByte);
		bnoDSCasciiAPlusB(bufAccm,bufRoot,bufRoot,ctrInByte);
	}

	// up to this point ctrRootBit must bigger than 0 otherwise error
	// that is why we check input 0 in the beginning
	// find minimum output size
	ASSERT(ctrBiggestBit != 0,"Error BiggestBit zero");
	ctrOutByte = (ctrBiggestBit >> 3);
	if(ctrBiggestBit & 0x7)
		ctrOutByte++;

	TRACE("RootBit:%d, OutByte:%d",ctrBiggestBit, ctrOutByte);
	if(*ctrOutSize < ctrOutByte)
	{	// return if insufficient
		*ctrOutSize = ctrOutByte;
		Z_FREE(bufRoot);
		return ERR_CNV_MORE_BUFFER;
	}
	*ctrOutSize = ctrOutByte;

	// create first mask
	flgMaskBit = 0x80;
	flgMaskBit >>= ((ctrOutByte << 3) - ctrBiggestBit);
	TRACE("Mask :%x, Root:%s",flgMaskBit,bufRoot);

	// clear output buffer
	memset(bufOutput, 0,ctrOutByte);
	// start counting
	for(ctrInLen = 0; ctrInLen < ctrOutByte; ctrInLen++)
	{
		while(flgMaskBit != 0)
		{
			if(bnoDSCasciiBMinA(bufAccm,bufRoot,bufPadIn,ctrInByte) == 0x30)
			{
				memcpy(bufPadIn,bufAccm,ctrInByte);
				bufOutput[ctrInLen] += flgMaskBit;
			}
			bnoDSCasciiDiv(bufRoot,bufRoot,ctrInByte);
			flgMaskBit >>= 1;
		}
		flgMaskBit = 0x80;
	}
	Z_FREE(bufRoot);
	return CNV_OK;
}

// convert BCD numerical form to Binary in form
// Binary using format bigEndian, MSB on lower address
// bufInput must be valid BCD numerical form with length equal to ctrInSize
// return:
// CNV_OK
// ERR_CNV_INPUT
// ERR_CNV_MORE_BUFFER
int dscBcdToBinary(BCD_T *bufInput, BIN_T *bufOutput, int ctrInSize, int *ctrOutSize)
{
	int ctrInByte, ctrTemp, ctrBit, ctrOutByte;
	HEX flgMask, *bufPadIn, *bufAccm, *bufRoot, *bufTemp;
	unsigned char ctrMsn, ctrLsn;
	// count BCD digit
	ctrTemp = 0;
	for(ctrInByte = 0; ctrInByte < ctrInSize; ctrInByte++)
	{
		ctrMsn = bufInput[ctrInByte] & 0xF0;
		ctrLsn = bufInput[ctrInByte] & 0xF;
		if(ctrMsn <= 0x90)
		{
			if(ctrMsn != 0)
				ctrTemp++;
		}
		else
			return ERR_CNV_INPUT;

		if(ctrLsn <= 9)
		{
			if(ctrLsn != 0)
				ctrTemp++;
		}
		else
			return ERR_CNV_INPUT;
	}
	// find out required buffer
	ctrTemp++;
	ctrInByte = ctrTemp >> 1;
	if(!(ctrTemp & 1))
		ctrInByte++;
	if(ctrInByte <= 0)
	{	// no output required
		*ctrOutSize = 0;
		return CNV_OK;
	}
	// create buffer
	bufRoot = Z_MALLOX(ctrInByte << 2);
	memset(bufRoot, 0, (ctrInByte << 2));
	bufAccm = bufRoot + ctrInByte;
	bufTemp = bufAccm + ctrInByte;
	bufPadIn = bufTemp + ctrInByte;
	// prepare buffer
	bufAccm[ctrInByte-1] += 1;
	if(ctrInByte >ctrInSize)
		memcpy(bufPadIn+1,bufInput,ctrInSize);
	else
		memcpy(bufPadIn,bufInput+(ctrInSize-ctrInByte),ctrInByte);
	// get biggest bit
	ctrBit = 0;
	while(1){
		if(dscBcdOperatorBorrow_BMinA(bufTemp,bufAccm,bufPadIn,ctrInByte << 1))
			break;
		memcpy(bufRoot, bufAccm, ctrInByte);
		ctrBit++;
		dscBcdOperatorCarry_APlusB(bufAccm,bufRoot,bufRoot,ctrInByte << 1);
	}
	// count minimum output byte
	ctrOutByte = ctrBit >> 3;
	if(ctrBit & 0x7)ctrOutByte++;
	if(*ctrOutSize < ctrOutByte)
	{
		*ctrOutSize = ctrOutByte;
		Z_FREE(bufRoot);
		return ERR_CNV_MORE_BUFFER;
	}
	*ctrOutSize = ctrOutByte;
	memset(bufOutput, 0, ctrOutByte);
	// create first mask
	flgMask = 0x80;
	flgMask >>= (ctrOutByte << 3) - ctrBit;
	// fill out bit
	TRACE("Mask :%x, OutByte: %d",flgMask, ctrOutByte);
	for(ctrTemp = 0; ctrTemp < ctrOutByte; ctrTemp++){
		while(flgMask){
			if(dscBcdOperatorBorrow_BMinA(bufAccm,bufRoot,bufPadIn, ctrInByte << 1) == 0){
				memcpy(bufPadIn,bufAccm,ctrInByte);
				bufOutput[ctrTemp] += flgMask;
			}
			dscBcdOperatorModulus_DivBy2(bufRoot,bufRoot,ctrInByte << 1);
			flgMask >>= 1;
		}
		flgMask = 0x80;
	}
	// free resources
	Z_FREE(bufRoot);
	return CNV_OK;
}

unsigned int dscBcdToBinary32(const BCD_T *BcdInput, int BcdDigit)
{
	BCD_T *Root, *Accm, *Temp, *Temp2;
	unsigned int DigitRoot, Result;
	int ByteCtr;

	if(BcdDigit <= 0)
		return 0;
	ByteCtr = (BcdDigit + 1)>>1;
	Root = Z_MALLOX(ByteCtr << 2);
	Temp = Root + ByteCtr;
	Accm = Temp + ByteCtr;
	Temp2 = Accm + ByteCtr;

	memset(Root, 0, ByteCtr << 2);
	memcpy(Accm, BcdInput,ByteCtr);
	if(BcdDigit & 1)
		dscNibbleOperator_RightShift(Accm, Accm, ByteCtr);
	if(!memcmp(Accm,Temp, ByteCtr))
		return 0;
	// finding biggest root loop
	Temp[ByteCtr-1] = 1;
	DigitRoot = 1;
	while(!dscBcdOperatorBorrow_BMinA(Temp2, Temp, Accm, ByteCtr << 1)){
		memcpy(Root,Temp, ByteCtr);
		dscBcdOperatorCarry_APlusB(Temp,Root, Root, ByteCtr	<< 1);
		DigitRoot <<= 1;
		if(DigitRoot == 0){
			Z_FREE(Root);
			return 0;
		}
	}
	DigitRoot >>= 1;
	// base translation loop
	for(Result = 0; DigitRoot > 0; DigitRoot >>= 1 ){
		if(dscBcdOperatorBorrow_BMinA(Temp, Root, Accm, ByteCtr << 1) == 0){
			memcpy(Accm, Temp, ByteCtr);
			Result += DigitRoot;
		}
		dscBcdOperatorModulus_DivBy2(Root, Root, ByteCtr << 1);
	}

	Z_FREE(Root);
	return Result;
};

#define MAX_ALLOWED_DIGIT		12
#define BCD_HIGH_DIGIT			0x10
#define BCD_HIGH_MASK			0x0F

void dscBinary32ToBcd(unsigned int InputNumber, BCD_T *BcdOutput, int BcdDigit)
{
	BCD_T Root[MAX_ALLOWED_DIGIT];
	int ByteLen = (BcdDigit+1)>> 1;

	memset(Root, 0, ByteLen);
	memset(BcdOutput, 0, ByteLen - 1);

	if(BcdDigit & 1){
		Root[ByteLen -1] = BCD_HIGH_DIGIT;
		BcdOutput[ByteLen - 1] &= BCD_HIGH_MASK;
	}
	else{
		Root[ByteLen -1] = 1;
		BcdOutput[ByteLen - 1] = 0;
	}

	while(InputNumber){
		if(InputNumber & 1){
			if(dscBcdOperatorCarry_APlusB(BcdOutput, BcdOutput, Root, BcdDigit)!= 0)
				break;
		}

		InputNumber >>= 1;
		dscBcdOperatorCarry_APlusB(Root, Root, Root, BcdDigit);
	}
}

#define ERROR_HEX_ASCII		0xFF
// return FF on error
unsigned char ConvertSingleByteHexAsciiToBcd(unsigned char AsciiCode){
	if(AsciiCode < 0x30)
		return ERROR_HEX_ASCII;
	if(AsciiCode < 0x3A)	// 0 - 9
		return AsciiCode - 0x30;
	if(AsciiCode < 0x41)
		return ERROR_HEX_ASCII;
	if(AsciiCode < 0x47)	// A - F
		return AsciiCode - 55;
	if(AsciiCode < 0x61)
		return ERROR_HEX_ASCII;
	if(AsciiCode < 0x67)	// a - f
		return AsciiCode - 87;

	return ERROR_HEX_ASCII;
}

// convert ASCII hexadecimal string to Binary form
// Binary with bigEndian format, MSB on lower address
// strInput must be valid ASCII hexadecimal ended with null token
// bufOutput byte length and ctrOutSize at least (strInput length/2)
// function will check minimum size of ctrOutSize
// return:
// CNV_OK
// ERR_CNV_INPUT
// ERR_CNV_MORE_BUFFER
int dscHexStrToBinary(const STR_HEX *strInput, BIN_T *bufOutput, int bufOutputSize)
{
	STR_HEX BinaryValue;
	int ctrNibble,i;

	bufOutputSize <<= 1;
	ctrNibble = 0;
	for(i =0; strInput[ctrNibble] != 0; ctrNibble++){

		BinaryValue = ConvertSingleByteHexAsciiToBcd(strInput[ctrNibble]);
		if(BinaryValue == ERROR_HEX_ASCII)
			return 0;

		if(ctrNibble & 1){
			bufOutput[i] += BinaryValue;
			++i;
		}
		else{
			if(bufOutputSize < ctrNibble)
				return 0;
			bufOutput[i] = BinaryValue << 4;
		}
	}

	++ctrNibble;
	if(!(ctrNibble & 1)){
		ctrNibble >>= 1;
		dscNibbleOperator_RightShift(bufOutput,bufOutput,ctrNibble);
	}
	else
		ctrNibble >>= 1;

	return ctrNibble;
}

#ifdef __cplusplus
}
#endif
