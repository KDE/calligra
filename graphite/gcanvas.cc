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

#include <gcanvas.h>

#include <kdebug.h>

#include <graphiteview.h>
#include <ruler.h>

GCanvas::GCanvas(GraphiteView *view, GraphitePart *doc)
    : QScrollView(view, "GCanvas", Qt::WNorthWestGravity | Qt::WResizeNoErase | Qt::WRepaintNoErase),
      m_doc(doc), m_view(view), m_vertical(0L), m_horizontal(0L) {

    viewport()->setFocusPolicy(QWidget::StrongFocus);
    viewport()->setMouseTracking(true);
    setMouseTracking(true);
    setFocus();
    viewport()->setBackgroundMode(QWidget::PaletteLight);
    installEventFilter(viewport());
    setFrameStyle(QFrame::NoFrame);
    resizeContents(1000,1400);
}

void GCanvas::setRulers(Ruler *hruler, Ruler *vruler) {
    m_horizontal=hruler;
    m_vertical=vruler;
}

void GCanvas::showMousePos(bool show) {
    m_vertical->showMousePos(show);
    m_horizontal->showMousePos(show);
}

void GCanvas::contentsMouseMoveEvent(QMouseEvent *e) {
    m_vertical->setMousePos(e->x()-contentsX(), e->y()-contentsY());
    m_horizontal->setMousePos(e->x()-contentsX(), e->y()-contentsY());
    m_doc->mouseMoveEvent(e, m_view);
}

void GCanvas::viewportPaintEvent(QPaintEvent *e) {

    m_doc->setGlobalZoom(m_view->zoom());
    QPainter p(viewport());
    p.setClipRect(e->rect());
    p.setClipping(true);
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
    kdDebug(37001) << "paintEvent: x=" << e->rect().x()
                   << " y=" << e->rect().y()
                   << " width=" << e->rect().width()
                   << " height=" << e->rect().height()
                   << " erased=" << e->erased()
                   << " | contents: width=" << contentsWidth()
                   << " height=" << contentsHeight()
                   << " | visible: width=" << visibleWidth()
                   << " height=" << visibleHeight()
                   << " | x-offset=" << contentsX()
                   << " y-offet=" << contentsY() << endl;
    m_doc->paintContent(p, e->rect());
    p.end();
}

bool GCanvas::eventFilter(QObject *obj, QEvent *e) {

    if(e->type()==QEvent::Enter)
        showMousePos(true);
    else if(e->type()==QEvent::Leave)
        showMousePos(false);
    return QScrollView::eventFilter(obj, e);
}

#include <gcanvas.moc>
