/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Carlos Licea <carlos.licea@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrArrowHeadWipeEffectFactory.h"

#include <cmath>

#include <KLazyLocalizedString>

#include "../KPrIrisWipeEffectStrategyBase.h"

#define ArrowHeadEffectFactoryId "ArrowHeadEffectFactory"

KPrArrowHeadWipeEffectFactory::KPrArrowHeadWipeEffectFactory()
    : KPrPageEffectFactory(ArrowHeadEffectFactoryId, i18n("ArrowHead"))
{
    QPainterPath shape;

    // up
    shape.moveTo(-25 * cos(M_PI / 2), -25 * sin(M_PI / 2));
    shape.lineTo(-25 * cos(7 * M_PI / 6), -25 * sin(7 * M_PI / 6));
    shape.lineTo(0, 7);
    shape.lineTo(-25 * cos(11 * M_PI / 6), -25 * sin(11 * M_PI / 6));
    shape.closeSubpath();
    addStrategy(new KPrIrisWipeEffectStrategyBase(shape, Up, "arrowHeadWipe", "up", false));

    // up reverse
    addStrategy(new KPrIrisWipeEffectStrategyBase(shape, UpReverse, "arrowHeadWipe", "up", true));

    // right
    shape = QPainterPath();
    shape.moveTo(25 * cos(0.0), 25 * sin(0.0));
    shape.lineTo(25 * cos(2 * M_PI / 3), 25 * sin(2 * M_PI / 3));
    shape.lineTo(-7, 0);
    shape.lineTo(25 * cos(4 * M_PI / 3), 25 * sin(4 * M_PI / 3));
    shape.closeSubpath();
    addStrategy(new KPrIrisWipeEffectStrategyBase(shape, Right, "arrowHeadWipe", "right", false));

    // right reverse
    addStrategy(new KPrIrisWipeEffectStrategyBase(shape, RightReverse, "arrowHeadWipe", "right", true));

    // down
    shape = QPainterPath();
    shape.moveTo(25 * cos(M_PI / 2), 25 * sin(M_PI / 2));
    shape.lineTo(25 * cos(7 * M_PI / 6), 25 * sin(7 * M_PI / 6));
    shape.lineTo(0, -7);
    shape.lineTo(25 * cos(11 * M_PI / 6), 25 * sin(11 * M_PI / 6));
    shape.closeSubpath();
    addStrategy(new KPrIrisWipeEffectStrategyBase(shape, Down, "arrowHeadWipe", "down", false));

    // down reverse
    addStrategy(new KPrIrisWipeEffectStrategyBase(shape, DownReverse, "arrowHeadWipe", "down", true));

    // left
    shape = QPainterPath();
    shape.moveTo(-25 * cos(0.0), 25 * sin(0.0));
    shape.lineTo(-25 * cos(2 * M_PI / 3), 25 * sin(2 * M_PI / 3));
    shape.lineTo(7, 0);
    shape.lineTo(-25 * cos(4 * M_PI / 3), 25 * sin(4 * M_PI / 3));
    shape.closeSubpath();
    addStrategy(new KPrIrisWipeEffectStrategyBase(shape, Left, "arrowHeadWipe", "left", false));

    // left reverse
    addStrategy(new KPrIrisWipeEffectStrategyBase(shape, LeftReverse, "arrowHeadWipe", "left", true));
}

KPrArrowHeadWipeEffectFactory::~KPrArrowHeadWipeEffectFactory() = default;

static const KLazyLocalizedString s_subTypes[] = {kli18n("Up"),
                                                  kli18n("Up Reverse"),
                                                  kli18n("Right"),
                                                  kli18n("Right Reverse"),
                                                  kli18n("Down"),
                                                  kli18n("Down Reverse"),
                                                  kli18n("Left"),
                                                  kli18n("Left Reverse")};

QString KPrArrowHeadWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return s_subTypes[subType].toString();
    } else {
        return i18n("Unknown subtype");
    }
}
