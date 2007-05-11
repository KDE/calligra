/****************************************************************************
 ** Copyright (C) 2006 Klar�vdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KD Chart library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KD Chart licenses may use this file in
 ** accordance with the KD Chart Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.kdab.net/kdchart for
 **   information about KDChart Commercial License Agreements.
 **
 ** Contact info@kdab.net if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/

#include "KDChartLegend.h"
#include "KDChartLegend_p.h"
#include <KDChartTextAttributes.h>
#include <KDChartMarkerAttributes.h>
#include <QFont>
#include <QPainter>
#include <QTextTableCell>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QTextDocumentFragment>
#include <QTimer>
#include <QAbstractTextDocumentLayout>
#include <QtDebug>
#include <QLabel>
#include <KDChartAbstractDiagram.h>
#include "KDTextDocument.h"
#include <KDChartDiagramObserver.h>
#include <QGridLayout>
#include "KDChartLayoutItems.h"

#include <KDABLibFakes>

using namespace KDChart;

Legend::Private::Private() :
    referenceArea(0),
    position( Position::East ),
    alignment( Qt::AlignCenter ),
    relativePosition( RelativePosition() ),
    orientation( Qt::Vertical ),
    showLines( false ),
    texts(),
    textAttributes(),
    titleText( QObject::tr( "Legend" ) ),
    titleTextAttributes(),
    spacing( 1 ),
    useAutomaticMarkerSize( true ),
    legendStyle( MarkersOnly )
    //needRebuild( true )
{
    // By default we specify a simple, hard point as the 'relative' position's ref. point,
    // since we can not be sure that there will be any parent specified for the legend.
    relativePosition.setReferencePoints(   PositionPoints( QPointF( 0.0, 0.0 ) ) );
    relativePosition.setReferencePosition( Position::NorthWest );
    relativePosition.setAlignment( Qt::AlignTop | Qt::AlignLeft );
    relativePosition.setHorizontalPadding( KDChart::Measure( 4.0, KDChartEnums::MeasureCalculationModeAbsolute ) );
    relativePosition.setVerticalPadding(   KDChart::Measure( 4.0, KDChartEnums::MeasureCalculationModeAbsolute ) );
}

Legend::Private::~Private()
{
    // this bloc left empty intentionally
}



#define d d_func()


Legend::Legend( QWidget* parent ) :
    AbstractAreaWidget( new Private(), parent )
{
    d->referenceArea = parent;
    init();
}

Legend::Legend( KDChart::AbstractDiagram* diagram, QWidget* parent ) :
    AbstractAreaWidget( new Private(), parent )
{
    d->referenceArea = parent;
    init();
    setDiagram( diagram );
}

Legend::~Legend()
{
    emit destroyedLegend( this );
}

void Legend::init()
{
    setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );

    d->layout = new QGridLayout( this );
    d->layout->setMargin( 2 );
    d->layout->setSpacing( d->spacing );
    //setLayout( d->layout );

    const Measure normalFontSizeTitle(  12, KDChartEnums::MeasureCalculationModeAbsolute );
    const Measure normalFontSizeLabels( 10, KDChartEnums::MeasureCalculationModeAbsolute );
    const Measure minimalFontSize(       4, KDChartEnums::MeasureCalculationModeAbsolute );

    TextAttributes textAttrs;
    textAttrs.setPen( QPen( Qt::black ) );
    textAttrs.setFont( QFont( QLatin1String( "helvetica" ), 10, QFont::Normal, false ) );
    textAttrs.setFontSize(        normalFontSizeLabels );
    textAttrs.setMinimalFontSize( minimalFontSize );
    setTextAttributes( textAttrs );

    TextAttributes titleTextAttrs;
    titleTextAttrs.setPen( QPen( Qt::black ) );
    titleTextAttrs.setFont( QFont( QLatin1String( "helvetica" ), 12, QFont::Bold, false ) );
    titleTextAttrs.setFontSize(        normalFontSizeTitle );
    titleTextAttrs.setMinimalFontSize( minimalFontSize );
    setTitleTextAttributes( titleTextAttrs );

    FrameAttributes frameAttrs;
    frameAttrs.setVisible( true );
    frameAttrs.setPen( QPen( Qt::black ) );
    frameAttrs.setPadding( 1 );
    setFrameAttributes( frameAttrs );

    d->position = Position::NorthEast;
    d->alignment = Qt::AlignCenter;
}


QSize Legend::minimumSizeHint() const
{
    return sizeHint();
}

//#define DEBUG_LEGEND_PAINT

QSize Legend::sizeHint() const
{
#ifdef DEBUG_LEGEND_PAINT
    qDebug()  << "Legend::sizeHint() started";
#endif
    Q_FOREACH( KDChart::AbstractLayoutItem* layoutItem, d->layoutItems ) {
        layoutItem->sizeHint();
    }
    return AbstractAreaWidget::sizeHint();
}

void Legend::needSizeHint()
{
    // Re-build the Legend's content, if it has not been build yet,
    // or if the Legend's geometry has changed, resp.
    buildLegend();
}

void Legend::resizeLayout( const QSize& size )
{
#ifdef DEBUG_LEGEND_PAINT
    qDebug() << "Legend::resizeLayout started";
#endif
    if( d->layout ){
        d->layout->setGeometry( QRect(QPoint(0,0), size) );
        activateTheLayout();
    }
#ifdef DEBUG_LEGEND_PAINT
    qDebug() << "Legend::resizeLayout done";
#endif
}

void Legend::activateTheLayout()
{
    if( d->layout && d->layout->parent() )
        d->layout->activate();
}


void Legend::setLegendStyle( LegendStyle style )
{
    if( d->legendStyle == style ) return;
    d->legendStyle = style;
    setNeedRebuild();
}

Legend::LegendStyle Legend::legendStyle() const
{
    return d->legendStyle;
}

Legend* Legend::clone() const
{
    Legend* legend = new Legend( new Private( *d ), 0 );
    legend->setTextAttributes( textAttributes() );
    legend->setTitleTextAttributes( titleTextAttributes() );
    legend->setFrameAttributes( frameAttributes() );
    legend->setUseAutomaticMarkerSize( useAutomaticMarkerSize() );
    legend->setPosition( position() );
    legend->setAlignment( alignment() );
    legend->setLegendStyle( legendStyle() );
    return legend;
}


bool Legend::compare( const Legend* other )const
{
    if( other == this ) return true;
    if( ! other ){
        //qDebug() << "Legend::compare() cannot compare to Null pointer";
        return false;
    }
    /*
    qDebug() << ( static_cast<const AbstractAreaBase*>(this)->compare( other ) );
    qDebug() << (isVisible()              == other->isVisible());
    qDebug() << (position()               == other->position());
    qDebug() << (alignment()              == other->alignment());
    qDebug() << (floatingPosition()       == other->floatingPosition());
    qDebug() << (orientation()            == other->orientation());
    qDebug() << (showLines()              == other->showLines());
    qDebug() << (texts()                  == other->texts());
    qDebug() << (brushes()                == other->brushes());
    qDebug() << (pens()                   == other->pens());
    qDebug() << (markerAttributes()       == other->markerAttributes());
    qDebug() << (useAutomaticMarkerSize() == other->useAutomaticMarkerSize());
    qDebug() << (textAttributes()         == other->textAttributes());
    qDebug() << (titleText()              == other->titleText());
    qDebug() << (titleTextAttributes()    == other->titleTextAttributes());
    qDebug() << (spacing()                == other->spacing());
    qDebug() << (legendStyle()            == other->legendStyle());
    */
    return  ( static_cast<const AbstractAreaBase*>(this)->compare( other ) ) &&
            (isVisible()              == other->isVisible()) &&
            (position()               == other->position()) &&
            (alignment()              == other->alignment())&&
            (floatingPosition()       == other->floatingPosition()) &&
            (orientation()            == other->orientation())&&
            (showLines()              == other->showLines())&&
            (texts()                  == other->texts())&&
            (brushes()                == other->brushes())&&
            (pens()                   == other->pens())&&
            (markerAttributes()       == other->markerAttributes())&&
            (useAutomaticMarkerSize() == other->useAutomaticMarkerSize()) &&
            (textAttributes()         == other->textAttributes()) &&
            (titleText()              == other->titleText())&&
            (titleTextAttributes()    == other->titleTextAttributes()) &&
            (spacing()                == other->spacing()) &&
            (legendStyle()            == other->legendStyle());
}


