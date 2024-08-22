/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrPinWheelWipeEffectFactory.h"

#include <KLazyLocalizedString>

#include "KPrClockWipeStrategy.h"

#define PinWheelWipeEffectId "PinWheelWipeEffect"

KPrPinWheelWipeEffectFactory::KPrPinWheelWipeEffectFactory()
    : KPrPageEffectFactory(PinWheelWipeEffectId, i18n("Pin Wheel"))
{
    addStrategy(new KPrClockWipeStrategy(270, 2, TwoBladeVerticalClockwise, "pinWheelWipe", "twoBladeVertical", false));
    addStrategy(new KPrClockWipeStrategy(0, 2, TwoBladeHorizontalClockwise, "pinWheelWipe", "twoBladeHorizontal", false));
    addStrategy(new KPrClockWipeStrategy(0, 4, FourBladeClockwise, "pinWheelWipe", "fourBlade", false));
    addStrategy(new KPrClockWipeStrategy(270, 2, TwoBladeVerticalCounterClockwise, "pinWheelWipe", "twoBladeVertical", true));
    addStrategy(new KPrClockWipeStrategy(0, 2, TwoBladeHorizontalCounterClockwise, "pinWheelWipe", "twoBladeHorizontal", true));
    addStrategy(new KPrClockWipeStrategy(0, 4, FourBladeCounterClockwise, "pinWheelWipe", "fourBlade", true));
}

KPrPinWheelWipeEffectFactory::~KPrPinWheelWipeEffectFactory() = default;

static const KLazyLocalizedString s_subTypes[] = {kli18n("Two Blades Vertical Clockwise"),
                                                  kli18n("Two Blades Horizontal Clockwise"),
                                                  kli18n("Four Blades Horizontal Clockwise"),
                                                  kli18n("Two Blades Vertical Counterclockwise"),
                                                  kli18n("Two Blades Horizontal Counterclockwise"),
                                                  kli18n("Four Blades Horizontal Counterclockwise")};

QString KPrPinWheelWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return s_subTypes[subType].toString();
    } else {
        return i18n("Unknown subtype");
    }
}
