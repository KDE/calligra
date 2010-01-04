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


#ifndef KCHART_DATASET_H
#define KCHART_DATASET_H


// Qt
#include <QPen>

// KChart
#include "ChartShape.h"
#include "CellRegion.h"

class KoOdfLoadingContext;

namespace KChart {

class KDChartModel;


/**
 * @brief The DataSet class stores properties of a single data series.
 * 
 * A global chart type can be overridden by setting a specific type
 * on a data series.
 * 
 * To change properties of a single data point inside a data series,
 * use the DataPointAttributes class. (To be implemented)
 */

class CHARTSHAPELIB_EXPORT DataSet
{
public:
    DataSet( ChartProxyModel *model );
    ~DataSet();

    // Getter methods
    QString       title() const;
    ChartType     chartType() const;
    ChartSubtype  chartSubType() const;
    ChartType     globalChartType() const;
    ChartSubtype  globalChartSubType() const;
    Axis         *attachedAxis() const;

    ChartProxyModel   *model() const;

    bool showValues() const;
    bool showLabels() const;

    // Graphics properties for the visualization of this dataset.
    QPen   pen() const;
    QBrush brush() const;
    QColor color() const;
    int    number() const;

    bool showMeanValue() const;
    QPen meanValuePen() const;

    bool showLowerErrorIndicator() const;
    bool showUpperErrorIndicator() const;
    QPen errorIndicatorPen() const;
    ErrorCategory errorCategory() const;
    qreal errorPercentage() const;
    qreal errorMargin() const;
    qreal lowerErrorLimit() const;
    qreal upperErrorLimit() const;

    // Setter methods
    void setChartType( ChartType type );
    void setChartSubType( ChartSubtype type );
    void setGlobalChartType( ChartType type );
    void setGlobalChartSubType( ChartSubtype type );
    void setAttachedAxis( Axis *axis );

    void setShowValues( bool showValues );
    void setShowLabels( bool showLabels );

    void setPen( const QPen &pen );
    void setBrush( const QBrush &brush );
    void setColor( const QColor &color );
    void setNumber( int num );

    void setShowMeanValue( bool b );
    void setMeanValuePen( const QPen &pen );

    void setShowLowerErrorIndicator( bool b );
    void setShowUpperErrorIndicator( bool b );
    void setShowErrorIndicators( bool lower, bool upper );
    void setErrorIndicatorPen( const QPen &pen );
    void setErrorCategory( ErrorCategory category );
    void setErrorPercentage( qreal percentage );
    void setErrorMargin( qreal margin );
    void setLowerErrorLimit( qreal limit );
    void setUpperErrorLimit( qreal limit );

    QVariant xData( int index ) const;
    QVariant yData( int index ) const;
    QVariant customData( int index ) const;
    QVariant categoryData( int index ) const;
    QVariant labelData() const;

    CellRegion xDataRegion() const;
    CellRegion yDataRegion() const;
    CellRegion customDataRegion() const;
    CellRegion categoryDataRegion() const;
    CellRegion labelDataRegion() const;
    // TODO: Region for custom colors

    QString xDataRegionString() const;
    QString yDataRegionString() const;
    QString customDataRegionString() const;
    QString categoryDataRegionString() const;
    QString labelDataRegionString() const;

    void setXDataRegion( const CellRegion &region );
    void setYDataRegion( const CellRegion &region );
    void setCustomDataRegion( const CellRegion &region );
    void setCategoryDataRegion( const CellRegion &region );
    void setLabelDataRegion( const CellRegion &region );

    void setXDataRegionString( const QString &region );
    void setYDataRegionString( const QString &region );
    void setCustomDataRegionString( const QString &region );
    void setCategoryDataRegionString( const QString &region );
    void setLabelDataRegionString( const QString &region );

    int size() const;
    int dimension() const;

    void setKdDiagram( KDChart::AbstractDiagram *diagram );
    void setKdDataSetNumber( int number );

    KDChart::AbstractDiagram *kdDiagram() const;
    int kdDataSetNumber() const;

    // Called by the proxy model
    void yDataChanged( int start, int end ) const;
    void xDataChanged( int start, int end ) const;
    void customDataChanged( int start, int end ) const;
    void labelDataChanged() const;
    void categoryDataChanged( int start, int end ) const;

    void yDataChanged( const QRect &region ) const;
    void xDataChanged( const QRect &region ) const;
    void customDataChanged( const QRect &region ) const;
    void labelDataChanged( const QRect &region ) const;
    void categoryDataChanged( const QRect &region ) const;

    void setKdChartModel( KDChartModel *model );
    KDChartModel *kdChartModel() const;
    
    void blockSignals( bool block );

    bool loadOdf( const KoXmlElement &n,
                  KoOdfLoadingContext &context );

private:
    class Private;
    Private *const d;
};

} // Namespace KChart

#endif // KCHART_DATASET_H

