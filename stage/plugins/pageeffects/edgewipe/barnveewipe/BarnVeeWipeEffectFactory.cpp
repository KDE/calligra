/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "BarnVeeWipeEffectFactory.h"

#include <KLazyLocalizedString>

#include "BarnVeeWipeStrategy.h"

#define BarnVeeWipeEffectId "BarnVeeWipeEffect"

BarnVeeWipeEffectFactory::BarnVeeWipeEffectFactory()
    : KPrPageEffectFactory(BarnVeeWipeEffectId, i18n("Barn Vee"))
{
    addStrategy(new BarnVeeWipeStrategy(FromTop, "down", false));
    addStrategy(new BarnVeeWipeStrategy(FromRight, "left", false));
    addStrategy(new BarnVeeWipeStrategy(FromBottom, "top", false));
    addStrategy(new BarnVeeWipeStrategy(FromLeft, "right", false));
}

BarnVeeWipeEffectFactory::~BarnVeeWipeEffectFactory() = default;

static const KLazyLocalizedString s_subTypes[] = {kli18n("From Top"), kli18n("From Right"), kli18n("From Bottom"), kli18n("From Left")};

QString BarnVeeWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return s_subTypes[subType].toString();
    } else {
        return i18n("Unknown subtype");
    }
}
