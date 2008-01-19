/* This file is part of the KDE project
 * Copyright (C) 2007      Inge Wallin <inge@lysator.liu.se>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */


#include "ChartTool.h"

// Qt
#include <QAction>
#include <QGridLayout>
#include <QToolButton>
#include <QCheckBox>
#include <QTabWidget>
#include <QPen>
#include <QBrush>

// KDE
#include <KLocale>
#include <KIcon>
#include <KDebug>

// KOffice
#include <KoCanvasBase.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoPointerEvent.h>

// ChartShape
#include "ChartConfigWidget.h"

// KDChart
#include <KDChartChart>
#include <KDChartCartesianAxis>
#include <KDChartGridAttributes>
#include <KDChartAbstractCartesianDiagram>
#include <KDChartCartesianCoordinatePlane>
#include <KDChartPosition>

using namespace KChart;


class ChartTool::Private
{
public:
    Private();
    ~Private();

    ChartShape  *shape;
    QModelIndex  datasetSelection;
    QPen         datasetSelectionPen;
    QBrush       datasetSelectionBrush;
};

ChartTool::Private::Private()
{
}

ChartTool::Private::~Private()
{
}

ChartTool::ChartTool( KoCanvasBase *canvas )
    : KoTool( canvas ),
      d( new Private() )
{
    // Create QActions here.
#if 0
    QActionGroup *group = new QActionGroup(this);
    m_foo  = new QAction(KIcon("this-action"), i18n("Do something"), this);
    m_foo->setCheckable(true);
    group->addAction(m_foo);
    connect( m_foo, SIGNAL(toggled(bool)), this, SLOT(catchFoo(bool)) );

    m_bar  = new QAction(KIcon("that-action"), i18n("Do something else"), this);
    m_bar->setCheckable(true);
    group->addAction(m_bar);
    connect( m_foo, SIGNAL(toggled(bool)), this, SLOT(catchBar(bool)) );

#endif
}

ChartTool::~ChartTool()
{
}


void ChartTool::paint( QPainter &painter, const KoViewConverter &converter)
{
}

void ChartTool::mousePressEvent( KoPointerEvent *event )
{
    // Select dataset
    if (    !d->shape || !d->shape->chart() || ! d->shape->chart()->coordinatePlane()
         || !d->shape->chart()->coordinatePlane()->diagram() )
        return;
    QPointF point = event->point - d->shape->position();
    QModelIndex selection = d->shape->chart()->coordinatePlane()->diagram()->indexAt( point.toPoint() );
    // Note: the dataset will always stay column() due to the transformations being
    // done internally by the ChartProxyModel
    int dataset = selection.column();
    
    if ( d->datasetSelection.isValid() ) {
        d->shape->chart()->coordinatePlane()->diagram()->setPen( d->datasetSelection.column(), d->datasetSelectionPen );
        //d->shape->chart()->coordinatePlane()->diagram()->setBrush( d->datasetSelection, d->datasetSelectionBrush );
    }
    if ( selection.isValid() ) {
        d->datasetSelection = selection;
        
        QPen pen( Qt::DotLine );
        pen.setColor( Qt::darkGray );
        pen.setWidth( 1 );
        
        d->datasetSelectionBrush = d->shape->chart()->coordinatePlane()->diagram()->brush( selection );
        d->datasetSelectionPen   = d->shape->chart()->coordinatePlane()->diagram()->pen( dataset );
        
        d->shape->chart()->coordinatePlane()->diagram()->setPen( dataset, pen );
        //d->shape->chart()->coordinatePlane()->diagram()->setBrush( selection, QBrush( Qt::lightGray ) );
    }
    ((ChartConfigWidget*)optionWidget())->selectDataset( dataset );
        
    d->shape->update();
}

void ChartTool::mouseMoveEvent( KoPointerEvent *event )
{
    event->ignore();
}

void ChartTool::mouseReleaseEvent( KoPointerEvent *event )
{
    event->ignore();
}

