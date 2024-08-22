/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "FourBoxWipeEffectFactory.h"

#include <KLazyLocalizedString>

#include "CornersInWipeStrategy.h"
#include "CornersOutWipeStrategy.h"

#define FourBoxWipeEffectId "FourBoxWipeEffect"

FourBoxWipeEffectFactory::FourBoxWipeEffectFactory()
    : KPrPageEffectFactory(FourBoxWipeEffectId, i18n("Four Box"))
{
    addStrategy(new CornersInWipeStrategy(false));
    addStrategy(new CornersInWipeStrategy(true));
    addStrategy(new CornersOutWipeStrategy(false));
    addStrategy(new CornersOutWipeStrategy(true));
}

FourBoxWipeEffectFactory::~FourBoxWipeEffectFactory() = default;

static const KLazyLocalizedString s_subTypes[] = {kli18n("Corners In"), kli18n("Corners In Reverse"), kli18n("Corners Out"), kli18n("Corners Out Reverse")};

QString FourBoxWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return s_subTypes[subType].toString();
    } else {
        return i18n("Unknown subtype");
    }
}
