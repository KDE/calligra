/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrClockWipeEffectFactory.h"

#include <KLazyLocalizedString>
#include <KLocalizedString>

#include "KPrClockWipeStrategy.h"

#define ClockWipeEffectId "ClockWipeEffect"

KPrClockWipeEffectFactory::KPrClockWipeEffectFactory()
    : KPrPageEffectFactory(ClockWipeEffectId, i18n("Clock"))
{
    addStrategy(new KPrClockWipeStrategy(90, 1, FromTwelveClockwise, "clockWipe", "clockwiseTwelve", false));
    addStrategy(new KPrClockWipeStrategy(0, 1, FromThreeClockwise, "clockWipe", "clockwiseThree", false));
    addStrategy(new KPrClockWipeStrategy(270, 1, FromSixClockwise, "clockWipe", "clockwiseSix", false));
    addStrategy(new KPrClockWipeStrategy(180, 1, FromNineClockwise, "clockWipe", "clockwiseNine", false));
    addStrategy(new KPrClockWipeStrategy(90, 1, FromTwelveCounterClockwise, "clockWipe", "clockwiseTwelve", true));
    addStrategy(new KPrClockWipeStrategy(0, 1, FromThreeCounterClockwise, "clockWipe", "clockwiseThree", true));
    addStrategy(new KPrClockWipeStrategy(270, 1, FromSixCounterClockwise, "clockWipe", "clockwiseSix", true));
    addStrategy(new KPrClockWipeStrategy(180, 1, FromNineCounterClockwise, "clockWipe", "clockwiseNine", true));
}

KPrClockWipeEffectFactory::~KPrClockWipeEffectFactory() = default;

static const KLazyLocalizedString s_subTypes[] = {kli18n("From Twelve Clockwise"),
                                                  kli18n("From Three Clockwise"),
                                                  kli18n("From Six Clockwise"),
                                                  kli18n("From Nine Clockwise"),
                                                  kli18n("From Twelve Counterclockwise"),
                                                  kli18n("From Three Counterclockwise"),
                                                  kli18n("From Six Counterclockwise"),
                                                  kli18n("From Nine Counterclockwise")};

QString KPrClockWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return s_subTypes[subType].toString();
    } else {
        return i18n("Unknown subtype");
    }
}
