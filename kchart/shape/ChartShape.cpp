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
   Boston, MA 02110-1301, USA.
*/

// Local
#include "ChartShape.h"
#include "KDChartConvertions.h"
#include "Axis.h"
#include "DataSet.h"
#include "Legend.h"
#include "PlotArea.h"
#include "Surface.h"
#include "TextLabel.h"
#include "ProxyModel.h"

// Posix
#include <float.h> // For basic data types characteristics.

// KOffice
#include <KoShapeLoadingContext.h>
#include <KoShapeSavingContext.h>
#include <KoViewConverter.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>
#include <KoXmlNS.h>
#include <KoGenStyles.h>

// KDChart
#include <KDChartChart>
#include <KDChartAbstractDiagram>
#include <KDChartCartesianAxis>
#include <KDChartCartesianCoordinatePlane>
#include <KDChartPolarCoordinatePlane>
// Attribute Classes
#include <KDChartDataValueAttributes>
#include <KDChartGridAttributes>
#include <KDChartTextAttributes>
#include <KDChartMarkerAttributes>
#include <KDChartThreeDPieAttributes>
#include <KDChartThreeDBarAttributes>
#include <KDChartThreeDLineAttributes>
// Diagram Classes
#include <KDChartBarDiagram>
#include <KDChartPieDiagram>
#include <KDChartLineDiagram>
#include <KDChartRingDiagram>
#include <KDChartPolarDiagram>

// Qt
#include <QImage>
#include <QPainter>
#include <QPointF>
#include <QSizeF>

// KDE
#include <KDebug>
#include <KApplication>

namespace KChart {

static const struct {
    ChartType   chartType;
    const char    *odfName;
} odfChartTypes[] = {
    { BarChartType ,    "chart:bar"     },
    { LineChartType,    "chart:line"    },
    { AreaChartType ,   "chart:area"    },
    { CircleChartType,  "chart:circle"  },
    { RingChartType,    "chart:ring"    },
    { ScatterChartType, "chart:scatter" },
    { RadarChartType,   "chart:radar"   },
    { StockChartType,   "chart:stock"   },
    { BubbleChartType,  "chart:bubble"  },
    { SurfaceChartType, "chart:surface" },
    { GanttChartType,   "chart:gantt"   },
};

const ChartSubtype  defaultSubtypes[NUM_CHARTTYPES] = {
    NormalChartSubtype,     // Bar
    NormalChartSubtype,     // Line
    NormalChartSubtype,     // Area
    NoChartSubtype,         // Circle
    NoChartSubtype,         // Ring
    NoChartSubtype,         // Scatter
    NoChartSubtype,         // radar
    NoChartSubtype,         // Stock
    NoChartSubtype,         // Bubble
    NoChartSubtype,         // Surface
    NoChartSubtype          // Gantt
};

static const unsigned int  numChartTypes = ( sizeof odfChartTypes
                                                / sizeof *odfChartTypes );

bool isPolar( ChartType type )
{
    return ( type == CircleChartType
         || type == RingChartType
         || type == RadarChartType );
}

bool isCartesian( ChartType type )
{
    return !isPolar( type );
}

QString saveOdfFont( KoGenStyles& mainStyles, 
                                 const QFont& font,
                                 const QColor& color )
{
    KoGenStyle::PropertyType tt = KoGenStyle::TextType;
    KoGenStyle autoStyle( KoGenStyle::StyleAuto, "chart", 0 );
    autoStyle.addProperty( "fo:font-family", font.family(), tt );
    autoStyle.addPropertyPt( "fo:font-size", font.pointSize(), tt );
    autoStyle.addProperty( "fo:color", color.isValid() ? color.name() : "#000000", tt );
    int w = font.weight();
    autoStyle.addProperty( "fo:font-weight", w == 50 ? "normal" : w == 75 ? "bold" : QString::number( qRound(  w / 10 ) * 100 ), tt );
    autoStyle.addProperty( "fo:font-style", font.italic() ? "italic" : "normal", tt );

    return mainStyles.lookup( autoStyle, "ch", KoGenStyles::ForceNumbering );
}


class ChartShape::Private
{
public:
    Private();
    ~Private();
    
    TextLabel *title;
    TextLabel *subTitle;
    TextLabel *footer;
    Legend *legend;
    PlotArea *plotArea;
    Surface *wall;
    Surface *floor;

    // We can rerender faster if we cache KDChart's output
    QImage   pixmap;
    QPointF  lastZoomLevel;
    QSizeF   lastSize;
    bool     pixmapRepaintRequested;
    
