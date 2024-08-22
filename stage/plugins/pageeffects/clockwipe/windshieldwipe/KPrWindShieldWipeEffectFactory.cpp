/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrWindShieldWipeEffectFactory.h"

#include <KLazyLocalizedString>

#include "KPrWindShieldWipeStrategy.h"

#define WindShieldWipeEffectId "WindShieldWipeEffect"

KPrWindShieldWipeEffectFactory::KPrWindShieldWipeEffectFactory()
    : KPrPageEffectFactory(WindShieldWipeEffectId, i18n("Windshield"))
{
    addStrategy(new KPrWindShieldWipeStrategy(Right, "windshieldWipe", "right", false));
    addStrategy(new KPrWindShieldWipeStrategy(Up, "windshieldWipe", "up", false));
    addStrategy(new KPrWindShieldWipeStrategy(Vertical, "windshieldWipe", "vertical", false));
    addStrategy(new KPrWindShieldWipeStrategy(Horizontal, "windshieldWipe", "horizontal", false));

    addStrategy(new KPrWindShieldWipeStrategy(RightReverse, "windshieldWipe", "right", true));
    addStrategy(new KPrWindShieldWipeStrategy(UpReverse, "windshieldWipe", "up", true));
    addStrategy(new KPrWindShieldWipeStrategy(VerticalReverse, "windshieldWipe", "vertical", true));
    addStrategy(new KPrWindShieldWipeStrategy(HorizontalReverse, "windshieldWipe", "horizontal", true));
}

KPrWindShieldWipeEffectFactory::~KPrWindShieldWipeEffectFactory() = default;

static const KLazyLocalizedString s_subTypes[] = {kli18n("Right"),
                                                  kli18n("Up"),
                                                  kli18n("Vertical"),
                                                  kli18n("Horizontal"),
                                                  kli18n("Right Reverse"),
                                                  kli18n("Up Reverse"),
                                                  kli18n("Vertical Reverse"),
                                                  kli18n("Horizontal Reverse")};

QString KPrWindShieldWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return s_subTypes[subType].toString();
    } else {
        return i18n("Unknown subtype");
    }
}