void Legend::paint( QPainter* painter )
{
#ifdef DEBUG_LEGEND_PAINT
    qDebug() << "entering Legend::paint( QPainter* painter )";
#endif
    // rule: We do not show a legend, if there is no diagram.
    if( ! diagram() ) return;

    // re-calculate/adjust the Legend's internal layout and contents, if needed:
    //buildLegend();

    // PENDING(kalle) Support palette

    Q_FOREACH( KDChart::AbstractLayoutItem* layoutItem, d->layoutItems ) {
        layoutItem->paint( painter );
    }
#ifdef DEBUG_LEGEND_PAINT
    qDebug() << "leaving Legend::paint( QPainter* painter )";
#endif
}


uint Legend::datasetCount() const
{
    int modelLabelsCount = 0;
    int modelBrushesCount = 0;
    for (int i = 0; i < d->observers.size(); ++i) {
        DiagramObserver * obs = d->observers.at(i);
        modelLabelsCount  += obs->diagram()->datasetLabels().count();
        modelBrushesCount += obs->diagram()->datasetBrushes().count();
    }
    Q_ASSERT( modelLabelsCount == modelBrushesCount );
    return modelLabelsCount;
}


void Legend::setReferenceArea( const QWidget* area )
{
    if( area == d->referenceArea ) return;
    d->referenceArea = area;
    setNeedRebuild();
}

