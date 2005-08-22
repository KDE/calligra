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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "kivio_doc.h"
#include "stencilbardockmanager.h"
#include "stencilbarbutton.h"
#include "kotooldockmovemanager.h"
#include "kivio_stackbar.h"
#include "kivio_view.h"

#include <qcursor.h>
#include <qvaluelist.h>
#include <kdebug.h>

#include <klocale.h>

StencilBarDockManager::StencilBarDockManager( KivioView* parent, const char* name )
: QWidget(parent,name)
{
  dragButton = 0L;
  dragWidget = 0L;
  m_pView = parent;
  m_destinationBar = 0L;
  dragPos = OnDesktop;

  m_pBars.setAutoDelete(true);

  m_pDoc = m_pView->doc();
  connect(m_pDoc, SIGNAL(sig_deleteStencilSet(DragBarButton*, QWidget*, KivioStackBar*)), this,
          SLOT(slotDeleteStencilSet(DragBarButton*, QWidget*, KivioStackBar*)));

  moveManager = new KoToolDockMoveManager();
  connect(moveManager,SIGNAL(positionChanged()),SLOT(slotMoving()));
}

StencilBarDockManager::~StencilBarDockManager()
{
  delete moveManager;
}

void StencilBarDockManager::insertStencilSet( QWidget* w, const QString& caption,
                                              BarPos pos, QRect r, KivioStackBar* destinationBar )
{
  KivioStackBar* bar = 0L;
  
  if(destinationBar) {
    bar = destinationBar;
  } else {
    Qt::Dock position = Qt::DockLeft;

    if ( pos == AutoSelect ) {
      pos = Left;
    }

    switch (pos) {
      case OnDesktop:
        position = Qt::DockTornOff;
        break;
      case Left:
      {
        QPtrList<QDockWindow> dockList = m_pView->mainWindow()->dockWindows(Qt::DockLeft);
        
        for(QDockWindow* dock = dockList.first(); dock; dock = dockList.next()) {
          if(::qt_cast<KivioStackBar*>(dock) && (static_cast<KivioStackBar*>(dock)->view() == m_pView)) {
            bar = static_cast<KivioStackBar*>(dock);
            break;
          }
        }

        position = Qt::DockLeft;
        break;
      }
      case Top:
      {
        QPtrList<QDockWindow> dockList = m_pView->mainWindow()->dockWindows(Qt::DockTop);
        
        for(QDockWindow* dock = dockList.first(); dock; dock = dockList.next()) {
          if(::qt_cast<KivioStackBar*>(dock) && (static_cast<KivioStackBar*>(dock)->view() == m_pView)) {
            bar = static_cast<KivioStackBar*>(dock);
            break;
          }
        }

        position = Qt::DockTop;
        break;
      }
      case Right:
      {
        QPtrList<QDockWindow> dockList = m_pView->mainWindow()->dockWindows(Qt::DockRight);
        
        for(QDockWindow* dock = dockList.first(); dock; dock = dockList.next()) {
          if(::qt_cast<KivioStackBar*>(dock) && (static_cast<KivioStackBar*>(dock)->view() == m_pView)) {
            bar = static_cast<KivioStackBar*>(dock);
            break;
          }
        }

        position = Qt::DockRight;
        break;
      }
      case Bottom:
      {
        QPtrList<QDockWindow> dockList = m_pView->mainWindow()->dockWindows(Qt::DockBottom);
        
        for(QDockWindow* dock = dockList.first(); dock; dock = dockList.next()) {
          if(::qt_cast<KivioStackBar*>(dock) && (static_cast<KivioStackBar*>(dock)->view() == m_pView)) {
            bar = static_cast<KivioStackBar*>(dock);
            break;
          }
        }

        position = Qt::DockBottom;
        break;
      }
      case OnTopLevelBar: // hmm this shouldn't happen :)
        position = Qt::DockTornOff;
        break;
      default:
        break;
    }

    if (!bar) {
      bar = new KivioStackBar(m_pView, m_pView->mainWindow());
      m_pView->mainWindow()->moveDockWindow(bar, position);
      m_pBars.append(bar);
      connect(bar,SIGNAL(beginDragPage(DragBarButton*)),SLOT(slotBeginDragPage(DragBarButton*)));
      connect(bar,SIGNAL(finishDragPage(DragBarButton*)),SLOT(slotFinishDragPage(DragBarButton*)));
      connect(bar,SIGNAL(deleteButton(DragBarButton*,QWidget*,KivioStackBar*)),
              m_pDoc, SLOT(slotDeleteStencilSet(DragBarButton*,QWidget*,KivioStackBar*)));
    }
  }

  bar->insertPage(w,caption);
  bar->showPage(w);
  bar->show();
}

