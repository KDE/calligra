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

// Local
#include "ChartTool.h"
#include "ProxyModel.h"
#include "Surface.h"
#include "PlotArea.h"
#include "Axis.h"
#include "TextLabel.h"
#include "DataSet.h"
#include "Legend.h"
#include "KDChartConvertions.h"

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
    return;
    /*
    // Select dataset
    if (    !d->shape || !d->shape->kdChart() || ! d->shape->kdChart()->coordinatePlane()
         || !d->shape->kdChart()->coordinatePlane()->diagram() )
        return;
    QPointF point = event->point - d->shape->position();
    QModelIndex selection = d->shape->kdChart()->coordinatePlane()->diagram()->indexAt( point.toPoint() );
    // Note: the dataset will always stay column() due to the transformations being
    // done internally by the ChartProxyModel
    int dataset = selection.column();
    
    if ( d->datasetSelection.isValid() ) {
        d->shape->kdChart()->coordinatePlane()->diagram()->setPen( d->datasetSelection.column(), d->datasetSelectionPen );
        //d->shape->kdChart()->coordinatePlane()->diagram()->setBrush( d->datasetSelection, d->datasetSelectionBrush );
    }
    if ( selection.isValid() ) {
        d->datasetSelection = selection;
        
        QPen pen( Qt::DotLine );
        pen.setColor( Qt::darkGray );
        pen.setWidth( 1 );
        
        d->datasetSelectionBrush = d->shape->kdChart()->coordinatePlane()->diagram()->brush( selection );
        d->datasetSelectionPen   = d->shape->kdChart()->coordinatePlane()->diagram()->pen( dataset );
        
        d->shape->kdChart()->coordinatePlane()->diagram()->setPen( dataset, pen );
        //d->shape->kdChart()->coordinatePlane()->diagram()->setBrush( selection, QBrush( Qt::lightGray ) );
    }
    ((ChartConfigWidget*)optionWidget())->selectDataset( dataset );
        
    d->shape->update();
    */
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
    
    connect( widget, SIGNAL( dataSetChartTypeChanged( DataSet*, ChartType ) ),
             this,   SLOT( setDataSetChartType( DataSet*, ChartType ) ) );
    connect( widget, SIGNAL( dataSetChartSubTypeChanged( DataSet*, ChartSubtype ) ),
             this,   SLOT( setDataSetChartSubType( DataSet*, ChartSubtype ) ) );
    connect( widget, SIGNAL( datasetColorChanged( DataSet*, const QColor& ) ),
             this, SLOT( setDatasetColor( DataSet*, const QColor& ) ) );
    connect( widget, SIGNAL( datasetShowValuesChanged( DataSet*, bool ) ),
             this, SLOT( setDatasetShowValues( DataSet*, bool ) ) );
    connect( widget, SIGNAL( datasetShowLabelsChanged( DataSet*, bool ) ),
             this, SLOT( setDatasetShowLabels( DataSet*, bool ) ) );
    connect( widget, SIGNAL( gapBetweenBarsChanged( int ) ),
             this,   SLOT( setGapBetweenBars( int ) ) );
    connect( widget, SIGNAL( gapBetweenSetsChanged( int ) ),
             this,   SLOT( setGapBetweenSets( int ) ) );
    
    connect( widget, SIGNAL( showLegendChanged( bool ) ),
             this,   SLOT( setShowLegend( bool ) ));

    connect( widget, SIGNAL( chartTypeChanged( ChartType ) ),
             this,   SLOT( setChartType( ChartType ) ) );
    connect( widget, SIGNAL( chartSubTypeChanged( ChartSubtype ) ),
             this,   SLOT( setChartSubType( ChartSubtype ) ) );
    connect( widget, SIGNAL( threeDModeToggled( bool ) ),
             this,   SLOT( setThreeDMode( bool ) ) );
    connect( widget, SIGNAL( showTitleChanged( bool ) ),
             this,   SLOT( setShowTitle( bool ) ) );
    connect( widget, SIGNAL( showSubTitleChanged( bool ) ),
             this,   SLOT( setShowSubTitle( bool ) ) );
    connect( widget, SIGNAL( showFooterChanged( bool ) ),
             this,   SLOT( setShowFooter( bool ) ) );

    connect( widget, SIGNAL( axisAdded( AxisPosition, const QString& ) ),
             this,   SLOT( addAxis( AxisPosition, const QString& ) ) );
    connect( widget, SIGNAL( axisRemoved( Axis* ) ),
             this,   SLOT( removeAxis( Axis* ) ) );
    connect( widget, SIGNAL( axisTitleChanged( Axis*, const QString& ) ),
    		 this,   SLOT( setAxisTitle( Axis*, const QString& ) ) );
    connect( widget, SIGNAL( axisShowTitleChanged( Axis*, bool ) ),
             this,   SLOT( setAxisShowTitle( Axis*, bool ) ) );
    connect( widget, SIGNAL( axisShowGridLinesChanged( Axis*, bool ) ),
    		 this,   SLOT( setAxisShowGridLines( Axis*, bool ) ) );
    connect( widget, SIGNAL( axisUseLogarithmicScalingChanged( Axis*, bool ) ),
    		 this,   SLOT( setAxisUseLogarithmicScaling( Axis*, bool ) ) );
    connect( widget, SIGNAL( axisStepWidthChanged( Axis*, double ) ),
    		 this,   SLOT( setAxisStepWidth( Axis*, double ) ) );
    connect( widget, SIGNAL( axisSubStepWidthChanged( Axis*, double ) ),
    		 this,   SLOT( setAxisSubStepWidth( Axis*, double ) ) );

    connect( widget, SIGNAL( legendTitleChanged( const QString& ) ),
             this,   SLOT( setLegendTitle( const QString& ) ) );
    connect( widget, SIGNAL( legendFontChanged( const QFont& ) ),
             this,   SLOT( setLegendFont( const QFont& ) ) );
    connect( widget, SIGNAL( legendFontSizeChanged( int ) ),
             this,   SLOT( setLegendFontSize( int ) ) );

    connect( widget, SIGNAL( legendOrientationChanged( Qt::Orientation ) ),
             this,   SLOT( setLegendOrientation( Qt::Orientation ) ) );
    connect( widget, SIGNAL( legendAlignmentChanged( Qt::Alignment ) ),
             this,   SLOT( setLegendAlignment( Qt::Alignment ) ) );

    connect( widget, SIGNAL( legendFixedPositionChanged( LegendPosition ) ),
             this,   SLOT( setLegendFixedPosition( LegendPosition ) ) );
    
    connect( widget, SIGNAL( legendBackgroundColorChanged( const QColor& ) ) ,
             this,   SLOT( setLegendBackgroundColor( const QColor& ) ) );
    connect( widget, SIGNAL( legendFrameColorChanged( const QColor& ) ) ,
             this,   SLOT( setLegendFrameColor( const QColor& ) ) );
    connect( widget, SIGNAL( legendShowFrameChanged( bool ) ) ,
             this,   SLOT( setLegendShowFrame( bool ) ) );

    return widget;
}