const QWidget* Legend::referenceArea() const
{
    //qDebug() << d->referenceArea;
    return (d->referenceArea ? d->referenceArea : static_cast<const QWidget*>(parent()));
}


AbstractDiagram* Legend::diagram() const
{
    if( d->observers.isEmpty() )
        return 0;
    return d->observers.first()->diagram();
}

DiagramList Legend::diagrams() const
{
    DiagramList list;
    for (int i = 0; i < d->observers.size(); ++i)
        list << d->observers.at(i)->diagram();
    return list;
}

ConstDiagramList Legend::constDiagrams() const
{
    ConstDiagramList list;
    for (int i = 0; i < d->observers.size(); ++i)
        list << d->observers.at(i)->diagram();
    return list;
}

void Legend::addDiagram( AbstractDiagram* newDiagram )
{
    if ( newDiagram )
    {
        DiagramObserver* observer = new DiagramObserver( newDiagram, this );

        DiagramObserver* oldObs = d->findObserverForDiagram( newDiagram );
        if( oldObs ){
            delete oldObs;
            d->observers[ d->observers.indexOf( oldObs ) ] = observer;
        }else{
            d->observers.append( observer );
        }
        connect( observer, SIGNAL( diagramDestroyed(AbstractDiagram*) ),
                        SLOT( resetDiagram(AbstractDiagram*) ));
        connect( observer, SIGNAL( diagramDataChanged(AbstractDiagram*) ),
                 SLOT( setNeedRebuild() ));
        connect( observer, SIGNAL( diagramDataHidden(AbstractDiagram*) ),
                 SLOT( setNeedRebuild() ));
        connect( observer, SIGNAL( diagramAttributesChanged(AbstractDiagram*) ),
                        SLOT( setNeedRebuild() ));
        setNeedRebuild();
    }
}

void Legend::removeDiagram( AbstractDiagram* oldDiagram )
{
    if( oldDiagram ){
        DiagramObserver* oldObs = d->findObserverForDiagram( oldDiagram );
        if( oldObs ){
            //qDebug() << "before delete oldObs;";
            delete oldObs;
            //qDebug() << "after delete oldObs;";
            d->observers.removeAt( d->observers.indexOf( oldObs ) );
            //qDebug() << "after d->observers.removeAt()";
        }
        setNeedRebuild();
    }
}

void Legend::removeDiagrams()
{
    for (int i = 0; i < d->observers.size(); ++i)
        removeDiagram( d->observers.at(i)->diagram() );
}

void Legend::replaceDiagram( AbstractDiagram* newDiagram,
                             AbstractDiagram* oldDiagram )
{
    KDChart::AbstractDiagram* old = oldDiagram;
    if( ! d->observers.isEmpty() && ! old ){
        old = d->observers.first()->diagram();
        if( ! old )
            d->observers.removeFirst(); // first entry had a 0 diagram
    }
    if( old )
        removeDiagram( old );
    if( newDiagram )
        addDiagram( newDiagram );
}

