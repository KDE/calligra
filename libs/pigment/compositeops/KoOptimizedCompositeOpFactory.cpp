/*
 *  SPDX-FileCopyrightText: 2012 Dmitry Kazakov <dimula73@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "KoOptimizedCompositeOpFactoryPerArch.h" // vc.h must come first
#include "KoOptimizedCompositeOpFactory.h"

#if defined(__clang__)
#pragma GCC diagnostic ignored "-Wundef"
#endif


static struct ArchReporter {
    ArchReporter() {
        createOptimizedClass<KoReportCurrentArch>(0);
    }
} StaticReporter;


KoCompositeOp* KoOptimizedCompositeOpFactory::createAlphaDarkenOp32(const KoColorSpace *cs)
{
    return createOptimizedClass<KoOptimizedCompositeOpFactoryPerArch<KoOptimizedCompositeOpAlphaDarken32> >(cs);
}

KoCompositeOp* KoOptimizedCompositeOpFactory::createOverOp32(const KoColorSpace *cs)
{
    return createOptimizedClass<KoOptimizedCompositeOpFactoryPerArch<KoOptimizedCompositeOpOver32> >(cs);
}

KoCompositeOp* KoOptimizedCompositeOpFactory::createAlphaDarkenOp128(const KoColorSpace *cs)
{
    return createOptimizedClass<KoOptimizedCompositeOpFactoryPerArch<KoOptimizedCompositeOpAlphaDarken128> >(cs);
}

KoCompositeOp* KoOptimizedCompositeOpFactory::createOverOp128(const KoColorSpace *cs)
{
    return createOptimizedClass<KoOptimizedCompositeOpFactoryPerArch<KoOptimizedCompositeOpOver128> >(cs);
}
