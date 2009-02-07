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

#ifndef SVGPATTERNHELPER_H
#define SVGPATTERNHELPER_H

#include <QtGui/QImage>
#include <QtGui/QMatrix>

class SvgPatternHelper
{
public:
    enum Units { UserSpaceOnUse, ObjectBoundingBox };

    SvgPatternHelper();
    ~SvgPatternHelper();

    void setPatternUnits( Units units );
    Units patternUnits() const;

    void setPatternContentUnits( Units units );
    Units patternContentUnits() const;

    void setTransform( const QMatrix &transform );
    QMatrix transform() const;

    void setImage( const QImage &image );
    QImage image() const;

    void setPosition( const QPointF & position );
    QPointF position() const;

    void setSize( const QSizeF & size );
    QSizeF size() const;

private:
    Units m_patternUnits;
    Units m_patternContentUnits;
    QMatrix m_transform;
    QImage m_image;
    QPointF m_position;
    QSizeF m_size;
};

#endif // SVGPATTERNHELPER_H
