/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "BarnDoorWipeEffectFactory.h"

#include <klocalizedstring.h>

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

static const char* const s_subTypes[] = {
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
