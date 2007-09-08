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

KPrCoverDownEffect::KPrCoverDownEffect()
: KPrPageEffect()
, m_count( 0 )
{
}

void KPrCoverDownEffect::setup( const Data &data, QTimeLine &timeLine )
{
    timeLine.setDuration( m_duration );
    timeLine.setFrameRange( 0, data.m_widget->height() );
    timeLine.setCurveShape( QTimeLine::LinearCurve );
}

bool KPrCoverDownEffect::paint( QPainter &p, const Data &data )
{
    int height = data.m_widget->height();
    ++m_count;

    int m_lastPos = data.m_timeLine.frameForTime( data.m_currentTime );

    bool finish = data.m_finished;

    if ( m_lastPos >= height )
    {
        finish = true;
    }

    if ( ! finish )
    {
        int width = data.m_widget->width();
        QRect rect1( 0, m_lastPos, width, height - m_lastPos );
        QRect rect2( 0, height - m_lastPos, width, m_lastPos );
        p.drawPixmap( QPoint( 0, m_lastPos ), data.m_oldPage, rect1 );
        p.drawPixmap( QPoint( 0, 0 ), data.m_newPage, rect2 );
    }
    else
    {
        p.drawPixmap( 0, 0, data.m_newPage );
        qDebug() << "m_count" << m_count;
    }

    return !finish;
}
