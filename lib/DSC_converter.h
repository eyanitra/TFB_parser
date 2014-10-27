/******************************************************************************
 * 	DSC_converter.h
 *
 * 		Description:
 *
 * 		Conversion function as your buffer operator and string processor.
 * 		code id cnv is contraction of conversion, part of Data String
 * 		Conversion library.
 *
 * 		Author : Evan
 * 		Date   : 17 Feb 2013
 *****************************************************************************/
#ifndef _DSC_CONVERSION_H_
#define _DSC_CONVERSION_H_

typedef unsigned char STR_DEC;	// token for decimal ASCII string representation
typedef unsigned char BCD_T;	// token for Binary coded decimal
typedef unsigned char BIN_T;	// token for native binary code/ Hexadecimal representation
typedef unsigned char STR_HEX;	// token for Hexadecimal ASCII string representation


typedef enum
{
	pad_back,		// zero back padding, -- 0 ditambahkan setelah angka terakhir
	pad_face,		// zero face padding, -- 0 ditambahkan sebelum angka pertama
	pad_none,		// no padding, -- angka ditulis sepanjang input
}ATC_CFG;

typedef enum
{
	fnibble_read,	// read from first digit of BCD number
	fnibble_skip,	// read from second digit of BCD number
	fzeros_skip,	// ignore zero's in front of number
}CTA_CFG;

typedef enum
{
	fd_read,
	fd_skip,
}CTB_CFG;

typedef enum
{
	cnv_uppercase,
	cnv_lowercase,
}BTH_CFG;


// macro to transform 4 BYTE to BIG_ENDIAN integer
#define BIN4_TO_INT_BE(output,inputBinArry, offset)					\
	(output) = ((unsigned int)(inputBinArry)[(offset)  ] << 24)|		\
			   ((unsigned int)(inputBinArry)[(offset)+1] << 16)|		\
			   ((unsigned int)(inputBinArry)[(offset)+2] <<  8)|		\
			   ((unsigned int)(inputBinArry)[(offset)+3]      )			\

// macro to transform BIG_ENDIAN integer to 4 BYTE array
#define BE_INT_TO_BIN4(input, outputBinArry, offset)			\
	do{																\
	(outputBinArry)[(offset)   ] = (BIN_T)(((input) >> 24) & 0xFF);	\
	(outputBinArry)[(offset)+1 ] = (BIN_T)(((input) >> 16) & 0xFF);	\
	(outputBinArry)[(offset)+2 ] = (BIN_T)(((input) >>  8) & 0xFF);	\
	(outputBinArry)[(offset)+3 ] = (BIN_T)(((input)	  ) & 0xFF);	\
	}while(0)														\

// macro to transform 3 BYTE to BIG_ENDIAN integer
#define BIN3_TO_INT_BE(output,inputBinArry, offset)			\
	(output) = ((unsigned int)(inputBinArry)[(offset)  ] << 16)|	\
			   ((unsigned int)(inputBinArry)[(offset)+1] << 8)|		\
			   ((unsigned int)(inputBinArry)[(offset)+2])			\

// macro to transform BIG_ENDIAN integer to 3 BYTE array
#define BE_INT_TO_BIN3(input, outputBinArry, offset)				\
	do{																\
	(outputBinArry)[(offset)   ] = (BIN_T)(((input) >> 16) & 0xFF);	\
	(outputBinArry)[(offset)+1 ] = (BIN_T)(((input) >> 8) & 0xFF);	\
	(outputBinArry)[(offset)+2 ] = (BIN_T)(((input)) & 0xFF);	\
	}while(0)

// macro to transform 2 BYTE array to BIG_ENDIAN integer
#define BIN2_TO_INT_BE(output, inputBinArry, offset)				\
	(output) = ((inputBinArry)[(offset)] << 8) | (inputBinArry)[(offset) +1]

// macro to transform BIG_ENDIAN integer to 2 BYTE array
#define BE_INT_TO_BIN2(input, outputBinArry, offset)				\
	do{																\
	(outputBinArry)[(offset)   ] = (BIN_T)(((input) >>  8) & 0xFF);	\
	(outputBinArry)[(offset)+1 ] = (BIN_T)(((input)	   ) & 0xFF);	\
	}while(0)


/********************************** Return Value **********************************************/

