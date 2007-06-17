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

#include "KPrCoverDownEffect.h"

#include <QPainter>
#include <QPixmap>
#include <QWidget>
#include <QDebug>

KPrCoverDownEffect::KPrCoverDownEffect( const QPixmap &px1, const QPixmap &px2, QWidget * w )
: KPrPageEffect( px1, px2, w )    
, m_count( 0 )    
{
    m_timeLine.setDuration( 5000 );
    m_timeLine.setFrameRange( 0, px1.height() );
    QObject::connect( &m_timeLine, SIGNAL( frameChanged( int ) ), w, SLOT( update() ) );
    m_timeLine.setCurveShape( QTimeLine::LinearCurve ); 
}

bool KPrCoverDownEffect::paint( QPainter &p, int currentTime )
{
    int height = m_widget->height();
    ++m_count;

    m_lastPos.setY( m_timeLine.frameForTime( currentTime ) );

    if ( m_lastPos.y() >= height )
    {
        m_finish = true;
    }

    if ( ! m_finish )
    {
        int width = m_widget->width();
        QRect rect1( 0, m_lastPos.y(), width, height - m_lastPos.y() );
        QRect rect2( 0, height - m_lastPos.y(), width, m_lastPos.y() );
        p.drawPixmap( QPoint( 0, m_lastPos.y() ), m_px1, rect1 );
        p.drawPixmap( QPoint( 0, 0 ), m_px2, rect2 );
    }
    else
    {
        p.drawPixmap( 0, 0, m_px2 );
        qDebug() << "m_count" << m_count;
    }

    return !m_finish;
}
