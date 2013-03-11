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

#include "KPrDoubleFanWipeEffectFactory.h"
#include <klocale.h>

#include "KPrCenterFanWipeStrategy.h"
#include "KPrSideFanWipeStrategy.h"

#define DoubleFanWipeEffectId  "DoubleFanWipeEffect"

KPrDoubleFanWipeEffectFactory::KPrDoubleFanWipeEffectFactory()
: KPrPageEffectFactory( DoubleFanWipeEffectId, i18n( "Double Fan" ) )
{
    addStrategy( new KPrCenterFanWipeStrategy( 90, 2, FanOutVertical, "doubleFanWipe", "fanOutVertical", false ) );
    addStrategy( new KPrCenterFanWipeStrategy( 0, 2, FanOutHorizontal, "doubleFanWipe", "fanOutHorizontal", false ) );
    addStrategy( new KPrSideFanWipeStrategy( 90, 2, FanInVertical, "doubleFanWipe", "fanInVertical", false ) );
    addStrategy( new KPrSideFanWipeStrategy( 180, 2, FanInHorizontal, "doubleFanWipe", "fanInHorizontal", false ) );
    addStrategy( new KPrSideFanWipeStrategy( 90, 2, FanInVerticalReverse, "doubleFanWipe", "fanInVertical", true ) );
    addStrategy( new KPrSideFanWipeStrategy( 180, 2, FanInHorizontalReverse, "doubleFanWipe", "fanInHorizontal", true ) );
}

KPrDoubleFanWipeEffectFactory::~KPrDoubleFanWipeEffectFactory()
{
}

static const char* s_subTypes[] = {
    I18N_NOOP( "Fan Out Vertical" ),
    I18N_NOOP( "Fan Out Horizontal" ),
    I18N_NOOP( "Fan In Vertical" ),
    I18N_NOOP( "Fan In Horizontal" ),
    I18N_NOOP( "Fan In Vertical Reverse" ),
    I18N_NOOP( "Fan In Horizontal Reverse" )
};

QString KPrDoubleFanWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return i18n( s_subTypes[subType] );
    } else {
        return i18n( "Unknown subtype" );
    }
}

