/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2010 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KPrFadeEffectFactory.h"

#include <klocalizedstring.h>

#include "KPrFadeCrossStrategy.h"
#include "KPrFadeOverColorStrategy.h"

#define FadeEffectId "FadeEffect"

KPrFadeEffectFactory::KPrFadeEffectFactory()
: KPrPageEffectFactory(FadeEffectId, i18n("Fade"))
{
    addStrategy(new KPrFadeCrossStrategy());
    addStrategy(new KPrFadeOverColorStrategy());
}

KPrFadeEffectFactory::~KPrFadeEffectFactory()
{
}

static const char* const s_subTypes[] = {
    I18N_NOOP("Crossfade"),
    I18N_NOOP("Fade over Color")
};

QString KPrFadeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return i18n(s_subTypes[subType]);
    }
    else {
        return i18n("Unknown subtype");
    }
}
