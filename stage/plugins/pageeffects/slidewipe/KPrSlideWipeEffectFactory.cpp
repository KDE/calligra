/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007-2008 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrSlideWipeEffectFactory.h"

#include <KLazyLocalizedString>

#include "KPrSlideWipeFromBottomStrategy.h"
#include "KPrSlideWipeFromLeftStrategy.h"
#include "KPrSlideWipeFromRightStrategy.h"
#include "KPrSlideWipeFromTopStrategy.h"
#include "KPrSlideWipeToBottomStrategy.h"
#include "KPrSlideWipeToLeftStrategy.h"
#include "KPrSlideWipeToRightStrategy.h"
#include "KPrSlideWipeToTopStrategy.h"

#define SlideWipeEffectId "SlideWipeEffect"

KPrSlideWipeEffectFactory::KPrSlideWipeEffectFactory()
    : KPrPageEffectFactory(SlideWipeEffectId, i18n("Slide"))
{
    addStrategy(new KPrSlideWipeFromTopStrategy());
    addStrategy(new KPrSlideWipeFromBottomStrategy());
    addStrategy(new KPrSlideWipeFromLeftStrategy());
    addStrategy(new KPrSlideWipeFromRightStrategy());
    addStrategy(new KPrSlideWipeToTopStrategy());
    addStrategy(new KPrSlideWipeToBottomStrategy());
    addStrategy(new KPrSlideWipeToLeftStrategy());
    addStrategy(new KPrSlideWipeToRightStrategy());
}

KPrSlideWipeEffectFactory::~KPrSlideWipeEffectFactory() = default;

static const KLazyLocalizedString s_subTypes[] = {kli18n("From Left"),
                                                  kli18n("From Right"),
                                                  kli18n("From Top"),
                                                  kli18n("From Bottom"),
                                                  kli18n("To Left"),
                                                  kli18n("To Right"),
                                                  kli18n("To Top"),
                                                  kli18n("To Bottom")};

QString KPrSlideWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return s_subTypes[subType].toString();
    } else {
        return i18n("Unknown subtype");
    }
}