void ChartTool::activate( bool )
{
    // Get the shape that the tool is working on. 
    // Let d->shape point to it.
    KoSelection *selection = m_canvas->shapeManager()->selection();
    foreach ( KoShape *shape, selection->selectedShapes() ) {
        d->shape = dynamic_cast<ChartShape*>( shape );
        if ( d->shape )
            break;
    }
    if ( !d->shape ) { // none found
        emit done();
        return;
    }

    useCursor( Qt::ArrowCursor, true );

    updateActions();
}

void ChartTool::deactivate()
{
    d->shape = 0;
}

void ChartTool::updateActions()
{
#if 0 // Taken from DivineProportion
    switch(d->shape->orientation()) {
        case ChartTypeShape::BottomRight: m_bottomRightOrientation->setChecked(true); break;
        case ChartTypeShape::BottomLeft: m_bottomLeftOrientation->setChecked(true); break;
        case ChartTypeShape::TopRight: m_topRightOrientation->setChecked(true); break;
        case ChartTypeShape::TopLeft: m_topLeftOrientation->setChecked(true); break;
    }
#endif
}


QWidget *ChartTool::createOptionWidget()
{
    ChartConfigWidget  *widget = new ChartConfigWidget();
    widget->open( d->shape );

    connect( widget, SIGNAL( dataDirectionChanged( Qt::Orientation ) ),
             this,    SLOT( setDataDirection( Qt::Orientation ) ) );
    connect( widget, SIGNAL( firstRowIsLabelChanged( bool ) ),
             this,    SLOT( setFirstRowIsLabel( bool ) ) );
    connect( widget, SIGNAL( firstColumnIsLabelChanged( bool ) ),
             this,    SLOT( setFirstColumnIsLabel( bool ) ) );
    
    connect( widget, SIGNAL( datasetColorChanged( int, const QColor& ) ),
             this, SLOT( setDatasetColor( int, const QColor& ) ) );
    connect( widget, SIGNAL( gapBetweenBarsChanged( int ) ),
             this,   SLOT( setGapBetweenBars( int ) ) );
    connect( widget, SIGNAL( gapBetweenSetsChanged( int ) ),
             this,   SLOT( setGapBetweenSets( int ) ) );
    
    connect( widget, SIGNAL( showLegendChanged( bool ) ),
             this,   SLOT( setShowLegend( bool ) ));

    connect( widget, SIGNAL( chartTypeChanged( OdfChartType, OdfChartSubtype ) ),
             this,    SLOT( setChartType( OdfChartType, OdfChartSubtype ) ) );
    connect( widget, SIGNAL( chartSubtypeChanged( OdfChartSubtype ) ),
             this,    SLOT( setChartSubtype( OdfChartSubtype ) ) );
    connect( widget, SIGNAL( threeDModeToggled( bool ) ),
             this,    SLOT( setThreeDMode( bool ) ) );

    connect( widget, SIGNAL( axisAdded( AxisPosition, const QString& ) ),
             this,   SLOT( addAxis( AxisPosition, const QString& ) ) );
    connect( widget, SIGNAL( axisRemoved( KDChart::CartesianAxis* ) ),
             this,   SLOT( removeAxis( KDChart::CartesianAxis* ) ) );
    connect( widget, SIGNAL( axisTitleChanged( KDChart::CartesianAxis*, const QString& ) ),
    		 this,   SLOT( setAxisTitle( KDChart::CartesianAxis*, const QString& ) ) );
    connect( widget, SIGNAL( axisShowGridLinesChanged( KDChart::CartesianAxis*, bool ) ),
    		 this,   SLOT( setAxisShowGridLines( KDChart::CartesianAxis*, bool ) ) );
    connect( widget, SIGNAL( axisUseLogarithmicScalingChanged( KDChart::CartesianAxis*, bool ) ),
    		 this,   SLOT( setAxisUseLogarithmicScaling( KDChart::CartesianAxis*, bool ) ) );
    connect( widget, SIGNAL( axisStepWidthChanged( KDChart::CartesianAxis*, double ) ),
    		 this,   SLOT( setAxisStepWidth( KDChart::CartesianAxis*, double ) ) );
    connect( widget, SIGNAL( axisSubStepWidthChanged( KDChart::CartesianAxis*, double ) ),
    		 this,   SLOT( setAxisSubStepWidth( KDChart::CartesianAxis*, double ) ) );

    connect( widget, SIGNAL( legendTitleChanged( const QString& ) ),
             this,   SLOT( setLegendTitle( const QString& ) ) );
    connect( widget, SIGNAL( legendTitleFontChanged( const QFont& ) ),
             this,   SLOT( setLegendTitleFont( const QFont& ) ) );
    connect( widget, SIGNAL( legendFontChanged( const QFont& ) ),
             this,   SLOT( setLegendFont( const QFont& ) ) );
    connect( widget, SIGNAL( legendSpacingChanged( int ) ),
             this,   SLOT( setLegendSpacing( int ) ) );
    connect( widget, SIGNAL( legendFontSizeChanged( int ) ),
             this,   SLOT( setLegendFontSize( int ) ) );
    connect( widget, SIGNAL( legendShowLinesToggled( bool ) ),
             this,   SLOT( setLegendShowLines( bool ) ) );

    connect( widget, SIGNAL( legendOrientationChanged( Qt::Orientation ) ),
             this,   SLOT( setLegendOrientation( Qt::Orientation ) ) );
    connect( widget, SIGNAL( legendAlignmentChanged( Qt::Alignment ) ),
             this,   SLOT( setLegendAlignment( Qt::Alignment ) ) );

    connect( widget, SIGNAL( legendFixedPositionChanged( KDChart::Position ) ),
             this,   SLOT( setLegendFixedPosition( KDChart::Position ) ) );
    
    connect( widget, SIGNAL( legendBackgroundColorChanged( const QColor& ) ) ,
             this,   SLOT( setLegendBackgroundColor( const QColor& ) ) );
    connect( widget, SIGNAL( legendFrameColorChanged( const QColor& ) ) ,
             this,   SLOT( setLegendFrameColor( const QColor& ) ) );
    connect( widget, SIGNAL( legendShowFrameChanged( bool ) ) ,
             this,   SLOT( setLegendShowFrame( bool ) ) );

    return widget;
}


