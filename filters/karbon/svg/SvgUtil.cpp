/* This file is part of the KDE project
 * Copyright (C) 2009 Jan Hambrecht <jaham@gmx.net>
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

#include "SvgUtil.h"

#include <QtCore/QString>
#include <QtCore/QRectF>

#define DPI 72.0

double SvgUtil::fromUserSpace( double value )
{
    return (value * DPI) / 90.0;
}

double SvgUtil::toUserSpace( double value )
{
    return (value * 90.0) / DPI;
}

double SvgUtil::toPercentage( QString s )
{
    if( s.endsWith( '%' ) )
        return s.remove( '%' ).toDouble();
    else
        return s.toDouble() * 100.0;
}

double SvgUtil::fromPercentage( QString s )
{
    if( s.endsWith( '%' ) )
        return s.remove( '%' ).toDouble() / 100.0;
    else
        return s.toDouble();
}

QPointF SvgUtil::objectToUserSpace( const QPointF &position, const QRectF &objectBound )
{
    qreal x = objectBound.left() + position.x() * objectBound.width(); 
    qreal y = objectBound.top() + position.y() * objectBound.height(); 
    return QPointF( x, y );
}

QSizeF SvgUtil::objectToUserSpace( const QSizeF &size, const QRectF &objectBound )
{
    qreal w = size.width() * objectBound.width(); 
    qreal h = size.height() * objectBound.height(); 
    return QSizeF( w, h );
}
