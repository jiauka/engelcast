/*
 */

#ifndef __UTIL_COMMON_H
#define __UTIL_COMMON_H

#include "defs.h"

#define NFREE(p) if (p) { free(p); p = NULL; }

u32_t 		gettime_ms(void);

char*		url_encode(char *str);
char*		url_decode(char *str);
char*		toxml(char *src);

char*		stristr(char *s1, char *s2);
char*		strdupn(char *p);
u32_t 		hash32(char *str);

bool 		get_interface(struct in_addr *addr);
in_addr_t 	get_localhost(char **name);

char*		make_dlna_content(char *mimetype, u32_t duration);
char*		mimetype2ext(char *mimetype);
u8_t 		mimetype2format(char *mimetype);
char*		find_mimetype(char codec, char *mimetypes[], char *out);
char*		find_pcm_mimetype(u8_t endian, u8_t *sample_size, bool truncable, u32_t sample_rate,
							  u8_t channels, char *mimetypes[], char *options);

#endif
