/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007-2008 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrSlideWipeEffectFactory.h"

#include <klocalizedstring.h>

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
: KPrPageEffectFactory( SlideWipeEffectId, i18n( "Slide" ) )
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

static const char* const s_subTypes[] = {
    I18N_NOOP( "From Left" ),
    I18N_NOOP( "From Right" ),
    I18N_NOOP( "From Top" ),
    I18N_NOOP( "From Bottom" ),
    I18N_NOOP( "To Left" ),
    I18N_NOOP( "To Right" ),
    I18N_NOOP( "To Top" ),
    I18N_NOOP( "To Bottom" )
};

QString KPrSlideWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return i18n( s_subTypes[subType] );
    } else {
        return i18n( "Unknown subtype" );
    }
}
