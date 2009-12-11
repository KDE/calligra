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

#include "KPrIrisWipeEffectFactory.h"

//KDE includes
#include <klocale.h>

#include "../KPrIrisWipeEffectStrategyBase.h"

#define IrisWipeEffectId "IrisWipeEffect"

KPrIrisWipeEffectFactory::KPrIrisWipeEffectFactory()
: KPrPageEffectFactory( IrisWipeEffectId, i18n("Iris") )
{

     QPainterPath shape;

    //rectangle
    shape.addRect( -25, -25, 50, 50 );
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, Rectangle, "irisWipe", "rectangle", false ) );

    //rectangle reverse
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, RectangleReverse, "irisWipe", "rectangle", true ) );

    //diamond
    shape = QPainterPath();
    shape.moveTo( 0, -25);
    shape.lineTo( 25, 0 );
    shape.lineTo( 0, 25 );
    shape.lineTo(-25, 0 );
    shape.closeSubpath();
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, Diamond, "irisWipe", "diamond", false ) );

    //diamond reverse
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, DiamondReverse, "irisWipe", "diamond", true ) );
}

KPrIrisWipeEffectFactory::~KPrIrisWipeEffectFactory()
{
}

static const char* s_subTypes[] = {
    I18N_NOOP( "Rectangular" ),
    I18N_NOOP( "Rectangular Reverse" ),
    I18N_NOOP( "Diamond" ),
    I18N_NOOP( "Diamond Reverse" )
};

QString KPrIrisWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return i18n( s_subTypes[subType] );
    } else {
        return i18n( "Unknown subtype" );
    }
}
