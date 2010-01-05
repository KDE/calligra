/* This file is part of the KDE project

   Copyright 2007-2008 Johannes Simon <johannes.simon@gmail.com>
   Copyright 2008-2009 Inge Wallin    <inge@lysator.liu.se>

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


// Own
#include "DataSet.h"

// Qt
#include <QAbstractItemModel>
#include <QString>
#include <QPen>
#include <QColor>

// KDE
#include <KLocale>

// KDChart
#include <KDChartDataValueAttributes>
#include <KDChartTextAttributes>
#include <KDChartAbstractDiagram>
#include <KDChartMeasure>
#include "KDChartModel.h"

// KChart
#include "ChartProxyModel.h"
#include "Axis.h"
#include "PlotArea.h"

// KOffice
#include <KoXmlNS.h>
#include <KoOdfGraphicStyles.h>
#include <KoXmlReader.h>
#include <KoOdfLoadingContext.h>


using namespace KChart;

class DataSet::Private
{
public:
    Private( DataSet *parent );
    ~Private();
    
    void       updateSize();
    void       updateDiagram() const;
    ChartType  effectiveChartType() const;
    bool       isValidDataPoint( const QPoint &point ) const;
    QVariant   data( const CellRegion &region, int index ) const;
    
    DataSet      *parent;
    
    ChartType     chartType;
    ChartSubtype  chartSubType;
    ChartType     globalChartType;
    ChartSubtype  globalChartSubType;

    Axis *attachedAxis;
    bool showMeanValue;
    QPen meanValuePen;
    bool showValues;
    bool showLabels;
    bool showLowerErrorIndicator;
    bool showUpperErrorIndicator;
    QPen errorIndicatorPen;
    ErrorCategory errorCategory;
    qreal errorPercentage;
    qreal errorMargin;
    qreal lowerErrorLimit;
    qreal upperErrorLimit;
    QPen pen;
    QBrush brush;
    int num;

    // The different CellRegions for a dataset
    // Note: These are all 1-dimensional, i.e. vectors.
    CellRegion labelDataRegion; // one cell that holds the label
    CellRegion yDataRegion;     // normal y values
    CellRegion xDataRegion;     // x values -- only for scatter & bubble charts
    CellRegion customDataRegion;// used for bubble width in bubble charts
    CellRegion categoryDataRegion; // x labels -- same for all datasets
    
    ChartProxyModel *model;
    KDChart::AbstractDiagram *kdDiagram;
    int kdDataSetNumber;
    
    KDChartModel *kdChartModel;
    
    int size;
    bool blockSignals;
};

DataSet::Private::Private( DataSet *parent )
{
    this->parent = parent;
    num = -1;
    globalChartType = LastChartType;
    globalChartSubType = NoChartSubtype;
    chartType = LastChartType;
    chartSubType = NoChartSubtype;
    kdChartModel = 0;
    kdDataSetNumber = -1;
    showMeanValue = false;
    showValues = false;
    showLabels = false;
    showLowerErrorIndicator = false;
    showUpperErrorIndicator = false;
    errorPercentage = 0.0;
    errorMargin = 0.0;
    lowerErrorLimit = 0.0;
    upperErrorLimit = 0.0;
    brush = QColor( Qt::white );
    pen = QPen( Qt::black );
    kdDiagram = 0;
    attachedAxis = 0;
    size = 0;
    blockSignals = false;
}

DataSet::Private::~Private()
{
}

void DataSet::Private::updateSize()
{
    int newSize = 0;
    newSize = qMax( newSize, xDataRegion.cellCount() );
    newSize = qMax( newSize, yDataRegion.cellCount() );
    newSize = qMax( newSize, customDataRegion.cellCount() );
    newSize = qMax( newSize, categoryDataRegion.cellCount() );
    
    size = newSize;
    // FIXME: The first test below can *never* fail because of the
    //        assignment just above. (iw)
    if ( newSize != size && !blockSignals && kdChartModel ) {
        kdChartModel->dataSetSizeChanged( parent, size );
    }
}

/**
 * Returns the effective chart type of this data set, i.e.
 * returns the chart type of the diagram this data set is
 * attached to if no chart type is set, or otherwise this data
 * set's chart type.
 */
ChartType DataSet::Private::effectiveChartType() const
{
    if ( attachedAxis && chartType == LastChartType )
        return attachedAxis->plotArea()->chartType();
    return chartType;
}

