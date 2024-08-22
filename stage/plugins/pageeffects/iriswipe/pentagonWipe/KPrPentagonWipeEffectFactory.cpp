/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Carlos Licea <carlos.licea@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrPentagonWipeEffectFactory.h"

#include <cmath>

#include <KLazyLocalizedString>

#include "../KPrIrisWipeEffectStrategyBase.h"

#define PentagonEffectFactoryId "PentagonEffectFactory"

KPrPentagonWipeEffectFactory::KPrPentagonWipeEffectFactory()
    : KPrPageEffectFactory(PentagonEffectFactoryId, i18n("Pentagon"))
{
    QPainterPath shape;

    // up
    shape.moveTo(25 * cos(M_PI / 2), -25 * sin(M_PI / 2));
    shape.lineTo(25 * cos(9 * M_PI / 10), -25 * sin(9 * M_PI / 10));
    shape.lineTo(25 * cos(13 * M_PI / 10), -25 * sin(13 * M_PI / 10));
    shape.lineTo(25 * cos(17 * M_PI / 10), -25 * sin(17 * M_PI / 10));
    shape.lineTo(25 * cos(M_PI / 10), -25 * sin(M_PI / 10));
    shape.closeSubpath();
    addStrategy(new KPrIrisWipeEffectStrategyBase(shape, Up, "pentagonWipe", "up", false));

    // up reverse
    addStrategy(new KPrIrisWipeEffectStrategyBase(shape, UpReverse, "pentagonWipe", "up", true));

    // down
    shape = QPainterPath();
    shape.moveTo(25 * cos(M_PI / 2), 25 * sin(M_PI / 2));
    shape.lineTo(25 * cos(9 * M_PI / 10), 25 * sin(9 * M_PI / 10));
    shape.lineTo(25 * cos(13 * M_PI / 10), 25 * sin(13 * M_PI / 10));
    shape.lineTo(25 * cos(17 * M_PI / 10), 25 * sin(17 * M_PI / 10));
    shape.lineTo(25 * cos(M_PI / 10), 25 * sin(M_PI / 10));
    shape.closeSubpath();
    addStrategy(new KPrIrisWipeEffectStrategyBase(shape, Down, "pentagonWipe", "down", false));

    // down reverse
    addStrategy(new KPrIrisWipeEffectStrategyBase(shape, DownReverse, "pentagonWipe", "down", true));
}

KPrPentagonWipeEffectFactory::~KPrPentagonWipeEffectFactory() = default;

static const KLazyLocalizedString s_subTypes[] = {kli18n("Up"), kli18n("Up Reverse"), kli18n("Down"), kli18n("Down Reverse")};

QString KPrPentagonWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return s_subTypes[subType].toString();
    } else {
        return i18n("Unknown subtype");
    }
}
