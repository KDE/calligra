/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "VeeWipeEffectFactory.h"

#include <KLazyLocalizedString>

#include "VeeWipeStrategy.h"

#define VeeWipeEffectId "VeeWipeEffect"

VeeWipeEffectFactory::VeeWipeEffectFactory()
    : KPrPageEffectFactory(VeeWipeEffectId, i18n("Vee"))
{
    addStrategy(new VeeWipeStrategy(FromTop, "down", false));
    addStrategy(new VeeWipeStrategy(FromRight, "left", false));
    addStrategy(new VeeWipeStrategy(FromBottom, "top", false));
    addStrategy(new VeeWipeStrategy(FromLeft, "right", false));
}

VeeWipeEffectFactory::~VeeWipeEffectFactory() = default;

static const KLazyLocalizedString s_subTypes[] = {kli18n("From Top"), kli18n("From Right"), kli18n("From Bottom"), kli18n("From Left")};

QString VeeWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return s_subTypes[subType].toString();
    } else {
        return i18n("Unknown subtype");
    }
}
