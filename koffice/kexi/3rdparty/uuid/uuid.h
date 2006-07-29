/*
 * Public include file for the UUID library
 * 
 * Copyright (C) 1996, 1997, 1998 Theodore Ts'o.
 *
 * %Begin-Header%
 * This file may be redistributed under the terms of the GNU 
 * Library General Public License.
 * %End-Header%
 */

#ifndef _UUID_UUID_H
#define _UUID_UUID_H

/*(js)*/
#ifndef KEXIUUID_EXPORT
# define KEXIUUID_EXPORT
#endif

#include <sys/types.h>
#include <sys/time.h>
#include <time.h>

typedef unsigned char uuid_t[16];

/* UUID Variant definitions */
#define UUID_VARIANT_NCS 	0
#define UUID_VARIANT_DCE 	1
#define UUID_VARIANT_MICROSOFT	2
#define UUID_VARIANT_OTHER	3

#ifdef __cplusplus
extern "C" {
#endif

/* clear.c */
void KEXIUUID_EXPORT uuid_clear(uuid_t uu);

/* compare.c */
int KEXIUUID_EXPORT uuid_compare(const uuid_t uu1, const uuid_t uu2);

/* copy.c */
void KEXIUUID_EXPORT uuid_copy(uuid_t dst, const uuid_t src);

/* gen_uuid.c */
void KEXIUUID_EXPORT uuid_generate(uuid_t out);
void KEXIUUID_EXPORT uuid_generate_random(uuid_t out);
void KEXIUUID_EXPORT uuid_generate_time(uuid_t out);

/* isnull.c */
int KEXIUUID_EXPORT uuid_is_null(const uuid_t uu);

/* parse.c */
int KEXIUUID_EXPORT uuid_parse(const char *in, uuid_t uu);

/* unparse.c */
void KEXIUUID_EXPORT uuid_unparse(const uuid_t uu, char *out);

/* uuid_time.c */
time_t KEXIUUID_EXPORT uuid_time(const uuid_t uu, struct timeval *ret_tv);
int KEXIUUID_EXPORT uuid_type(const uuid_t uu);
int KEXIUUID_EXPORT uuid_variant(const uuid_t uu);

#ifdef __cplusplus
}
#endif

#endif /* _UUID_UUID_H */
