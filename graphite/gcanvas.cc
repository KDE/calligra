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
    : QScrollView(view, "GCanvas", Qt::WNorthWestGravity | Qt::WResizeNoErase | Qt::WRepaintNoErase),
      m_doc(doc), m_view(view), m_vertical(0L), m_horizontal(0L) {

    viewport()->setFocusPolicy(QWidget::StrongFocus);
    viewport()->setMouseTracking(true);
    setMouseTracking(true);
    setFocus();
    viewport()->setBackgroundMode(QWidget::NoBackground);
    installEventFilter(viewport());
    setFrameStyle(QFrame::NoFrame);
}

void GCanvas::setRulers(KoRuler *vertical, KoRuler *horizontal) {
    m_vertical=vertical;
    m_horizontal=horizontal;
}

void GCanvas::showMousePos(bool pos) {

    if(m_vertical)
        m_vertical->showMousePos(pos);
    if(m_horizontal)
        m_horizontal->showMousePos(pos);
}

void GCanvas::viewportResizeEvent(QResizeEvent */*e*/) {
    //kdDebug() << "GCanvas::viewportResizeEvent() width=" << e->size().width()
    //        << " height=" << e->size().height() << endl;
}

void GCanvas::contentsMouseMoveEvent(QMouseEvent *e) {

    //kdDebug() << "GCanvas::contentsMouseMoveEvent: e->x(): " << e->x() << " e->y(): " << e->y() << endl;
    if(m_vertical)
        m_vertical->setMousePos(e->x(), e->y());
    if(m_horizontal)
        m_horizontal->setMousePos(e->x(), e->y());
    m_doc->mouseMoveEvent(e, m_view);
}

void GCanvas::viewportPaintEvent(QPaintEvent */*e*/) {

    // TODO: 1 - define the region which has to be
    //           repainted and  call m_doc->preparePainting(zoom)!!!
    //       2 - set the clipping region
    //       3 - call m_doc->painContent(). This draws
    //           the objects.
    //       4 - then the ROP has to be changed and the
    //           "selection" (i.e. cursors) have to be drawn
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

bool GCanvas::eventFilter(QObject *obj, QEvent *e) {

    if(e->type()==QEvent::Enter)
        showMousePos(true);
    else if(e->type()==QEvent::Leave)
        showMousePos(false);
    return QScrollView::eventFilter(obj, e);
}

#include <gcanvas.moc>