void StencilBarDockManager::slotBeginDragPage( DragBarButton* w )
{
  m_destinationBar = 0L;
  dragButton = w;
  dragWidget = ((KivioStackBar*)sender())->findPage(dragButton);
  dragButton->setUpdatesEnabled(false);
  dragWidget->setUpdatesEnabled(false);
  dragPos = OnDesktop;
  moveManager->doMove(dragButton);
}

void StencilBarDockManager::slotFinishDragPage( DragBarButton* )
{
  moveManager->stop();
  dragButton->setUpdatesEnabled(true);
  dragButton->update();

  dragWidget->setUpdatesEnabled(true);
  dragWidget->update();

  QString caption(dragButton->text());
  KivioStackBar* bar = (KivioStackBar*)sender();
  bar->removePage(dragWidget);

  insertStencilSet(dragWidget,caption,dragPos,moveManager->geometry(),m_destinationBar);

  // remove KivioStackBar if no more pages
  if (!bar->visiblePage()) {
    int k = m_pBars.findRef(bar);

    if ( k!= -1 ) {
      m_pBars.remove(k);
      m_pBars.insert(k,0L);
    }
  }

  dragButton = 0L;
  dragWidget = 0L;
  m_destinationBar = 0L;
}


void StencilBarDockManager::slotMoving()
{
  m_destinationBar = 0L;
  QPoint p = QCursor::pos();
  QPoint globalPos;

  // check existing bars
  for ( KivioStackBar* bar = m_pBars.first(); bar; bar = m_pBars.next() ) {
    if ( bar->geometry().contains(p) ) {
      if(bar->place() == QDockWindow::OutsideDock) {
        dragPos = OnTopLevelBar;
        globalPos = bar->geometry().topLeft();
      } else {
        dragPos = AutoSelect;
        globalPos = bar->mapToGlobal(bar->geometry().topLeft());
      }
      
      m_destinationBar = bar;
      moveManager->movePause(true,false);
      moveManager->setGeometry(globalPos.x(),
                               moveManager->y(),
                               bar->width(),
                               moveManager->getWidget()->sizeHint().height());
      return;
    }
  }

  // check "on mainView"
  QRect mr(m_pView->mapToGlobal(QPoint(0,0)),m_pView->size());

  if ( mr.contains(p) ) {
    QRect r(mr);
    r.setWidth(r.width()/4);
    if ( r.contains(p) && !m_pBars.at(Left) ) {
      moveManager->movePause();
      moveManager->setGeometry(r);
      dragPos = Left;
      return;
    }

    r.moveBy(r.width()*3,0);

    if ( r.contains(p) && !m_pBars.at(Right) ) {
      moveManager->movePause();
      moveManager->setGeometry(r);
      dragPos = Right;
      return;
    }

    QRect t(mr);

    t.setHeight(t.height()/4);
    if ( t.contains(p) && !m_pBars.at(Top) ) {
      moveManager->movePause();
      moveManager->setGeometry(t);
      dragPos = Top;
      return;
    }

    t.moveBy(0,t.height()*3);

    if ( t.contains(p) && !m_pBars.at(Bottom) ) {
      moveManager->movePause();
      moveManager->setGeometry(t);
      dragPos = Bottom;
      return;
    }
  }

  // on desktop
  moveManager->resize(dragWidget->size());
  moveManager->movePause(false,false);
  dragPos = OnDesktop;
}

void StencilBarDockManager::slotDeleteStencilSet( DragBarButton* pBtn, QWidget *, KivioStackBar *pBar )
{
  pBar->deletePageAndButton(pBtn);

  // remove KivioStackBar if no more pages
  if (!pBar->visiblePage()) {
    int k = m_pBars.findRef(pBar);

    if ( k!= -1 ) {
      m_pBars.remove(k);
      pBar = 0;
    } else {
      delete pBar;
      pBar = 0;
    }
  }
}

void StencilBarDockManager::setAllStackBarsShown(bool shown)
{
  for(KivioStackBar* bar = m_pBars.first(); bar; bar = m_pBars.next()) {
    if(shown) {
      bar->show();
    } else {
      bar->hide();
    }
  }
}

#include "stencilbardockmanager.moc"
