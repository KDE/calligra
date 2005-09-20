/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2003 theKompany.com & Dave Marotti,
 *                         Peter Simonsson
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
/**************************************************************************************
 *
 * The code for dragging and resizing stencils is all contained in this class. KivioCanvas
 * is used only for drawing since it's a canvas.
 *
 */

#include "tool_select.h"

#include "kivio_view.h"
#include "kivio_doc.h"
#include "kivio_canvas.h"
#include "kivio_page.h"

#include "kivio_custom_drag_data.h"
#include "kivio_layer.h"
#include "kivio_stencil.h"
#include "object.h"

#include <kactionclasses.h>
#include <kpopupmenu.h>
#include <kdebug.h>
#include <kozoomhandler.h>
#include <koPoint.h>
#include <klocale.h>
#include "kivio_command.h"

#include <qwmatrix.h>

#include "kivio_pluginmanager.h"

SelectTool::SelectTool( KivioView* parent ) : Kivio::MouseTool(parent, "Selection Mouse Tool")
{
  view()->pluginManager()->setDefaultTool(this);

  m_selectAction = new KRadioAction(i18n("&Select"), "select", Key_Space, actionCollection(), "select");
  connect(m_selectAction, SIGNAL(toggled(bool)), this, SLOT(setActivated(bool)));

  m_textEditAction = new KAction(i18n("&Edit Text..."), "text", Key_F2, this, SLOT(editStencilText()), actionCollection(), "editText");
  m_textFormatAction = new KAction(i18n("Format &Text..."), 0, 0, view(), SLOT(textFormat()), actionCollection(), "formatText");
  (void) new KAction(i18n("Format &Stencils && Connectors..."), 0, 0, view(), SLOT(stencilFormat()),
                          actionCollection(), "formatStencil");
  m_arrowHeadAction = new KAction(i18n("Format &Arrowheads..."), 0, 0, view(), SLOT(arrowHeadFormat()),
                                  actionCollection(), "formatConnector");

  m_mode = MSelect;
  m_clickedObject = 0;
}

SelectTool::~SelectTool()
{
}


/**
 * Event delegation
 *
 * @param e The event to be identified and processed
 *
 */
bool SelectTool::processEvent(QEvent* e)
{
  KivioCanvas* canvas = view()->canvasWidget();
  QMouseEvent *m;

  switch (e->type())
  {
    case QEvent::MouseButtonDblClick:
      m = (QMouseEvent *)e;

      if( m->button() == LeftButton ) {
        leftDoubleClick(m->pos());
      }

      canvas->setFocus();
      return true;
      break;

    case QEvent::MouseButtonPress:
    {
      QMouseEvent* me = static_cast<QMouseEvent*>(e);

      if( me->button() == RightButton ) {
        showPopupMenu(me->globalPos());
      } else if( me->button() == LeftButton ) {
        leftMouseButtonPressed(me->pos(), (me->state() & ControlButton));
      }

      canvas->setFocus();
      return true;
      break;
    }

    case QEvent::MouseButtonRelease:
      endMove();
      canvas->setFocus();
      return true;
      break;

    case QEvent::MouseMove:
      mouseMove( static_cast<QMouseEvent*>(e));
      return true;
      break;

    case QEvent::KeyPress:
      if((static_cast<QKeyEvent*>(e)->key() >= Key_Left) && (static_cast<QKeyEvent*>(e)->key() <= Key_Down)) {
        keyPress(static_cast<QKeyEvent*>(e));
        return true;
      }
      break;

    default:
      break;
  }

  return false;
}

void SelectTool::setActivated(bool a)
{
  if(a) {
    m_selectAction->setChecked(true);
    view()->canvasWidget()->setCursor(arrowCursor);
    m_mode = MSelect;
    emit activated(this);
  } else if(m_selectAction->isChecked()) {
    m_selectAction->setChecked(false);
    view()->canvasWidget()->activePage()->setPaintSelected(true);
    m_clickedObject = 0;
  }
}

