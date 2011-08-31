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
: KPrPageEffectFactory( FanWipeEffectId, i18n( "Fan" ) )
{
    addStrategy( new KPrCenterFanWipeStrategy( 0, 1, CenterRight, "fanWipe", "centerRight", false ) );
    addStrategy( new KPrCenterFanWipeStrategy( 90, 1, CenterTop, "fanWipe", "centerTop", false ) );
    addStrategy( new KPrCenterFanWipeStrategy( 180, 1, CenterLeft, "fanWipe", "centerRight", true ) );
    addStrategy( new KPrCenterFanWipeStrategy( 270, 1, CenterBottom, "fanWipe", "centerTop", true ) );

    addStrategy( new KPrSideFanWipeStrategy( 90, 1, FanOutTop, "fanWipe", "top", false ) );
    addStrategy( new KPrSideFanWipeStrategy( 0, 1, FanOutRight, "fanWipe", "right", false ) );
    addStrategy( new KPrSideFanWipeStrategy( 270, 1, FanOutBottom, "fanWipe", "bottom", false ) );
    addStrategy( new KPrSideFanWipeStrategy( 180, 1, FanOutLeft, "fanWipe", "left", false ) );
    addStrategy( new KPrSideFanWipeStrategy( 90, 1, FanInTop, "fanWipe", "top", true ) );
    addStrategy( new KPrSideFanWipeStrategy( 0, 1, FanInRight, "fanWipe", "right", true ) );
    addStrategy( new KPrSideFanWipeStrategy( 270, 1, FanInBottom, "fanWipe", "bottom", true ) );
    addStrategy( new KPrSideFanWipeStrategy( 180, 1, FanInLeft, "fanWipe", "left", true ) );
}

KPrFanWipeEffectFactory::~KPrFanWipeEffectFactory()
{
}

static const char* s_subTypes[] = {
    I18N_NOOP( "Center Right" ),
    I18N_NOOP( "Center Top" ),
    I18N_NOOP( "Center Left" ),
    I18N_NOOP( "Center Bottom" ),
    I18N_NOOP( "Fan Out Top" ),
    I18N_NOOP( "Fan Out Right" ),
    I18N_NOOP( "Fan Out Bottom" ),
    I18N_NOOP( "Fan Out Left" ),
    I18N_NOOP( "Fan In Top" ),
    I18N_NOOP( "Fan In Right" ),
    I18N_NOOP( "Fan In Bottom" ),
    I18N_NOOP( "Fan In Left" )
};

QString KPrFanWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return i18n( s_subTypes[subType] );
    } else {
        return i18n( "Unknown subtype" );
    }
}
