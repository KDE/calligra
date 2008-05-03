/* This file is part of the KDE project

   Copyright 2007-2008 Johannes Simon <johannes.simon@gmail.com>

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

// Local
#include "CellDataSet.h"
#include "CellRegion.h"
#include "ProxyModel.h"
#include "Axis.h"
#include "PlotArea.h"
#include "KDChartModel.h"
#include <interfaces/KoChartModel.h>

// Qt
#include <QAbstractItemModel>
#include <QString>
#include <QPen>
#include <QColor>

// KDChart
#include <KDChartDataValueAttributes>
#include <KDChartAbstractDiagram>

using namespace KChart;

CellDataSet::CellDataSet( ProxyModel *proxyModel )
    : DataSet( proxyModel )
{
}

CellDataSet::~CellDataSet()
{
    if ( m_attachedAxis )
        m_attachedAxis->detachDataSet( this, true );
}


QVariant CellDataSet::xData( int index ) const
{
    if ( !m_xDataRegion.isValid() )
            return QVariant();
    QAbstractItemModel *model = m_model->sourceModel();
    if ( !model )
        return QVariant();
        
    QPoint dataPoint = m_xDataRegion.pointAtIndex( index );
    if ( dataPoint.x() < 0 || dataPoint.y() < 0 )
        return QVariant();
    return model->data( model->index( dataPoint.y(), dataPoint.x() ) );
}

QVariant CellDataSet::yData( int index ) const
{
    if ( !m_yDataRegion.isValid() )
            return QVariant();
    QAbstractItemModel *model = m_model->sourceModel();
    if ( !model )
        return QVariant();
        
    QPoint dataPoint = m_yDataRegion.pointAtIndex( index );
    if ( dataPoint.x() < 0 || dataPoint.y() < 0 )
        return QVariant();
    return model->data( model->index( dataPoint.y(), dataPoint.x() ) );
}

QVariant CellDataSet::customData( int index ) const
{
    if ( !m_customDataRegion.isValid() )
            return QVariant();
    QAbstractItemModel *model = m_model->sourceModel();
    if ( !model )
        return QVariant();
        
    QPoint dataPoint = m_customDataRegion.pointAtIndex( index );
    if ( dataPoint.x() < 0 || dataPoint.y() < 0 )
        return QVariant();
    return model->data( model->index( dataPoint.y(), dataPoint.x() ) );
}

QVariant CellDataSet::categoryData( int index ) const
{
    if ( !m_categoryDataRegion.isValid() )
            return QVariant();
        
    QPoint dataPoint = m_categoryDataRegion.pointAtIndex( index );
    return m_model->sourceModel()->data( m_model->index( dataPoint.y(), dataPoint.x() ) );
}

QVariant CellDataSet::labelData() const
{
    if ( !m_labelDataRegion.isValid() )
            return QVariant();
    
    QString label;
    
    int cellCount = m_labelDataRegion.cellCount();
    for ( int i = 0; i < cellCount; i++ )
    {
        QPoint dataPoint = m_labelDataRegion.pointAtIndex( i );
        label += m_model->sourceModel()->data( m_model->index( dataPoint.y(), dataPoint.x() ) ).toString();
    }
    
    return QVariant( label );
}


CellRegion CellDataSet::xDataRegion() const
{
    return m_xDataRegion;
}

CellRegion CellDataSet::yDataRegion() const
{
    return m_yDataRegion;
}

CellRegion CellDataSet::customDataRegion() const
{
    return m_customDataRegion;
}

CellRegion CellDataSet::categoryDataRegion() const
{
    return m_categoryDataRegion;
}

CellRegion CellDataSet::labelDataRegion() const
{
    return m_labelDataRegion;
}

QString CellDataSet::xDataRegionString() const
{
    if ( !m_model->spreadSheetModel() )
        return QString();
    
    return m_model->spreadSheetModel()->regionToString( m_xDataRegion.rects() );
}

QString CellDataSet::yDataRegionString() const
{
    if ( !m_model->spreadSheetModel() )
        return QString();
    
    return m_model->spreadSheetModel()->regionToString( m_yDataRegion.rects() );
}

QString CellDataSet::customDataRegionString() const
{
    if ( !m_model->spreadSheetModel() )
        return QString();
    
    return m_model->spreadSheetModel()->regionToString( m_customDataRegion.rects() );
}

QString CellDataSet::categoryDataRegionString() const
{
    if ( !m_model->spreadSheetModel() )
        return QString();
    
    return m_model->spreadSheetModel()->regionToString( m_categoryDataRegion.rects() );
}

QString CellDataSet::labelDataRegionString() const
{
    if ( !m_model->spreadSheetModel() )
        return QString();
    
    return m_model->spreadSheetModel()->regionToString( m_labelDataRegion.rects() );
}


void CellDataSet::setXDataRegion( const CellRegion &region )
{
    m_xDataRegion = region;
    int newSize = 0;
    
    QRect boundingRect;
    foreach ( QRect rect, region.rects() )
    {
        newSize += qMax( rect.width(), rect.height() );
        boundingRect |= rect;
    }
    
    if ( boundingRect.height() > 1 )
        m_xDataDirection = Qt::Vertical;
    else
        m_xDataDirection = Qt::Horizontal;

    m_size = qMax( m_size, newSize );
}

void CellDataSet::setYDataRegion( const CellRegion &region )
{
    m_yDataRegion = region;
    m_yDataDirection = region.orientation();
    const int oldSize = m_size;
    m_size = region.cellCount();
    
    if ( !m_blockSignals && m_kdChartModel )
    {
        if ( oldSize != m_size )
            m_kdChartModel->dataSetSizeChanged( this, m_size );
        m_kdChartModel->dataChanged( m_kdChartModel->index( 0,          m_kdDataSetNumber ),
                                     m_kdChartModel->index( m_size - 1, m_kdDataSetNumber ) );
    }
}

void CellDataSet::setCustomDataRegion( const CellRegion &region )
{
    m_customDataRegion = region;
    int newSize = 0;
    
    QRect boundingRect;
    foreach ( QRect rect, region.rects() )
    {
        newSize += qMax( rect.width(), rect.height() );
        boundingRect |= rect;
    }
    
    if ( boundingRect.height() > 1 )
        m_customDataDirection = Qt::Vertical;
    else
        m_customDataDirection = Qt::Horizontal;

    m_size = qMax( m_size, newSize );
}

void CellDataSet::setCategoryDataRegion( const CellRegion &region )
{
    m_categoryDataRegion = region;
    int newSize = 0;
    
    QRect boundingRect;
    foreach ( QRect rect, region.rects() )
    {
        newSize += qMax( rect.width(), rect.height() );
        boundingRect |= rect;
    }
    
    if ( boundingRect.height() > 1 )
        m_categoryDataDirection = Qt::Vertical;
    else
        m_categoryDataDirection = Qt::Horizontal;
    
    m_size = qMax( m_size, newSize );
}

void CellDataSet::setLabelDataRegion( const CellRegion &region )
{
    m_labelDataRegion = region;
    
    QRect boundingRect;
    foreach ( QRect rect, region.rects() )
        boundingRect |= rect;
    
    if ( boundingRect.height() > 1 )
        m_labelDataDirection = Qt::Vertical;
    else
        m_labelDataDirection = Qt::Horizontal;
}

void CellDataSet::setXDataRegionString( const QString &string )
{
    if ( !m_model->spreadSheetModel() )
        return;
    
    setXDataRegion( m_model->spreadSheetModel()->stringToRegion( string ) );
}

void CellDataSet::setYDataRegionString( const QString &string )
{
    if ( !m_model->spreadSheetModel() )
        return;
    
    setYDataRegion( m_model->spreadSheetModel()->stringToRegion( string ) );
}

void CellDataSet::setCustomDataRegionString( const QString &string )
{
    if ( !m_model->spreadSheetModel() )
        return;
    
    setCustomDataRegion( m_model->spreadSheetModel()->stringToRegion( string ) );
}

void CellDataSet::setCategoryDataRegionString( const QString &string )
{
    if ( !m_model->spreadSheetModel() )
        return;
    
    setCategoryDataRegion( m_model->spreadSheetModel()->stringToRegion( string ) );
}

void CellDataSet::setLabelDataRegionString( const QString &string )
{
    if ( !m_model->spreadSheetModel() )
        return;
    
    setLabelDataRegion( m_model->spreadSheetModel()->stringToRegion( string ) );
}


int CellDataSet::size() const
{
    qDebug() << "CellDataSet::size(): Returning" << m_size;
    return m_size;
}

void CellDataSet::yDataChanged( const QRect &rect ) const
{    
    int start, end;
    
    QVector<QRect> yDataRegionRects = m_yDataRegion.rects();
    
    if ( m_yDataDirection == Qt::Horizontal )
    {
        QPoint topLeft = rect.topLeft();
        QPoint topRight = rect.topRight();
        
        int totalWidth = 0;
        int i;
        
        for ( i = 0; i < yDataRegionRects.size(); i++ )
        {
            if ( yDataRegionRects[i].contains( topLeft ) )
            {
                start = totalWidth + topLeft.x() - yDataRegionRects[i].topLeft().x();
                break;
            }
            totalWidth += yDataRegionRects[i].width();
        }
        
        for ( i; i < yDataRegionRects.size(); i++ )
        {
            if ( yDataRegionRects[i].contains( topRight ) )
            {
                end = totalWidth + topRight.x() - yDataRegionRects[i].topLeft().x();
                break;
            }
            totalWidth += yDataRegionRects[i].width();
        }
    }
    else
    {
    }
    
    qDebug() << "y data changed:" << start << end << "[this=" << this << "]";
    
    if ( !m_blockSignals && m_kdChartModel )
    {
        m_kdChartModel->dataChanged( m_kdChartModel->index( start, m_kdDataSetNumber ),
                                     m_kdChartModel->index( end,   m_kdDataSetNumber ) );
    }
}

void CellDataSet::xDataChanged( const QRect &region ) const
{
}

void CellDataSet::customDataChanged( const QRect &region ) const
{
}

void CellDataSet::labelDataChanged( const QRect &region ) const
{
}

void CellDataSet::categoryDataChanged( const QRect &region ) const
{
}

