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
#include "Legend.h"
#include "PlotArea.h"
#include "KDChartConvertions.h"

// Qt
#include <QString>
#include <QSizeF>
#include <QPen>
#include <QColor>
#include <QBrush>
#include <QFont>
#include <QImage>

// KDChart
#include <KDChartChart>
#include <KDChartBarDiagram>
#include <KDChartAbstractDiagram>
#include <KDChartFrameAttributes>
#include <KDChartBackgroundAttributes>
#include <KDChartLegend>

// KOffice
#include <KoXmlReader.h>
#include <KoXmlWriter.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeSavingContext.h>
#include <KoOdfLoadingContext.h>
#include <KoOdfStylesReader.h>
#include <KoXmlNS.h>
#include <KoGenStyles.h>
#include <KoUnit.h>

using namespace KChart;

class Legend::Private {
public:
    Private();
    ~Private();
    
    ChartShape *shape;
    QString title;
    bool showFrame;
    QPen framePen;
    QBrush backgroundBrush;
    LegendExpansion expansion;
    LegendPosition position;
    QFont font;
    QFont titleFont;
    QColor fontColor;
    Qt::Alignment alignment;
    
    KDChart::Legend *kdLegend;
    
    QImage image;
    
    bool pixmapRepaintRequested;
    QSizeF lastSize;
    QPointF lastZoomLevel;
};

Legend::Private::Private()
{
    showFrame = true;
    framePen = QPen();
    backgroundBrush = QBrush();
    expansion = HighLegendExpansion;
    alignment = Qt::AlignRight;
    pixmapRepaintRequested = true;
    position = EndLegendPosition;
}

Legend::Private::~Private()
{
}


Legend::Legend( ChartShape *parent )
    : d( new Private() )
{
    Q_ASSERT( parent );
    
    d->shape = parent;
    
    d->kdLegend = new KDChart::Legend();
    
    setTitleFontSize( 7 );
    setTitle( QString() );
    
    KDChart::TextAttributes attributes = d->kdLegend->textAttributes();
    attributes.setFontSize( KDChart::Measure( 6, KDChartEnums::MeasureCalculationModeAbsolute ) );
    attributes.setAutoShrink( false );
    d->kdLegend->setTextAttributes( attributes );
    
    parent->addChild( this );
}

Legend::~Legend()
{
}


QString Legend::title() const
{
    return d->title;
}

void Legend::setTitle( const QString &title )
{
    d->title = title;
    d->kdLegend->setTitleText( title );
    d->pixmapRepaintRequested = true;
}

bool Legend::showFrame() const
{
    return d->showFrame;
}

void Legend::setShowFrame( bool show )
{
    d->showFrame = show;
    
    // KDChart
    KDChart::FrameAttributes attributes = d->kdLegend->frameAttributes();
    attributes.setVisible( show );
    d->kdLegend->setFrameAttributes( attributes );
    d->pixmapRepaintRequested = true;
}

QPen Legend::framePen() const
{
    return d->framePen;
}

void Legend::setFramePen( const QPen &pen )
{
    d->framePen = pen;
    
    // KDChart
    KDChart::FrameAttributes attributes = d->kdLegend->frameAttributes();
    attributes.setPen( pen  );
    d->kdLegend->setFrameAttributes( attributes );
    d->pixmapRepaintRequested = true;
}

QColor Legend::frameColor() const
{
    return d->framePen.color();
}

void Legend::setFrameColor( const QColor &color )
{
    d->framePen.setColor( color );
    
    // KDChart
    KDChart::FrameAttributes attributes = d->kdLegend->frameAttributes();
    attributes.setVisible( true );
    QPen pen = attributes.pen();
    pen.setColor( color );
    attributes.setPen( pen );
    d->kdLegend->setFrameAttributes( attributes );
    d->pixmapRepaintRequested = true;
}

QBrush Legend::backgroundBrush() const
{
    return d->backgroundBrush;
}

void Legend::setBackgroundBrush( const QBrush &brush )
{
    d->backgroundBrush = brush;

    // KDChart
    KDChart::BackgroundAttributes attributes = d->kdLegend->backgroundAttributes();
    attributes.setVisible( true );
    attributes.setBrush( brush );
    d->kdLegend->setBackgroundAttributes( attributes );
    d->pixmapRepaintRequested = true;
}

QColor Legend::backgroundColor() const
{
    return d->backgroundBrush.color();
}