void DataSet::Private::updateDiagram() const
{
    if ( kdDiagram && kdDataSetNumber >= 0 && size > 0 ) {

	// FIXME: This should be done in a more proper way.
	// The problem here is that line diagrams don't use the brush
	// to paint their lines, but the pen. We on the other hand set
	// the data set color on the brush, not the pen.
        if ( effectiveChartType() == LineChartType )
            kdDiagram->setPen( kdDataSetNumber, brush.color() );
        else
            kdDiagram->setPen( kdDataSetNumber, pen );
        kdDiagram->setBrush( kdDataSetNumber, brush );
    }
}

bool DataSet::Private::isValidDataPoint( const QPoint &point ) const
{
    if ( point.y() < 0 || point.x() < 0 )
        return false;

    // We can't point to horizontal and vertical header data at the same time
    if ( point.x() == 0 && point.y() == 0 )
        return false;

    return true;
}

QVariant DataSet::Private::data( const CellRegion &region, int index ) const
{
    if ( !region.isValid() )
        return QVariant(); 

    QAbstractItemModel *model = this->model->sourceModel();
    if ( !model )
        return QVariant();
    
    // The result
    QVariant data;
    
    // Convert the given index in this dataset to a data point in the
    // source model.
    QPoint dataPoint = region.pointAtIndex( index );

    // FIXME: Why not use this immediately if true?
    const bool verticalHeaderData   = dataPoint.x() == 0;
    const bool horizontalHeaderData = dataPoint.y() == 0;

    // Check if the data point is valid
    const bool validDataPoint = isValidDataPoint( dataPoint );

    // Remove, since it makes kspread crash when inserting a chart for
    // a 1x1 cell region.
    //Q_ASSERT( validDataPoint );
    if ( !validDataPoint )
        return QVariant();    

    // The top-left point is (1,1). (0,y) or (x,0) refers to header data.
    const int row = dataPoint.y() - 1;
    const int col = dataPoint.x() - 1;

    if ( verticalHeaderData )
        data = model->headerData( row, Qt::Vertical );
    else if ( horizontalHeaderData )
        data = model->headerData( col, Qt::Horizontal );
    else {
        const QModelIndex &index = model->index( row, col );
        // FIXME: This causes a crash in KSpread when a document is loaded.
        //Q_ASSERT( index.isValid() );
        if ( index.isValid() )
            data = model->data( index );
    }
    
    return data;
}

DataSet::DataSet( ChartProxyModel *proxyModel )
    : d( new Private( this ) )
{
    d->model = proxyModel;
}

DataSet::~DataSet()
{
    if ( d->attachedAxis )
        d->attachedAxis->detachDataSet( this, true );

    delete d;
}


ChartType DataSet::chartType() const
{
    return d->chartType;
}

ChartSubtype DataSet::chartSubType() const
{
    return d->chartSubType;
}

ChartType DataSet::globalChartType() const
{
    return d->globalChartType;
}

ChartSubtype DataSet::globalChartSubType() const
{
    return d->globalChartSubType;
}

Axis *DataSet::attachedAxis() const
{
    return d->attachedAxis;
}

// FIXME: Should this method also be called proxyModel?
ChartProxyModel *DataSet::model() const
{
    return d->model;
}

bool DataSet::showMeanValue() const
{
    return d->showMeanValue;
}

QPen DataSet::meanValuePen() const
{
    return d->meanValuePen;   
}

bool DataSet::showLowerErrorIndicator() const
{
    return d->showLowerErrorIndicator;
}

bool DataSet::showUpperErrorIndicator() const
{
    return d->showUpperErrorIndicator;
}

QPen DataSet::errorIndicatorPen() const
{
    return d->errorIndicatorPen;
}

ErrorCategory DataSet::errorCategory() const
{
    return d->errorCategory;
}

qreal DataSet::errorPercentage() const
{
    return d->errorPercentage;
}

qreal DataSet::errorMargin() const
{
    return d->errorMargin;   
}

qreal DataSet::lowerErrorLimit() const
{
    return d->lowerErrorLimit;
}

qreal DataSet::upperErrorLimit() const
{
    return d->upperErrorLimit;
}


void DataSet::setChartType( ChartType type )
{
    if ( type == d->chartType )
        return;
    
    Axis  *axis = d->attachedAxis;
    axis->detachDataSet( this );
    
    d->chartType = type;
    
    axis->attachDataSet( this );
}

void DataSet::setChartSubType( ChartSubtype subType )
{
    if ( subType == d->chartSubType )
        return;
    
    Axis *axis = d->attachedAxis;
    axis->detachDataSet( this );
    
    d->chartSubType = subType;
    
    axis->attachDataSet( this );
}

void DataSet::setGlobalChartType( ChartType type )
{
    d->globalChartType = type;
}

