
/* This file is part of the KDE project
   Copyright (C) 2008 Carlos Licea <carlos.licea@kdemail.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software itation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software itation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KPrEyeWipeEffectFactory.h"

#include <cmath>

#include <klocale.h>

#include "../KPrIrisWipeEffectStrategyBase.h"

#define EyeWipeEffectId "EyeWipeEffect"

KPrEyeWipeEffectFactory::KPrEyeWipeEffectFactory()
: KPrPageEffectFactory( EyeWipeEffectId, i18n("Eye") )
{
     QPainterPath shape;

    //vertical
    shape.moveTo( 0, -12 );
    shape.quadTo( 10, 0, 0, 12);
    shape.quadTo( -10, 0, 0, -12);
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, Vertical, "eyeWipe", "vertical", false ) );

    //vertical reverse
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, VerticalReverse, "eyeWipe", "vertical", true ) );

    //horizontal
    shape = QPainterPath();
    shape.moveTo( -12, 0 );
    shape.quadTo( 0, 10, 12, 0 );
    shape.quadTo( 0, -10, -12, 0);
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, Horizontal, "eyeWipe", "horizontal", false ) );

    //horizontal reverse
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, HorizontalReverse, "eyeWipe", "horizontal", true ) );
}

KPrEyeWipeEffectFactory::~KPrEyeWipeEffectFactory()
{
}

static const char* s_subTypes[] = {
    I18N_NOOP( "Vertical" ),
    I18N_NOOP( "Vertical Reverse" ),
    I18N_NOOP( "Horizontal" ),
    I18N_NOOP( "Horizontal Reverse" ),
};

QString KPrEyeWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return i18n( s_subTypes[subType] );
    } else {
        return i18n( "Unknown subtype" );
    }
}
