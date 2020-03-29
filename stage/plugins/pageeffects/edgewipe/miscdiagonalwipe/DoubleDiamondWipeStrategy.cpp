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
#include "MiscDiagonalWipeEffectFactory.h"
#include <QWidget>
#include <QPainter>
#include <QPainterPath>

const int StepCount = 250;

DoubleDiamondWipeStrategy::DoubleDiamondWipeStrategy()
    : KPrPageEffectStrategy( MiscDiagonalWipeEffectFactory::DoubleDiamond, "miscDiagonalWipe", "doubleDiamond", false )
{
}

DoubleDiamondWipeStrategy::~DoubleDiamondWipeStrategy()
{
}

void DoubleDiamondWipeStrategy::setup( const KPrPageEffect::Data &data, QTimeLine &timeLine )
{
    Q_UNUSED( data );
    timeLine.setFrameRange( 0, StepCount );
}

void DoubleDiamondWipeStrategy::paintStep( QPainter &p, int currPos, const KPrPageEffect::Data &data )
{
    p.drawPixmap( QPoint( 0, 0 ), data.m_oldPage, data.m_widget->rect() );
    p.setClipPath( clipPath( currPos, data.m_widget->rect() ) );
    p.drawPixmap( QPoint( 0, 0 ), data.m_newPage, data.m_widget->rect() );
}

void DoubleDiamondWipeStrategy::next( const KPrPageEffect::Data &data )
{
    data.m_widget->update();
}

QPainterPath DoubleDiamondWipeStrategy::clipPath( int step, const QRect &area )
{
    int width_2 = area.width() >> 1;
    int height_2 = area.height() >> 1;

    qreal percent = static_cast<qreal>(StepCount - step) / static_cast<qreal>(StepCount);
    int stepx = static_cast<int>( width_2 * percent );
    int stepy = static_cast<int>( height_2 * percent );

    QPainterPath outerPath;
    outerPath.moveTo( area.topLeft() + QPoint( stepx, 0 ) );
    outerPath.lineTo( area.topRight() - QPoint( stepx, 0 ) );
    outerPath.lineTo( area.topRight() + QPoint( 0, stepy ) );
    outerPath.lineTo( area.bottomRight() - QPoint( 0, stepy ) );
    outerPath.lineTo( area.bottomRight() - QPoint( stepx, 0 ) );
    outerPath.lineTo( area.bottomLeft() + QPoint( stepx, 0 ) );
    outerPath.lineTo( area.bottomLeft() - QPoint( 0, stepy ) );
    outerPath.lineTo( area.topLeft() + QPoint( 0, stepy ) );
    outerPath.closeSubpath();

    QPainterPath innerPath;
    innerPath.moveTo( area.center() - QPoint( 0, stepy ) );
    innerPath.lineTo( area.center() + QPoint( stepx, 0 ) );
    innerPath.lineTo( area.center() + QPoint( 0, stepy ) );
    innerPath.lineTo( area.center() - QPoint( stepx, 0 ) );
    innerPath.closeSubpath();

    return outerPath.subtracted( innerPath );
}
