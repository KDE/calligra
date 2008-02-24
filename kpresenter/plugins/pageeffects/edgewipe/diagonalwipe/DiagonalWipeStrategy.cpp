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

#include "DiagonalWipeStrategy.h"
#include <QtGui/QWidget>
#include <QtGui/QPainter>

const int StepCount = 150;

DiagonalWipeStrategy::DiagonalWipeStrategy( KPrPageEffect::SubType subtype, const char *smilSubType, bool reverse )
    : KPrPageEffectStrategy( subtype, "diagonalWipe", smilSubType, reverse )
{
}

DiagonalWipeStrategy::~DiagonalWipeStrategy()
{
}

void DiagonalWipeStrategy::setup( const KPrPageEffect::Data &data, QTimeLine &timeLine )
{
    timeLine.setFrameRange( 0, StepCount );
}

void DiagonalWipeStrategy::paintStep( QPainter &p, int currPos, const KPrPageEffect::Data &data )
{
    p.drawPixmap( QPoint( 0, 0 ), data.m_oldPage, data.m_widget->rect() );
    // remove next line to make it fast ;-)
    p.setClipRegion( clipRegion( currPos, data.m_widget->rect() ) );
    p.drawPixmap( QPoint( 0, 0 ), data.m_newPage, data.m_widget->rect() );
}

void DiagonalWipeStrategy::next( const KPrPageEffect::Data &data )
{
    int lastPos = data.m_timeLine.frameForTime( data.m_lastTime );
    int currPos = data.m_timeLine.frameForTime( data.m_currentTime );
    if( lastPos == currPos )
        return;

    QRegion oldRegion = clipRegion( lastPos, data.m_widget->rect() );
    QRegion newRegion = clipRegion( currPos, data.m_widget->rect() );
    data.m_widget->update( newRegion.subtracted( oldRegion ) );
}

QRegion DiagonalWipeStrategy::clipRegion( int step, const QRect &area )
{
    qreal percent = static_cast<qreal>(step) / static_cast<qreal>(StepCount);

    QPolygon poly;

    QPoint vecx( static_cast<int>(2.0 * area.width() * percent), 0 ) ;
    QPoint vecy( 0, static_cast<int>( 2.0 * area.height() * percent ) );

    switch( subType() )
    {
        case KPrPageEffect::FromTopLeft:
            poly.append( area.topLeft() );
            poly.append( area.topLeft() + vecx );
            poly.append( area.topLeft() + vecy );
            break;
        case KPrPageEffect::FromTopRight:
            poly.append( area.topRight() );
            poly.append( area.topRight() - vecx );
            poly.append( area.topRight() + vecy );
            break;
        case KPrPageEffect::FromBottomLeft:
            poly.append( area.bottomLeft() );
            poly.append( area.bottomLeft() + vecx );
            poly.append( area.bottomLeft() - vecy );
            break;
        case KPrPageEffect::FromBottomRight:
            poly.append( area.bottomRight() );
            poly.append( area.bottomRight() - vecx );
            poly.append( area.bottomRight() - vecy );
            break;
        default:
            return QRegion();
    }

    return QRegion( poly ).intersected( QRegion( area ) );
}