void DataSet::setGlobalChartSubType( ChartSubtype type )
{
    d->globalChartSubType = type;
}


void DataSet::setAttachedAxis( Axis *axis )
{
    d->attachedAxis = axis;
}

bool DataSet::showValues() const
{
    return d->showValues;
}

bool DataSet::showLabels() const
{
    return d->showLabels;
}

void DataSet::setShowValues( bool showValues )
{
    if ( !d->kdDiagram )
        return;

    d->showValues = showValues;
    
    KDChart::DataValueAttributes attributes = d->kdDiagram->dataValueAttributes( d->kdDataSetNumber );
    attributes.setVisible( showValues );

    // FIXME: This should be a dynamic property that can be changed by
    //        the user (for 2.1)
    KDChart::TextAttributes textAttributes = attributes.textAttributes();
    textAttributes.setFontSize( KDChart::Measure( 6, KDChartEnums::MeasureCalculationModeAbsolute ) );
    attributes.setTextAttributes( textAttributes );
    d->kdDiagram->setDataValueAttributes( d->kdDataSetNumber, attributes );
    
    if ( d->attachedAxis )
        d->attachedAxis->update();
}

void DataSet::setShowLabels( bool showLabels )
{
    d->showLabels = showLabels;
}

QPen DataSet::pen() const
{
    return d->pen;
}

QBrush DataSet::brush() const
{
    return d->brush;
}

void DataSet::setPen( const QPen &pen )
{
    d->pen = pen;
    d->updateDiagram();
    if ( !d->blockSignals && d->attachedAxis )
        d->attachedAxis->update();
}

void DataSet::setBrush( const QBrush &brush )
{
    d->brush = brush;
    d->updateDiagram();
    if ( !d->blockSignals && d->attachedAxis )
        d->attachedAxis->update();
}

QColor DataSet::color() const
{
    return d->brush.color();
}

void DataSet::setColor( const QColor &color )
{
    QBrush brush = d->brush;
    brush.setColor( color );
    setBrush( brush );
}

int DataSet::number() const
{
    return d->num;
}

void DataSet::setNumber( int num )
{
    if ( !d->blockSignals && d->attachedAxis )
        d->attachedAxis->detachDataSet( this );
    
    d->num = num;
    
    if ( !d->blockSignals && d->attachedAxis )
        d->attachedAxis->attachDataSet( this );
}

void DataSet::setShowMeanValue( bool show )
{
    d->showMeanValue = show;
}

void DataSet::setMeanValuePen( const QPen &pen )
{
    d->meanValuePen = pen;
}

void DataSet::setShowLowerErrorIndicator( bool show )
{
    d->showLowerErrorIndicator = show;
}

void DataSet::setShowUpperErrorIndicator( bool show )
{
    d->showUpperErrorIndicator = show;
}

void DataSet::setShowErrorIndicators( bool lower, bool upper )
{
    setShowLowerErrorIndicator( lower );
    setShowUpperErrorIndicator( upper );
}

void DataSet::setErrorIndicatorPen( const QPen &pen )
{
    d->errorIndicatorPen = pen;
}

void DataSet::setErrorCategory( ErrorCategory category )
{
    d->errorCategory = category;
}

void DataSet::setErrorPercentage( qreal percentage )
{
    d->errorPercentage = percentage;
}

void DataSet::setErrorMargin( qreal margin )
{
    d->errorMargin = margin;
}

void DataSet::setLowerErrorLimit( qreal limit )
{
    d->lowerErrorLimit = limit;
}

void DataSet::setUpperErrorLimit( qreal limit )
{
    d->upperErrorLimit = limit;
}

QVariant DataSet::xData( int index ) const
{
    return d->data( d->xDataRegion, index );
}

QVariant DataSet::yData( int index ) const
{
    return d->data( d->yDataRegion, index );
}

QVariant DataSet::customData( int index ) const
{
    return d->data( d->customDataRegion, index );
}

QVariant DataSet::categoryData( int index ) const
{
    return d->data( d->categoryDataRegion, index );
}

QVariant DataSet::labelData() const
{
    QString label;
    
    const int cellCount = d->labelDataRegion.cellCount();
    for ( int i = 0; i < cellCount; i++ )
        label += d->data( d->labelDataRegion, i ).toString();
    
    if ( label.isEmpty() )
        label = i18n( "Series %1", number() );

    return QVariant( label );
}


CellRegion DataSet::xDataRegion() const
{
    return d->xDataRegion;
}

CellRegion DataSet::yDataRegion() const
{
    return d->yDataRegion;
}

CellRegion DataSet::customDataRegion() const
{
    return d->customDataRegion;
}

