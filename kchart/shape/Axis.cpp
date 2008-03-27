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
#include "TextLabel.h"
#include "PlotArea.h"
#include "KDChartModel.h"
#include "DataSet.h"
#include "Legend.h"
#include "KDChartConvertions.h"

// KOffice
#include <KoShapeLoadingContext.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>
#include <KoGenStyles.h>
#include <KoXmlNS.h>

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
#include <KDChartThreeDBarAttributes>
#include <KDChartThreeDLineAttributes>

// Qt
#include <QList>
#include <QString>

using namespace KChart;

class Axis::Private
{
public:
    Private();
    ~Private();
    
    void createBarDiagram();
    void createLineDiagram();
    void createAreaDiagram();
    void createCircleDiagram();
    void createRadarDiagram();
    void createScatterDiagram();
    
    PlotArea *plotArea;
    
    AxisPosition position;
    TextLabel *title;
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
}

void Axis::Private::createBarDiagram()
{
    if ( kdBarDiagramModel == 0 )
        kdBarDiagramModel = new KDChartModel;
    if ( kdBarDiagram == 0 )
    {
        kdBarDiagram = new KDChart::BarDiagram( plotArea->kdChart(), kdPlane );
        kdBarDiagram->setModel( kdBarDiagramModel );
        kdBarDiagram->setPen( QPen( Qt::black, 0.0 ) );
        
        kdBarDiagram->addAxis( kdAxis );
        kdPlane->addDiagram( kdBarDiagram );
    }
}

void Axis::Private::createLineDiagram()
{
    if ( kdLineDiagramModel == 0 )
        kdLineDiagramModel = new KDChartModel;
    if ( kdLineDiagram == 0 )
    {
        kdLineDiagram = new KDChart::LineDiagram( plotArea->kdChart(), kdPlane );
        kdLineDiagram->setModel( kdLineDiagramModel );
        
        kdLineDiagram->addAxis( kdAxis );
        kdPlane->addDiagram( kdLineDiagram );
    }
}

void Axis::Private::createAreaDiagram()
{
    if ( kdAreaDiagramModel == 0 )
        kdAreaDiagramModel = new KDChartModel;
    if ( kdAreaDiagram == 0 )
    {
        kdAreaDiagram = new KDChart::LineDiagram( plotArea->kdChart(), kdPlane );
        KDChart::LineAttributes attr = kdAreaDiagram->lineAttributes();
        // Draw the area under the lines. This makes this diagram an area chart.
        attr.setDisplayArea( true );
        kdAreaDiagram->setLineAttributes( attr );
        kdAreaDiagram->setModel( kdAreaDiagramModel );
        
        kdAreaDiagram->addAxis( kdAxis );
        kdPlane->addDiagram( kdAreaDiagram );
    }
}

void Axis::Private::createCircleDiagram()
{
    if ( kdCircleDiagramModel == 0 )
        kdCircleDiagramModel = new KDChartModel;
    if ( kdCircleDiagram == 0 )
    {
        kdCircleDiagram = new KDChart::PieDiagram( plotArea->kdChart(), kdPolarPlane );
        kdCircleDiagram->setModel( kdCircleDiagramModel );
        
        //plotArea->parent()->legend()->kdLegend()->addDiagram( kdBarDiagram );
        //kdCircleDiagram->addAxis( kdAxis );
        kdPolarPlane->addDiagram( kdCircleDiagram );
    }
}

void Axis::Private::createRadarDiagram()
{
    if ( kdRadarDiagramModel == 0 )
        kdRadarDiagramModel = new KDChartModel;
    if ( kdRadarDiagram == 0 )
    {
        kdRadarDiagram = new KDChart::PolarDiagram( plotArea->kdChart(), kdPolarPlane );
        kdRadarDiagram->setModel( kdRadarDiagramModel );
        
        //plotArea->parent()->legend()->kdLegend()->addDiagram( kdBarDiagram );
        //kdRadarDiagram->addAxis( kdAxis );
        kdPolarPlane->addDiagram( kdRadarDiagram );
    }
}

