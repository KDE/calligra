/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "DiagonalWipeEffectFactory.h"

#include <KLazyLocalizedString>

#include "DiagonalWipeStrategy.h"

#define DiagonalWipeEffectId "DiagonalWipeEffect"

DiagonalWipeEffectFactory::DiagonalWipeEffectFactory()
    : KPrPageEffectFactory(DiagonalWipeEffectId, i18n("Diagonal"))
{
    addStrategy(new DiagonalWipeStrategy(FromTopLeft, "topLeft", false));
    addStrategy(new DiagonalWipeStrategy(FromBottomRight, "topLeft", true));
    addStrategy(new DiagonalWipeStrategy(FromTopRight, "topRight", false));
    addStrategy(new DiagonalWipeStrategy(FromBottomLeft, "topRight", true));
}

DiagonalWipeEffectFactory::~DiagonalWipeEffectFactory() = default;

static const KLazyLocalizedString s_subTypes[] = {kli18n("From Top Left"), kli18n("From Bottom Right"), kli18n("From Top Right"), kli18n("From Bottom Left")};

QString DiagonalWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return s_subTypes[subType].toString();
    } else {
        return i18n("Unknown subtype");
    }
}
