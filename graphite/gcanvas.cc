/* This file is part of the KDE project
   Copyright (C) 2000 Werner Trobin <wtrobin@mandrakesoft.com>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <gcanvas.h>
#include <kdebug.h>

GCanvas::GCanvas(GraphiteView *view, GraphitePart *doc)
    : QScrollView(reinterpret_cast<QWidget*>(view), "GCanvas", WNorthWestGravity),
      m_view(view), m_doc(doc) {

    setFocusPolicy(QWidget::StrongFocus);
    viewport()->setFocusProxy(this);
    setMouseTracking(true);
    viewport()->setMouseTracking(true);
    setFocus();
    setBackgroundMode(NoBackground);
    //resizeContents(2000, 2000);
}

void GCanvas::viewportPaintEvent(QPaintEvent */*e*/) {

    // TODO: 1 - define the region which has to be
    //           repainted. (Don't forget to add the offset!)
    //           call m_doc->preparePainting(zoom)!!!
    //           Create a dbuffer and create a painter on this buffer.
    //       2 - call m_doc->painContent(). This draws
    //           the objects to the buffer. Note: don't
    //           draw active or deleted objects to the buffer
    //       3 - bitBlt the buffer
    //       4 - let the active object draw itself (no problem
    //           when embedded, since we don't have an active
    //           object...
    // - m_doc->paintContent() is responsilbe to traverse
    //   the tree of gobject's and let them paint themselves.
    // - Each object decides if it has to repaint itself
    //   depending on it's position/state. If the transparent
    //   flag is false each object is authorized to use
    //   a double buffer! (Normally we won't use dbuffers
    //   for normal objects, unless there are *real* problems
    //   (flickering,...)
    // - Double buffers are invalidated via: zoomfactor
    //   changes, background changes,...
    /*kdDebug(37001) << "paintEvent: x=" << e->rect().x()
		   << " y=" << e->rect().y()
		   << " width=" << e->rect().width()
		   << " height=" << e->rect().height()
		   << " erased=" << e->erased()
		   << " | contents: width=" << contentsWidth()
		   << " height=" << contentsHeight()
		   << " | visible: width=" << visibleWidth()
		   << " height=" << visibleHeight()
		   << " | x-offset=" << contentsX()
		   << " y-offet=" << contentsY() << endl;*/
}
#include <gcanvas.moc>
