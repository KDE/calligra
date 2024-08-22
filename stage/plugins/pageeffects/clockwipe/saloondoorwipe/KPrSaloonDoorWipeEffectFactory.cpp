/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrSaloonDoorWipeEffectFactory.h"

#include <KLazyLocalizedString>

#include "KPrSaloonDoorWipeStrategy.h"

#define SaloonDoorWipeEffectId "SaloonDoorWipeEffect"

KPrSaloonDoorWipeEffectFactory::KPrSaloonDoorWipeEffectFactory()
    : KPrPageEffectFactory(SaloonDoorWipeEffectId, i18n("Saloon Door"))
{
    addStrategy(new KPrSaloonDoorWipeStrategy(FromTop, "saloonDoorWipe", "top", false));
    addStrategy(new KPrSaloonDoorWipeStrategy(FromLeft, "saloonDoorWipe", "left", false));
    addStrategy(new KPrSaloonDoorWipeStrategy(FromBottom, "saloonDoorWipe", "bottom", false));
    addStrategy(new KPrSaloonDoorWipeStrategy(FromRight, "saloonDoorWipe", "right", false));

    addStrategy(new KPrSaloonDoorWipeStrategy(ToTop, "saloonDoorWipe", "top", true));
    addStrategy(new KPrSaloonDoorWipeStrategy(ToLeft, "saloonDoorWipe", "left", true));
    addStrategy(new KPrSaloonDoorWipeStrategy(ToBottom, "saloonDoorWipe", "bottom", true));
    addStrategy(new KPrSaloonDoorWipeStrategy(ToRight, "saloonDoorWipe", "right", true));
}

KPrSaloonDoorWipeEffectFactory::~KPrSaloonDoorWipeEffectFactory() = default;

static const KLazyLocalizedString s_subTypes[] = {kli18n("From Top"),
                                                  kli18n("From Left"),
                                                  kli18n("From Bottom"),
                                                  kli18n("From Right"),
                                                  kli18n("To Top"),
                                                  kli18n("To Left"),
                                                  kli18n("To Bottom"),
                                                  kli18n("To Right")};

QString KPrSaloonDoorWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return s_subTypes[subType].toString();
    } else {
        return i18n("Unknown subtype");
    }
}
