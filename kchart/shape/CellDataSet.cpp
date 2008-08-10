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
    return data( m_xDataRegion, index );
}

QVariant CellDataSet::yData( int index ) const
{
    return data( m_yDataRegion, index );
}

QVariant CellDataSet::customData( int index ) const
{
    return data( m_customDataRegion, index );
}

QVariant CellDataSet::categoryData( int index ) const
{
    return data( m_categoryDataRegion, index );
}

QVariant CellDataSet::labelData() const
{
    QString label;
    
    const int cellCount = m_labelDataRegion.cellCount();
    for ( int i = 0; i < cellCount; i++ )
        label += data( m_labelDataRegion, i ).toString();

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
    
    return CellRegion::regionToString( m_xDataRegion.rects() );
}

QString CellDataSet::yDataRegionString() const
{
    if ( !m_model->spreadSheetModel() )
        return QString();
    
    return CellRegion::regionToString( m_yDataRegion.rects() );
}

QString CellDataSet::customDataRegionString() const
{
    if ( !m_model->spreadSheetModel() )
        return QString();
    
    return CellRegion::regionToString( m_customDataRegion.rects() );
}

QString CellDataSet::categoryDataRegionString() const
{
    if ( !m_model->spreadSheetModel() )
        return QString();
    
    return CellRegion::regionToString( m_categoryDataRegion.rects() );
}

QString CellDataSet::labelDataRegionString() const
{
    if ( !m_model->spreadSheetModel() )
        return QString();
    
    return CellRegion::regionToString( m_labelDataRegion.rects() );
}


void CellDataSet::setXDataRegion( const CellRegion &region )
{
    m_xDataRegion = region;
    updateSize();
}

void CellDataSet::setYDataRegion( const CellRegion &region )
{
    m_yDataRegion = region;
    updateSize();
    
    if ( !m_blockSignals && m_kdChartModel )
    {
        m_kdChartModel->dataChanged( m_kdChartModel->index( 0,          m_kdDataSetNumber ),
                                     m_kdChartModel->index( m_size - 1, m_kdDataSetNumber ) );
    }
}

void CellDataSet::setCustomDataRegion( const CellRegion &region )
{
    m_customDataRegion = region;
    updateSize();
}

void CellDataSet::setCategoryDataRegion( const CellRegion &region )
{
    m_categoryDataRegion = region;
    updateSize();
}

void CellDataSet::setLabelDataRegion( const CellRegion &region )
{
    m_labelDataRegion = region;
    updateSize();
}

void CellDataSet::setXDataRegionString( const QString &string )
{
    if ( !m_model->spreadSheetModel() )
        return;
    
    setXDataRegion( CellRegion::stringToRegion( string ) );
}

void CellDataSet::setYDataRegionString( const QString &string )
{
    if ( !m_model->spreadSheetModel() )
        return;
    
    setYDataRegion( CellRegion::stringToRegion( string ) );
}

void CellDataSet::setCustomDataRegionString( const QString &string )
{
    if ( !m_model->spreadSheetModel() )
        return;
    
    setCustomDataRegion( CellRegion::stringToRegion( string ) );
}

void CellDataSet::setCategoryDataRegionString( const QString &string )
{
    if ( !m_model->spreadSheetModel() )
        return;
    
    setCategoryDataRegion( CellRegion::stringToRegion( string ) );
}

void CellDataSet::setLabelDataRegionString( const QString &string )
{
    if ( !m_model->spreadSheetModel() )
        return;
    
    setLabelDataRegion( CellRegion::stringToRegion( string ) );
}


int CellDataSet::size() const
{
    return m_size > 0 ? m_size : 1;
}

void CellDataSet::yDataChanged( const QRect &rect ) const
{    
    int start, end;
    
    QVector<QRect> yDataRegionRects = m_yDataRegion.rects();
    
    if ( m_yDataRegion.orientation() == Qt::Horizontal )
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
        QPoint topLeft = rect.topLeft();
        QPoint bottomLeft = rect.bottomLeft();
        
        int totalHeight = 0;
        int i;
        for ( i = 0; i < yDataRegionRects.size(); i++ )
        {
            if ( yDataRegionRects[i].contains( topLeft ) )
            {
                start = totalHeight + topLeft.y() - yDataRegionRects[i].topLeft().y();
                break;
            }
            totalHeight += yDataRegionRects[i].height();
        }
        
        for ( i; i < yDataRegionRects.size(); i++ )
        {
            if ( yDataRegionRects[i].contains( bottomLeft ) )
            {
                end = totalHeight + bottomLeft.y() - yDataRegionRects[i].topLeft().y();
                break;
            }
            totalHeight += yDataRegionRects[i].height();
        }
    }
    
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

QVariant CellDataSet::data( const CellRegion &region, int index ) const
{
    if ( !region.isValid() )
        return QVariant(); 
    QAbstractItemModel *model = m_model->sourceModel();
    if ( !model )
        return QVariant();
    
    QVariant data;
        
    QPoint dataPoint = region.pointAtIndex( index );
    if ( dataPoint.x() > 0 )
    {
        if ( dataPoint.y() > 0 )
            data = model->data( model->index( dataPoint.y() - 1, dataPoint.x() - 1 ) );
        else if ( dataPoint.y() == 0 )
            data = model->headerData( dataPoint.x() - 1, Qt::Horizontal );
    }
    else if ( dataPoint.x() == 0 )
    {
        if ( dataPoint.y() > 0 )
            data = model->headerData( dataPoint.y() - 1, Qt::Vertical );
    }
    
    return data;
}