void ChartTool::setChartType( ChartType type, ChartSubtype subtype )
{
    Q_ASSERT( d->shape );
    if ( d->shape != 0 ) {
        d->shape->setChartType( type );
        d->shape->setChartSubType( subtype );
    }
    if ( optionWidget() )
        optionWidget()->update();
}


void ChartTool::setChartSubType( ChartSubtype subtype )
{
    Q_ASSERT( d->shape );
    if ( d->shape != 0 )
        d->shape->setChartSubType( subtype );
}

void ChartTool::setDataSetChartType( DataSet *dataSet, ChartType type )
{
    Q_ASSERT( dataSet );
    if ( dataSet )
        dataSet->setChartType( type );
}

void ChartTool::setDataSetChartSubType( DataSet *dataSet, ChartSubtype subType )
{
    Q_ASSERT( dataSet );
    if ( dataSet )
        dataSet->setChartSubType( subType );
}

void ChartTool::setThreeDMode( bool threeD )
{
    Q_ASSERT( d->shape );
    if ( d->shape != 0 )
        d->shape->setThreeD( threeD );
}

void ChartTool::setShowTitle( bool show )
{
    Q_ASSERT( d->shape );
    if ( d->shape ) {
        d->shape->title()->setVisible( show );
        d->shape->update();
    }
}

void ChartTool::setShowSubTitle( bool show )
{
    Q_ASSERT( d->shape );
    if ( d->shape ) {
        d->shape->subTitle()->setVisible( show );
        d->shape->update();
    }
}

void ChartTool::setShowFooter( bool show )
{
    Q_ASSERT( d->shape );
    if ( d->shape ) {
        d->shape->footer()->setVisible( show );
        d->shape->update();
    }
}

void ChartTool::setDataDirection( Qt::Orientation direction )
{
    Q_ASSERT( d->shape );
    if ( d->shape != 0 )
        d->shape->proxyModel()->setDataDirection( direction );
}

