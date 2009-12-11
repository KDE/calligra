/* This file is part of the KDE project
   Copyright (C) 2008 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>

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

#include "KPrBoxSnakesWipeEffectFactory.h"

#include <klocale.h>

#include "KPrBoxSnakesWipeStrategy.h"

#define BoxSnakesWipeEffectId "BoxSnakesWipeEffect"

KPrBoxSnakesWipeEffectFactory::KPrBoxSnakesWipeEffectFactory()
: KPrPageEffectFactory( BoxSnakesWipeEffectId, i18n( "Box Snakes" ) )
{
    addStrategy( new KPrBoxSnakesWipeStrategy(2, 1, true, false) );
    addStrategy( new KPrBoxSnakesWipeStrategy(2, 1, false, false) );
    addStrategy( new KPrBoxSnakesWipeStrategy(1, 2, true, false) );
    addStrategy( new KPrBoxSnakesWipeStrategy(1, 2, false, false) );
    addStrategy( new KPrBoxSnakesWipeStrategy(2, 2, true, false) );
    addStrategy( new KPrBoxSnakesWipeStrategy(2, 2, false, false) );
    addStrategy( new KPrBoxSnakesWipeStrategy(2, 1, true, true) );
    addStrategy( new KPrBoxSnakesWipeStrategy(2, 1, false, true) );
    addStrategy( new KPrBoxSnakesWipeStrategy(1, 2, true, true) );
    addStrategy( new KPrBoxSnakesWipeStrategy(1, 2, false, true) );
    addStrategy( new KPrBoxSnakesWipeStrategy(2, 2, true, true) );
    addStrategy( new KPrBoxSnakesWipeStrategy(2, 2, false, true) );
}

KPrBoxSnakesWipeEffectFactory::~KPrBoxSnakesWipeEffectFactory()
{
}

static const char* s_subTypes[] = {
    I18N_NOOP( "Two Box Top In" ),
    I18N_NOOP( "Two Box Bottom In" ),
    I18N_NOOP( "Two Box Left In" ),
    I18N_NOOP( "Two Box Right In" ),
    I18N_NOOP( "Four Box Vertical In" ),
    I18N_NOOP( "Four Box Horizontal In" ),
    I18N_NOOP( "Two Box Top Out" ),
    I18N_NOOP( "Two Box Bottom Out" ),
    I18N_NOOP( "Two Box Left Out" ),
    I18N_NOOP( "Two Box Right Out" ),
    I18N_NOOP( "Four Box Vertical Out" ),
    I18N_NOOP( "Four Box Horizontal Out" )
};

QString KPrBoxSnakesWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return i18n( s_subTypes[subType] );
    } else {
        return i18n( "Unknown subtype" );
    }
}
