/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2010 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KPrFadeEffectFactory.h"

#include <KLazyLocalizedString>

#include "KPrFadeCrossStrategy.h"
#include "KPrFadeOverColorStrategy.h"

#define FadeEffectId "FadeEffect"

KPrFadeEffectFactory::KPrFadeEffectFactory()
    : KPrPageEffectFactory(FadeEffectId, i18n("Fade"))
{
    addStrategy(new KPrFadeCrossStrategy());
    addStrategy(new KPrFadeOverColorStrategy());
}

KPrFadeEffectFactory::~KPrFadeEffectFactory() = default;

static const KLazyLocalizedString s_subTypes[] = {kli18n("Crossfade"), kli18n("Fade over Color")};

QString KPrFadeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return s_subTypes[subType].toString();
    } else {
        return i18n("Unknown subtype");
    }
}
