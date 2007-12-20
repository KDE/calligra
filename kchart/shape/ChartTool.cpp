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
#include <KDChartAbstractCoordinatePlane>
#include <KDChartPosition>

using namespace KChart;


class ChartTool::Private
{
public:
    Private();
    ~Private();

    ChartShape  *shape;
    QModelIndex  selection;
    QBrush       selectionBrush;
    QPen         selectionPen;

    ChartShape  *currentShape;
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
    event->ignore();
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
    // Let d->currentShape point to it.
    KoSelection *selection = m_canvas->shapeManager()->selection();
    foreach ( KoShape *shape, selection->selectedShapes() ) {
        d->currentShape = dynamic_cast<ChartShape*>( shape );
        if ( d->currentShape )
            break;
    }
    if ( !d->currentShape ) { // none found
        emit done();
        return;
    }

    useCursor( Qt::ArrowCursor, true );

    updateActions();
}

void ChartTool::deactivate()
{
    d->currentShape = 0;
}

void ChartTool::updateActions()
{
#if 0 // Taken from DivineProportion
    switch(d->currentShape->orientation()) {
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
    widget->open( d->currentShape );

    connect( widget, SIGNAL( dataDirectionChanged( Qt::Orientation ) ),
             this,    SLOT( setDataDirection( Qt::Orientation ) ) );
    connect( widget, SIGNAL( firstRowIsLabelChanged( bool ) ),
             this,    SLOT( setFirstRowIsLabel( bool ) ) );
    connect( widget, SIGNAL( firstColumnIsLabelChanged( bool ) ),
             this,    SLOT( setFirstColumnIsLabel( bool ) ) );

    connect( widget, SIGNAL( chartTypeChange( KChart::OdfChartType ) ),
	     this,    SLOT( setChartType( KChart::OdfChartType ) ) );
    connect( widget, SIGNAL( chartSubtypeChange( KChart::OdfChartSubtype ) ),
             this,    SLOT( setChartSubtype( KChart::OdfChartSubtype ) ) );
    connect( widget, SIGNAL( threeDModeToggled( bool ) ),
             this,    SLOT( setThreeDMode( bool ) ) );

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


void ChartTool::setChartType( OdfChartType type )
{
    if ( d->currentShape != 0 )
        d->currentShape->setChartType( type );
}


void ChartTool::setChartSubtype( OdfChartSubtype subtype )
{
    if ( d->currentShape != 0 )
        d->currentShape->setChartSubtype( subtype );
}

void ChartTool::setThreeDMode( bool threeD )
{
    if ( d->currentShape != 0 )
        d->currentShape->setThreeDMode( threeD );
}

void ChartTool::setDataDirection( Qt::Orientation direction )
{
    if ( d->currentShape != 0 )
        d->currentShape->setDataDirection( direction );
}

void ChartTool::setFirstRowIsLabel( bool b )
{
    if ( d->currentShape != 0 )
        d->currentShape->setFirstRowIsLabel( b );
}

void ChartTool::setFirstColumnIsLabel( bool b )
{
    if ( d->currentShape != 0 )
        d->currentShape->setFirstColumnIsLabel( b );
}


void ChartTool::setLegendTitle( const QString &title )
{
    d->currentShape->setLegendTitle( title );
}

// Deprecated method
void ChartTool::setLegendTitleFont( const QFont &font )
{
    d->currentShape->setLegendTitleFont( font );
}

void ChartTool::setLegendFont( const QFont &font )
{
    // There only is a general font, for the legend items and the legend title
    d->currentShape->setLegendFont( font );
    d->currentShape->setLegendTitleFont( font );
}

void ChartTool::setLegendSpacing( int spacing )
{
    d->currentShape->setLegendSpacing( spacing );
}

void ChartTool::setLegendFontSize( int size )
{
    d->currentShape->setLegendFontSize( size );
}

void ChartTool::setLegendShowLines( bool b )
{
    d->currentShape->setLegendShowLines( b );
}

void ChartTool::setLegendOrientation( Qt::Orientation orientation )
{
    d->currentShape->setLegendOrientation( orientation );
}

void ChartTool::setLegendAlignment( Qt::Alignment alignment )
{
    d->currentShape->setLegendAlignment( alignment );
}

void ChartTool::setLegendFixedPosition( KDChart::Position position )
{
    d->currentShape->setLegendFixedPosition( position );
    ( ( ChartConfigWidget* ) optionWidget() )->updateFixedPosition( position );
}

void ChartTool::setLegendBackgroundColor( const QColor& color )
{
    d->currentShape->setLegendBackgroundColor( color );
}

void ChartTool::setLegendFrameColor( const QColor& color )
{
    d->currentShape->setLegendFrameColor( color );
}

void ChartTool::setLegendShowFrame( bool show )
{
    d->currentShape->setLegendShowFrame( show );
}


void ChartTool::setDatasetColor( int dataset, const QColor& color )
{
    if (    !d->shape || !d->shape->chart() || ! d->shape->chart()->coordinatePlane()
         || !d->shape->chart()->coordinatePlane()->diagram() )
        return;
    d->shape->chart()->coordinatePlane()->diagram()->setBrush( dataset, QBrush( color ) );
    d->shape->repaint();
}

#include "ChartTool.moc"
