/*
 * uuid.h -- private header file for uuids
 * 
 * Copyright (C) 1996, 1997 Theodore Ts'o.
 *
 * %Begin-Header%
 * This file may be redistributed under the terms of the GNU 
 * Library General Public License.
 * %End-Header%
 */

#include <sys/types.h>
/*jowenn: #include <uuid/uuid_types.h>*/

/*js*/
#ifdef _WIN32
/*.. */
# define longlong __int64
#else /* non-win32 systems */
# define longlong long long
#endif

#ifdef __linux__
# include <linux/types.h>
#endif

#ifdef __FreeBSD__
typedef u_int32_t __u32;
typedef u_int16_t __u16;
typedef u_int8_t __u8;
#endif

#include "uuid.h"

/*
 * Offset between 15-Oct-1582 and 1-Jan-70
 */
#define TIME_OFFSET_HIGH 0x01B21DD2
#define TIME_OFFSET_LOW  0x13814000

struct uuid {
	__u32	time_low;
	__u16	time_mid;
	__u16	time_hi_and_version;
	__u16	clock_seq;
	__u8	node[6];
};


/*
 * prototypes
 */
void uuid_pack(const struct uuid *uu, uuid_t ptr);
void uuid_unpack(const uuid_t in, struct uuid *uu);




