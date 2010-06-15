/*************************************************************************
 * This implementation has been taken from the OpenOffice 1.0 and modified
 * to use KSpread data types.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Sun has made the contents of this file available subject to the
 *  terms of GNU Lesser General Public License Version 2.1 as
 *  specified in sal/rtl/source/digest.c in the OpenOffice package.
 *
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): Matthias Huetsch <matthias.huetsch@sun.com>
 *
 *
 ************************************************************************/

#include "Digest.h"

#include <stdlib.h>
#include <string.h>

#include <KoConfig.h> // WORDS_BIGENDIAN

#include <kdebug.h>
#include <kcodecs.h>

typedef quint8 sal_uInt8;
typedef quint16 sal_uInt16;
typedef quint32 sal_uInt32;

void rtl_freeZeroMemory(void * p, sal_uInt32 n);
void rtl_freeMemory(void * p);
void rtl_zeroMemory(void * Ptr, sal_uInt32 Bytes);
void rtl_copyMemory(void *Dst, const void *Src, sal_uInt32 Bytes);

#ifndef OSL_LOBYTE
# define OSL_LOBYTE(w)           ((sal_uInt8)((sal_uInt16)(w) & 0xFF))
#endif
#ifndef OSL_HIBYTE
# define OSL_HIBYTE(w)           ((sal_uInt8)(((sal_uInt16)(w) >> 8) & 0xFF))
#endif
#ifndef OSL_MAKEWORD
# define OSL_MAKEWORD(bl, bh)    ((sal_uInt16)((bl) & 0xFF) | (((sal_uInt16)(bh) & 0xFF) << 8))
#endif
#ifndef OSL_MAKEDWORD
# define OSL_MAKEDWORD(wl, wh)   ((sal_uInt32)((wl) & 0xFFFF) | (((sal_uInt32)(wh) & 0xFFFF) << 16))
#endif
#ifndef OSL_LOWORD
# define OSL_LOWORD(d)           ((sal_uInt16)((sal_uInt32)(d) & 0xFFFF))
#endif
#ifndef OSL_HIWORD
# define OSL_HIWORD(d)           ((sal_uInt16)(((sal_uInt32)(d) >> 16) & 0xFFFF))
#endif

/** Define macros for swapping between byte orders.
 */
#ifndef OSL_SWAPWORD
# define OSL_SWAPWORD(w)         OSL_MAKEWORD(OSL_HIBYTE(w),OSL_LOBYTE(w))
#endif
#ifndef OSL_SWAPDWORD
# define OSL_SWAPDWORD(d)        OSL_MAKEDWORD(OSL_SWAPWORD(OSL_HIWORD(d)),OSL_SWAPWORD(OSL_LOWORD(d)))
#endif


/*========================================================================
 *
 * rtlDigest.
 *
 *======================================================================*/
/** Digest Handle opaque type.
 */
typedef void* rtlDigest;

/** Digest Algorithm enumeration.
    @see rtl_digest_create()
 */
enum __rtl_DigestAlgorithm {
    rtl_Digest_AlgorithmMD2,
    rtl_Digest_AlgorithmMD5,
    rtl_Digest_AlgorithmSHA,
    rtl_Digest_AlgorithmSHA1,

    rtl_Digest_AlgorithmHMAC_MD5,
    rtl_Digest_AlgorithmHMAC_SHA1,

    rtl_Digest_AlgorithmInvalid,
    rtl_Digest_Algorithm_FORCE_EQUAL_SIZE
};

/** Digest Algorithm type.
 */
typedef enum __rtl_DigestAlgorithm rtlDigestAlgorithm;


/** Error Code enumeration.
 */
enum __rtl_DigestError {
    rtl_Digest_E_None,
    rtl_Digest_E_Argument,
    rtl_Digest_E_Algorithm,
    rtl_Digest_E_BufferSize,
    rtl_Digest_E_Memory,
    rtl_Digest_E_Unknown,
    rtl_Digest_E_FORCE_EQUAL_SIZE
};

