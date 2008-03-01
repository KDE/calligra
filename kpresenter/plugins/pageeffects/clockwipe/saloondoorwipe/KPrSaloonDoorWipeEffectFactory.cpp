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

#include "KPrSaloonDoorWipeEffectFactory.h"
#include <klocale.h>

#include "KPrSaloonDoorWipeStrategy.h"

#define SaloonDoorWipeEffectId  "SaloonDoorWipeEffect"

KPrSaloonDoorWipeEffectFactory::KPrSaloonDoorWipeEffectFactory()
: KPrPageEffectFactory( SaloonDoorWipeEffectId, i18n( "Saloon Door Wipe Effect" ) )
{
    addStrategy( new KPrSaloonDoorWipeStrategy( KPrPageEffect::FromTop, "saloonDoorWipe", "top", false ) );
    addStrategy( new KPrSaloonDoorWipeStrategy( KPrPageEffect::FromLeft, "saloonDoorWipe", "left", false ) );
    addStrategy( new KPrSaloonDoorWipeStrategy( KPrPageEffect::FromBottom, "saloonDoorWipe", "bottom", false ) );
    addStrategy( new KPrSaloonDoorWipeStrategy( KPrPageEffect::FromRight, "saloonDoorWipe", "right", false ) );

    addStrategy( new KPrSaloonDoorWipeStrategy( KPrPageEffect::ToTop, "saloonDoorWipe", "top", true ) );
    addStrategy( new KPrSaloonDoorWipeStrategy( KPrPageEffect::ToLeft, "saloonDoorWipe", "left", true ) );
    addStrategy( new KPrSaloonDoorWipeStrategy( KPrPageEffect::ToBottom, "saloonDoorWipe", "bottom", true ) );
    addStrategy( new KPrSaloonDoorWipeStrategy( KPrPageEffect::ToRight, "saloonDoorWipe", "right", true ) );
}

KPrSaloonDoorWipeEffectFactory::~KPrSaloonDoorWipeEffectFactory()
{
}

