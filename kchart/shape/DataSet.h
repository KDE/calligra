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

// Local
#include "ChartShape.h"
#include "CellRegion.h"

// Qt
#include <QPen>

namespace KChart {

class DataSet
{
public:
    DataSet( ProxyModel *model );
    virtual ~DataSet();

    // Getter methods
    QString title() const;
    ChartType chartType() const;
    ChartSubtype chartSubType() const;
    Axis *attachedAxis() const;

    ProxyModel *model() const;

    bool showValues() const;
    bool showLabels() const;

    QPen pen() const;
    QBrush brush() const;
    QColor color() const;

    bool showMeanValue() const;
    QPen meanValuePen() const;

    bool showLowerErrorIndicator() const;
    bool showUpperErrorIndicator() const;
    QPen errorIndicatorPen() const;
    ErrorCategory errorCategory() const;
    double errorPercentage() const;
    double errorMargin() const;
    double lowerErrorLimit() const;
    double upperErrorLimit() const;

    // Setter methods
    void setChartType( ChartType type );
    void setChartSubType( ChartSubtype type );
    void setAttachedAxis( Axis *axis );

    void setShowValues( bool showValues );
    void setShowLabels( bool showLabels );

    void setPen( const QPen &pen );
    void setBrush( const QBrush &brush );
    void setColor( const QColor &color );

    void setShowMeanValue( bool b );
    void setMeanValuePen( const QPen &pen );

    void setShowLowerErrorIndicator( bool b );
    void setShowUpperErrorIndicator( bool b );
    void setShowErrorIndicators( bool lower, bool upper );
    void setErrorIndicatorPen( const QPen &pen );
    void setErrorCategory( ErrorCategory category );
    void setErrorPercentage( double percentage );
    void setErrorMargin( double margin );
    void setLowerErrorLimit( double limit );
    void setUpperErrorLimit( double limit );

    virtual QVariant xData( int index ) const;
    virtual QVariant yData( int index ) const;
    virtual QVariant customData( int index ) const;
    virtual QVariant categoryData( int index ) const;
    virtual QVariant labelData() const;

    virtual CellRegion xDataRegion() const { return CellRegion(); };
    virtual CellRegion yDataRegion() const { return CellRegion(); };
    virtual CellRegion customDataRegion() const { return CellRegion(); };
    virtual CellRegion categoryDataRegion() const { return CellRegion(); };
    virtual CellRegion labelDataRegion() const { return CellRegion(); };
    // TODO: Region for custom colors

    virtual QString xDataRegionString() const { return QString(); };
    virtual QString yDataRegionString() const { return QString(); };
    virtual QString customDataRegionString() const { return QString(); };
    virtual QString categoryDataRegionString() const { return QString(); };
    virtual QString labelDataRegionString() const { return QString(); };

    virtual void setXDataRegion( const CellRegion &region ) {};
    virtual void setYDataRegion( const CellRegion &region ) {};
    virtual void setCustomDataRegion( const CellRegion &region ) {};
    virtual void setCategoryDataRegion( const CellRegion &region ) {};
    virtual void setLabelDataRegion( const CellRegion &region ) {};

    virtual void setXDataRegionString( const QString &region ) {};
    virtual void setYDataRegionString( const QString &region ) {};
    virtual void setCustomDataRegionString( const QString &region ) {};
    virtual void setCategoryDataRegionString( const QString &region ) {};
    virtual void setLabelDataRegionString( const QString &region ) {};

    virtual int size() const;

    void setKdDiagram( KDChart::AbstractDiagram *diagram );
    void setKdDataSetNumber( int number );

    KDChart::AbstractDiagram *kdDiagram() const;

    // Called by the proxy model
    void yDataChanged( int start, int end ) const;
    void xDataChanged( int start, int end ) const;
    void customDataChanged( int start, int end ) const;
    void labelDataChanged() const;
    void categoryDataChanged( int start, int end ) const;

    void yDataChanged( const QRect &region ) const {};
    void xDataChanged( const QRect &region ) const {};
    void customDataChanged( const QRect &region ) const {};
    void labelDataChanged( const QRect &region ) const {};
    void categoryDataChanged( const QRect &region ) const {};

    void setKdChartModel( KDChartModel *model );
    KDChartModel *kdChartModel() const;
    
    void blockSignals( bool block );

protected:
    ChartType m_chartType;
    ChartSubtype m_chartSubType;
    Axis *m_attachedAxis;
    bool m_showMeanValue;
    QPen m_meanValuePen;
    bool m_showValues;
    bool m_showLabels;
    bool m_showLowerErrorIndicator;
    bool m_showUpperErrorIndicator;
    QPen m_errorIndicatorPen;
    ErrorCategory m_errorCategory;
    double m_errorPercentage;
    double m_errorMargin;
    double m_lowerErrorLimit;
    double m_upperErrorLimit;
    QPen m_pen;
    QBrush m_brush;
    
    CellRegion m_xDataRegion;
    CellRegion m_yDataRegion;
    CellRegion m_customDataRegion;
    CellRegion m_labelDataRegion;
    CellRegion m_categoryDataRegion;
    
    Qt::Orientation m_xDataDirection;
    Qt::Orientation m_yDataDirection;
    Qt::Orientation m_customDataDirection;
    Qt::Orientation m_labelDataDirection;
    Qt::Orientation m_categoryDataDirection;
    
    ProxyModel *m_model;
    KoChart::ChartModel *m_spreadSheetModel;
    KDChart::AbstractDiagram *m_kdDiagram;
    int m_kdDataSetNumber;
    
    bool m_sourceIsSpreadSheet;
    
    KDChartModel *m_kdChartModel;
    
    int m_size;
    bool m_blockSignals;
};

} // Namespace KChart

#endif // KCHART_DATASET_H

