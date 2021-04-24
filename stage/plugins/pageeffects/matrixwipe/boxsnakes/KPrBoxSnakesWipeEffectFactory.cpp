/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrBoxSnakesWipeEffectFactory.h"

#include <klocalizedstring.h>

#include "KPrBoxSnakesWipeStrategy.h"

#define BoxSnakesWipeEffectId "BoxSnakesWipeEffect"

KPrBoxSnakesWipeEffectFactory::KPrBoxSnakesWipeEffectFactory()
: KPrPageEffectFactory( BoxSnakesWipeEffectId, i18n( "Box Snakes" ) )
{
    addStrategy( new KPrBoxSnakesWipeStrategy(2, 1, true, false) );
    addStrategy( new KPrBoxSnakesWipeStrategy(2, 1, false, false) );
    addStrategy( new KPrBoxSnakesWipeStrategy(1, 2, true, false) );
    addStrategy( new KPrBoxSnakesWipeStrategy(1, 2, false, false) );
    addStrategy( new KPrBoxSnakesWipeStrategy(2, 2, true, false) );
    addStrategy( new KPrBoxSnakesWipeStrategy(2, 2, false, false) );
    addStrategy( new KPrBoxSnakesWipeStrategy(2, 1, true, true) );
    addStrategy( new KPrBoxSnakesWipeStrategy(2, 1, false, true) );
    addStrategy( new KPrBoxSnakesWipeStrategy(1, 2, true, true) );
    addStrategy( new KPrBoxSnakesWipeStrategy(1, 2, false, true) );
    addStrategy( new KPrBoxSnakesWipeStrategy(2, 2, true, true) );
    addStrategy( new KPrBoxSnakesWipeStrategy(2, 2, false, true) );
}

KPrBoxSnakesWipeEffectFactory::~KPrBoxSnakesWipeEffectFactory()
{
}

static const char* const s_subTypes[] = {
    I18N_NOOP( "Two Box Top In" ),
    I18N_NOOP( "Two Box Bottom In" ),
    I18N_NOOP( "Two Box Left In" ),
    I18N_NOOP( "Two Box Right In" ),
    I18N_NOOP( "Four Box Vertical In" ),
    I18N_NOOP( "Four Box Horizontal In" ),
    I18N_NOOP( "Two Box Top Out" ),
    I18N_NOOP( "Two Box Bottom Out" ),
    I18N_NOOP( "Two Box Left Out" ),
    I18N_NOOP( "Two Box Right Out" ),
    I18N_NOOP( "Four Box Vertical Out" ),
    I18N_NOOP( "Four Box Horizontal Out" )
};

QString KPrBoxSnakesWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return i18n( s_subTypes[subType] );
    } else {
        return i18n( "Unknown subtype" );
    }
}