void Legend::setDiagram( KDChart::AbstractDiagram* newDiagram )
{
    replaceDiagram( newDiagram );
}

void Legend::resetDiagram( AbstractDiagram* oldDiagram )
{
    //qDebug() << oldDiagram;
    removeDiagram( oldDiagram );
}

void Legend::setVisible( bool visible )
{
    QWidget::setVisible( visible );
    emitPositionChanged();
}

void Legend::setNeedRebuild()
{
    //qDebug() << "setNeedRebuild()";
    buildLegend();
    sizeHint();
}

void Legend::setPosition( Position position )
{
    d->position = position;
    emitPositionChanged();
}

void Legend::emitPositionChanged()
{
    emit positionChanged( this );
    emit propertiesChanged();
}


Position Legend::position() const
{
    return d->position;
}

void Legend::setAlignment( Qt::Alignment alignment )
{
    d->alignment = alignment;
    emitPositionChanged();
}

Qt::Alignment Legend::alignment() const
{
    return d->alignment;
}

void Legend::setFloatingPosition( const RelativePosition& relativePosition )
{
    d->position = Position::Floating;
    if( d->relativePosition != relativePosition ){
        d->relativePosition  = relativePosition;
        emitPositionChanged();
    }
}

const RelativePosition Legend::floatingPosition() const
{
    return d->relativePosition;
}

void Legend::setOrientation( Qt::Orientation orientation )
{
    if( d->orientation == orientation ) return;
    d->orientation = orientation;
    setNeedRebuild();
    emitPositionChanged();
}

Qt::Orientation Legend::orientation() const
{
    return d->orientation;
}

void Legend::setShowLines( bool legendShowLines )
{
    if( d->showLines == legendShowLines ) return;
    d->showLines = legendShowLines;
    setNeedRebuild();
    emitPositionChanged();
}

bool Legend::showLines() const
{
    return d->showLines;
}

void Legend::setUseAutomaticMarkerSize( bool useAutomaticMarkerSize )
{
    d->useAutomaticMarkerSize = useAutomaticMarkerSize;
    setNeedRebuild();
    emitPositionChanged();
}

bool Legend::useAutomaticMarkerSize() const
{
    return d->useAutomaticMarkerSize;
}

/**
    \brief Removes all legend texts that might have been set by setText.

    This resets the Legend to default behaviour: Texts are created automatically.
*/
void Legend::resetTexts()
{
    if( ! d->texts.count() ) return;
    d->texts.clear();
    setNeedRebuild();
}

void Legend::setText( uint dataset, const QString& text )
{
    if( d->texts[ dataset ] == text ) return;
    d->texts[ dataset ] = text;
    setNeedRebuild();
}

QString Legend::text( uint dataset ) const
{
    if( d->texts.find( dataset ) != d->texts.end() ){
        //qDebug() << "Legend::text(" << dataset << ") returning d->texts[" << dataset << "] :" << d->texts[ dataset ];
        return d->texts[ dataset ];
    }else{
        //qDebug() << "Legend::text(" << dataset << ") returning d->modelLabels[" << dataset << "] :" << d->modelLabels[ dataset ];
        return d->modelLabels[ dataset ];
    }
}

const QMap<uint,QString> Legend::texts() const
{
    return d->texts;
}

void Legend::setColor( uint dataset, const QColor& color )
{
    if( d->brushes[ dataset ] == color ) return;
    d->brushes[ dataset ] = color;
    setNeedRebuild();
}

void Legend::setBrush( uint dataset, const QBrush& brush )
{
    if( d->brushes[ dataset ] == brush ) return;
    d->brushes[ dataset ] = brush;
    setNeedRebuild();
}

QBrush Legend::brush( uint dataset ) const
{
    if( d->brushes.find( dataset ) != d->brushes.end() )
        return d->brushes[ dataset ];
    else
        return d->modelBrushes[ dataset ];
}

const QMap<uint,QBrush> Legend::brushes() const
{
    return d->brushes;
}


