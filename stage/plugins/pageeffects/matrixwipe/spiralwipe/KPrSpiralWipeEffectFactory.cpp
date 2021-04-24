/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrSpiralWipeEffectFactory.h"

#include <klocalizedstring.h>

#include "KPrSpiralWipeStrategy.h"

#define SpiralWipeEffectId "SpiralWipeEffect"

KPrSpiralWipeEffectFactory::KPrSpiralWipeEffectFactory()
: KPrPageEffectFactory( SpiralWipeEffectId, i18n( "Spiral" ) )
{
    addStrategy( new KPrSpiralWipeStrategy(0, true, false) );
    addStrategy( new KPrSpiralWipeStrategy(1, true, false) );
    addStrategy( new KPrSpiralWipeStrategy(2, true, false) );
    addStrategy( new KPrSpiralWipeStrategy(3, true, false) );
    addStrategy( new KPrSpiralWipeStrategy(0, false, false) );
    addStrategy( new KPrSpiralWipeStrategy(3, false, false) );
    addStrategy( new KPrSpiralWipeStrategy(2, false, false) );
    addStrategy( new KPrSpiralWipeStrategy(1, false, false) );
    addStrategy( new KPrSpiralWipeStrategy(0, true, true) );
    addStrategy( new KPrSpiralWipeStrategy(1, true, true) );
    addStrategy( new KPrSpiralWipeStrategy(2, true, true) );
    addStrategy( new KPrSpiralWipeStrategy(3, true, true) );
    addStrategy( new KPrSpiralWipeStrategy(0, false, true) );
    addStrategy( new KPrSpiralWipeStrategy(3, false, true) );
    addStrategy( new KPrSpiralWipeStrategy(2, false, true) );
    addStrategy( new KPrSpiralWipeStrategy(1, false, true) );
}

KPrSpiralWipeEffectFactory::~KPrSpiralWipeEffectFactory()
{
}

static const char* const s_subTypes[] = {
    I18N_NOOP( "Clockwise Top Left In" ),
    I18N_NOOP( "Clockwise Top Left Out" ),
    I18N_NOOP( "Clockwise Top Right In" ),
    I18N_NOOP( "Clockwise Top Right Out" ),
    I18N_NOOP( "Clockwise Bottom Left In" ),
    I18N_NOOP( "Clockwise Bottom Left Out" ),
    I18N_NOOP( "Clockwise Bottom Right In" ),
    I18N_NOOP( "Clockwise Bottom Right Out" ),
    I18N_NOOP( "Counterclockwise Top Left In" ),
    I18N_NOOP( "Counterclockwise Top Left Out" ),
    I18N_NOOP( "Counterclockwise Top Right In" ),
    I18N_NOOP( "Counterclockwise Top Right Out" ),
    I18N_NOOP( "Counterclockwise Bottom Left In" ),
    I18N_NOOP( "Counterclockwise Bottom Left Out" ),
    I18N_NOOP( "Counterclockwise Bottom Right In" ),
    I18N_NOOP( "Counterclockwise Bottom Right Out" )
};

QString KPrSpiralWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return i18n( s_subTypes[subType] );
    } else {
        return i18n( "Unknown subtype" );
    }
}

