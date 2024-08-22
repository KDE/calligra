// This file is part of the KDE project
// SPDX-FileCopyrightText: 2008 Carlos Licea <carlos.licea@kdemail.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "KPrRoundRectWipeEffectFactory.h"

#include <KLazyLocalizedString>

#include "../KPrIrisWipeEffectStrategyBase.h"

#define RoundRectWipeEffectFactoryId "RoundRectWipeEffectFactory"

KPrRoundRectWipeEffectFactory::KPrRoundRectWipeEffectFactory()
    : KPrPageEffectFactory(RoundRectWipeEffectFactoryId, i18n("RoundRect"))
{
    QPainterPath shape;

    // horizontal
    shape.addRoundedRect(-25, -12, 50, 24, 10, Qt::AbsoluteSize);
    addStrategy(new KPrIrisWipeEffectStrategyBase(shape, Horizontal, "RoundRect", "horizontal", false));

    // horizontal reverse
    addStrategy(new KPrIrisWipeEffectStrategyBase(shape, HorizontalReverse, "RoundRect", "horizontal", true));

    // vertical
    shape = QPainterPath();
    shape.addRoundedRect(-12, -25, 24, 50, 10, Qt::AbsoluteSize);
    addStrategy(new KPrIrisWipeEffectStrategyBase(shape, Vertical, "RoundRect", "vertical", false));

    // vertical reverse
    addStrategy(new KPrIrisWipeEffectStrategyBase(shape, VerticalReverse, "RoundRect", "vertical", true));
}

KPrRoundRectWipeEffectFactory::~KPrRoundRectWipeEffectFactory() = default;

static const KLazyLocalizedString s_subTypes[] = {kli18n("Horizontal"), kli18n("Horizontal Reverse"), kli18n("Vertical"), kli18n("Vertical Reverse")};

QString KPrRoundRectWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return s_subTypes[subType].toString();
    } else {
        return i18n("Unknown subtype");
    }
}
