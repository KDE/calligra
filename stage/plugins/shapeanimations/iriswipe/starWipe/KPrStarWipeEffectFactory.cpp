
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

#include "KPrStarWipeEffectFactory.h"

#include <cmath>

#include <klocale.h>

#include "../KPrIrisWipeEffectStrategyBase.h"

using std::sin;
using std::cos;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define StarWipeEffectId "StarWipeEffect"

KPrStarWipeEffectFactory::KPrStarWipeEffectFactory()
: KPrPageEffectFactory( StarWipeEffectId, i18n("Star") )
{

     QPainterPath shape;

    //fourPoint
    shape = QPainterPath();
    shape.moveTo( 0, -24 );
    shape.lineTo( 12*cos( 3 * M_PI / 4 ), -12*sin( 3 * M_PI / 4 ) );
    shape.lineTo( 24*cos( M_PI ), -24*sin( M_PI ) );
    shape.lineTo( 12*cos( 5 * M_PI / 4 ), -12*sin( 5 * M_PI / 4 ) );
    shape.lineTo( 24*cos( 6 * M_PI / 4 ), -24*sin( 6 * M_PI / 4 ) );
    shape.lineTo( 12*cos( 7 * M_PI / 4 ), -12*sin( 7 * M_PI / 4 ) );
    shape.lineTo( 24*cos( 2 * M_PI ), -24*sin( 2 * M_PI ) );
    shape.lineTo( 12*cos( M_PI / 4 ), -12*sin( M_PI / 4 ) );
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, FourPoint, "starWipe", "fourPoint", false ) );

    //fourPoint reverse
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, FourPointReverse, "starWipe", "fourPoint", true ) );

    //fivePoint
    shape = QPainterPath();
    shape.moveTo( 0, -24 );
    shape.lineTo( 12*cos( 7 * M_PI / 10 ), -12*sin( 7 * M_PI / 10 ) );
    shape.lineTo( 24*cos( 9 * M_PI / 10 ), -24*sin( 9 * M_PI / 10 ) );
    shape.lineTo( 12*cos( 11 * M_PI / 10 ), -12*sin( 11 * M_PI / 10 ) );
    shape.lineTo( 24*cos( 13 * M_PI / 10 ), -24*sin( 13 * M_PI / 10 ) );
    shape.lineTo( 12*cos( 15 * M_PI / 10 ), -12*sin( 15 * M_PI / 10 ) );
    shape.lineTo( 24*cos( 17 * M_PI / 10 ), -24*sin( 17 * M_PI / 10 ) );
    shape.lineTo( 12*cos( 19 * M_PI / 10 ), -12*sin( 19 * M_PI / 10 ) );
    shape.lineTo( 24*cos( 1 * M_PI / 10 ), -24*sin( 1 * M_PI / 10 ) );
    shape.lineTo( 12*cos( 3 * M_PI / 10 ), -12*sin( 3 * M_PI / 10 ) );
    shape.closeSubpath();
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, FivePoint, "starWipe", "fivePoint", false ) );

    //fivePoint reverse
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, FivePointReverse, "starWipe", "fivePoint", true ) );

    //sixPoint
    shape = QPainterPath();
    shape.moveTo( 0, -24 );
    shape.lineTo( 12*cos( 4 * M_PI / 6 ), -12*sin( 4 * M_PI / 6 ) );
    shape.lineTo( 24*cos( 5 * M_PI / 6 ), -24*sin( 5 * M_PI / 6 ) );
    shape.lineTo( 12*cos( M_PI ), -12*sin( M_PI ) );
    shape.lineTo( 24*cos( 7 * M_PI / 6 ), -24*sin( 7 * M_PI / 6 ) );
    shape.lineTo( 12*cos( 8 * M_PI / 6 ), -12*sin( 8 * M_PI / 6 ) );
    shape.lineTo( 24*cos( 9 * M_PI / 6 ), -24*sin( 9 * M_PI / 6 ) );
    shape.lineTo( 12*cos( 10 * M_PI / 6 ), -12*sin( 10 * M_PI / 6 ) );
    shape.lineTo( 24*cos( 11 * M_PI / 6 ), -24*sin( 11 * M_PI / 6 ) );
    shape.lineTo( 12*cos( 2 * M_PI ), -12*sin( 2 * M_PI ) );
    shape.lineTo( 24*cos( 1 * M_PI / 6 ), -24*sin( 1 * M_PI / 6 ) );
    shape.lineTo( 12*cos( 2 * M_PI / 6 ), -12*sin( 2 * M_PI / 6 ) );
    shape.closeSubpath();
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, SixPoint, "starWipe", "sixPoint", false ) );

    //sixPoint reverse
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, SixPointReverse, "starWipe", "sixPoint", true ) );
}

KPrStarWipeEffectFactory::~KPrStarWipeEffectFactory()
{
}

static const char* s_subTypes[] = {
    I18N_NOOP( "Four Point Star" ),
    I18N_NOOP( "Four Point Star Reverse" ),
    I18N_NOOP( "Five Point Star" ),
    I18N_NOOP( "Five Point Star Reverse" ),
    I18N_NOOP( "Six Point Star" ),
    I18N_NOOP( "Six Point Star Reverse" )
};

QString KPrStarWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return i18n( s_subTypes[subType] );
    } else {
        return i18n( "Unknown subtype" );
    }
}
