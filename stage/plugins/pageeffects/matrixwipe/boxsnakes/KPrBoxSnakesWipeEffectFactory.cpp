/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrBoxSnakesWipeEffectFactory.h"

#include <KLazyLocalizedString>

#include "KPrBoxSnakesWipeStrategy.h"

#define BoxSnakesWipeEffectId "BoxSnakesWipeEffect"

KPrBoxSnakesWipeEffectFactory::KPrBoxSnakesWipeEffectFactory()
    : KPrPageEffectFactory(BoxSnakesWipeEffectId, i18n("Box Snakes"))
{
    addStrategy(new KPrBoxSnakesWipeStrategy(2, 1, true, false));
    addStrategy(new KPrBoxSnakesWipeStrategy(2, 1, false, false));
    addStrategy(new KPrBoxSnakesWipeStrategy(1, 2, true, false));
    addStrategy(new KPrBoxSnakesWipeStrategy(1, 2, false, false));
    addStrategy(new KPrBoxSnakesWipeStrategy(2, 2, true, false));
    addStrategy(new KPrBoxSnakesWipeStrategy(2, 2, false, false));
    addStrategy(new KPrBoxSnakesWipeStrategy(2, 1, true, true));
    addStrategy(new KPrBoxSnakesWipeStrategy(2, 1, false, true));
    addStrategy(new KPrBoxSnakesWipeStrategy(1, 2, true, true));
    addStrategy(new KPrBoxSnakesWipeStrategy(1, 2, false, true));
    addStrategy(new KPrBoxSnakesWipeStrategy(2, 2, true, true));
    addStrategy(new KPrBoxSnakesWipeStrategy(2, 2, false, true));
}

KPrBoxSnakesWipeEffectFactory::~KPrBoxSnakesWipeEffectFactory() = default;

static const KLazyLocalizedString s_subTypes[] = {kli18n("Two Box Top In"),
                                                  kli18n("Two Box Bottom In"),
                                                  kli18n("Two Box Left In"),
                                                  kli18n("Two Box Right In"),
                                                  kli18n("Four Box Vertical In"),
                                                  kli18n("Four Box Horizontal In"),
                                                  kli18n("Two Box Top Out"),
                                                  kli18n("Two Box Bottom Out"),
                                                  kli18n("Two Box Left Out"),
                                                  kli18n("Two Box Right Out"),
                                                  kli18n("Four Box Vertical Out"),
                                                  kli18n("Four Box Horizontal Out")};

QString KPrBoxSnakesWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return s_subTypes[subType].toString();
    } else {
        return i18n("Unknown subtype");
    }
}
