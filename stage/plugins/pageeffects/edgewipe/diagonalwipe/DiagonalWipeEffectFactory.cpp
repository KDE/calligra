/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "DiagonalWipeEffectFactory.h"

#include <klocalizedstring.h>

#include "DiagonalWipeStrategy.h"

#define DiagonalWipeEffectId "DiagonalWipeEffect"

DiagonalWipeEffectFactory::DiagonalWipeEffectFactory()
: KPrPageEffectFactory( DiagonalWipeEffectId, i18n( "Diagonal" ) )
{
    addStrategy( new DiagonalWipeStrategy( FromTopLeft, "topLeft", false ) );
    addStrategy( new DiagonalWipeStrategy( FromBottomRight, "topLeft", true ) );
    addStrategy( new DiagonalWipeStrategy( FromTopRight, "topRight", false ) );
    addStrategy( new DiagonalWipeStrategy( FromBottomLeft, "topRight", true ) );
}

DiagonalWipeEffectFactory::~DiagonalWipeEffectFactory()
{
}

static const char* const s_subTypes[] = {
    I18N_NOOP( "From Top Left" ),
    I18N_NOOP( "From Bottom Right" ),
    I18N_NOOP( "From Top Right" ),
    I18N_NOOP( "From Bottom Left" )
};

QString DiagonalWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return i18n( s_subTypes[subType] );
    } else {
        return i18n( "Unknown subtype" );
    }
}