/**
 * Selects all stencils inside a given rect
 */
void SelectTool::select(const QRect &r)
{
    // Calculate the start and end clicks in terms of page coordinates
    KoPoint startPoint = view()->canvasWidget()->mapFromScreen( QPoint( r.x(), r.y() ) );
    KoPoint releasePoint = view()->canvasWidget()->mapFromScreen( QPoint( r.x() + r.width(), r.y() + r.height() ) );


    double x, y, w, h;

    // Calculate the x,y position of the selection box
    x = startPoint.x() < releasePoint.x() ? startPoint.x() : releasePoint.x();
    y = startPoint.y() < releasePoint.y() ? startPoint.y() : releasePoint.y();

    // Calculate the w/h of the selection box
    w = releasePoint.x() - startPoint.x();

    if( w < 0.0 ) {
        w *= -1.0;
    }

    h = releasePoint.y() - startPoint.y();

    if( h < 0.0 ) {
        h *= -1.0;
    }

    // Tell the page to select all stencils in this box
    view()->activePage()->selectStencils( x, y, w, h );
}

void SelectTool::leftMouseButtonPressed(const QPoint& pos, bool selectMultiple)
{
  Kivio::CollisionFeedback collisionFeedback;
  KoPoint p = view()->canvasWidget()->mapFromScreen(pos);
  Kivio::Object* object = view()->activePage()->checkForCollision(p, collisionFeedback);

  kdDebug() << "Select!" << endl;

  if(!object) {
    if(!selectMultiple) {
      view()->activePage()->unselectAllStencils();
      view()->canvasWidget()->update();
    }

    return;
  }

  kdDebug() << "Select object: " << object->name() << endl;

  if(!selectMultiple && !object->selected()) {
    view()->activePage()->unselectAllStencils();
  }

  if(object->selected() && selectMultiple) {
    view()->activePage()->unselectStencil(object);
  } else {
    view()->activePage()->selectStencil(object);
  }

  view()->canvasWidget()->update();

  if(object->selected()) {
    if(collisionFeedback.type == CTBody) {
      m_mode = MMove;
    } else if(collisionFeedback.type == CTResizePoint) {
      m_mode = MResize;
    }

    if((m_mode == MResize) || (m_mode == MMove)) {
      QValueList<Kivio::Object*>::iterator itEnd = view()->activePage()->selectedStencils()->end();

      for(QValueList<Kivio::Object*>::iterator it = view()->activePage()->selectedStencils()->begin(); it != itEnd; ++it) {
        m_origObjectList.append((*it)->duplicate());
      }
    }

    m_previousPos = p;
    m_collisionFeedback = collisionFeedback;
    m_clickedObject = object;
  }

  changeMouseCursor(pos);
}


void SelectTool::move(const QPoint& pos)
{
  KoPoint p = view()->canvasWidget()->mapFromScreen(pos);
  KoPoint move = p - m_previousPos;

  QValueList<Kivio::Object*>::iterator itEnd = view()->activePage()->selectedStencils()->end();

  for(QValueList<Kivio::Object*>::iterator it = view()->activePage()->selectedStencils()->begin(); it != itEnd; ++it) {
    (*it)->move(move.x(), move.y()); // Move the Object
  }

  view()->canvasWidget()->update();
  view()->updateToolBars();
  m_previousPos = p;
}

