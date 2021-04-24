/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "BoxWipeEffectFactory.h"

#include <klocalizedstring.h>

#include "BoxWipeStrategy.h"

#define BoxWipeEffectId "BoxWipeEffect"

BoxWipeEffectFactory::BoxWipeEffectFactory()
: KPrPageEffectFactory( BoxWipeEffectId, i18n( "Box" ) )
{
    addStrategy( new BoxWipeStrategy( FromTopLeft, "topLeft", false ) );
    addStrategy( new BoxWipeStrategy( FromTopRight, "topRight", false ) );
    addStrategy( new BoxWipeStrategy( FromBottomRight, "bottomRight", false ) );
    addStrategy( new BoxWipeStrategy( FromBottomLeft, "bottomLeft", false ) );
    addStrategy( new BoxWipeStrategy( CenterTop, "topCenter", false ) );
    addStrategy( new BoxWipeStrategy( CenterRight, "rightCenter", false ) );
    addStrategy( new BoxWipeStrategy( CenterBottom, "bottomCenter", false ) );
    addStrategy( new BoxWipeStrategy( CenterLeft, "leftCenter", false ) );
}

BoxWipeEffectFactory::~BoxWipeEffectFactory()
{
}

static const char* const s_subTypes[] = {
    I18N_NOOP( "From Top Left" ),
    I18N_NOOP( "From Top Right" ),
    I18N_NOOP( "From Bottom Left" ),
    I18N_NOOP( "From Bottom Right" ),
    I18N_NOOP( "Center Top" ),
    I18N_NOOP( "Center Right" ),
    I18N_NOOP( "Center Bottom" ),
    I18N_NOOP( "Center Left" )
};

QString BoxWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return i18n( s_subTypes[subType] );
    } else {
        return i18n( "Unknown subtype" );
    }
}
