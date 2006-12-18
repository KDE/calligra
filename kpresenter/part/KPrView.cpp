/* This file is part of the KDE project
   Copyright (C) 2006 Thorsten Zachmann <zachmann@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#include "KPrView.h"

#include <QGridLayout>

#include <KoCanvasController.h>
#include <KoToolManager.h>
#include <KoZoomHandler.h>
#include <KoToolBoxFactory.h>
#include <KoShapeSelectorFactory.h>
#include <KoShapeController.h>
#include <KoShapeManager.h>

#include <KPrCanvas.h>
#include <KPrDocument.h>
#include <KPrPage.h>

#include <klocale.h>
#include <ktoggleaction.h>

KPrView::KPrView( KPrDocument *document, QWidget *parent )
: KoView( document, parent )
, m_doc( document )
{
    m_activePage = 0;
    initGUI();
    initActions();
    if(m_doc->pageCount() > 0)
        setActivePage(m_doc->pageByIndex(0));
}

KPrView::~KPrView()
{
    KoToolManager::instance()->removeCanvasController( m_canvasController );
}


KPrPage* KPrView::activePage() const
{
    return m_activePage;
}

void KPrView::updateReadWrite( bool readwrite )
{
}

void KPrView::initGUI()
{
    QGridLayout * gridLayout = new QGridLayout( this );
    gridLayout->setMargin( 0 );
    gridLayout->setSpacing( 0 );
    setLayout( gridLayout );

    m_canvas = new KPrCanvas( this, m_doc );
    m_canvasController = new KoCanvasController( this );
    m_canvasController->setCanvas( m_canvas );
    KoToolManager::instance()->addControllers( m_canvasController );

    //Ruler
    m_horizontalRuler = new KoRuler(this, Qt::Horizontal, viewConverter());
    m_horizontalRuler->setShowMousePosition(true);
    m_horizontalRuler->setUnit(m_doc->unit());
    m_verticalRuler = new KoRuler(this, Qt::Vertical, viewConverter());
    m_verticalRuler->setUnit(m_doc->unit());
    m_verticalRuler->setShowMousePosition(true);

    connect(m_doc, SIGNAL(unitChanged(KoUnit)), m_horizontalRuler, SLOT(setUnit(KoUnit)));
    connect(m_doc, SIGNAL(unitChanged(KoUnit)), m_verticalRuler, SLOT(setUnit(KoUnit)));

    gridLayout->addWidget(m_horizontalRuler, 0, 1);
    gridLayout->addWidget(m_verticalRuler, 1, 0);
    gridLayout->addWidget( m_canvasController, 1, 1 );

    connect(m_canvasController, SIGNAL(canvasOffsetXChanged(int)),
            m_horizontalRuler, SLOT(setOffset(int)));
    connect(m_canvasController, SIGNAL(canvasOffsetYChanged(int)),
            m_verticalRuler, SLOT(setOffset(int)));

    KoToolBoxFactory toolBoxFactory( "Tools" );
    createDockWidget( &toolBoxFactory );
    KoShapeSelectorFactory shapeSelectorFactory;
    createDockWidget( &shapeSelectorFactory );
    show();
}

void KPrView::initActions()
{
    if ( !m_doc->isReadWrite() )
       setXMLFile( "kpresenter_readonly.rc" );
    else
       setXMLFile( "kpresenter.rc" );

    m_actionViewShowGrid = new KToggleAction( i18n( "Show &Grid" ), KShortcut(),
                                            this, SLOT( viewGrid() ),
                                            actionCollection(), "view_grid" );
    m_actionViewShowGrid->setCheckedState(KGuiItem(i18n("Hide &Grid")));

    m_actionViewSnapToGrid= new KToggleAction( i18n( "Snap to Grid" ), KShortcut(),
                                             this, SLOT(viewSnapToGrid() ),
                                             actionCollection(), "view_snaptogrid" );

}

void KPrView::viewSnapToGrid()
{
}

void KPrView::viewGrid()
{

}

KoShapeManager* KPrView::shapeManager() const
{
    return m_canvas->shapeManager();
}

KPrCanvas* KPrView::canvasWidget() const
{
    return m_canvas;
}

void KPrView::setActivePage(KPrPage* page)
{
    if ( !page )
        return;
    m_activePage=page;

    m_canvas->shapeController()->setShapeControllerBase(m_activePage);
    shapeManager()->setShapes(m_activePage->shapes());

    KoPageLayout layout = m_activePage->pageLayout();
    m_horizontalRuler->setRulerLength(layout.ptWidth);
    m_verticalRuler->setRulerLength(layout.ptHeight);
    m_horizontalRuler->setActiveRange(layout.ptLeft, layout.ptWidth - layout.ptRight);
    m_verticalRuler->setActiveRange(layout.ptTop, layout.ptHeight - layout.ptBottom);

}

#include "KPrView.moc"
