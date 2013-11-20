#include <stdio.h>
#include "types.h"

int main()
{
	ui64 a = 0xaabbccddeeff0011;
#ifdef x86
	printf("My target is x86\n");
#endif
	return 0;
}
