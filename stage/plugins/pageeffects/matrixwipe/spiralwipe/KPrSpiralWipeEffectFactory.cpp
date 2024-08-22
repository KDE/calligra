/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrSpiralWipeEffectFactory.h"

#include <KLazyLocalizedString>

#include "KPrSpiralWipeStrategy.h"

#define SpiralWipeEffectId "SpiralWipeEffect"

KPrSpiralWipeEffectFactory::KPrSpiralWipeEffectFactory()
    : KPrPageEffectFactory(SpiralWipeEffectId, i18n("Spiral"))
{
    addStrategy(new KPrSpiralWipeStrategy(0, true, false));
    addStrategy(new KPrSpiralWipeStrategy(1, true, false));
    addStrategy(new KPrSpiralWipeStrategy(2, true, false));
    addStrategy(new KPrSpiralWipeStrategy(3, true, false));
    addStrategy(new KPrSpiralWipeStrategy(0, false, false));
    addStrategy(new KPrSpiralWipeStrategy(3, false, false));
    addStrategy(new KPrSpiralWipeStrategy(2, false, false));
    addStrategy(new KPrSpiralWipeStrategy(1, false, false));
    addStrategy(new KPrSpiralWipeStrategy(0, true, true));
    addStrategy(new KPrSpiralWipeStrategy(1, true, true));
    addStrategy(new KPrSpiralWipeStrategy(2, true, true));
    addStrategy(new KPrSpiralWipeStrategy(3, true, true));
    addStrategy(new KPrSpiralWipeStrategy(0, false, true));
    addStrategy(new KPrSpiralWipeStrategy(3, false, true));
    addStrategy(new KPrSpiralWipeStrategy(2, false, true));
    addStrategy(new KPrSpiralWipeStrategy(1, false, true));
}

KPrSpiralWipeEffectFactory::~KPrSpiralWipeEffectFactory() = default;

static const KLazyLocalizedString s_subTypes[] = {kli18n("Clockwise Top Left In"),
                                                  kli18n("Clockwise Top Left Out"),
                                                  kli18n("Clockwise Top Right In"),
                                                  kli18n("Clockwise Top Right Out"),
                                                  kli18n("Clockwise Bottom Left In"),
                                                  kli18n("Clockwise Bottom Left Out"),
                                                  kli18n("Clockwise Bottom Right In"),
                                                  kli18n("Clockwise Bottom Right Out"),
                                                  kli18n("Counterclockwise Top Left In"),
                                                  kli18n("Counterclockwise Top Left Out"),
                                                  kli18n("Counterclockwise Top Right In"),
                                                  kli18n("Counterclockwise Top Right Out"),
                                                  kli18n("Counterclockwise Bottom Left In"),
                                                  kli18n("Counterclockwise Bottom Left Out"),
                                                  kli18n("Counterclockwise Bottom Right In"),
                                                  kli18n("Counterclockwise Bottom Right Out")};

QString KPrSpiralWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return s_subTypes[subType].toString();
    } else {
        return i18n("Unknown subtype");
    }
}
