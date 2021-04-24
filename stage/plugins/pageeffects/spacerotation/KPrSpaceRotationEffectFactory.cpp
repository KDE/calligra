/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Benjamin Port <port.benjamin@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrSpaceRotationEffectFactory.h"

#include <klocalizedstring.h>

#include "KPrSpaceRotationFromBottomStrategy.h"
#include "KPrSpaceRotationFromTopStrategy.h"
#include "KPrSpaceRotationFromLeftStrategy.h"
#include "KPrSpaceRotationFromRightStrategy.h"

#define SpaceRotationEffectId "SpaceRotationEffect"

KPrSpaceRotationEffectFactory::KPrSpaceRotationEffectFactory()
: KPrPageEffectFactory(SpaceRotationEffectId, i18n("Space Rotation"))
{
    addStrategy(new KPrSpaceRotationFromBottomStrategy());
    addStrategy(new KPrSpaceRotationFromTopStrategy());
    addStrategy(new KPrSpaceRotationFromLeftStrategy());
    addStrategy(new KPrSpaceRotationFromRightStrategy());
}

KPrSpaceRotationEffectFactory::~KPrSpaceRotationEffectFactory()
{
}

static const char* const s_subTypes[] = {
    I18N_NOOP("From Bottom"),
    I18N_NOOP("From Top"),
    I18N_NOOP("From Left"),
    I18N_NOOP("From Right")
};

QString KPrSpaceRotationEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return i18n(s_subTypes[subType]);
    } else {
        return i18n("Unknown subtype");
    }
}