void SelectTool::endMove()
{
  QValueList<Kivio::Object*>::iterator it = m_origObjectList.begin();
  QValueList<Kivio::Object*>::iterator itEnd = m_origObjectList.end();
  QValueList<Kivio::Object*>::iterator it2 = view()->activePage()->selectedStencils()->begin();
  QValueList<Kivio::Object*>::iterator itEnd2 = view()->activePage()->selectedStencils()->end();
  KMacroCommand* macro = new KMacroCommand(i18n("Move Stencil"));
  bool moved = false;

  while((it != itEnd) && (it2 != itEnd2)) {
    if((*it)->position() != (*it2)->position()) {
      KivioMoveStencilCommand * cmd = new KivioMoveStencilCommand( i18n("Move Stencil"),
          (*it2), (*it)->position(), (*it2)->position(), view()->activePage());
      macro->addCommand( cmd);
      moved = true;
    }

    ++it;
    ++it2;
  }

  if(moved) {
    view()->doc()->addCommand(macro);
  } else {
    delete macro;
  }

  it = m_origObjectList.begin();

  while(it != itEnd) {
    delete (*it);
    it = m_origObjectList.remove(it);
  }

  m_mode = MSelect;
  m_clickedObject = 0;
  view()->doc()->updateView(view()->activePage());
}

void SelectTool::resize(const QPoint& pos)
{
  if(!m_clickedObject) {
    return;
  }

  KoPoint p = view()->canvasWidget()->mapFromScreen(pos);
  KoPoint move = p - m_previousPos;

  m_clickedObject->moveResizePoint(m_collisionFeedback.resizePointId, move);
  m_previousPos = p;
  view()->canvasWidget()->update();
  view()->updateToolBars();
}

void SelectTool::endResize()
{
  QValueList<Kivio::Object*>::iterator it = m_origObjectList.begin();
  QValueList<Kivio::Object*>::iterator itEnd = m_origObjectList.end();

  while(it != itEnd) {
    delete (*it);
    it = m_origObjectList.remove(it);
  }

  m_mode = MSelect;
  m_clickedObject = 0;
  view()->doc()->updateView(view()->activePage());
}

/**
 * Tests if we should start rubber banding (always returns true).
 */
bool SelectTool::startRubberBanding(const QPoint &pos)
{
/*  KivioCanvas* canvas = view()->canvasWidget();
  // We didn't find a stencil, so unselect everything if we aren't holding the control key down
  if( !m_controlKey )
    canvas->activePage()->unselectAllStencils();

  canvas->startRectDraw( pos, KivioCanvas::Rubber );
  canvas->repaint();

  return true;*/
}


/**
 * Tests if we can start dragging a stencil.
 */
bool SelectTool::startDragging(const QPoint &pos, bool onlySelected)
{
/*  KivioCanvas* canvas = view()->canvasWidget();
  KivioPage *pPage = canvas->activePage();
  KivioStencil *pStencil;
  int colType;

  // Figure out how big 4 pixels is in terms of points
  double threshold =  view()->zoomHandler()->unzoomItY(4);

  KoPoint pagePoint = canvas->mapFromScreen( pos );

  pStencil = pPage->checkForStencil( &pagePoint, &colType, threshold, onlySelected );

  if( !pStencil )
    return false;

  canvas->setEnabled(false);

  if( pStencil->isSelected() )
  {
    // If we are clicking an already selected stencil, and the control
    // key down, then unselect this stencil
    if( m_controlKey==true ) {
      pPage->unselectStencil( pStencil );
    }

    // Otherwise, it means we are just moving
  }
  else
  {
    // Clicking a new stencil, and the control key is not down
    if( !m_controlKey )
      pPage->unselectAllStencils();

    pPage->selectStencil( pStencil );
  }

  // Create a new painter object
  canvas->beginUnclippedSpawnerPainter();

  // Build the list of old geometry
  KivioSelectDragData *pData;
  m_lstOldGeometry.clear();
  pStencil = canvas->activePage()->selectedStencils()->first();

  while( pStencil )
  {
    pData = new KivioSelectDragData;
    pData->rect = pStencil->rect();
    m_lstOldGeometry.append(pData);

    pStencil = canvas->activePage()->selectedStencils()->next();
  }

  m_selectedRect = view()->activePage()->getRectForAllSelectedStencils();
  changeMouseCursor(pos);
  // Set the mode
  m_mode = stmDragging;
  m_firstTime = true;
  canvas->setEnabled(true);*/
  return true;
}

