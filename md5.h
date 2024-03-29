/*
 * This is an OpenSSL-compatible implementation of the RSA Data Security, Inc.
 * MD5 Message-Digest Algorithm (RFC 1321).
 *
 * Homepage:
 * http://openwall.info/wiki/people/solar/software/public-domain-source-code/md5
 *
 * Author:
 * Alexander Peslyak, better known as Solar Designer <solar at openwall.com>
 *
 * This software was written by Alexander Peslyak in 2001.  No copyright is
 * claimed, and the software is hereby placed in the public domain.
 * In case this attempt to disclaim copyright and place the software in the
 * public domain is deemed null and void, then the software is
 * Copyright (c) 2001 Alexander Peslyak and it is hereby released to the
 * general public under the following terms:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted.
 *
 * There's ABSOLUTELY NO WARRANTY, express or implied.
 *
 * See md5.c for more information.
 */
 
#ifndef MD5_H
#define MD5_H

#include <stdint.h>

#define MD5_SIZE 16

struct md5_ctx {
	uint32_t lo, hi;
	uint32_t a, b, c, d;
	unsigned char buffer[64];
	uint32_t block[16];
};
 
extern void md5_init(struct md5_ctx *ctx);
extern void md5_update(struct md5_ctx *ctx, void *data, unsigned long size);
extern void md5_final(unsigned char *result, struct md5_ctx *ctx);

#endif /* MD5_H */
