/* This file is part of the KDE project
   Copyright (C) 2007-2008 Thorsten Zachmann <zachmann@kde.org>

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

#include "KPrSlideWipeEffectFactory.h"

#include <klocale.h>

#include "KPrSlideWipeFromBottomStrategy.h"
#include "KPrSlideWipeFromLeftStrategy.h"
#include "KPrSlideWipeFromRightStrategy.h"
#include "KPrSlideWipeFromTopStrategy.h"
#include "KPrSlideWipeToBottomStrategy.h"
#include "KPrSlideWipeToLeftStrategy.h"
#include "KPrSlideWipeToRightStrategy.h"
#include "KPrSlideWipeToTopStrategy.h"

#define SlideWipeEffectId "SlideWipeEffect"

KPrSlideWipeEffectFactory::KPrSlideWipeEffectFactory()
: KPrPageEffectFactory( SlideWipeEffectId, i18n( "Slide Wipe Effect" ) )
{
    addStrategy( new KPrSlideWipeFromTopStrategy() );
    addStrategy( new KPrSlideWipeFromBottomStrategy() );
    addStrategy( new KPrSlideWipeFromLeftStrategy() );
    addStrategy( new KPrSlideWipeFromRightStrategy() );
    addStrategy( new KPrSlideWipeToTopStrategy() );
    addStrategy( new KPrSlideWipeToBottomStrategy() );
    addStrategy( new KPrSlideWipeToLeftStrategy() );
    addStrategy( new KPrSlideWipeToRightStrategy() );
}

KPrSlideWipeEffectFactory::~KPrSlideWipeEffectFactory()
{
}
