/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrDoubleFanWipeEffectFactory.h"

#include <klocalizedstring.h>

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

static const char* const s_subTypes[] = {
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

