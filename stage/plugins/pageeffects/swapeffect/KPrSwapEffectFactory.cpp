/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Benjamin Port <port.benjamin@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrSwapEffectFactory.h"

#include <klocalizedstring.h>

#include "KPrSwapEffectHorizontalStrategy.h"

#define SwapEffectId "SwapEffect"

KPrSwapEffectFactory::KPrSwapEffectFactory()
: KPrPageEffectFactory(SwapEffectId, i18n("Swap effect"))
{
    addStrategy(new KPrSwapEffectHorizontalStrategy());
}

KPrSwapEffectFactory::~KPrSwapEffectFactory()
{
}

static const char* const s_subTypes[] = {
    I18N_NOOP("Horizontal")
};

QString KPrSwapEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return i18n(s_subTypes[subType]);
    }
    else {
        return i18n("Unknown subtype");
    }
}
