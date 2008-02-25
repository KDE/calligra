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

#define DoubleFanWipeEffectId  "DoubleFanWipeEffect"

KPrDoubleFanWipeEffectFactory::KPrDoubleFanWipeEffectFactory()
: KPrPageEffectFactory( DoubleFanWipeEffectId, i18n( "Double Fan Wipe Effect" ) )
{
    addStrategy( new KPrCenterFanWipeStrategy( 90, 2, KPrPageEffect::FanOutVertical, "doubleFanWipe", "fanOutVertical", false ) );
    addStrategy( new KPrCenterFanWipeStrategy( 0, 2, KPrPageEffect::FanOutHorizontal, "doubleFanWipe", "fanOutHorizontal", false ) );
}

KPrDoubleFanWipeEffectFactory::~KPrDoubleFanWipeEffectFactory()
{
}

