/*
 *  SPDX-FileCopyrightText: 2012 Dmitry Kazakov <dimula73@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef KOOPTIMIZEDCOMPOSITEOPFACTORYPERARCH_H
#define KOOPTIMIZEDCOMPOSITEOPFACTORYPERARCH_H

#include "KoVcMultiArchBuildSupport.h"

class KoCompositeOp;
class KoColorSpace;

template<Vc::Implementation _impl>
class KoOptimizedCompositeOpAlphaDarken32;

template<Vc::Implementation _impl>
class KoOptimizedCompositeOpOver32;

template<Vc::Implementation _impl>
class KoOptimizedCompositeOpAlphaDarken128;

template<Vc::Implementation _impl>
class KoOptimizedCompositeOpOver128;

template<template<Vc::Implementation I> class CompositeOp>
struct KoOptimizedCompositeOpFactoryPerArch {
    typedef const KoColorSpace *ParamType;
    typedef KoCompositeOp *ReturnType;

    template<Vc::Implementation _impl>
    static ReturnType create(ParamType param);
};

struct KoReportCurrentArch {
    typedef void *ParamType;
    typedef void ReturnType;

    template<Vc::Implementation _impl>
    static ReturnType create(ParamType);
};

#endif /* KOOPTIMIZEDCOMPOSITEOPFACTORYPERARCH_H */
