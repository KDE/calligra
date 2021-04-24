/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRFANWIPEEFFECTFACTORY_H
#define KPRFANWIPEEFFECTFACTORY_H

#include <KPrPageEffectFactory.h>

class KPrFanWipeEffectFactory : public KPrPageEffectFactory
{
public:
    KPrFanWipeEffectFactory();
    ~KPrFanWipeEffectFactory() override;
    QString subTypeName(int subType) const override;

    enum SubType {
        CenterRight,
        CenterTop,
        CenterLeft,
        CenterBottom,
        FanOutTop,
        FanOutRight,
        FanOutBottom,
        FanOutLeft,
        FanInTop,
        FanInRight,
        FanInBottom,
        FanInLeft
    };
};

#endif /* KPRFANWIPEEFFECTFACTORY_H */
