/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrParallelSnakesWipeEffectFactory.h"

#include <KLazyLocalizedString>

#include "KPrParallelSnakesWipeDiagonalStrategy.h"
#include "KPrParallelSnakesWipeHorizontalStrategy.h"
#include "KPrParallelSnakesWipeVerticalStrategy.h"

#define ParallelSnakesWipeEffectId "ParallelSnakesWipeEffect"

KPrParallelSnakesWipeEffectFactory::KPrParallelSnakesWipeEffectFactory()
    : KPrPageEffectFactory(ParallelSnakesWipeEffectId, i18n("Parallel Snakes"))
{
    addStrategy(new KPrParallelSnakesWipeVerticalStrategy(false, false, false));
    addStrategy(new KPrParallelSnakesWipeVerticalStrategy(true, true, false));
    addStrategy(new KPrParallelSnakesWipeVerticalStrategy(false, true, false));
    addStrategy(new KPrParallelSnakesWipeVerticalStrategy(true, false, false));
    addStrategy(new KPrParallelSnakesWipeVerticalStrategy(false, false, true));
    addStrategy(new KPrParallelSnakesWipeVerticalStrategy(true, true, true));
    addStrategy(new KPrParallelSnakesWipeVerticalStrategy(false, true, true));
    addStrategy(new KPrParallelSnakesWipeVerticalStrategy(true, false, true));
    addStrategy(new KPrParallelSnakesWipeHorizontalStrategy(false, false, false));
    addStrategy(new KPrParallelSnakesWipeHorizontalStrategy(true, true, false));
    addStrategy(new KPrParallelSnakesWipeHorizontalStrategy(false, true, false));
    addStrategy(new KPrParallelSnakesWipeHorizontalStrategy(true, false, false));
    addStrategy(new KPrParallelSnakesWipeHorizontalStrategy(false, false, true));
    addStrategy(new KPrParallelSnakesWipeHorizontalStrategy(true, true, true));
    addStrategy(new KPrParallelSnakesWipeHorizontalStrategy(false, true, true));
    addStrategy(new KPrParallelSnakesWipeHorizontalStrategy(true, false, true));
    addStrategy(new KPrParallelSnakesWipeDiagonalStrategy(false, true));
    addStrategy(new KPrParallelSnakesWipeDiagonalStrategy(true, true));
    addStrategy(new KPrParallelSnakesWipeDiagonalStrategy(false, false));
    addStrategy(new KPrParallelSnakesWipeDiagonalStrategy(true, false));
}

KPrParallelSnakesWipeEffectFactory::~KPrParallelSnakesWipeEffectFactory() = default;

static const KLazyLocalizedString s_subTypes[] = {kli18n("Vertical Top Same In"),
                                                  kli18n("Vertical Top Same Out"),
                                                  kli18n("Vertical Bottom Same In"),
                                                  kli18n("Vertical Bottom Same Out"),
                                                  kli18n("Vertical Top Left Opposite In"),
                                                  kli18n("Vertical Top Left Opposite Out"),
                                                  kli18n("Vertical Bottom Left Opposite In"),
                                                  kli18n("Vertical Bottom Left Opposite Out"),
                                                  kli18n("Horizontal Left Same In"),
                                                  kli18n("Horizontal Left Same Out"),
                                                  kli18n("Horizontal Right Same In"),
                                                  kli18n("Horizontal Right Same Out"),
                                                  kli18n("Horizontal Top Left Opposite In"),
                                                  kli18n("Horizontal Top Left Opposite Out"),
                                                  kli18n("Horizontal Top Right Opposite In"),
                                                  kli18n("Horizontal Top Right Opposite Out"),
                                                  kli18n("Diagonal Bottom Left Opposite In"),
                                                  kli18n("Diagonal Bottom Left Opposite Out"),
                                                  kli18n("Diagonal Top Left Opposite In"),
                                                  kli18n("Diagonal Top Left Opposite Out")};

QString KPrParallelSnakesWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return s_subTypes[subType].toString();
    } else {
        return i18n("Unknown subtype");
    }
}
