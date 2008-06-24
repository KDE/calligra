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
#include "ChartDocument.h"

// Posix
#include <float.h> // For basic data types characteristics.

// KOffice
#include <KoShapeLoadingContext.h>
#include <KoOdfLoadingContext.h>
#include <KoStore.h>
#include <KoDocument.h>
#include <KoShapeSavingContext.h>
#include <KoViewConverter.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>
#include <KoXmlNS.h>
#include <KoGenStyles.h>
#include <KoShapeRegistry.h>
#include <KoToolRegistry.h>
#include <KoTextShapeData.h>
#include <KoOdfReadStore.h>
#include <KoQueryTrader.h>
#include <KoOdfStylesReader.h>

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
#include <KMimeType>
#include <KUrl>

// Define the protocol used here for embedded documents' URL
// This used to "store" but KUrl didn't like it,
// so let's simply make it "tar" !
#define STORE_PROTOCOL "tar"
#define INTERNAL_PROTOCOL "intern"

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
    switch ( type )
    {
    case CircleChartType:
    case RingChartType:
    case RadarChartType:
    case BubbleChartType:
    case SurfaceChartType:
        return true;
    default:
        return false;
    }
    return false;
}

bool isCartesian( ChartType type )
{
    return !isPolar( type );
}

enum OdfLabelType {
    Title,
    SubTitle,
    Footer
};

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

bool loadOdfLabel( KoShape *label, KoXmlElement &labelElement, const KoOdfStylesReader &stylesReader )
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

void saveOdfLabel( KoShape *label, KoXmlWriter &bodyWriter, KoGenStyles &mainStyles, OdfLabelType odfLabelType )
{
    TextLabelData *labelData = qobject_cast<TextLabelData*>( label->userData() );
    if ( !labelData )
        return;
    
    if ( odfLabelType == Footer )
        bodyWriter.startElement( "chart:footer" );
    else if ( odfLabelType == SubTitle )
        bodyWriter.startElement( "chart:subtitle" );
    else // if ( odfLabelType == Title )
        bodyWriter.startElement( "chart:title" );
    bodyWriter.addAttributePt( "svg:x", label->position().x() );
    bodyWriter.addAttributePt( "svg:y", label->position().y() );
    // TODO: Save text label color
    bodyWriter.addAttribute( "chart:style-name", saveOdfFont( mainStyles, labelData->document()->defaultFont(), QColor() ) );
    bodyWriter.startElement( "text:p" );
    bodyWriter.addTextNode( labelData->document()->toPlainText() );
    bodyWriter.endElement(); // text:p
    bodyWriter.endElement(); // chart:title/subtitle/footer
    
    qDebug() << QString( "123test" );
}

const int MAX_PIXMAP_SIZE = 1000;

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
    bool     paintPixmap;
    QPointF  lastZoomLevel;
    QSizeF   lastSize;
    mutable bool pixmapRepaintRequested;
    
    ProxyModel *model;
    
    QList<KoShape*> hiddenChildren;
    
    ChartDocument *document;
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
    paintPixmap = true;
    document = 0;
}

ChartShape::Private::~Private()
{
}

