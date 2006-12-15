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


#include <KPrCanvas.h>
#include <KPrDocument.h>

#include <klocale.h>
#include <ktoggleaction.h>
	
KPrView::KPrView( KPrDocument *document, QWidget *parent )
: KoView( document, parent )
, m_doc( document )
{
    initGUI();
    initActions();
}

KPrView::~KPrView()
{
    KoToolManager::instance()->removeCanvasController( m_canvasController );
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

#include "KPrView.moc"