void Axis::Private::createScatterDiagram()
{
    if ( kdScatterDiagramModel == 0 )
    {
        kdScatterDiagramModel = new KDChartModel;
        kdScatterDiagramModel->setDataDimensions( 2 );
    }
    if ( kdScatterDiagram == 0 )
    {
        kdScatterDiagram = new KDChart::Plotter( plotArea->kdChart(), kdPlane );
        kdScatterDiagram->setModel( kdScatterDiagramModel );
        
        //plotArea->parent()->legend()->kdLegend()->addDiagram( kdBarDiagram );
        kdScatterDiagram->addAxis( kdAxis );
        kdPlane->addDiagram( kdScatterDiagram );
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
    
    d->kdBarDiagram = new KDChart::BarDiagram( d->plotArea->kdChart(), d->kdPlane );
    d->kdBarDiagram->setPen( QPen( Qt::black, 0.0 ) );
    
    d->kdBarDiagram->addAxis( d->kdAxis );
    d->kdBarDiagramModel = new KDChartModel;
    d->kdBarDiagram->setModel( d->kdBarDiagramModel );
    d->kdPlane->addDiagram( d->kdBarDiagram );
    //d->plotArea->parent()->legend()->kdLegend()->addDiagram( d->kdBarDiagram );
    
    setShowGrid( false );
    
    d->title = new TextLabel( d->plotArea->parent() );
    
    d->plotAreaChartType = d->plotArea->chartType();
    d->plotAreaChartSubType = d->plotArea->chartSubType();
}

Axis::~Axis()
{	
}

AxisPosition Axis::position() const
{
    return d->position;
}

void Axis::setPosition( AxisPosition position )
{
    d->position = position;
    
    if ( position == LeftAxisPosition )
        d->title->rotate( -90 );
    else if ( position == RightAxisPosition )
        d->title->rotate( 90 );
    
    // KDChart
    d->kdAxis->setPosition( AxisPositionToKDChartAxisPosition( position ) );
    
    update();
}

TextLabel *Axis::title() const
{
    return d->title;
}

QString Axis::titleText() const
{
    return d->title->text();
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

bool Axis::attachDataSet( DataSet *dataSet )
{
    if ( d->dataSets.contains( dataSet ) )
        return false;
    d->dataSets.append( dataSet );
    dataSet->setAttachedAxis( this );
    
    ChartType chartType = dataSet->chartType();
    if ( chartType == LastChartType )
        chartType = d->plotArea->chartType();
    
    switch ( chartType )
    {
    case BarChartType:
    {
        if ( !d->kdBarDiagram )
            d->createBarDiagram();
        d->kdBarDiagramModel->addDataSet( dataSet );
        dataSet->setKdDiagram( d->kdBarDiagram );
        dataSet->setKdDataSetNumber( d->kdBarDiagramModel->dataSets().indexOf( dataSet ) );
    }
    break;
    case LineChartType:
    {
        if ( !d->kdLineDiagram )
            d->createLineDiagram();
        d->kdLineDiagramModel->addDataSet( dataSet );
        dataSet->setKdDiagram( d->kdLineDiagram );
        dataSet->setKdDataSetNumber( d->kdLineDiagramModel->dataSets().indexOf( dataSet ) );
    }
    break;
    }
    
    update();
    
    return true;
}

bool Axis::detachDataSet( DataSet *dataSet )
{
    if ( !d->dataSets.contains( dataSet ) )
        return false;
    d->dataSets.removeAll( dataSet );
    dataSet->setAttachedAxis( 0 );
    
    ChartType chartType = dataSet->chartType();
    if ( chartType == LastChartType )
        chartType = d->plotArea->chartType();
    
    switch ( chartType )
    {
    case BarChartType:
    {
        if ( d->kdBarDiagramModel )
        {
            // TODO (Johannes): Remove diagram if no
            // datasets are displayed on it anymore
            d->kdBarDiagramModel->removeDataSet( dataSet );
        }
        dataSet->setKdDiagram( 0 );
        dataSet->setKdDataSetNumber( 0 );
    }
    break;
    case LineChartType:
    {
        if ( d->kdLineDiagramModel )
        {
            // TODO (Johannes): Remove diagram if no
            // datasets are displayed on it anymore
            d->kdLineDiagramModel->removeDataSet( dataSet );
        }
        dataSet->setKdDiagram( 0 );
        dataSet->setKdDataSetNumber( 0 );
    }
    break;
    }
    
    update();
    
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
    
    update();
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
    
    update();
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
    
    update();
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
	
	update();
}

void Axis::setTitleText( const QString &text )
{
    d->title->setText( text );
    
    update();
}

Qt::Orientation Axis::orientation()
{
    if ( d->position == BottomAxisPosition || d->position == TopAxisPosition )
        return Qt::Horizontal;
    return Qt::Vertical;
}

bool Axis::loadOdf( const KoXmlElement &axisElement, KoShapeLoadingContext &context )
{
    d->kdAxis = new KDChart::CartesianAxis();
    if ( !axisElement.isNull() ) {
        if ( axisElement.hasAttributeNS( KoXmlNS::chart, "axis-name" ) ) {
            const QString name = axisElement.attributeNS( KoXmlNS::chart, "axis-name", QString() );
            setTitleText( name );
        }
        if ( axisElement.hasAttributeNS( KoXmlNS::chart, "dimension" ) ) {
            const QString dimension = axisElement.attributeNS( KoXmlNS::chart, "dimension", QString() );
            if ( dimension == "x" )
                setPosition( BottomAxisPosition );
            if ( dimension == "y" )
                setPosition( LeftAxisPosition );
        }
    }
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

    bodyWriter.addAttribute( "chart:axis-name", d->title->text() );

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
    if ( d->dataSets.isEmpty() || chartType == d->plotAreaChartType )
        return; // Return if there's nothing to do
    
    KDChartModel *oldModel = 0;
    KDChartModel *newModel = 0;
    KDChart::AbstractDiagram *newDiagram = 0;
    
    switch ( d->plotAreaChartType )
    {
    case BarChartType:
        oldModel = d->kdBarDiagramModel;
        break;
    case LineChartType:
        oldModel = d->kdLineDiagramModel;
        break;
    case AreaChartType:
        oldModel = d->kdAreaDiagramModel;
        break;
    case CircleChartType:
        oldModel = d->kdCircleDiagramModel;
        break;
    case RadarChartType:
        oldModel = d->kdRadarDiagramModel;
        break;
    case ScatterChartType:
        oldModel = d->kdScatterDiagramModel;
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
    
    foreach( DataSet *dataSet, d->dataSets )
    {
        if ( dataSet->chartType() != LastChartType )
            continue;
        if ( oldModel )
            oldModel->removeDataSet( dataSet );
        newModel->addDataSet( dataSet );
        dataSet->setKdDiagram( newDiagram );
        dataSet->setKdDataSetNumber( newModel->dataSets().indexOf( dataSet ) );
    }
    
    d->plotAreaChartType = chartType;
    
    d->kdPlane->layoutPlanes();
}

void Axis::plotAreaChartSubTypeChanged( ChartSubtype subType )
{
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

    update();
}
