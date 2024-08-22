// This file is part of the KDE project
// SPDX-FileCopyrightText: 2008 Carlos Licea <carlos.licea@kdemail.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "KPrStarWipeEffectFactory.h"

#include <cmath>

#include <KLazyLocalizedString>

#include "../KPrIrisWipeEffectStrategyBase.h"

using std::cos;
using std::sin;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define StarWipeEffectId "StarWipeEffect"

KPrStarWipeEffectFactory::KPrStarWipeEffectFactory()
    : KPrPageEffectFactory(StarWipeEffectId, i18n("Star"))
{
    QPainterPath shape;

    // fourPoint
    shape = QPainterPath();
    shape.moveTo(0, -24);
    shape.lineTo(12 * cos(3 * M_PI / 4), -12 * sin(3 * M_PI / 4));
    shape.lineTo(24 * cos(M_PI), -24 * sin(M_PI));
    shape.lineTo(12 * cos(5 * M_PI / 4), -12 * sin(5 * M_PI / 4));
    shape.lineTo(24 * cos(6 * M_PI / 4), -24 * sin(6 * M_PI / 4));
    shape.lineTo(12 * cos(7 * M_PI / 4), -12 * sin(7 * M_PI / 4));
    shape.lineTo(24 * cos(2 * M_PI), -24 * sin(2 * M_PI));
    shape.lineTo(12 * cos(M_PI / 4), -12 * sin(M_PI / 4));
    addStrategy(new KPrIrisWipeEffectStrategyBase(shape, FourPoint, "starWipe", "fourPoint", false));

    // fourPoint reverse
    addStrategy(new KPrIrisWipeEffectStrategyBase(shape, FourPointReverse, "starWipe", "fourPoint", true));

    // fivePoint
    shape = QPainterPath();
    shape.moveTo(0, -24);
    shape.lineTo(12 * cos(7 * M_PI / 10), -12 * sin(7 * M_PI / 10));
    shape.lineTo(24 * cos(9 * M_PI / 10), -24 * sin(9 * M_PI / 10));
    shape.lineTo(12 * cos(11 * M_PI / 10), -12 * sin(11 * M_PI / 10));
    shape.lineTo(24 * cos(13 * M_PI / 10), -24 * sin(13 * M_PI / 10));
    shape.lineTo(12 * cos(15 * M_PI / 10), -12 * sin(15 * M_PI / 10));
    shape.lineTo(24 * cos(17 * M_PI / 10), -24 * sin(17 * M_PI / 10));
    shape.lineTo(12 * cos(19 * M_PI / 10), -12 * sin(19 * M_PI / 10));
    shape.lineTo(24 * cos(1 * M_PI / 10), -24 * sin(1 * M_PI / 10));
    shape.lineTo(12 * cos(3 * M_PI / 10), -12 * sin(3 * M_PI / 10));
    shape.closeSubpath();
    addStrategy(new KPrIrisWipeEffectStrategyBase(shape, FivePoint, "starWipe", "fivePoint", false));

    // fivePoint reverse
    addStrategy(new KPrIrisWipeEffectStrategyBase(shape, FivePointReverse, "starWipe", "fivePoint", true));

    // sixPoint
    shape = QPainterPath();
    shape.moveTo(0, -24);
    shape.lineTo(12 * cos(4 * M_PI / 6), -12 * sin(4 * M_PI / 6));
    shape.lineTo(24 * cos(5 * M_PI / 6), -24 * sin(5 * M_PI / 6));
    shape.lineTo(12 * cos(M_PI), -12 * sin(M_PI));
    shape.lineTo(24 * cos(7 * M_PI / 6), -24 * sin(7 * M_PI / 6));
    shape.lineTo(12 * cos(8 * M_PI / 6), -12 * sin(8 * M_PI / 6));
    shape.lineTo(24 * cos(9 * M_PI / 6), -24 * sin(9 * M_PI / 6));
    shape.lineTo(12 * cos(10 * M_PI / 6), -12 * sin(10 * M_PI / 6));
    shape.lineTo(24 * cos(11 * M_PI / 6), -24 * sin(11 * M_PI / 6));
    shape.lineTo(12 * cos(2 * M_PI), -12 * sin(2 * M_PI));
    shape.lineTo(24 * cos(1 * M_PI / 6), -24 * sin(1 * M_PI / 6));
    shape.lineTo(12 * cos(2 * M_PI / 6), -12 * sin(2 * M_PI / 6));
    shape.closeSubpath();
    addStrategy(new KPrIrisWipeEffectStrategyBase(shape, SixPoint, "starWipe", "sixPoint", false));

    // sixPoint reverse
    addStrategy(new KPrIrisWipeEffectStrategyBase(shape, SixPointReverse, "starWipe", "sixPoint", true));
}

KPrStarWipeEffectFactory::~KPrStarWipeEffectFactory() = default;

static const KLazyLocalizedString s_subTypes[] = {kli18n("Four Point Star"),
                                                  kli18n("Four Point Star Reverse"),
                                                  kli18n("Five Point Star"),
                                                  kli18n("Five Point Star Reverse"),
                                                  kli18n("Six Point Star"),
                                                  kli18n("Six Point Star Reverse")};

QString KPrStarWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return s_subTypes[subType].toString();
    } else {
        return i18n("Unknown subtype");
    }
}
