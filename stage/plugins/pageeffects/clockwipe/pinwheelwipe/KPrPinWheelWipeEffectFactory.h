/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRPINWHEELWIPEEFFECTFACTORY_H
#define KPRPINWHEELWIPEEFFECTFACTORY_H

#include <KPrPageEffectFactory.h>

class KPrPinWheelWipeEffectFactory : public KPrPageEffectFactory
{
public:
    KPrPinWheelWipeEffectFactory();
    ~KPrPinWheelWipeEffectFactory() override;
    QString subTypeName(int subType) const override;

    enum SubType {
        TwoBladeVerticalClockwise,
        TwoBladeHorizontalClockwise,
        FourBladeClockwise,
        TwoBladeVerticalCounterClockwise,
        TwoBladeHorizontalCounterClockwise,
        FourBladeCounterClockwise
    };
};

#endif /* KPRPINWHEELWIPEEFFECTFACTORY_H */
