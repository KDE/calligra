/* This file is part of the KDE project

   Copyright 2007 Johannes Simon <johannes.simon@gmail.com>

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

// Local
#include "Axis.h"
#include "PlotArea.h"
#include "KDChartModel.h"
#include "DataSet.h"
#include "Legend.h"
#include "KDChartConvertions.h"
#include "ProxyModel.h"
#include "TextLabelDummy.h"

// KOffice
#include <KoShapeLoadingContext.h>
#include <KoShapeRegistry.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>
#include <KoGenStyles.h>
#include <KoXmlNS.h>
#include <KoTextShapeData.h>
#include <KoOdfStylesReader.h>
#include <KoUnit.h>

// KDChart
#include <KDChartChart>
#include <KDChartLegend>
#include <KDChartCartesianAxis>
#include <KDChartCartesianCoordinatePlane>
#include <KDChartGridAttributes>
#include <KDChartBarDiagram>
#include <KDChartLineDiagram>
#include <KDChartPieDiagram>
#include <KDChartPlotter>
#include <KDChartRingDiagram>
#include <KDChartPolarDiagram>
#include <KDChartBarAttributes>
#include <KDChartPieAttributes>
#include <KDChartThreeDBarAttributes>
#include <KDChartThreeDPieAttributes>
#include <KDChartThreeDLineAttributes>

// Qt
#include <QList>
#include <QString>
#include <QTextDocument>

using namespace KChart;

class Axis::Private
{
public:
    Private();
    ~Private();
    
    void registerKDChartModel( KDChartModel *model );
    void deregisterKDChartModel( KDChartModel *model );
    
    void createBarDiagram();
    void createLineDiagram();
    void createAreaDiagram();
    void createCircleDiagram();
    void createRadarDiagram();
    void createScatterDiagram();
    
    PlotArea *plotArea;
    
    AxisPosition position;
    KoShape *title;
    TextLabelData *titleData;
    QString id;
    QList<DataSet*> dataSets;
    double majorInterval;
    int minorIntervalDevisor;
    bool showInnerMinorTicks;
    bool showOuterMinorTicks;
    bool showInnerMajorTicks;
    bool showOuterMajorTicks;
    bool logarithmicScaling;
    bool showGrid;
    
    KDChart::CartesianAxis *kdAxis;
    KDChart::CartesianCoordinatePlane *kdPlane;
    KDChart::PolarCoordinatePlane *kdPolarPlane;
    
    KDChart::BarDiagram *kdBarDiagram;
    KDChart::LineDiagram *kdLineDiagram;
    KDChart::LineDiagram *kdAreaDiagram;
    KDChart::PieDiagram *kdCircleDiagram;
    KDChart::PolarDiagram *kdRadarDiagram;
    KDChart::Plotter *kdScatterDiagram;

    KDChartModel *kdBarDiagramModel;
    KDChartModel *kdLineDiagramModel;
    KDChartModel *kdAreaDiagramModel;
    KDChartModel *kdCircleDiagramModel;
    KDChartModel *kdRadarDiagramModel;
    KDChartModel *kdScatterDiagramModel;
    
    ChartType plotAreaChartType;
    ChartSubtype plotAreaChartSubType;
};

Axis::Private::Private()
{
    kdBarDiagram = 0;
    kdLineDiagram = 0;
    kdAreaDiagram = 0;
    kdCircleDiagram = 0;
    kdRadarDiagram = 0;
    kdScatterDiagram = 0;

    kdBarDiagramModel = 0;
    kdLineDiagramModel = 0;
    kdAreaDiagramModel = 0;
    kdCircleDiagramModel = 0;
    kdRadarDiagramModel = 0;
    kdScatterDiagramModel = 0;
}

Axis::Private::~Private()
{
    delete kdBarDiagram;
    delete kdLineDiagram;
    delete kdAreaDiagram;
    delete kdCircleDiagram;
    delete kdRadarDiagram;
    delete kdScatterDiagram;

    delete kdBarDiagramModel;
    delete kdLineDiagramModel;
    delete kdAreaDiagramModel;
    delete kdCircleDiagramModel;
    delete kdRadarDiagramModel;
    delete kdScatterDiagramModel;
}

void Axis::Private::registerKDChartModel( KDChartModel *model )
{
	QObject::connect( model, SIGNAL( modelReset() ), plotArea, SLOT( update() ) );
	QObject::connect( model, SIGNAL( rowsInserted( const QModelIndex&, int, int ) ), plotArea, SLOT( update() ) );
	QObject::connect( model, SIGNAL( rowsRemoved( const QModelIndex&, int, int ) ), plotArea, SLOT( update() ) );
	QObject::connect( model, SIGNAL( columnsInserted( const QModelIndex&, int, int ) ), plotArea, SLOT( update() ) );
	QObject::connect( model, SIGNAL( columnsRemoved( const QModelIndex&, int, int ) ), plotArea, SLOT( update() ) );
    
    QObject::connect( plotArea->proxyModel(), SIGNAL( modelReset() ),
    		          model,                  SLOT( emitReset() ) );
    QObject::connect( plotArea->proxyModel(), SIGNAL( columnsInserted( const QModelIndex&, int, int ) ),
                      model,                  SLOT( slotColumnsInserted( const QModelIndex&, int, int ) ) );
}
void Axis::Private::deregisterKDChartModel( KDChartModel *model )
{
	QObject::disconnect( model, SIGNAL( modelReset() ), plotArea, SLOT( update() ) );
	QObject::disconnect( model, SIGNAL( rowsInserted( const QModelIndex&, int, int ) ), plotArea, SLOT( update() ) );
	QObject::disconnect( model, SIGNAL( rowsRemoved( const QModelIndex&, int, int ) ), plotArea, SLOT( update() ) );
	QObject::disconnect( model, SIGNAL( columnsInserted( const QModelIndex&, int, int ) ), plotArea, SLOT( update() ) );
	QObject::disconnect( model, SIGNAL( columnsRemoved( const QModelIndex&, int, int ) ), plotArea, SLOT( update() ) );
    
	QObject::disconnect( plotArea->proxyModel(), SIGNAL( modelReset() ),
    		             model,                  SLOT( emitReset() ) );
    QObject::disconnect( plotArea->proxyModel(), SIGNAL( columnsInserted( const QModelIndex&, int, int ) ),
                         model,                  SLOT( slotColumnsInserted( const QModelIndex&, int, int ) ) );
}

void Axis::Private::createBarDiagram()
{
    if ( kdBarDiagramModel == 0 )
    {
        kdBarDiagramModel = new KDChartModel;
        registerKDChartModel( kdBarDiagramModel );
    }
    if ( kdBarDiagram == 0 )
    {
        kdBarDiagram = new KDChart::BarDiagram( plotArea->kdChart(), kdPlane );
        kdBarDiagram->setModel( kdBarDiagramModel );
        kdBarDiagram->setPen( QPen( Qt::black, 0.0 ) );
        
        kdBarDiagram->addAxis( kdAxis );
        kdPlane->addDiagram( kdBarDiagram );

        Q_ASSERT( plotArea );
        foreach ( Axis *axis, plotArea->axes() )
        {
            if ( axis->dimension() == XAxisDimension )
                kdBarDiagram->addAxis( axis->kdAxis() );
        }
        
        plotArea->parent()->legend()->kdLegend()->addDiagram( kdBarDiagram );
    }
}

void Axis::Private::createLineDiagram()
{
    if ( kdLineDiagramModel == 0 )
    {
        kdLineDiagramModel = new KDChartModel;
        registerKDChartModel( kdLineDiagramModel );
    }
    if ( kdLineDiagram == 0 )
    {
        kdLineDiagram = new KDChart::LineDiagram( plotArea->kdChart(), kdPlane );
        kdLineDiagram->setModel( kdLineDiagramModel );
        
        kdLineDiagram->addAxis( kdAxis );
        kdPlane->addDiagram( kdLineDiagram );

        Q_ASSERT( plotArea );
        foreach ( Axis *axis, plotArea->axes() )
        {
            if ( axis->dimension() == XAxisDimension )
                kdLineDiagram->addAxis( axis->kdAxis() );
        }

        plotArea->parent()->legend()->kdLegend()->addDiagram( kdLineDiagram );
    }
}

void Axis::Private::createAreaDiagram()
{
    if ( kdAreaDiagramModel == 0 )
    {
        kdAreaDiagramModel = new KDChartModel;
        registerKDChartModel( kdAreaDiagramModel );
    }
    if ( kdAreaDiagram == 0 )
    {
        kdAreaDiagram = new KDChart::LineDiagram( plotArea->kdChart(), kdPlane );
        KDChart::LineAttributes attr = kdAreaDiagram->lineAttributes();
        // Draw the area under the lines. This makes this diagram an area chart.
        attr.setDisplayArea( true );
        kdAreaDiagram->setLineAttributes( attr );
        kdAreaDiagram->setModel( kdAreaDiagramModel );
        kdAreaDiagram->setPen( QPen( Qt::black, 0.0 ) );
        
        kdAreaDiagram->addAxis( kdAxis );
        kdPlane->addDiagram( kdAreaDiagram );

        Q_ASSERT( plotArea );
        foreach ( Axis *axis, plotArea->axes() )
        {
            if ( axis->dimension() == XAxisDimension )
                kdAreaDiagram->addAxis( axis->kdAxis() );
        }

        plotArea->parent()->legend()->kdLegend()->addDiagram( kdAreaDiagram );
    }
}

void Axis::Private::createCircleDiagram()
{
    if ( kdCircleDiagramModel == 0 )
    {
        kdCircleDiagramModel = new KDChartModel;
        registerKDChartModel( kdCircleDiagramModel );
    }
    if ( kdCircleDiagram == 0 )
    {
        kdCircleDiagram = new KDChart::PieDiagram( plotArea->kdChart(), kdPolarPlane );
        kdCircleDiagram->setModel( kdCircleDiagramModel );

        plotArea->parent()->legend()->kdLegend()->addDiagram( kdCircleDiagram );
        kdPolarPlane->addDiagram( kdCircleDiagram );
    }
}

void Axis::Private::createRadarDiagram()
{
    if ( kdRadarDiagramModel == 0 )
    {
        kdRadarDiagramModel = new KDChartModel;
        registerKDChartModel( kdRadarDiagramModel );
    }
    if ( kdRadarDiagram == 0 )
    {
        kdRadarDiagram = new KDChart::PolarDiagram( plotArea->kdChart(), kdPolarPlane );
        kdRadarDiagram->setModel( kdRadarDiagramModel );

        plotArea->parent()->legend()->kdLegend()->addDiagram( kdRadarDiagram );
        kdPolarPlane->addDiagram( kdRadarDiagram );
    }
}

void Axis::Private::createScatterDiagram()
{
    if ( kdScatterDiagramModel == 0 )
    {
        kdScatterDiagramModel = new KDChartModel;
        registerKDChartModel( kdScatterDiagramModel );
        kdScatterDiagramModel->setDataDimensions( 2 );
    }
    if ( kdScatterDiagram == 0 )
    {
        kdScatterDiagram = new KDChart::Plotter( plotArea->kdChart(), kdPlane );
        kdScatterDiagram->setModel( kdScatterDiagramModel );
        
        kdScatterDiagram->addAxis( kdAxis );
        kdPlane->addDiagram( kdScatterDiagram );

        Q_ASSERT( plotArea );
        foreach ( Axis *axis, plotArea->axes() )
        {
            if ( axis->dimension() == XAxisDimension )
                kdScatterDiagram->addAxis( axis->kdAxis() );
        }

        plotArea->parent()->legend()->kdLegend()->addDiagram( kdScatterDiagram );
    }
}

Axis::Axis( PlotArea *parent )
    : d( new Private )
{
    Q_ASSERT( parent );
    
    d->plotArea = parent;
    d->kdAxis = new KDChart::CartesianAxis();
    d->kdPlane = new KDChart::CartesianCoordinatePlane();
    d->kdPlane->setReferenceCoordinatePlane( d->plotArea->kdPlane() );
    d->kdPolarPlane = new KDChart::PolarCoordinatePlane();
    d->kdPolarPlane->setReferenceCoordinatePlane( d->plotArea->kdPlane() );
    
    KDChart::GridAttributes gridAttributes = d->kdPlane->gridAttributes( Qt::Horizontal );
    gridAttributes.setGridVisible( false );
    d->kdPlane->setGridAttributes( Qt::Horizontal, gridAttributes );
    d->plotArea->kdChart()->addCoordinatePlane( d->kdPlane );
    
    gridAttributes = d->kdPolarPlane->gridAttributes( Qt::Horizontal );
    gridAttributes.setGridVisible( false );
    d->kdPolarPlane->setGridAttributes( Qt::Horizontal, gridAttributes );
    
    //d->createBarDiagram();
    //d->plotArea->parent()->legend()->kdLegend()->addDiagram( d->kdBarDiagram );
    
    setShowGrid( false );
    
    d->title = KoShapeRegistry::instance()->value( TextShapeId )->createDefaultShapeAndInit( 0 );
    if ( d->title )
    {
        d->titleData = qobject_cast<TextLabelData*>( d->title->userData() );
        if ( d->titleData == 0 )
        {
            d->titleData = new TextLabelData;
            d->title->setUserData( d->titleData );
        }
    }
    else
    {
        d->title = new TextLabelDummy;
        d->titleData = new TextLabelData;
        d->title->setUserData( d->titleData );
    }
    d->title->setSize( QSizeF( CM_TO_POINT( 5 ), CM_TO_POINT( 0.75 ) ) );
    
    d->plotArea->parent()->addChild( d->title );
    
    d->plotAreaChartType = d->plotArea->chartType();
    d->plotAreaChartSubType = d->plotArea->chartSubType();
    
    connect( d->plotArea, SIGNAL( gapBetweenBarsChanged( int ) ),
             this,        SLOT( setGapBetweenBars( int ) ) );
    connect( d->plotArea, SIGNAL( gapBetweenSetsChanged( int ) ),
             this,        SLOT( setGapBetweenSets( int ) ) );
    connect( d->plotArea, SIGNAL( pieExplodeFactorChanged( DataSet*, int ) ),
             this,        SLOT( setPieExplodeFactor( DataSet*, int ) ) );
}

Axis::~Axis()
{
    Q_ASSERT( d->plotArea );
    d->plotArea->parent()->KoShapeContainer::removeChild( d->title );
    delete d;
}

AxisPosition Axis::position() const
{
    return d->position;
}

void Axis::setPosition( AxisPosition position )
{
    d->position = position;
    
    if ( position == LeftAxisPosition )
        d->title->rotate( -90 - d->title->rotation() );
    else if ( position == RightAxisPosition )
        d->title->rotate( 90 - d->title->rotation() );
    
    // KDChart
    d->kdAxis->setPosition( AxisPositionToKDChartAxisPosition( position ) );
    
    requestRepaint();
}

KoShape *Axis::title() const
{
    return d->title;
}

QString Axis::titleText() const
{
    return d->titleData->document()->toPlainText();
}

QString Axis::id() const
{
    return d->id;
}

AxisDimension Axis::dimension() const
{
    if ( d->position == LeftAxisPosition || d->position == RightAxisPosition )
        return YAxisDimension;
    return XAxisDimension;
}

QList<DataSet*> Axis::dataSets() const
{
    return d->dataSets;
}

bool Axis::attachDataSet( DataSet *dataSet, bool silent )
{
    if ( d->dataSets.contains( dataSet ) )
        return false;
    d->dataSets.append( dataSet );
    dataSet->setAttachedAxis( this );
    
    ChartType chartType = dataSet->chartType();
    if ( chartType == LastChartType )
        chartType = d->plotAreaChartType;
    
    KDChart::AbstractDiagram *diagram = 0;
    KDChartModel *model = 0;
    
    switch ( chartType )
    {
    case BarChartType:
        if ( !d->kdBarDiagram )
            d->createBarDiagram();
        model = d->kdBarDiagramModel;
        diagram = d->kdBarDiagram;
        break;
    case LineChartType:
        if ( !d->kdLineDiagram )
            d->createLineDiagram();
        model = d->kdLineDiagramModel;
        diagram = d->kdLineDiagram;
        break;
    case AreaChartType:
        if ( !d->kdAreaDiagram )
            d->createAreaDiagram();
        model = d->kdAreaDiagramModel;
        diagram = d->kdAreaDiagram;
        break;
    case CircleChartType:
        if ( !d->kdCircleDiagram )
            d->createCircleDiagram();
        model = d->kdCircleDiagramModel;
        diagram = d->kdCircleDiagram;
        break;
    case RadarChartType:
        if ( !d->kdRadarDiagram )
            d->createRadarDiagram();
        model = d->kdRadarDiagramModel;
        diagram = d->kdRadarDiagram;
        break;
    case ScatterChartType:
        if ( !d->kdScatterDiagram )
            d->createScatterDiagram();
        model = d->kdScatterDiagramModel;
        diagram = d->kdScatterDiagram;
        break;
    }
    
    Q_ASSERT( model );
    Q_ASSERT( diagram );

    dataSet->setKdDiagram( diagram );
    if ( model )
    {
        model->addDataSet( dataSet, silent );
        dataSet->setKdDataSetNumber( model->dataSets().indexOf( dataSet ) );
    }
    
    if ( !silent )
    {
        layoutPlanes();
        requestRepaint();
    }
    
    return true;
}

bool Axis::detachDataSet( DataSet *dataSet, bool silent )
{
    if ( !d->dataSets.contains( dataSet ) )
        return false;
    d->dataSets.removeAll( dataSet );
    dataSet->setAttachedAxis( 0 );
    
    ChartType chartType = dataSet->chartType();
    if ( chartType == LastChartType )
        chartType = d->plotAreaChartType;
    
    KDChart::AbstractDiagram **oldDiagram = 0;
    KDChartModel **oldModel = 0;
    
    switch ( chartType )
    {
    case BarChartType:
        oldModel = &d->kdBarDiagramModel;
        oldDiagram = (KDChart::AbstractDiagram**)&d->kdBarDiagram;
        break;
    case LineChartType:
        oldModel = &d->kdLineDiagramModel;
        oldDiagram = (KDChart::AbstractDiagram**)&d->kdLineDiagram;
        break;
    case AreaChartType:
        oldModel = &d->kdAreaDiagramModel;
        oldDiagram = (KDChart::AbstractDiagram**)&d->kdAreaDiagram;
        break;
    case CircleChartType:
        oldModel = &d->kdCircleDiagramModel;
        oldDiagram = (KDChart::AbstractDiagram**)&d->kdCircleDiagram;
        break;
    case RadarChartType:
        oldModel = &d->kdRadarDiagramModel;
        oldDiagram = (KDChart::AbstractDiagram**)&d->kdRadarDiagram;
        break;
    case ScatterChartType:
        oldModel = &d->kdScatterDiagramModel;
        oldDiagram = (KDChart::AbstractDiagram**)&d->kdScatterDiagram;
        break;
    }
    
    if ( oldModel && *oldModel )
    {
        if ( (*oldModel)->columnCount() == 1 )
        {
            if ( (*oldDiagram)->coordinatePlane() )
                (*oldDiagram)->coordinatePlane()->takeDiagram( (*oldDiagram) );
            if ( d->plotArea->parent()->legend()->kdLegend() )
            {
                d->plotArea->parent()->legend()->kdLegend()->removeDiagram( (*oldDiagram) );
            }
            Q_ASSERT( oldDiagram );
            Q_ASSERT( *oldDiagram );
            if ( *oldDiagram )
                delete *oldDiagram;
            delete *oldModel;
            *oldModel = 0;
            *oldDiagram = 0;
        }
        else
            (*oldModel)->removeDataSet( dataSet, silent );
    }
    
    dataSet->setKdDiagram( 0 );
    dataSet->setKdDataSetNumber( 0 );
    

    if ( !silent )
    {
        layoutPlanes();
        requestRepaint();
    }
    
    return true; 
}

double Axis::majorInterval() const
{
    return d->majorInterval;
}

void Axis::setMajorInterval( double interval )
{
    // Don't overwrite if automatic interval is being requested ( for interval = 0 )
    if ( interval != 0.0 )
        d->majorInterval = interval;
    
    // KDChart
    KDChart::CartesianCoordinatePlane *plane = dynamic_cast<KDChart::CartesianCoordinatePlane*>(d->kdPlane);
    if ( !plane )
        return;
    
    KDChart::GridAttributes attributes = plane->gridAttributes( orientation() );
    attributes.setGridStepWidth( interval );
    plane->setGridAttributes( orientation(), attributes );
    
    requestRepaint();
}

double Axis::minorInterval() const
{
    return ( d->majorInterval / (double)d->minorIntervalDevisor ); 
}

void Axis::setMinorInterval( double interval )
{
    setMinorIntervalDevisor( (int)( d->majorInterval / interval ) );
}

int Axis::minorIntervalDevisor() const
{
    return d->minorIntervalDevisor;
}

void Axis::setMinorIntervalDevisor( int devisor )
{
    d->minorIntervalDevisor = devisor;
    
    // KDChart
    KDChart::CartesianCoordinatePlane *plane = dynamic_cast<KDChart::CartesianCoordinatePlane*>(d->kdPlane);
        if ( !plane )
            return;
        
        
    KDChart::GridAttributes attributes = plane->gridAttributes( orientation() );
    attributes.setGridSubStepWidth( d->majorInterval / devisor );
    plane->setGridAttributes( orientation(), attributes );
    
    requestRepaint();
}

bool Axis::showInnerMinorTicks() const
{
    return d->showInnerMinorTicks;
}

bool Axis::showOuterMinorTicks() const
{
    return d->showOuterMinorTicks;
}

bool Axis::showInnerMajorTicks() const
{
    return d->showInnerMinorTicks;
}

bool Axis::showOuterMajorTicks() const
{
    return d->showOuterMajorTicks;
}

void Axis::setScalingLogarithmic( bool logarithmicScaling )
{
    d->logarithmicScaling = logarithmicScaling;
    
    if ( dimension() != YAxisDimension )
        return;

    d->kdPlane->setAxesCalcModeY( d->logarithmicScaling ? KDChart::AbstractCoordinatePlane::Logarithmic : KDChart::AbstractCoordinatePlane::Linear );
    
    requestRepaint();
}

bool Axis::scalingIsLogarithmic() const
{
    return d->logarithmicScaling;
}

bool Axis::showGrid() const
{
    return d->showGrid;
}

void Axis::setShowGrid( bool showGrid )
{
    d->showGrid = showGrid;

    // KDChart
    KDChart::CartesianCoordinatePlane *plane = dynamic_cast<KDChart::CartesianCoordinatePlane*>(d->kdPlane);
        if ( !plane )
            return;
        
        
    KDChart::GridAttributes attributes = plane->gridAttributes( orientation() );
	attributes.setGridVisible( d->showGrid );
	plane->setGridAttributes( orientation(), attributes );
    
    requestRepaint();
}

void Axis::setTitleText( const QString &text )
{
    d->titleData->document()->setHtml( "<div align=center>" + text + "</div>" );
}

Qt::Orientation Axis::orientation()
{
    if ( d->position == BottomAxisPosition || d->position == TopAxisPosition )
        return Qt::Horizontal;
    return Qt::Vertical;
}

bool Axis::loadOdf( const KoXmlElement &axisElement, const KoOdfStylesReader &stylesReader )
{
    d->title->setVisible( false );
    
    KDChart::GridAttributes attr = d->kdPlane->gridAttributes( orientation() );
    attr.setGridVisible( false );
    attr.setSubGridVisible( false );
    d->kdPlane->setGridAttributes( orientation(), attr );
    
    if ( !axisElement.isNull() ) {
        KoXmlElement n;
        for ( n = axisElement.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement() )
        {
            if ( n.namespaceURI() != KoXmlNS::chart )
                continue;
            if ( n.localName() == "title" )
            {
                if ( n.hasAttributeNS( KoXmlNS::svg, "x" ) && n.hasAttributeNS( KoXmlNS::svg, "y" ) )
                {
                    const qreal x = KoUnit::parseValue( n.attributeNS( KoXmlNS::svg, "x" ) );
                    const qreal y = KoUnit::parseValue( n.attributeNS( KoXmlNS::svg, "y" ) );
                    d->title->setPosition( QPointF( x, y ) );
                }
                
                if ( n.hasAttributeNS( KoXmlNS::svg, "width" ) && n.hasAttributeNS( KoXmlNS::svg, "height" ) )
                {
                    const qreal width = KoUnit::parseValue( n.attributeNS( KoXmlNS::svg, "width" ) );
                    const qreal height = KoUnit::parseValue( n.attributeNS( KoXmlNS::svg, "height" ) );
                    d->title->setSize( QSizeF( width, height ) );
                }
                
                if ( n.hasAttributeNS( KoXmlNS::chart, "style-name" ) )
                {
                    const QString styleName = n.attributeNS( KoXmlNS::chart, "style-name" );
                    const KoXmlElement *styleElement = stylesReader.findStyle( styleName, "chart" );
                    if ( styleElement )
                    {
                        KoXmlElement textPropertiesElement = styleElement->namedItemNS( KoXmlNS::style, "text-properties" ).toElement();
                        if ( !textPropertiesElement.isNull() )
                        {
                            if ( textPropertiesElement.hasAttributeNS( KoXmlNS::fo, "font-size" ) )
                            {
                                const qreal fontSize = KoUnit::parseValue( textPropertiesElement.attributeNS( KoXmlNS::fo, "font-size" ) );
                                QFont font = d->titleData->document()->defaultFont();
                                font.setPointSizeF( fontSize );
                                d->titleData->document()->setDefaultFont( font );
                            }
                        }
                    }
                }
                
                const KoXmlElement textElement = KoXml::namedItemNS( n, KoXmlNS::text, "p" );
                if ( !textElement.isNull() )
                {
                    d->title->setVisible( true );
                    setTitleText( textElement.text() );
                }
                else
                {
                    qWarning() << "Error: Axis' <chart:title> element contains no <text:p>";
                }
            }
            else if ( n.localName() == "grid" )
            {
                bool major = false;
                if ( n.hasAttributeNS( KoXmlNS::chart, "class" ) )
                {
                    const QString className = n.attributeNS( KoXmlNS::chart, "class" );
                    if ( className == "major" )
                        major = true;
                }
                if ( n.hasAttributeNS( KoXmlNS::chart, "style-name" ) )
                {
                    const QString styleName = n.attributeNS( KoXmlNS::chart, "style-name" );
                    const KoXmlElement *styleElement = stylesReader.findStyle( styleName, "chart" );
                    if ( styleElement )
                    {
                        KoXmlElement graphicPropertiesElement = styleElement->namedItemNS( KoXmlNS::style, "graphic-properties" ).toElement();
                        if ( !graphicPropertiesElement.isNull() )
                        {
                            if ( graphicPropertiesElement.hasAttributeNS( KoXmlNS::svg, "stroke-color" ) )
                            {
                                const QString strokeColor = graphicPropertiesElement.attributeNS( KoXmlNS::svg, "stroke-color" );
                                KDChart::GridAttributes attr = d->kdPlane->gridAttributes( orientation() );
                                attr.setGridVisible( true );
                                if ( major )
                                {
                                    attr.setGridPen( QColor( strokeColor ) );
                                    attr.setSubGridVisible( false );
                                }
                                else
                                {
                                    attr.setSubGridPen( QColor( strokeColor ) );
                                    attr.setSubGridVisible( true );
                                }
                                d->kdPlane->setGridAttributes( orientation(), attr );
                            }
                        }
                    }
                }
            }
        }
        
        if ( axisElement.hasAttributeNS( KoXmlNS::chart, "axis-name" ) ) {
            const QString name = axisElement.attributeNS( KoXmlNS::chart, "axis-name", QString() );
            //setTitleText( name );
        }
        if ( axisElement.hasAttributeNS( KoXmlNS::chart, "dimension" ) ) {
            const QString dimension = axisElement.attributeNS( KoXmlNS::chart, "dimension", QString() );
            if ( dimension == "x" )
                setPosition( BottomAxisPosition );
            if ( dimension == "y" )
                setPosition( LeftAxisPosition );
        }
    }
    
    requestRepaint();
    
    return true;
}

void Axis::saveOdf( KoXmlWriter &bodyWriter, KoGenStyles &mainStyles )
{
    bodyWriter.startElement( "chart:axis" );

    KoGenStyle axisStyle( KoGenStyle::StyleAuto, "chart" );
    axisStyle.addProperty( "chart:display-label", "true" );

    const QString styleName = mainStyles.lookup( axisStyle, "ch" );
    bodyWriter.addAttribute( "chart:style-name", styleName );

    // TODO scale: logarithmic/linear
    // TODO visibility

    if ( dimension() == XAxisDimension )
        bodyWriter.addAttribute( "chart:dimension", "x" );
    else if ( dimension() == YAxisDimension )
        bodyWriter.addAttribute( "chart:dimension", "y" );

    bodyWriter.addAttribute( "chart:axis-name", d->titleData->document()->toPlainText() );

    bodyWriter.endElement(); // chart:axis
}

void Axis::update() const
{
    if ( d->kdBarDiagram )
    {
        d->kdBarDiagram->doItemsLayout();
        d->kdBarDiagram->update();
    }
    if ( d->kdLineDiagram )
    {
        d->kdLineDiagram->doItemsLayout();
        d->kdLineDiagram->update();
    }
    d->plotArea->parent()->relayout();
}

KDChart::CartesianAxis *Axis::kdAxis() const
{
    return d->kdAxis;
}

KDChart::AbstractCoordinatePlane *Axis::kdPlane() const
{
    return d->kdPlane;
}

void Axis::plotAreaChartTypeChanged( ChartType chartType )
{
    // Return if there's nothing to do
    if ( chartType == d->plotAreaChartType )
        return;
    if ( d->dataSets.isEmpty() )
    {
        d->plotAreaChartType = chartType;
        return;
    }
    
    KDChartModel **oldModel = 0;
    KDChartModel *newModel = 0;
    KDChart::AbstractDiagram **oldDiagram = 0;
    KDChart::AbstractDiagram *newDiagram = 0;
    
    switch ( d->plotAreaChartType )
    {
    case BarChartType:
        oldModel = &d->kdBarDiagramModel;
        oldDiagram = (KDChart::AbstractDiagram**)&d->kdBarDiagram;
        break;
    case LineChartType:
        oldModel = &d->kdLineDiagramModel;
        oldDiagram = (KDChart::AbstractDiagram**)&d->kdLineDiagram;
        break;
    case AreaChartType:
        oldModel = &d->kdAreaDiagramModel;
        oldDiagram = (KDChart::AbstractDiagram**)&d->kdAreaDiagram;
        break;
    case CircleChartType:
        oldModel = &d->kdCircleDiagramModel;
        oldDiagram = (KDChart::AbstractDiagram**)&d->kdCircleDiagram;
        break;
    case RadarChartType:
        oldModel = &d->kdRadarDiagramModel;
        oldDiagram = (KDChart::AbstractDiagram**)&d->kdRadarDiagram;
        break;
    case ScatterChartType:
        oldModel = &d->kdScatterDiagramModel;
        oldDiagram = (KDChart::AbstractDiagram**)&d->kdScatterDiagram;
        break;
    }
    
    switch ( chartType )
    {
    case BarChartType:
        if ( !d->kdBarDiagram )
           d->createBarDiagram();
        newModel = d->kdBarDiagramModel;
        newDiagram = d->kdBarDiagram;
        break;
    case LineChartType:
        if ( !d->kdLineDiagram )
           d->createLineDiagram();
        newModel = d->kdLineDiagramModel;
        newDiagram = d->kdLineDiagram;
        break;
    case AreaChartType:
        if ( !d->kdAreaDiagram )
           d->createAreaDiagram();
        newModel = d->kdAreaDiagramModel;
        newDiagram = d->kdAreaDiagram;
        break;
    case CircleChartType:
        if ( !d->kdCircleDiagram )
           d->createCircleDiagram();
        newModel = d->kdCircleDiagramModel;
        newDiagram = d->kdCircleDiagram;
        break;
    case RadarChartType:
        if ( !d->kdRadarDiagram )
           d->createRadarDiagram();
        newModel = d->kdRadarDiagramModel;
        newDiagram = d->kdRadarDiagram;
        break;
    case ScatterChartType:
        if ( !d->kdScatterDiagram )
           d->createScatterDiagram();
        newModel = d->kdScatterDiagramModel;
        newDiagram = d->kdScatterDiagram;
        break;
    }
    
    Q_ASSERT( newModel );
    
    if ( isCartesian( d->plotAreaChartType ) && isPolar( chartType ) )
    {
        d->plotArea->kdChart()->takeCoordinatePlane( d->kdPlane );
        d->plotArea->kdChart()->addCoordinatePlane( d->kdPolarPlane );
    }
    if ( isPolar( d->plotAreaChartType ) && isCartesian( chartType ) )
    {
        d->plotArea->kdChart()->takeCoordinatePlane( d->kdPolarPlane );
        d->plotArea->kdChart()->addCoordinatePlane( d->kdPlane );
    }
    
    if (    isPolar( chartType ) && !isPolar( d->plotAreaChartType )
         || !isPolar( chartType ) && isPolar( d->plotAreaChartType ) )
    {
        foreach( DataSet *dataSet, d->dataSets )
        {
            if ( dataSet->chartType() != LastChartType )
            {
                dataSet->setChartType( LastChartType );
                dataSet->setChartSubType( NoChartSubtype );
            }
        }
    }
    
    foreach( DataSet *dataSet, d->dataSets )
    {
        if ( dataSet->chartType() != LastChartType )
            continue;
        newModel->addDataSet( dataSet );
        if ( oldModel && *oldModel )
        {
        	if ( (*oldModel)->columnCount() == 1 )
        	{
        		if ( (*oldDiagram)->coordinatePlane() )
        			(*oldDiagram)->coordinatePlane()->takeDiagram( (*oldDiagram) );
        		if ( d->plotArea->parent()->legend()->kdLegend() )
        		{
        		    d->plotArea->parent()->legend()->kdLegend()->removeDiagram( (*oldDiagram) );
        		}
        		Q_ASSERT( oldDiagram );
        		Q_ASSERT( *oldDiagram );
		        if ( *oldDiagram )
		            delete *oldDiagram;
		        delete *oldModel;
		        *oldModel = 0;
		        *oldDiagram = 0;
        	}
        	else
        		(*oldModel)->removeDataSet( dataSet );
        }
        dataSet->setKdDiagram( newDiagram );
        dataSet->setKdDataSetNumber( newModel->dataSets().indexOf( dataSet ) );
        dataSet->setGlobalChartType( chartType );
    }
    
    if ( *oldModel )
    	(*oldModel)->emitReset();
    if ( newModel )
    	newModel->emitReset();
    
    d->plotAreaChartType = chartType;
    
    d->kdPlane->layoutPlanes();
    
    requestRepaint();
}

void Axis::plotAreaChartSubTypeChanged( ChartSubtype subType )
{
    switch ( d->plotAreaChartType )
    {
    case BarChartType:
        if ( d->kdBarDiagram )
        {
            KDChart::BarDiagram::BarType type;
            switch ( subType )
            {
            case StackedChartSubtype:
                type = KDChart::BarDiagram::Stacked; break;
            case PercentChartSubtype:
                type = KDChart::BarDiagram::Percent; break;
            default:
                type = KDChart::BarDiagram::Normal;
            }
            d->kdBarDiagram->setType( type );
        }
        break;
    case LineChartType:
        if ( d->kdLineDiagram )
        {
            KDChart::LineDiagram::LineType type;
            switch ( subType )
            {
            case StackedChartSubtype:
                type = KDChart::LineDiagram::Stacked; break;
            case PercentChartSubtype:
                type = KDChart::LineDiagram::Percent; break;
            default:
                type = KDChart::LineDiagram::Normal;
            }
            d->kdLineDiagram->setType( type );
        }
        break;
    case AreaChartType:
        if ( d->kdAreaDiagram )
        {
            KDChart::LineDiagram::LineType type;
            switch ( subType )
            {
            case StackedChartSubtype:
                type = KDChart::LineDiagram::Stacked; break;
            case PercentChartSubtype:
                type = KDChart::LineDiagram::Percent; break;
            default:
                type = KDChart::LineDiagram::Normal;
            }
            d->kdAreaDiagram->setType( type );
        }
        break;
    }
    foreach ( DataSet *dataSet, d->dataSets )
        dataSet->setGlobalChartSubType( subType );
        
}

void Axis::registerKdXAxis( KDChart::CartesianAxis *axis )
{
    if ( d->kdBarDiagram )
        d->kdBarDiagram->addAxis( axis );
    if ( d->kdLineDiagram )
        d->kdLineDiagram->addAxis( axis );
    if ( d->kdAreaDiagram )
        d->kdAreaDiagram->addAxis( axis );
    if ( d->kdScatterDiagram )
        d->kdScatterDiagram->addAxis( axis );
}

void Axis::deregisterKdXAxis( KDChart::CartesianAxis *axis )
{
    if ( d->kdBarDiagram )
        d->kdBarDiagram->takeAxis( axis );
    if ( d->kdLineDiagram )
        d->kdLineDiagram->takeAxis( axis );
    if ( d->kdAreaDiagram )
        d->kdAreaDiagram->takeAxis( axis );
    if ( d->kdScatterDiagram )
        d->kdScatterDiagram->takeAxis( axis );
}

void Axis::setThreeD( bool threeD )
{
    // KDChart
    if ( d->kdBarDiagram )
    {
        KDChart::ThreeDBarAttributes attributes( d->kdBarDiagram->threeDBarAttributes() );
        attributes.setEnabled( threeD );
        attributes.setDepth( 15.0 );
        d->kdBarDiagram->setThreeDBarAttributes( attributes );
    }
    if ( d->kdLineDiagram )
    {
        KDChart::ThreeDLineAttributes attributes( d->kdLineDiagram->threeDLineAttributes() );
        attributes.setEnabled( threeD );
        attributes.setDepth( 15.0 );
        d->kdLineDiagram->setThreeDLineAttributes( attributes );
    }
    if ( d->kdAreaDiagram )
    {
        KDChart::ThreeDLineAttributes attributes( d->kdAreaDiagram->threeDLineAttributes() );
        attributes.setEnabled( threeD );
        attributes.setDepth( 15.0 );
        d->kdAreaDiagram->setThreeDLineAttributes( attributes );
    }
    if ( d->kdCircleDiagram )
    {
        KDChart::ThreeDPieAttributes attributes( d->kdCircleDiagram->threeDPieAttributes() );
        attributes.setEnabled( threeD );
        attributes.setDepth( 15.0 );
        d->kdCircleDiagram->setThreeDPieAttributes( attributes );
    }
    
    requestRepaint();
}

void Axis::requestRepaint() const
{
    d->plotArea->requestRepaint();
}

void Axis::layoutPlanes()
{
    if ( d->kdPlane )
        d->kdPlane->layoutPlanes();
    if ( d->kdPolarPlane )
        d->kdPolarPlane->layoutPlanes();
}

void Axis::setGapBetweenBars( int percent )
{
    if ( d->kdBarDiagram )
    {
        KDChart::BarAttributes attributes = d->kdBarDiagram->barAttributes();
        attributes.setBarGapFactor( (float)percent / 100.0 );
        d->kdBarDiagram->setBarAttributes( attributes );
    }
    
    requestRepaint();
}

void Axis::setGapBetweenSets( int percent )
{
    if ( d->kdBarDiagram )
    {
        KDChart::BarAttributes attributes = d->kdBarDiagram->barAttributes();
        attributes.setGroupGapFactor( (float)percent / 100.0 );
        d->kdBarDiagram->setBarAttributes( attributes );
    }
    
    requestRepaint();
}

void Axis::setPieExplodeFactor( DataSet *dataSet, int percent )
{
    if ( d->kdCircleDiagram )
    {
        KDChart::PieAttributes attributes = d->kdCircleDiagram->pieAttributes();
        attributes.setExplodeFactor( (float)percent / 100.0 );
        d->kdCircleDiagram->setPieAttributes( dataSet->kdDataSetNumber(), attributes );
    }
    
    requestRepaint();
}

#include "Axis.moc"
