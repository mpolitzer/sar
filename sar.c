/* Simple ARchiver has the files concatenated and a block with the indexes and
 * filenames at the end. */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include <sys/stat.h> /* mkdir */
#include <errno.h>

#include "sar.h"
#include "md5.h"

#define MIN(a,b) ((a) < (b) ? (a) : (b))

static int mkdirp(char *s);
static size_t getfsize(FILE *in);
static void docpy(FILE *in, FILE *out, size_t size);
static void domd5(uint8_t res[MD5_SIZE], FILE *in, size_t size);

int sar_create(const char *fout, int argc, const char *argv[])
{
	int i;
	FILE *in, *out;
	struct sar_header header;
	struct sar_entry entry;

	assert(out = fopen(fout, "wb"));

	/* copy header */
	memset(&header, 0, sizeof(header));
	header.n = argc;
	strncpy(header.sar, "SAR", 4);
	assert(fwrite(&header, sizeof(header), 1, out) == 1);

	/* copy files */
	for (i=0; i<argc; i++) {
		assert(in = fopen(argv[i], "rb"));
		docpy(in, out, getfsize(in));
		assert(!fclose(in));
	}

	/* make index & md5 */
	for (i=0, entry.beg = sizeof(header);
			i < argc;
			i++, entry.beg += entry.size) {
		strncpy(entry.fname, argv[i], sizeof(entry.fname));

		assert(in = fopen(argv[i], "rb"));
		entry.size = getfsize(in);
		domd5(entry.md5, in, entry.size);
		assert(!fclose(in)); 

		assert(fwrite(&entry, sizeof(entry), 1, out) == 1);
	}
	assert(!fclose(out));
	return 0;
}

/* ========================================================================== */
/*  sar_it */

int sar_it_init(struct sar_it *it, const char *fname)
{
	struct sar_header header;

	if ((it->hp = fopen(fname, "rb")) == NULL) return 0;
	if ((it->wp = fopen(fname, "rb")) == NULL) return 0;

	assert(fread(&header, sizeof(header), 1, it->hp) == 1);
	if (strncmp(header.sar, "SAR", 4)) {
		fclose(it->hp);
		fclose(it->wp);
		return 0;
	}
	it->n = header.n;
	return 1;
}

void sar_it_fini(struct sar_it *it)
{
	fclose(it->hp);
	fclose(it->wp);
}

int sar_it_x(struct sar_it *it)
{
	return sar_it_x_root(it, "");
}

int sar_it_x_root(struct sar_it *it, const char *root)
{
	FILE *out;
	char name[256];
	short rlen = strlen(root), nlen = strlen(it->cur.fname);

	/* make the string with path */
	strncpy(name, root, sizeof(name));
	strncpy(name+rlen, it->cur.fname, sizeof(name) - nlen);
	mkdirp(name);

	if (!(out = fopen(name, "wb")))
		return 0;
	fseek(it->wp, it->cur.beg, SEEK_SET);
	docpy(it->wp, out, it->cur.size);
	assert(!fclose(out));
	
	return 1;
}

int sar_it_md5check(struct sar_it *it)
{
	uint8_t md5[MD5_SIZE];
	fseek(it->wp, it->cur.beg, SEEK_SET);
	domd5(md5, it->wp, it->cur.size);
	return memcmp(md5, it->cur.md5, MD5_SIZE) == 0;
}

int sar_it_first(struct sar_it *it)
{
	fseek(it->hp, (signed)-it->n*sizeof(it->cur), SEEK_END);
	return fread(&it->cur, sizeof(it->cur), 1, it->hp) == 1;
}

int sar_it_next(struct sar_it *it)
{
	return fread(&it->cur, sizeof(it->cur), 1, it->hp) == 1;
}

const char *sar_it_getname(struct sar_it *it)
{
	return it->cur.fname;
}

uint32_t sar_it_getsize(struct sar_it *it)
{
	return it->cur.size;
}

const uint8_t *sar_it_getmd5(struct sar_it *it)
{
	return it->cur.md5;
}

#if 0
static uint8_t sar_is_entry_valid(FILE *in,
		const char *fname,
		struct sar_entry *entry)
{
	uint8_t md5[MD5_SIZE];

	fseek(in, entry->beg, SEEK_SET);
	domd5(md5, in, entry->size);
	return memcmp(md5, entry->md5, MD5_SIZE) == 0;
}
#endif

/* ========================================================================== */
/*  helper */

static int mkdirp(char *s)
{
	int err=0, i;
	
	for (i = s[0]=='/' ? 1:0; s[i+1]; i++)
		if (s[i] == '/') {
			s[i] = '\0';
			if (mkdir(s, 0777) && errno != EEXIST) err=-1;
			s[i] = '/';
		}
	return err;
}

static size_t getfsize(FILE *in)
{
	long end, cur = ftell(in);
	fseek(in, 0L, SEEK_END);
	end = ftell(in);
	fseek(in, cur, SEEK_SET);
	return end;
}

static void docpy(FILE *in, FILE *out, size_t size)
{
	while (size) {
		char buf[256];
		size_t bsize = MIN(size, sizeof(buf));
		assert(fread (buf, 1, bsize, in)  == bsize);
		assert(fwrite(buf, 1, bsize, out) == bsize);
		size -= bsize;
	}
}

static void domd5(uint8_t result[MD5_SIZE], FILE *in, size_t size)
{
	struct md5_ctx ctx;

	md5_init(&ctx);
	while (size) {
		char buf[256];
		size_t bsize = MIN(size, sizeof(buf));
		assert(fread (buf, 1, bsize, in) == bsize);
		md5_update(&ctx, buf, bsize);
		size -= bsize;
	}
	md5_final(result, &ctx);
}
