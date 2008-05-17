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

#include "CellRegion.h"

#include <QPoint>
#include <QRect>
#include <QVector>
#include <QPoint>
#include <QDebug>

using namespace KChart;


CellRegion::CellRegion()
{
}

CellRegion::CellRegion( const QPoint &point )
{
    add( point );
}

CellRegion::CellRegion( const QRect &rect )
{
    add( rect );
}

CellRegion::CellRegion( const QVector<QRect> &rects )
{
    add( rects );
}

CellRegion::~CellRegion()
{
}

QVector<QRect> CellRegion::rects() const
{
    return m_rects;
}

int CellRegion::rectCount() const
{
    return m_rects.size();
}

bool CellRegion::isValid() const
{
    return m_rects.size() > 0;
}

bool CellRegion::contains( const QPoint &point, bool proper ) const
{
    foreach ( const QRect &rect, m_rects )
    {
        if ( rect.contains( point, proper ) )
            return true;
    }
    
    return false;
}

bool CellRegion::contains( const QRect &rect, bool proper ) const
{
    foreach ( const QRect &r, m_rects )
    {
        if ( r.contains( rect, proper ) )
            return true;
    }
    
    return false;
}

Qt::Orientation CellRegion::orientation() const
{
    foreach ( const QRect &rect, m_rects )
    {
    	if ( rect.width() > 1 )
    		return Qt::Horizontal;
    	if ( rect.height() > 1 )
    		return Qt::Vertical;
    }
    
    // Default if region is only one cell
    return Qt::Vertical;
}

int CellRegion::cellCount() const
{
    int count = 0;
    if ( orientation() == Qt::Horizontal )
    {
        foreach ( const QRect &rect, m_rects )
            count += rect.width();
    }
    else
    {
        foreach( const QRect &rect, m_rects )
            count += rect.height();
    }
    
    return count;
}

void CellRegion::add( const QPoint &point )
{
    add( QRect( point, QSize( 1, 1 ) ) );
}

void CellRegion::add( const QRect &rect )
{
    if ( !rect.isValid() )
    {
        qWarning() << "CellRegion::add() Attempt to add invalid rectangle";
        qWarning() << "CellRegion::add():" << rect;
        return;
    }
    
    if ( rect.width() > 1 && rect.height() > 1 )
    {
        qWarning() << "CellRegion::add() Attempt to add rectangle with height AND width > 1";
        qWarning() << "CellRegion::add():" << rect;
        return;
    }
    
    m_rects.append( rect );
    m_boundingRect |= rect;
}

void CellRegion::add( const QVector<QRect> &rects )
{
    foreach ( const QRect &rect, rects )
        add( rect );
}

void CellRegion::subtract( const QPoint &point )
{
    if ( orientation() == Qt::Horizontal )
    {
        for ( int i = 0; i < m_rects.size(); i++ )
        {
            if ( m_rects[ i ].contains( point ) )
            {
                if ( m_rects[ i ].topLeft().x() == point.x() )
                {
                    m_rects[ i ].translate( 1, 0 );
                    m_rects[ i ].setWidth( m_rects[ i ].width() - 1 );
                }
                else if ( m_rects[ i ].topRight().x() == point.x() )
                {
                    m_rects[ i ].setWidth( m_rects[ i ].width() - 1 );
                }
                return;
            }
        }
    }
    else
    {
        for ( int i = 0; i < m_rects.size(); i++ )
        {
            if ( m_rects[ i ].contains( point ) )
            {
                if ( m_rects[ i ].topLeft().y() == point.y() )
                {
                    m_rects[ i ].translate( 0, 1 );
                    m_rects[ i ].setHeight( m_rects[ i ].height() - 1 );
                }
                else if ( m_rects[ i ].bottomLeft().y() == point.y() )
                {
                    m_rects[ i ].setHeight( m_rects[ i ].height() - 1 );
                }
                return;
            }
        }
    }

    // Recalculate bounding rectangle
    m_boundingRect = QRect();
    foreach ( const QRect &rect, m_rects )
        m_boundingRect |= rect;
}

QRect CellRegion::boundingRect() const
{
    return m_boundingRect;
}

QPoint CellRegion::pointAtIndex( int index ) const
{
    // sum of all previous rectangle indices
    int i = 0;
    
    foreach ( const QRect &rect, m_rects )
    {
        // Rectangle is horizontal
        if ( rect.width() > 1 )
        {
            // Found it!
            // Index refers to point in current rectangle
            if ( i + rect.width() > index )
            {
                // Local index of point in this rectangle
                int j = index - i;
                return QPoint( rect.x() + j, rect.y() );
            }

            // add number of indices in current rectangle to total index count
            i += rect.width();
        }
        else
        {
            // Found it!
            // Index refers to point in current rectangle
            if ( i + rect.height() > index )
            {
                // Local index of point in this rectangle
                int j = index - i;
                return QPoint( rect.x(), rect.y() + j );
            }

            // add number of indices in current rectangle to total index count
            i += rect.height();
        }
    }
    
    // Invalid index!
    return QPoint( -1, -1 );
}

int CellRegion::indexAtPoint( const QPoint &point ) const
{
}