/** Error Code type.
 */
typedef enum __rtl_DigestError rtlDigestError;

typedef rtlDigestError Digest_init_t(void * ctx, const sal_uInt8 * Data, sal_uInt32 DatLen);

typedef void Digest_delete_t(void *ctx);

typedef rtlDigestError Digest_update_t(void * ctx, const void * Data, sal_uInt32 DatLen);

typedef rtlDigestError Digest_get_t(void * ctx, sal_uInt8 * Buffer, sal_uInt32 BufLen);

/*========================================================================
 *
 * rtl_digest_SHA1 interface.
 *
 *======================================================================*/
#define RTL_DIGEST_LENGTH_SHA1 20

/** Create a SHA1 Digest.handle.
    @descr The SHA1 digest algorithm is specified in

    FIPS PUB 180-1 (Supersedes FIPS PUB 180)
      Secure Hash Standard

    @see rtl_digest_create()
 */
rtlDigest rtl_digest_createSHA1(void);


/** Destroy a SHA1 Digest.handle.
    @see rtl_digest_destroy()
 */
void rtl_digest_destroySHA1(rtlDigest Digest);


/** Update a SHA1 digest with given data.
    @see rtl_digest_update()
 */
rtlDigestError rtl_digest_updateSHA1(rtlDigest Digest, const void * pData, uint nDatLen);


/** Finalize a SHA1 digest and retrieve the digest value.
    @see rtl_digest_get()
 */
rtlDigestError rtl_digest_getSHA1(rtlDigest Digest, sal_uInt8 * pBuffer, uint nBufLen);


/** Evaluate a SHA1 digest value from given data.
    @descr This function performs an optimized call sequence on a
 single data buffer, avoiding digest creation and destruction.

 @see rtl_digest_updateSHA1()
 @see rtl_digest_getSHA1()

 @param  pData   [in] data buffer.
 @param  nDatLen [in] data length.
 @param  pBuffer [in] digest value buffer.
 @param  nBufLen [in] digest value length.

 @return rtl_Digest_E_None upon success.
 */
rtlDigestError rtl_digest_SHA1(const void * pData,      uint nDatLen,
                               unsigned char * pBuffer, uint nBufLen);


/*========================================================================
 *
 * rtlDigest internals.
 *
 *======================================================================*/

void rtl_zeroMemory(void * Ptr, sal_uInt32 Bytes)
{
    memset(Ptr, 0, Bytes);
}

void rtl_copyMemory(void *Dst, const void *Src, sal_uInt32 Bytes)
{
    memcpy(Dst, Src, Bytes);
}

void rtl_freeMemory(void * p)
{
    free(p);
}

void rtl_freeZeroMemory(void * p, sal_uInt32 n)
{
    if (p) {
        memset(p, 0, n);
        free(p);
    }
}

#define RTL_DIGEST_CREATE(T) ((T*)(malloc(sizeof(T))))

#define RTL_DIGEST_ROTL(a,n) (((a) << (n)) | ((a) >> (32 - (n))))

#define RTL_DIGEST_HTONL(l,c) \
    (*((c)++) = (sal_uInt8)(((l) >> 24L) & 0xff), \
     *((c)++) = (sal_uInt8)(((l) >> 16L) & 0xff), \
     *((c)++) = (sal_uInt8)(((l) >>  8L) & 0xff), \
     *((c)++) = (sal_uInt8)(((l)       ) & 0xff))

#define RTL_DIGEST_LTOC(l,c) \
    (*((c)++) = (sal_uInt8)(((l)       ) & 0xff), \
     *((c)++) = (sal_uInt8)(((l) >>  8L) & 0xff), \
     *((c)++) = (sal_uInt8)(((l) >> 16L) & 0xff), \
     *((c)++) = (sal_uInt8)(((l) >> 24L) & 0xff))

