#include "VF_file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void print8L(char *buff, int len);

#define TEST_F_NAME	"frantic"

void test_1(){
	// write
	VF_FOLDER dummy;
	VF_FILE hdl;
	int result;
	uch *testV;
	
	result = VF_open(dummy,TEST_F_NAME,&hdl,0);
	printf("VF_open ret: %d\n",result);
	
	testV = (uch*) "00000000";
	result = VF_write(testV,8,0,hdl);
	printf("VF_write ret: %d\n",result);
	
	testV = (uch*) "11111111";
	result = VF_write(testV,8,8,hdl);
	printf("VF_write ret: %d\n",result);
	
	testV = (uch*) "22222222";
	result = VF_write(testV,8,16,hdl);
	printf("VF_write ret: %d\n",result);
	
	testV = (uch*) "33333333";
	result = VF_write(testV,8,24,hdl);
	printf("VF_write ret: %d\n",result);
	
	testV = (uch*) "44444444";
	result = VF_write(testV,8,32,hdl);
	printf("VF_write ret: %d\n",result);
	
	VF_close(hdl);
}

void test_2(){
	// reopen and write
	VF_FOLDER dummy;
	VF_FILE hdl;
	VF_OFFSET sz;
	int result;
	//uch *testV;
	
	result = VF_open(dummy,TEST_F_NAME,&hdl,&sz);
	printf("VF_open ret: %d\n",result);
	printf("size : %d\n", (int)sz);
	
	result = VF_write((uch*)"55555555",8,sz,hdl);
	printf("VF_write ret: %d\n",result);
	
	VF_close(hdl);
}

void test_3(){
	// reopen and read
	VF_FOLDER dummy;
	VF_FILE hdl;
	VF_OFFSET sz;
	int result, copy, i;
	uch *buf;
	
	result = VF_open(dummy,TEST_F_NAME,&hdl, &sz);
	printf("VF_open ret: %d\n",result);
	printf("size : %d\n", (int)sz);
	
	buf = (uch*)malloc(sz);
	copy = sz;
	result = VF_read(buf,&copy,sz,hdl);
	printf("VF_read ret: %d\n", result);
	printf("size : %d\n",(int)copy);
	for(i = 0; i < copy; i += 8){
		print8L((char*)&buf[i],8);
		printf("\n");
	}
	
	VF_close(hdl);
}

void test_4(){
	// insert
	VF_FOLDER dummy;
	VF_FILE hdl;
	VF_OFFSET sz;
	int result;
	
	result = VF_open(dummy,TEST_F_NAME,&hdl, &sz);
	printf("VF_open ret: %d\n",result);
	printf("size : %d\n", (int)sz);
	
	result = VF_insert((uch*)"aaaaaaaa",8,16,0,hdl);
	printf("VF_insert ret: %d\n", result);
	
	result = VF_insert((uch*)"bbbbbbbb",8,24,0,hdl);
	printf("VF_insert ret: %d\n", result);
	
	result = VF_insert((uch*)"ffffffff",8,48,0,hdl);
	printf("VF_insert ret: %d\n", result);
	
	result = VF_insert((uch*)"cccccccc",8,48,8,hdl);
	printf("VF_insert ret: %d\n", result);
	
	VF_close(hdl);
}

void test_5(){
	VF_FOLDER dummy;
	VF_deleteClosed(dummy,TEST_F_NAME);
}