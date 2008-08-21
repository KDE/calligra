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

#include "ChartShape.h"

#include <Qt>
#include <QVector>
#include <QRect>

class QRect;
class QPoint;

namespace KChart {

/**
 * @brief The CellRegion class handles regions in a table.
 *
 * In contrast to a QItemSelection, a cell region can include header
 * data. Therefore, CellRegion( QPoint( 1, 1 ) ) represents the
 * top-left item of a QAbstractItemModel.
 *  
 * An instance can represent either a simple, continuous region of
 * cells, as in most cases, or a more complex discontinuous region. In
 * its second form, the orientation of each separate continuous region
 * can vary, as well as their sizes.
 */
class CHARTSHAPELIB_EXPORT CellRegion
{
public:
    CellRegion();
    CellRegion( const QPoint &point );
    CellRegion( const QRect &rect );
    CellRegion( const QPoint &point, const QSize &size );
    CellRegion( const QVector<QRect> &rects );
    ~CellRegion();
    
    QVector<QRect> rects() const;
    
    bool isValid() const;
    
    bool contains( const QPoint &point, bool proper = false ) const;
    bool contains( const QRect &rect, bool proper = false ) const;
    
    bool intersects( const QRect &rect ) const;
    
    CellRegion intersected( const QRect &rect ) const;
    
    int cellCount() const;
    int rectCount() const;
    
    Qt::Orientation orientation() const;
    
    void add( const CellRegion &other );
    void add( const QPoint &point );
    void add( const QRect &rect );
    void add( const QVector<QRect> &rects );
    
    void subtract( const QPoint &point );
    
    QRect boundingRect() const;
    
    QPoint pointAtIndex( int index ) const;
    int indexAtPoint( const QPoint &point ) const;
    
    static QString regionToString( const QVector<QRect> &region );
    static QVector<QRect> stringToRegion( const QString &string );
    
    static int rangeCharToInt( char c );
    static int rangeStringToInt( const QString &string );
    static QString rangeIntToString( int i );
    
    bool operator == ( const CellRegion &other ) const;

private:
    QVector<QRect> m_rects;
    QRect m_boundingRect;
};

}

#endif // KCHART_CELLREGION_H
