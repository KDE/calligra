/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "FourBoxWipeEffectFactory.h"

#include <klocalizedstring.h>

#include "CornersInWipeStrategy.h"
#include "CornersOutWipeStrategy.h"

#define FourBoxWipeEffectId "FourBoxWipeEffect"

FourBoxWipeEffectFactory::FourBoxWipeEffectFactory()
: KPrPageEffectFactory( FourBoxWipeEffectId, i18n( "Four Box" ) )
{
    addStrategy( new CornersInWipeStrategy(false) );
    addStrategy( new CornersInWipeStrategy(true) );
    addStrategy( new CornersOutWipeStrategy(false) );
    addStrategy( new CornersOutWipeStrategy(true) );
}

FourBoxWipeEffectFactory::~FourBoxWipeEffectFactory()
{
}

static const char* const s_subTypes[] = {
    I18N_NOOP( "Corners In" ),
    I18N_NOOP( "Corners In Reverse" ),
    I18N_NOOP( "Corners Out" ),
    I18N_NOOP( "Corners Out Reverse" )
};

QString FourBoxWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return i18n( s_subTypes[subType] );
    } else {
        return i18n( "Unknown subtype" );
    }
}
