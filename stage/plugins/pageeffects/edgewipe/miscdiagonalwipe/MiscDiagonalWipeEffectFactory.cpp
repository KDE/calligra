/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "MiscDiagonalWipeEffectFactory.h"

#include <klocalizedstring.h>

#include "DoubleBarnDoorWipeStrategy.h"
#include "DoubleDiamondWipeStrategy.h"

#define MiscDiagonalWipeEffectId "MiscDiagonalWipeEffect"

MiscDiagonalWipeEffectFactory::MiscDiagonalWipeEffectFactory()
: KPrPageEffectFactory( MiscDiagonalWipeEffectId, i18n( "Misc Diagonal" ) )
{
    addStrategy( new DoubleBarnDoorWipeStrategy() );
    addStrategy( new DoubleDiamondWipeStrategy() );
}

MiscDiagonalWipeEffectFactory::~MiscDiagonalWipeEffectFactory()
{
}

static const char* const s_subTypes[] = {
    I18N_NOOP( "Double Barn Door" ),
    I18N_NOOP( "Double Diamond" ),
};

QString MiscDiagonalWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return i18n( s_subTypes[subType] );
    } else {
        return i18n( "Unknown subtype" );
    }
}
