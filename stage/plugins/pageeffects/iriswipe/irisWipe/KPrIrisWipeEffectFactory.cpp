/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Carlos Licea <carlos.licea@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrIrisWipeEffectFactory.h"

// KF5 includes
#include <KLazyLocalizedString>

#include "../KPrIrisWipeEffectStrategyBase.h"

#define IrisWipeEffectId "IrisWipeEffect"

KPrIrisWipeEffectFactory::KPrIrisWipeEffectFactory()
    : KPrPageEffectFactory(IrisWipeEffectId, i18n("Iris"))
{
    QPainterPath shape;

    // rectangle
    shape.addRect(-25, -25, 50, 50);
    addStrategy(new KPrIrisWipeEffectStrategyBase(shape, Rectangle, "irisWipe", "rectangle", false));

    // rectangle reverse
    addStrategy(new KPrIrisWipeEffectStrategyBase(shape, RectangleReverse, "irisWipe", "rectangle", true));

    // diamond
    shape = QPainterPath();
    shape.moveTo(0, -25);
    shape.lineTo(25, 0);
    shape.lineTo(0, 25);
    shape.lineTo(-25, 0);
    shape.closeSubpath();
    addStrategy(new KPrIrisWipeEffectStrategyBase(shape, Diamond, "irisWipe", "diamond", false));

    // diamond reverse
    addStrategy(new KPrIrisWipeEffectStrategyBase(shape, DiamondReverse, "irisWipe", "diamond", true));
}

KPrIrisWipeEffectFactory::~KPrIrisWipeEffectFactory() = default;

static const KLazyLocalizedString s_subTypes[] = {kli18n("Rectangular"), kli18n("Rectangular Reverse"), kli18n("Diamond"), kli18n("Diamond Reverse")};

QString KPrIrisWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return s_subTypes[subType].toString();
    } else {
        return i18n("Unknown subtype");
    }
}
