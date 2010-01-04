/* This file is part of the KDE project

   Copyright 2007 Stefan Nikolaus     <stefan.nikolaus@kdemail.net>
   Copyright 2007-2009 Inge Wallin    <inge@lysator.liu.se>
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


// Own
#include "ChartShape.h"

// Posix
#include <float.h> // For basic data types characteristics.

// Qt
#include <QPointF>
#include <QPainter>
#include <QSizeF>
#include <QTextDocument>
#include <QStandardItemModel>

// KDE
#include <KDebug>
#include <KApplication>
#include <KMessageBox>
#include <KMimeType>
#include <KUrl>

// KDChart
#include <KDChartChart>
#include <KDChartAbstractDiagram>
#include <KDChartCartesianAxis>
#include <KDChartCartesianCoordinatePlane>
#include <KDChartPolarCoordinatePlane>
#include "KDChartConvertions.h"
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

// KOffice
#include <KoShapeLoadingContext.h>
#include <KoOdfLoadingContext.h>
#include <KoEmbeddedDocumentSaver.h>
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
#include <KoDocumentEntry.h>
#include <KoOdfStylesReader.h>
#include <KoCanvasBase.h>
#include <KoShapeManager.h>
#include <KoSelection.h>
#include <KoShapeBackground.h>
#include <KoInsets.h>
#include <KoShapeBorderModel.h>
#include <KoColorBackground.h>
#include <KoLineBorder.h>

// KChart
#include "Axis.h"
#include "DataSet.h"
#include "Legend.h"
#include "PlotArea.h"
#include "Surface.h"
#include "ChartProxyModel.h"
#include "TextLabelDummy.h"
#include "ChartDocument.h"
#include "ChartTableModel.h"


// Define the protocol used here for embedded documents' URL
// This used to "store" but KUrl didn't like it,
// so let's simply make it "tar" !
#define STORE_PROTOCOL "tar"
#define INTERNAL_PROTOCOL "intern"

namespace KChart {

const char *ODF_CHARTTYPES[ NUM_CHARTTYPES ] = {
    "chart:bar",
    "chart:line",
    "chart:area",
    "chart:circle",
    "chart:ring",
    "chart:scatter",
    "chart:radar",
    "chart:stock",
    "chart:bubble",
    "chart:surface",
    "chart:gantt"
};

const ChartSubtype  defaultSubtypes[ NUM_CHARTTYPES ] = {
    NormalChartSubtype,     // Bar
    NormalChartSubtype,     // Line
    NormalChartSubtype,     // Area
    NoChartSubtype,         // Circle
    NoChartSubtype,         // Ring
    NoChartSubtype,         // Scatter
    NormalChartSubtype,     // Radar
    NoChartSubtype,         // Stock
    NoChartSubtype,         // Bubble
    NoChartSubtype,         // Surface
    NoChartSubtype          // Gantt
};

bool isPolar( ChartType type )
{
    switch ( type )
    {
    case CircleChartType:
    case RingChartType:
    case RadarChartType:
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

    label->loadOdf( labelElement, context );

    // TODO: Read optional attributes
    // 1. Table range
    KoXmlElement  pElement = KoXml::namedItemNS( labelElement,
                                                 KoXmlNS::text, "p" );

    labelData->document()->setPlainText( pElement.text() );

    label->setVisible( true );

    return true;
}

void saveOdfLabel( KoShape *label, KoXmlWriter &bodyWriter,
                   KoGenStyles &mainStyles, LabelType labelType )
{
    // Don't save hidden labels, as that's the way of removing them
    // from a chart.
    if ( !label->isVisible() )
        return;

    TextLabelData *labelData = qobject_cast<TextLabelData*>( label->userData() );
    if ( !labelData )
        return;

    if ( labelType == FooterLabelType )
        bodyWriter.startElement( "chart:footer" );
    else if ( labelType == SubTitleLabelType )
        bodyWriter.startElement( "chart:subtitle" );
    else // if ( labelType == TitleLabelType )
        bodyWriter.startElement( "chart:title" );

    bodyWriter.addAttributePt( "svg:x", label->position().x() );
    bodyWriter.addAttributePt( "svg:y", label->position().y() );
    // TODO: Save text label color
    bodyWriter.addAttribute( "chart:style-name", saveOdfFont( mainStyles, labelData->document()->defaultFont(), QColor() ) );

    bodyWriter.startElement( "text:p" );
    bodyWriter.addTextNode( labelData->document()->toPlainText() );
    bodyWriter.endElement(); // text:p
    bodyWriter.endElement(); // chart:title/subtitle/footer
}


const int NUM_DEFAULT_DATASET_COLORS = 12;

const char *defaultDataSetColors[NUM_DEFAULT_DATASET_COLORS] =
{
    "#004586",
    "#ff420e",
    "#ffd320",
    "#579d1c",
    "#7e0021",
    "#83caff",
    "#314004",
    "#aecf00",
    "#4b1f6f",
    "#ff950e",
    "#c5000b",
    "#0084d1",
};

QColor defaultDataSetColor( int dataSetNum )
{
    dataSetNum %= NUM_DEFAULT_DATASET_COLORS;
    return QColor( defaultDataSetColors[ dataSetNum ] );
}


// ================================================================
//                     The Private class


class ChartShape::Private
{
public:
    Private( ChartShape *shape );
    ~Private();

    void showLabel( KoShape *label );

    // The components of a chart
    KoShape   *title;
    KoShape   *subTitle;
    KoShape   *footer;
    Legend    *legend;
    PlotArea  *plotArea;

    // Data
    QAbstractItemModel  *internalModel; // The actual data
    ChartProxyModel     *proxyModel;	// What's presented to KDChart

    ChartDocument *document;

    ChartShape *shape;		// The chart that owns this ChartShape::Private

    QMap<QString, KoDataCenter*> dataCenterMap;
};


ChartShape::Private::Private( ChartShape *shape )
{
    // Register the owner.
    this->shape = shape;

    // Components
    title    = 0;
    subTitle = 0;
    footer   = 0;
    legend   = 0;
    plotArea = 0;

    // Data
    internalModel = 0;
    proxyModel    = 0;

    document = 0;
}

ChartShape::Private::~Private()
{
}


//
// Show a label, which means either the Title, Subtitle or Footer.
//
// If there is too little room, then make space by shrinking the Plotarea.
//
// FIXME: Make it take a bool parameter to show / hide the label in
//        question and then also grow the Plotarea again if the label
//        is hidden.
void ChartShape::Private::showLabel( KoShape *label )
{
    Q_ASSERT( label );

    const QSizeF  plotAreaSize = plotArea->size();
    if ( label->position().y() + label->size().height() / 2.0
         < shape->size().height() / 2.0 )
    {
        const qreal verticalSpaceRemaining = plotArea->position().y();
        const qreal spaceToExpand          = ( label->position().y() + label->size().height() ) - verticalSpaceRemaining;

        if ( spaceToExpand > 0.0 ) {
            plotArea->setSize( QSizeF( plotAreaSize.width(),
                                       plotAreaSize.height() - spaceToExpand ) );
            plotArea->setPosition( QPointF( plotArea->position().x(),
                                            plotArea->position().y() + spaceToExpand ) );
        }
    }
    else {
        const qreal verticalSpaceRemaining = label->position().y() - plotArea->position().y() - plotArea->size().height();
        qreal spaceToExpand = ( shape->size().height() - label->position().y() - label->size().height() ) - verticalSpaceRemaining;

        if ( spaceToExpand < 0.0 )
            spaceToExpand = 0.0;

        foreach ( Axis *axis, plotArea->axes() ) {
            if ( axis->position() != BottomAxisPosition )
                continue;

            qreal _spaceToExpand = ( label->size().height()
                                      - ( shape->size().height()
                                          - axis->title()->position().y()
                                          - axis->title()->size().height() ) );
            if ( _spaceToExpand > 0.0 ) {
                axis->title()->setPosition( axis->title()->position()
                                            - QPointF( 0.0, _spaceToExpand ) );
                spaceToExpand += _spaceToExpand;
            }
        }

        if ( spaceToExpand > 0.0 ) {
            plotArea->setSize( QSizeF( plotAreaSize.width(),
				       plotAreaSize.height() - spaceToExpand ) );
        }
    }

    label->setVisible( true );
}


// ================================================================
//                         Class ChartShape
// ================================================================


ChartShape::ChartShape()
    : KoFrameShape( KoXmlNS::draw, "object" )
    , d ( new Private( this ) )
{
    setShapeId( ChartShapeId );

    // Instantiated all children first
    d->proxyModel = new ChartProxyModel();

    d->plotArea = new PlotArea( this );
    d->document = new ChartDocument( this );
    d->legend   = new Legend( this );

    // Configure the plotarea.
    // We need this as the very first step, because some methods
    // here rely on the d->plotArea pointer.
    addChild( d->plotArea );
    d->plotArea->plotAreaInit();
    d->plotArea->setZIndex( 0 );
    setClipping( d->plotArea, true );

    // Configure the legend.
    d->legend->setVisible( true );
    d->legend->setZIndex( 1 );
    setClipping( d->legend, true );

    // A few simple defaults (chart type and subtype in this case)
    d->plotArea->setChartType( BarChartType );
    d->plotArea->setChartSubType( NormalChartSubtype );

    // Create the Title, which is a standard TextShape.
    // We use a empty dataCenterMap here for the creation and then in
    // init() we init the shapes with the correct data.
    QMap<QString, KoDataCenter *> dataCenterMap;
    d->title = KoShapeRegistry::instance()->value( TextShapeId )->createDefaultShapeAndInit( dataCenterMap );
    if ( !d->title ) {
        d->title = new TextLabelDummy;
        KMessageBox::error( 0, i18n("The plugin needed for displaying text labels in a chart is not available."), i18n("Plugin Missing") );
    }
    if ( dynamic_cast<TextLabelData*>( d->title->userData() ) == 0 ) {
        KMessageBox::error( 0, i18n("The plugin needed for displaying text labels is not compatible with the current version of the chart Flake shape."),
                            i18n("Plugin Incompatible") );
        TextLabelData *dataDummy = new TextLabelData;
        d->title->setUserData( dataDummy );
    }

    // Add the title to the shape
    addChild( d->title );
    QFont font = titleData()->document()->defaultFont();
    font.setPointSizeF( 12.0 );
    titleData()->document()->setDefaultFont( font );
    titleData()->document()->setHtml( "<div align=\"center\">" + i18n( "Title" ) + "</font></div>" );
    d->title->setSize( QSizeF( CM_TO_POINT( 5 ), CM_TO_POINT( 0.7 ) ) );
    d->title->setPosition( QPointF( size().width() / 2.0 - d->title->size().width() / 2.0, 0.0 ) );
    d->title->setVisible( false );
    d->title->setZIndex( 2 );
    setClipping( d->title, true );

    // Create the Subtitle and add it to the shape.
    d->subTitle = KoShapeRegistry::instance()->value( TextShapeId )->createDefaultShapeAndInit( dataCenterMap );
    if ( !d->subTitle ) {
        d->subTitle = new TextLabelDummy;
    }
    if ( dynamic_cast<TextLabelData*>( d->subTitle->userData() ) == 0 ) {
        TextLabelData *dataDummy = new TextLabelData;
        d->subTitle->setUserData( dataDummy );
    }
    addChild( d->subTitle );
    font = subTitleData()->document()->defaultFont();
    font.setPointSizeF( 10.0 );
    subTitleData()->document()->setDefaultFont( font );
    subTitleData()->document()->setHtml( "<div align=\"center\">" + i18n( "Subtitle" ) + "</div>" );
    d->subTitle->setSize( QSizeF( CM_TO_POINT( 5 ), CM_TO_POINT( 0.6 ) ) );
    d->subTitle->setPosition( QPointF( size().width() / 2.0 - d->title->size().width() / 2.0,
                                       d->title->size().height() ) );
    d->subTitle->setVisible( false );
    d->subTitle->setZIndex( 3 );
    setClipping( d->subTitle, true );

    // Create the Footer and add it to the shape.
    d->footer = KoShapeRegistry::instance()->value( TextShapeId )->createDefaultShapeAndInit( dataCenterMap );
    if ( !d->footer ) {
        d->footer = new TextLabelDummy;
    }
    if ( dynamic_cast<TextLabelData*>( d->subTitle->userData() ) == 0 ) {
        TextLabelData *dataDummy = new TextLabelData;
        d->footer->setUserData( dataDummy );
    }
    addChild( d->footer );
    font = footerData()->document()->defaultFont();
    font.setPointSizeF( 10.0 );
    footerData()->document()->setDefaultFont( font );
    footerData()->document()->setHtml( "<div align=\"center\">" + i18n( "Footer" ) + "</div>" );
    d->footer->setSize( QSizeF( CM_TO_POINT( 5 ), CM_TO_POINT( 0.6 ) ) );
    d->footer->setPosition( QPointF( size().width() / 2.0 - d->footer->size().width() / 2.0, size().height() - d->footer->size().height() ) );
    d->footer->setVisible( false );
    d->footer->setZIndex( 4 );
    setClipping( d->footer, true );

    KoColorBackground *background = new KoColorBackground( Qt::white );
    setBackground( background );

    KoLineBorder *border = new KoLineBorder( 0, Qt::black );
    setBorder( border );

    // Default size of the chart.
    KoShape::setSize( QSizeF( CM_TO_POINT( 8 ), CM_TO_POINT( 5 ) ) );

    requestRepaint();
}

ChartShape::~ChartShape()
{
    delete d->title;
    delete d->subTitle;
    delete d->footer;

    delete d->legend;
    delete d->plotArea;

    delete d->proxyModel;
    delete d->internalModel;    // Ok to call even when 0.

    delete d->document;

    delete d;
}


QAbstractItemModel *ChartShape::model() const
{
    // Can't return d->internalModel because the data may come from
    // the outside, e.g. a spreadsheet.  We only use d->internalModel
    // for a model that we own ourselves.
    return d->proxyModel->sourceModel();
}

ChartProxyModel *ChartShape::proxyModel() const
{
    return d->proxyModel;
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
    // There has to be a valid legend even, if it's hidden.
    Q_ASSERT( d->legend );
    return d->legend;
}

PlotArea *ChartShape::plotArea() const
{
    return d->plotArea;
}


void ChartShape::showTitle()
{
    d->showLabel( d->title );
}

void ChartShape::showSubTitle()
{
    d->showLabel( d->subTitle );
}

void ChartShape::showFooter()
{
    d->showLabel( d->footer );
}

void ChartShape::setModel( QAbstractItemModel *model,
                           bool takeOwnershipOfModel )
{
    Q_ASSERT( model );
    //kDebug(35001) << "Setting" << model << "as chart model.";

    // Only do something if we are not already using the new model.
    if ( model == d->internalModel )
        return;

    // If we already have an old internal model, delete it first.
    if ( d->internalModel ) {
        delete d->internalModel;
    }

    d->internalModel = ( takeOwnershipOfModel ? model : 0 );

    d->proxyModel->setSourceModel( model );

    requestRepaint();
}

void ChartShape::setModel( QAbstractItemModel *model,
                           const QVector<QRect> &selection )
{
    Q_ASSERT( model );
    kDebug(35001) << "Setting" << model << "as chart model.";
    kDebug(35001) << "Selection:" << selection;

    d->proxyModel->setSourceModel( model, selection );

    if ( d->internalModel ) {
        delete d->internalModel;
        d->internalModel = 0;
    }

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

static QPointF scalePointCenterLeft( QPointF center, qreal factorX, qreal factorY, const QSizeF &size )
{
    center.rx() -= size.width() / 2.0;
    center.rx() *= factorX;
    center.ry() *= factorY;
    center.rx() += size.width() / 2.0;

    return center;
}

static QPointF scalePointCenterRight( QPointF center, qreal factorX, qreal factorY, const QSizeF &size )
{
    center.rx() += size.width() / 2.0;
    center.rx() *= factorX;
    center.ry() *= factorY;
    center.rx() -= size.width() / 2.0;

    return center;
}

static QPointF scalePointCenterTop( QPointF center, qreal factorX, qreal factorY, const QSizeF &size )
{
    center.ry() -= size.height() / 2.0;
    center.rx() *= factorX;
    center.ry() *= factorY;
    center.ry() += size.height() / 2.0;

    return center;
}

static QPointF scalePointCenterBottom( QPointF center, qreal factorX, qreal factorY, const QSizeF &size )
{
    center.ry() += size.height() / 2.0;
    center.rx() *= factorX;
    center.ry() *= factorY;
    center.ry() -= size.height() / 2.0;

    return center;
}

void ChartShape::setSize( const QSizeF &newSize )
{
    Q_ASSERT( d->plotArea );

    const qreal factorX = newSize.width() / size().width();
    const qreal factorY = newSize.height() / size().height();

    // Reposition the Axes within the shape.
    foreach( Axis *axis, d->plotArea->axes() ) {
        KoShape *title = axis->title();
        switch( axis->position() ) {
        case TopAxisPosition:
            title->setAbsolutePosition( scalePointCenterTop( title->absolutePosition(), factorX, factorY, title->boundingRect().size() ) );
            break;
        case BottomAxisPosition:
            title->setAbsolutePosition( scalePointCenterBottom( title->absolutePosition(), factorX, factorY, title->boundingRect().size() ) );
            break;
        case LeftAxisPosition:
            title->setAbsolutePosition( scalePointCenterLeft( title->absolutePosition(), factorX, factorY, title->boundingRect().size() ) );
            break;
        case RightAxisPosition:
            title->setAbsolutePosition( scalePointCenterRight( title->absolutePosition(), factorX, factorY, title->boundingRect().size() ) );
            break;
        }
    }

    // Reposition the Legend within the shape.
    switch ( d->legend->legendPosition() ) {
    case TopLegendPosition:
        d->legend->setAbsolutePosition( scalePointCenterTop( d->legend->absolutePosition(), factorX, factorY, d->legend->boundingRect().size() ) );
        break;
    case BottomLegendPosition:
        d->legend->setAbsolutePosition( scalePointCenterBottom( d->legend->absolutePosition(), factorX, factorY, d->legend->boundingRect().size() ) );
        break;
    case StartLegendPosition:
        d->legend->setAbsolutePosition( scalePointCenterLeft( d->legend->absolutePosition(), factorX, factorY, d->legend->boundingRect().size() ) );
        break;
    case EndLegendPosition:
        d->legend->setAbsolutePosition( scalePointCenterRight( d->legend->absolutePosition(), factorX, factorY, d->legend->boundingRect().size() ) );
        break;
    case TopStartLegendPosition:
    case BottomStartLegendPosition:
    case TopEndLegendPosition:
    case BottomEndLegendPosition:
        // FIXME: These are not handled.
        break;
    }

    // Reposition the Title, Subtitle and Footer within the shape.
    d->title->setAbsolutePosition( scalePointCenterTop( d->title->absolutePosition(), factorX, factorY, d->title->boundingRect().size() ) );
    d->subTitle->setAbsolutePosition( scalePointCenterTop( d->subTitle->absolutePosition(), factorX, factorY, d->subTitle->boundingRect().size() ) );
    d->footer->setAbsolutePosition( scalePointCenterBottom( d->footer->absolutePosition(), factorX, factorY, d->footer->boundingRect().size() ) );

    // Finally, resize the plotarea.
    const QSizeF plotAreaSize = d->plotArea->size();
    d->plotArea->setSize( QSizeF( plotAreaSize.width() + newSize.width() - size().width(), 
                                  plotAreaSize.height() + newSize.height() - size().height() ) );

    // Oh yeah, the whole shape needs resizing too.
    KoShape::setSize( newSize );
}

void ChartShape::updateChildrenPositions()
{
    Q_ASSERT( d->plotArea );

    foreach( Axis *axis, d->plotArea->axes() ) {
        KoShape  *title = axis->title();
        QPointF   titlePosition;

        // FIXME BUG: titlePosition is uninitialized here!
        title->setPosition( titlePosition );
    }

    const qreal legendXOffset = 10.0;
    d->legend->setPosition( QPointF( size().width() + legendXOffset,
                                     size().height() / 2.0 - d->legend->size().height() / 2.0 ) );
}


// ----------------------------------------------------------------
//                         getters and setters


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
    d->proxyModel->setFirstRowIsLabel( isLabel );

    requestRepaint();
}

void ChartShape::setFirstColumnIsLabel( bool isLabel )
{
    d->proxyModel->setFirstColumnIsLabel( isLabel );

    requestRepaint();
}

void ChartShape::setDataDirection( Qt::Orientation orientation )
{
    Q_ASSERT( d->proxyModel );
    d->proxyModel->setDataDirection( orientation );
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


// ----------------------------------------------------------------


void ChartShape::paintComponent( QPainter &painter,
                                 const KoViewConverter &converter )
{
    // Paint the background
    if ( background() ) {
        applyConversion( painter, converter );

        // Calculate the clipping rect
        QRectF paintRect = QRectF( QPointF( 0, 0 ), size() );
        painter.setClipRect( paintRect );

        QPainterPath p;
        p.addRect( paintRect );
        background()->paint( painter, p );
    }
}

void ChartShape::paintDecorations( QPainter &painter,
                                   const KoViewConverter &converter,
                                   const KoCanvasBase *canvas )
{
    // This only is a helper decoration, do nothing if we're already
    // painting handles anyway.
    Q_ASSERT( canvas );
    if ( canvas->shapeManager()->selection()->selectedShapes().contains( this ) )
        return;

    if ( border() )
        return;

    QRectF border = QRectF( QPointF( -1.5, -1.5 ),
                            converter.documentToView( size() ) + QSizeF( 1.5, 1.5 ) );

    painter.setPen( QPen( Qt::lightGray, 0 ) );
    painter.drawRect( border );
}


// ----------------------------------------------------------------
//                         Loading and Saving


bool ChartShape::loadEmbeddedDocument( KoStore *store, 
                                       const KoXmlElement &objectElement, 
                                       const KoXmlDocument &manifestDocument )
{
    if ( !objectElement.hasAttributeNS( KoXmlNS::xlink, "href" ) ) {
        kError() << "Object element has no valid xlink:href attribute";
        return false;
    }

    QString url = objectElement.attributeNS( KoXmlNS::xlink, "href" );

    // It can happen that the url is empty e.g. when it is a
    // presentation:placeholder.
    if ( url.isEmpty() ) {
        return true;
    }

    QString tmpURL;
    if ( url[0] == '#' )
        url = url.mid( 1 );

    if (KUrl::isRelativeUrl( url )) {
        if ( url.startsWith( "./" ) )
            tmpURL = QString( INTERNAL_PROTOCOL ) + ":/" + url.mid( 2 );
        else
            tmpURL = QString( INTERNAL_PROTOCOL ) + ":/" + url;
    }
    else
        tmpURL = url;

    QString path = tmpURL;
    if ( tmpURL.startsWith( INTERNAL_PROTOCOL ) ) {
        path = store->currentDirectory();
        if ( !path.isEmpty() && !path.endsWith( '/' ) )
            path += '/';
        QString relPath = KUrl( tmpURL ).path();
        path += relPath.mid( 1 ); // remove leading '/'
    }
    if ( !path.endsWith( '/' ) )
        path += '/';

    const QString mimeType = KoOdfReadStore::mimeForPath( manifestDocument, path );
    //kDebug(35001) << "path for manifest file=" << path << "mimeType=" << mimeType;
    if ( mimeType.isEmpty() ) {
        //kDebug(35001) << "Manifest doesn't have media-type for" << path;
        return false;
    }

    const bool isOdf = mimeType.startsWith( "application/vnd.oasis.opendocument" );
    if ( !isOdf ) {
        tmpURL += "/maindoc.xml";
        //kDebug(35001) << "tmpURL adjusted to" << tmpURL;
    }

    //kDebug(35001) << "tmpURL=" << tmpURL;
    QString errorMsg;
    KoDocumentEntry e = KoDocumentEntry::queryByMimeType( mimeType );
    if ( e.isEmpty() ) {
        return false;
    }

    bool res = true;
    bool internalURL = false;
    if ( tmpURL.startsWith( STORE_PROTOCOL )
         || tmpURL.startsWith( INTERNAL_PROTOCOL )
         || KUrl::isRelativeUrl( tmpURL ) )
    {
        if ( isOdf ) {
            store->pushDirectory();
            Q_ASSERT( tmpURL.startsWith( INTERNAL_PROTOCOL ) );
            QString relPath = KUrl( tmpURL ).path().mid( 1 );
            store->enterDirectory( relPath );
            res = d->document->loadOasisFromStore( store );
            store->popDirectory();
        } else {
            if ( tmpURL.startsWith( INTERNAL_PROTOCOL ) )
                tmpURL = KUrl( tmpURL ).path().mid( 1 );
            res = d->document->loadFromStore( store, tmpURL );
        }
        internalURL = true;
        d->document->setStoreInternal( true );
    }
    else {
        // Reference to an external document. Hmmm...
        d->document->setStoreInternal( false );
        KUrl url( tmpURL );
        if ( !url.isLocalFile() ) {
            //QApplication::restoreOverrideCursor();

            // For security reasons we need to ask confirmation if the
            // url is remote.
            int result = KMessageBox::warningYesNoCancel(
                0, i18n( "This document contains an external link to a remote document\n%1", tmpURL ),
                i18n( "Confirmation Required" ), KGuiItem( i18n( "Download" ) ), KGuiItem( i18n( "Skip" ) ) );

            if ( result == KMessageBox::Cancel ) {
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

    if ( !res ) {
        QString errorMessage = d->document->errorMessage();
        return false;
    }
        // Still waiting...
        //QApplication::setOverrideCursor( Qt::WaitCursor );

    tmpURL.clear();

    // see KoDocument::insertChild for an explanation what's going on
    // now :-)
    /*if ( parentDocument() ) {
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

bool ChartShape::loadOdf( const KoXmlElement &element,
                          KoShapeLoadingContext &context )
{
    // Invalidate all data sets before we new load data sets from ODF
    // and *before* we delete existing axes.
    proxyModel()->invalidateDataSets();

    // When loading from ODF, all data sets are added explicitly.
    proxyModel()->setAutomaticDataSetCreation( false );

    // Load common attributes of (frame) shapes.  If you change here,
    // don't forget to also change in saveOdf().
    loadOdfAttributes( element, context, OdfMandatories | OdfGeometry | OdfAdditionalAttributes );
    return loadOdfFrame( element, context );
}

// Used to load the actual contents from the ODF frame that surrounds
// the chart in the ODF file.
bool ChartShape::loadOdfFrameElement( const KoXmlElement &element,
                                      KoShapeLoadingContext &context )
{
    if ( element.tagName() == "object" )
        return loadEmbeddedDocument( context.odfLoadingContext().store(),
                                     element,
                                     context.odfLoadingContext().manifestDocument() );

    qWarning() << "Unknown frame element <" << element.tagName() << ">";
    return false;
}

bool ChartShape::loadOdfEmbedded( const KoXmlElement &chartElement,
                                  KoShapeLoadingContext &context )
{
    KoStyleStack &styleStack = context.odfLoadingContext().styleStack();
    styleStack.save();

    styleStack.clear();
    if ( chartElement.hasAttributeNS( KoXmlNS::chart, "style-name" ) ) {
        context.odfLoadingContext().fillStyleStack( chartElement, KoXmlNS::chart, "style-name", "chart" );
        styleStack.setTypeProperties( "graphic" );
    }
    loadOdfAttributes( chartElement, context,
                       OdfAdditionalAttributes | OdfMandatories | OdfCommonChildElements | OdfStyle );

    // Check if we're loading an embedded document
    if ( !chartElement.hasAttributeNS( KoXmlNS::chart, "class" ) ) {
        kDebug(35001) << "Error: Embedded document has no chart:class attribute.";
        return false;
    }

    Q_ASSERT( d->plotArea );


    // 1. Load the chart type.
    const QString chartClass = chartElement.attributeNS( KoXmlNS::chart,
                                                         "class", QString() );
    // Find out what charttype the chart class corresponds to.
    bool  knownType = false;
    for ( int type = 0; type < (int)LastChartType; ++type ) {
        if ( chartClass == ODF_CHARTTYPES[ (ChartType)type ] ) {
            //kDebug(35001) <<"found chart of type" << chartClass;

            d->plotArea->setChartType( (ChartType)type );
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
        if ( !loadOdfLabel( d->title, titleElem, context) )
            return false;
    }

    // 3. Load the subtitle.
    KoXmlElement subTitleElem = KoXml::namedItemNS( chartElement,
                                                    KoXmlNS::chart, "subtitle" );
    if ( !subTitleElem.isNull() ) {
        if ( !loadOdfLabel( d->subTitle, subTitleElem, context) )
            return false;
    }

    // 4. Load the footer.
    KoXmlElement footerElem = KoXml::namedItemNS( chartElement,
                                                  KoXmlNS::chart, "footer" );
    if ( !footerElem.isNull() ) {
        if ( !loadOdfLabel( d->footer, footerElem, context ) )
            return false;
    }

    // 5. Load the legend.
    KoXmlElement legendElem = KoXml::namedItemNS( chartElement, KoXmlNS::chart,
                          "legend" );
    if ( !legendElem.isNull() ) {
        if ( !d->legend->loadOdf( legendElem, context ) )
            return false;
    }
    d->legend->update();

    // 6. Load the data
    KoXmlElement  dataElem = KoXml::namedItemNS( chartElement,
                                                 KoXmlNS::table, "table" );
    if ( !dataElem.isNull() ) {
        if ( !loadOdfData( dataElem, context ) )
            return false;
    }

    // 7. Load the plot area (this is where the meat is!).
    KoXmlElement  plotareaElem = KoXml::namedItemNS( chartElement,
                                                     KoXmlNS::chart, "plot-area" );
    if ( !plotareaElem.isNull() ) {
        if ( !d->plotArea->loadOdf( plotareaElem, context ) )
            return false;
    }

    requestRepaint();

    styleStack.restore();

    return true;
}

bool ChartShape::loadOdfData( const KoXmlElement &tableElement,
                              KoShapeLoadingContext &context )
{
    // There is no table element to load
    if ( tableElement.isNull() || !tableElement.isElement() )
        return true;

    // FIXME: Make model->loadOdf() return a bool, and use it here.
    ChartTableModel *model = new ChartTableModel;
    model->loadOdf( tableElement, context );

    setModel( model, true );

    return true;
}

void ChartShape::saveOdf( KoShapeSavingContext & context ) const
{
    Q_ASSERT( d->plotArea );

    KoXmlWriter&  bodyWriter = context.xmlWriter();

    // Check if we're saving to a chart document. If not, embed a
    // chart document.  ChartShape::saveOdf() will then be called
    // again later, when the current document saves the embedded
    // documents.
    //
    // FIXME: The check isEmpty() fixes a crash that happened when a
    //        chart shape was saved from KWord.  There are two
    //        problems with this fix:
    //        1. Checking the tag hierarchy is hardly the right way to do this
    //        2. The position doesn't seem to be saved yet.
    //
    //        Also, I have to check with the other apps, e.g. kspread,
    //        if it works there too.
    //
    QList<const char*>  tagHierarchy = bodyWriter.tagHierarchy();
    if ( tagHierarchy.isEmpty() 
         || QString( tagHierarchy.last() ) != "office:chart" )
    {
        bodyWriter.startElement( "draw:frame" );
        // See also loadOdf() in loadOdfAttributes.
        saveOdfAttributes( context, OdfMandatories | OdfGeometry | OdfAdditionalAttributes );

        bodyWriter.startElement( "draw:object" );
        context.embeddedSaver().embedDocument( bodyWriter, d->document );
        bodyWriter.endElement(); // draw:object

        bodyWriter.endElement(); // draw:frame
        return;
    }

    KoGenStyles&  mainStyles( context.mainStyles() );

    bodyWriter.startElement( "chart:chart" );

    saveOdfAttributes( context, OdfAllAttributes ^ OdfMandatories );

    KoGenStyle style;
    style = KoGenStyle( KoGenStyle::StyleGraphicAuto, "chart" );
    bodyWriter.addAttribute( "chart:style-name", saveStyle( style, context ) );

    // 1. Write the chart type.
    bodyWriter.addAttribute( "chart:class", ODF_CHARTTYPES[ d->plotArea->chartType() ] );

    // 2. Write the title.
    saveOdfLabel( d->title, bodyWriter, mainStyles, TitleLabelType );

    // 3. Write the subtitle.
    saveOdfLabel( d->subTitle, bodyWriter, mainStyles, SubTitleLabelType );

    // 4. Write the footer.
    saveOdfLabel( d->footer, bodyWriter, mainStyles, FooterLabelType );

    // 5. Write the legend.
    d->legend->saveOdf( context );

    // 6. Write the plot area (this is where the real action is!).
    d->plotArea->saveOdf( context );

    // 7. Save the data
    saveOdfData( bodyWriter, mainStyles );

    bodyWriter.endElement(); // chart:chart
}

void ChartShape::saveOdfData( KoXmlWriter &bodyWriter, KoGenStyles &mainStyles ) const
{
    Q_UNUSED( mainStyles );

    // Only save the data if we actually have some.
    if ( !d->internalModel )
        return;

    const int cols = d->internalModel->columnCount();
    const int rows = d->internalModel->rowCount();

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
    // FIXME: Shouldn't we actually save the headers here?
    bodyWriter.endElement(); // text:p
    bodyWriter.endElement(); // table:table-cell

    bodyWriter.endElement(); // table:table-row
    bodyWriter.endElement(); // table:table-header-rows

    bodyWriter.startElement( "table:table-rows" );
    //QStringList::const_iterator rowLabelIt = m_rowLabels.begin();
    for ( int row = 0; row < rows ; ++row ) {
        bodyWriter.startElement( "table:table-row" );
        for ( int col = 0; col < cols; ++col ) {
            //QVariant value( d->internalModel.cellVal( row, col ) );
            QModelIndex  index = d->internalModel->index( row, col );
            QVariant     value = d->internalModel->data( index );

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
            default:
                kDebug(35001) <<"ERROR: cell" << row <<"," << col
                              << " has unknown type." << endl;
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

void ChartShape::init( const QMap<QString, KoDataCenter *> & dataCenterMap )
{
    d->dataCenterMap = dataCenterMap;
    d->title->init( dataCenterMap );
    d->subTitle->init( dataCenterMap );
    d->footer->init( dataCenterMap );
}

void ChartShape::update() const
{
    KoShape::update();
}

void ChartShape::relayout() const
{
    Q_ASSERT( d->plotArea );
    d->plotArea->relayout();
    KoShape::update();
}

void ChartShape::requestRepaint() const
{
    Q_ASSERT( d->plotArea );
    d->plotArea->requestRepaint();
}

QMap<QString, KoDataCenter*> ChartShape::dataCenterMap() const
{
    return d->dataCenterMap;
}

} // Namespace KChart
