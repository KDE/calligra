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
: KPrPageEffectFactory( PinWheelWipeEffectId, i18n( "Pin Wheel Wipe Effect" ) )
{
    addStrategy( new KPrClockWipeStrategy( 270, 2, KPrPageEffect::TwoBladeVerticalClockwise, "pinWheelWipe", "twoBladeVertical", false ) );
    addStrategy( new KPrClockWipeStrategy( 0, 2, KPrPageEffect::TwoBladeHorizontalClockwise, "pinWheelWipe", "twoBladeHorizontal", false ) );
    addStrategy( new KPrClockWipeStrategy( 0, 4, KPrPageEffect::FourBladeClockwise, "pinWheelWipe", "fourBlade", false ) );
    addStrategy( new KPrClockWipeStrategy( 270, 2, KPrPageEffect::TwoBladeVerticalCounterClockwise, "pinWheelWipe", "twoBladeVertical", true ) );
    addStrategy( new KPrClockWipeStrategy( 0, 2, KPrPageEffect::TwoBladeHorizontalCounterClockwise, "pinWheelWipe", "twoBladeHorizontal", true ) );
    addStrategy( new KPrClockWipeStrategy( 0, 4, KPrPageEffect::FourBladeCounterClockwise, "pinWheelWipe", "fourBlade", true ) );
}

KPrPinWheelWipeEffectFactory::~KPrPinWheelWipeEffectFactory()
{
}