void Legend::setBrushesFromDiagram( KDChart::AbstractDiagram* diagram )
{
    bool bChangesDone = false;
    QList<QBrush> datasetBrushes = diagram->datasetBrushes();
    for( int i = 0; i < datasetBrushes.count(); i++ ){
        if( d->brushes[ i ] != datasetBrushes[ i ] ){
            d->brushes[ i ]  = datasetBrushes[ i ];
            bChangesDone = true;
        }
    }
    if( bChangesDone )
        setNeedRebuild();
}


void Legend::setPen( uint dataset, const QPen& pen )
{
    if( d->pens[dataset] == pen ) return;
    d->pens[dataset] = pen;
    setNeedRebuild();
}

QPen Legend::pen( uint dataset ) const
{
    if( d->pens.find( dataset ) != d->pens.end() )
        return d->pens[dataset];
    else
        return d->modelPens[ dataset ];
}

const QMap<uint,QPen> Legend::pens() const
{
    return d->pens;
}


void Legend::setMarkerAttributes( uint dataset, const MarkerAttributes& markerAttributes )
{
    if( d->markerAttributes[dataset] == markerAttributes ) return;
    d->markerAttributes[ dataset ] = markerAttributes;
    setNeedRebuild();
}

MarkerAttributes Legend::markerAttributes( uint dataset ) const
{
    if( d->markerAttributes.find( dataset ) != d->markerAttributes.end() )
        return d->markerAttributes[ dataset ];
    else if ( static_cast<uint>( d->modelMarkers.count() ) > dataset )
        return d->modelMarkers[ dataset ];
    return MarkerAttributes();
}

const QMap<uint, MarkerAttributes> Legend::markerAttributes() const
{
    return d->markerAttributes;
}


void Legend::setTextAttributes( const TextAttributes &a )
{
    if( d->textAttributes == a ) return;
    d->textAttributes = a;
    setNeedRebuild();
}

TextAttributes Legend::textAttributes() const
{
    return d->textAttributes;
}

void Legend::setTitleText( const QString& text )
{
    if( d->titleText == text ) return;
    d->titleText = text;
    setNeedRebuild();
}

QString Legend::titleText() const
{
    return d->titleText;
}

void Legend::setTitleTextAttributes( const TextAttributes &a )
{
    if( d->titleTextAttributes == a ) return;
    d->titleTextAttributes = a;
    setNeedRebuild();
}

TextAttributes Legend::titleTextAttributes() const
{
    return d->titleTextAttributes;
}

void Legend::forceRebuild()
{
#ifdef DEBUG_LEGEND_PAINT
    qDebug() << "entering Legend::forceRebuild()";
#endif
    //setSpacing(d->layout->spacing());
    buildLegend();
#ifdef DEBUG_LEGEND_PAINT
    qDebug() << "leaving Legend::forceRebuild()";
#endif
}

void Legend::setSpacing( uint space )
{
    if( d->spacing == space && d->layout->spacing() == static_cast<int>(space) ) return;
    d->spacing = space;
    d->layout->setSpacing( space );
    setNeedRebuild();
}

uint Legend::spacing() const
{
    return d->spacing;
}

void Legend::setDefaultColors()
{
    setColor(  0, Qt::red );
    setColor(  1, Qt::green );
    setColor(  2, Qt::blue );
    setColor(  3, Qt::cyan );
    setColor(  4, Qt::magenta );
    setColor(  5, Qt::yellow );
    setColor(  6, Qt::darkRed );
    setColor(  7, Qt::darkGreen );
    setColor(  8, Qt::darkBlue );
    setColor(  9, Qt::darkCyan );
    setColor( 10, Qt::darkMagenta );
    setColor( 11, Qt::darkYellow );
}

void Legend::setRainbowColors()
{
    setColor(  0, QColor(255,  0,196) );
    setColor(  1, QColor(255,  0, 96) );
    setColor(  2, QColor(255, 128,64) );
    setColor(  3, Qt::yellow );
    setColor(  4, Qt::green );
    setColor(  5, Qt::cyan );
    setColor(  6, QColor( 96, 96,255) );
    setColor(  7, QColor(160,  0,255) );
    for( int i = 8; i < 16; ++i )
        setColor( i, brush( i - 8 ).color().light() );
}

