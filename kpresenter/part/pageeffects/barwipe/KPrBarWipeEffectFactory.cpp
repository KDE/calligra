/* This file is part of the KDE project
   Copyright (C) 2007 Timothee Lacroix <dakeyras.khan@gmail.com>

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

#include "KPrBarWipeEffectFactory.h"

#include <klocale.h>

#include "KPrBarWipeFromTopStrategy.h"
#include "KPrBarWipeFromLeftStrategy.h"

#define BarWipeEffectId "BarWipeEffect"

QList<KPrPageEffect::SubType> KPrBarWipeEffectFactory::initSubTypes()
{
    QList<KPrPageEffect::SubType> subTypes;
    subTypes << KPrPageEffect::FromLeft << KPrPageEffect::FromTop;
    return subTypes;
}

KPrBarWipeEffectFactory::KPrBarWipeEffectFactory()
: KPrPageEffectFactory( BarWipeEffectId, i18n( "Bar Wipe Effect" ), initSubTypes() )
{
}

KPrBarWipeEffectFactory::~KPrBarWipeEffectFactory()
{
}

KPrPageEffect * KPrBarWipeEffectFactory::createPageEffect( const Properties & properties ) const
{
    KPrPageEffectStrategy * strategy = 0;

    switch ( properties.subType )
    {
        case KPrPageEffect::FromTop:
            strategy = new KPrBarWipeFromTopStrategy();
            break;
        case KPrPageEffect::FromLeft:
            strategy = new KPrBarWipeFromLeftStrategy();
            break;
        default:
            //TODO error message
            strategy = new KPrBarWipeFromTopStrategy();
            break;
    }
    return new KPrPageEffect( properties.duration, BarWipeEffectId, strategy );
}

KPrPageEffect * KPrBarWipeEffectFactory::createPageEffect( const KoXmlElement & element ) const
{
}