    ProxyModel *model;
};

ChartShape::Private::Private()
{
}

ChartShape::Private::~Private()
{
}

ChartShape::ChartShape()
    : d ( new Private )
{
    setShapeId( ChartShapeId );
    
    d->legend = new Legend( this );
    
    d->plotArea = new PlotArea( this );
    d->model = new ProxyModel ( d->plotArea );
    
    d->plotArea->setChartType( BarChartType );
    d->plotArea->setChartSubType( NormalChartSubtype );
    
    d->title = new TextLabel( this );
    d->title->setType( TitleLabelType );
    d->title->setText( i18n( "Title" ) );
    
    d->subTitle = new TextLabel( this );
    d->subTitle->setType( SubTitleLabelType );
    d->subTitle->setText( i18n( "Subtitle" ) );
    
    d->footer = new TextLabel( this );
    d->footer->setType( FooterLabelType );
    d->footer->setText( i18n( "Footer" ) );
    
    d->floor = new Surface( d->plotArea );
    d->wall = new Surface( d->plotArea );
    
    d->pixmapRepaintRequested = true;
}

ChartShape::~ChartShape()
{
}


QAbstractItemModel *ChartShape::model() const
{
    return d->model->sourceModel();
}

ProxyModel *ChartShape::proxyModel() const
{
    return d->model;
}

TextLabel *ChartShape::title() const
{
    return d->title;
};

TextLabel *ChartShape::subTitle() const
{
    return d->subTitle;
};

TextLabel *ChartShape::footer() const
{
    return d->footer;
};

Legend *ChartShape::legend() const
{
    return d->legend;
};

PlotArea *ChartShape::plotArea() const
{
    return d->plotArea;
};

Surface *ChartShape::wall() const
{
    return d->wall;
};

Surface *ChartShape::floor() const
{
    return d->floor;
};


void ChartShape::setModel( QAbstractItemModel *model, bool takeOwnershipOfModel )
{
    Q_ASSERT( model );
    d->model->setSourceModel( model );
    
    d->plotArea->init();
}

bool ChartShape::addAxis( Axis *axis )
{
    Q_ASSERT( d->plotArea );
    return d->plotArea->addAxis( axis );
}

bool ChartShape::removeAxis( Axis *axis )
{
    Q_ASSERT( d->plotArea );
    return d->plotArea->removeAxis( axis );
}

void ChartShape::setPosition( const QPointF &pos )
{
    //QPointF relativePosition = pos - position();
    //if ( relativePosition.x() > 0.01 || relativePosition.y() > 0.01 )
    //    d->legend->setPosition( d->legend->position() - relativePosition );
    KoShape::setPosition( pos );
}

void ChartShape::setSize( const QSizeF &size )
{
    // Usually, this is done by signals from the QWidget that we resize.
    // But since a KoShape is not a QWidget, we need to do this manually.
    d->plotArea->kdChart()->resize( size.toSize() );
    KoShape::setSize( size );
}

ChartType ChartShape::chartType() const
{
    return d->plotArea->chartType();
}

ChartSubtype ChartShape::chartSubType() const
{
    return d->plotArea->chartSubType();
}

bool ChartShape::isThreeD() const
{
    return d->plotArea->isThreeD();
}

void ChartShape::setFirstRowIsLabel( bool isLabel )
{
    d->model->setFirstRowIsLabel( isLabel );
}

void ChartShape::setFirstColumnIsLabel( bool isLabel )
{
    d->model->setFirstColumnIsLabel( isLabel );
}

void ChartShape::setChartType( ChartType type )
{
    d->plotArea->setChartType( type );
}

void ChartShape::setChartSubType( ChartSubtype subType )
{
    d->plotArea->setChartSubType( subType );
}

void ChartShape::setThreeD( bool threeD )
{
    d->plotArea->setThreeD( threeD );
}

void ChartShape::paintPixmap( QPainter &painter, const KoViewConverter &converter )
{
    // Adjust the size of the painting area to the current zoom level
    const QSize paintRectSize = converter.documentToView( size() ).toSize();
    d->pixmap = QPixmap( paintRectSize );
    const QRect paintRect = QRect( QPoint( 0, 0 ), paintRectSize );

    // Copy the painter's render hints, such as antialiasing
    QPainter pixmapPainter( &d->pixmap );
    pixmapPainter.setRenderHints( painter.renderHints() );
    pixmapPainter.setRenderHint( QPainter::Antialiasing, false );

    // Paint the background
    pixmapPainter.fillRect( paintRect, KApplication::palette().base() );

    // scale the painter's coordinate system to fit the current zoom level
    applyConversion( pixmapPainter, converter );

    d->plotArea->paint( pixmapPainter );
}

void ChartShape::paint( QPainter& painter, const KoViewConverter& converter )
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
        paintPixmap( painter, converter );
        d->pixmapRepaintRequested = false;
        d->lastZoomLevel = zoomLevel;
        d->lastSize      = size();
    }

    // Paint the cached pixmap
    painter.drawImage( 0, 0, d->pixmap );
}

