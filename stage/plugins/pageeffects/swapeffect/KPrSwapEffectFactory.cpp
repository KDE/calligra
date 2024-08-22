/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Benjamin Port <port.benjamin@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrSwapEffectFactory.h"

#include <klazylocalizedstring.h>

#include "KPrSwapEffectHorizontalStrategy.h"

#define SwapEffectId "SwapEffect"

KPrSwapEffectFactory::KPrSwapEffectFactory()
    : KPrPageEffectFactory(SwapEffectId, i18n("Swap effect"))
{
    addStrategy(new KPrSwapEffectHorizontalStrategy());
}

KPrSwapEffectFactory::~KPrSwapEffectFactory() = default;

static const KLazyLocalizedString s_subTypes[] = {kli18n("Horizontal")};

QString KPrSwapEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return s_subTypes[subType].toString();
    } else {
        return i18n("Unknown subtype");
    }
}
