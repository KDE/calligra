/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "BarnZigZagWipeEffectFactory.h"
#include "BarnZigZagWipeStrategy.h"

#include <klocalizedstring.h>

#define BarnZigZagWipeEffectId "BarnZigZagWipeEffect"

BarnZigZagWipeEffectFactory::BarnZigZagWipeEffectFactory()
: KPrPageEffectFactory( BarnZigZagWipeEffectId, i18n( "Barn Zig Zag" ) )
{
    addStrategy( new BarnZigZagWipeStrategy( Vertical, "vertical", false ) );
    addStrategy( new BarnZigZagWipeStrategy( Horizontal, "horizontal", false ) );
    addStrategy( new BarnZigZagWipeStrategy( VerticalReversed, "vertical", true ) );
    addStrategy( new BarnZigZagWipeStrategy( HorizontalReversed, "horizontal", true ) );
}

BarnZigZagWipeEffectFactory::~BarnZigZagWipeEffectFactory()
{
}

static const char* const s_subTypes[] = {
    I18N_NOOP( "Vertical" ),
    I18N_NOOP( "Horizontal" ),
    I18N_NOOP( "Vertical Reversed" ),
    I18N_NOOP( "Horizontal Reversed" )
};

QString BarnZigZagWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return i18n( s_subTypes[subType] );
    } else {
        return i18n( "Unknown subtype" );
    }
}
