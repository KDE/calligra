/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000 theKompany.com
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
#include "tooldockmanager.h"
#include "tooldockbase.h"

#include <qapplication.h>
#include <qcursor.h>

ToolDockManager::ToolDockManager( QWidget* parent, const char* name )
: QObject(parent,name)
{
  m_pView = parent;
  m_pView->installEventFilter(this);
  connect(&timer,SIGNAL(timeout()),SLOT(slotTimeOut()));
  timer.start(1000);
}

ToolDockManager::~ToolDockManager()
{
}

void ToolDockManager::slotTimeOut()
{
  QPoint p = m_pView->mapFromGlobal(QCursor::pos());
  for (ToolDockBase* b = tools.first(); b; b = tools.next() )
    b->mouseStatus(b->geometry().contains(p));
}

void ToolDockManager::addToolWindow( ToolDockBase* t )
{
  tools.append(t);
  connect(t,SIGNAL(destroyed()),SLOT(removeToolWindow()));
}

void ToolDockManager::removeToolWindow( ToolDockBase* t )
{
  tools.removeRef(t);
}

void ToolDockManager::removeToolWindow()
{
  removeToolWindow((ToolDockBase*)sender());
}

bool ToolDockManager::eventFilter( QObject* obj, QEvent* ev )
{
  if (obj == m_pView && ev->type() == QEvent::Resize ) {
    for (ToolDockBase* b = tools.first(); b; b = tools.next() ) {
      QRect g = b->geometry();
      if (b->snaps.get(ToolDockRight)==m_pView) {
        if (b->snaps.get(ToolDockLeft)==m_pView) {
          g.setWidth(m_pView->width());
        } else {
          g.moveTopLeft(QPoint(m_pView->width()-b->width(),g.y()));
        }
      }
      if (b->snaps.get(ToolDockBottom)==m_pView) {
        if (b->snaps.get(ToolDockTop)==m_pView) {
          g.setHeight(m_pView->height());
        } else {
          g.moveTopLeft(QPoint(g.x(),m_pView->height()-b->height()));
        }
      }
      if (b->geometry()!=g)
        b->setGeometry(g);
    }
    return false;
  }

  return false;
}

ToolDockBase* ToolDockManager::createToolDock( QWidget* view, const QString& caption , const char* name )
{
  ToolDockBase* b = new ToolDockBase(m_pView,caption,name);
  b->setView(view);
  addToolWindow(b);
  return b;
}
#include "tooldockmanager.moc"
