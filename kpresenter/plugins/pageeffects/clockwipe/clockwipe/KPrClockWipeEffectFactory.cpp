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

#include "KPrClockWipeEffectFactory.h"
#include <klocale.h>

#include "KPrClockWipeStrategy.h"

#define ClockWipeEffectId "ClockWipeEffect"

KPrClockWipeEffectFactory::KPrClockWipeEffectFactory()
: KPrPageEffectFactory( ClockWipeEffectId, i18n( "Clock Wipe Effect" ) )
{
    addStrategy( new KPrClockWipeStrategy( 90, 1, KPrPageEffect::FromTwelveClockwise, "clockWipe", "clockwiseTwelve", false ) );
    addStrategy( new KPrClockWipeStrategy( 0, 1, KPrPageEffect::FromThreeClockwise, "clockWipe", "clockwiseThree", false ) );
    addStrategy( new KPrClockWipeStrategy( 270, 1, KPrPageEffect::FromSixClockwise, "clockWipe", "clockwiseSix", false ) );
    addStrategy( new KPrClockWipeStrategy( 180, 1, KPrPageEffect::FromNineClockwise, "clockWipe", "clockwiseNine", false ) );
    addStrategy( new KPrClockWipeStrategy( 90, 1, KPrPageEffect::FromTwelveCounterClockwise, "clockWipe", "clockwiseTwelve", true ) );
    addStrategy( new KPrClockWipeStrategy( 0, 1, KPrPageEffect::FromThreeCounterClockwise, "clockWipe", "clockwiseThree", true ) );
    addStrategy( new KPrClockWipeStrategy( 270, 1, KPrPageEffect::FromSixCounterClockwise, "clockWipe", "clockwiseSix", true ) );
    addStrategy( new KPrClockWipeStrategy( 180, 1, KPrPageEffect::FromNineCounterClockwise, "clockWipe", "clockwiseNine", true ) );
}

KPrClockWipeEffectFactory::~KPrClockWipeEffectFactory()
{
}

