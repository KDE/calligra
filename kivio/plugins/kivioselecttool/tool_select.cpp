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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
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

#include <kactionclasses.h>
#include <kmenu.h>
#include <kdebug.h>
#include <KoZoomHandler.h>
#include <KoPoint.h>
#include <klocale.h>
#include <KoGuides.h>
#include "kivio_command.h"

#include <qwmatrix.h>

#include "kivio_pluginmanager.h"

SelectTool::SelectTool( KivioView* parent ) : Kivio::MouseTool(parent, "Selection Mouse Tool")
{
  view()->pluginManager()->setDefaultTool(this);

  KShortcut selectShortCut(Qt::Key_Space);
  selectShortCut.setSeq(1, QKeySequence(Qt::Key_Escape));
  m_selectAction = new KRadioAction(i18n("&Select"), "select", selectShortCut, actionCollection(), "select");
  connect(m_selectAction, SIGNAL(toggled(bool)), this, SLOT(setActivated(bool)));

  m_textEditAction = new KAction(i18n("&Edit Text..."), "text", Qt::Key_F2,
                                 this, SLOT(editStencilText()), actionCollection(), "editText");
  (void) new KAction(i18n("Format &Stencils && Connectors..."), 0, 0, view(), SLOT(stencilFormat()),
                          actionCollection(), "formatStencil");
  m_arrowHeadAction = new KAction(i18n("Format &Arrowheads..."), 0, 0, view(), SLOT(arrowHeadFormat()),
                                  actionCollection(), "formatConnector");

  m_mode = stmNone;
  m_pResizingStencil = NULL;
  m_pCustomDraggingStencil = NULL;

  m_lstOldGeometry.setAutoDelete(true);

  m_customDragID = 0;
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
      m = (QMouseEvent *)e;

      if( m->button() == RightButton ) {
        showPopupMenu(m->globalPos());
      } else if( m->button() == LeftButton ) {
        if(m->state() & Qt::ControlButton) {
          m_controlKey = true;
        } else {
          m_controlKey = false;
        }

        mousePress( m->pos() );
      }

      canvas->setFocus();
      return true;
      break;

    case QEvent::MouseButtonRelease:
      mouseRelease( ((QMouseEvent *)e)->pos() );
      canvas->setFocus();
      return true;
      break;

    case QEvent::MouseMove:
      mouseMove( static_cast<QMouseEvent*>(e));
      return true;
      break;

    case QEvent::KeyPress:
      if((static_cast<QKeyEvent*>(e)->key() >= Qt::Key_Left) && (static_cast<QKeyEvent*>(e)->key() <= Qt::Key_Down)) {
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
    view()->canvasWidget()->unsetCursor();
    m_mode = stmNone;
    emit activated(this);
  } else if(m_selectAction->isChecked()) {
    m_selectAction->setChecked(false);
    view()->canvasWidget()->activePage()->setPaintSelected(true);
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

void SelectTool::mousePress(const QPoint &pos)
{
  // Last point is used for undrawing at the last position and calculating the distance the mouse has moved
  m_lastPoint = view()->canvasWidget()->mapFromScreen(pos);
  m_origPoint = m_lastPoint;

  // Check if we nailed a custom drag point on a selected stencil
  if( startCustomDragging(pos, true) )
  {
    m_mode = stmCustomDragging;
    return;
  }

  // Check if we are resizing
  if( startResizing(pos) )
  {
    m_mode = stmResizing;
    return;
  }


  // Check if we nailed a custom drag point on any other stencil
  if( startCustomDragging(pos, false) )
  {
    m_mode = stmCustomDragging;
    return;
  }

  // Check if we can drag a stencil
  if( startDragging(pos, false) )
  {
    m_mode = stmDragging;
    return;
  }

  // This should always be the last 'start' call since it always returns true
  if( startRubberBanding(pos) )
  {
    m_mode = stmDrawRubber;
    return;
  }
}


/**
 * Tests if we should start rubber banding (always returns true).
 */
bool SelectTool::startRubberBanding(const QPoint &pos)
{
  KivioCanvas* canvas = view()->canvasWidget();
  // We didn't find a stencil, so unselect everything if we aren't holding the control key down
  if( !m_controlKey )
    canvas->activePage()->unselectAllStencils();

  canvas->startRectDraw( pos, KivioCanvas::Rubber );
  canvas->repaint();

  return true;
}


/**
 * Tests if we can start dragging a stencil.
 */
bool SelectTool::startDragging(const QPoint &pos, bool onlySelected)
{
  KivioCanvas* canvas = view()->canvasWidget();
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
    // Update the auto guidelines
    view()->canvasWidget()->updateAutoGuideLines();
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
  canvas->setEnabled(true);
  return true;
}

bool SelectTool::startCustomDragging(const QPoint &pos, bool selectedOnly )
{
  KivioCanvas* canvas = view()->canvasWidget();
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
  m_customDragOrigPoint = pStencil->customIDPoint(m_customDragID);

  view()->canvasWidget()->setShowConnectorTargets(true);
  view()->canvasWidget()->repaint();

  // Create a new painter object
  canvas->beginUnclippedSpawnerPainter();
  m_firstTime = true;

  return true;
}

/**
 * Tests if we can start resizing a stencil
 */
bool SelectTool::startResizing(const QPoint &pos)
{
  KivioCanvas* canvas = view()->canvasWidget();
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

  return false;
}



void SelectTool::mouseMove(QMouseEvent* e)
{
    QPoint pos = e->pos();
    bool ignoreGridGuides = e->state() & Qt::ShiftButton;
    
    switch( m_mode )
    {
        case stmDrawRubber:
            continueRubberBanding(pos);
            break;

        case stmDragging:
            continueDragging(pos, ignoreGridGuides);
            break;

        case stmCustomDragging:
            continueCustomDragging(pos);
            break;

        case stmResizing:
            continueResizing(pos, ignoreGridGuides);
            break;

        default:
            changeMouseCursor(pos);
            break;
    }

    m_lastPoint = view()->canvasWidget()->mapFromScreen(pos);
}

void SelectTool::continueRubberBanding(const QPoint &pos)
{
    view()->canvasWidget()->continueRectDraw( pos, KivioCanvas::Rubber );
}


/**
 * Continues the dragging process of a stencil (moving)
 *
 * How does this work?  Initially we create a list of all the original
 * geometry of all the selected stencils.  We use that to calculate delta
 * movements and snap them to the grid.
 */
void SelectTool::continueDragging(const QPoint &pos, bool ignoreGridGuides)
{
  KivioCanvas* canvas = view()->canvasWidget();
  KoPoint pagePoint = canvas->mapFromScreen( pos );

  double dx = pagePoint.x() - m_origPoint.x();
  double dy = pagePoint.y() - m_origPoint.y();

  bool snappedX;
  bool snappedY;

  double newX, newY;

  // Undraw the old stencils
  if(!m_firstTime) {
    canvas->drawSelectedStencilsXOR();
  } else {
    canvas->activePage()->setPaintSelected(false);
    canvas->repaint();
    m_firstTime = false;
  }

  // Translate to the new position
  KoPoint p;

  newX = m_selectedRect.x() + dx;
  newY = m_selectedRect.y() + dy;

  if(!ignoreGridGuides) {
    // First attempt a snap-to-grid
    p.setCoords(newX, newY);

    p = canvas->snapToGrid(p);

    newX = p.x();
    newY = p.y();

    // Now the guides override the grid so we attempt to snap to them
    // The bottom
    p.setCoords(m_selectedRect.x() + dx + m_selectedRect.width(), m_selectedRect.y() + dy + m_selectedRect.height());
    p = canvas->snapToGuides(p, snappedX, snappedY);

    if(snappedX) {
      newX = p.x() - m_selectedRect.width();
    }

    if(snappedY) {
      newY = p.y() - m_selectedRect.height();
    }

    // The middle
    p.setCoords(m_selectedRect.x() + dx + (m_selectedRect.width() / 2.0),
                m_selectedRect.y() + dy + (m_selectedRect.height() / 2.0));
    p = canvas->snapToGuides(p, snappedX, snappedY);

    if(snappedX) {
      newX = p.x() - (m_selectedRect.width() / 2.0);
    }

    if(snappedY) {
      newY = p.y() - (m_selectedRect.height() / 2.0);
    }

    // The top
    p.setCoords(m_selectedRect.x() + dx, m_selectedRect.y() + dy);
    p = canvas->snapToGuides(p, snappedX, snappedY);

    if(snappedX) {
      newX = p.x();
    }

    if(snappedY) {
      newY = p.y();
    }
  }

  dx = newX - m_selectedRect.x();
  dy = newY - m_selectedRect.y();

  KivioSelectDragData *pData;
  KivioStencil *pStencil = canvas->activePage()->selectedStencils()->first();
  pData = m_lstOldGeometry.first();

  while( pStencil && pData )
  {
    newX = pData->rect.x() + dx;
    newY = pData->rect.y() + dy;

    if( pStencil->protection()->at( kpX ) == false ) {
      pStencil->setX(newX);
    }
    if( pStencil->protection()->at( kpY ) == false ) {
      pStencil->setY(newY);
    }

    pData = m_lstOldGeometry.next();
    pStencil = canvas->activePage()->selectedStencils()->next();
  }

  // Draw the stencils
  canvas->drawSelectedStencilsXOR();
  view()->updateToolBars();
}

void SelectTool::continueCustomDragging(const QPoint &pos)
{
  KivioCanvas* canvas = view()->canvasWidget();
  KoPoint pagePoint = canvas->mapFromScreen(pos);
  bool hit = false;

  if(m_pCustomDraggingStencil->type() == kstConnector){
    pagePoint = canvas->activePage()->snapToTarget(pagePoint, 8.0, hit);
  }

  if(!hit) {
    pagePoint = canvas->snapToGridAndGuides( pagePoint );
  }

  KivioCustomDragData data;
  data.page = canvas->activePage();
  data.dx = pagePoint.x() - m_lastPoint.x();
  data.dy = pagePoint.y() - m_lastPoint.y();
  data.x = pagePoint.x();
  data.y = pagePoint.y();
  data.id = m_customDragID;
  data.scale = view()->zoomHandler()->zoomedResolutionY();


  if(m_pCustomDraggingStencil->type() != kstConnector){
    // Undraw the old stencils
    if(!m_firstTime) {
      canvas->drawStencilXOR(m_pCustomDraggingStencil);
    } else {
      m_pCustomDraggingStencil->setHidden(true);
      canvas->repaint();
      m_firstTime = false;
    }
  }

  // Custom dragging can only occur on one stencil
  if( m_pCustomDraggingStencil )
    m_pCustomDraggingStencil->customDrag( &data );

  // Draw the stencils
  if(m_pCustomDraggingStencil->type() != kstConnector){
    canvas->drawStencilXOR(m_pCustomDraggingStencil);
  } else {
    view()->canvasWidget()->repaint();
  }

  view()->updateToolBars();
}


void SelectTool::continueResizing(const QPoint &pos, bool ignoreGridGuides)
{
  KivioCanvas* canvas = view()->canvasWidget();
  KoPoint pagePoint = canvas->mapFromScreen(pos);
  
  if(!ignoreGridGuides) {
    pagePoint = canvas->snapToGridAndGuides( pagePoint );
  }
  
  KivioSelectDragData *pData = m_lstOldGeometry.first();

/*  QMatrix m;
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
  
  if( !pData )
  {
      kDebug(43000) << "SelectTool::continueResizing() - Original geometry not found" << endl;
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
        kDebug(43000) << "SelectTool::continueResizing() - unknown resize handle: " <<  m_resizeHandle << endl;
        break;
    }
  
    canvas->drawStencilXOR( m_pResizingStencil );
    view()->updateToolBars();
  }
}


/**
 * Change the mouse cursor based on what it is over.
 */
void SelectTool::changeMouseCursor(const QPoint &pos)
{
  KivioCanvas* canvas = view()->canvasWidget();
  KoPoint pagePoint = canvas->mapFromScreen(pos);
  KivioStencil *pStencil;
  double threshold = view()->zoomHandler()->unzoomItY(4);
  int cursorType;

  // Iterate through all the selected stencils
  pStencil = canvas->activePage()->selectedStencils()->first();
  while( pStencil )
  {
    cursorType = isOverResizeHandle(pStencil, pagePoint.x(), pagePoint.y());
    switch( cursorType )
    {
      case 1: // top left
        canvas->setCursor( sizeFDiagCursor );
        return;

      case 2: // top
        canvas->setCursor( sizeVerCursor );
        return;

      case 3: // top right
        canvas->setCursor( sizeBDiagCursor );
        return;

      case 4: // right
        canvas->setCursor( sizeHorCursor );
        return;

      case 5: // bottom right
        canvas->setCursor( sizeFDiagCursor );
        return;

      case 6: // bottom
        canvas->setCursor( sizeVerCursor );
        return;

      case 7: // bottom left
        canvas->setCursor( sizeBDiagCursor );
        return;

      case 8: // left
        canvas->setCursor( sizeHorCursor );
        return;

      default:
        if( pStencil->checkForCollision( &pagePoint, threshold )!= kctNone )
        {
          canvas->setCursor( sizeAllCursor );
          return;
        }
        break;

    }


    pStencil = canvas->activePage()->selectedStencils()->next();
  }

  canvas->unsetCursor();
}


/**
 * Tests a box for a point.  Used only by isOverResizeHandle().
 */
#define RESIZE_BOX_TEST( x, y, bx, by ) \
x >= bx-three_pixels && \
x <= bx+three_pixels && \
y >= by-three_pixels && \
y <= by+three_pixels

/**
 * Tests if a point is over a stencils
 */
int SelectTool::isOverResizeHandle( KivioStencil *pStencil, const double x, const double y )
{
  double three_pixels = 4.0;

  int available;

  QMatrix m;
  double w = pStencil->w();
  double h = pStencil->h();
  double w2 = pStencil->w() / 2.0;
  double h2 = pStencil->h() / 2.0;
  m.translate(pStencil->x(), pStencil->y());
  m.translate(w2, h2);
  m.rotate(pStencil->rotation());
  m.translate(-w2, -h2);

  // FIXME: this needs to be optimized!!!!
  KoPoint tl(0 * m.m11() + 0 * m.m21() + m.dx(), 0 * m.m12() + 0 * m.m22() + m.dy());
  KoPoint t(w2 * m.m11() + 0 * m.m21() + m.dx(), w2 * m.m12() + 0 * m.m22() + m.dy());
  KoPoint tr(w * m.m11() + 0 * m.m21() + m.dx(), w * m.m12() + 0 * m.m22() + m.dy());
  KoPoint r(w * m.m11() + h2 * m.m21() + m.dx(), w * m.m12() + h2 * m.m22() + m.dy());
  KoPoint br(w * m.m11() + h * m.m21() + m.dx(), w * m.m12() + h * m.m22() + m.dy());
  KoPoint b(w2 * m.m11() + h * m.m21() + m.dx(), w2 * m.m12() + h * m.m22() + m.dy());
  KoPoint bl(0 * m.m11() + h * m.m21() + m.dx(), 0 * m.m12() + h * m.m22() + m.dy());
  KoPoint l(0 * m.m11() + h2 * m.m21() + m.dx(), 0 * m.m12() + h2 * m.m22() + m.dy());

  available = pStencil->resizeHandlePositions();

  // Quick reject
  if( !available )
    return 0;


  // Top left
  if( available & krhpNW &&
    RESIZE_BOX_TEST( x, y, tl.x(), tl.y() ) )
    return 1;

  // Top
  if( available & krhpN &&
    RESIZE_BOX_TEST( x, y, t.x(), t.y() ) )
    return 2;

  // Top right
  if( available & krhpNE &&
    RESIZE_BOX_TEST( x, y, tr.x(), tr.y()  ) )
    return 3;

  // Right
  if( available & krhpE &&
    RESIZE_BOX_TEST( x, y, r.x(), r.y() ) )
    return 4;

  // Bottom right
  if( available & krhpSE &&
    RESIZE_BOX_TEST( x, y, br.x(), br.y() ) )
    return 5;

  // Bottom
  if( available & krhpS &&
    RESIZE_BOX_TEST( x, y, b.x(), b.y() ) )
    return 6;

  // Bottom left
  if( available & krhpSW &&
    RESIZE_BOX_TEST( x, y, bl.x(), bl.y() ) )
    return 7;

  // Left
  if( available & krhpW &&
    RESIZE_BOX_TEST( x, y, l.x(), l.y() ) )
    return 8;

  // Nothing found
  return 0;
}


void SelectTool::mouseRelease(const QPoint &pos)
{
  m_releasePoint = pos;

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

  view()->canvasWidget()->guideLines().repaintAfterSnapping();
  view()->doc()->updateView(view()->activePage());
}

void SelectTool::endRubberBanding(const QPoint &pos)
{
  KivioCanvas* canvas = view()->canvasWidget();
  // End the rubber-band drawing
  canvas->endRectDraw();

  KoPoint p = canvas->mapFromScreen(pos);

  // We can't select if the start and end points are the same
  if( m_origPoint.x() != p.x() && m_origPoint.y() != p.y() )
  {
    select(canvas->rect());
  }

  view()->updateToolBars();
}

void SelectTool::endDragging(const QPoint&)
{
  KivioCanvas* canvas = view()->canvasWidget();
  canvas->activePage()->setPaintSelected(true);
  KMacroCommand *macro=new KMacroCommand( i18n("Move Stencil"));
  KivioStencil *pStencil = canvas->activePage()->selectedStencils()->first();
  KivioSelectDragData *pData = m_lstOldGeometry.first();
  bool moved = false;

  while( pStencil && pData )
  {
    if((pData->rect.x() != pStencil->rect().x()) || (pData->rect.y() != pStencil->rect().y())) {
      KivioMoveStencilCommand * cmd = new KivioMoveStencilCommand( i18n("Move Stencil"),
        pStencil, pData->rect, pStencil->rect(), canvas->activePage());
      macro->addCommand( cmd);

      if(pStencil->type() == kstConnector) {
        pStencil->searchForConnections(view()->activePage(), view()->zoomHandler()->unzoomItY(4));
      }

      moved = true;
    }

    pData = m_lstOldGeometry.next();
    pStencil = canvas->activePage()->selectedStencils()->next();
  }

  if(moved) {
    canvas->doc()->addCommand( macro );
  } else {
    delete macro;
  }

  canvas->drawSelectedStencilsXOR();
  canvas->endUnclippedSpawnerPainter();
  // Clear the list of old geometry
  m_lstOldGeometry.clear();
}

void SelectTool::endCustomDragging(const QPoint&)
{
  KivioCanvas* canvas = view()->canvasWidget();
  m_pCustomDraggingStencil->setHidden(false);
  KivioCustomDragCommand* cmd = new KivioCustomDragCommand(i18n("Move Connector Point"), view()->activePage(),
      m_pCustomDraggingStencil, m_customDragID, m_customDragOrigPoint,
      m_pCustomDraggingStencil->customIDPoint(m_customDragID));
  view()->doc()->addCommand(cmd);
  m_customDragID = 0;
  KivioStencil *pStencil = canvas->activePage()->selectedStencils()->first();

  while( pStencil )
  {
    if(pStencil->type() == kstConnector) {
      pStencil->searchForConnections(view()->activePage(), view()->zoomHandler()->unzoomItY(4));
    }

    pStencil = canvas->activePage()->selectedStencils()->next();
  }

  canvas->endUnclippedSpawnerPainter();

  canvas->setShowConnectorTargets(false);
  canvas->repaint();
}

void SelectTool::endResizing(const QPoint&)
{
  KivioCanvas* canvas = view()->canvasWidget();
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
  m_resizeHandle = 0;
}

/**
 * Shows the popupmenu at a given point.
 */
void SelectTool::showPopupMenu( const QPoint &pos )
{
  KMenu* menu = 0;

  if(view()->activePage()->selectedStencils()->count() < 1) {
    menu = static_cast<KMenu*>(view()->factory()->container("PagePopup", view()));
  } else {
    menu = static_cast<KMenu*>(view()->factory()->container("StencilPopup", view()));
    m_arrowHeadAction->setEnabled(view()->activePage()->checkForStencilTypeInSelection(kstConnector));

    if(view()->activePage()->checkForTextBoxesInSelection()) {
      m_textEditAction->setEnabled(true);
    } else {
      m_textEditAction->setEnabled(false);
    }
  }

  if(menu) {
    m_lastPoint = view()->canvasWidget()->mapFromScreen(pos);
    menu->popup(pos);
  } else {
    kDebug(43000) << "What no popup! *ARGH*!" << endl;
  }
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
  if( view()->activePage()->selectedStencils()->count() <= 0 )
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
      kDebug(43000) << "SelectTool::leftDoubleClick() - unable to locate Text Tool" << endl;
      return;
    }
    
    static_cast<Kivio::MouseTool*>(p)->applyToolAction(stencil, pagePoint);
  }
}

void SelectTool::editText(QPtrList<KivioStencil>* stencils)
{
  // Locate the text tool.  If not found, bail with an error
  Kivio::Plugin *p = view()->pluginManager()->findPlugin("Text Mouse Tool");
  if( !p )
  {
    kDebug(43000) << "SelectTool::leftDoubleClick() - unable to locate Text Tool" << endl;
    return;
  }
  
  // Select the text tool (which makes the text dialog pop up)
  static_cast<Kivio::MouseTool*>(p)->applyToolAction(stencils);
}

void SelectTool::showProperties()
{
  //FIXME: This needs to be implemented ;)
  if(view()->activePage()->selectedStencils()->count() == 0) {
    view()->paperLayoutDlg();
  }
}

void SelectTool::editStencilText()
{
  editText(view()->activePage()->selectedStencils());
}

void SelectTool::keyPress(QKeyEvent* e)
{
  KivioCanvas* canvas = view()->canvasWidget();
  
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
  bool ignoreGridGuides = e->state() & Qt::ShiftButton;
  double distX, distY;
  
  if(ignoreGridGuides || !view()->doc()->grid().isSnap) {
    distX = view()->zoomHandler()->unzoomItX(1);
    distY = view()->zoomHandler()->unzoomItY(1);
  } else {
    distX = gd.freq.width();
    distY = gd.freq.height();
  }
  
  switch(e->key()) {
    case Qt::Key_Left:
      continueDragging(canvas->mapToScreen(KoPoint(m_selectedRect.x() - distX,
        m_selectedRect.y())), ignoreGridGuides);
      break;
    case Qt::Key_Up:
      continueDragging(canvas->mapToScreen(KoPoint(m_selectedRect.x(),
        m_selectedRect.y() - distY)), ignoreGridGuides);
      break;
    case Qt::Key_Right:
      continueDragging(canvas->mapToScreen(KoPoint(m_selectedRect.x() + distX,
        m_selectedRect.y())), ignoreGridGuides);
      break;
    case Qt::Key_Down:
      continueDragging(canvas->mapToScreen(KoPoint(m_selectedRect.x(),
        m_selectedRect.y() + distY)), ignoreGridGuides);
      break;
    default:
      break;
  }
  
  endDragging(QPoint());
  canvas->guideLines().repaintAfterSnapping();
  canvas->setFocus();
}

#include "tool_select.moc"
