/* This file is part of the KDE project
   Copyright (C) 2008 Sven Langkamp <sven.langkamp@gmail.com>

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

#include "KPrDoubleSweepWipeEffectFactory.h"
#include <klocale.h>

#include "KPrDoubleSweepWipeStrategy.h"

#define DoubleSweepWipeEffectId  "DoubleSweepWipeEffect"

KPrDoubleSweepWipeEffectFactory::KPrDoubleSweepWipeEffectFactory()
: KPrPageEffectFactory( DoubleSweepWipeEffectId, i18n( "Double Sweep Wipe Effect" ) )
{
    addStrategy( new KPrDoubleSweepWipeStrategy( KPrPageEffect::ParallelVertical, "doubleSweepWipe", "parallelVertical", false ) );
    addStrategy( new KPrDoubleSweepWipeStrategy( KPrPageEffect::ParallelDiagonal, "doubleSweepWipe", "parallelDiagonal", false ) );
    addStrategy( new KPrDoubleSweepWipeStrategy( KPrPageEffect::OppositeVertical, "doubleSweepWipe", "oppositeVertical", false ) );
    addStrategy( new KPrDoubleSweepWipeStrategy( KPrPageEffect::OppositeHorizontal, "doubleSweepWipe", "oppositeHorizontal", false ) );
    addStrategy( new KPrDoubleSweepWipeStrategy( KPrPageEffect::ParallelDiagonalTopLeft, "doubleSweepWipe", "parallelDiagonalTopLeft", false ) );
    addStrategy( new KPrDoubleSweepWipeStrategy( KPrPageEffect::ParallelDiagonalBottomLeft, "doubleSweepWipe", "parallelDiagonalBottomLeft", false ) );
    addStrategy( new KPrDoubleSweepWipeStrategy( KPrPageEffect::ParallelVerticalReverse, "doubleSweepWipe", "parallelVertical", true ) );
    addStrategy( new KPrDoubleSweepWipeStrategy( KPrPageEffect::ParallelDiagonalReverse, "doubleSweepWipe", "parallelDiagonal", true ) );
    addStrategy( new KPrDoubleSweepWipeStrategy( KPrPageEffect::OppositeVerticalReverse, "doubleSweepWipe", "oppositeVertical", true ) );
    addStrategy( new KPrDoubleSweepWipeStrategy( KPrPageEffect::OppositeHorizontalReverse, "doubleSweepWipe", "oppositeHorizontal", true ) );
    addStrategy( new KPrDoubleSweepWipeStrategy( KPrPageEffect::ParallelDiagonalTopLeftReverse, "doubleSweepWipe", "parallelDiagonalTopLeft", true ) );
    addStrategy( new KPrDoubleSweepWipeStrategy( KPrPageEffect::ParallelDiagonalBottomLeftReverse, "doubleSweepWipe", "parallelDiagonalBottomLeft", true ) );
}

KPrDoubleSweepWipeEffectFactory::~KPrDoubleSweepWipeEffectFactory()
{
}

