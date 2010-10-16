/*===--- common.h - common functions ------------------------------------===//
 *
 * Copyright 2010 Bobby Powers
 * Licensed under the GPLv2 license
 *
 * see COPYING for details.
 *
 *===--------------------------------------------------------------------===//
 */
#ifndef _COMMON_H_
#define _COMMON_H_

// for asprintf
#define _GNU_SOURCE 1

#include <string.h>
#include <stdbool.h>
#include <stdint.h>

// the '!!' is from Love's LSP book.  not quite sure why they're
// desirable.  The ones listed on KernelTrap don't have them.
#define likely(x)   __builtin_expect(!!(x),1)
#define unlikely(x) __builtin_expect(!!(x),0)
#define __packed    __attribute__((packed))

// backlog arg for listen(2); max clients to keep in queue
extern const int BACKLOG;
extern const char *program_name;

// allocates memory or exits
void *xmalloc0(size_t size);
void *xmalloc(size_t size);

// passed to evbuffer_add_reference as a function to call when the
// buffer it points to isn't referenced anymore.
void free_cb(const void *data, size_t datalen, void *extra);

// calls perror with msg and exits, indicating failure
void exit_perr(const char *msg_fmt, ...);
// prints msg to stderr and exits, indicating failure
void exit_msg(const char *msg_fmt, ...);

int sha256(uint8_t *hash, void *data, size_t len);
char *sha256_hex_file(const char *path, size_t len);

struct ccgi_state {
	int32_t socket;
	const char *mount_point; // not currently used
	struct evhttp *ev_http;
	struct event_base *ev_base;
	const char *artist_list;
	const char *album_list;
};

void ccgi_state_init(struct ccgi_state *state);

#endif // _COMMON_H_