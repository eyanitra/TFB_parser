#include <stdio.h>
#include <string.h>
#include "TFB_parser.h"
#include "BPPblackListRecord.h"
#include "DSC_converter.h"
#include "TLV_serializer.h"
#include "BPP_blacklistControl.h"

extern void print8L(char *buffer, int bufferLength);
extern void printCode(char *buffer, int bufferLength);

//@testFile.c
extern void test_1();
extern void test_2();
extern void test_3();
extern void test_4();
extern void test_5();

extern void testPars_1();

void testing()
{	
	// todo complete testing here
	blackListGetVersion();
	blackListSetVersion((int)30);
	blackListIsElementExist((uch*)"\x01\x02\x03\x04\x05\x06\x07\x08");
}

void pControlLine(){
	unsigned char lv0[50], *d, lv1[25];
	int len;
	
	// tag without child, must have length == 0
	d = lv1;
	d += TLV_writeTlv(0,BL_VERSION_TAG,0,(unsigned char*)" ",d);
	d += TLV_writeTlv(0,PARAM_VERSION_TAG,0,(unsigned char*)" ",d);
	d += TLV_writeTlv(0,PARAM_DATA_TAG, 0,0,d);
	d += TLV_writeTlv(0,VALID_NODE_TAG,0,0,d);
	len =  d - lv1;
	d = lv0;
	d += TLV_writeTlv(0,TAG_CHECKER,len,lv1,lv0);
	len = d - lv0;
	print8L((char*)lv0,len);
	printCode((char*)lv0, len);
}

void testFile()
{
	test_5();
	test_1();
	test_2();
	test_3();
	test_4();
	test_3();
	
}

int main(int argc, char **argv)
{
	
	testPars_1();
	//testFile();
	// build control string
	pControlLine();
	// test blacklist
	return 0;
}