typedef struct digest_impl_st {
    rtlDigestAlgorithm  m_algorithm;
    sal_uInt32          m_length;
    Digest_init_t      *m_init;
    Digest_delete_t    *m_delete;
    Digest_update_t    *m_update;
    Digest_get_t       *m_get;
} Digest_Impl;

/*
 * __rtl_digest_swapLong.
 */
static void __rtl_digest_swapLong(sal_uInt32 *pData, sal_uInt32 nDatLen)
{
    register sal_uInt32 *X;
    register int         i, n;

    X = pData;
    n = nDatLen;

    for (i = 0; i < n; i++)
        X[i] = OSL_SWAPDWORD(X[i]);
}

/*========================================================================
 *
 * rtlDigest implementation.
 *
 *======================================================================*/
/*
 * rtl_digest_create.

rtlDigest rtl_digest_create (rtlDigestAlgorithm Algorithm)
{
 rtlDigest Digest = (rtlDigest)0;
 switch (Algorithm)
 {
  case rtl_Digest_AlgorithmMD2:
   Digest = rtl_digest_createMD2();
   break;

  case rtl_Digest_AlgorithmMD5:
   Digest = rtl_digest_createMD5();
   break;

  case rtl_Digest_AlgorithmSHA:
   Digest = rtl_digest_createSHA();
   break;

  case rtl_Digest_AlgorithmSHA1:
   Digest = rtl_digest_createSHA1();
   break;

  case rtl_Digest_AlgorithmHMAC_MD5:
   Digest = rtl_digest_createHMAC_MD5();
   break;

  case rtl_Digest_AlgorithmHMAC_SHA1:
   Digest = rtl_digest_createHMAC_SHA1();
   break;

                default: // rtl_Digest_AlgorithmInvalid
   break;
 }
 return Digest;
}


// rtl_digest_queryAlgorithm.

rtlDigestAlgorithm rtl_digest_queryAlgorithm (rtlDigest Digest)
{
 Digest_Impl *pImpl = (Digest_Impl *)Digest;
 if (pImpl)
  return pImpl->m_algorithm;
 else
  return rtl_Digest_AlgorithmInvalid;
}

 // rtl_digest_queryLength.
sal_uInt32 rtl_digest_queryLength (rtlDigest Digest)
{
 Digest_Impl *pImpl = (Digest_Impl *)Digest;
 if (pImpl)
  return pImpl->m_length;
 else
  return 0;
}

// * rtl_digest_init.
rtlDigestError rtl_digest_init (
 rtlDigest Digest, const sal_uInt8 *pData, sal_uInt32 nDatLen)
{
 Digest_Impl *pImpl = (Digest_Impl *)Digest;
 if (pImpl)
 {
  if (pImpl->m_init)
   return pImpl->m_init (Digest, pData, nDatLen);
  else
   return rtl_Digest_E_None;
 }
 return rtl_Digest_E_Argument;
}

// * rtl_digest_update.
rtlDigestError rtl_digest_update (
 rtlDigest Digest, const void *pData, sal_uInt32 nDatLen)
{
 Digest_Impl *pImpl = (Digest_Impl *)Digest;
 if (pImpl && pImpl->m_update)
  return pImpl->m_update (Digest, pData, nDatLen);
 else
  return rtl_Digest_E_Argument;
}

// * rtl_digest_get.
rtlDigestError rtl_digest_get (
 rtlDigest Digest, sal_uInt8 *pBuffer, sal_uInt32 nBufLen)
{
 Digest_Impl *pImpl = (Digest_Impl *)Digest;
 if (pImpl && pImpl->m_get)
  return pImpl->m_get (Digest, pBuffer, nBufLen);
 else
  return rtl_Digest_E_Argument;
}

// * rtl_digest_destroy.
void rtl_digest_destroy (rtlDigest Digest)
{
 Digest_Impl *pImpl = (Digest_Impl *)Digest;
 if (pImpl && pImpl->m_delete)
  pImpl->m_delete (Digest);
}
*/

