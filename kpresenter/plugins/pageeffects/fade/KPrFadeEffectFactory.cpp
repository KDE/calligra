/* This file is part of the KDE project
 *
 * Copyright (C) 2010 Thorsten Zachmann <zachmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KPrFadeEffectFactory.h"

#include <klocale.h>

#include "KPrFadeCrossStrategy.h"

#define FadeEffectId "FadeEffect"

KPrFadeEffectFactory::KPrFadeEffectFactory()
: KPrPageEffectFactory(FadeEffectId, i18n("Fade"))
{
    addStrategy(new KPrFadeCrossStrategy());
}

KPrFadeEffectFactory::~KPrFadeEffectFactory()
{
}

static const char* s_subTypes[] = {
    I18N_NOOP("Crossfade")
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
