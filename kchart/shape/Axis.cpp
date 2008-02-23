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

// KOffice
#include <KoShapeLoadingContext.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>
#include <KoGenStyles.h>
#include <KoXmlNS.h>

// KDChart
#include <KDChartCartesianAxis>
#include <KDChartCartesianCoordinatePlane>
#include <KDChartGridAttributes>
#include <KDChartAbstractDiagram>

// Qt
#include <QList>
#include <QString>

using namespace KChart;

class Axis::Private
{
public:
    Private();
    ~Private();
    
    PlotArea *plotArea;
    
    AxisPosition position;
    TextLabel *title;
    QString id;
    AxisDimension dimension;
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
    KDChart::AbstractDiagram *kdDiagram;
};

Axis::Private::Private()
{
}

Axis::Private::~Private()
{
}

Axis::Axis( PlotArea *parent )
    : d( new Private() )
{
    d->plotArea = parent;
    d->kdAxis = new KDChart::CartesianAxis();
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
    
    // KDChart
    d->kdAxis->setPosition( AxisPositionToKDChartAxisPosition( position ) );
    update();
}

TextLabel *Axis::title() const
{
    return d->title;
}

QString Axis::id() const
{
    return d->id;
}

AxisDimension Axis::dimension() const
{
    return d->dimension;
}

void Axis::setDimension( AxisDimension dimension )
{
    d->dimension = dimension;
    update();
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
    return true;
}

bool Axis::detachDataSet( DataSet *dataSet )
{
    if ( !d->dataSets.contains( dataSet ) )
        return false;
    d->dataSets.removeAll( dataSet );
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
    
    KDChart::CartesianCoordinatePlane *plane = dynamic_cast<KDChart::CartesianCoordinatePlane*>( d->kdAxis->diagram()->coordinatePlane() );
    Q_ASSERT( plane );
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
    
    KDChart::CartesianCoordinatePlane *plane = dynamic_cast<KDChart::CartesianCoordinatePlane*>( d->kdAxis->diagram()->coordinatePlane() );
    Q_ASSERT( plane );
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
    
    KDChart::CartesianCoordinatePlane *plane = dynamic_cast<KDChart::CartesianCoordinatePlane*>( d->kdAxis->diagram()->coordinatePlane() );
    Q_ASSERT( plane );
    if ( !plane )
        return;

    plane->setAxesCalcModeY( d->logarithmicScaling ? KDChart::AbstractCoordinatePlane::Logarithmic : KDChart::AbstractCoordinatePlane::Linear );
    
    //d->kdAxis->coordinatePlane()->relayout();
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
	KDChart::CartesianCoordinatePlane *plane = dynamic_cast<KDChart::CartesianCoordinatePlane*>( d->kdAxis->diagram()->coordinatePlane() );
    Q_ASSERT( plane );
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
    
    // KDChart
    d->kdAxis->setTitleText( text );
    
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
};

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

void Axis::update()
{
    // TODO: Update diagram here
}

KDChart::CartesianAxis *Axis::kdAxis() const
{
    return d->kdAxis;
}

KDChart::AbstractDiagram *Axis::kdDiagram() const
{
    return d->kdDiagram;
}

