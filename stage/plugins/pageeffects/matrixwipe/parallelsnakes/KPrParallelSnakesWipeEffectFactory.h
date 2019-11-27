/* This file is part of the KDE project
   Copyright (C) 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
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
