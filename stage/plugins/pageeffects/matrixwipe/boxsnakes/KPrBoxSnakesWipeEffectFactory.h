/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRBOXSNAKESWIPEEFFECTFACTORY_H
#define KPRBOXSNAKESWIPEEFFECTFACTORY_H

#include <KPrPageEffectFactory.h>

class KPrBoxSnakesWipeEffectFactory : public KPrPageEffectFactory
{
public:
    KPrBoxSnakesWipeEffectFactory();
    ~KPrBoxSnakesWipeEffectFactory() override;
    QString subTypeName(int subType) const override;

    enum SubType {
        TwoBoxTopIn,
        TwoBoxBottomIn,
        TwoBoxLeftIn,
        TwoBoxRightIn,
        FourBoxVerticalIn,
        FourBoxHorizontalIn,
        TwoBoxTopOut,
        TwoBoxBottomOut,
        TwoBoxLeftOut,
        TwoBoxRightOut,
        FourBoxVerticalOut,
        FourBoxHorizontalOut
    };
};

#endif /* KPRBOXSNAKESWIPEEFFECTFACTORY_H */
