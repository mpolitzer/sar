#ifndef SAR_H
#define SAR_H

#include <stdint.h>
#include <assert.h>

#define SAR "SAR"
/* opaque structure, only here to avoid mallocs */
struct sar_header {
	char     sar[sizeof(SAR)];
	uint32_t n;
};

struct sar_entry {
	char fname[104];
	uint8_t  md5[16];
	uint32_t beg, size;
};

struct sar_it {
	struct sar_entry cur;
	FILE  *hp, *wp;
	uint32_t n;
};

int         sar_create     (const char *out, int argc, const char *argv[]);

int         sar_it_init    (struct sar_it *it, const char *fname);
void        sar_it_fini    (struct sar_it *it);

int         sar_it_x_root  (struct sar_it *it, const char *root);
int         sar_it_x       (struct sar_it *it);
int         sar_it_md5check(struct sar_it *it);

int         sar_it_first   (struct sar_it *it);
int         sar_it_next    (struct sar_it *it);

const char *sar_it_getname (struct sar_it *it);
uint32_t    sar_it_getsize (struct sar_it *it);

#endif /* SAR_H */