void Legend::setSubduedColors( bool ordered )
{
static const int NUM_SUBDUEDCOLORS = 18;
static const QColor SUBDUEDCOLORS[ NUM_SUBDUEDCOLORS ] = {
    QColor( 0xe0,0x7f,0x70 ),
    QColor( 0xe2,0xa5,0x6f ),
    QColor( 0xe0,0xc9,0x70 ),
    QColor( 0xd1,0xe0,0x70 ),
    QColor( 0xac,0xe0,0x70 ),
    QColor( 0x86,0xe0,0x70 ),
    QColor( 0x70,0xe0,0x7f ),
    QColor( 0x70,0xe0,0xa4 ),
    QColor( 0x70,0xe0,0xc9 ),
    QColor( 0x70,0xd1,0xe0 ),
    QColor( 0x70,0xac,0xe0 ),
    QColor( 0x70,0x86,0xe0 ),
    QColor( 0x7f,0x70,0xe0 ),
    QColor( 0xa4,0x70,0xe0 ),
    QColor( 0xc9,0x70,0xe0 ),
    QColor( 0xe0,0x70,0xd1 ),
    QColor( 0xe0,0x70,0xac ),
    QColor( 0xe0,0x70,0x86 ),
};
    if( ordered )
        for(int i=0; i<NUM_SUBDUEDCOLORS; ++i)
            setColor( i, SUBDUEDCOLORS[i] );
    else{
        setColor( 0, SUBDUEDCOLORS[ 0] );
        setColor( 1, SUBDUEDCOLORS[ 5] );
        setColor( 2, SUBDUEDCOLORS[10] );
        setColor( 3, SUBDUEDCOLORS[15] );
        setColor( 4, SUBDUEDCOLORS[ 2] );
        setColor( 5, SUBDUEDCOLORS[ 7] );
        setColor( 6, SUBDUEDCOLORS[12] );
        setColor( 7, SUBDUEDCOLORS[17] );
        setColor( 8, SUBDUEDCOLORS[ 4] );
        setColor( 9, SUBDUEDCOLORS[ 9] );
        setColor(10, SUBDUEDCOLORS[14] );
        setColor(11, SUBDUEDCOLORS[ 1] );
        setColor(12, SUBDUEDCOLORS[ 6] );
        setColor(13, SUBDUEDCOLORS[11] );
        setColor(14, SUBDUEDCOLORS[16] );
        setColor(15, SUBDUEDCOLORS[ 3] );
        setColor(16, SUBDUEDCOLORS[ 8] );
        setColor(17, SUBDUEDCOLORS[13] );
    }
}

void Legend::resizeEvent ( QResizeEvent * event )
{
#ifdef DEBUG_LEGEND_PAINT
    qDebug() << "Legend::resizeEvent() called";
#endif
    forceRebuild();
    sizeHint();
    QTimer::singleShot(0, this, SLOT(emitPositionChanged()));
}