void Legend::setBackgroundColor( const QColor &color )
{
    d->backgroundBrush.setColor( color );

    // KDChart
    KDChart::BackgroundAttributes attributes = d->kdLegend->backgroundAttributes();
    attributes.setVisible( true );
    QBrush brush = attributes.brush();
    brush.setColor( color );
    attributes.setBrush( brush );
    d->kdLegend->setBackgroundAttributes( attributes );
    d->pixmapRepaintRequested = true;
}

QFont Legend::font() const
{
    return d->font;
}

void Legend::setFont( const QFont &font )
{
    d->font = font;

    // KDChart
    KDChart::TextAttributes attributes = d->kdLegend->textAttributes();
    attributes.setFont( font );
    d->kdLegend->setTextAttributes( attributes );
    d->pixmapRepaintRequested = true;
}

double Legend::fontSize() const
{
    return d->font.pointSizeF();
}

void Legend::setFontSize( double size )
{
    d->font.setPointSizeF( size );

    // KDChart
    KDChart::TextAttributes attributes = d->kdLegend->textAttributes();
    attributes.setFontSize( KDChart::Measure( size, KDChartEnums::MeasureCalculationModeAbsolute ) );
    d->kdLegend->setTextAttributes( attributes );
    d->pixmapRepaintRequested = true;
}

QFont Legend::titleFont() const
{
    return d->titleFont;
}

void Legend::setTitleFont( const QFont &font )
{
    d->titleFont = font;

    // KDChart
    KDChart::TextAttributes attributes = d->kdLegend->titleTextAttributes();
    attributes.setFont( font );
    d->kdLegend->setTitleTextAttributes( attributes );
    d->pixmapRepaintRequested = true;
}

double Legend::titleFontSize() const
{
    return d->titleFont.pointSizeF();
}

void Legend::setTitleFontSize( double size )
{
    d->titleFont.setPointSizeF( size );

    // KDChart
    KDChart::TextAttributes attributes = d->kdLegend->titleTextAttributes();
    attributes.setFontSize( KDChart::Measure( size, KDChartEnums::MeasureCalculationModeAbsolute ) );
    d->kdLegend->setTitleTextAttributes( attributes );
    d->pixmapRepaintRequested = true;
}

LegendExpansion Legend::expansion() const
{
    return d->expansion;
}

void Legend::setExpansion( LegendExpansion expansion )
{
    d->expansion = expansion;
    d->pixmapRepaintRequested = true;
}

Qt::Alignment Legend::alignment() const
{
    return d->alignment;
}

void Legend::setAlignment( Qt::Alignment alignment )
{
    d->alignment = alignment;
}

LegendPosition Legend::legendPosition() const
{
    return d->position;
}

void Legend::setLegendPosition( LegendPosition position )
{
    d->position = position;
    d->pixmapRepaintRequested = true;
}

void Legend::setSize( const QSizeF &size )
{
    d->kdLegend->resize( size.toSize() );
    d->kdLegend->resizeLayout( size.toSize() );
    KoShape::setSize( size );
}


void Legend::paintPixmap( QPainter &painter, const KoViewConverter &converter )
{
    // Adjust the size of the painting area to the current zoom level
    const QSize paintRectSize = converter.documentToView( d->lastSize ).toSize();
    const QRect paintRect = QRect( QPoint( 0, 0 ), paintRectSize );
    d->image = QImage( paintRectSize, QImage::Format_ARGB32 );
    
    QPainter pixmapPainter( &d->image );
    pixmapPainter.setRenderHints( painter.renderHints() );
    pixmapPainter.setRenderHint( QPainter::Antialiasing, false );

    // Paint the background
    pixmapPainter.fillRect( paintRect, Qt::white );

    // scale the painter's coordinate system to fit the current zoom level
    applyConversion( pixmapPainter, converter );
    d->kdLegend->paint( &pixmapPainter );
}

void Legend::paint( QPainter &painter, const KoViewConverter &converter )
{
    // Calculate the clipping rect
    QRectF paintRect = QRectF( QPointF( 0, 0 ), size() );
    //clipRect.intersect( paintRect );
    painter.setClipRect( converter.documentToView( paintRect ) );

    // Get the current zoom level
    QPointF zoomLevel;
    converter.zoom( &zoomLevel.rx(), &zoomLevel.ry() );

    // Only repaint the pixmap if it is scheduled, the zoom level changed or the shape was resized
    if (    d->pixmapRepaintRequested
         || d->lastZoomLevel != zoomLevel
         || d->lastSize      != size() ) {
        // TODO: What if two zoom levels are constantly being requested?
        // At the moment, this *is* the case, due to the fact
        // that the shape is also rendered in the page overview
        // in KPresenter
        // Everytime the window is hidden and shown again, a repaint is
        // requested --> laggy performance, especially when quickly
        // switching through windows
        d->pixmapRepaintRequested = false;
        d->lastZoomLevel = zoomLevel;
        d->lastSize      = size();
        
        paintPixmap( painter, converter );
    }

    // Paint the cached pixmap
    painter.drawImage( 0, 0, d->image );
}


