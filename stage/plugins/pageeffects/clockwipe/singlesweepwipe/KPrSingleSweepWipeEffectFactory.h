/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRSINGLESWEEPWIPEEFFECTFACTORY_H
#define KPRSINGLESWEEPWIPEEFFECTFACTORY_H

#include <KPrPageEffectFactory.h>

class KPrSingleSweepWipeEffectFactory : public KPrPageEffectFactory
{
public:
    KPrSingleSweepWipeEffectFactory();
    ~KPrSingleSweepWipeEffectFactory() override;
    QString subTypeName(int subType) const override;

    enum SubType {
        ClockwiseTop,
        ClockwiseRight,
        ClockwiseBottom,
        ClockwiseLeft,
        ClockwiseTopLeft,
        CounterClockwiseBottomLeft,
        ClockwiseBottomRight,
        CounterClockwiseTopRight,
        CounterClockwiseTop,
        CounterClockwiseRight,
        CounterClockwiseBottom,
        CounterClockwiseLeft,
        CounterClockwiseTopLeft,
        ClockwiseBottomLeft,
        CounterClockwiseBottomRight,
        ClockwiseTopRight
    };
};

#endif /* KPRSINGLESWEEPWIPEEFFECTFACTORY_H */
