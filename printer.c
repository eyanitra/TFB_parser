#include <stdio.h>

void print8L(char *buffer, int bufferLength)
{
	char LineCache[30];
	int i;
	char n;

	memset(LineCache,0x00,sizeof(LineCache));
	for(i = 0, n = 0; i<bufferLength; i++){
		sprintf(&LineCache[n*3], "%02X ", buffer[i]);
		if( n == 7 ){
			Printf(LineCache);
			n = 0;
			memset(LineCache, 0x00, sizeof(LineCache));
		}
		else
			++n;
	}
	if (n != 0){
		strcat(LineCache, "\n");
		Printf(LineCache);
	}
}