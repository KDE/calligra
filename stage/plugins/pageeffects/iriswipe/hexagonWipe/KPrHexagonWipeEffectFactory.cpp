// This file is part of the KDE project
// SPDX-FileCopyrightText: 2008 Carlos Licea <carlos.licea@kdemail.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "KPrHexagonWipeEffectFactory.h"

#include <cmath>

#include <KLazyLocalizedString>

#include "../KPrIrisWipeEffectStrategyBase.h"

using std::cos;
using std::sin;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define HexagonWipeEffectId "HexagonWipeEffect"

KPrHexagonWipeEffectFactory::KPrHexagonWipeEffectFactory()
    : KPrPageEffectFactory(HexagonWipeEffectId, i18n("Hexagon"))
{
    QPainterPath shape;

    // horizontal
    shape.moveTo(25, 0);
    shape.lineTo(25 * cos(M_PI / 3), -25 * sin(M_PI / 3));
    shape.lineTo(25 * cos(2 * M_PI / 3), -25 * sin(2 * M_PI / 3));
    shape.lineTo(25 * cos(M_PI), -25 * sin(M_PI));
    shape.lineTo(25 * cos(4 * M_PI / 3), -25 * sin(4 * M_PI / 3));
    shape.lineTo(25 * cos(5 * M_PI / 3), -25 * sin(5 * M_PI / 3));
    shape.lineTo(25 * cos(2 * M_PI), -25 * sin(2 * M_PI));
    shape.closeSubpath();
    addStrategy(new KPrIrisWipeEffectStrategyBase(shape, Horizontal, "hexagonWipe", "horizontal", false));

    // horizontal reverse
    addStrategy(new KPrIrisWipeEffectStrategyBase(shape, HorizontalReverse, "hexagonWipe", "horizontal", true));

    // vertical
    shape = QPainterPath();
    shape.moveTo(0, -25);
    shape.lineTo(25 * cos(M_PI / 2), -25 * sin(M_PI / 2));
    shape.lineTo(25 * cos(5 * M_PI / 6), -25 * sin(5 * M_PI / 6));
    shape.lineTo(25 * cos(7 * M_PI / 6), -25 * sin(7 * M_PI / 6));
    shape.lineTo(25 * cos(9 * M_PI / 6), -25 * sin(9 * M_PI / 6));
    shape.lineTo(25 * cos(11 * M_PI / 6), -25 * sin(11 * M_PI / 6));
    shape.lineTo(25 * cos(M_PI / 6), -25 * sin(M_PI / 6));
    shape.closeSubpath();
    addStrategy(new KPrIrisWipeEffectStrategyBase(shape, Vertical, "hexagonWipe", "vertical", false));

    // vertical reverse
    addStrategy(new KPrIrisWipeEffectStrategyBase(shape, VerticalReverse, "hexagonWipe", "vertical", true));
}

KPrHexagonWipeEffectFactory::~KPrHexagonWipeEffectFactory() = default;

static const KLazyLocalizedString s_subTypes[] = {kli18n("Horizontal"), kli18n("Horizontal Reverse"), kli18n("Vertical"), kli18n("Vertical Reverse")};

QString KPrHexagonWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return s_subTypes[subType].toString();
    } else {
        return i18n("Unknown subtype");
    }
}
