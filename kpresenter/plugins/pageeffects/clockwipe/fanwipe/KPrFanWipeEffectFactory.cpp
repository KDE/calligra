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

#include "KPrFanWipeEffectFactory.h"
#include <klocale.h>

#include "KPrCenterFanWipeStrategy.h"
#include "KPrSideFanWipeStrategy.h"

#define FanWipeEffectId  "FanWipeEffect"

KPrFanWipeEffectFactory::KPrFanWipeEffectFactory()
: KPrPageEffectFactory( FanWipeEffectId, i18n( "Fan Wipe Effect" ) )
{
    addStrategy( new KPrCenterFanWipeStrategy( 0, 1, KPrPageEffect::CenterRight, "fanWipe", "centerRight", false ) );
    addStrategy( new KPrCenterFanWipeStrategy( 90, 1, KPrPageEffect::CenterTop, "fanWipe", "centerTop", false ) );
    addStrategy( new KPrCenterFanWipeStrategy( 180, 1, KPrPageEffect::CenterLeft, "fanWipe", "centerRight", true ) );
    addStrategy( new KPrCenterFanWipeStrategy( 270, 1, KPrPageEffect::CenterBottom, "fanWipe", "centerTop", true ) );

    addStrategy( new KPrSideFanWipeStrategy( 90, 1, KPrPageEffect::FanOutTop, "fanWipe", "top", false ) );
    addStrategy( new KPrSideFanWipeStrategy( 0, 1, KPrPageEffect::FanOutRight, "fanWipe", "right", false ) );
    addStrategy( new KPrSideFanWipeStrategy( 270, 1, KPrPageEffect::FanOutBottom, "fanWipe", "bottom", false ) );
    addStrategy( new KPrSideFanWipeStrategy( 180, 1, KPrPageEffect::FanOutLeft, "fanWipe", "left", false ) );
    addStrategy( new KPrSideFanWipeStrategy( 90, 1, KPrPageEffect::FanInTop, "fanWipe", "top", true ) );
    addStrategy( new KPrSideFanWipeStrategy( 0, 1, KPrPageEffect::FanInRight, "fanWipe", "right", true ) );
    addStrategy( new KPrSideFanWipeStrategy( 270, 1, KPrPageEffect::FanInBottom, "fanWipe", "bottom", true ) );
    addStrategy( new KPrSideFanWipeStrategy( 180, 1, KPrPageEffect::FanInLeft, "fanWipe", "left", true ) );
}

KPrFanWipeEffectFactory::~KPrFanWipeEffectFactory()
{
}

