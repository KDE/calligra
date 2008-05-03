/* This file is part of the KDE project

   Copyright 2007 Stefan Nikolaus     <stefan.nikolaus@kdemail.net>
   Copyright 2007 Inge Wallin         <inge@lysator.liu.se>
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
#include "ProxyModel.h"
#include "TextLabelDummy.h"
#include "KoTextShapeData.h"

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
#include <KoShapeRegistry.h>
#include <KoToolRegistry.h>
#include <KoTextShapeData.h>

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
#include <QTextDocument>

// KDE
#include <KDebug>
#include <KApplication>
#include <KMessageBox>

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

bool loadOdfLabel( KoShape *label, KoXmlElement &labelElement, KoShapeLoadingContext &context )
{   
    TextLabelData *labelData = qobject_cast<TextLabelData*>( label->userData() );
    if ( !labelData )
        return false;
    
    // TODO: Read optional attributes
    // 1. Table range
    // 2. Position and size
    // 3. Style name
    KoXmlElement  pElement = KoXml::namedItemNS( labelElement,
                                            KoXmlNS::text, "p" );
    
    labelData->document()->setPlainText( pElement.text() );
    
    return true;
}

void saveOdfLabel( KoShape *label, KoXmlWriter &bodyWriter, KoGenStyles &mainStyles, const QString &odfLabelType )
{
    TextLabelData *labelData = qobject_cast<TextLabelData*>( label->userData() );
    if ( !labelData )
        return;
    
    bodyWriter.startElement( QString( "chart:" + odfLabelType ).toAscii() );
    bodyWriter.addAttributePt( "svg:x", label->position().x() );
    bodyWriter.addAttributePt( "svg:y", label->position().y() );
    // TODO: Save text label color
    bodyWriter.addAttribute( "chart:style-name", saveOdfFont( mainStyles, labelData->document()->defaultFont(), QColor() ) );
    bodyWriter.startElement( "text:p" );
    bodyWriter.addTextNode( labelData->document()->toPlainText() );
    bodyWriter.endElement(); // text:p
    bodyWriter.endElement(); // chart:title/subtitle/footer
}


class ChartShape::Private
{
public:
    Private();
    ~Private();
    
    KoShape *title;
    KoShape *subTitle;
    KoShape *footer;
    Legend *legend;
    PlotArea *plotArea;
    Surface *wall;
    Surface *floor;

    // We can rerender faster if we cache KDChart's output
    QImage   image;
    QPointF  lastZoomLevel;
    QSizeF   lastSize;
    mutable bool pixmapRepaintRequested;
    
    ProxyModel *model;
};

ChartShape::Private::Private()
{
    title = 0;
    subTitle = 0;
    footer = 0;
    legend = 0;
    plotArea = 0;
    wall = 0;
    floor = 0;
    model = 0;
    pixmapRepaintRequested = true;
}

ChartShape::Private::~Private()
{
}

ChartShape::ChartShape()
    : d ( new Private )
{
    setShapeId( ChartShapeId );
    
    d->model = new ProxyModel();
    
    // We need this as the very first step, because some methods
    // here rely on the d->plotArea pointer
    d->plotArea = new PlotArea( this );
    
    d->legend = new Legend( this );
    d->legend->setVisible( false );
    
    QObject::connect( d->model, SIGNAL( modelReset() ), d->plotArea, SLOT( dataSetCountChanged() ) );
    QObject::connect( d->model, SIGNAL( rowsInserted( const QModelIndex, int, int ) ), d->plotArea, SLOT( dataSetCountChanged() ) );
    QObject::connect( d->model, SIGNAL( rowsRemoved( const QModelIndex, int, int ) ), d->plotArea, SLOT( dataSetCountChanged() ) );
    QObject::connect( d->model, SIGNAL( dataChanged() ), d->plotArea, SLOT( update() ) );
    
    d->plotArea->setChartType( BarChartType );
    d->plotArea->setChartSubType( NormalChartSubtype );
    
    d->title = KoShapeRegistry::instance()->value( TextShapeId )->createDefaultShape( 0 );
    if ( !d->title )
    {
        d->title = new TextLabelDummy;
        KMessageBox::error( 0, "The plugin needed for displaying text labels in a chart is not available.", "Plugin Missing" );
    }
    if ( dynamic_cast<TextLabelData*>( d->title->userData() ) == 0 )
    {
        KMessageBox::error( 0, "The plugin needed for displaying text labels is not compatible with the current version of the chart Flake shape.", "Plugin Incompatible" );
        TextLabelData *dataDummy = new TextLabelData;
        d->title->setUserData( dataDummy );
    }
    
    addChild( d->title );
    titleData()->document()->setPlainText( i18n( "Title" ) );
    d->title->setVisible( false );

    d->subTitle = KoShapeRegistry::instance()->value( TextShapeId )->createDefaultShape( 0 );
    if ( !d->subTitle )
    {
        d->subTitle = new TextLabelDummy;
    }
    if ( dynamic_cast<TextLabelData*>( d->subTitle->userData() ) == 0 )
    {
        TextLabelData *dataDummy = new TextLabelData;
        d->subTitle->setUserData( dataDummy );
    }
    addChild( d->subTitle );
    subTitleData()->document()->setPlainText( i18n( "Subtitle" ) );
    d->subTitle->setVisible( false );

    d->footer = KoShapeRegistry::instance()->value( TextShapeId )->createDefaultShape( 0 );
    if ( !d->footer )
    {
        d->footer = new TextLabelDummy;
    }
    if ( dynamic_cast<TextLabelData*>( d->subTitle->userData() ) == 0 )
    {
        TextLabelData *dataDummy = new TextLabelData;
        d->footer->setUserData( dataDummy );
    }
    addChild( d->footer );
    footerData()->document()->setPlainText( i18n( "Footer" ) );
    d->footer->setVisible( false );
    
    d->floor = new Surface( d->plotArea );
    d->wall = new Surface( d->plotArea );
    
    requestRepaint();
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

KoShape *ChartShape::title() const
{
    return d->title;
}

TextLabelData *ChartShape::titleData() const
{
    TextLabelData *data = qobject_cast<TextLabelData*>( d->title->userData() );
    return data;
}
    

KoShape *ChartShape::subTitle() const
{
    return d->subTitle;
}

TextLabelData *ChartShape::subTitleData() const
{
    TextLabelData *data = qobject_cast<TextLabelData*>( d->subTitle->userData() );
    return data;
}

KoShape *ChartShape::footer() const
{
    return d->footer;
}

TextLabelData *ChartShape::footerData() const
{
    TextLabelData *data = qobject_cast<TextLabelData*>( d->footer->userData() );
    return data;
}

Legend *ChartShape::legend() const
{
    return d->legend;
}

PlotArea *ChartShape::plotArea() const
{
    return d->plotArea;
}

Surface *ChartShape::wall() const
{
    return d->wall;
}

Surface *ChartShape::floor() const
{
    return d->floor;
}


void ChartShape::setModel( QAbstractItemModel *model, bool takeOwnershipOfModel )
{
    Q_ASSERT( model );
    d->model->setSourceModel( model );
    
    requestRepaint();
}

void ChartShape::setModel( KoChart::ChartModel *model, const QVector<QRect> &selection )
{
    Q_ASSERT( model );
    d->model->setSourceModel( model, selection );
    
    requestRepaint();
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
    Q_ASSERT( d->plotArea );
    
    // Usually, this is done by signals from the QWidget that we resize.
    // But since a KoShape is not a QWidget, we need to do this manually.
    d->plotArea->kdChart()->resize( size.toSize() );
    
    KoShape::setSize( size );
    
    updateChildrenPositions();
}

void ChartShape::updateChildrenPositions()
{
    Q_ASSERT( d->plotArea );
    
    foreach( Axis *axis, d->plotArea->axes() )
    {
        KoShape *title = axis->title();
        QPointF titlePosition;
        if ( axis->position() == BottomAxisPosition )
            titlePosition = QPointF( size().width() / 2.0 - title->size().width() / 2.0, size().height() );
        else if ( axis->position() == TopAxisPosition )
            titlePosition = QPointF( size().width() / 2.0, -title->size().height() );
        else if ( axis->position() == LeftAxisPosition )
            titlePosition = QPointF( -title->size().width() / 2.0 - title->size().height() / 2.0, size().height() / 2.0 );
        else if ( axis->position() == RightAxisPosition )
            titlePosition = QPointF( size().width(), size().height() / 2.0 );
        title->setPosition( titlePosition );
    }
}

QRectF ChartShape::boundingRect() const
{
    QRectF rect = KoShape::boundingRect();
    
    foreach( KoShape *shape, iterator() )
    {
        if ( !shape->isVisible() )
            continue;
        rect = rect.united( shape->boundingRect() );
    }
    
    return rect;
}

ChartType ChartShape::chartType() const
{
    Q_ASSERT( d->plotArea );
    return d->plotArea->chartType();
}

ChartSubtype ChartShape::chartSubType() const
{
    Q_ASSERT( d->plotArea );
    return d->plotArea->chartSubType();
}

bool ChartShape::isThreeD() const
{
    Q_ASSERT( d->plotArea );
    return d->plotArea->isThreeD();
}

void ChartShape::setFirstRowIsLabel( bool isLabel )
{
    d->model->setFirstRowIsLabel( isLabel );
    
    requestRepaint();
}

void ChartShape::setFirstColumnIsLabel( bool isLabel )
{
    d->model->setFirstColumnIsLabel( isLabel );
    
    requestRepaint();
}

void ChartShape::setChartType( ChartType type )
{
    Q_ASSERT( d->plotArea );
    d->plotArea->setChartType( type );
}

void ChartShape::setChartSubType( ChartSubtype subType )
{
    Q_ASSERT( d->plotArea );
    d->plotArea->setChartSubType( subType );
}

void ChartShape::setThreeD( bool threeD )
{
    Q_ASSERT( d->plotArea );
    d->plotArea->setThreeD( threeD );
}

void ChartShape::paintPixmap( QPainter &painter, const KoViewConverter &converter )
{
    Q_ASSERT( d->plotArea );
    if ( !d->plotArea )
        return;
    
    // Adjust the size of the painting area to the current zoom level
    const QSize paintRectSize = converter.documentToView( size() ).toSize();
    d->image = QImage( paintRectSize, QImage::Format_ARGB32 );
    const QRect paintRect = QRect( QPoint( 0, 0 ), paintRectSize );

    // Copy the painter's render hints, such as antialiasing
    QPainter pixmapPainter( &d->image );
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
        // TODO: What if two zoom levels are constantly being requested?
        // At the moment, this *is* the case, due to the fact
        // that the shape is also rendered in the page overview
        // in KPresenter
        // Everytime the window is hidden and shown again, a repaint is
        // requested --> laggy performance, especially when quickly
        // switching through windows
        paintPixmap( painter, converter );
        d->pixmapRepaintRequested = false;
        d->lastZoomLevel = zoomLevel;
        d->lastSize      = size();
    }

    // Paint the cached pixmap
    painter.drawImage( 0, 0, d->image );
}

void ChartShape::paintComponent( QPainter &painter, const KoViewConverter &converter )
{
}

bool ChartShape::loadOdf( const KoXmlElement &chartElement, KoShapeLoadingContext &context )
{
    Q_ASSERT( d->plotArea );
    
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
    
    requestRepaint();

    return true;
}

bool ChartShape::loadOdfData( const KoXmlElement &chartElement, KoShapeLoadingContext &context )
{
    return true;
}

void ChartShape::saveOdf( KoShapeSavingContext & context ) const
{
    Q_ASSERT( d->plotArea );
    
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
    saveOdfLabel( d->title, bodyWriter, mainStyles, "title" );

    // 3. Write the subtitle.
    saveOdfLabel( d->subTitle, bodyWriter, mainStyles, "subtitle" );

    // 4. Write the footer.
    saveOdfLabel( d->footer, bodyWriter, mainStyles, "footer" );

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

void ChartShape::requestRepaint() const
{
    d->pixmapRepaintRequested = true;
}

} // Namespace KChart