void ChartTool::setFirstRowIsLabel( bool b )
{
    Q_ASSERT( d->shape );
    if ( d->shape != 0 )
        d->shape->proxyModel()->setFirstRowIsLabel( b );
}

void ChartTool::setFirstColumnIsLabel( bool b )
{
    Q_ASSERT( d->shape );
    if ( d->shape != 0 )
        d->shape->proxyModel()->setFirstColumnIsLabel( b );
}


void ChartTool::setLegendTitle( const QString &title )
{
    Q_ASSERT( d->shape );
    d->shape->legend()->setTitle( title );
}

void ChartTool::setLegendFont( const QFont &font )
{
    Q_ASSERT( d->shape );
    // There only is a general font, for the legend items and the legend title
    d->shape->legend()->setFont( font );
}
void ChartTool::setLegendFontSize( int size )
{
    Q_ASSERT( d->shape );
    d->shape->legend()->setFontSize( size );
}

void ChartTool::setLegendOrientation( Qt::Orientation orientation )
{
    Q_ASSERT( d->shape );
    d->shape->legend()->setExpansion( QtOrientationToLegendExpansion( orientation ) );
}

void ChartTool::setLegendAlignment( Qt::Alignment alignment )
{
    Q_ASSERT( d->shape );
    d->shape->legend()->setAlignment( alignment );
}

void ChartTool::setLegendFixedPosition( LegendPosition position )
{
    Q_ASSERT( d->shape );
    d->shape->legend()->setLegendPosition( position );
    ( ( ChartConfigWidget* ) optionWidget() )->updateFixedPosition( position );
}

void ChartTool::setLegendBackgroundColor( const QColor& color )
{
    Q_ASSERT( d->shape );
    d->shape->legend()->setBackgroundColor( color );
}

void ChartTool::setLegendFrameColor( const QColor& color )
{
    Q_ASSERT( d->shape );
    d->shape->legend()->setFrameColor( color );
}

void ChartTool::setLegendShowFrame( bool show )
{
    Q_ASSERT( d->shape );
    d->shape->legend()->setShowFrame( show );
}


void ChartTool::setDatasetColor( DataSet *dataSet, const QColor& color )
{
    if ( !dataSet )
        return;
    dataSet->setColor( color );
}

void ChartTool::setDatasetShowValues( DataSet *dataSet, bool b )
{
    if ( !dataSet )
        return;
    dataSet->setShowValues( b );
}

void ChartTool::setDatasetShowLabels( DataSet *dataSet, bool b )
{
    if ( !dataSet )
        return;
    dataSet->setShowLabels( b );
}

void ChartTool::addAxis( AxisPosition position, const QString& title ) {
    Q_ASSERT( d->shape );
    Axis *axis = new Axis( d->shape->plotArea() );
    axis->setPosition( position );
    axis->setTitleText( title );
    d->shape->plotArea()->addAxis( axis );
}

void ChartTool::removeAxis( Axis *axis )
{
    Q_ASSERT( d->shape );
    d->shape->plotArea()->removeAxis( axis );
}

void ChartTool::setAxisTitle( Axis *axis, const QString& title )
{
	axis->setTitleText( title );
}

void ChartTool::setAxisShowTitle( Axis *axis, bool show )
{
    Q_ASSERT( d->shape );
    axis->title()->setVisible( show );
    d->shape->update();
}

void ChartTool::setAxisShowGridLines( Axis *axis, bool b )
{
	axis->setShowGrid( b );
}

void ChartTool::setAxisUseLogarithmicScaling( Axis *axis, bool b )
{
    axis->setScalingLogarithmic( b );
}

void ChartTool::setAxisStepWidth( Axis *axis, double width )
{
    axis->setMajorInterval( width );
}

void ChartTool::setAxisSubStepWidth( Axis *axis, double width )
{
    axis->setMinorInterval( width );
}


void ChartTool::setGapBetweenBars( int percent )
{
    Q_ASSERT( d->shape );
    d->shape->plotArea()->setGapBetweenBars( percent );
}

void ChartTool::setGapBetweenSets( int percent )
{
    Q_ASSERT( d->shape );
    d->shape->plotArea()->setGapBetweenSets( percent );
}

void ChartTool::setShowLegend( bool b )
{
    Q_ASSERT( d->shape );
    d->shape->legend()->setVisible( b );
    d->shape->legend()->update();
}

#include "ChartTool.moc"
