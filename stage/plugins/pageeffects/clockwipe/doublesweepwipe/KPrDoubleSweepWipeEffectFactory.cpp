/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrDoubleSweepWipeEffectFactory.h"

#include <KLazyLocalizedString>

#include "KPrDoubleSweepWipeStrategy.h"

#define DoubleSweepWipeEffectId "DoubleSweepWipeEffect"

KPrDoubleSweepWipeEffectFactory::KPrDoubleSweepWipeEffectFactory()
    : KPrPageEffectFactory(DoubleSweepWipeEffectId, i18n("Double Sweep"))
{
    addStrategy(new KPrDoubleSweepWipeStrategy(ParallelVertical, "doubleSweepWipe", "parallelVertical", false));
    addStrategy(new KPrDoubleSweepWipeStrategy(ParallelDiagonal, "doubleSweepWipe", "parallelDiagonal", false));
    addStrategy(new KPrDoubleSweepWipeStrategy(OppositeVertical, "doubleSweepWipe", "oppositeVertical", false));
    addStrategy(new KPrDoubleSweepWipeStrategy(OppositeHorizontal, "doubleSweepWipe", "oppositeHorizontal", false));
    addStrategy(new KPrDoubleSweepWipeStrategy(ParallelDiagonalTopLeft, "doubleSweepWipe", "parallelDiagonalTopLeft", false));
    addStrategy(new KPrDoubleSweepWipeStrategy(ParallelDiagonalBottomLeft, "doubleSweepWipe", "parallelDiagonalBottomLeft", false));
    addStrategy(new KPrDoubleSweepWipeStrategy(ParallelVerticalReverse, "doubleSweepWipe", "parallelVertical", true));
    addStrategy(new KPrDoubleSweepWipeStrategy(ParallelDiagonalReverse, "doubleSweepWipe", "parallelDiagonal", true));
    addStrategy(new KPrDoubleSweepWipeStrategy(OppositeVerticalReverse, "doubleSweepWipe", "oppositeVertical", true));
    addStrategy(new KPrDoubleSweepWipeStrategy(OppositeHorizontalReverse, "doubleSweepWipe", "oppositeHorizontal", true));
    addStrategy(new KPrDoubleSweepWipeStrategy(ParallelDiagonalTopLeftReverse, "doubleSweepWipe", "parallelDiagonalTopLeft", true));
    addStrategy(new KPrDoubleSweepWipeStrategy(ParallelDiagonalBottomLeftReverse, "doubleSweepWipe", "parallelDiagonalBottomLeft", true));
}

KPrDoubleSweepWipeEffectFactory::~KPrDoubleSweepWipeEffectFactory() = default;

static const KLazyLocalizedString s_subTypes[] = {kli18n("Parallel Vertical"),
                                                  kli18n("Parallel Diagonal"),
                                                  kli18n("Opposite Vertical"),
                                                  kli18n("Opposite Horizontal"),
                                                  kli18n("Parallel Diagonal Top Left"),
                                                  kli18n("Parallel Diagonal Bottom Left"),
                                                  kli18n("Parallel Vertical Reverse"),
                                                  kli18n("Parallel Diagonal Reverse"),
                                                  kli18n("Opposite Vertical Reverse"),
                                                  kli18n("Opposite Horizontal Reverse"),
                                                  kli18n("Parallel Diagonal Top Left Reverse"),
                                                  kli18n("Parallel Diagonal Bottom Left Reverse")};

QString KPrDoubleSweepWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return s_subTypes[subType].toString();
    } else {
        return i18n("Unknown subtype");
    }
}
