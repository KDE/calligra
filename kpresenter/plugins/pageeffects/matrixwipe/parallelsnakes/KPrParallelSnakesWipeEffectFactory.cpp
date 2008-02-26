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
    : KPrPageEffectFactory( ParallelSnakesWipeEffectId, i18n( "Parallel Snakes Wipe Effect" ) )
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

