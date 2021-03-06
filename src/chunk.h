#ifndef CMARK_CHUNK_H
#define CMARK_CHUNK_H

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>
#include "buffer.h"

typedef struct {
	unsigned char *data;
	int len;
	int alloc;  // also implies a NULL-terminated string
} chunk;

static inline void chunk_free(chunk *c)
{
	if (c->alloc)
		free(c->data);

	c->data = NULL;
	c->alloc = 0;
	c->len = 0;
}

static inline void chunk_ltrim(chunk *c)
{
	assert(!c->alloc);

	while (c->len && isspace(c->data[0])) {
		c->data++;
		c->len--;
	}
}

static inline void chunk_rtrim(chunk *c)
{
	while (c->len > 0) {
		if (!isspace(c->data[c->len - 1]))
			break;

		c->len--;
	}
}

static inline void chunk_trim(chunk *c)
{
	chunk_ltrim(c);
	chunk_rtrim(c);
}

static inline int chunk_strchr(chunk *ch, int c, int offset)
{
	const unsigned char *p = (unsigned char *)memchr(ch->data + offset, c, ch->len - offset);
	return p ? (int)(p - ch->data) : ch->len;
}

static inline const char *chunk_to_cstr(chunk *c)
{
	unsigned char *str;

	if (c->alloc) {
		return (char *)c->data;
	}
	str = (unsigned char *)malloc(c->len + 1);
	if(str != NULL) {
		memcpy(str, c->data, c->len);
		str[c->len] = 0;
	}
	c->data  = str;
	c->alloc = 1;

	return (char *)str;
}

static inline void chunk_set_cstr(chunk *c, const char *str)
{
	if (c->alloc) {
		free(c->data);
	}
	c->len   = strlen(str);
	c->data  = (unsigned char *)malloc(c->len + 1);
	c->alloc = 1;
	memcpy(c->data, str, c->len + 1);
}

static inline chunk chunk_literal(const char *data)
{
	chunk c = {(unsigned char *)data, data ? strlen(data) : 0, 0};
	return c;
}

static inline chunk chunk_dup(const chunk *ch, int pos, int len)
{
	chunk c = {ch->data + pos, len, 0};
	return c;
}

static inline chunk chunk_buf_detach(strbuf *buf)
{
	chunk c;

	c.len = buf->size;
	c.data = strbuf_detach(buf);
	c.alloc = 1;

	return c;
}

#endif
