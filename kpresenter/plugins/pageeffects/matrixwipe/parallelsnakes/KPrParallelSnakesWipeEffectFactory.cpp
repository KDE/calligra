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

#include "KPrParallelSnakesWipeEffectFactory.h"

#include <klocale.h>

#include "KPrParallelSnakesWipeVerticalStrategy.h"
#include "KPrParallelSnakesWipeHorizontalStrategy.h"
#include "KPrParallelSnakesWipeDiagonalStrategy.h"

#define ParallelSnakesWipeEffectId "ParallelSnakesWipeEffect"

KPrParallelSnakesWipeEffectFactory::KPrParallelSnakesWipeEffectFactory()
    : KPrPageEffectFactory( ParallelSnakesWipeEffectId, i18n( "Parallel Snakes" ) )
{
    addStrategy( new KPrParallelSnakesWipeVerticalStrategy(false, false, false) );
    addStrategy( new KPrParallelSnakesWipeVerticalStrategy(true, true, false) );
    addStrategy( new KPrParallelSnakesWipeVerticalStrategy(false, true, false) );
    addStrategy( new KPrParallelSnakesWipeVerticalStrategy(true, false, false) );
    addStrategy( new KPrParallelSnakesWipeVerticalStrategy(false, false, true) );
    addStrategy( new KPrParallelSnakesWipeVerticalStrategy(true, true, true) );
    addStrategy( new KPrParallelSnakesWipeVerticalStrategy(false, true, true) );
    addStrategy( new KPrParallelSnakesWipeVerticalStrategy(true, false, true) );
    addStrategy( new KPrParallelSnakesWipeHorizontalStrategy(false, false, false) );
    addStrategy( new KPrParallelSnakesWipeHorizontalStrategy(true, true, false) );
    addStrategy( new KPrParallelSnakesWipeHorizontalStrategy(false, true, false) );
    addStrategy( new KPrParallelSnakesWipeHorizontalStrategy(true, false, false) );
    addStrategy( new KPrParallelSnakesWipeHorizontalStrategy(false, false, true) );
    addStrategy( new KPrParallelSnakesWipeHorizontalStrategy(true, true, true) );
    addStrategy( new KPrParallelSnakesWipeHorizontalStrategy(false, true, true) );
    addStrategy( new KPrParallelSnakesWipeHorizontalStrategy(true, false, true) );
    addStrategy( new KPrParallelSnakesWipeDiagonalStrategy(false, true) );
    addStrategy( new KPrParallelSnakesWipeDiagonalStrategy(true, true) );
    addStrategy( new KPrParallelSnakesWipeDiagonalStrategy(false, false) );
    addStrategy( new KPrParallelSnakesWipeDiagonalStrategy(true, false) );
}

KPrParallelSnakesWipeEffectFactory::~KPrParallelSnakesWipeEffectFactory()
{
}

static const char* s_subTypes[] = {
    I18N_NOOP( "Vertical Top Same In" ),
    I18N_NOOP( "Vertical Top Same Out" ),
    I18N_NOOP( "Vertical Bottom Same In" ),
    I18N_NOOP( "Vertical Bottom Same Out" ),
    I18N_NOOP( "Vertical Top Left Opposite In" ),
    I18N_NOOP( "Vertical Top Left Opposite Out" ),
    I18N_NOOP( "Vertical Bottom Left Opposite In" ),
    I18N_NOOP( "Vertical Bottom Left Opposite Out" ),
    I18N_NOOP( "Horizontal Left Same In" ),
    I18N_NOOP( "Horizontal Left Same Out" ),
    I18N_NOOP( "Horizontal Right Same In" ),
    I18N_NOOP( "Horizontal Right Same Out" ),
    I18N_NOOP( "Horizontal Top Left Opposite In" ),
    I18N_NOOP( "Horizontal Top Left Opposite Out" ),
    I18N_NOOP( "Horizontal Top Right Opposite In" ),
    I18N_NOOP( "Horizontal Top Right Opposite Out" ),
    I18N_NOOP( "Diagonal Bottom Left Opposite In" ),
    I18N_NOOP( "Diagonal Bottom Left Opposite Out" ),
    I18N_NOOP( "Diagonal Top Left Opposite In" ),
    I18N_NOOP( "Diagonal Top Left Opposite Out" )
};

QString KPrParallelSnakesWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return i18n( s_subTypes[subType] );
    } else {
        return i18n( "Unknown subtype" );
    }
}
