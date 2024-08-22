/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "BarnDoorWipeEffectFactory.h"

#include <KLazyLocalizedString>

#include "BarnDoorWipeStrategy.h"

#define BarnDoorWipeEffectId "BarnDoorWipeEffect"

BarnDoorWipeEffectFactory::BarnDoorWipeEffectFactory()
    : KPrPageEffectFactory(BarnDoorWipeEffectId, i18n("Barn Door"))
{
    addStrategy(new BarnDoorWipeStrategy(Vertical, "vertical", false));
    addStrategy(new BarnDoorWipeStrategy(VerticalReverse, "vertical", true));
    addStrategy(new BarnDoorWipeStrategy(Horizontal, "horizontal", false));
    addStrategy(new BarnDoorWipeStrategy(HorizontalReverse, "horizontal", true));
    addStrategy(new BarnDoorWipeStrategy(DiagonalBottomLeft, "diagonalBottomLeft", false));
    addStrategy(new BarnDoorWipeStrategy(DiagonalBottomLeftReverse, "diagonalBottomLeft", true));
    addStrategy(new BarnDoorWipeStrategy(DiagonalTopLeft, "diagonalTopLeft", false));
    addStrategy(new BarnDoorWipeStrategy(DiagonalTopLeftReverse, "diagonalTopLeft", true));
}

BarnDoorWipeEffectFactory::~BarnDoorWipeEffectFactory() = default;

static const KLazyLocalizedString s_subTypes[] = {kli18n("Vertical"),
                                                  kli18n("Vertical Reverse"),
                                                  kli18n("Horizontal"),
                                                  kli18n("Horizontal Reverse"),
                                                  kli18n("Diagonal Bottom Left"),
                                                  kli18n("Diagonal Bottom Left Reverse"),
                                                  kli18n("Diagonal Top Left"),
                                                  kli18n("Diagonal Top Left Reverse")};

QString BarnDoorWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return s_subTypes[subType].toString();
    } else {
        return i18n("Unknown subtype");
    }
}
