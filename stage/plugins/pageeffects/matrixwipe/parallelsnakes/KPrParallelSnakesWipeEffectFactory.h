/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRPARALLELSNAKESWIPEEFFECTFACTORY_H
#define KPRPARALLELSNAKESWIPEEFFECTFACTORY_H

#include <KPrPageEffectFactory.h>

class KPrParallelSnakesWipeEffectFactory : public KPrPageEffectFactory
{
public:
    KPrParallelSnakesWipeEffectFactory();
    ~KPrParallelSnakesWipeEffectFactory() override;
    QString subTypeName(int subType) const override;

    enum SubType {
        VerticalTopSameIn,
        VerticalTopSameOut,
        VerticalBottomSameIn,
        VerticalBottomSameOut,
        VerticalTopLeftOppositeIn,
        VerticalTopLeftOppositeOut,
        VerticalBottomLeftOppositeIn,
        VerticalBottomLeftOppositeOut,
        HorizontalLeftSameIn,
        HorizontalLeftSameOut,
        HorizontalRightSameIn,
        HorizontalRightSameOut,
        HorizontalTopLeftOppositeIn,
        HorizontalTopLeftOppositeOut,
        HorizontalTopRightOppositeIn,
        HorizontalTopRightOppositeOut,
        DiagonalBottomLeftOppositeIn,
        DiagonalBottomLeftOppositeOut,
        DiagonalTopLeftOppositeIn,
        DiagonalTopLeftOppositeOut
    };
};

#endif /* KPRPARALLELSNAKESWIPEEFFECTFACTORY_H */
