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
#include "DiagonalWipeEffectFactory.h"
#include <QWidget>
#include <QPainter>
#include <QPainterPath>

const int StepCount = 250;

DiagonalWipeStrategy::DiagonalWipeStrategy( int subtype, const char *smilSubType, bool reverse )
    : KPrPageEffectStrategy( subtype, "diagonalWipe", smilSubType, reverse )
{
}

DiagonalWipeStrategy::~DiagonalWipeStrategy()
{
}

void DiagonalWipeStrategy::setup( const KPrPageEffect::Data &data, QTimeLine &timeLine )
{
    Q_UNUSED( data );
    timeLine.setFrameRange( 0, StepCount );
}

void DiagonalWipeStrategy::paintStep( QPainter &p, int currPos, const KPrPageEffect::Data &data )
{
    p.drawPixmap( QPoint( 0, 0 ), data.m_oldPage, data.m_widget->rect() );
    p.setClipPath( clipPath( currPos, data.m_widget->rect() ) );
    p.drawPixmap( QPoint( 0, 0 ), data.m_newPage, data.m_widget->rect() );
}

void DiagonalWipeStrategy::next( const KPrPageEffect::Data &data )
{
    data.m_widget->update();
}

QPainterPath DiagonalWipeStrategy::clipPath( int step, const QRect &area )
{
    qreal percent = static_cast<qreal>(step) / static_cast<qreal>(StepCount);

    QPoint vecx( static_cast<int>(2.0 * area.width() * percent), 0 ) ;
    QPoint vecy( 0, static_cast<int>( 2.0 * area.height() * percent ) );

    QPainterPath path;

    switch( subType() )
    {
        case DiagonalWipeEffectFactory::FromTopLeft:
            path.moveTo( area.topLeft() );
            path.lineTo( area.topLeft() + vecx );
            path.lineTo( area.topLeft() + vecy );
            break;
        case DiagonalWipeEffectFactory::FromTopRight:
            path.moveTo( area.topRight() );
            path.lineTo( area.topRight() - vecx );
            path.lineTo( area.topRight() + vecy );
            break;
        case DiagonalWipeEffectFactory::FromBottomLeft:
            path.moveTo( area.bottomLeft() );
            path.lineTo( area.bottomLeft() + vecx );
            path.lineTo( area.bottomLeft() - vecy );
            break;
        case DiagonalWipeEffectFactory::FromBottomRight:
            path.moveTo( area.bottomRight() );
            path.lineTo( area.bottomRight() - vecx );
            path.lineTo( area.bottomRight() - vecy );
            break;
        default:
            return QPainterPath();
    }

    path.closeSubpath();

    return path;
}
