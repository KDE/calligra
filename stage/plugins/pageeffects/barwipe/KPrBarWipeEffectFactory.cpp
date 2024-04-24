/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Timothee Lacroix <dakeyras.khan@gmail.com>
   SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrBarWipeEffectFactory.h"

#include <klocalizedstring.h>
#include <klazylocalizedstring.h>

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

static const KLazyLocalizedString s_subTypes[] = {
    kli18n( "From Left" ),
    kli18n( "From Top" ),
    kli18n( "From Right" ),
    kli18n( "From Bottom" )
};

QString KPrBarWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return s_subTypes[subType].toString();
    } else {
        return i18n( "Unknown subtype" );
    }
}
