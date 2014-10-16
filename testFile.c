#include "VF_file.h"
#include <stdio.h>
#include <string.h>

void test_1(){
	VF_FOLDER dummy;
	VF_FILE hdl;
	printf("VF_open ret: %d\n",VF_open(dummy,"frantic",&hdl,0));
}