/*
 *  SPDX-FileCopyrightText: 2012 Dmitry Kazakov <dimula73@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef __KOVCMULTIARCHBUILDSUPPORT_H
#define __KOVCMULTIARCHBUILDSUPPORT_H

#include "config-vc.h"

#ifdef HAVE_VC

#if defined(__clang__)

#pragma GCC diagnostic ignored "-Wlocal-type-template-args"
#endif

#if defined _MSC_VER
// Lets shut up the "possible loss of data" and "forcing value to bool 'true' or 'false'
#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4800)
#endif
#include <Vc/Vc>
#include <Vc/global.h>
#include <Vc/support.h>
#if defined _MSC_VER
#pragma warning(pop)
#endif

#else /* HAVE_VC */

namespace Vc
{
enum Implementation /*: std::uint_least32_t*/ {
    ScalarImpl,
};
class CurrentImplementation
{
public:
    static constexpr Implementation current()
    {
        return static_cast<Implementation>(ScalarImpl);
    }
};
}

#ifdef DO_PACKAGERS_BUILD
#ifdef __GNUC__
#warning "Packagers build is not available without the presence of Vc library. Disabling."
#endif
#undef DO_PACKAGERS_BUILD
#endif

#endif /* HAVE_VC */

#ifdef DO_PACKAGERS_BUILD

template<class FactoryType>
typename FactoryType::ReturnType createOptimizedClass(typename FactoryType::ParamType param)
{
    /**
     * We use SSE2, SSSE3, SSE4.1, AVX and AVX2.
     * The rest are integer and string instructions mostly.
     *
     * TODO: Add FMA3/4 when it is adopted by Vc
     */
    if (Vc::isImplementationSupported(Vc::AVX2Impl)) {
        return FactoryType::template create<Vc::AVX2Impl>(param);
    } else if (Vc::isImplementationSupported(Vc::AVXImpl)) {
        return FactoryType::template create<Vc::AVXImpl>(param);
    } else if (Vc::isImplementationSupported(Vc::SSE41Impl)) {
        return FactoryType::template create<Vc::SSE41Impl>(param);
    } else if (Vc::isImplementationSupported(Vc::SSSE3Impl)) {
        return FactoryType::template create<Vc::SSSE3Impl>(param);
    } else if (Vc::isImplementationSupported(Vc::SSE2Impl)) {
        return FactoryType::template create<Vc::SSE2Impl>(param);
    } else {
        return FactoryType::template create<Vc::ScalarImpl>(param);
    }
}

#else /* DO_PACKAGERS_BUILD */

/**
 * When doing not a packager's build we have one architecture only,
 * so the factory methods are simplified
 */

template<class FactoryType>
typename FactoryType::ReturnType createOptimizedClass(typename FactoryType::ParamType param)
{
    return FactoryType::template create<Vc::CurrentImplementation::current()>(param);
}

#endif /* DO_PACKAGERS_BUILD */

#endif /* __KOVCMULTIARCHBUILDSUPPORT_H */