/*========================================================================
 *
 * rtl_digest_(SHA|SHA1) common internals.
 *
 *======================================================================*/
#define DIGEST_CBLOCK_SHA 64
#define DIGEST_LBLOCK_SHA 16

typedef sal_uInt32 DigestSHA_update_t (sal_uInt32 x);

static sal_uInt32 __rtl_digest_updateSHA_1(sal_uInt32 x);

typedef struct digestSHA_context_st {
    DigestSHA_update_t *m_update;
    sal_uInt32          m_nDatLen;
    sal_uInt32          m_pData[DIGEST_LBLOCK_SHA];
    sal_uInt32          m_nA, m_nB, m_nC, m_nD, m_nE;
    sal_uInt32          m_nL, m_nH;
} DigestContextSHA;

typedef struct digestSHA_impl_st {
    Digest_Impl      m_digest;
    DigestContextSHA m_context;
} DigestSHA_Impl;

static void __rtl_digest_initSHA(
    DigestContextSHA *ctx, DigestSHA_update_t *fct);

static void __rtl_digest_updateSHA(DigestContextSHA *ctx);
static void __rtl_digest_endSHA(DigestContextSHA *ctx);

#define K_00_19 (sal_uInt32)0x5a827999L
#define K_20_39 (sal_uInt32)0x6ed9eba1L
#define K_40_59 (sal_uInt32)0x8f1bbcdcL
#define K_60_79 (sal_uInt32)0xca62c1d6L

#define F_00_19(b,c,d) ((((c) ^ (d)) & (b)) ^ (d))
#define F_20_39(b,c,d) ((b) ^ (c) ^ (d))
#define F_40_59(b,c,d) (((b) & (c)) | ((b) & (d)) | ((c) & (d)))
#define F_60_79(b,c,d) F_20_39(b,c,d)

#define BODY_X(i) \
    (X[(i)&0x0f] ^ X[((i)+2)&0x0f] ^ X[((i)+8)&0x0f] ^ X[((i)+13)&0x0f])

#define BODY_00_15(u,i,a,b,c,d,e,f) \
    (f)  = X[i]; \
    (f) += (e) + K_00_19 + RTL_DIGEST_ROTL((a), 5) + F_00_19((b), (c), (d)); \
    (b)  = RTL_DIGEST_ROTL((b), 30);

#define BODY_16_19(u,i,a,b,c,d,e,f) \
    (f)  = BODY_X((i)); \
    (f)  = X[(i)&0x0f] = (u)((f)); \
    (f) += (e) + K_00_19 + RTL_DIGEST_ROTL((a), 5) + F_00_19((b), (c), (d)); \
    (b)  = RTL_DIGEST_ROTL((b), 30);

#define BODY_20_39(u,i,a,b,c,d,e,f) \
    (f)  = BODY_X((i)); \
    (f)  = X[(i)&0x0f] = (u)((f)); \
    (f) += (e) + K_20_39 + RTL_DIGEST_ROTL((a), 5) + F_20_39((b), (c), (d)); \
    (b)  = RTL_DIGEST_ROTL((b), 30);

#define BODY_40_59(u,i,a,b,c,d,e,f) \
    (f)  = BODY_X((i)); \
    (f)  = X[(i)&0x0f] = (u)((f)); \
    (f) += (e) + K_40_59 + RTL_DIGEST_ROTL((a), 5) + F_40_59((b), (c), (d)); \
    (b)  = RTL_DIGEST_ROTL((b), 30);

#define BODY_60_79(u,i,a,b,c,d,e,f) \
    (f)  = BODY_X((i)); \
    (f)  = X[(i)&0x0f] = (u)((f)); \
    (f) += (e) + K_60_79 + RTL_DIGEST_ROTL((a), 5) + F_60_79((b), (c), (d)); \
    (b)  = RTL_DIGEST_ROTL((b), 30);

/*
 * __rtl_digest_initSHA.
 */
