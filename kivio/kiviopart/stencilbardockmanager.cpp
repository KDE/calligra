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
#include "kivio_doc.h"
#include "stencilbardockmanager.h"
#include "stencilbarbutton.h"
#include "kotooldockmovemanager.h"
#include "kivio_stackbar.h"

#include <qlayout.h>
#include <qsplitter.h>
#include <qcursor.h>
#include <qvaluelist.h>
#include <kdebug.h>

#include <klocale.h>

StencilBarDockManager::StencilBarDockManager( QWidget* parent, const char* name )
: QWidget(parent,name)
{
  dragButton = 0L;
  dragWidget = 0L;
  m_pView = 0L;
  topLevelDropBar = 0L;
  dragPos = OnDesktop;

  m_pDoc = NULL;

  moveManager = new KoToolDockMoveManager();
  connect(moveManager,SIGNAL(positionChanged()),SLOT(slotMoving()));

  QVBoxLayout* l = new QVBoxLayout(this);
  l->setResizeMode(QLayout::Minimum);

  split1 = new QSplitter(Horizontal,this);
  l->addWidget(split1);

  split2 = new QSplitter(Vertical,split1);
}

StencilBarDockManager::~StencilBarDockManager()
{
  delete moveManager;
}

void StencilBarDockManager::setView( QWidget* view )
{
  m_pView = view;
  view->reparent(split2,QPoint(0,0),true);
}

void StencilBarDockManager::insertStencilSet( QWidget* w, const QString& caption, BarPos pos, QRect r, KivioStackBar* onTopLevelBar )
{
  if ( pos == AutoSelect ) {
    if (m_pBars.at(Left))
      pos = Left;
    else
      if (m_pBars.at(Top))
        pos = Top;
      else
        if (m_pBars.at(Right))
          pos = Right;
        else
          if (m_pBars.at(Bottom))
            pos = Bottom;
          else
            pos = Left;
  }

  KivioStackBar* bar = 0L;

  switch (pos) {
    case OnDesktop:
      bar = new KivioStackBar(0L);
      bar->setCaption(i18n("Stencil Sets"));
      m_pTopLevelBars.append(bar);
      connect(bar,SIGNAL(beginDragPage(DragBarButton*)),SLOT(slotBeginDragPage(DragBarButton*)));
      connect(bar,SIGNAL(finishDragPage(DragBarButton*)),SLOT(slotFinishDragPage(DragBarButton*)));
      connect(bar,SIGNAL(deleteButton(DragBarButton*,QWidget*,KivioStackBar*)), m_pDoc, SLOT(slotDeleteStencilSet(DragBarButton*,QWidget*,KivioStackBar*)));
      if (r.isNull())
        r = QRect(50,50,200,300);
      bar->setGeometry(r);
      break;
    case Left:
    case Top:
    case Right:
    case Bottom:
      bar = m_pBars.at(pos);
      if (!bar) {
        bar = new KivioStackBar((pos==Left || pos==Right)?split1:split2);
        connect(bar,SIGNAL(beginDragPage(DragBarButton*)),SLOT(slotBeginDragPage(DragBarButton*)));
        connect(bar,SIGNAL(finishDragPage(DragBarButton*)),SLOT(slotFinishDragPage(DragBarButton*)));
        connect(bar,SIGNAL(deleteButton(DragBarButton*,QWidget*,KivioStackBar*)), m_pDoc, SLOT(slotDeleteStencilSet(DragBarButton*,QWidget*,KivioStackBar*)));
        m_pBars.insert(pos,bar);

        if (pos==Left) {
          QValueList<int> sizes;
          QValueList<int> newSizes;
          sizes = split1->sizes();
          split1->moveToFirst(bar);
          newSizes << 1; // We want the minimum size of the stencilbar, but we haven't loaded the stencils yet...
          newSizes << sizes[0] - 1;
          if(sizes.count() > 2) {
            newSizes << sizes[1];
          }
          split1->setSizes(newSizes);
        }
        if (pos==Top) {
          split2->moveToFirst(bar);
        }
      }
      break;
    case OnTopLevelBar:
      bar = onTopLevelBar;
      break;
    default:
      break;
  }

  bar->insertPage(w,caption);
  bar->showPage(w);
  bar->show();
}

void StencilBarDockManager::slotBeginDragPage( DragBarButton* w )
{
  topLevelDropBar = 0L;
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

  insertStencilSet(dragWidget,caption,dragPos,moveManager->geometry(),topLevelDropBar);

  // remove KivioStackBar if no more pages
  if (!bar->visiblePage()) {
    int k = m_pBars.findRef(bar);
    if ( k!= -1 ) {
      m_pBars.remove(k);
      m_pBars.insert(k,0L);
    } else {
      k = m_pTopLevelBars.findRef(bar);
      if ( k!= -1 ) {
        m_pTopLevelBars.remove(k);
      }
    }
    delete bar;
  }

  dragButton = 0L;
  dragWidget = 0L;
}


void StencilBarDockManager::slotMoving()
{
  topLevelDropBar = 0L;
  QPoint p = QCursor::pos();

  // check "on stackBar"
  for ( uint k = 0; k<4; k++ ) {
    KivioStackBar* bar = m_pBars.at(k);
    if ( bar ) {
      QRect br(bar->mapToGlobal(QPoint(0,0)),bar->size());
      if ( bar && br.contains(p) ) {
        dragPos = (BarPos)m_pBars.findRef(bar);
        moveManager->movePause(true,false);
        moveManager->setGeometry(bar->mapToGlobal(QPoint(0,0)).x(),
                                 moveManager->y(),
                                 bar->width(),
                                 moveManager->getWidget()->sizeHint().height());
        return;
      }
    }
  }

  // check "on topLevel bar"
  for ( KivioStackBar* bar = m_pTopLevelBars.first(); bar; bar = m_pTopLevelBars.next() ) {
    if ( bar->geometry().contains(p) ) {
      dragPos = OnTopLevelBar;
      topLevelDropBar = bar;
      moveManager->movePause(true,false);
      moveManager->setGeometry(bar->geometry().x(),
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
      m_pBars.insert(k,0L);
    } else {
      k = m_pTopLevelBars.findRef(pBar);
      if ( k!= -1 ) {
        m_pTopLevelBars.remove(k);
      }
    }
    delete pBar;
  }
}

void StencilBarDockManager::setDoc( KivioDoc *p )
{
    if( m_pDoc )
    {
       kdDebug(43000) << "StencilBarDockManager::setDoc() - This should never be called twice - error!!" << endl;
        return;
    }
    m_pDoc = p;

    connect(m_pDoc, SIGNAL(sig_deleteStencilSet(DragBarButton*,QWidget*,KivioStackBar*)), this, SLOT(slotDeleteStencilSet( DragBarButton*,QWidget*,KivioStackBar* )));
}
#include "stencilbardockmanager.moc"
