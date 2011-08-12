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

#include "KPrPinWheelWipeEffectFactory.h"
#include <klocale.h>

#include "KPrClockWipeStrategy.h"

#define PinWheelWipeEffectId  "PinWheelWipeEffect"

KPrPinWheelWipeEffectFactory::KPrPinWheelWipeEffectFactory()
: KPrPageEffectFactory( PinWheelWipeEffectId, i18n( "Pin Wheel" ) )
{
    addStrategy( new KPrClockWipeStrategy( 270, 2, TwoBladeVerticalClockwise, "pinWheelWipe", "twoBladeVertical", false ) );
    addStrategy( new KPrClockWipeStrategy( 0, 2, TwoBladeHorizontalClockwise, "pinWheelWipe", "twoBladeHorizontal", false ) );
    addStrategy( new KPrClockWipeStrategy( 0, 4, FourBladeClockwise, "pinWheelWipe", "fourBlade", false ) );
    addStrategy( new KPrClockWipeStrategy( 270, 2, TwoBladeVerticalCounterClockwise, "pinWheelWipe", "twoBladeVertical", true ) );
    addStrategy( new KPrClockWipeStrategy( 0, 2, TwoBladeHorizontalCounterClockwise, "pinWheelWipe", "twoBladeHorizontal", true ) );
    addStrategy( new KPrClockWipeStrategy( 0, 4, FourBladeCounterClockwise, "pinWheelWipe", "fourBlade", true ) );
}

KPrPinWheelWipeEffectFactory::~KPrPinWheelWipeEffectFactory()
{
}

static const char* s_subTypes[] = {
    I18N_NOOP( "Two Blades Vertical Clockwise" ),
    I18N_NOOP( "Two Blades Horizontal Clockwise" ),
    I18N_NOOP( "Four Blades Horizontal Clockwise" ),
    I18N_NOOP( "Two Blades Vertical Counterclockwise" ),
    I18N_NOOP( "Two Blades Horizontal Counterclockwise" ),
    I18N_NOOP( "Four Blades Horizontal Counterclockwise" )
};

QString KPrPinWheelWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return i18n( s_subTypes[subType] );
    } else {
        return i18n( "Unknown subtype" );
    }
}