static void __rtl_digest_initSHA(
    DigestContextSHA *ctx, DigestSHA_update_t *fct)
{
    rtl_zeroMemory(ctx, sizeof(DigestContextSHA));
    ctx->m_update = fct;

    ctx->m_nA = (sal_uInt32)0x67452301L;
    ctx->m_nB = (sal_uInt32)0xefcdab89L;
    ctx->m_nC = (sal_uInt32)0x98badcfeL;
    ctx->m_nD = (sal_uInt32)0x10325476L;
    ctx->m_nE = (sal_uInt32)0xc3d2e1f0L;
}

/*
 * __rtl_digest_updateSHA.
 */
static void __rtl_digest_updateSHA(DigestContextSHA *ctx)
{
    register sal_uInt32  A, B, C, D, E, T;
    register sal_uInt32 *X;

    register DigestSHA_update_t *U;
    U = ctx->m_update;

    A = ctx->m_nA;
    B = ctx->m_nB;
    C = ctx->m_nC;
    D = ctx->m_nD;
    E = ctx->m_nE;
    X = ctx->m_pData;

    BODY_00_15(U,  0, A, B, C, D, E, T);
    BODY_00_15(U,  1, T, A, B, C, D, E);
    BODY_00_15(U,  2, E, T, A, B, C, D);
    BODY_00_15(U,  3, D, E, T, A, B, C);
    BODY_00_15(U,  4, C, D, E, T, A, B);
    BODY_00_15(U,  5, B, C, D, E, T, A);
    BODY_00_15(U,  6, A, B, C, D, E, T);
    BODY_00_15(U,  7, T, A, B, C, D, E);
    BODY_00_15(U,  8, E, T, A, B, C, D);
    BODY_00_15(U,  9, D, E, T, A, B, C);
    BODY_00_15(U, 10, C, D, E, T, A, B);
    BODY_00_15(U, 11, B, C, D, E, T, A);
    BODY_00_15(U, 12, A, B, C, D, E, T);
    BODY_00_15(U, 13, T, A, B, C, D, E);
    BODY_00_15(U, 14, E, T, A, B, C, D);
    BODY_00_15(U, 15, D, E, T, A, B, C);
    BODY_16_19(U, 16, C, D, E, T, A, B);
    BODY_16_19(U, 17, B, C, D, E, T, A);
    BODY_16_19(U, 18, A, B, C, D, E, T);
    BODY_16_19(U, 19, T, A, B, C, D, E);

    BODY_20_39(U, 20, E, T, A, B, C, D);
    BODY_20_39(U, 21, D, E, T, A, B, C);
    BODY_20_39(U, 22, C, D, E, T, A, B);
    BODY_20_39(U, 23, B, C, D, E, T, A);
    BODY_20_39(U, 24, A, B, C, D, E, T);
    BODY_20_39(U, 25, T, A, B, C, D, E);
    BODY_20_39(U, 26, E, T, A, B, C, D);
    BODY_20_39(U, 27, D, E, T, A, B, C);
    BODY_20_39(U, 28, C, D, E, T, A, B);
    BODY_20_39(U, 29, B, C, D, E, T, A);
    BODY_20_39(U, 30, A, B, C, D, E, T);
    BODY_20_39(U, 31, T, A, B, C, D, E);
    BODY_20_39(U, 32, E, T, A, B, C, D);
    BODY_20_39(U, 33, D, E, T, A, B, C);
    BODY_20_39(U, 34, C, D, E, T, A, B);
    BODY_20_39(U, 35, B, C, D, E, T, A);
    BODY_20_39(U, 36, A, B, C, D, E, T);
    BODY_20_39(U, 37, T, A, B, C, D, E);
    BODY_20_39(U, 38, E, T, A, B, C, D);
    BODY_20_39(U, 39, D, E, T, A, B, C);

    BODY_40_59(U, 40, C, D, E, T, A, B);
    BODY_40_59(U, 41, B, C, D, E, T, A);
    BODY_40_59(U, 42, A, B, C, D, E, T);
    BODY_40_59(U, 43, T, A, B, C, D, E);
    BODY_40_59(U, 44, E, T, A, B, C, D);
    BODY_40_59(U, 45, D, E, T, A, B, C);
    BODY_40_59(U, 46, C, D, E, T, A, B);
    BODY_40_59(U, 47, B, C, D, E, T, A);
    BODY_40_59(U, 48, A, B, C, D, E, T);
    BODY_40_59(U, 49, T, A, B, C, D, E);
    BODY_40_59(U, 50, E, T, A, B, C, D);
    BODY_40_59(U, 51, D, E, T, A, B, C);
    BODY_40_59(U, 52, C, D, E, T, A, B);
    BODY_40_59(U, 53, B, C, D, E, T, A);
    BODY_40_59(U, 54, A, B, C, D, E, T);
    BODY_40_59(U, 55, T, A, B, C, D, E);
    BODY_40_59(U, 56, E, T, A, B, C, D);
    BODY_40_59(U, 57, D, E, T, A, B, C);
    BODY_40_59(U, 58, C, D, E, T, A, B);
    BODY_40_59(U, 59, B, C, D, E, T, A);

    BODY_60_79(U, 60, A, B, C, D, E, T);
    BODY_60_79(U, 61, T, A, B, C, D, E);
    BODY_60_79(U, 62, E, T, A, B, C, D);
    BODY_60_79(U, 63, D, E, T, A, B, C);
    BODY_60_79(U, 64, C, D, E, T, A, B);
    BODY_60_79(U, 65, B, C, D, E, T, A);
    BODY_60_79(U, 66, A, B, C, D, E, T);
    BODY_60_79(U, 67, T, A, B, C, D, E);
    BODY_60_79(U, 68, E, T, A, B, C, D);
    BODY_60_79(U, 69, D, E, T, A, B, C);
    BODY_60_79(U, 70, C, D, E, T, A, B);
    BODY_60_79(U, 71, B, C, D, E, T, A);
    BODY_60_79(U, 72, A, B, C, D, E, T);
    BODY_60_79(U, 73, T, A, B, C, D, E);
    BODY_60_79(U, 74, E, T, A, B, C, D);
    BODY_60_79(U, 75, D, E, T, A, B, C);
    BODY_60_79(U, 76, C, D, E, T, A, B);
    BODY_60_79(U, 77, B, C, D, E, T, A);
    BODY_60_79(U, 78, A, B, C, D, E, T);
    BODY_60_79(U, 79, T, A, B, C, D, E);

    ctx->m_nA += E;
    ctx->m_nB += T;
    ctx->m_nC += A;
    ctx->m_nD += B;
    ctx->m_nE += C;
}

