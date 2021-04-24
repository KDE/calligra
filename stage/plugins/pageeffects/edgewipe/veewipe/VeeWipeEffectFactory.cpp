/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "VeeWipeEffectFactory.h"

#include <klocalizedstring.h>

#include "VeeWipeStrategy.h"

#define VeeWipeEffectId "VeeWipeEffect"

VeeWipeEffectFactory::VeeWipeEffectFactory()
: KPrPageEffectFactory( VeeWipeEffectId, i18n( "Vee" ) )
{
    addStrategy( new VeeWipeStrategy( FromTop, "down", false ) );
    addStrategy( new VeeWipeStrategy( FromRight, "left", false ) );
    addStrategy( new VeeWipeStrategy( FromBottom, "top", false ) );
    addStrategy( new VeeWipeStrategy( FromLeft, "right", false ) );
}

VeeWipeEffectFactory::~VeeWipeEffectFactory()
{
}

static const char* const s_subTypes[] = {
    I18N_NOOP( "From Top" ),
    I18N_NOOP( "From Right" ),
    I18N_NOOP( "From Bottom" ),
    I18N_NOOP( "From Left" )
};

QString VeeWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return i18n( s_subTypes[subType] );
    } else {
        return i18n( "Unknown subtype" );
    }
}
