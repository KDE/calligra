/* This file is part of the KDE project
   Copyright (C) 2000 Werner Trobin <trobin@kde.org>

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

#include <koRuler.h>

#include <gcanvas.h>
#include <graphiteview.h>
#include <kdebug.h>


GCanvas::GCanvas(GraphiteView *view, GraphitePart *doc)
    : QScrollView(view, "GCanvas", WNorthWestGravity),
      m_vertical(0L), m_horizontal(0L) {

    m_widget=new GCanvasWidget(this, view, doc);
    addChild(m_widget);
}

void GCanvas::setRulers(KoRuler *vertical, KoRuler *horizontal) {
    m_vertical=vertical;
    m_horizontal=horizontal;
}

void GCanvas::updateMousePos(QMouseEvent *e) {
    m_vertical->setMousePos(e->x()-contentsX(), e->y()-contentsY());
    m_horizontal->setMousePos(e->x()-contentsX(), e->y()-contentsY());
}

void GCanvas::showMousePos(const bool &pos) {
    m_vertical->showMousePos(pos);
    m_horizontal->showMousePos(pos);
}

void GCanvas::resizeEvent(QResizeEvent *e) {
    // for now
    //kdDebug() << "GCanvas::resizeEvent(): width=" << e->size().width()
    //	      << " height=" << e->size().height() << endl;
    m_widget->resize(e->size().width()-4, e->size().height()-4);
}

void GCanvas::viewportResizeEvent(QResizeEvent */*e*/) {
    //kdDebug() << "GCanvas::viewportResizeEvent() width=" << e->size().width()
    //	      << " height=" << e->size().height() << endl;
}


GCanvasWidget::GCanvasWidget(GCanvas *canvas, GraphiteView *view,
			     GraphitePart *doc) :
    QWidget(canvas->viewport(), 0L, WNorthWestGravity), m_view(view),
    m_doc(doc), m_canvas(canvas), m_updateRulers(false) {

    setFocusPolicy(QWidget::StrongFocus);
    setMouseTracking(true);
    setFocus();
    setBackgroundMode(NoBackground);
}

void GCanvasWidget::mouseMoveEvent(QMouseEvent *e) {

    if(m_updateRulers)
	m_canvas->updateMousePos(e);
    m_doc->mouseMoveEvent(e, m_view);
}

void GCanvasWidget::paintEvent(QPaintEvent */*e*/) {

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

void GCanvasWidget::leaveEvent(QEvent *) {
    m_updateRulers=false;
    m_canvas->showMousePos(false);
    kdDebug(37001) << "leaving Canvas" << endl;
}

void GCanvasWidget::enterEvent(QEvent *) {
    m_updateRulers=true;
    m_canvas->showMousePos(true);
    kdDebug(37001) << "entering Canvas" << endl;
}
#include <gcanvas.moc>
