/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrWaterfallWipeEffectFactory.h"

#include <klocalizedstring.h>

#include "KPrWaterfallWipeTopLeftStrategy.h"
#include "KPrWaterfallWipeTopRightStrategy.h"
#include "KPrWaterfallWipeBottomLeftStrategy.h"
#include "KPrWaterfallWipeBottomRightStrategy.h"

#define WaterfallWipeEffectId "WaterfallWipeEffect"

KPrWaterfallWipeEffectFactory::KPrWaterfallWipeEffectFactory()
    : KPrPageEffectFactory( WaterfallWipeEffectId, i18n( "Waterfall" ) )
{
    addStrategy( new KPrWaterfallWipeTopLeftStrategy(KPrMatrixWipeStrategy::TopToBottom) );
    addStrategy( new KPrWaterfallWipeTopLeftStrategy(KPrMatrixWipeStrategy::LeftToRight) );
    addStrategy( new KPrWaterfallWipeTopRightStrategy(KPrMatrixWipeStrategy::TopToBottom) );
    addStrategy( new KPrWaterfallWipeTopRightStrategy(KPrMatrixWipeStrategy::RightToLeft) );
    addStrategy( new KPrWaterfallWipeBottomLeftStrategy(KPrMatrixWipeStrategy::BottomToTop) );
    addStrategy( new KPrWaterfallWipeBottomLeftStrategy(KPrMatrixWipeStrategy::LeftToRight) );
    addStrategy( new KPrWaterfallWipeBottomRightStrategy(KPrMatrixWipeStrategy::BottomToTop) );
    addStrategy( new KPrWaterfallWipeBottomRightStrategy(KPrMatrixWipeStrategy::RightToLeft) );
}

KPrWaterfallWipeEffectFactory::~KPrWaterfallWipeEffectFactory()
{
}

static const char* const s_subTypes[] = {
    I18N_NOOP( "Top Left Vertical" ),
    I18N_NOOP( "Top Left Horizontal" ),
    I18N_NOOP( "Top Right Vertical" ),
    I18N_NOOP( "Top Right Horizontal" ),
    I18N_NOOP( "Bottom Left Vertical" ),
    I18N_NOOP( "Bottom Left Horizontal" ),
    I18N_NOOP( "Bottom Right Vertical" ),
    I18N_NOOP( "Bottom Right Horizontal" )
};

QString KPrWaterfallWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return i18n( s_subTypes[subType] );
    } else {
        return i18n( "Unknown subtype" );
    }
}

