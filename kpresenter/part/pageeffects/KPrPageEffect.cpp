/* This file is part of the KDE project
   Copyright (C) 2007-2008 Thorsten Zachmann <zachmann@kde.org>
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

#include "KPrPageEffect.h"

#include <QWidget>
#include <QPainter>

#include "KPrPageEffectStrategy.h"

KPrPageEffect::KPrPageEffect( int duration, const QString & id, KPrPageEffectStrategy * strategy )
: m_duration( duration )
, m_id( id )
, m_strategy( strategy )
{
    Q_ASSERT( strategy );
}

KPrPageEffect::~KPrPageEffect()
{
}

void KPrPageEffect::setup( const Data &data, QTimeLine &timeLine )
{
    timeLine.setDuration( m_duration );
    m_strategy->setup( data, timeLine );
    timeLine.setCurveShape( QTimeLine::LinearCurve );
}

bool KPrPageEffect::useGraphicsView()
{
    return m_strategy->useGraphicsView();
}

bool KPrPageEffect::paint( QPainter &p, const Data &data )
{
    int currPos = data.m_timeLine.frameForTime( data.m_currentTime );

    bool finish = data.m_finished;

    if ( currPos >= data.m_timeLine.endFrame() ) {
        finish = true;
    }

    if ( ! finish ) {
        m_strategy->paintStep( p, currPos, data );
    }
    else {
        p.drawPixmap( 0, 0, data.m_newPage );
    }

    return !finish;
}

void KPrPageEffect::next( const Data &data )
{
    m_strategy->next( data );
}

void KPrPageEffect::finish( const Data &data )
{
    m_strategy->finish( data );
}

int KPrPageEffect::duration() const
{
    return m_duration;
}

const QString & KPrPageEffect::id() const
{
    return m_id;
}

int KPrPageEffect::subType() const
{
    return m_strategy->subType();
}

void KPrPageEffect::saveOdfSmilAttributes( KoXmlWriter & xmlWriter ) const
{
    return m_strategy->saveOdfSmilAttributes( xmlWriter );
}

void KPrPageEffect::saveOdfSmilAttributes( KoGenStyle & style ) const
{
    return m_strategy->saveOdfSmilAttributes( style );
}

void KPrPageEffect::loadOdf( const KoXmlElement & /*element*/ )
{
}