bool SelectTool::startCustomDragging(const QPoint &pos, bool selectedOnly )
{
/*  KivioCanvas* canvas = view()->canvasWidget();
  KivioPage *pPage = canvas->activePage();
  KivioStencil *pStencil;
  int colType;

  KoPoint pagePoint = canvas->mapFromScreen( pos );
  
  // Figure out how big 4 pixels is in terms of points
  double threshold =  view()->zoomHandler()->unzoomItY(4);

  pStencil = pPage->checkForStencil( &pagePoint, &colType, threshold, selectedOnly );

  if( !pStencil || colType < kctCustom ) {
    return false;
  }


  if(pStencil->isSelected()) {
    // If we are clicking an already selected stencil, and the control
    // key down, then unselect this stencil
    if(m_controlKey) {
      pPage->unselectStencil(pStencil);
    }
  } else {
    // Clicking a new stencil, and the control key is not down
    if(!m_controlKey) {
      pPage->unselectAllStencils();
    }

    pPage->selectStencil( pStencil );
  }

  m_pCustomDraggingStencil = pStencil;

  // Set the mode
  m_mode = stmCustomDragging;

  m_customDragID = colType;

  // Create a new painter object
  canvas->beginUnclippedSpawnerPainter();
  m_firstTime = true;
*/
  return true;
}

/**
 * Tests if we can start resizing a stencil
 */
bool SelectTool::startResizing(const QPoint &pos)
{
/*  KivioCanvas* canvas = view()->canvasWidget();
  KoPoint pagePoint = canvas->mapFromScreen(pos);
  KivioSelectDragData *pData;

  double x = pagePoint.x();
  double y = pagePoint.y();

  // Search selected stencils to see if we have a resizing point
  KivioStencil *pStencil = canvas->activePage()->selectedStencils()->first();
  while( pStencil )
  {
    m_resizeHandle = isOverResizeHandle(pStencil, x, y);
    if( m_resizeHandle > 0 )
    {
      switch( m_resizeHandle )
      {
        case 1: // top left
          m_origPoint.setCoords(pStencil->x(), pStencil->y());
          break;

        case 2:
          m_origPoint.setCoords((pStencil->x() + pStencil->w()) / 2.0, pStencil->y());
          break;

        case 3:
          m_origPoint.setCoords(pStencil->x() + pStencil->w(), pStencil->y());
          break;

        case 4:
          m_origPoint.setCoords(pStencil->x() + pStencil->w(), (pStencil->y() + pStencil->h()) / 2.0);
          break;

        case 5:
          m_origPoint.setCoords(pStencil->x() + pStencil->w(), pStencil->y() + pStencil->h());
          break;

        case 6:
          m_origPoint.setCoords((pStencil->x() + pStencil->w()) / 2.0, pStencil->y() + pStencil->h());
          break;

        case 7:
          m_origPoint.setCoords(pStencil->x(), pStencil->y() + pStencil->h());
          break;

        case 8:
          m_origPoint.setCoords(pStencil->x(), (pStencil->y() + pStencil->h()) / 2.0);
          break;
      }

      m_lstOldGeometry.clear();
      pData = new KivioSelectDragData;
      pData->rect = pStencil->rect();
      m_lstOldGeometry.append(pData);

      m_pResizingStencil = pStencil;

      // Create a new painter object
      canvas->beginUnclippedSpawnerPainter();
      m_firstTime = true;

      return true;
    }

    pStencil = canvas->activePage()->selectedStencils()->next();
  }
*/
  return false;
}



void SelectTool::mouseMove(QMouseEvent* e)
{
  QPoint pos = e->pos();
  bool ignoreGridGuides = e->state() & ShiftButton;

  switch(m_mode) {
    case MSelect:
      changeMouseCursor(pos);
      break;

    case MMove:
      move(pos);
      break;

    case MResize:
      resize(pos);
      break;

    default:
      break;
  }
}

