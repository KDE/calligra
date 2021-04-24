/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ZigZagWipeEffectFactory.h"
#include "ZigZagWipeStrategy.h"

#include <klocalizedstring.h>

#define ZigZagWipeEffectId "ZigZagWipeEffect"

ZigZagWipeEffectFactory::ZigZagWipeEffectFactory()
: KPrPageEffectFactory( ZigZagWipeEffectId, i18n( "Zig Zag" ) )
{
    addStrategy( new ZigZagWipeStrategy( FromLeft, "leftToRight", false ) );
    addStrategy( new ZigZagWipeStrategy( FromTop, "topToBottom", false ) );
    addStrategy( new ZigZagWipeStrategy( FromRight, "leftToRight", true ) );
    addStrategy( new ZigZagWipeStrategy( FromBottom, "topToBottom", true ) );
}

ZigZagWipeEffectFactory::~ZigZagWipeEffectFactory()
{
}

static const char* const s_subTypes[] = {
    I18N_NOOP( "From Left" ),
    I18N_NOOP( "From Top" ),
    I18N_NOOP( "From Right" ),
    I18N_NOOP( "From Bottom" )
};

QString ZigZagWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return i18n( s_subTypes[subType] );
    } else {
        return i18n( "Unknown subtype" );
    }
}