void ChartShape::paintComponent( QPainter &painter, const KoViewConverter &converter )
{
}

bool ChartShape::loadOdf( const KoXmlElement &chartElement, KoShapeLoadingContext &context )
{
    if ( chartElement.hasAttributeNS( KoXmlNS::chart, "class" ) ) {
        kDebug() << " ---------------------------------------------------------------- " ;
        kDebug() << " Chart class: " 
                 <<  chartElement.attributeNS( KoXmlNS::chart, "class" );
    }
    else
        return false;


    // 1. Load the chart type.
    const QString chartClass = chartElement.attributeNS( KoXmlNS::chart,
                                                         "class", QString() );

    // Find out what charttype the chart class corresponds to.
    bool  knownType = false;
    for ( unsigned int i = 0 ; i < numChartTypes ; ++i ) {
        if ( chartClass == odfChartTypes[i].odfName ) {
            kDebug(35001) <<"found chart of type" << chartClass;

            d->plotArea->setChartType( odfChartTypes[i].chartType );
            knownType = true;
            break;  
        }
    }

    // If we can't find out what charttype it is, we might as well end here.
    if ( !knownType ) {
        // FIXME: Find out what the equivalent of
        //        KoDocument::setErrorMessage() is for KoShape.
        //setErrorMessage( i18n( "Unknown chart type %1" ,chartClass ) );
        return false;
    }

    // 2. Load the title.
    KoXmlElement titleElem = KoXml::namedItemNS( chartElement, 
                                                 KoXmlNS::chart, "title" );
    if ( !titleElem.isNull() ) {
        if ( !d->title->loadOdf( titleElem, context) )
            return false;
    }

    // 3. Load the subtitle.
    KoXmlElement subTitleElem = KoXml::namedItemNS( chartElement, 
                                                    KoXmlNS::chart, "subtitle" );
    if ( !subTitleElem.isNull() ) {
        if ( !d->subTitle->loadOdf( subTitleElem, context) )
            return false;
    }

    // 4. Load the footer.
    KoXmlElement footerElem = KoXml::namedItemNS( chartElement, 
                                                  KoXmlNS::chart, "footer" );
    if ( !footerElem.isNull() ) {
        if ( !d->footer->loadOdf( footerElem, context) )
            return false;
    }

    // 5. Load the legend.
    KoXmlElement legendElem = KoXml::namedItemNS( chartElement, KoXmlNS::chart,
                          "legend" );
    if ( !legendElem.isNull() ) {
    if ( !d->legend->loadOdf( legendElem, context ) )
        return false;
    }

    // 6. Load the plot area (this is where the real action is!).
    KoXmlElement  plotareaElem = KoXml::namedItemNS( chartElement,
                             KoXmlNS::chart, "plot-area" );
    if ( !plotareaElem.isNull() ) {
    if ( !d->plotArea->loadOdf( plotareaElem, context ) )
        return false;
    }

    // 7. Load the data
    KoXmlElement  dataElem = KoXml::namedItemNS( chartElement,
                         KoXmlNS::table, "table" );
    if ( !dataElem.isNull() ) {
    if ( !loadOdfData( dataElem, context ) )
        return false;
    }


    return true;
}

bool ChartShape::loadOdfData( const KoXmlElement &chartElement, KoShapeLoadingContext &context )
{
    return true;
}

