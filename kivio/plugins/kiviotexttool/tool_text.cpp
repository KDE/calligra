/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include "tool_text.h"

#include <qcursor.h>
#include <kiconloader.h>
#include <kstddirs.h>
#include <kdebug.h>

#include "kivio_view.h"
#include "kivio_canvas.h"
#include "kivio_page.h"
#include "kivio_doc.h"
#include "stencil_text_dlg.h"

#include "kivio_stencil_spawner_set.h"
#include "kivio_stencil_spawner.h"
#include "kivio_custom_drag_data.h"
#include "kivio_layer.h"
#include "kivio_point.h"
#include "kivio_stencil.h"

TextTool::TextTool( KivioView* view )
:Tool(view,"Text")
{
  setSortNum(2);

  ToolSelectAction* text = new ToolSelectAction( actionCollection(), "ToolAction" );
  KAction* m_z1 = new KAction( i18n("Edit Stencil Text"), "kivio_text", CTRL+Key_T, actionCollection(), "text" );
  text->insert(m_z1);

  m_mode = stmNone;

  QPixmap pix = BarIcon("kivio_text_cursor");
  m_pTextCursor = new QCursor(pix,2,2);
}

TextTool::~TextTool()
{
}


/**
 * Event delegation
 *
 * @param e The event to be identified and processed
 *
 */
void TextTool::processEvent( QEvent* e )
{
    switch (e->type())
    {
    case QEvent::MouseButtonPress:
        mousePress( (QMouseEvent*)e );
        break;

    case QEvent::MouseButtonRelease:
        mouseRelease( (QMouseEvent*)e );
        break;

    case QEvent::MouseMove:
        mouseMove( (QMouseEvent*)e );
        break;

    default:
      break;
    }
}

void TextTool::activate()
{
   kdDebug() << "TextTool activate" << endl;
    m_pCanvas->setCursor(*m_pTextCursor);
    m_mode = stmNone;
    
    KivioPage *pPage =m_pView->activePage();
    KivioStencil *pStencil = pPage->selectedStencils()->first();

    if( !pStencil )
        return;
    
    setStencilText();

    controller()->activateDefault();
}

void TextTool::setStencilText()
{
    KivioDoc* doc = m_pView->doc();
    KivioPage *page =m_pView->activePage();
    KivioStencil *stencil = page->selectedStencils()->first();

    if( !stencil )
        return;

    KivioStencilTextDlg d(0, stencil->text());

    if( !d.exec() )
        return;

    QString text = d.text();

    while( stencil )
    {
        stencil->setText( text );
        stencil = page->selectedStencils()->next();
    }

    doc->updateView(page);
}

void TextTool::deactivate()
{
}

void TextTool::configure()
{
}


void TextTool::text(QRect r)
{
    // Calculate the start and end clicks in terms of page coordinates
    TKPoint startPoint = m_pCanvas->mapFromScreen( QPoint( r.x(), r.y() ) );
    TKPoint releasePoint = m_pCanvas->mapFromScreen( QPoint( r.x() + r.width(), r.y() + r.height() ) );

    // Calculate the x,y position of the textion box
    float x = startPoint.x < releasePoint.x ? startPoint.x : releasePoint.x;
    float y = startPoint.y < releasePoint.y ? startPoint.y : releasePoint.y;

    // Calculate the w/h of the textion box
    float w = releasePoint.x - startPoint.x;
    if( w < 0.0 )
        w *= -1.0;

    float h = releasePoint.y - startPoint.y;
    if( h < 0.0 )
        h *= -1.0;

    KivioDoc* doc = m_pView->doc();
    KivioPage* page = m_pCanvas->activePage();
    
    KivioStencilSpawner* ss = doc->findInternalStencilSpawner("Text");
    if (!ss)
        return;
        
    KivioStencil* stencil = ss->newStencil();
    stencil->setPosition(x,y);
    stencil->setDimensions(w,h);
    stencil->setText("");
    page->unselectAllStencils();
    page->addStencil(stencil);
    page->selectStencil(stencil);
    
    doc->updateView(page,false);

    setStencilText();
    
    if (stencil->text().isEmpty()) {
        page->deleteSelectedStencils();
        doc->updateView(page,false);
    }
}

void TextTool::mousePress( QMouseEvent *e )
{
    if( startRubberBanding( e ) )
    {
        m_mode = stmDrawRubber;
    }
}


/**
 * Tests if we should start rubber banding (always returns true).
 */
bool TextTool::startRubberBanding( QMouseEvent *e )
{
    m_pCanvas->startRectDraw( e->pos(), KivioCanvas::Rubber );
    m_pCanvas->repaint();

    return true;
}

void TextTool::mouseMove( QMouseEvent * e )
{
    switch( m_mode )
    {
        case stmDrawRubber:
            continueRubberBanding(e);
            break;
        
        default:
            break;
    }
}

void TextTool::continueRubberBanding( QMouseEvent *e )
{
    m_pCanvas->continueRectDraw( e->pos(), KivioCanvas::Rubber );
}

void TextTool::mouseRelease( QMouseEvent *e )
{
    m_releasePoint = e->pos();

    switch( m_mode )
    {
        case stmDrawRubber:
            endRubberBanding(e);
            break;
    }
	
  	m_mode = stmNone;

    m_pCanvas->repaint();
}

void TextTool::endRubberBanding(QMouseEvent */*e*/)
{
    // End the rubber-band drawing
    m_pCanvas->endRectDraw();

    // We can't text if the start and end points are the same
    if( m_startPoint != m_releasePoint )
    {
        text(m_pCanvas->rect());
    }
}
#include "tool_text.moc"
