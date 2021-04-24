/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRSPIRALWIPEEFFECTFACTORY_H
#define KPRSPIRALWIPEEFFECTFACTORY_H

#include <KPrPageEffectFactory.h>

class KPrSpiralWipeEffectFactory : public KPrPageEffectFactory
{
public:
    KPrSpiralWipeEffectFactory();
    ~KPrSpiralWipeEffectFactory() override;
    QString subTypeName(int subType) const override;

    enum SubType {
        ClockwiseTopLeftIn,
        ClockwiseTopLeftOut,
        ClockwiseTopRightIn,
        ClockwiseTopRightOut,
        ClockwiseBottomLeftIn,
        ClockwiseBottomLeftOut,
        ClockwiseBottomRightIn,
        ClockwiseBottomRightOut,
        CounterClockwiseTopLeftIn,
        CounterClockwiseTopLeftOut,
        CounterClockwiseTopRightIn,
        CounterClockwiseTopRightOut,
        CounterClockwiseBottomLeftIn,
        CounterClockwiseBottomLeftOut,
        CounterClockwiseBottomRightIn,
        CounterClockwiseBottomRightOut
    };
};

#endif /* KPRSPIRALWIPEEFFECTFACTORY_H */
