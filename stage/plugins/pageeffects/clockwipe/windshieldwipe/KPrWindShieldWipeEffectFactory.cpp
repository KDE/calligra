/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrWindShieldWipeEffectFactory.h"

#include <klocalizedstring.h>

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

static const char* const s_subTypes[] = {
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
