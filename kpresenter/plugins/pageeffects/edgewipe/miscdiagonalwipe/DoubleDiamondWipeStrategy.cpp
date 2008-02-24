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

#include "DoubleDiamondWipeStrategy.h"
#include <QtGui/QWidget>
#include <QtGui/QPainter>

const int StepCount = 150;

DoubleDiamondWipeStrategy::DoubleDiamondWipeStrategy()
    : KPrPageEffectStrategy( KPrPageEffect::DoubleDiamond, "miscDiagonalWipe", "doubleDiamond", false )
{
}

DoubleDiamondWipeStrategy::~DoubleDiamondWipeStrategy()
{
}

void DoubleDiamondWipeStrategy::setup( const KPrPageEffect::Data &data, QTimeLine &timeLine )
{
    timeLine.setFrameRange( 0, StepCount );
}

void DoubleDiamondWipeStrategy::paintStep( QPainter &p, int currPos, const KPrPageEffect::Data &data )
{
    p.drawPixmap( QPoint( 0, 0 ), data.m_oldPage, data.m_widget->rect() );
    p.drawPixmap( QPoint( 0, 0 ), data.m_newPage, data.m_widget->rect() );
}

void DoubleDiamondWipeStrategy::next( const KPrPageEffect::Data &data )
{
    int lastPos = data.m_timeLine.frameForTime( data.m_lastTime );
    int currPos = data.m_timeLine.frameForTime( data.m_currentTime );
    if( lastPos == currPos )
        return;

    QRegion oldRegion = clipRegion( lastPos, data.m_widget->rect() );
    QRegion newRegion = clipRegion( currPos, data.m_widget->rect() );
    data.m_widget->update( newRegion.subtracted( oldRegion ) );
}

QRegion DoubleDiamondWipeStrategy::clipRegion( int step, const QRect &area )
{
    int width_2 = area.width() >> 1;
    int height_2 = area.height() >> 1;

    qreal percent = static_cast<qreal>(StepCount - step) / static_cast<qreal>(StepCount);
    int stepx = static_cast<int>( width_2 * percent );
    int stepy = static_cast<int>( height_2 * percent );

    QPolygon outerPoly;
    outerPoly.append( area.topLeft() + QPoint( stepx, 0 ) );
    outerPoly.append( area.topRight() - QPoint( stepx, 0 ) );
    outerPoly.append( area.topRight() + QPoint( 0, stepy ) );
    outerPoly.append( area.bottomRight() - QPoint( 0, stepy ) );
    outerPoly.append( area.bottomRight() - QPoint( stepx, 0 ) );
    outerPoly.append( area.bottomLeft() + QPoint( stepx, 0 ) );
    outerPoly.append( area.bottomLeft() - QPoint( 0, stepy ) );
    outerPoly.append( area.topLeft() + QPoint( 0, stepy ) );

    QPolygon innerPoly;
    innerPoly.append( area.center() - QPoint( 0, stepy ) );
    innerPoly.append( area.center() + QPoint( stepx, 0 ) );
    innerPoly.append( area.center() + QPoint( 0, stepy ) );
    innerPoly.append( area.center() - QPoint( stepx, 0 ) );

    return QRegion( outerPoly ).subtracted( QRegion( innerPoly ) );
}
