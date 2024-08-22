/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrDoubleFanWipeEffectFactory.h"

#include <KLazyLocalizedString>

#include "KPrCenterFanWipeStrategy.h"
#include "KPrSideFanWipeStrategy.h"

#define DoubleFanWipeEffectId "DoubleFanWipeEffect"

KPrDoubleFanWipeEffectFactory::KPrDoubleFanWipeEffectFactory()
    : KPrPageEffectFactory(DoubleFanWipeEffectId, i18n("Double Fan"))
{
    addStrategy(new KPrCenterFanWipeStrategy(90, 2, FanOutVertical, "doubleFanWipe", "fanOutVertical", false));
    addStrategy(new KPrCenterFanWipeStrategy(0, 2, FanOutHorizontal, "doubleFanWipe", "fanOutHorizontal", false));
    addStrategy(new KPrSideFanWipeStrategy(90, 2, FanInVertical, "doubleFanWipe", "fanInVertical", false));
    addStrategy(new KPrSideFanWipeStrategy(180, 2, FanInHorizontal, "doubleFanWipe", "fanInHorizontal", false));
    addStrategy(new KPrSideFanWipeStrategy(90, 2, FanInVerticalReverse, "doubleFanWipe", "fanInVertical", true));
    addStrategy(new KPrSideFanWipeStrategy(180, 2, FanInHorizontalReverse, "doubleFanWipe", "fanInHorizontal", true));
}

KPrDoubleFanWipeEffectFactory::~KPrDoubleFanWipeEffectFactory() = default;

static const KLazyLocalizedString s_subTypes[] = {kli18n("Fan Out Vertical"),
                                                  kli18n("Fan Out Horizontal"),
                                                  kli18n("Fan In Vertical"),
                                                  kli18n("Fan In Horizontal"),
                                                  kli18n("Fan In Vertical Reverse"),
                                                  kli18n("Fan In Horizontal Reverse")};

QString KPrDoubleFanWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return s_subTypes[subType].toString();
    } else {
        return i18n("Unknown subtype");
    }
}
