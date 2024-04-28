/*
 *  SPDX-FileCopyrightText: 2012 Dmitry Kazakov <dimula73@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "KoOptimizedCompositeOpFactoryPerArch.h"

#include "KoColorSpaceTraits.h"
#include "KoCompositeOpAlphaDarken.h"
#include "KoCompositeOpOver.h"

template<>
template<>
KoOptimizedCompositeOpFactoryPerArch<KoOptimizedCompositeOpAlphaDarken32>::ReturnType
KoOptimizedCompositeOpFactoryPerArch<KoOptimizedCompositeOpAlphaDarken32>::create<Vc::ScalarImpl>(ParamType param)
{
    return new KoCompositeOpAlphaDarken<KoBgrU8Traits>(param);
}

template<>
template<>
KoOptimizedCompositeOpFactoryPerArch<KoOptimizedCompositeOpOver32>::ReturnType
KoOptimizedCompositeOpFactoryPerArch<KoOptimizedCompositeOpOver32>::create<Vc::ScalarImpl>(ParamType param)
{
    return new KoCompositeOpOver<KoBgrU8Traits>(param);
}

template<>
template<>
KoOptimizedCompositeOpFactoryPerArch<KoOptimizedCompositeOpAlphaDarken128>::ReturnType
KoOptimizedCompositeOpFactoryPerArch<KoOptimizedCompositeOpAlphaDarken128>::create<Vc::ScalarImpl>(ParamType param)
{
    return new KoCompositeOpAlphaDarken<KoRgbF32Traits>(param);
}

template<>
template<>
KoOptimizedCompositeOpFactoryPerArch<KoOptimizedCompositeOpOver128>::ReturnType
KoOptimizedCompositeOpFactoryPerArch<KoOptimizedCompositeOpOver128>::create<Vc::ScalarImpl>(ParamType param)
{
    return new KoCompositeOpOver<KoRgbF32Traits>(param);
}

template<>
KoReportCurrentArch::ReturnType KoReportCurrentArch::create<Vc::ScalarImpl>(ParamType)
{
    //     dbgPigment << "Legacy integer arithmetics implementation";
}
