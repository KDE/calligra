/*
 *  SPDX-FileCopyrightText: 2012 Dmitry Kazakov <dimula73@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#if !defined _MSC_VER
#pragma GCC diagnostic ignored "-Wundef"
#endif

#include "KoOptimizedCompositeOpFactoryPerArch.h"
#include "KoOptimizedCompositeOpAlphaDarken128.h"
#include "KoOptimizedCompositeOpAlphaDarken32.h"
#include "KoOptimizedCompositeOpOver128.h"
#include "KoOptimizedCompositeOpOver32.h"

#include "DebugPigment.h"
#include <QString>

#include <KoCompositeOpRegistry.h>

#if defined(__clang__)
#pragma GCC diagnostic ignored "-Wlocal-type-template-args"
#endif

template<>
template<>
KoOptimizedCompositeOpFactoryPerArch<KoOptimizedCompositeOpAlphaDarken32>::ReturnType
KoOptimizedCompositeOpFactoryPerArch<KoOptimizedCompositeOpAlphaDarken32>::create<Vc::CurrentImplementation::current()>(ParamType param)
{
    return new KoOptimizedCompositeOpAlphaDarken32<Vc::CurrentImplementation::current()>(param);
}

template<>
template<>
KoOptimizedCompositeOpFactoryPerArch<KoOptimizedCompositeOpOver32>::ReturnType
KoOptimizedCompositeOpFactoryPerArch<KoOptimizedCompositeOpOver32>::create<Vc::CurrentImplementation::current()>(ParamType param)
{
    return new KoOptimizedCompositeOpOver32<Vc::CurrentImplementation::current()>(param);
}

template<>
template<>
KoOptimizedCompositeOpFactoryPerArch<KoOptimizedCompositeOpAlphaDarken128>::ReturnType
KoOptimizedCompositeOpFactoryPerArch<KoOptimizedCompositeOpAlphaDarken128>::create<Vc::CurrentImplementation::current()>(ParamType param)
{
    return new KoOptimizedCompositeOpAlphaDarken128<Vc::CurrentImplementation::current()>(param);
}

template<>
template<>
KoOptimizedCompositeOpFactoryPerArch<KoOptimizedCompositeOpOver128>::ReturnType
KoOptimizedCompositeOpFactoryPerArch<KoOptimizedCompositeOpOver128>::create<Vc::CurrentImplementation::current()>(ParamType param)
{
    return new KoOptimizedCompositeOpOver128<Vc::CurrentImplementation::current()>(param);
}

#define __stringify(_s) #_s
#define stringify(_s) __stringify(_s)

inline void printFeatureSupported(const QString &feature, Vc::Implementation impl)
{
    dbgPigment << "\t" << feature << "\t---\t" << (Vc::isImplementationSupported(impl) ? "yes" : "no");
}

template<>
KoReportCurrentArch::ReturnType KoReportCurrentArch::create<Vc::CurrentImplementation::current()>(ParamType)
{
    dbgPigment << "Compiled for arch:" << Vc::CurrentImplementation::current();
    dbgPigment << "Features supported:";
    printFeatureSupported("SSE2", Vc::SSE2Impl);
    printFeatureSupported("SSSE3", Vc::SSSE3Impl);
    printFeatureSupported("SSE4.1", Vc::SSE41Impl);
    printFeatureSupported("AVX ", Vc::AVXImpl);
    printFeatureSupported("AVX2 ", Vc::AVX2Impl);
}
