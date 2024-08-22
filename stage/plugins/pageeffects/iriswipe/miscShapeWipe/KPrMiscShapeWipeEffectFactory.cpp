// This file is part of the KDE project
// SPDX-FileCopyrightText: 2008 Carlos Licea <carlos.licea@kdemail.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "KPrMiscShapeWipeEffectFactory.h"

#include <KLazyLocalizedString>

#include "../KPrIrisWipeEffectStrategyBase.h"

#define MiscShapeWipeEffectFactoryId "MiscShapeWipeEffect"

KPrMiscShapeWipeEffectFactory::KPrMiscShapeWipeEffectFactory()
    : KPrPageEffectFactory(MiscShapeWipeEffectFactoryId, i18n("Misc"))
{
    QPainterPath shape;

    // heart
    //     shape.moveTo( 0, 0 );
    //     shape.quadTo( -6, -12, -12, 0 );
    //     shape.lineTo( 0, 12 );
    //     shape.lineTo( 12, 0 );
    //     shape.quadTo( 6, -12, 0, 0 );
    //     addStrategy(new KPrIrisWipeEffectStrategyBase( shape, Heart, "miscShapeWipe", "heart", false ) );

    // keyhole
    shape = QPainterPath();
    shape.moveTo(0, -12);
    shape.lineTo(-6, 12);
    shape.lineTo(6, 12);
    shape.closeSubpath();
    QPainterPath shape2;
    shape2.addEllipse(-6, -12, 12, 12);
    shape = shape.united(shape2);
    addStrategy(new KPrIrisWipeEffectStrategyBase(shape, Keyhole, "miscShapeWipe", "keyhole", false));

    // keyhole reverse
    addStrategy(new KPrIrisWipeEffectStrategyBase(shape, KeyholeReverse, "miscShapeWipe", "keyhole", true));
}

KPrMiscShapeWipeEffectFactory::~KPrMiscShapeWipeEffectFactory() = default;

static const KLazyLocalizedString s_subTypes[] = {kli18n("Heart"), kli18n("Heart Reverse"), kli18n("Keyhole"), kli18n("Keyhole Reverse")};

QString KPrMiscShapeWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return s_subTypes[subType].toString();
    } else {
        return i18n("Unknown subtype");
    }
}
