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

#include "BoxWipeStrategy.h"
#include "BoxWipeEffectFactory.h"
#include <QWidget>
#include <QPainter>
#include <QPainterPath>

const int StepCount = 250;

BoxWipeStrategy::BoxWipeStrategy( int subtype, const char *smilSubType, bool reverse )
    : KPrPageEffectStrategy( subtype, "boxWipe", smilSubType, reverse )
{
}

BoxWipeStrategy::~BoxWipeStrategy()
{
}

void BoxWipeStrategy::setup( const KPrPageEffect::Data &data, QTimeLine &timeLine )
{
    Q_UNUSED( data );
    timeLine.setFrameRange( 0, StepCount );
}

void BoxWipeStrategy::paintStep( QPainter &p, int currPos, const KPrPageEffect::Data &data )
{
    p.drawPixmap( QPoint( 0, 0 ), data.m_oldPage, data.m_widget->rect() );
    p.setClipPath( clipPath( currPos, data.m_widget->rect() ) );
    p.drawPixmap( QPoint( 0, 0 ), data.m_newPage, data.m_widget->rect() );
}

void BoxWipeStrategy::next( const KPrPageEffect::Data &data )
{
    data.m_widget->update();
}

QPainterPath BoxWipeStrategy::clipPath( int step, const QRect &area )
{
    qreal percent = static_cast<qreal>(step) / static_cast<qreal>(StepCount);
    if( reverse() )
        percent = static_cast<qreal>(StepCount-step) / static_cast<qreal>(StepCount);

    int stepx = static_cast<int>( area.width() * percent );
    int stepy = static_cast<int>( area.height() * percent );

    QRect clipRect( QPoint( 0, 0 ), QSize( stepx, stepy ) );
    switch( subType() )
    {
        case BoxWipeEffectFactory::FromTopLeft:
            clipRect.moveTopLeft( area.topLeft() );
            break;
        case BoxWipeEffectFactory::FromTopRight:
            clipRect.moveTopRight( area.topRight() );
            break;
        case BoxWipeEffectFactory::FromBottomLeft:
            clipRect.moveBottomLeft( area.bottomLeft() );
            break;
        case BoxWipeEffectFactory::FromBottomRight:
            clipRect.moveBottomRight( area.bottomRight() );
            break;
        case BoxWipeEffectFactory::CenterTop:
            clipRect.moveCenter( area.center() );
            clipRect.moveTop( 0 );
            break;
        case BoxWipeEffectFactory::CenterRight:
            clipRect.moveCenter( area.center() );
            clipRect.moveRight( area.width() );
            break;
        case BoxWipeEffectFactory::CenterBottom:
            clipRect.moveCenter( area.center() );
            clipRect.moveBottom( area.height() );
            break;
        case BoxWipeEffectFactory::CenterLeft:
            clipRect.moveCenter( area.center() );
            clipRect.moveLeft( 0 );
            break;
        default:
            return QPainterPath();
    }

    QPainterPath path;
    path.addRect( clipRect );

    if( reverse() )
    {
        QPainterPath areaPath;
        areaPath.addRect( area );
        path = areaPath.subtracted( path );
    }

    return path;
}