void Legend::buildLegend()
{
    /*
    if( !d->needRebuild ) {
#ifdef DEBUG_LEGEND_PAINT
        qDebug() << "leaving Legend::buildLegend() with NO action (was already build)";
#endif
        // Note: We do *not* need to send positionChanged here,
        //       because we send it in the resizeEvent, so layouting
        //       is done at the right time.
        return;
    }
#ifdef DEBUG_LEGEND_PAINT
    qDebug() << "entering Legend::buildLegend() **********************************";
#endif
    d->needRebuild = false;
    */

    Q_FOREACH( QLayoutItem* layoutItem, d->layoutItems ) {
        d->layout->removeItem( layoutItem );
    }
    qDeleteAll( d->layoutItems );
    d->layoutItems.clear();

    if( orientation() == Qt::Vertical ) {
        d->layout->setColumnStretch( 4, 1 );
    } else {
        d->layout->setColumnStretch( 4, 0 );
    }

    d->modelLabels.clear();
    d->modelBrushes.clear();
    d->modelPens.clear();
    d->modelMarkers.clear();
    // retrieve the diagrams' settings for all non-hidden datasets
    for (int i = 0; i < d->observers.size(); ++i){
        const AbstractDiagram* diagram = d->observers.at(i)->diagram();
        if( diagram ){
            //qDebug() << "accessing" << diagram;
            const QStringList             diagramLabels(  diagram->datasetLabels()  );
            const QList<QBrush>           diagramBrushes( diagram->datasetBrushes() );
            const QList<QPen>             diagramPens(    diagram->datasetPens()    );
            const QList<MarkerAttributes> diagramMarkers( diagram->datasetMarkers() );
            for ( int dataset = 0; dataset < diagramLabels.count(); dataset++ ) {
                // only show the label if the diagrams is NOT having the dataset set to hidden
                if( ! diagram->isHidden( dataset ) ){
                    d->modelLabels  += diagramLabels[   dataset ];
                    d->modelBrushes += diagramBrushes[  dataset ];
                    d->modelPens    += diagramPens[     dataset ];
                    d->modelMarkers += diagramMarkers[  dataset ];
                }
            }
        }
    }

    Q_ASSERT( d->modelLabels.count() == d->modelBrushes.count() );

    // legend caption
    if( !titleText().isEmpty() && titleTextAttributes().isVisible() ) {
        // PENDING(kalle) Other properties!
        KDChart::TextLayoutItem* titleItem =
            new KDChart::TextLayoutItem( titleText(),
                titleTextAttributes(),
                referenceArea(),
                (orientation() == Qt::Vertical)
                ? KDChartEnums::MeasureOrientationMinimum
                : KDChartEnums::MeasureOrientationHorizontal,
                Qt::AlignCenter );
        titleItem->setParentWidget( this );

        d->layoutItems << titleItem;
        if( orientation() == Qt::Vertical )
            d->layout->addItem( titleItem, 0, 0, 1, 5, Qt::AlignCenter );
        else
            d->layout->addItem( titleItem, 0, 0, 1, d->modelLabels.count() ? (d->modelLabels.count()*4) : 1, Qt::AlignCenter );

        // The line between the title and the legend items, if any.
        if( showLines() && d->modelLabels.count() ) {
            KDChart::HorizontalLineLayoutItem* lineItem = new KDChart::HorizontalLineLayoutItem();
            d->layoutItems << lineItem;
            if( orientation() == Qt::Vertical )
                d->layout->addItem( lineItem, 1, 0, 1, 5, Qt::AlignCenter );
            else
                d->layout->addItem( lineItem, 1, 0, 1, d->modelLabels.count()*4, Qt::AlignCenter );
        }
    }

    const KDChartEnums::MeasureOrientation orient =
            (orientation() == Qt::Vertical)
            ? KDChartEnums::MeasureOrientationMinimum
            : KDChartEnums::MeasureOrientationHorizontal;
    const TextAttributes labelAttrs( textAttributes() );
    const qreal fontHeight = labelAttrs.calculatedFontSize( referenceArea(), orient );
    const LegendStyle style = legendStyle();
    //qDebug() << "fontHeight:" << fontHeight;

    const bool bShowMarkers = (style != LinesOnly);

    QSizeF maxMarkersSize(1.0, 1.0);
    QVector <MarkerAttributes> markerAttrs( d->modelLabels.count() );
    if( bShowMarkers ){
        for ( int dataset = 0; dataset < d->modelLabels.count(); ++dataset ) {
            markerAttrs[dataset] = markerAttributes( dataset );
            QSizeF siz;
            if( useAutomaticMarkerSize() ||
                ! markerAttrs[dataset].markerSize().isValid() )
            {
                siz = QSizeF(fontHeight, fontHeight);
                markerAttrs[dataset].setMarkerSize( siz );
            }else{
                siz = markerAttrs[dataset].markerSize();
            }
            maxMarkersSize =
                    QSizeF(qMax(maxMarkersSize.width(),  siz.width()),
                           qMax(maxMarkersSize.height(), siz.height()));
        }
    }

    // If we show a marker on a line, we paint it after 4 pixels
    // of the line have been painted. This allows to see the line style
    // at the right side of the marker without the line needing to
    // be too long.
    // (having the marker in the middle of the line would require longer lines)
    const int markerOffsOnLine = 8;

    int maxLineLength = 18;
    {
        bool hasComplexPenStyle = false;
        for ( int dataset = 0; dataset < d->modelLabels.count(); ++dataset ){
            const QPen pn = pen(dataset);
            const Qt::PenStyle ps = pn.style();
            if( ps != Qt::NoPen ){
                maxLineLength = qMax( pn.width() * 18, maxLineLength );
                if( ps != Qt::SolidLine )
                    hasComplexPenStyle = true;
            }
        }
        if( hasComplexPenStyle && bShowMarkers )
            maxLineLength =
                    maxLineLength + markerOffsOnLine +
                    static_cast<int>(maxMarkersSize.width());
    }

    for ( int dataset = 0; dataset < d->modelLabels.count(); ++dataset ) {
        KDChart::AbstractLayoutItem* markerLineItem = 0;
        switch( style ){
            case( MarkersOnly ):
                markerLineItem = new KDChart::MarkerLayoutItem(
                        diagram(),
                        markerAttrs[dataset],
                        brush( dataset ),
                        pen( dataset ),
                        Qt::AlignLeft );
                break;
            case( LinesOnly ):
                markerLineItem = new KDChart::LineLayoutItem(
                        diagram(),
                        maxLineLength,
                        pen( dataset ),
                        Qt::AlignCenter );
                break;
            case( MarkersAndLines ):
                markerLineItem = new KDChart::LineWithMarkerLayoutItem(
                        diagram(),
                        maxLineLength,
                        pen( dataset ),
                        markerOffsOnLine,
                        markerAttrs[dataset],
                        brush( dataset ),
                        pen( dataset ),
                        Qt::AlignCenter );
                break;
            default:
                Q_ASSERT( false ); // all styles need to be handled
        }
        if( markerLineItem ){
            d->layoutItems << markerLineItem;
            if( orientation() == Qt::Vertical )
                d->layout->addItem( markerLineItem,
                                    dataset*2+2, // first row is title, second is line
                                    1,
                                    1, 1, Qt::AlignCenter );
            else
                d->layout->addItem( markerLineItem,
                                    2, // all in row two
                                    dataset*4 );
        }

        // PENDING(kalle) Other properties!
        KDChart::TextLayoutItem* labelItem =
            new KDChart::TextLayoutItem( text( dataset ),
                labelAttrs,
                referenceArea(), orient,
                Qt::AlignLeft );
        labelItem->setParentWidget( this );

        d->layoutItems << labelItem;
        if( orientation() == Qt::Vertical )
            d->layout->addItem( labelItem,
                                dataset*2+2, // first row is title, second is line
                                3 );
        else
            d->layout->addItem( labelItem,
                                2, // all in row two
                                dataset*4+1 );

        // horizontal lines (only in vertical mode, and not after the last item)
        if( orientation() == Qt::Vertical && showLines() && dataset != d->modelLabels.count()-1 ) {
            KDChart::HorizontalLineLayoutItem* lineItem = new KDChart::HorizontalLineLayoutItem();
            d->layoutItems << lineItem;
            d->layout->addItem( lineItem,
                                dataset*2+1+2,
                                0,
                                1, 5, Qt::AlignCenter );
        }

        // vertical lines (only in horizontal mode, and not after the last item)
        if( orientation() == Qt::Horizontal && showLines() && dataset != d->modelLabels.count()-1 ) {
            KDChart::VerticalLineLayoutItem* lineItem = new KDChart::VerticalLineLayoutItem();
            d->layoutItems << lineItem;
            d->layout->addItem( lineItem,
                                2, // all in row two
                                style == MarkersAndLines ? dataset*4+3 : dataset*4+2 ,
                                1, 1, Qt::AlignCenter );
        }

        if( orientation() != Qt::Vertical ) { // Horizontal needs a spacer
            d->layout->addItem( new QSpacerItem( spacing(), 1 ),
                                2, // all in row two
                                dataset*4+3 );
        }
    }

    // vertical line (only in vertical mode)
    if( orientation() == Qt::Vertical && showLines() && d->modelLabels.count() ) {
        KDChart::VerticalLineLayoutItem* lineItem = new KDChart::VerticalLineLayoutItem();
        d->layoutItems << lineItem;
        d->layout->addItem( lineItem, 2, 2, d->modelLabels.count()*2, 1 );
    }

    // This line is absolutely necessary, otherwise: #2516.
    activateTheLayout();

    emit propertiesChanged();
    //emit positionChanged( this );
    //emitPositionChanged();
#ifdef DEBUG_LEGEND_PAINT
    qDebug() << "leaving Legend::buildLegend()";
#endif
}

