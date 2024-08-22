/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Benjamin Port <port.benjamin@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrSpaceRotationEffectFactory.h"

#include <KLazyLocalizedString>

#include "KPrSpaceRotationFromBottomStrategy.h"
#include "KPrSpaceRotationFromLeftStrategy.h"
#include "KPrSpaceRotationFromRightStrategy.h"
#include "KPrSpaceRotationFromTopStrategy.h"

#define SpaceRotationEffectId "SpaceRotationEffect"

KPrSpaceRotationEffectFactory::KPrSpaceRotationEffectFactory()
    : KPrPageEffectFactory(SpaceRotationEffectId, i18n("Space Rotation"))
{
    addStrategy(new KPrSpaceRotationFromBottomStrategy());
    addStrategy(new KPrSpaceRotationFromTopStrategy());
    addStrategy(new KPrSpaceRotationFromLeftStrategy());
    addStrategy(new KPrSpaceRotationFromRightStrategy());
}

KPrSpaceRotationEffectFactory::~KPrSpaceRotationEffectFactory() = default;

static const KLazyLocalizedString s_subTypes[] = {kli18n("From Bottom"), kli18n("From Top"), kli18n("From Left"), kli18n("From Right")};

QString KPrSpaceRotationEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return s_subTypes[subType].toString();
    } else {
        return i18n("Unknown subtype");
    }
}
