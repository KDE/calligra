/* This file is part of the KDE project
   Copyright (C) 2008 Sven Langkamp <sven.langkamp@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KPrSingleSweepWipeEffectFactory.h"
#include <klocale.h>

#include "KPrSingleSweepWipeStrategy.h"

#define SingleSweepWipeEffectId  "SingleSweepWipeEffect"

KPrSingleSweepWipeEffectFactory::KPrSingleSweepWipeEffectFactory()
: KPrPageEffectFactory( SingleSweepWipeEffectId, i18n( "Single Sweep" ) )
{
    addStrategy( new KPrSingleSweepWipeStrategy( 0.5, 0.0, 0, -180,
                                                 ClockwiseTop, "singleSweepWipe", "clockwiseTop", false ) );
    addStrategy( new KPrSingleSweepWipeStrategy( 1.0, 0.5, 270, -180,
                                                 ClockwiseRight, "singleSweepWipe", "clockwiseRight", false ) );
    addStrategy( new KPrSingleSweepWipeStrategy( 0.5, 1.0, 180, -180,
                                                 ClockwiseBottom, "singleSweepWipe", "clockwiseBottom", false ) );
    addStrategy( new KPrSingleSweepWipeStrategy( 0.0, 0.5, 90, -180,
                                                 ClockwiseLeft, "singleSweepWipe", "clockwiseLeft", false ) );
    addStrategy( new KPrSingleSweepWipeStrategy( 0.0, 0.0, 0, -90,
                                                 ClockwiseTopLeft, "singleSweepWipe", "clockwiseTopLeft", false ) );
    addStrategy( new KPrSingleSweepWipeStrategy( 0.0, 1.0, 0, 90,
                                                 CounterClockwiseBottomLeft, "singleSweepWipe", "counterClockwiseBottomLeft", false ) );
    addStrategy( new KPrSingleSweepWipeStrategy( 1.0, 1.0, 180, -90,
                                                 ClockwiseBottomRight, "singleSweepWipe", "clockwiseBottomRight", false ) );
    addStrategy( new KPrSingleSweepWipeStrategy( 1.0, 0.0, 180, 90,
                                                 CounterClockwiseTopRight, "singleSweepWipe", "counterClockwiseTopRight", false ) );
    addStrategy( new KPrSingleSweepWipeStrategy( 0.5, 0.0, 180, 180,
                                                 CounterClockwiseTop, "singleSweepWipe", "clockwiseTop", true ) );
    addStrategy( new KPrSingleSweepWipeStrategy( 1.0, 0.5, 90, 180,
                                                 CounterClockwiseRight, "singleSweepWipe", "clockwiseRight", true ) );
    addStrategy( new KPrSingleSweepWipeStrategy( 0.5, 1.0, 0, 180,
                                                 CounterClockwiseBottom, "singleSweepWipe", "clockwiseBottom", true ) );
    addStrategy( new KPrSingleSweepWipeStrategy( 0.0, 0.5, 270, 180,
                                                 CounterClockwiseLeft, "singleSweepWipe", "clockwiseLeft", true ) );
    addStrategy( new KPrSingleSweepWipeStrategy( 0.0, 0.0, 270, 90,
                                                 CounterClockwiseTopLeft, "singleSweepWipe", "clockwiseTopLeft", true ) );
    addStrategy( new KPrSingleSweepWipeStrategy( 0.0, 1.0, 90, -90,
                                                 ClockwiseBottomLeft, "singleSweepWipe", "counterClockwiseBottomLeft", true ) );
    addStrategy( new KPrSingleSweepWipeStrategy( 1.0, 1.0, 90, 90,
                                                 CounterClockwiseBottomRight, "singleSweepWipe", "clockwiseBottomRight", true ) );
    addStrategy( new KPrSingleSweepWipeStrategy( 1.0, 0.0, 270, -90,
                                                 ClockwiseTopRight, "singleSweepWipe", "counterClockwiseTopRight", true ) );
}

KPrSingleSweepWipeEffectFactory::~KPrSingleSweepWipeEffectFactory()
{
}

static const char* s_subTypes[] = {
    I18N_NOOP( "Clockwise Top" ),
    I18N_NOOP( "Clockwise Right" ),
    I18N_NOOP( "Clockwise Bottom" ),
    I18N_NOOP( "Clockwise Left" ),
    I18N_NOOP( "Clockwise Top Left" ),
    I18N_NOOP( "Counterclockwise Bottom Left" ),
    I18N_NOOP( "Clockwise Bottom Right" ),
    I18N_NOOP( "Counterclockwise Top Right" ),
    I18N_NOOP( "Counterclockwise Top" ),
    I18N_NOOP( "Counterclockwise Right" ),
    I18N_NOOP( "Counterclockwise Bottom" ),
    I18N_NOOP( "Counterclockwise Left" ),
    I18N_NOOP( "Counterclockwise Top Left" ),
    I18N_NOOP( "Clockwise Bottom Left" ),
    I18N_NOOP( "Counterclockwise Bottom Right" ),
    I18N_NOOP( "Clockwise Top Right" )
};

QString KPrSingleSweepWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return i18n( s_subTypes[subType] );
    } else {
        return i18n( "Unknown subtype" );
    }
}
