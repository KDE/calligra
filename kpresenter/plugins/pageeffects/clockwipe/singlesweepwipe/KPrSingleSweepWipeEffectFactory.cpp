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
: KPrPageEffectFactory( SingleSweepWipeEffectId, i18n( "Single Sweep Wipe Effect" ) )
{
    addStrategy( new KPrSingleSweepWipeStrategy( 0.5, 0.0, 0, -180,
                                                 KPrPageEffect::ClockwiseTop, "singleSweepWipe", "clockwiseTop", false ) );
    addStrategy( new KPrSingleSweepWipeStrategy( 1.0, 0.5, 270, -180,
                                                 KPrPageEffect::ClockwiseRight, "singleSweepWipe", "clockwiseRight", false ) );
    addStrategy( new KPrSingleSweepWipeStrategy( 0.5, 1.0, 180, -180,
                                                 KPrPageEffect::ClockwiseBottom, "singleSweepWipe", "clockwiseBottom", false ) );
    addStrategy( new KPrSingleSweepWipeStrategy( 0.0, 0.5, 90, -180,
                                                 KPrPageEffect::ClockwiseLeft, "singleSweepWipe", "clockwiseLeft", false ) );
    addStrategy( new KPrSingleSweepWipeStrategy( 0.0, 0.0, 0, -90,
                                                 KPrPageEffect::ClockwiseTopLeft, "singleSweepWipe", "clockwiseTopLeft", false ) );
    addStrategy( new KPrSingleSweepWipeStrategy( 0.0, 1.0, 0, 90,
                                                 KPrPageEffect::CounterClockwiseBottomLeft, "singleSweepWipe", "counterClockwiseBottomLeft", false ) );
    addStrategy( new KPrSingleSweepWipeStrategy( 1.0, 1.0, 180, -90,
                                                 KPrPageEffect::ClockwiseBottomRight, "singleSweepWipe", "clockwiseBottomRight", false ) );
    addStrategy( new KPrSingleSweepWipeStrategy( 1.0, 0.0, 180, 90,
                                                 KPrPageEffect::CounterClockwiseTopRight, "singleSweepWipe", "counterClockwiseTopRight", false ) );
    addStrategy( new KPrSingleSweepWipeStrategy( 0.5, 0.0, 180, 180,
                                                 KPrPageEffect::CounterClockwiseTop, "singleSweepWipe", "clockwiseTop", true ) );
    addStrategy( new KPrSingleSweepWipeStrategy( 1.0, 0.5, 90, 180,
                                                 KPrPageEffect::CounterClockwiseRight, "singleSweepWipe", "clockwiseRight", true ) );
    addStrategy( new KPrSingleSweepWipeStrategy( 0.5, 1.0, 0, 180,
                                                 KPrPageEffect::CounterClockwiseBottom, "singleSweepWipe", "clockwiseBottom", true ) );
    addStrategy( new KPrSingleSweepWipeStrategy( 0.0, 0.5, 270, 180,
                                                 KPrPageEffect::CounterClockwiseLeft, "singleSweepWipe", "clockwiseLeft", true ) );
    addStrategy( new KPrSingleSweepWipeStrategy( 0.0, 0.0, 270, 90,
                                                 KPrPageEffect::CounterClockwiseTopLeft, "singleSweepWipe", "clockwiseTopLeft", true ) );
    addStrategy( new KPrSingleSweepWipeStrategy( 0.0, 1.0, 90, -90,
                                                 KPrPageEffect::ClockwiseBottomLeft, "singleSweepWipe", "counterClockwiseBottomLeft", true ) );
    addStrategy( new KPrSingleSweepWipeStrategy( 1.0, 1.0, 90, 90,
                                                 KPrPageEffect::CounterClockwiseBottomRight, "singleSweepWipe", "clockwiseBottomRight", true ) );
    addStrategy( new KPrSingleSweepWipeStrategy( 1.0, 0.0, 270, -90,
                                                 KPrPageEffect::ClockwiseTopRight, "singleSweepWipe", "counterClockwiseTopRight", true ) );
}

KPrSingleSweepWipeEffectFactory::~KPrSingleSweepWipeEffectFactory()
{
}

