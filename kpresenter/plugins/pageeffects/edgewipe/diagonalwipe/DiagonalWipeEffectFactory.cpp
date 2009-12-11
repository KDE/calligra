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

#include "DiagonalWipeEffectFactory.h"
#include <klocale.h>

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

static const char* s_subTypes[] = {
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