/*
 * __rtl_digest_endSHA.
 */
static void __rtl_digest_endSHA(DigestContextSHA *ctx)
{
    static const sal_uInt8 end[4] = {
        0x80, 0x00, 0x00, 0x00
    };
    register const sal_uInt8 *p = end;

    register sal_uInt32 *X;
    register int         i;

    X = ctx->m_pData;
    i = (ctx->m_nDatLen >> 2);

#ifdef WORDS_BIGENDIAN
    __rtl_digest_swapLong(X, i + 1);
#endif

    switch (ctx->m_nDatLen & 0x03) {
    case 1: X[i] &= 0x000000ff; break;
    case 2: X[i] &= 0x0000ffff; break;
    case 3: X[i] &= 0x00ffffff; break;
    }

    switch (ctx->m_nDatLen & 0x03) {
    case 0: X[i]  = ((sal_uInt32)(*(p++))) <<  0L;
    case 1: X[i] |= ((sal_uInt32)(*(p++))) <<  8L;
    case 2: X[i] |= ((sal_uInt32)(*(p++))) << 16L;
    case 3: X[i] |= ((sal_uInt32)(*(p++))) << 24L;
    }

    __rtl_digest_swapLong(X, i + 1);

    i += 1;

    if (i >= (DIGEST_LBLOCK_SHA - 2)) {
        for (; i < DIGEST_LBLOCK_SHA; i++)
            X[i] = 0;
        __rtl_digest_updateSHA(ctx);
        i = 0;
    }

    for (; i < (DIGEST_LBLOCK_SHA - 2); i++)
        X[i] = 0;

    X[DIGEST_LBLOCK_SHA - 2] = ctx->m_nH;
    X[DIGEST_LBLOCK_SHA - 1] = ctx->m_nL;

    __rtl_digest_updateSHA(ctx);
}

