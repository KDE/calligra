/* This file is part of the KDE project
 * Copyright (C) 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "CornersInWipeStrategy.h"
#include "FourBoxWipeEffectFactory.h"
#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QPainterPath>

const int StepCount = 250;

CornersInWipeStrategy::CornersInWipeStrategy( bool reverse )
    : KPrPageEffectStrategy( reverse ? FourBoxWipeEffectFactory::CornersInReverse : FourBoxWipeEffectFactory::CornersIn, "fourBoxWipe", "cornersIn", reverse )
{
}

CornersInWipeStrategy::~CornersInWipeStrategy()
{
}

void CornersInWipeStrategy::setup( const KPrPageEffect::Data &data, QTimeLine &timeLine )
{
    Q_UNUSED( data );
    timeLine.setFrameRange( 0, StepCount );
}

void CornersInWipeStrategy::paintStep( QPainter &p, int currPos, const KPrPageEffect::Data &data )
{
    p.drawPixmap( QPoint( 0, 0 ), data.m_oldPage, data.m_widget->rect() );
    p.setClipPath( clipPath( currPos, data.m_widget->rect() ) );
    p.drawPixmap( QPoint( 0, 0 ), data.m_newPage, data.m_widget->rect() );
}

void CornersInWipeStrategy::next( const KPrPageEffect::Data &data )
{
    data.m_widget->update();
}

QPainterPath CornersInWipeStrategy::clipPath( int step, const QRect &area )
{
    int width_2 = area.width() >> 1;
    int height_2 = area.height() >> 1;

    qreal percent = static_cast<qreal>(step) / static_cast<qreal>(StepCount);
    int stepx = static_cast<int>( width_2 * percent );
    int stepy = static_cast<int>( height_2 * percent );

    QPainterPath path;

    if( ! reverse() )
    {
        QSize rectSize( stepx, stepy );

        QRect topLeft( area.topLeft(), rectSize );
        QRect topRight( area.topRight() - QPoint( stepx, 0 ), rectSize );
        QRect bottomRight( area.bottomRight() - QPoint( stepx, stepy), rectSize );
        QRect bottomLeft( area.bottomLeft() - QPoint( 0, stepy ), rectSize );

        path.addRect( topLeft );
        path.addRect( topRight );
        path.addRect( bottomRight );
        path.addRect( bottomLeft );
    }
    else
    {
        QRect horzRect( QPoint( 0, 0 ), QSize( 2 * stepx, area.height() ) );
        horzRect.moveCenter( area.center() );
        QRect vertRect( QPoint( 0, 0 ), QSize( area.width(), 2 * stepy ) );
        vertRect.moveCenter( area.center() );

        path.addRect( horzRect );
        path.addRect( vertRect );
        path.setFillRule( Qt::WindingFill );
    }

    return path;
}
