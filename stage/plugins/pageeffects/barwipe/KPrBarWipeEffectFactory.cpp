/* This file is part of the KDE project
   Copyright (C) 2007 Timothee Lacroix <dakeyras.khan@gmail.com>
   Copyright (C) 2008 Thorsten Zachmann <zachmann@kde.org>

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

#include "KPrBarWipeEffectFactory.h"

#include <klocale.h>

#include "KPrBarWipeFromTopStrategy.h"
#include "KPrBarWipeFromLeftStrategy.h"
#include "KPrBarWipeFromBottomStrategy.h"
#include "KPrBarWipeFromRightStrategy.h"

#define BarWipeEffectId "BarWipeEffect"

KPrBarWipeEffectFactory::KPrBarWipeEffectFactory()
: KPrPageEffectFactory( BarWipeEffectId, i18n( "Bar" ) )
{
    addStrategy( new KPrBarWipeFromTopStrategy() );
    addStrategy( new KPrBarWipeFromBottomStrategy() );
    addStrategy( new KPrBarWipeFromLeftStrategy() );
    addStrategy( new KPrBarWipeFromRightStrategy() );
}

KPrBarWipeEffectFactory::~KPrBarWipeEffectFactory()
{
}

static const char* s_subTypes[] = {
    I18N_NOOP( "From Left" ),
    I18N_NOOP( "From Top" ),
    I18N_NOOP( "From Right" ),
    I18N_NOOP( "From Bottom" )
};

QString KPrBarWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return i18n( s_subTypes[subType] );
    } else {
        return i18n( "Unknown subtype" );
    }
}