void ChartTool::setChartType( OdfChartType type, OdfChartSubtype subtype )
{
    Q_ASSERT( d->shape );
    if ( d->shape != 0 )
        d->shape->setChartType( type, subtype );
    if ( optionWidget() )
        optionWidget()->update();
}


void ChartTool::setChartSubtype( OdfChartSubtype subtype )
{
    Q_ASSERT( d->shape );
    if ( d->shape != 0 )
        d->shape->setChartSubtype( subtype );
}

void ChartTool::setThreeDMode( bool threeD )
{
    Q_ASSERT( d->shape );
    if ( d->shape != 0 )
        d->shape->setThreeDMode( threeD );
}

void ChartTool::setDataDirection( Qt::Orientation direction )
{
    Q_ASSERT( d->shape );
    if ( d->shape != 0 )
        d->shape->setDataDirection( direction );
}

void ChartTool::setFirstRowIsLabel( bool b )
{
    Q_ASSERT( d->shape );
    if ( d->shape != 0 )
        d->shape->setFirstRowIsLabel( b );
}

void ChartTool::setFirstColumnIsLabel( bool b )
{
    Q_ASSERT( d->shape );
    if ( d->shape != 0 )
        d->shape->setFirstColumnIsLabel( b );
}


void ChartTool::setLegendTitle( const QString &title )
{
    Q_ASSERT( d->shape );
    d->shape->setLegendTitle( title );
}

// Deprecated method
void ChartTool::setLegendTitleFont( const QFont &font )
{
    Q_ASSERT( d->shape );
    d->shape->setLegendTitleFont( font );
}