void SelectTool::continueRubberBanding(const QPoint &pos)
{
    view()->canvasWidget()->continueRectDraw( pos, KivioCanvas::Rubber );
}

void SelectTool::continueCustomDragging(const QPoint &pos)
{
/*  KivioCanvas* canvas = view()->canvasWidget();
  KoPoint pagePoint = canvas->mapFromScreen(pos);
  bool hit = false;

  if(m_pCustomDraggingStencil->type() == kstConnector){
    pagePoint = canvas->activePage()->snapToTarget(pagePoint, 8.0, hit);
  }

  if(!hit) {
    pagePoint = canvas->snapToGrid( pagePoint );
  }

  KivioCustomDragData data;
  data.page = canvas->activePage();
  data.dx = pagePoint.x() - m_lastPoint.x();
  data.dy = pagePoint.y() - m_lastPoint.y();
  data.x = pagePoint.x();
  data.y = pagePoint.y();
  data.id = m_customDragID;
  data.scale = view()->zoomHandler()->zoomedResolutionY();


  // Undraw the old stencils
  if(!m_firstTime) {
    canvas->drawStencilXOR(m_pCustomDraggingStencil);
  } else {
    m_pCustomDraggingStencil->setHidden(true);
    canvas->repaint();
    m_firstTime = false;
  }


  // Custom dragging can only occur on one stencil
  if( m_pCustomDraggingStencil )
    m_pCustomDraggingStencil->customDrag( &data );

  // Draw the stencils
  canvas->drawStencilXOR(m_pCustomDraggingStencil);
  view()->updateToolBars();*/
}


