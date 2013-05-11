#include <stdio.h>
#include "sar.h"

int main(int argc, const char *argv[])
{
	sar_create(argv[1], argc-2, argv+2);
	return 0;
}
