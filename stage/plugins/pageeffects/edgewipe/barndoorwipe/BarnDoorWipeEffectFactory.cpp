/* This file is part of the KDE project
 * Copyright (C) 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "BarnDoorWipeEffectFactory.h"
#include <klocale.h>

#include "BarnDoorWipeStrategy.h"

#define BarnDoorWipeEffectId "BarnDoorWipeEffect"

BarnDoorWipeEffectFactory::BarnDoorWipeEffectFactory()
: KPrPageEffectFactory( BarnDoorWipeEffectId, i18n( "Barn Door" ) )
{
    addStrategy( new BarnDoorWipeStrategy( Vertical, "vertical", false ) );
    addStrategy( new BarnDoorWipeStrategy( VerticalReverse, "vertical", true ) );
    addStrategy( new BarnDoorWipeStrategy( Horizontal, "horizontal", false ) );
    addStrategy( new BarnDoorWipeStrategy( HorizontalReverse, "horizontal", true ) );
    addStrategy( new BarnDoorWipeStrategy( DiagonalBottomLeft, "diagonalBottomLeft", false ) );
    addStrategy( new BarnDoorWipeStrategy( DiagonalBottomLeftReverse, "diagonalBottomLeft", true ) );
    addStrategy( new BarnDoorWipeStrategy( DiagonalTopLeft, "diagonalTopLeft", false ) );
    addStrategy( new BarnDoorWipeStrategy( DiagonalTopLeftReverse, "diagonalTopLeft", true ) );
}

BarnDoorWipeEffectFactory::~BarnDoorWipeEffectFactory()
{
}

static const char* s_subTypes[] = {
    I18N_NOOP( "Vertical" ),
    I18N_NOOP( "Vertical Reverse" ),
    I18N_NOOP( "Horizontal" ),
    I18N_NOOP( "Horizontal Reverse" ),
    I18N_NOOP( "Diagonal Bottom Left" ),
    I18N_NOOP( "Diagonal Bottom Left Reverse" ),
    I18N_NOOP( "Diagonal Top Left" ),
    I18N_NOOP( "Diagonal Top Left Reverse" )
};

QString BarnDoorWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return i18n( s_subTypes[subType] );
    } else {
        return i18n( "Unknown subtype" );
    }
}
