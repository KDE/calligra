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
#include "ChartLegendTool.h"

// KOffice
#include <KoCanvasBase.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoPointerEvent.h>

// ChartShape
#include "ChartLegendConfigWidget.h"
 
using namespace KChart;
 
ChartLegendTool::ChartLegendTool( KoCanvasBase *canvas )
    : KoTool( canvas ),
    m_currentShape( 0 )
{
}

ChartLegendTool::~ChartLegendTool()
{
}

void ChartLegendTool::paint( QPainter &painter, const KoViewConverter &converter)
{
}

void ChartLegendTool::mousePressEvent( KoPointerEvent *event )
{
    event->ignore();
}

void ChartLegendTool::mouseMoveEvent( KoPointerEvent *event )
{
    event->ignore();
}

void ChartLegendTool::mouseReleaseEvent( KoPointerEvent *event )
{
    event->ignore();
}

void ChartLegendTool::activate( bool )
{
    // Get the shape that the tool is working on. 
    // Let m_currentShape point to it.
    KoSelection *selection = m_canvas->shapeManager()->selection();
    foreach ( KoShape *shape, selection->selectedShapes() ) {
        m_currentShape = dynamic_cast<ChartShape*>( shape );
        if ( m_currentShape )
            break;
    }
    if ( !m_currentShape ) { // none found
        emit done();
        return;
    }

    //useCursor() // lets keep the forbidden cursor for now; as this tool doesn't really allow mouse interaction anyway

    updateActions();
}

void ChartLegendTool::deactivate()
{
    m_currentShape = 0;
}

void ChartLegendTool::updateActions()
{
}


QWidget *ChartLegendTool::createOptionWidget()
{
    ChartLegendConfigWidget *widget = new ChartLegendConfigWidget();

    connect( widget, SIGNAL( legendTitleChanged( QString& ) ),
       this, SLOT( setLegendTitle( QString& ) ) );

    return widget;
}

void ChartLegendTool::setLegendTitle( QString &title )
{
}