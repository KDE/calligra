/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrSaloonDoorWipeEffectFactory.h"

#include <klocalizedstring.h>

#include "KPrSaloonDoorWipeStrategy.h"

#define SaloonDoorWipeEffectId  "SaloonDoorWipeEffect"

KPrSaloonDoorWipeEffectFactory::KPrSaloonDoorWipeEffectFactory()
: KPrPageEffectFactory( SaloonDoorWipeEffectId, i18n( "Saloon Door" ) )
{
    addStrategy( new KPrSaloonDoorWipeStrategy( FromTop, "saloonDoorWipe", "top", false ) );
    addStrategy( new KPrSaloonDoorWipeStrategy( FromLeft, "saloonDoorWipe", "left", false ) );
    addStrategy( new KPrSaloonDoorWipeStrategy( FromBottom, "saloonDoorWipe", "bottom", false ) );
    addStrategy( new KPrSaloonDoorWipeStrategy( FromRight, "saloonDoorWipe", "right", false ) );

    addStrategy( new KPrSaloonDoorWipeStrategy( ToTop, "saloonDoorWipe", "top", true ) );
    addStrategy( new KPrSaloonDoorWipeStrategy( ToLeft, "saloonDoorWipe", "left", true ) );
    addStrategy( new KPrSaloonDoorWipeStrategy( ToBottom, "saloonDoorWipe", "bottom", true ) );
    addStrategy( new KPrSaloonDoorWipeStrategy( ToRight, "saloonDoorWipe", "right", true ) );
}

KPrSaloonDoorWipeEffectFactory::~KPrSaloonDoorWipeEffectFactory()
{
}

static const char* const s_subTypes[] = {
    I18N_NOOP( "From Top" ),
    I18N_NOOP( "From Left" ),
    I18N_NOOP( "From Bottom" ),
    I18N_NOOP( "From Right" ),
    I18N_NOOP( "To Top" ),
    I18N_NOOP( "To Left" ),
    I18N_NOOP( "To Bottom" ),
    I18N_NOOP( "To Right" )
};

QString KPrSaloonDoorWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return i18n( s_subTypes[subType] );
    } else {
        return i18n( "Unknown subtype" );
    }
}