/*========================================================================
 *
 * rtl_digest_SHA1 internals.
 *
 *======================================================================*/
/*
 * __rtl_digest_SHA_1.
 */
static const Digest_Impl __rtl_digest_SHA_1 = { rtl_Digest_AlgorithmSHA1,
        RTL_DIGEST_LENGTH_SHA1,
        0,
        rtl_digest_destroySHA1,
        rtl_digest_updateSHA1,
        rtl_digest_getSHA1
                                              };

/*
 * __rtl_digest_updateSHA_1.
 */
static sal_uInt32 __rtl_digest_updateSHA_1(sal_uInt32 x)
{
    return RTL_DIGEST_ROTL(x, 1);
}

/*========================================================================
 *
 * rtl_digest_SHA1 implementation.
 *
 *======================================================================*/
/*
 * rtl_digest_SHA1.
 */
rtlDigestError rtl_digest_SHA1(
    const void *pData,   sal_uInt32 nDatLen,
    sal_uInt8  *pBuffer, sal_uInt32 nBufLen)
{
    DigestSHA_Impl digest;
    rtlDigestError result;

    digest.m_digest = __rtl_digest_SHA_1;
    __rtl_digest_initSHA(&(digest.m_context), __rtl_digest_updateSHA_1);

    result = rtl_digest_updateSHA1(&digest, pData, nDatLen);
    if (result == rtl_Digest_E_None)
        result = rtl_digest_getSHA1(&digest, pBuffer, nBufLen);

    rtl_zeroMemory(&digest, sizeof(digest));
    return (result);
}

/*
 * rtl_digest_createSHA1.
 */
rtlDigest rtl_digest_createSHA1(void)
{
    DigestSHA_Impl *pImpl = (DigestSHA_Impl*)0;
    pImpl = RTL_DIGEST_CREATE(DigestSHA_Impl);
    if (pImpl) {
        pImpl->m_digest = __rtl_digest_SHA_1;
        __rtl_digest_initSHA(&(pImpl->m_context), __rtl_digest_updateSHA_1);
    }
    return ((rtlDigest)pImpl);
}

/*
 * rtl_digest_updateSHA1.
 */
