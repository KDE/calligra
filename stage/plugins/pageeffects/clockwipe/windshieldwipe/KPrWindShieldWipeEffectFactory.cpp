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

#include "KPrWindShieldWipeEffectFactory.h"
#include <klocale.h>

#include "KPrWindShieldWipeStrategy.h"

#define WindShieldWipeEffectId  "WindShieldWipeEffect"

KPrWindShieldWipeEffectFactory::KPrWindShieldWipeEffectFactory()
: KPrPageEffectFactory( WindShieldWipeEffectId, i18n( "Windshield" ) )
{
    addStrategy( new KPrWindShieldWipeStrategy( Right, "windshieldWipe", "right", false ) );
    addStrategy( new KPrWindShieldWipeStrategy( Up, "windshieldWipe", "up", false ) );
    addStrategy( new KPrWindShieldWipeStrategy( Vertical, "windshieldWipe", "vertical", false ) );
    addStrategy( new KPrWindShieldWipeStrategy( Horizontal, "windshieldWipe", "horizontal", false ) );

    addStrategy( new KPrWindShieldWipeStrategy( RightReverse, "windshieldWipe", "right", true ) );
    addStrategy( new KPrWindShieldWipeStrategy( UpReverse, "windshieldWipe", "up", true ) );
    addStrategy( new KPrWindShieldWipeStrategy( VerticalReverse, "windshieldWipe", "vertical", true ) );
    addStrategy( new KPrWindShieldWipeStrategy( HorizontalReverse, "windshieldWipe", "horizontal", true ) );
}

KPrWindShieldWipeEffectFactory::~KPrWindShieldWipeEffectFactory()
{
}

static const char* s_subTypes[] = {
    I18N_NOOP( "Right" ),
    I18N_NOOP( "Up" ),
    I18N_NOOP( "Vertical" ),
    I18N_NOOP( "Horizontal" ),
    I18N_NOOP( "Right Reverse" ),
    I18N_NOOP( "Up Reverse" ),
    I18N_NOOP( "Vertical Reverse" ),
    I18N_NOOP( "Horizontal Reverse" )
};

QString KPrWindShieldWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return i18n( s_subTypes[subType] );
    } else {
        return i18n( "Unknown subtype" );
    }
}
