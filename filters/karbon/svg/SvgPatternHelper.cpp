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

#include "SvgPatternHelper.h"

SvgPatternHelper::SvgPatternHelper()
: m_patternUnits( ObjectBoundingBox ), m_patternContentUnits( UserSpaceOnUse )
{
}

SvgPatternHelper::~SvgPatternHelper()
{
}

void SvgPatternHelper::setPatternUnits( Units units )
{
    m_patternUnits = units;
}

SvgPatternHelper::Units SvgPatternHelper::patternUnits() const
{
    return m_patternUnits;
}

void SvgPatternHelper::setPatternContentUnits( Units units )
{
    m_patternContentUnits = units;
}

SvgPatternHelper::Units SvgPatternHelper::patternContentUnits() const
{
    return m_patternContentUnits;
}

void SvgPatternHelper::setTransform( const QMatrix &transform )
{
    m_transform = transform;
}

QMatrix SvgPatternHelper::transform() const
{
    return m_transform;
}

void SvgPatternHelper::setImage( const QImage &image )
{
    m_image = image;
}

QImage SvgPatternHelper::image() const
{
    return m_image;
}

void SvgPatternHelper::setPosition( const QPointF & position )
{
    m_position = position;
}

QPointF SvgPatternHelper::position() const
{
    return m_position;
}

void SvgPatternHelper::setSize( const QSizeF & size )
{
    m_size = size;
}

QSizeF SvgPatternHelper::size() const
{
    return m_size;
}