#define CNV_OK						0			// no error
#define ERR_CNV_MORE_BUFFER			1			// supplied buffer is insufficient
#define ERR_CNV_INPUT				2			// input buffer contain unacceptable token
#define ERR_CNV_CONFIG				3			// wrong parameter configuration
#define ERR_CNV_BCD_READ			4			// wrong number of BCD nibble, can not negative

// convert bufInput into ASCII decimal string
// bufBinary must be in bigEndian format (MSB on lower address)
// strDecimal or ctrDecimalBufferLen must large enough to contain null pointer
// since length Output computed with biggest bit possible
// there is chance that output number will padded with preceding zero
// if ctrDecimalBufferLen length bigger than bytes required
// return :
// CNV_OK
// ERR_CNV_MORE_BUFFER
int dscBinaryToDecimalStr(BIN_T *bufBinary, STR_DEC *strDecimal, int ctrBinaryBufferLen, int ctrDecimalBufferLen);


void dscBinary32ToDecimalStr(unsigned int number, STR_DEC *strDec, int strDecLen);

// convert bufInput binary form to BCD numerical representation
// bufInput must be in bigEndian format (MSB on lower address)
// ctrOutByte initially tells size of bufOutput
// since length bufOutput computed with biggest bit possible
// there is chance that output number will padded with preceding zero
// if bufOutput length or ctrOutByte number less than bytes required
// return:
// CNV_OK
// ERR_CNV_MORE_BUFFER
int dscBinaryToBcd(BIN_T *bufInput, BCD_T *bufOutput, int ctrInByte, int *ctrOutByte);

// convert bufInput into ASCII hexadecimal string
// bufInput must be in bigEndian format (MSB on lower address)
// bufOutput or ctrOutByte must large enough to contain null pointer
int dscBinaryToHexStr(BIN_T *bufInput, STR_HEX *strOutput, int ctrInByte, int *ctrOutByte, BTH_CFG flgConf);

// always using uppercase
// bufInput must be in bigEndian format (MSB on lower address)
void dscBinaryToHexStrX(const BIN_T *bufBinary, STR_HEX *strOut,  int strLenPlusOne);

// always using uppercase
// number must be in bigEndian format (MSB on lower address)
void dscBinary32ToHexStrX(unsigned int number, STR_HEX *strOut, int strLenPlusOne);

// convert ASCII decimal string to BCD representation
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
int dscDecimalStrToBcd(STR_DEC *strInput, BCD_T *bufOutput, int *ctrOutSize, ATC_CFG flgConf);

// buffer size must at least half length of outDigit
// if input string shorter than expected digit
// output will be padded on front
int dscDecimalStrToBcdX(const STR_DEC *strInput, BCD_T *out, int outDigit);


// convert ASCII decimal string to Binary form
// Binary with bigEndian format, MSB on lower address
// strInput must be valid ASCII numerical ended with null token
// bufOutput byte length and ctrOutSize at least (strInput length/2)+1
// function will check minimum size of ctrOutSize
// return:
// CNV_OK
// ERR_CNV_INPUT
// ERR_CNV_MORE_BUFFER
int dscDecimalStrToBinary(STR_DEC *strInput, BIN_T *bufOutput, int *ctrOutSize);

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
int dscBcdToDecimalStr(BCD_T *bufInput, STR_DEC *strOutput, int ctrDigitToRead, CTA_CFG flgConf);

// convert BCD numerical form to Binary in form
// Binary using format bigEndian, MSB on lower address
// bufInput must be valid BCD numerical form with length equal to ctrInSize
// return:
// CNV_OK
// ERR_CNV_INPUT
// ERR_CNV_MORE_BUFFER
int dscBcdToBinary(BCD_T *bufInput, BIN_T *bufOutput, int ctrInSize, int *ctrOutSize);

unsigned int dscBcdToBinary32(const BCD_T *BcdInput, int BcdDigit);

void dscBinary32ToBcd(unsigned int InputNumber, BCD_T *BcdOutput, int BcdDigit);

// convert ASCII hexadecimal string to Binary form
// Binary with bigEndian format, MSB on lower address
// strInput must be valid ASCII hexadecimal ended with null token
// bufOutput byte length and ctrOutSize at least (strInput length/2)+1
// function will check minimum size of ctrOutSize
// return output length
int dscHexStrToBinary(STR_HEX *strInput, BIN_T *bufOutput, int bufOutputSize);

#endif	//end _RSD_TIME_COUNTER_H_ guard