ChartShape::ChartShape()
    : d ( new Private )
    , KoFrameShape( "KoXmlNS::draw", "object" )
{
    setShapeId( ChartShapeId );
    
    d->model = new ProxyModel();
    
    // We need this as the very first step, because some methods
    // here rely on the d->plotArea pointer
    d->plotArea = new PlotArea( this );
    
    d->document = new ChartDocument( this );
    qDebug() << QString( "123test" );
    
    d->legend = new Legend( this );
    d->legend->setVisible( true );
    
    QObject::connect( d->model, SIGNAL( modelReset() ), d->plotArea, SLOT( dataSetCountChanged() ) );
    QObject::connect( d->model, SIGNAL( rowsInserted( const QModelIndex, int, int ) ), d->plotArea, SLOT( dataSetCountChanged() ) );
    QObject::connect( d->model, SIGNAL( rowsRemoved( const QModelIndex, int, int ) ), d->plotArea, SLOT( dataSetCountChanged() ) );
    QObject::connect( d->model, SIGNAL( dataChanged() ), d->plotArea, SLOT( update() ) );
    
    d->plotArea->setChartType( BarChartType );
    d->plotArea->setChartSubType( NormalChartSubtype );
    
    d->title = KoShapeRegistry::instance()->value( TextShapeId )->createDefaultShapeAndInit( 0 );
    if ( !d->title )
    {
        d->title = new TextLabelDummy;
        KMessageBox::error( 0, i18n("The plugin needed for displaying text labels in a chart is not available."), i18n("Plugin Missing") );
    }
    if ( dynamic_cast<TextLabelData*>( d->title->userData() ) == 0 )
    {
        KMessageBox::error( 0, i18n("The plugin needed for displaying text labels is not compatible with the current version of the chart Flake shape."), i18n("Plugin Incompatible") );
        TextLabelData *dataDummy = new TextLabelData;
        d->title->setUserData( dataDummy );
    }
    
    addChild( d->title );
    titleData()->document()->setPlainText( i18n( "Title" ) );
    d->title->setVisible( false );

    d->subTitle = KoShapeRegistry::instance()->value( TextShapeId )->createDefaultShapeAndInit( 0 );
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

    d->footer = KoShapeRegistry::instance()->value( TextShapeId )->createDefaultShapeAndInit( 0 );
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
    delete d->title;
    delete d->subTitle;
    delete d->footer;
    delete d->document;
    delete d->floor;
    delete d->wall;
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
    // There has to be a valid legend
    // even, if it's hidden
    Q_ASSERT( d->legend );
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
    
    const double legendXOffset = 10.0;
    d->legend->setPosition( QPointF( size().width() + legendXOffset, size().height() / 2.0 - d->legend->size().height() / 2.0 ) );
}

