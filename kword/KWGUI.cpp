/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <faure@kde.org>
   Copyright (C) 2005-2006 Thomas Zander <zander@kde.org>

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
 * Boston, MA 02110-1301, USA
*/

#include "KWGUI.h"
#include "KWDocStruct.h"
#include "KWCanvas.h"
#include "KWDocument.h"
#include "KWView.h"

#include <KoTabChooser.h>
#include <KoCanvasView.h>
#include <KoToolManager.h>

#include <QGridLayout>
#include <QSplitter>
#include <QWidget>

#include <kstatusbar.h>

/******************************************************************/
/* Class: KWGUI                                                */
/******************************************************************/
KWGUI::KWGUI( const QString& viewMode, QWidget *parent, KWView *daView )
  : KHBox( parent),
    m_view ( daView )
{

    KWDocument * doc = m_view->kWordDocument();

    m_horRuler  = 0;
    m_vertRuler = 0;

    // The splitter
    m_panner = new QSplitter( Qt::Horizontal, this );

    // The left side
    m_docStruct = new KWDocStruct( m_panner, doc, this );
    m_docStruct->setMinimumWidth( 0 );

    // The right side
    m_right = new QWidget( m_panner );
    QGridLayout *gridLayout = new QGridLayout( m_right );
    gridLayout->setMargin(0);
    gridLayout->setSpacing(0);
    m_canvas = new KWCanvas( viewMode, doc, this );
    m_canvasView = new KoCanvasView(m_right);
    m_canvasView->setCanvas(m_canvas);
    KoToolManager::instance()->addCanvasView(m_canvasView);
    gridLayout->addWidget( m_canvasView, 1, 1 );

    QList<int> l;
    l << 10;
    l << 90;
    m_panner->setSizes( l );

    KoPageLayout layout = doc->pageLayout();

    m_tabChooser = new KoTabChooser( m_right, KoTabChooser::TAB_ALL );
    m_tabChooser->setReadWrite(doc->isReadWrite());
    gridLayout->addWidget( m_tabChooser, 0, 0 );

    m_horRuler = new KoRuler( m_right, m_canvas, Qt::Horizontal, layout,
			      KoRuler::F_INDENTS | KoRuler::F_TABS,
			      doc->unit(), m_tabChooser );
    m_horRuler->setReadWrite(doc->isReadWrite());
    gridLayout->addWidget( m_horRuler, 0, 1 );

    m_vertRuler = new KoRuler( m_right, m_canvas, Qt::Vertical, layout,
			       0, doc->unit() );
    m_vertRuler->setReadWrite(doc->isReadWrite());
    gridLayout->addWidget( m_vertRuler, 1, 0 );

    m_horRuler->setZoom( doc->zoomedResolutionX() );
    m_vertRuler->setZoom( doc->zoomedResolutionY() );

    m_horRuler->setGridSize(doc->gridX());

    connect( m_horRuler, SIGNAL( newPageLayout( const KoPageLayout & ) ), m_view, SLOT( newPageLayout( const KoPageLayout & ) ) );
    connect( m_horRuler, SIGNAL( newLeftIndent( double ) ), m_view, SLOT( newLeftIndent( double ) ) );
    connect( m_horRuler, SIGNAL( newFirstIndent( double ) ), m_view, SLOT( newFirstIndent( double ) ) );
    connect( m_horRuler, SIGNAL( newRightIndent( double ) ), m_view, SLOT( newRightIndent( double ) ) );

    connect( m_horRuler, SIGNAL( doubleClicked() ), m_view, SLOT( slotHRulerDoubleClicked() ) );
    connect( m_horRuler, SIGNAL( doubleClicked(double) ), m_view, SLOT( slotHRulerDoubleClicked(double) ) );
    connect( m_horRuler, SIGNAL( unitChanged( KoUnit::Unit ) ), this, SLOT( unitChanged( KoUnit::Unit ) ) );
    connect( m_vertRuler, SIGNAL( newPageLayout( const KoPageLayout & ) ), m_view, SLOT( newPageLayout( const KoPageLayout & ) ) );
    connect( m_vertRuler, SIGNAL( doubleClicked() ), m_view, SLOT( formatPage() ) );
    connect( m_vertRuler, SIGNAL( unitChanged( KoUnit::Unit ) ), this, SLOT( unitChanged( KoUnit::Unit ) ) );

    m_horRuler->hide();
    m_vertRuler->hide();

    reorganize();

    connect( m_horRuler, SIGNAL( tabListChanged( const KoTabulatorList & ) ), m_view,
             SLOT( tabListChanged( const KoTabulatorList & ) ) );

    setKeyCompression( true );
    setAcceptDrops( true );
    setFocusPolicy( Qt::NoFocus );
}

void KWGUI::showGUI()
{
    reorganize();
}

void KWGUI::resizeEvent( QResizeEvent *e )
{
    QWidget::resizeEvent( e );
    reorganize();
}

void KWGUI::reorganize()
{
    int hSpace = m_vertRuler->minimumSizeHint().width();
    int vSpace = m_horRuler->minimumSizeHint().height();
    if(m_view->kWordDocument()->showRuler())
    {
        m_vertRuler->show();
        m_horRuler->show();
        m_tabChooser->show();
        m_tabChooser->setGeometry( 0, 0, hSpace, vSpace );
    }
    else
    {
        m_vertRuler->hide();
        m_horRuler->hide();
        m_tabChooser->hide();
        hSpace = 0;
        vSpace = 0;
    }

    if(m_view->kWordDocument()->showdocStruct()) {
        if(m_docStruct->isHidden()) {
            m_docStruct->show();
            if(m_panner->sizes()[0] < 50) {
                QList<int> l;
                l << 100;
                l << width()-100;
                m_panner->setSizes( l );
            }
        }
    } else
        m_docStruct->hide();

    if( m_view->statusBar())
    {
        if(m_view->kWordDocument()->showStatusBar())
            m_view->statusBar()->show();
        else
            m_view->statusBar()->hide();
    }

    if ( m_view->kWordDocument()->showScrollBar())
    {
        //m_canvas->setVScrollBarMode(Q3ScrollView::Auto);
        //m_canvas->setHScrollBarMode(Q3ScrollView::Auto);
    }
    else
    {
        //m_canvas->setVScrollBarMode(Q3ScrollView::AlwaysOff);
        //m_canvas->setHScrollBarMode(Q3ScrollView::AlwaysOff);
    }
}

void KWGUI::unitChanged( KoUnit::Unit u )
{
    m_view->kWordDocument()->setUnit( u );
}

int KWGUI::visibleWidth() const {
    return m_canvasView->visibleWidth();
}

int KWGUI::visibleHeight() const {
    return m_canvasView->visibleHeight();
}

#include "KWGUI.moc"
