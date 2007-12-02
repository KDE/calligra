/* This file is part of the KDE project

   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2007 Inge Wallin     <inge@lysator.liu.se>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef KCHART_CHART_SHAPE
#define KCHART_CHART_SHAPE


// Qt
#include <QModelIndex>

// KOffice
#include <KoShape.h>
#include "koChart.h"

// KChart
#include "kchart_export.h"
#include "kchart_global.h"


#define ChartShapeId "ChartShape"

class QAbstractItemModel;

namespace KDChart
{
    class Chart;
    class AbstractDiagram;
    class Position;
}

namespace KChart
{

class CHARTSHAPELIB_EXPORT ChartShape : public KoShape, public KoChart::ChartInterface
{
 public:
    ChartShape();
    virtual ~ChartShape();

    void refreshPixmap( QPainter& painter, const KoViewConverter& converter );

    KDChart::Chart* chart() const;

    /// reimplemented
    virtual void setModel( QAbstractItemModel *model, bool takeOwnershipOfModel = false );
    void setInternalModel( QAbstractItemModel *model );
    bool hasInternalModel();
    QAbstractItemModel *model();
    
    /// reimplemented
    virtual void paint( QPainter& painter, const KoViewConverter& converter );
    /// reimplemented
    virtual bool loadOdf( const KoXmlElement    &chartElement,
                          KoShapeLoadingContext &context );
    /// reimplemented
    virtual void saveOdf( KoShapeSavingContext & context ) const;

    OdfChartType    chartType() const;
    OdfChartSubtype chartSubtype() const;
    bool            threeDMode() const;

    ChartTypeOptions chartTypeOptions( OdfChartType type ) const;

 private:
    bool loadOdfLegend( const KoXmlElement    &legendElement, 
			KoShapeLoadingContext &context );
    bool loadOdfPlotarea( const KoXmlElement    &plotareaElement, 
			  KoShapeLoadingContext &context );
    bool loadOdfData( const KoXmlElement    &plotareaElement, 
                        KoShapeLoadingContext &context );
    bool loadOdfTitle( const KoXmlElement &titleElement, 
                        KoShapeLoadingContext &context );
    bool loadOdfSubTitle( const KoXmlElement &titleElement, 
                        KoShapeLoadingContext &context );
    bool loadOdfFooter ( const KoXmlElement &footerElement,
                         KoShapeLoadingContext &context ) 
    void saveOdfTitle( KoXmlWriter &bodyWriter,
                       KoGenStyles &mainStyles ) const;
    void saveOdfSubTitle( KoXmlWriter &bodyWriter,
                          KoGenStyles &mainStyles ) const;
    void saveOdfFooter( KoXmlWriter &bodyWriter,
                        KoGenStyles &mainStyles ) const;
    void saveOdfLegend( KoXmlWriter &bodyWriter,
                        KoGenStyles &mainStyles ) const;
    void saveOdfPlotarea( KoXmlWriter &xmlWriter,
                          KoGenStyles &mainStyles) const;
    void saveOdfData( KoXmlWriter& bodyWriter,
                      KoGenStyles& mainStyles ) const;
    QString saveOdfFont( KoGenStyles& mainStyles, 
                         const QFont& font,
                         const QColor& color ) const;


 public Q_SLOTS:
    virtual void update();
    void dataChanged( const QModelIndex &topLeft, const QModelIndex &bottomRight );

    /// Set new chart type and subtype.
    void setChartType( OdfChartType newType, 
		       OdfChartSubtype newSubtype = NoChartSubtype );
    void setChartSubtype( OdfChartSubtype newSubtype );
    void setThreeDMode( bool threeD );
    void setFirstRowIsLabel( bool b );
    void setFirstColumnIsLabel( bool b);
    void setDataDirection( Qt::Orientation orientation );
    void setLegendTitle( const QString& title );
    void setLegendTitleFont( const QFont& font );
    void setLegendFont( const QFont& font );
    void setLegendFontSize( int size );
    void setLegendSpacing( int spacing );
    void setLegendShowLines( bool b );
    void setLegendOrientation( Qt::Orientation orientation );
    void setLegendAlignment( Qt::Alignment alignment );
    void setLegendFixedPosition( KDChart::Position position );
    void modelChanged();

    void saveChartTypeOptions();
    void restoreChartTypeOptions( OdfChartType type );

 private:
    void setChartDefaults();
    void setDiagramDefaults( OdfChartType type = LastChartType );

    //static bool  isCartesian( OdfChartType type );
    //static bool  isPolar( OdfChartType type );
    Q_DISABLE_COPY( ChartShape )

 private:
    class Private;
    Private * const d;

};

} // namespace KChart


#endif // KCHART_CHART_SHAPE