void SelectTool::continueResizing(const QPoint &pos, bool ignoreGridGuides)
{
//   KivioCanvas* canvas = view()->canvasWidget();
//   KoPoint pagePoint = canvas->mapFromScreen(pos);
//   
//   if(!ignoreGridGuides) {
//     pagePoint = canvas->snapToGridAndGuides( pagePoint );
//   }
//   
//   KivioSelectDragData *pData = m_lstOldGeometry.first();

/*  QWMatrix m;
  double w2 = m_pResizingStencil->w() / 2.0;
  double h2 = m_pResizingStencil->h() / 2.0;
  m.translate(m_pResizingStencil->x(), m_pResizingStencil->y());
  m.translate(m_pResizingStencil->pinPoint().x(), m_pResizingStencil->pinPoint().y());
  m.rotate(-m_pResizingStencil->rotation());
  m.translate(-m_pResizingStencil->pinPoint().x(), -m_pResizingStencil->pinPoint().y());
  m.translate(-m_pResizingStencil->x(), -m_pResizingStencil->y());
  m.invert();
  
  double x = pagePoint.x() * m.m11() + pagePoint.y() * m.m21() + m.dx();
  double y = pagePoint.x() * m.m12() + pagePoint.y() * m.m22() + m.dy();*/
  
/*  if( !pData )
  {
      kdDebug(43000) << "SelectTool::continueResizing() - Original geometry not found" << endl;
      return;
  }

  double dx = pagePoint.x() - m_origPoint.x();
  double dy = pagePoint.y() - m_origPoint.y();
      
  if((dx > 0) || (dy > 0) || (dx < 0) || (dy < 0)) { // Do we really need to redraw?
    // Undraw the old outline
    if(!m_firstTime) {
      canvas->drawStencilXOR( m_pResizingStencil );
    } else {
      m_pResizingStencil->setHidden(true);
      canvas->repaint();
      m_firstTime = false;
    }
  
    double sx = pData->rect.x();
    double sy = pData->rect.y();
    double sw = pData->rect.width();
    double sh = pData->rect.height();
    double ratio = sw / sh;
  
    switch( m_resizeHandle )
    {
      case 1: // top left
        if( m_pResizingStencil->protection()->testBit( kpWidth )==false &&
          m_pResizingStencil->protection()->testBit( kpHeight )==false )
        {
          if((dx > dy) && (dx != 0)) {
            dy = dx / ratio;
          } else {
            dx = dy * ratio;
          }
  
          m_pResizingStencil->setX( sx + dx );
          m_pResizingStencil->setW( sw - dx );
  
          m_pResizingStencil->setY( sy + dy );
          m_pResizingStencil->setH( sh - dy );
        }
        break;
  
      case 2: // top
        if( m_pResizingStencil->protection()->testBit( kpHeight )==false )
        {
          m_pResizingStencil->setY( sy + dy );
          m_pResizingStencil->setH( sh - dy );
        }
        break;
  
      case 3: // top right
        if( m_pResizingStencil->protection()->testBit( kpHeight )==false &&
          m_pResizingStencil->protection()->testBit( kpWidth )==false )
        {
          if((dx > dy) && (dx != 0)) {
            dy = -(dx / ratio);
          } else {
            dx = -(dy * ratio);
          }
  
          m_pResizingStencil->setY( sy + dy );
          m_pResizingStencil->setH( sh - dy );
  
          m_pResizingStencil->setW( sw + dx );
        }
        break;
  
      case 4: // right
        if( m_pResizingStencil->protection()->testBit( kpWidth )==false )
        {
          // see old kivio source when snaptogrid gets implemented
          //setX( SnapToGrid(sx+sw+dx)-sx )
          m_pResizingStencil->setW( sw + dx );
        }
        break;
  
      case 5: // bottom right
        if( m_pResizingStencil->protection()->testBit( kpWidth )==false &&
          m_pResizingStencil->protection()->testBit( kpHeight )==false )
        {
          if((dx > dy) && (dx != 0)) {
            dy = dx / ratio;
          } else {
            dx = dy * ratio;
          }
  
          m_pResizingStencil->setW( sw + dx );
          m_pResizingStencil->setH( sh + dy );
        }
        break;
  
      case 6: // bottom
        if( m_pResizingStencil->protection()->testBit( kpHeight )==false )
        {
          m_pResizingStencil->setH( sh + dy );
        }
        break;
  
      case 7: // bottom left
        if( m_pResizingStencil->protection()->testBit( kpWidth )==false &&
          m_pResizingStencil->protection()->testBit( kpHeight )==false )
        {
          if((dx > dy) && (dx != 0)) {
            dy = -(dx / ratio);
          } else {
            dx = -(dy * ratio);
          }
  
          m_pResizingStencil->setX( sx + dx );
          m_pResizingStencil->setW( sw - dx );
  
          m_pResizingStencil->setH( sh + dy );
        }
        break;
  
      case 8: // left
        if( m_pResizingStencil->protection()->testBit( kpWidth )==false )
        {
          KoPoint pinPoint = m_pResizingStencil->pinPoint();
          m_pResizingStencil->setPinPoint(KoPoint(pinPoint.x() - (dx / 2.0), pinPoint.y()));
          m_pResizingStencil->setX( sx + dx );
          m_pResizingStencil->setW( sw - dx );
        }
        break;
  
      default:
        kdDebug(43000) << "SelectTool::continueResizing() - unknown resize handle: " <<  m_resizeHandle << endl;
        break;
    }
  
    canvas->drawStencilXOR( m_pResizingStencil );
    view()->updateToolBars();
  }*/
}


/**
 * Change the mouse cursor based on what it is over.
 */
void SelectTool::changeMouseCursor(const QPoint &pos)
{
  KivioCanvas* canvas = view()->canvasWidget();
  Kivio::CollisionFeedback collisionFeedback;
  KoPoint p = canvas->mapFromScreen(pos);
  Kivio::Object* object = view()->activePage()->checkForCollision(p, collisionFeedback);

  if((collisionFeedback.type == CTBody) && object->selected()) {
    canvas->setCursor(sizeAllCursor);
  } else {
    canvas->setCursor(arrowCursor);
  }
}

