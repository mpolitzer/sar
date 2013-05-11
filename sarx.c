#include <stdio.h>
#include <assert.h>

#include "sar.h"

void sar_foreach_print(const char *fname)
{
	struct sar_it it;

	assert(sar_it_init(&it, fname));
	sar_it_first(&it);
	do {
		if (!sar_it_md5check(&it))
			printf("failed to extract %s\n", sar_it_getname(&it));
		else
			assert(sar_it_x(&it));
	} while (sar_it_next(&it));
	sar_it_fini(&it);
}

int main(int argc, const char *argv[])
{
	sar_foreach_print(argv[1]);
	return 0;
}
