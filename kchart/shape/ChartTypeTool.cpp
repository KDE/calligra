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


#include "ChartTypeTool.h"

// Qt
#include <QAction>
#include <QGridLayout>
#include <QToolButton>
#include <QCheckBox>
#include <QTabWidget>

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
#include "ChartDataConfigWidget.h"
#include "ChartTypeConfigWidget.h"


using namespace KChart;


ChartTypeTool::ChartTypeTool( KoCanvasBase *canvas )
    : KoTool( canvas ),
      m_currentShape( 0 )
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

ChartTypeTool::~ChartTypeTool()
{
}


void ChartTypeTool::paint( QPainter &painter, const KoViewConverter &converter)
{
}

void ChartTypeTool::mousePressEvent( KoPointerEvent *event )
{
    event->ignore();
}

void ChartTypeTool::mouseMoveEvent( KoPointerEvent *event )
{
    event->ignore();
}

void ChartTypeTool::mouseReleaseEvent( KoPointerEvent *event )
{
    event->ignore();
}

void ChartTypeTool::activate( bool )
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

void ChartTypeTool::deactivate()
{
    m_currentShape = 0;
}

void ChartTypeTool::updateActions()
{
#if 0 // Taken from DivineProportion
    switch(m_currentShape->orientation()) {
        case ChartTypeShape::BottomRight: m_bottomRightOrientation->setChecked(true); break;
        case ChartTypeShape::BottomLeft: m_bottomLeftOrientation->setChecked(true); break;
        case ChartTypeShape::TopRight: m_topRightOrientation->setChecked(true); break;
        case ChartTypeShape::TopLeft: m_topLeftOrientation->setChecked(true); break;
    }
#endif
}


QWidget *ChartTypeTool::createOptionWidget()
{
    QTabWidget  *tabWidget = new QTabWidget();
    
    ChartDataConfigWidget  *widget1 = new ChartDataConfigWidget();
    widget1->open( m_currentShape );
    tabWidget->addTab( widget1, i18n( "Data" ) );

    ChartTypeConfigWidget  *widget2 = new ChartTypeConfigWidget();
    widget2->open( m_currentShape );
    tabWidget->addTab( widget2, i18n( "Type" ) );

    connect( widget1, SIGNAL( dataDirectionChanged( Qt::Orientation ) ),
             this,    SLOT( setDataDirection( Qt::Orientation ) ) );
    connect( widget1, SIGNAL( firstRowIsLabelChanged( bool ) ),
             this,    SLOT( setFirstRowIsLabel( bool ) ) );
    connect( widget1, SIGNAL( firstColumnIsLabelChanged( bool ) ),
             this,    SLOT( setFirstColumnIsLabel( bool ) ) );

    connect( widget2, SIGNAL( chartTypeChange( KChart::OdfChartType ) ),
	     this,    SLOT( setChartType( KChart::OdfChartType ) ) );
    connect( widget2, SIGNAL( chartSubtypeChange( KChart::OdfChartSubtype ) ),
             this,    SLOT( setChartSubtype( KChart::OdfChartSubtype ) ) );
    connect( widget2, SIGNAL( threeDModeToggled( bool ) ),
             this,    SLOT( setThreeDMode( bool ) ) );

    return tabWidget;
}


void ChartTypeTool::setChartType( OdfChartType type )
{
    if ( m_currentShape != 0 )
        m_currentShape->setChartType( type );
}


void ChartTypeTool::setChartSubtype( OdfChartSubtype subtype )
{
    if ( m_currentShape != 0 )
        m_currentShape->setChartSubtype( subtype );
}

void ChartTypeTool::setThreeDMode( bool threeD )
{
    if ( m_currentShape != 0 )
        m_currentShape->setThreeDMode( threeD );
}

void ChartTypeTool::setDataDirection( Qt::Orientation direction )
{
    if ( m_currentShape != 0 )
        m_currentShape->setDataDirection( direction );
}

void ChartTypeTool::setFirstRowIsLabel( bool b )
{
    if ( m_currentShape != 0 )
        m_currentShape->setFirstRowIsLabel( b );
}

void ChartTypeTool::setFirstColumnIsLabel( bool b )
{
    if ( m_currentShape != 0 )
        m_currentShape->setFirstColumnIsLabel( b );
}

#include "ChartTypeTool.moc"
