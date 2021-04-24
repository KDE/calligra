/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrSnakeWipeEffectFactory.h"

#include <klocalizedstring.h>

#include "KPrSnakeWipeBottomRightHorizontalStrategy.h"
#include "KPrSnakeWipeBottomRightVerticalStrategy.h"
#include "KPrSnakeWipeTopLeftHorizontalStrategy.h"
#include "KPrSnakeWipeTopLeftVerticalStrategy.h"
#include "KPrSnakeWipeTopLeftDiagonalStrategy.h"
#include "KPrSnakeWipeTopRightDiagonalStrategy.h"
#include "KPrSnakeWipeBottomLeftDiagonalStrategy.h"
#include "KPrSnakeWipeBottomRightDiagonalStrategy.h"

#define SnakeWipeEffectId "SnakeWipeEffect"

KPrSnakeWipeEffectFactory::KPrSnakeWipeEffectFactory()
: KPrPageEffectFactory( SnakeWipeEffectId, i18n( "Snake" ) )
{
    addStrategy( new KPrSnakeWipeTopLeftHorizontalStrategy() );
    addStrategy( new KPrSnakeWipeBottomRightHorizontalStrategy() );
    addStrategy( new KPrSnakeWipeTopLeftVerticalStrategy() );
    addStrategy( new KPrSnakeWipeBottomRightVerticalStrategy() );
    addStrategy( new KPrSnakeWipeTopLeftDiagonalStrategy() );
    addStrategy( new KPrSnakeWipeTopRightDiagonalStrategy() );
    addStrategy( new KPrSnakeWipeBottomRightDiagonalStrategy() );
    addStrategy( new KPrSnakeWipeBottomLeftDiagonalStrategy() );
}

KPrSnakeWipeEffectFactory::~KPrSnakeWipeEffectFactory()
{
}

static const char* const s_subTypes[] = {
    I18N_NOOP( "From Left" ),
    I18N_NOOP( "From Right" ),
    I18N_NOOP( "From Top" ),
    I18N_NOOP( "From Bottom" ),
    I18N_NOOP( "From Top Left" ),
    I18N_NOOP( "From Top Right" ),
    I18N_NOOP( "From Bottom Left" ),
    I18N_NOOP( "From Bottom Right" )
};

QString KPrSnakeWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return i18n( s_subTypes[subType] );
    } else {
        return i18n( "Unknown subtype" );
    }
}