void ChartShape::saveOdf( KoShapeSavingContext & context ) const
{
    KoXmlWriter&  bodyWriter = context.xmlWriter();
    KoGenStyles&  mainStyles( context.mainStyles() );

    kError(32001) << "----------------------------------------------------------------";

    bodyWriter.startElement( "chart:chart" );

    // 1. Write the chart type.
    bool knownType = false;
    for ( unsigned int i = 0 ; i < numChartTypes ; ++i ) {
        if ( d->plotArea->chartType() == odfChartTypes[i].chartType ) {
            bodyWriter.addAttribute( "chart:class", odfChartTypes[i].odfName );
            knownType = true;
            break;
        }
    }
    if ( !knownType ) {
        kError(32001) << "Unknown chart type in ChartShape::saveOdf:"
                      << (int) d->plotArea->chartType() << endl;
    }

    // 2. Write the title.
    d->title->saveOdf( bodyWriter, mainStyles );

    // 3. Write the subtitle.
    d->subTitle->saveOdf( bodyWriter, mainStyles );

    // 4. Write the footer.
    d->footer->saveOdf( bodyWriter, mainStyles );

    // 5. Write the legend.
    d->legend->saveOdf( bodyWriter, mainStyles );

    // 6. Write the plot area (this is where the real action is!).
    bodyWriter.startElement( "chart:plot-area" );
    d->plotArea->saveOdf( bodyWriter, mainStyles );
    bodyWriter.endElement();

    // 7. Save the data
    saveOdfData( bodyWriter, mainStyles );

    bodyWriter.endElement(); // chart:chart
}

void ChartShape::saveOdfData( KoXmlWriter &bodyWriter, KoGenStyles &mainStyles ) const
{
    Q_UNUSED( mainStyles );

    const int cols = d->model->columnCount();
    const int rows = d->model->rowCount();

    bodyWriter.startElement( "table:table" );
    bodyWriter.addAttribute( "table:name", "local-table" );

    // Exactly one header column, always.
    bodyWriter.startElement( "table:table-header-columns" );
    bodyWriter.startElement( "table:table-column" );
    bodyWriter.endElement(); // table:table-column
    bodyWriter.endElement(); // table:table-header-columns

    // Then "cols" columns
    bodyWriter.startElement( "table:table-columns" );
    bodyWriter.startElement( "table:table-column" );
    bodyWriter.addAttribute( "table:number-columns-repeated", cols );
    bodyWriter.endElement(); // table:table-column
    bodyWriter.endElement(); // table:table-columns

    // Exactly one header row, always.
    bodyWriter.startElement( "table:table-header-rows" );
    bodyWriter.startElement( "table:table-row" );

    // The first column in header row is just the header column - no title needed
    bodyWriter.startElement( "table:table-cell" );
    bodyWriter.addAttribute( "office:value-type", "string" );
    bodyWriter.startElement( "text:p" );
    bodyWriter.endElement(); // text:p
    bodyWriter.endElement(); // table:table-cell

    bodyWriter.endElement(); // table:table-row
    bodyWriter.endElement(); // table:table-header-rows

    bodyWriter.startElement( "table:table-rows" );
    //QStringList::const_iterator rowLabelIt = m_rowLabels.begin();
    for ( int row = 0; row < rows ; ++row ) {
        bodyWriter.startElement( "table:table-row" );
        for ( int col = 0; col < cols; ++col ) {
            //QVariant value( d->model.cellVal( row, col ) );
            QModelIndex  index = d->model->index( row, col );
            QVariant     value = d->model->data( index );

            QString  valType;
            QString  valStr;

            switch ( value.type() ) {
            case QVariant::Invalid:
        break;
            case QVariant::String:
        valType = "string";
        valStr  = value.toString();
        break;
            case QVariant::Double:
        valType = "float";
        valStr  = QString::number( value.toDouble(), 'g', DBL_DIG );
        break;
            case QVariant::DateTime:

#define ChartShapeId "ChartShapeId"
        valType = "date";
        valStr  = ""; /* like in saveXML, but why? */
        break;
            default: {
                kDebug(35001) <<"ERROR: cell" << row <<"," << col
                               << " has unknown type." << endl;
                }
            }

        // Add the value type and the string to the XML tree.
            bodyWriter.startElement( "table:table-cell" );
            if ( !valType.isEmpty() ) {
                bodyWriter.addAttribute( "office:value-type", valType );
                if ( value.type() == QVariant::Double )
                    bodyWriter.addAttribute( "office:value", valStr );

                bodyWriter.startElement( "text:p" );
                bodyWriter.addTextNode( valStr );
                bodyWriter.endElement(); // text:p
            }
        bodyWriter.endElement(); // table:table-cell
        }
        bodyWriter.endElement(); // table:table-row
    }

    bodyWriter.endElement(); // table:table-rows
    bodyWriter.endElement(); // table:table
}

KoShape *ChartShape::cloneShape() const
{
    return 0;
}

void ChartShape::update() const
{
    KoShape::update();
}

void ChartShape::relayout() const
{
    d->pixmapRepaintRequested = true;
    KoShape::update();
}

} // Namespace KChart
