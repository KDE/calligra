/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ZigZagWipeEffectFactory.h"
#include "ZigZagWipeStrategy.h"

#include <KLazyLocalizedString>

#define ZigZagWipeEffectId "ZigZagWipeEffect"

ZigZagWipeEffectFactory::ZigZagWipeEffectFactory()
    : KPrPageEffectFactory(ZigZagWipeEffectId, i18n("Zig Zag"))
{
    addStrategy(new ZigZagWipeStrategy(FromLeft, "leftToRight", false));
    addStrategy(new ZigZagWipeStrategy(FromTop, "topToBottom", false));
    addStrategy(new ZigZagWipeStrategy(FromRight, "leftToRight", true));
    addStrategy(new ZigZagWipeStrategy(FromBottom, "topToBottom", true));
}

ZigZagWipeEffectFactory::~ZigZagWipeEffectFactory() = default;

static const KLazyLocalizedString s_subTypes[] = {kli18n("From Left"), kli18n("From Top"), kli18n("From Right"), kli18n("From Bottom")};

QString ZigZagWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return s_subTypes[subType].toString();
    } else {
        return i18n("Unknown subtype");
    }
}
