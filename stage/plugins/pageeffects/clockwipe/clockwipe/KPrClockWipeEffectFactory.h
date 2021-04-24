/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRCLOCKWIPEEFFECTFACTORY_H
#define KPRCLOCKWIPEEFFECTFACTORY_H

#include <KPrPageEffectFactory.h>

class KPrClockWipeEffectFactory : public KPrPageEffectFactory
{
public:
    KPrClockWipeEffectFactory();
    ~KPrClockWipeEffectFactory() override;
    QString subTypeName(int subType) const override;

    enum SubType {
        FromTwelveClockwise,
        FromThreeClockwise,
        FromSixClockwise,
        FromNineClockwise,
        FromTwelveCounterClockwise,
        FromThreeCounterClockwise,
        FromSixCounterClockwise,
        FromNineCounterClockwise
    };
};

#endif /* KPRCLOCKWIPEEFFECTFACTORY_H */