// Only reimplemneted because pure virtual in KoShape, but not needed
bool Legend::loadOdf( const KoXmlElement &legendElement, KoShapeLoadingContext &context )
{
    return loadOdf( legendElement, context.odfLoadingContext().stylesReader() );
}

bool Legend::loadOdf( const KoXmlElement &legendElement, const KoOdfStylesReader &stylesReader )
{
    // TODO: Read optional attributes
    // 1. Legend expansion
    // 2. Advanced legend styling
    //KDChart::Legend  *oldKdchartLegend = d->kdchartLegend;
    //d->kdLegend = new KDChart::Legend( d->diagram, d->chart );
    
    if ( legendElement.hasAttributeNS( KoXmlNS::svg, "x" ) && legendElement.hasAttributeNS( KoXmlNS::svg, "y" ) )
    {
        const qreal x = KoUnit::parseValue( legendElement.attributeNS( KoXmlNS::svg, "x" ) );
        const qreal y = KoUnit::parseValue( legendElement.attributeNS( KoXmlNS::svg, "y" ) );
        setPosition( QPointF( x, y ) );
    }
    
    if ( legendElement.hasAttributeNS( KoXmlNS::svg, "width" ) && legendElement.hasAttributeNS( KoXmlNS::svg, "height" ) )
    {
        const qreal width = KoUnit::parseValue( legendElement.attributeNS( KoXmlNS::svg, "width" ) );
        const qreal height = KoUnit::parseValue( legendElement.attributeNS( KoXmlNS::svg, "height" ) );
        setSize( QSizeF( width, height ) );
    }

    if ( !legendElement.isNull() ) {
        QString lp;
        if ( legendElement.hasAttributeNS( KoXmlNS::chart, "legend-position" ) ) {
            lp = legendElement.attributeNS( KoXmlNS::chart, "legend-position", QString() );
        }
        QString lalign;
        if ( legendElement.hasAttributeNS( KoXmlNS::chart, "legend-align" ) ) {
            lalign = legendElement.attributeNS( KoXmlNS::chart, "legend-align", QString() );
        }
        
        if ( legendElement.hasAttributeNS( KoXmlNS::koffice, "legend-expansion" ) ) {
            QString lexpansion = legendElement.attributeNS( KoXmlNS::koffice, "legend-expansion", QString() );
            if ( lexpansion == "wide" )
                setExpansion( WideLegendExpansion );
            else if ( lexpansion == "high" )
                setExpansion( HighLegendExpansion );
            else
                setExpansion( BalancedLegendExpansion );
        }

        if ( lalign == "start" ) {
            setAlignment( Qt::AlignLeft );
        }
        else if ( lalign == "end" ) {
            setAlignment( Qt::AlignRight );
        }
        else {
            setAlignment( Qt::AlignCenter );
        }

        if ( lp == "start" ) {
            setLegendPosition( StartLegendPosition );
        }
        else if ( lp == "top" ) {
            setLegendPosition( TopLegendPosition );
        }
        else if ( lp == "bottom" ) {
            setLegendPosition( BottomLegendPosition );
        }
        else if ( lp == "top-start" ) {
            setLegendPosition( TopStartLegendPosition );
        }
        else if ( lp == "bottom-start" ) {
            setLegendPosition( BottomStartLegendPosition );
        }
        else if ( lp == "top-end" ) {
            setLegendPosition( TopEndLegendPosition );
        }
        else if ( lp == "bottom-end" ) {
            setLegendPosition( BottomEndLegendPosition );
        }
        else {
            setLegendPosition( EndLegendPosition );
        }
        
        if ( legendElement.hasAttributeNS( KoXmlNS::koffice, "title" ) ) {
            setTitle( legendElement.attributeNS( KoXmlNS::koffice, 
                                                       "title", QString() ) );
        }
        
        if ( legendElement.hasAttributeNS( KoXmlNS::chart, "style-name" ) ) {
            QString styleName = legendElement.attributeNS( KoXmlNS::chart, "style-name", QString() );
            const KoXmlElement *styleElement = stylesReader.findStyle( styleName, "chart" );
            if ( styleElement ) {
                KoXmlElement graphicsPropertiesElement = styleElement->namedItemNS( KoXmlNS::style, "graphic-properties" ).toElement();
                if ( !graphicsPropertiesElement.isNull() ) {
                    if ( graphicsPropertiesElement.hasAttributeNS( KoXmlNS::draw, "stroke" ) ) {
                        // TODO (Johannes): set stroke type of legend border
                        QString stroke = graphicsPropertiesElement.attributeNS( KoXmlNS::draw, "stroke", QString() );
                        QString strokeColor = graphicsPropertiesElement.attributeNS( KoXmlNS::draw, "stroke-color", QString() );
                        // use overloaded QColor constructor to convert QString (in form of "#rrggbb") to QColor
                        setFrameColor( strokeColor );
                    }
                    if ( graphicsPropertiesElement.hasAttributeNS( KoXmlNS::draw, "fill" ) ) {
                        QString fill = graphicsPropertiesElement.attributeNS( KoXmlNS::draw, "fill", QString() );
                        if ( fill == "solid" ) {
                            QString fillColor = graphicsPropertiesElement.attributeNS( KoXmlNS::draw, "fill-color", QString() );
                            // use overloaded QColor constructor to convert QString (in form of "#rrggbb") to QColor
                            setBackgroundColor( fillColor );
                        }
                    }
                }

                KoXmlElement textPropertiesElement = styleElement->namedItemNS( KoXmlNS::style, "text-properties" ).toElement();
                if ( !textPropertiesElement.isNull() )
                {
                    if ( textPropertiesElement.hasAttributeNS( KoXmlNS::fo, "font-size" ) )
                    {
                        const qreal fontSize = KoUnit::parseValue( textPropertiesElement.attributeNS( KoXmlNS::fo, "font-size" ) );
                        setFontSize( fontSize );
                    }
                }
            }
        }
    }
    else {
        // No legend element, use default legend.
        // FIXME: North??  Isn't that a bit strange as default? /IW
        setLegendPosition( TopLegendPosition );
        setAlignment( Qt::AlignCenter );
    }
    
    //d->chart->replaceLegend( d->legend, oldLegend );
    
    d->pixmapRepaintRequested = true;

    return true;
}

