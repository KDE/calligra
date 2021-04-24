/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrDoubleSweepWipeEffectFactory.h"

#include <klocalizedstring.h>

#include "KPrDoubleSweepWipeStrategy.h"

#define DoubleSweepWipeEffectId  "DoubleSweepWipeEffect"

KPrDoubleSweepWipeEffectFactory::KPrDoubleSweepWipeEffectFactory()
: KPrPageEffectFactory( DoubleSweepWipeEffectId, i18n( "Double Sweep" ) )
{
    addStrategy( new KPrDoubleSweepWipeStrategy( ParallelVertical, "doubleSweepWipe", "parallelVertical", false ) );
    addStrategy( new KPrDoubleSweepWipeStrategy( ParallelDiagonal, "doubleSweepWipe", "parallelDiagonal", false ) );
    addStrategy( new KPrDoubleSweepWipeStrategy( OppositeVertical, "doubleSweepWipe", "oppositeVertical", false ) );
    addStrategy( new KPrDoubleSweepWipeStrategy( OppositeHorizontal, "doubleSweepWipe", "oppositeHorizontal", false ) );
    addStrategy( new KPrDoubleSweepWipeStrategy( ParallelDiagonalTopLeft, "doubleSweepWipe", "parallelDiagonalTopLeft", false ) );
    addStrategy( new KPrDoubleSweepWipeStrategy( ParallelDiagonalBottomLeft, "doubleSweepWipe", "parallelDiagonalBottomLeft", false ) );
    addStrategy( new KPrDoubleSweepWipeStrategy( ParallelVerticalReverse, "doubleSweepWipe", "parallelVertical", true ) );
    addStrategy( new KPrDoubleSweepWipeStrategy( ParallelDiagonalReverse, "doubleSweepWipe", "parallelDiagonal", true ) );
    addStrategy( new KPrDoubleSweepWipeStrategy( OppositeVerticalReverse, "doubleSweepWipe", "oppositeVertical", true ) );
    addStrategy( new KPrDoubleSweepWipeStrategy( OppositeHorizontalReverse, "doubleSweepWipe", "oppositeHorizontal", true ) );
    addStrategy( new KPrDoubleSweepWipeStrategy( ParallelDiagonalTopLeftReverse, "doubleSweepWipe", "parallelDiagonalTopLeft", true ) );
    addStrategy( new KPrDoubleSweepWipeStrategy( ParallelDiagonalBottomLeftReverse, "doubleSweepWipe", "parallelDiagonalBottomLeft", true ) );
}

KPrDoubleSweepWipeEffectFactory::~KPrDoubleSweepWipeEffectFactory()
{
}

static const char* const s_subTypes[] = {
    I18N_NOOP( "Parallel Vertical" ),
    I18N_NOOP( "Parallel Diagonal" ),
    I18N_NOOP( "Opposite Vertical" ),
    I18N_NOOP( "Opposite Horizontal" ),
    I18N_NOOP( "Parallel Diagonal Top Left" ),
    I18N_NOOP( "Parallel Diagonal Bottom Left" ),
    I18N_NOOP( "Parallel Vertical Reverse" ),
    I18N_NOOP( "Parallel Diagonal Reverse" ),
    I18N_NOOP( "Opposite Vertical Reverse" ),
    I18N_NOOP( "Opposite Horizontal Reverse" ),
    I18N_NOOP( "Parallel Diagonal Top Left Reverse" ),
    I18N_NOOP( "Parallel Diagonal Bottom Left Reverse" )
};

QString KPrDoubleSweepWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return i18n( s_subTypes[subType] );
    } else {
        return i18n( "Unknown subtype" );
    }
}
