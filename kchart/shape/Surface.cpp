/* This file is part of the KDE project

   Copyright 2007 Johannes Simon <johannes.simon@gmail.com>
   Copyright 2009 Inge Wallin    <ingwa@lysator.liu.se>

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

// Own
#include "Surface.h"

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
#include <KoOdfWorkaround.h>

// KDChart
#include <KDChartAbstractCoordinatePlane>
#include <KDChartBackgroundAttributes>
#include <KDChartFrameAttributes>

// KChart
#include "PlotArea.h"


using namespace KChart;

class Surface::Private
{
public:
    Private();
    ~Private();

    PlotArea *plotArea;
    
    QPointF  position;
    int      width;

    QBrush   brush;
    QPen     framePen;

    KDChart::AbstractCoordinatePlane *kdPlane;
};

Surface::Private::Private()
{
}

Surface::Private::~Private()
{
}


// ================================================================


Surface::Surface( PlotArea *parent )
    : d( new Private )
{
    Q_ASSERT( parent );
    
    // FIXME: init in private's constructor
    d->plotArea = parent;

    // FIXME: Make this class capable of storing floor-specific
    // attributes as well. Right now, it's really only used
    // and designed to load and save the chart's wall.
    d->kdPlane = d->plotArea->kdPlane();
    Q_ASSERT( d->kdPlane );
}

Surface::~Surface()
{
    delete d;
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

bool Surface::loadOdf( const KoXmlElement &surfaceElement,
                       KoShapeLoadingContext &context )
{
    // Get the current style stack and save it's state.
    KoStyleStack &styleStack = context.odfLoadingContext().styleStack();
    styleStack.save();

    bool brushLoaded = false;
    
    if ( surfaceElement.hasAttributeNS( KoXmlNS::chart, "style-name" ) ) {
        KDChart::BackgroundAttributes backgroundAttributes = d->kdPlane->backgroundAttributes();
        KDChart::FrameAttributes frameAttributes = d->kdPlane->frameAttributes();
        
        // Add the chart style to the style stack.
        styleStack.clear();
        context.odfLoadingContext().fillStyleStack( surfaceElement, 
                                                    KoXmlNS::chart, "style-name", "chart" );
        
        styleStack.setTypeProperties( "graphic" );
        
        // If there is a "stroke" property, then get the stroke style
        // and set the pen accordingly.
        if ( styleStack.hasProperty( KoXmlNS::draw, "stroke" ) ) {
            frameAttributes.setVisible( true );

            QString  stroke = styleStack.property( KoXmlNS::draw, "stroke" );
            if ( stroke == "solid" || stroke == "dash" ) {
                QPen pen = KoOdfGraphicStyles::loadOdfStrokeStyle( styleStack, stroke, context.odfLoadingContext().stylesReader() );

                frameAttributes.setPen( pen );
            }
        }
        
        // If there is a "fill" property, then get the fill style, and
        // set the brush for the surface accordingly.
        if ( styleStack.hasProperty( KoXmlNS::draw, "fill" ) ) {
            backgroundAttributes.setVisible( true );

            QBrush   brush;
            QString  fill = styleStack.property( KoXmlNS::draw, "fill" );
            if ( fill == "solid" || fill == "hatch" ) {
                brushLoaded = true;
                brush = KoOdfGraphicStyles::loadOdfFillStyle( styleStack, fill,
                                                              context.odfLoadingContext().stylesReader() );
            }
            else if ( fill == "gradient" ) {
                brushLoaded = true;
                brush = KoOdfGraphicStyles::loadOdfGradientStyle( styleStack, context.odfLoadingContext().stylesReader(), QSizeF( 5.0, 60.0 ) );
            }
            else if ( fill == "bitmap" ) {
                brushLoaded = true;
                brush = KoOdfGraphicStyles::loadOdfPatternStyle( styleStack, context.odfLoadingContext(), QSizeF( 5.0, 60.0 ) );
            }

            backgroundAttributes.setBrush( brush );
        }
        
        // Finally actually set the attributes.
        d->kdPlane->setBackgroundAttributes( backgroundAttributes );
        d->kdPlane->setFrameAttributes( frameAttributes );
    }

#ifndef NWORKAROUND_ODF_BUGS
    if ( !brushLoaded ) {
        KDChart::BackgroundAttributes backgroundAttributes = d->kdPlane->backgroundAttributes();
        QColor fillColor = KoOdfWorkaround::fixMissingFillColor( surfaceElement, context );
        if ( fillColor.isValid() ) {
            backgroundAttributes.setVisible( true );
            backgroundAttributes.setBrush( fillColor );
            d->kdPlane->setBackgroundAttributes( backgroundAttributes );
        }
    }
#endif

    // Restore the style stack to what it was before entering this
    // function.
    styleStack.restore();
    
    return true;
}

void Surface::saveOdf( KoShapeSavingContext &context, 
                       const char *elementName )
{
    KoXmlWriter  &bodyWriter = context.xmlWriter();
    KoGenStyles  &mainStyles = context.mainStyles();
    KoGenStyle    style      = KoGenStyle( KoGenStyle::StyleGraphicAuto, 
                                           "chart" );

    // elementName is chart:floor or chart:wall
    bodyWriter.startElement( elementName );

    QBrush  backgroundBrush = d->kdPlane->backgroundAttributes().brush();
    QPen    framePen        = d->kdPlane->frameAttributes().pen();

    KoOdfGraphicStyles::saveOdfFillStyle( style, mainStyles, backgroundBrush );
    KoOdfGraphicStyles::saveOdfStrokeStyle( style, mainStyles, framePen );

    bodyWriter.addAttribute( "chart:style-name", mainStyles.lookup( style, "ch" ) );

    bodyWriter.endElement(); // chart:floor or chart:wall
}

