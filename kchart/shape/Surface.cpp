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
#include "Surface.h"
#include "PlotArea.h"

// Qt
#include <QPointF>
#include <QBrush>
#include <QPen>
#include <QDebug>

// KOffice
#include <KoXmlReader.h>
#include <KoXmlWriter.h>
#include <KoXmlNS.h>
#include <KoOdfStylesReader.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeSavingContext.h>
#include <KoOdfLoadingContext.h>
#include <KoStyleStack.h>
#include <KoOdfGraphicStyles.h>
#include <KoGenStyles.h>

// KDChart
#include <KDChartAbstractCoordinatePlane>
#include <KDChartBackgroundAttributes>
#include <KDChartFrameAttributes>

using namespace KChart;

class Surface::Private
{
public:
    Private();
    ~Private();

    PlotArea *plotArea;
    
    KDChart::AbstractCoordinatePlane *kdPlane;

    QPointF position;
    int width;
    QBrush brush;
    QPen framePen;
};

Surface::Private::Private()
{
}

Surface::Private::~Private()
{
}


Surface::Surface( PlotArea *parent )
    : d( new Private )
{
    Q_ASSERT( parent );
    
    d->plotArea = parent;
    d->kdPlane = d->plotArea->kdPlane();
    Q_ASSERT( d->kdPlane );
}

Surface::~Surface()
{
}


QPointF Surface::position() const
{
    return d->position;
}

void Surface::setPosition( const QPointF &position )
{
    d->position = position;
}

int Surface::width() const
{
    return d->width;
}

void Surface::setWidth( int width )
{
    d->width = width;
}

QBrush Surface::brush() const
{
    return d->brush;
}

void Surface::setBrush( const QBrush &brush )
{
    d->brush = brush;
}

QPen Surface::framePen() const
{
    return d->framePen;
}

void Surface::setFramePen( const QPen &pen )
{
    d->framePen = pen;
}

bool Surface::loadOdf( const KoXmlElement &surfaceElement, KoShapeLoadingContext &context )
{
    KoStyleStack &styleStack = context.odfLoadingContext().styleStack();
    styleStack.save();
    
    if ( surfaceElement.hasAttributeNS( KoXmlNS::chart, "style-name" ) )
    {
        KDChart::BackgroundAttributes backgroundAttributes = d->kdPlane->backgroundAttributes();
        KDChart::FrameAttributes frameAttributes = d->kdPlane->frameAttributes();
        
        styleStack.clear();
        context.odfLoadingContext().fillStyleStack( surfaceElement, KoXmlNS::chart, "style-name", "chart" );
        
        styleStack.setTypeProperties( "graphic" );
        
        if ( styleStack.hasProperty( KoXmlNS::draw, "stroke" ) )
        {
            frameAttributes.setVisible( true );
            QString stroke = styleStack.property( KoXmlNS::draw, "stroke" );
            if( stroke == "solid" || stroke == "dash" )
            {
                QPen pen = KoOdfGraphicStyles::loadOasisStrokeStyle( styleStack, stroke, context.odfLoadingContext().stylesReader() );
                frameAttributes.setPen( pen );
            }
        }
        
        if ( styleStack.hasProperty( KoXmlNS::draw, "fill" ) )
        {
            backgroundAttributes.setVisible( true );
            QBrush brush;
            QString fill = styleStack.property( KoXmlNS::draw, "fill" );
            if ( fill == "solid" || fill == "hatch" )
                brush = KoOdfGraphicStyles::loadOasisFillStyle( styleStack, fill, context.odfLoadingContext().stylesReader() );
            else if ( fill == "gradient" )
            {
                brush = KoOdfGraphicStyles::loadOasisGradientStyle( styleStack, context.odfLoadingContext().stylesReader(), QSizeF( 5.0, 60.0 ) );
            }
            else if ( fill == "bitmap" )
                brush = KoOdfGraphicStyles::loadOasisPatternStyle( styleStack, context.odfLoadingContext(), QSizeF( 5.0, 60.0 ) );
            backgroundAttributes.setBrush( brush );
        }
        
        d->kdPlane->setBackgroundAttributes( backgroundAttributes );
        d->kdPlane->setFrameAttributes( frameAttributes );
    }

    styleStack.restore();
    
    return true;
}

void Surface::saveOdf( KoShapeSavingContext &context )
{
    KoXmlWriter &bodyWriter = context.xmlWriter();
    KoGenStyles &mainStyles = context.mainStyles();
    KoGenStyle style = KoGenStyle( KoGenStyle::StyleGraphicAuto, "chart" );

    // Fixme: Also save floor
    bodyWriter.startElement( "chart:wall" );

    QBrush backgroundBrush = d->kdPlane->backgroundAttributes().brush();
    QPen framePen = d->kdPlane->frameAttributes().pen();
    KoOdfGraphicStyles::saveOasisFillStyle( style, mainStyles, backgroundBrush );
    KoOdfGraphicStyles::saveOasisStrokeStyle( style, mainStyles, framePen );

    bodyWriter.addAttribute( "chart:style-name", mainStyles.lookup( style, "ch" ) );

    bodyWriter.endElement(); // chart:wall
}