CellRegion DataSet::categoryDataRegion() const
{
    return d->categoryDataRegion;
}

CellRegion DataSet::labelDataRegion() const
{
    return d->labelDataRegion;
}

QString DataSet::xDataRegionString() const
{
    return CellRegion::regionToString( d->xDataRegion.rects() );
}

QString DataSet::yDataRegionString() const
{
    return CellRegion::regionToString( d->yDataRegion.rects() );
}

QString DataSet::customDataRegionString() const
{
    return CellRegion::regionToString( d->customDataRegion.rects() );
}

QString DataSet::categoryDataRegionString() const
{
    return CellRegion::regionToString( d->categoryDataRegion.rects() );
}

QString DataSet::labelDataRegionString() const
{
    return CellRegion::regionToString( d->labelDataRegion.rects() );
}


void DataSet::setXDataRegion( const CellRegion &region )
{
    d->xDataRegion = region;
    d->updateSize();
}

void DataSet::setYDataRegion( const CellRegion &region )
{
    d->yDataRegion = region;
    d->updateSize();
    
    if ( !d->blockSignals && d->kdChartModel ) {
        d->kdChartModel->dataChanged( d->kdChartModel->index( 0,           d->kdDataSetNumber ),
				      d->kdChartModel->index( d->size - 1, d->kdDataSetNumber ) );
    }
}

void DataSet::setCustomDataRegion( const CellRegion &region )
{
    d->customDataRegion = region;
    d->updateSize();
}

void DataSet::setCategoryDataRegion( const CellRegion &region )
{
    d->categoryDataRegion = region;
    d->updateSize();
}

void DataSet::setLabelDataRegion( const CellRegion &region )
{
    d->labelDataRegion = region;
    d->updateSize();
}

void DataSet::setXDataRegionString( const QString &string )
{
    setXDataRegion( CellRegion::stringToRegion( string ) );
}

void DataSet::setYDataRegionString( const QString &string )
{
    setYDataRegion( CellRegion::stringToRegion( string ) );
}

void DataSet::setCustomDataRegionString( const QString &string )
{
    setCustomDataRegion( CellRegion::stringToRegion( string ) );
}

void DataSet::setCategoryDataRegionString( const QString &string )
{
    setCategoryDataRegion( CellRegion::stringToRegion( string ) );
}

void DataSet::setLabelDataRegionString( const QString &string )
{
    setLabelDataRegion( CellRegion::stringToRegion( string ) );
}


int DataSet::size() const
{
    return d->size > 0 ? d->size : 1;
}

void DataSet::yDataChanged( const QRect &rect ) const
{    
    int  start = -1;
    int  end = -1;
    
    QVector<QRect> yDataRegionRects = d->yDataRegion.rects();
    
    if ( d->yDataRegion.orientation() == Qt::Horizontal ) {
        QPoint  topLeft  = rect.topLeft();
        QPoint  topRight = rect.topRight();
        
        int totalWidth = 0;
        int i;
        
        for ( i = 0; i < yDataRegionRects.size(); i++ ) {
            if ( yDataRegionRects[i].contains( topLeft ) ) {
                start = totalWidth + topLeft.x() - yDataRegionRects[i].topLeft().x();
                break;
            }
            totalWidth += yDataRegionRects[i].width();
        }
        
        for ( ; i < yDataRegionRects.size(); i++ ) {
            if ( yDataRegionRects[i].contains( topRight ) ) {
                end = totalWidth + topRight.x() - yDataRegionRects[i].topLeft().x();
                break;
            }

            totalWidth += yDataRegionRects[i].width();
        }
    }
    else {
        QPoint  topLeft    = rect.topLeft();
        QPoint  bottomLeft = rect.bottomLeft();
        
        int totalHeight = 0;
        int i;
        for ( i = 0; i < yDataRegionRects.size(); i++ ) {
            if ( yDataRegionRects[i].contains( topLeft ) ) {
                start = totalHeight + topLeft.y() - yDataRegionRects[i].topLeft().y();
                break;
            }

            totalHeight += yDataRegionRects[i].height();
        }
        
        for ( ; i < yDataRegionRects.size(); i++ ) {
            if ( yDataRegionRects[i].contains( bottomLeft ) ) {
                end = totalHeight + bottomLeft.y() - yDataRegionRects[i].topLeft().y();
                break;
            }

            totalHeight += yDataRegionRects[i].height();
        }
    }
    
    if ( !d->blockSignals && d->kdChartModel ) {
        d->kdChartModel->dataChanged( d->kdChartModel->index( start, d->kdDataSetNumber ),
                                     d->kdChartModel->index( end,   d->kdDataSetNumber ) );
    }
}