void SelectTool::mouseRelease(const QPoint &pos)
{
/*  m_releasePoint = pos;

  switch( m_mode )
  {
    case stmDrawRubber:
      endRubberBanding(pos);
      break;

    case stmCustomDragging:
      endCustomDragging(pos);
      break;

    case stmDragging:
      endDragging(pos);
      break;

    case stmResizing:
      endResizing(pos);
      break;
  }

  m_mode = stmNone;

  view()->doc()->updateView(view()->activePage());*/
}

void SelectTool::endRubberBanding(const QPoint &pos)
{
/*  KivioCanvas* canvas = view()->canvasWidget();
  // End the rubber-band drawing
  canvas->endRectDraw();

  KoPoint p = canvas->mapFromScreen(pos);

  // We can't select if the start and end points are the same
  if( m_origPoint.x() != p.x() && m_origPoint.y() != p.y() )
  {
    select(canvas->rect());
  }

  view()->updateToolBars();*/
}

void SelectTool::endCustomDragging(const QPoint&)
{
/*  KivioCanvas* canvas = view()->canvasWidget();
  m_pCustomDraggingStencil->setHidden(false);
  m_customDragID = 0;
  canvas->drawStencilXOR(m_pCustomDraggingStencil);
  KivioStencil *pStencil = canvas->activePage()->selectedStencils()->first();

  while( pStencil )
  {
    if(pStencil->type() == kstConnector) {
      pStencil->searchForConnections(view()->activePage(), view()->zoomHandler()->unzoomItY(4));
    }

    pStencil = canvas->activePage()->selectedStencils()->next();
  }

  canvas->endUnclippedSpawnerPainter();*/
}

void SelectTool::endResizing(const QPoint&)
{
/*  KivioCanvas* canvas = view()->canvasWidget();
  m_pResizingStencil->setHidden(false);
  KivioResizeStencilCommand * cmd = new KivioResizeStencilCommand( i18n("Resize Stencil"),
    m_pResizingStencil, m_lstOldGeometry.first()->rect, m_pResizingStencil->rect(), view()->activePage());
  canvas->doc()->addCommand( cmd );
  // Undraw the last outline
  canvas->drawStencilXOR( m_pResizingStencil );

  if(m_pResizingStencil->type() == kstConnector) {
    m_pResizingStencil->searchForConnections(view()->activePage(), view()->zoomHandler()->unzoomItY(4));
  }

  // Deallocate the painter object
  canvas->endUnclippedSpawnerPainter();

  // Set the class vars to nothing
  m_pResizingStencil = NULL;
  m_resizeHandle = 0;*/
}

/**
 * Shows the popupmenu at a given point.
 */
void SelectTool::showPopupMenu( const QPoint &pos )
{
/*  KPopupMenu* menu = 0;

  if(view()->activePage()->selectedStencils()->count() < 1) {
    menu = static_cast<KPopupMenu*>(view()->factory()->container("PagePopup", view()));
  } else {
    menu = static_cast<KPopupMenu*>(view()->factory()->container("StencilPopup", view()));
    m_arrowHeadAction->setEnabled(view()->activePage()->checkForStencilTypeInSelection(kstConnector));

    if(view()->activePage()->checkForTextBoxesInSelection()) {
      m_textEditAction->setEnabled(true);
      m_textFormatAction->setEnabled(true);
    } else {
      m_textEditAction->setEnabled(false);
      m_textFormatAction->setEnabled(false);
    }
  }

  if(menu) {
    m_lastPoint = view()->canvasWidget()->mapFromScreen(pos);
    menu->popup(pos);
  } else {
    kdDebug(43000) << "What no popup! *ARGH*!" << endl;
  }*/
}


/**
 * Handles what happens when a left-button double click occurs.
 *
 * If there are no stencils selected, this function returns.  Otherwise
 * it launches the text tool on the selected stencils and switches back
 * to this tool when it's done.
 */
