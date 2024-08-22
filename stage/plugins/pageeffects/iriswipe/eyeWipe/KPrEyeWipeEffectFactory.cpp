
/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Carlos Licea <carlos.licea@kdemail.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrEyeWipeEffectFactory.h"

#include <KLazyLocalizedString>

#include "../KPrIrisWipeEffectStrategyBase.h"

#define EyeWipeEffectId "EyeWipeEffect"

KPrEyeWipeEffectFactory::KPrEyeWipeEffectFactory()
    : KPrPageEffectFactory(EyeWipeEffectId, i18n("Eye"))
{
    QPainterPath shape;

    // vertical
    shape.moveTo(0, -12);
    shape.quadTo(10, 0, 0, 12);
    shape.quadTo(-10, 0, 0, -12);
    addStrategy(new KPrIrisWipeEffectStrategyBase(shape, Vertical, "eyeWipe", "vertical", false));

    // vertical reverse
    addStrategy(new KPrIrisWipeEffectStrategyBase(shape, VerticalReverse, "eyeWipe", "vertical", true));

    // horizontal
    shape = QPainterPath();
    shape.moveTo(-12, 0);
    shape.quadTo(0, 10, 12, 0);
    shape.quadTo(0, -10, -12, 0);
    addStrategy(new KPrIrisWipeEffectStrategyBase(shape, Horizontal, "eyeWipe", "horizontal", false));

    // horizontal reverse
    addStrategy(new KPrIrisWipeEffectStrategyBase(shape, HorizontalReverse, "eyeWipe", "horizontal", true));
}

KPrEyeWipeEffectFactory::~KPrEyeWipeEffectFactory() = default;

static const KLazyLocalizedString s_subTypes[] = {
    kli18n("Vertical"),
    kli18n("Vertical Reverse"),
    kli18n("Horizontal"),
    kli18n("Horizontal Reverse"),
};

QString KPrEyeWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return s_subTypes[subType].toString();
    } else {
        return i18n("Unknown subtype");
    }
}
