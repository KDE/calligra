/* This file is part of the KDE project
   Copyright (C) 2008 Thorsten Zachmann <zachmann@kde.org>
   Copyright (C) 2010 Benjamin Port <port.benjamin@gmail.com>

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

#include "KPrPageEffectStrategy.h"

#include <QWidget>

#include <KoXmlWriter.h>
#include <KoGenStyle.h>

KPrPageEffectStrategy::KPrPageEffectStrategy( int subType, const char * smilType, const char *smilSubType, bool reverse, bool graphicsView )
: m_subType( subType )
, m_smilData( smilType, smilSubType, reverse )
, m_graphicsView(graphicsView)
{
}

KPrPageEffectStrategy::~KPrPageEffectStrategy()
{
}

int KPrPageEffectStrategy::subType() const
{
    return m_subType;
}

void KPrPageEffectStrategy::finish( const KPrPageEffect::Data &data )
{
    data.m_widget->update();
}

void KPrPageEffectStrategy::saveOdfSmilAttributes( KoXmlWriter & xmlWriter ) const
{
    xmlWriter.addAttribute( "smil:type", m_smilData.type );
    xmlWriter.addAttribute( "smil:subtype", m_smilData.subType );
    if ( m_smilData.reverse ) {
        xmlWriter.addAttribute( "smil:direction", "reverse" );
    }
}

void KPrPageEffectStrategy::saveOdfSmilAttributes( KoGenStyle & style ) const
{
    style.addProperty( "smil:type", m_smilData.type );
    style.addProperty( "smil:subtype", m_smilData.subType );
    if ( m_smilData.reverse ) {
        style.addProperty( "smil:direction", "reverse" );
    }
}

void KPrPageEffectStrategy::loadOdfSmilAttributes( const KoXmlElement & element )
{
    Q_UNUSED( element );
}

const QString & KPrPageEffectStrategy::smilType() const
{
    return m_smilData.type;
}

const QString & KPrPageEffectStrategy::smilSubType() const
{
    return m_smilData.subType;
}

bool KPrPageEffectStrategy::reverse() const
{
    return m_smilData.reverse;
}

bool KPrPageEffectStrategy::useGraphicsView() const
{
    return m_graphicsView;
}
