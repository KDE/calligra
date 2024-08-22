/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "BoxWipeEffectFactory.h"

#include <KLazyLocalizedString>

#include "BoxWipeStrategy.h"

#define BoxWipeEffectId "BoxWipeEffect"

BoxWipeEffectFactory::BoxWipeEffectFactory()
    : KPrPageEffectFactory(BoxWipeEffectId, i18n("Box"))
{
    addStrategy(new BoxWipeStrategy(FromTopLeft, "topLeft", false));
    addStrategy(new BoxWipeStrategy(FromTopRight, "topRight", false));
    addStrategy(new BoxWipeStrategy(FromBottomRight, "bottomRight", false));
    addStrategy(new BoxWipeStrategy(FromBottomLeft, "bottomLeft", false));
    addStrategy(new BoxWipeStrategy(CenterTop, "topCenter", false));
    addStrategy(new BoxWipeStrategy(CenterRight, "rightCenter", false));
    addStrategy(new BoxWipeStrategy(CenterBottom, "bottomCenter", false));
    addStrategy(new BoxWipeStrategy(CenterLeft, "leftCenter", false));
}

BoxWipeEffectFactory::~BoxWipeEffectFactory() = default;

static const KLocalizedString s_subTypes[] = {kli18n("From Top Left"),
                                              kli18n("From Top Right"),
                                              kli18n("From Bottom Left"),
                                              kli18n("From Bottom Right"),
                                              kli18n("Center Top"),
                                              kli18n("Center Right"),
                                              kli18n("Center Bottom"),
                                              kli18n("Center Left")};

QString BoxWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return s_subTypes[subType].toString();
    } else {
        return i18n("Unknown subtype");
    }
}
