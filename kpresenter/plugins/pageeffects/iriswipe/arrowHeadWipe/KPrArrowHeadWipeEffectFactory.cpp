/* This file is part of the KDE project
   Copyright (C) 2008 Carlos Licea <carlos.licea@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/


#include <cmath>

#include <klocale.h>

#include "KPrArrowHeadWipeEffectFactory.h"
#include "../KPrIrisWipeEffectStrategyBase.h"

#define ArrowHeadEffectFactoryId "ArrowHeadEffectFactory"

KPrArrowHeadWipeEffectFactory::KPrArrowHeadWipeEffectFactory()
: KPrPageEffectFactory( ArrowHeadEffectFactoryId, i18n("ArrowHead Wipe Effect") )
{
    QPainterPath shape;

    //arrowHeadWipe up
    shape.moveTo( -25*cos( M_PI/2 ), -25*sin( M_PI/2 ) );
    shape.lineTo( -25*cos( 7 * M_PI / 6 ), -25*sin( 7 * M_PI / 6 ) );
    shape.lineTo( 0, 7 );
    shape.lineTo( -25*cos( 11 * M_PI / 6 ), -25*sin( 11 * M_PI / 6 ) );
    shape.closeSubpath();
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, Up, "arrowHeadWipe", "up", false) );

    //arrowHeadWipe right
    shape = QPainterPath();
    shape.moveTo( 25*cos( 0 ), 25*sin( 0 ) );
    shape.lineTo( 25*cos( 2 * M_PI / 3), 25*sin( 2 * M_PI / 3 ) );
    shape.lineTo( -7, 0 );
    shape.lineTo( 25*cos( 4 * M_PI / 3), 25*sin( 4 * M_PI / 3 ) );
    shape.closeSubpath();
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, Right, "arrowHeadWipe", "right", false) );

    //arrowHeadWipe down
    shape = QPainterPath();
    shape.moveTo( 25*cos( M_PI/2 ), 25*sin( M_PI/2 ) );
    shape.lineTo( 25*cos( 7 * M_PI / 6), 25*sin( 7 * M_PI / 6 ) );
    shape.lineTo( 0, -7 );
    shape.lineTo( 25*cos( 11 * M_PI / 6), 25*sin( 11 * M_PI / 6 ) );
    shape.closeSubpath();
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, Down, "arrowHeadWipe", "down", false) );

    //arrowHeadWipe left
    shape = QPainterPath();
    shape.moveTo( -25*cos( 0 ), 25*sin( 0 ) );
    shape.lineTo( -25*cos( 2 * M_PI / 3), 25*sin( 2 * M_PI / 3 ) );
    shape.lineTo( 7, 0 );
    shape.lineTo( -25*cos( 4 * M_PI / 3), 25*sin( 4 * M_PI / 3 ) );
    shape.closeSubpath();
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, Left, "arrowHeadWipe", "left", false) );
}

KPrArrowHeadWipeEffectFactory::~KPrArrowHeadWipeEffectFactory()
{
}

static const char* s_subTypes[] = {
    I18N_NOOP( "Up" ),
    I18N_NOOP( "Right" ),
    I18N_NOOP( "Down" ),
    I18N_NOOP( "Left" )
};

QString KPrArrowHeadWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return i18n( s_subTypes[subType] );
    } else {
        return i18n( "Unknown subtype" );
    }
}
