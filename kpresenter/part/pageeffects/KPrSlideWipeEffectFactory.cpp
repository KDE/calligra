/* This file is part of the KDE project
   Copyright (C) 2007 Thorsten Zachmann <zachmann@kde.org>

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

#include "KPrSlideWipeEffectFactory.h"

#include <klocale.h>

#include "KPrSlideWipeEffect.h"

QList<KPrPageEffect::SubType> KPrSlideWipeEffectFactory::initSubTypes()
{
    QList<KPrPageEffect::SubType> subTypes;
    subTypes << KPrPageEffect::LeftToRight << KPrPageEffect::RightToLeft << KPrPageEffect::TopToBottom << KPrPageEffect::BottomToTop;
    return subTypes;
}

KPrSlideWipeEffectFactory::KPrSlideWipeEffectFactory()
: KPrPageEffectFactory( SlideWipeEffectId, i18n( "Slide Wipe Effect" ), initSubTypes() )
{
}

KPrSlideWipeEffectFactory::~KPrSlideWipeEffectFactory()
{
}

KPrPageEffect * KPrSlideWipeEffectFactory::createPageEffect( const Properties & properties ) const
{
    return new KPrSlideWipeEffect( properties.duration );
}
