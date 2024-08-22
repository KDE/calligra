/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrSingleSweepWipeEffectFactory.h"

#include <KLazyLocalizedString>
#include <KLocalizedString>

#include "KPrSingleSweepWipeStrategy.h"

#define SingleSweepWipeEffectId "SingleSweepWipeEffect"

KPrSingleSweepWipeEffectFactory::KPrSingleSweepWipeEffectFactory()
    : KPrPageEffectFactory(SingleSweepWipeEffectId, i18n("Single Sweep"))
{
    addStrategy(new KPrSingleSweepWipeStrategy(0.5, 0.0, 0, -180, ClockwiseTop, "singleSweepWipe", "clockwiseTop", false));
    addStrategy(new KPrSingleSweepWipeStrategy(1.0, 0.5, 270, -180, ClockwiseRight, "singleSweepWipe", "clockwiseRight", false));
    addStrategy(new KPrSingleSweepWipeStrategy(0.5, 1.0, 180, -180, ClockwiseBottom, "singleSweepWipe", "clockwiseBottom", false));
    addStrategy(new KPrSingleSweepWipeStrategy(0.0, 0.5, 90, -180, ClockwiseLeft, "singleSweepWipe", "clockwiseLeft", false));
    addStrategy(new KPrSingleSweepWipeStrategy(0.0, 0.0, 0, -90, ClockwiseTopLeft, "singleSweepWipe", "clockwiseTopLeft", false));
    addStrategy(new KPrSingleSweepWipeStrategy(0.0, 1.0, 0, 90, CounterClockwiseBottomLeft, "singleSweepWipe", "counterClockwiseBottomLeft", false));
    addStrategy(new KPrSingleSweepWipeStrategy(1.0, 1.0, 180, -90, ClockwiseBottomRight, "singleSweepWipe", "clockwiseBottomRight", false));
    addStrategy(new KPrSingleSweepWipeStrategy(1.0, 0.0, 180, 90, CounterClockwiseTopRight, "singleSweepWipe", "counterClockwiseTopRight", false));
    addStrategy(new KPrSingleSweepWipeStrategy(0.5, 0.0, 180, 180, CounterClockwiseTop, "singleSweepWipe", "clockwiseTop", true));
    addStrategy(new KPrSingleSweepWipeStrategy(1.0, 0.5, 90, 180, CounterClockwiseRight, "singleSweepWipe", "clockwiseRight", true));
    addStrategy(new KPrSingleSweepWipeStrategy(0.5, 1.0, 0, 180, CounterClockwiseBottom, "singleSweepWipe", "clockwiseBottom", true));
    addStrategy(new KPrSingleSweepWipeStrategy(0.0, 0.5, 270, 180, CounterClockwiseLeft, "singleSweepWipe", "clockwiseLeft", true));
    addStrategy(new KPrSingleSweepWipeStrategy(0.0, 0.0, 270, 90, CounterClockwiseTopLeft, "singleSweepWipe", "clockwiseTopLeft", true));
    addStrategy(new KPrSingleSweepWipeStrategy(0.0, 1.0, 90, -90, ClockwiseBottomLeft, "singleSweepWipe", "counterClockwiseBottomLeft", true));
    addStrategy(new KPrSingleSweepWipeStrategy(1.0, 1.0, 90, 90, CounterClockwiseBottomRight, "singleSweepWipe", "clockwiseBottomRight", true));
    addStrategy(new KPrSingleSweepWipeStrategy(1.0, 0.0, 270, -90, ClockwiseTopRight, "singleSweepWipe", "counterClockwiseTopRight", true));
}

KPrSingleSweepWipeEffectFactory::~KPrSingleSweepWipeEffectFactory() = default;

static const KLazyLocalizedString s_subTypes[] = {kli18n("Clockwise Top"),
                                                  kli18n("Clockwise Right"),
                                                  kli18n("Clockwise Bottom"),
                                                  kli18n("Clockwise Left"),
                                                  kli18n("Clockwise Top Left"),
                                                  kli18n("Counterclockwise Bottom Left"),
                                                  kli18n("Clockwise Bottom Right"),
                                                  kli18n("Counterclockwise Top Right"),
                                                  kli18n("Counterclockwise Top"),
                                                  kli18n("Counterclockwise Right"),
                                                  kli18n("Counterclockwise Bottom"),
                                                  kli18n("Counterclockwise Left"),
                                                  kli18n("Counterclockwise Top Left"),
                                                  kli18n("Clockwise Bottom Left"),
                                                  kli18n("Counterclockwise Bottom Right"),
                                                  kli18n("Clockwise Top Right")};

QString KPrSingleSweepWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return s_subTypes[subType].toString();
    } else {
        return i18n("Unknown subtype");
    }
}
