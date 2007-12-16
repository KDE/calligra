/* This file is part of the KDE project
 *
 * Copyright 2007 Johannes Simon <johannes.simon@gmail.com>
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
#include "ChartDatasetTool.h"

// KDE
#include <KDebug>

// KOffice
#include <KoCanvasBase.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoPointerEvent.h>

#include <KDChartChart>
#include <KDChartAbstractCoordinatePlane>
#include <KDChartAbstractDiagram>

// ChartShape
#include "ChartDatasetConfigWidget.h"
 
using namespace KChart;

class ChartDatasetTool::Private
{
public:
    Private();
    ~Private();
    
    ChartDatasetConfigWidget *widget;
    
    ChartShape  *shape;
    QModelIndex  selection;
    QBrush       selectionBrush;
    QPen         selectionPen;
};

ChartDatasetTool::Private::Private()
{
    shape = 0;
    widget = 0;
}

ChartDatasetTool::Private::~Private()
{
}

ChartDatasetTool::ChartDatasetTool( KoCanvasBase *canvas )
    : KoTool( canvas ),
    d( new Private )
{
}

ChartDatasetTool::~ChartDatasetTool()
{
}

void ChartDatasetTool::paint( QPainter &painter, const KoViewConverter &converter)
{
}

void ChartDatasetTool::mousePressEvent( KoPointerEvent *event )
{
    if (    !d->shape || !d->shape->chart() || ! d->shape->chart()->coordinatePlane()
         || !d->shape->chart()->coordinatePlane()->diagram() )
        return;
    QPointF point = event->point - d->shape->position();
    QModelIndex selection = d->shape->chart()->coordinatePlane()->diagram()->indexAt( point.toPoint() );
    // Note: the dataset will always stay column() due to the transformations being
    // done internally by the ChartProxyModel
    int dataset = selection.column();
    
    if ( d->selection.isValid() ) {
        d->shape->chart()->coordinatePlane()->diagram()->setPen( d->selection.column(), d->selectionPen );
        //d->shape->chart()->coordinatePlane()->diagram()->setBrush( d->selection, d->selectionBrush );
    }
    if ( selection.isValid() ) {
        d->selection = selection;
        updateWidget();
        
        QPen pen( Qt::DotLine );
        pen.setColor( Qt::darkGray );
        pen.setWidth( 1 );
        
        d->selectionBrush = d->shape->chart()->coordinatePlane()->diagram()->brush( selection );
        d->selectionPen   = d->shape->chart()->coordinatePlane()->diagram()->pen( dataset );
        
        d->shape->chart()->coordinatePlane()->diagram()->setPen( dataset, pen );
        //d->shape->chart()->coordinatePlane()->diagram()->setBrush( selection, QBrush( Qt::lightGray ) );
    }
        
    d->shape->update();
}

void ChartDatasetTool::mouseMoveEvent( KoPointerEvent *event )
{
    event->ignore();
}

void ChartDatasetTool::mouseReleaseEvent( KoPointerEvent *event )
{
    event->ignore();
}

void ChartDatasetTool::activate( bool )
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

void ChartDatasetTool::deactivate()
{
    d->shape = 0;
}

void ChartDatasetTool::updateActions()
{
}

void ChartDatasetTool::updateWidget()
{
    if ( !d->widget )
        return;
    int dataset;
    if ( d->selection.isValid() )
        dataset = d->selection.column();
    else
        dataset = -1;
    d->widget->selectDataset( dataset );
}

QWidget *ChartDatasetTool::createOptionWidget()
{
    d->widget = new ChartDatasetConfigWidget();
    d->widget->open( d->shape );
    connect( d->widget, SIGNAL( datasetColorChanged( int, const QColor& ) ),
             this,      SLOT( setDatasetColor( int, const QColor& ) ) );

    return d->widget;
}

void ChartDatasetTool::setDatasetColor( int dataset, const QColor& color )
{
    if (    !d->shape || !d->shape->chart() || ! d->shape->chart()->coordinatePlane()
         || !d->shape->chart()->coordinatePlane()->diagram() )
        return;
    kDebug() << color;
    kDebug() << dataset;
    d->shape->chart()->coordinatePlane()->diagram()->setBrush( dataset, QBrush( color ) );
    d->shape->repaint();
}
