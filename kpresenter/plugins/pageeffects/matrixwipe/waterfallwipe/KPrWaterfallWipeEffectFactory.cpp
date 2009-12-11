/* This file is part of the KDE project
   Copyright (C) 2008 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>

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

#include "KPrWaterfallWipeEffectFactory.h"

#include <klocale.h>

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

static const char* s_subTypes[] = {
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

