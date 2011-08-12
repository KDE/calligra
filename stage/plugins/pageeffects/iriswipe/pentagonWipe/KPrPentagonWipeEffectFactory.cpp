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

#include "KPrPentagonWipeEffectFactory.h"

#include <cmath>

#include <klocale.h>

#include "../KPrIrisWipeEffectStrategyBase.h"

#define PentagonEffectFactoryId "PentagonEffectFactory"

KPrPentagonWipeEffectFactory::KPrPentagonWipeEffectFactory()
: KPrPageEffectFactory( PentagonEffectFactoryId, i18n("Pentagon") )
{
    QPainterPath shape;

    //up
    shape.moveTo( 25*cos( M_PI/2 ), -25*sin( M_PI/2 ) );
    shape.lineTo( 25*cos( 9 * M_PI / 10 ), -25*sin( 9 * M_PI / 10 ) );
    shape.lineTo( 25*cos( 13 * M_PI / 10 ),-25*sin( 13 * M_PI / 10 ) );
    shape.lineTo( 25*cos( 17 * M_PI / 10 ),-25*sin( 17 * M_PI / 10 ) );
    shape.lineTo( 25*cos( M_PI / 10 ), -25*sin( M_PI / 10 ) );
    shape.closeSubpath();
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, Up, "pentagonWipe", "up", false ) );

    //up reverse
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, UpReverse, "pentagonWipe", "up", true ) );

    //down
    shape = QPainterPath();
    shape.moveTo( 25*cos( M_PI/2 ), 25*sin( M_PI/2 ) );
    shape.lineTo( 25*cos( 9 * M_PI / 10 ), 25*sin( 9 * M_PI / 10 ) );
    shape.lineTo( 25*cos( 13 * M_PI / 10 ),25*sin( 13 * M_PI / 10 ) );
    shape.lineTo( 25*cos( 17 * M_PI / 10 ),25*sin( 17 * M_PI / 10 ) );
    shape.lineTo( 25*cos( M_PI / 10 ), 25*sin( M_PI / 10 ) );
    shape.closeSubpath();
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, Down, "pentagonWipe", "down", false ) );

    //down reverse
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, DownReverse, "pentagonWipe", "down", true ) );
}

KPrPentagonWipeEffectFactory::~KPrPentagonWipeEffectFactory()
{
}

static const char* s_subTypes[] = {
    I18N_NOOP( "Up" ),
    I18N_NOOP( "Up Reverse" ),
    I18N_NOOP( "Down" ),
    I18N_NOOP( "Down Reverse" )
};

QString KPrPentagonWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return i18n( s_subTypes[subType] );
    } else {
        return i18n( "Unknown subtype" );
    }
}
