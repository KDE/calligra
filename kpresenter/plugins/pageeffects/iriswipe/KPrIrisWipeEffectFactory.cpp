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

#include <klocale.h>

#include "KPrIrisWipeEffectFactory.h"
#include "KPrIrisWipeEffectStrategyBase.h"

#define IrisWipeEffectId "IrisWipeEffect"

KPrIrisWipeEffectFactory::KPrIrisWipeEffectFactory()
: KPrPageEffectFactory( IrisWipeEffectId, i18n("Iris Wipe Effect") )
{
    QPainterPath shape;

    shape.addRect( -50, -50, 100, 100 );
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, Rectangle, "irisWipe", "rectangle", false) );

    shape = QPainterPath();
    shape.moveTo( 0, -50);
    shape.lineTo( 50, 0 );
    shape.lineTo( 0, 50 );
    shape.lineTo(-50, 0 );
    shape.closeSubpath();
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, Diamond, "irisWipe", "diamond", false) );
}

KPrIrisWipeEffectFactory::~KPrIrisWipeEffectFactory()
{
}

static const char* s_subTypes[] = {
    I18N_NOOP( "Rectangular" ),
    I18N_NOOP( "Diamond" )
};

QString KPrIrisWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return i18n( s_subTypes[subType] );
    } else {
        return i18n( "Unknown subtype" );
    }
}