rtlDigestError rtl_digest_updateSHA1(
    rtlDigest Digest, const void *pData, sal_uInt32 nDatLen)
{
    DigestSHA_Impl   *pImpl = (DigestSHA_Impl *)Digest;
    const sal_uInt8  *d     = (const sal_uInt8 *)pData;

    DigestContextSHA *ctx;
    sal_uInt32        len;

    if ((pImpl == 0) || (pData == 0))
        return rtl_Digest_E_Argument;

    if (!(pImpl->m_digest.m_algorithm == rtl_Digest_AlgorithmSHA1))
        return rtl_Digest_E_Algorithm;

    if (nDatLen == 0)
        return rtl_Digest_E_None;

    ctx = &(pImpl->m_context);

    len = ctx->m_nL + (nDatLen << 3);
    if (len < ctx->m_nL) ctx->m_nH += 1;
    ctx->m_nH += (nDatLen >> 29);
    ctx->m_nL  = len;

    if (ctx->m_nDatLen) {
        sal_uInt8  *p = (sal_uInt8 *)(ctx->m_pData) + ctx->m_nDatLen;
        sal_uInt32  n = DIGEST_CBLOCK_SHA - ctx->m_nDatLen;

        if (nDatLen < n) {
            rtl_copyMemory(p, d, nDatLen);
            ctx->m_nDatLen += nDatLen;

            return rtl_Digest_E_None;
        }

        rtl_copyMemory(p, d, n);
        d       += n;
        nDatLen -= n;

#ifndef WORDS_BIGENDIAN
        __rtl_digest_swapLong(ctx->m_pData, DIGEST_LBLOCK_SHA);
#endif

        __rtl_digest_updateSHA(ctx);
        ctx->m_nDatLen = 0;
    }

    while (nDatLen >= DIGEST_CBLOCK_SHA) {
        rtl_copyMemory(ctx->m_pData, d, DIGEST_CBLOCK_SHA);
        d       += DIGEST_CBLOCK_SHA;
        nDatLen -= DIGEST_CBLOCK_SHA;

#ifndef WORDS_BIGENDIAN
        __rtl_digest_swapLong(ctx->m_pData, DIGEST_LBLOCK_SHA);
#endif

        __rtl_digest_updateSHA(ctx);
    }

    rtl_copyMemory(ctx->m_pData, d, nDatLen);
    ctx->m_nDatLen = nDatLen;

    return rtl_Digest_E_None;
}

/*
 * rtl_digest_getSHA1.
 */
rtlDigestError rtl_digest_getSHA1(
    rtlDigest Digest, sal_uInt8 *pBuffer, sal_uInt32 nBufLen)
{
    DigestSHA_Impl   *pImpl = (DigestSHA_Impl *)Digest;
    sal_uInt8        *p     = pBuffer;

    DigestContextSHA *ctx;

    if ((pImpl == 0) || (pBuffer == 0))
        return rtl_Digest_E_Argument;

    if (!(pImpl->m_digest.m_algorithm == rtl_Digest_AlgorithmSHA1))
        return rtl_Digest_E_Algorithm;

    if (!(pImpl->m_digest.m_length <= nBufLen))
        return rtl_Digest_E_BufferSize;

    ctx = &(pImpl->m_context);

    __rtl_digest_endSHA(ctx);
    RTL_DIGEST_HTONL(ctx->m_nA, p);
    RTL_DIGEST_HTONL(ctx->m_nB, p);
    RTL_DIGEST_HTONL(ctx->m_nC, p);
    RTL_DIGEST_HTONL(ctx->m_nD, p);
    RTL_DIGEST_HTONL(ctx->m_nE, p);
    __rtl_digest_initSHA(ctx, __rtl_digest_updateSHA_1);

    return rtl_Digest_E_None;
}

/*
 * rtl_digest_destroySHA1.
 */
void rtl_digest_destroySHA1(rtlDigest Digest)
{
    DigestSHA_Impl *pImpl = (DigestSHA_Impl *)Digest;
    if (pImpl) {
        if (pImpl->m_digest.m_algorithm == rtl_Digest_AlgorithmSHA1)
            rtl_freeZeroMemory(pImpl, sizeof(DigestSHA_Impl));
        else
            rtl_freeMemory(pImpl);
    }
}

/*========================================================================
 *
 * The End.
 *
 *======================================================================*/

bool SHA1::getHash(QString const & text, QByteArray & hash)
{
    rtlDigest aDigest     = rtl_digest_createSHA1();
    rtlDigestError aError = rtl_digest_updateSHA1(aDigest, text.unicode(), text.length() * sizeof(QChar));

    if (aError == rtl_Digest_E_None) {
        QByteArray digest;
        digest.resize(RTL_DIGEST_LENGTH_SHA1 + 1);
        digest.fill('\0', RTL_DIGEST_LENGTH_SHA1);

        aError = rtl_digest_getSHA1(aDigest, (unsigned char *) digest.data(), RTL_DIGEST_LENGTH_SHA1);

        if (aError != rtl_Digest_E_None)
            return false;

        hash = digest;

        return true;
    }

    return false;
}
