/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRDOUBLESWEEPWIPEEFFECTFACTORY_H
#define KPRDOUBLESWEEPWIPEEFFECTFACTORY_H

#include <KPrPageEffectFactory.h>

class KPrDoubleSweepWipeEffectFactory : public KPrPageEffectFactory
{
public:
    KPrDoubleSweepWipeEffectFactory();
    ~KPrDoubleSweepWipeEffectFactory() override;
    QString subTypeName(int subType) const override;

    enum SubType {
        ParallelVertical,
        ParallelDiagonal,
        OppositeVertical,
        OppositeHorizontal,
        ParallelDiagonalTopLeft,
        ParallelDiagonalBottomLeft,
        ParallelVerticalReverse,
        ParallelDiagonalReverse,
        OppositeVerticalReverse,
        OppositeHorizontalReverse,
        ParallelDiagonalTopLeftReverse,
        ParallelDiagonalBottomLeftReverse
    };
};

#endif /* KPRDOUBLESWEEPWIPEEFFECTFACTORY_H */