QRectF ChartShape::boundingRect() const
{
    QRectF rect = KoShape::boundingRect();
    
    foreach( KoShape *shape, iterator() )
    {
        // Before we can hide the child, we need to do a repaint
        // with the old bounding rect, to avoid shapes not being
        // painted over (i.e. hidden) because of the changed geometry
        if ( !shape->isVisible() && d->hiddenChildren.contains( shape ) )
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

void ChartShape::setDataDirection( Qt::Orientation orientation )
{
    Q_ASSERT( d->model );
    d->model->setDataDirection( orientation );
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
    const QRect paintRect = QRect( QPoint( 0, 0 ), paintRectSize );

    // Only use a pixmap with sane sizes
    d->paintPixmap = paintRectSize.width() < MAX_PIXMAP_SIZE || paintRectSize.height() < MAX_PIXMAP_SIZE;
    
    if ( d->paintPixmap ) {
        d->image = QImage( paintRectSize, QImage::Format_ARGB32 );
    
        // Copy the painter's render hints, such as antialiasing
        QPainter pixmapPainter( &d->image );
        pixmapPainter.setRenderHints( painter.renderHints() );
        pixmapPainter.setRenderHint( QPainter::Antialiasing, false );
    
        // Paint the background
        QBrush bgBrush( Qt::white );
        if ( d->plotArea->chartType() == CircleChartType || d->plotArea->chartType() == RingChartType )
            bgBrush = QBrush( QColor( 255, 255, 255, 0 ) );
        pixmapPainter.fillRect( paintRect, bgBrush );
    
        // scale the painter's coordinate system to fit the current zoom level
        applyConversion( pixmapPainter, converter );
    
        d->plotArea->paint( pixmapPainter );
    } else {
        // Paint the background
        painter.fillRect( paintRect, QColor( 255, 255, 255, 0 ) );
    
        // scale the painter's coordinate system to fit the current zoom level
        applyConversion( painter, converter );
    
        d->plotArea->paint( painter );
    }
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
         || d->lastSize      != size()
         || !d->paintPixmap ) {
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
    
    foreach( KoShape *shape, iterator() )
    {
        if ( !shape->isVisible() && !d->hiddenChildren.contains( shape ) )
            d->hiddenChildren.append( shape );
        else if ( d->hiddenChildren.contains( shape ) )
            d->hiddenChildren.removeAll( shape );
    }

    // Paint the cached pixmap if we got a GO from paintPixmap()
    if ( d->paintPixmap )
        painter.drawImage( 0, 0, d->image );
}

void ChartShape::paintComponent( QPainter &painter, const KoViewConverter &converter )
{
}

bool ChartShape::loadOdfFrame( const KoXmlElement &chartElement, KoShapeLoadingContext &context )
{
    qDebug() << "loadOdfFrame()";
    return true;
}

bool ChartShape::loadOdfFrameElement( const KoXmlElement &chartElement, KoShapeLoadingContext &context )
{
    qDebug() << "loadOdfFrameElement()";
    return true;
}

bool ChartShape::loadEmbeddedDocument( KoStore *store, const KoXmlElement &objectElement, const KoXmlDocument &manifestDocument )
{
    QString url = objectElement.attributeNS( KoXmlNS::xlink, "href", QString() );
    QString m_tmpURL;
    if ( url[0] == '#' )
        url = url.mid( 1 );
    if ( url.startsWith( "./" ) )
        m_tmpURL = QString( INTERNAL_PROTOCOL ) + ":/" + url.mid( 2 );
    else
        m_tmpURL = url;
    qDebug() << m_tmpURL;
    ////////////////////////////
    QString path = m_tmpURL;
    if ( m_tmpURL.startsWith( INTERNAL_PROTOCOL ) ) {
        path = store->currentDirectory();
        if ( !path.isEmpty() )
            path += '/';
        QString relPath = KUrl( m_tmpURL ).path();
        path += relPath.mid( 1 ); // remove leading '/'
    }
    if ( !path.endsWith( '/' ) )
        path += '/';
    const QString mimeType = KoOdfReadStore::mimeForPath( manifestDocument, path );
    qDebug() <<"path for manifest file=" << path <<" mimeType=" << mimeType;
    if ( mimeType.isEmpty() ) {
        qDebug() << "Manifest doesn't have media-type for " << path << endl;
        return false;
    }

    const bool oasis = mimeType.startsWith( "application/vnd.oasis.opendocument" );
    if ( !oasis ) {
        m_tmpURL += "/maindoc.xml";
        kDebug(30003) <<" m_tmpURL adjusted to" << m_tmpURL;
    }
    ///////////////////////////
    qDebug() <<"KoDocumentChild::loadDocumentInternal m_tmpURL=" << m_tmpURL;
    QString errorMsg;
    KoDocumentEntry e = KoDocumentEntry::queryByMimeType( mimeType );
    if ( e.isEmpty() )
    {
        return false;
    }

    //////////////////////////////
    bool res = true;
    bool internalURL = false;
    if ( m_tmpURL.startsWith( STORE_PROTOCOL ) || m_tmpURL.startsWith( INTERNAL_PROTOCOL ) || KUrl::isRelativeUrl( m_tmpURL ) )
    {
        if ( oasis ) {
            store->pushDirectory();
            Q_ASSERT( m_tmpURL.startsWith( INTERNAL_PROTOCOL ) );
            QString relPath = KUrl( m_tmpURL ).path().mid( 1 );
            store->enterDirectory( relPath );
            res = d->document->loadOasisFromStore( store );
            store->popDirectory();
        } else {
            if ( m_tmpURL.startsWith( INTERNAL_PROTOCOL ) )
                m_tmpURL = KUrl( m_tmpURL ).path().mid( 1 );
            res = d->document->loadFromStore( store, m_tmpURL );
        }
        internalURL = true;
        d->document->setStoreInternal( true );
    }
    else
    {
        // Reference to an external document. Hmmm...
        d->document->setStoreInternal( false );
        KUrl url( m_tmpURL );
        if ( !url.isLocalFile() )
        {
            //QApplication::restoreOverrideCursor();
            // For security reasons we need to ask confirmation if the url is remote
            int result = KMessageBox::warningYesNoCancel(
                0, i18n( "This document contains an external link to a remote document\n%1", m_tmpURL),
                i18n( "Confirmation Required" ), KGuiItem(i18n( "Download" )), KGuiItem(i18n( "Skip" ) ));

            if ( result == KMessageBox::Cancel )
            {
                //d->m_parent->setErrorMessage("USER_CANCELED");
                return false;
            }
            if ( result == KMessageBox::Yes )
                res = d->document->openUrl( url );
            // and if == No, res will still be false so we'll use a kounavail below
        }
        else
            res = d->document->openUrl( url );
    }
    if ( !res )
    {
        QString errorMessage = d->document->errorMessage();
        return false;
    }
        // Still waiting...
        //QApplication::setOverrideCursor( Qt::WaitCursor );

    m_tmpURL = QString();

    // see KoDocument::insertChild for an explanation what's going on
    // now :-)
    /*if ( parentDocument() )
    {
        KoDocument *parent = parentDocument();

        KParts::PartManager* manager = parent->manager();
        if ( manager && !manager->parts().isEmpty() )
        {
            if ( !manager->parts().contains( d->document ) &&
                 !parent->isSingleViewMode() )
                manager->addPart( d->document, false );
        }
    }*/

    //QApplication::restoreOverrideCursor();

    return res;
}

bool ChartShape::loadOdf( const KoXmlElement &chartElement, KoShapeLoadingContext &context )
{
    // Check if we're loading an embedded document
    if ( chartElement.tagName() != "frame" )
        return false;
    
    const qreal x = KoUnit::parseValue( chartElement.attributeNS( KoXmlNS::svg, "x" ) );
    const qreal y = KoUnit::parseValue( chartElement.attributeNS( KoXmlNS::svg, "x" ) );
    const qreal width = KoUnit::parseValue( chartElement.attributeNS( KoXmlNS::svg, "width" ) );
    const qreal height = KoUnit::parseValue( chartElement.attributeNS( KoXmlNS::svg, "height" ) );
    
    setPosition( QPointF( x, y ) );
    setSize( QSizeF( width, height ) );
    
    KoXmlElement objectElement = KoXml::namedItemNS( chartElement, KoXmlNS::draw, "object" );
    
    return loadEmbeddedDocument( context.odfLoadingContext().store(), objectElement, context.odfLoadingContext().manifestDocument() );
}

bool ChartShape::loadOdfEmbedded( const KoXmlElement &chartElement, const KoOdfStylesReader &stylesReader )
{
    // Check if we're loading an embedded document
    if ( !chartElement.hasAttributeNS( KoXmlNS::chart, "class" ) ) {
        qDebug() << "Embedded document has no chart:class element. Aborting.";
        return false;
    }

    Q_ASSERT( d->plotArea );


    // 1. Load the chart type.
    const QString chartClass = chartElement.attributeNS( KoXmlNS::chart,
                                                         "class", QString() );
    // Find out what charttype the chart class corresponds to.
    bool  knownType = false;
    for ( unsigned int i = 0 ; i < numChartTypes ; ++i ) {
        if ( chartClass == odfChartTypes[i].odfName ) {
            qDebug() <<"found chart of type" << chartClass;

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
        if ( !loadOdfLabel( d->title, titleElem, stylesReader) )
            return false;
    }

    // 3. Load the subtitle.
    KoXmlElement subTitleElem = KoXml::namedItemNS( chartElement, 
                                                    KoXmlNS::chart, "subtitle" );
    if ( !subTitleElem.isNull() ) {
        if ( !loadOdfLabel( d->subTitle, subTitleElem, stylesReader) )
            return false;
    }

    // 4. Load the footer.
    KoXmlElement footerElem = KoXml::namedItemNS( chartElement, 
                                                  KoXmlNS::chart, "footer" );
    if ( !footerElem.isNull() ) {
        if ( !loadOdfLabel( d->footer, footerElem, stylesReader) )
            return false;
    }

    // 5. Load the legend.
    KoXmlElement legendElem = KoXml::namedItemNS( chartElement, KoXmlNS::chart,
                          "legend" );
    if ( !legendElem.isNull() ) {
    if ( !d->legend->loadOdf( legendElem, stylesReader ) )
        return false;
    }

    // 6. Load the plot area (this is where the real action is!).
    KoXmlElement  plotareaElem = KoXml::namedItemNS( chartElement,
                             KoXmlNS::chart, "plot-area" );
    if ( !plotareaElem.isNull() ) {
    if ( !d->plotArea->loadOdf( plotareaElem, stylesReader ) )
        return false;
    }

    // 7. Load the data
    KoXmlElement  dataElem = KoXml::namedItemNS( chartElement,
                         KoXmlNS::table, "table" );
    if ( !dataElem.isNull() ) {
    if ( !loadOdfData( dataElem, stylesReader ) )
        return false;
    }
    
    requestRepaint();

    return true;
}

bool ChartShape::loadOdfData( const KoXmlElement &chartElement, const KoOdfStylesReader &stylesReader )
{
    return true;
}

void ChartShape::saveOdf( KoShapeSavingContext & context ) const
{
    // Create document
    // Add document using KoEmbeddedDocumentSaver::embedDocument
    // That's it =)
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
    saveOdfLabel( d->title, bodyWriter, mainStyles, Title );

    // 3. Write the subtitle.
    saveOdfLabel( d->subTitle, bodyWriter, mainStyles, SubTitle );

    // 4. Write the footer.
    saveOdfLabel( d->footer, bodyWriter, mainStyles, Footer );

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
