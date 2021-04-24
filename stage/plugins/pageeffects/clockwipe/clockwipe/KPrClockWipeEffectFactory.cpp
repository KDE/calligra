/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrClockWipeEffectFactory.h"

#include <klocalizedstring.h>

#include "KPrClockWipeStrategy.h"

#define ClockWipeEffectId "ClockWipeEffect"

KPrClockWipeEffectFactory::KPrClockWipeEffectFactory()
: KPrPageEffectFactory( ClockWipeEffectId, i18n( "Clock" ) )
{
    addStrategy( new KPrClockWipeStrategy( 90, 1, FromTwelveClockwise, "clockWipe", "clockwiseTwelve", false ) );
    addStrategy( new KPrClockWipeStrategy( 0, 1, FromThreeClockwise, "clockWipe", "clockwiseThree", false ) );
    addStrategy( new KPrClockWipeStrategy( 270, 1, FromSixClockwise, "clockWipe", "clockwiseSix", false ) );
    addStrategy( new KPrClockWipeStrategy( 180, 1, FromNineClockwise, "clockWipe", "clockwiseNine", false ) );
    addStrategy( new KPrClockWipeStrategy( 90, 1, FromTwelveCounterClockwise, "clockWipe", "clockwiseTwelve", true ) );
    addStrategy( new KPrClockWipeStrategy( 0, 1, FromThreeCounterClockwise, "clockWipe", "clockwiseThree", true ) );
    addStrategy( new KPrClockWipeStrategy( 270, 1, FromSixCounterClockwise, "clockWipe", "clockwiseSix", true ) );
    addStrategy( new KPrClockWipeStrategy( 180, 1, FromNineCounterClockwise, "clockWipe", "clockwiseNine", true ) );
}

KPrClockWipeEffectFactory::~KPrClockWipeEffectFactory()
{
}

static const char* const s_subTypes[] = {
    I18N_NOOP( "From Twelve Clockwise" ),
    I18N_NOOP( "From Three Clockwise" ),
    I18N_NOOP( "From Six Clockwise" ),
    I18N_NOOP( "From Nine Clockwise" ),
    I18N_NOOP( "From Twelve Counterclockwise" ),
    I18N_NOOP( "From Three Counterclockwise" ),
    I18N_NOOP( "From Six Counterclockwise" ),
    I18N_NOOP( "From Nine Counterclockwise" )
};

QString KPrClockWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return i18n( s_subTypes[subType] );
    } else {
        return i18n( "Unknown subtype" );
    }
}

