/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrFanWipeEffectFactory.h"

#include <KLocalizedString>
#include <klazylocalizedstring.h>

#include "KPrCenterFanWipeStrategy.h"
#include "KPrSideFanWipeStrategy.h"

#define FanWipeEffectId "FanWipeEffect"

KPrFanWipeEffectFactory::KPrFanWipeEffectFactory()
    : KPrPageEffectFactory(FanWipeEffectId, i18n("Fan"))
{
    addStrategy(new KPrCenterFanWipeStrategy(0, 1, CenterRight, "fanWipe", "centerRight", false));
    addStrategy(new KPrCenterFanWipeStrategy(90, 1, CenterTop, "fanWipe", "centerTop", false));
    addStrategy(new KPrCenterFanWipeStrategy(180, 1, CenterLeft, "fanWipe", "centerRight", true));
    addStrategy(new KPrCenterFanWipeStrategy(270, 1, CenterBottom, "fanWipe", "centerTop", true));

    addStrategy(new KPrSideFanWipeStrategy(90, 1, FanOutTop, "fanWipe", "top", false));
    addStrategy(new KPrSideFanWipeStrategy(0, 1, FanOutRight, "fanWipe", "right", false));
    addStrategy(new KPrSideFanWipeStrategy(270, 1, FanOutBottom, "fanWipe", "bottom", false));
    addStrategy(new KPrSideFanWipeStrategy(180, 1, FanOutLeft, "fanWipe", "left", false));
    addStrategy(new KPrSideFanWipeStrategy(90, 1, FanInTop, "fanWipe", "top", true));
    addStrategy(new KPrSideFanWipeStrategy(0, 1, FanInRight, "fanWipe", "right", true));
    addStrategy(new KPrSideFanWipeStrategy(270, 1, FanInBottom, "fanWipe", "bottom", true));
    addStrategy(new KPrSideFanWipeStrategy(180, 1, FanInLeft, "fanWipe", "left", true));
}

KPrFanWipeEffectFactory::~KPrFanWipeEffectFactory() = default;

static const KLazyLocalizedString s_subTypes[] = {kli18n("Center Right"),
                                                  kli18n("Center Top"),
                                                  kli18n("Center Left"),
                                                  kli18n("Center Bottom"),
                                                  kli18n("Fan Out Top"),
                                                  kli18n("Fan Out Right"),
                                                  kli18n("Fan Out Bottom"),
                                                  kli18n("Fan Out Left"),
                                                  kli18n("Fan In Top"),
                                                  kli18n("Fan In Right"),
                                                  kli18n("Fan In Bottom"),
                                                  kli18n("Fan In Left")};

QString KPrFanWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return s_subTypes[subType].toString();
    } else {
        return i18n("Unknown subtype");
    }
}
