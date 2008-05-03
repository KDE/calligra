/* This file is part of the KDE project

   Copyright 2008 Johannes Simon <johannes.simon@gmail.com>

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

#ifndef KCHART_CELLREGION_H
#define KCHART_CELLREGION_H

#include <Qt>
#include <QVector>
#include <QRect>

class QRect;
class QPoint;

namespace KChart {

class CellRegion
{
public:
    CellRegion();
    CellRegion( const QPoint &point );
    CellRegion( const QRect &rect );
    CellRegion( const QVector<QRect> &rects );
    ~CellRegion();
    
    QVector<QRect> rects() const;
    
    bool isValid() const;
    
    bool contains( const QPoint &point, bool proper = false ) const;
    bool contains( const QRect &region, bool proper = false ) const;
    
    int cellCount() const;
    int rectCount() const;
    
    Qt::Orientation orientation() const;
    
    void add( const QPoint &point );
    void add( const QRect &rect );
    void add( const QVector<QRect> &rects );
    
    void subtract( const QPoint &point );
    
    QRect boundingRect() const;
    
    QPoint pointAtIndex( int index ) const;
    int indexAtPoint( const QPoint &point ) const;
    
private:
    QVector<QRect> m_rects;
    QRect m_boundingRect;
};

}

#endif // KCHART_CELLREGION_H
