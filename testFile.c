#include "VF_file.h"
#include <stdio.h>
#include <string.h>

void test_1(){
	VF_FOLDER dummy;
	VF_FILE hdl;
	int result;
	uch *testV;
	
	result = VF_open(dummy,"frantic",&hdl,0);
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
	result = VF_write(testV,8,32,hdl);
	printf("VF_write ret: %d\n",result);
	
	testV = (uch*) "44444444";
	result = VF_write(testV,8,48,hdl);
	printf("VF_write ret: %d\n",result);
	
	VF_close(hdl);
}