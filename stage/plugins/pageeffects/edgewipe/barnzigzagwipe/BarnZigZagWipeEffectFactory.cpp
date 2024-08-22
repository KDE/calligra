/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "BarnZigZagWipeEffectFactory.h"
#include "BarnZigZagWipeStrategy.h"

#include <KLazyLocalizedString>

#define BarnZigZagWipeEffectId "BarnZigZagWipeEffect"

BarnZigZagWipeEffectFactory::BarnZigZagWipeEffectFactory()
    : KPrPageEffectFactory(BarnZigZagWipeEffectId, i18n("Barn Zig Zag"))
{
    addStrategy(new BarnZigZagWipeStrategy(Vertical, "vertical", false));
    addStrategy(new BarnZigZagWipeStrategy(Horizontal, "horizontal", false));
    addStrategy(new BarnZigZagWipeStrategy(VerticalReversed, "vertical", true));
    addStrategy(new BarnZigZagWipeStrategy(HorizontalReversed, "horizontal", true));
}

BarnZigZagWipeEffectFactory::~BarnZigZagWipeEffectFactory() = default;

static const KLazyLocalizedString s_subTypes[] = {kli18n("Vertical"), kli18n("Horizontal"), kli18n("Vertical Reversed"), kli18n("Horizontal Reversed")};

QString BarnZigZagWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return s_subTypes[subType].toString();
    } else {
        return i18n("Unknown subtype");
    }
}