void ChartTool::setLegendFont( const QFont &font )
{
    Q_ASSERT( d->shape );
    // There only is a general font, for the legend items and the legend title
    d->shape->setLegendFont( font );
    d->shape->setLegendTitleFont( font );
}

void ChartTool::setLegendSpacing( int spacing )
{
    Q_ASSERT( d->shape );
    d->shape->setLegendSpacing( spacing );
}

void ChartTool::setLegendFontSize( int size )
{
    Q_ASSERT( d->shape );
    d->shape->setLegendFontSize( size );
}

void ChartTool::setLegendShowLines( bool b )
{
    Q_ASSERT( d->shape );
    d->shape->setLegendShowLines( b );
}

void ChartTool::setLegendOrientation( Qt::Orientation orientation )
{
    Q_ASSERT( d->shape );
    d->shape->setLegendOrientation( orientation );
}

void ChartTool::setLegendAlignment( Qt::Alignment alignment )
{
    Q_ASSERT( d->shape );
    d->shape->setLegendAlignment( alignment );
}

void ChartTool::setLegendFixedPosition( KDChart::Position position )
{
    Q_ASSERT( d->shape );
    d->shape->setLegendFixedPosition( position );
    ( ( ChartConfigWidget* ) optionWidget() )->updateFixedPosition( position );
}

void ChartTool::setLegendBackgroundColor( const QColor& color )
{
    Q_ASSERT( d->shape );
    d->shape->setLegendBackgroundColor( color );
}

void ChartTool::setLegendFrameColor( const QColor& color )
{
    Q_ASSERT( d->shape );
    d->shape->setLegendFrameColor( color );
}

void ChartTool::setLegendShowFrame( bool show )
{
    Q_ASSERT( d->shape );
    d->shape->setLegendShowFrame( show );
}


void ChartTool::setDatasetColor( int dataset, const QColor& color )
{
    if (    !d->shape || !d->shape->diagram() )
        return;
    d->shape->diagram()->setBrush( dataset, QBrush( color ) );
    d->shape->update();
}


void ChartTool::addAxis( AxisPosition position, const QString& title ) {
    Q_ASSERT( d->shape );
    d->shape->addAxis( position, title );
}

void ChartTool::removeAxis( KDChart::CartesianAxis *axis )
{
    Q_ASSERT( d->shape );
    d->shape->removeAxis( axis );
}

void ChartTool::setAxisTitle( KDChart::CartesianAxis *axis, const QString& title )
{
	Q_ASSERT( d->shape );
	d->shape->setAxisTitle( axis, title );
}

void ChartTool::setAxisShowGridLines( KDChart::CartesianAxis *axis, bool b )
{
	Q_ASSERT( d->shape );
	d->shape->setAxisShowGridLines( axis, b );
}

void ChartTool::setAxisUseLogarithmicScaling( KDChart::CartesianAxis *axis, bool b )
{
    Q_ASSERT( d->shape );
    d->shape->setAxisUseLogarithmicScaling( axis, b );
}

void ChartTool::setAxisStepWidth( KDChart::CartesianAxis *axis, double width )
{
    Q_ASSERT( d->shape );
    d->shape->setAxisStepWidth( axis, width );
}

void ChartTool::setAxisSubStepWidth( KDChart::CartesianAxis *axis, double width )
{
    Q_ASSERT( d->shape );
    d->shape->setAxisSubStepWidth( axis, width );
}


void ChartTool::setGapBetweenBars( int percent )
{
    Q_ASSERT( d->shape );
    d->shape->setGapBetweenBars( percent );
}

void ChartTool::setGapBetweenSets( int percent )
{
    Q_ASSERT( d->shape );
    d->shape->setGapBetweenSets( percent );
}

void ChartTool::setShowLegend( bool b )
{
    Q_ASSERT( d->shape );
    d->shape->setShowLegend( b );
}

#include "ChartTool.moc"
