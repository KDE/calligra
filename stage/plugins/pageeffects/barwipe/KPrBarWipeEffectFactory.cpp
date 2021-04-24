/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Timothee Lacroix <dakeyras.khan@gmail.com>
   SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrBarWipeEffectFactory.h"

#include <klocalizedstring.h>

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

static const char* const s_subTypes[] = {
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
