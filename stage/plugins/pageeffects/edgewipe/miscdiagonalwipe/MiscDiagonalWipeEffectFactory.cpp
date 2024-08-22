/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "MiscDiagonalWipeEffectFactory.h"

#include <KLazyLocalizedString>

#include "DoubleBarnDoorWipeStrategy.h"
#include "DoubleDiamondWipeStrategy.h"

#define MiscDiagonalWipeEffectId "MiscDiagonalWipeEffect"

MiscDiagonalWipeEffectFactory::MiscDiagonalWipeEffectFactory()
    : KPrPageEffectFactory(MiscDiagonalWipeEffectId, i18n("Misc Diagonal"))
{
    addStrategy(new DoubleBarnDoorWipeStrategy());
    addStrategy(new DoubleDiamondWipeStrategy());
}

MiscDiagonalWipeEffectFactory::~MiscDiagonalWipeEffectFactory() = default;

static const KLazyLocalizedString s_subTypes[] = {
    kli18n("Double Barn Door"),
    kli18n("Double Diamond"),
};

QString MiscDiagonalWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return s_subTypes[subType].toString();
    } else {
        return i18n("Unknown subtype");
    }
}
