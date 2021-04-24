/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrPinWheelWipeEffectFactory.h"

#include <klocalizedstring.h>

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

static const char* const s_subTypes[] = {
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