void SelectTool::leftDoubleClick(const QPoint& pos)
{
/*  if( view()->activePage()->selectedStencils()->count() <= 0 )
    return;
  
  KoPoint pagePoint = view()->canvasWidget()->mapFromScreen(pos);
  // Figure out how big 4 pixels is in terms of points
  double threshold =  view()->zoomHandler()->unzoomItY(4);
  int colType;
  KivioPage *page = view()->activePage();
  KivioStencil* stencil = page->checkForStencil( &pagePoint, &colType, threshold, false);
  
  if(stencil) {
    // Locate the text tool.  If not found, bail with an error
    Kivio::Plugin *p = view()->pluginManager()->findPlugin("Text Mouse Tool");
    
    if( !p )
    {
      kdDebug(43000) << "SelectTool::leftDoubleClick() - unable to locate Text Tool" << endl;
      return;
    }
    
    static_cast<Kivio::MouseTool*>(p)->applyToolAction(stencil, pagePoint);
  }*/
}

void SelectTool::editText(QPtrList<KivioStencil>* stencils)
{
/*  // Locate the text tool.  If not found, bail with an error
  Kivio::Plugin *p = view()->pluginManager()->findPlugin("Text Mouse Tool");
  if( !p )
  {
    kdDebug(43000) << "SelectTool::leftDoubleClick() - unable to locate Text Tool" << endl;
    return;
  }
  
  // Select the text tool (which makes the text dialog pop up)
  static_cast<Kivio::MouseTool*>(p)->applyToolAction(stencils);*/
}

void SelectTool::editStencilText()
{
//   editText(view()->activePage()->selectedStencils());
}

void SelectTool::keyPress(QKeyEvent* e)
{
/*  KivioCanvas* canvas = view()->canvasWidget();
  
  canvas->setEnabled(false);

  // Create a new painter object
  canvas->beginUnclippedSpawnerPainter();

  // Build the list of old geometry
  KivioSelectDragData *pData;
  m_lstOldGeometry.clear();
  KivioStencil* pStencil = canvas->activePage()->selectedStencils()->first();

  while( pStencil )
  {
    pData = new KivioSelectDragData;
    pData->rect = pStencil->rect();
    m_lstOldGeometry.append(pData);


    pStencil = canvas->activePage()->selectedStencils()->next();
  }

  m_selectedRect = view()->activePage()->getRectForAllSelectedStencils();
  // Set the mode
  m_mode = stmDragging;
  canvas->setEnabled(true);
  m_origPoint = m_selectedRect.topLeft();
  KivioGridData gd = view()->doc()->grid();
  bool ignoreGridGuides = e->state() & ShiftButton;
  double distX, distY;
  
  if(ignoreGridGuides || !view()->doc()->grid().isSnap) {
    distX = view()->zoomHandler()->unzoomItX(1);
    distY = view()->zoomHandler()->unzoomItY(1);
  } else {
    distX = gd.freq.width();
    distY = gd.freq.height();
  }
  
  switch(e->key()) {
    case Key_Left:
      continueDragging(canvas->mapToScreen(KoPoint(m_selectedRect.x() - distX,
        m_selectedRect.y())), ignoreGridGuides);
      break;
    case Key_Up:
      continueDragging(canvas->mapToScreen(KoPoint(m_selectedRect.x(),
        m_selectedRect.y() - distY)), ignoreGridGuides);
      break;
    case Key_Right:
      continueDragging(canvas->mapToScreen(KoPoint(m_selectedRect.x() + distX,
        m_selectedRect.y())), ignoreGridGuides);
      break;
    case Key_Down:
      continueDragging(canvas->mapToScreen(KoPoint(m_selectedRect.x(),
        m_selectedRect.y() + distY)), ignoreGridGuides);
      break;
    default:
      break;
  }
  
  endDragging(QPoint());
  canvas->setFocus();*/
}

#include "tool_select.moc"