void Legend::saveOdf( KoXmlWriter &bodyWriter, KoGenStyles &mainStyles ) const
{
    bodyWriter.startElement( "chart:legend" );

    QString lp = LegendPositionToString( d->position );
    
    QString lalign;

    if ( !lp.isEmpty() ) {
        bodyWriter.addAttribute( "chart:legend-position", lp );
    }
    if ( !lalign.isEmpty() ) {
        bodyWriter.addAttribute( "chart:legend-align", lalign );
    }
    
    QString styleName = saveOdfFont( mainStyles, d->font, d->fontColor );
    bodyWriter.addAttribute( "chart:style-name", styleName );
    
    KoGenStyle::PropertyType gt = KoGenStyle::GraphicType;
    KoGenStyle *style = ( KoGenStyle* )( mainStyles.style( styleName ) );
    if ( style ) {
        style->addProperty( "draw:stroke", "solid", gt );
        style->addProperty( "draw:stroke-color", frameColor().name(), gt );
        style->addProperty( "draw:fill", "solid", gt );
        style->addProperty( "draw:fill-color", backgroundColor().name(), gt );
    }
    
    QString  lexpansion;
    switch ( expansion() )
    {
    case WideLegendExpansion:
        lexpansion = "wide";
        break;
    case HighLegendExpansion:
        lexpansion = "high";
        break;
    case BalancedLegendExpansion:
        lexpansion = "balanced";
        break;
    };
    
    bodyWriter.addAttribute( "koffice:legend-expansion", lexpansion );
    if ( !title().isEmpty() )
        bodyWriter.addAttribute( "koffice:title", title() );
    bodyWriter.endElement(); // chart:legend
}

void Legend::saveOdf( KoShapeSavingContext &context ) const
{
    Q_UNUSED( context );
}

KDChart::Legend *Legend::kdLegend() const
{
    // There has to be a valid KDChart instance of this legend
    Q_ASSERT( d->kdLegend );
    return d->kdLegend;
}

void Legend::update()
{
    d->pixmapRepaintRequested = true;
    QSize size = d->kdLegend->sizeHint();
    // Scale size from px to pt
    setSize( QSizeF( size ) );
    KoShape::update();
}