void DataSet::xDataChanged( const QRect &region ) const
{
    Q_UNUSED( region );
}

void DataSet::customDataChanged( const QRect &region ) const
{
    Q_UNUSED( region );
}

void DataSet::labelDataChanged( const QRect &region ) const
{
    Q_UNUSED( region );
}

void DataSet::categoryDataChanged( const QRect &region ) const
{
    Q_UNUSED( region );
}

int DataSet::dimension() const
{
    const ChartType chartType = ( d->chartType != LastChartType
				  ? d->chartType
				  : d->globalChartType );
    // FIXME BUG: Ring, Surface
    switch ( chartType ) {
    case BarChartType:
    case AreaChartType:
    case LineChartType:
    case CircleChartType:
    case RadarChartType:
    case SurfaceChartType:
	return 1;

    case RingChartType:
    case ScatterChartType:
    case GanttChartType:
	return 2;

    case BubbleChartType:
	return 3;

    case StockChartType:
        return 4;

	// We can only determine the dimension if
	// a chart type is set
    case LastChartType:
	return 0;
    }
	
    // Avoid warnings from the compiler.
    return 0;
}

void DataSet::setKdDiagram( KDChart::AbstractDiagram *diagram )
{
    d->kdDiagram = diagram;
}

KDChart::AbstractDiagram *DataSet::kdDiagram() const
{
    return d->kdDiagram;
}

int DataSet::kdDataSetNumber() const
{
    return d->kdDataSetNumber;
}

void DataSet::setKdDataSetNumber( int number )
{
    d->kdDataSetNumber = number;
    d->updateDiagram();
}

void DataSet::setKdChartModel( KDChartModel *model )
{
    d->kdChartModel = model;
}

KDChartModel *DataSet::kdChartModel() const
{
    return d->kdChartModel;
}

void DataSet::blockSignals( bool block )
{
    d->blockSignals = block;
}

bool DataSet::loadOdf( const KoXmlElement &n,
                       KoOdfLoadingContext &context )
{

    KoStyleStack &styleStack = context.styleStack();

    if ( n.hasAttributeNS( KoXmlNS::chart, "style-name" ) ) {
        styleStack.clear();
        context.fillStyleStack( n, KoXmlNS::chart, "style-name", "chart" );

        //styleStack.setTypeProperties( "chart" );

        // FIXME: Load Pie explode factors
        //if ( styleStack.hasProperty( KoXmlNS::chart, "pie-offset" ) )
        //    setPieExplodeFactor( dataSet, styleStack.property( KoXmlNS::chart, "pie-offset" ).toInt() );

        styleStack.setTypeProperties( "graphic" );

        if ( styleStack.hasProperty( KoXmlNS::draw, "stroke" ) ) {
            QString stroke = styleStack.property( KoXmlNS::draw, "stroke" );
            if( stroke == "solid" || stroke == "dash" ) {
                QPen pen = KoOdfGraphicStyles::loadOdfStrokeStyle( styleStack, stroke, context.stylesReader() );
                setPen( pen );
            }
        }

        if ( styleStack.hasProperty( KoXmlNS::draw, "fill" ) ) {
            QString fill = styleStack.property( KoXmlNS::draw, "fill" );
            QBrush brush;
            if ( fill == "solid" || fill == "hatch" ) {
                brush = KoOdfGraphicStyles::loadOdfFillStyle( styleStack, fill, context.stylesReader() );
            } else if ( fill == "gradient" ) {
                brush = KoOdfGraphicStyles::loadOdfGradientStyle( styleStack, context.stylesReader(), QSizeF( 5.0, 60.0 ) );
            } else if ( fill == "bitmap" )
                brush = KoOdfGraphicStyles::loadOdfPatternStyle( styleStack, context, QSizeF( 5.0, 60.0 ) );
            setBrush( brush );
        } else {
            setColor( defaultDataSetColor( number() ) );
        }
    }

    if ( n.hasAttributeNS( KoXmlNS::chart, "values-cell-range-address" ) ) {
        const QString region = n.attributeNS( KoXmlNS::chart, "values-cell-range-address", QString() );
        setYDataRegionString( region );
    }
    if ( n.hasAttributeNS( KoXmlNS::chart, "label-cell-address" ) ) {
        const QString region = n.attributeNS( KoXmlNS::chart, "label-cell-address", QString() );
        setLabelDataRegionString( region );
    }

    KoXmlElement m;
    forEachElement ( m, n ) {
        if ( m.namespaceURI() != KoXmlNS::chart )
            continue;
        // FIXME: Load data points
    }

    return true;
}
