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

// Uncommenting the #define will result in some debug output and a green clipping rect
#define GRAPHITE_DEBUG_PAINTING 1

GCanvas::GCanvas(GraphiteView *view, GraphitePart *doc)
    : QScrollView(view, "GCanvas", Qt::WNorthWestGravity | Qt::WResizeNoErase | Qt::WRepaintNoErase),
      m_doc(doc), m_view(view), m_vertical(0L), m_horizontal(0L), m_eraseWidth(0), m_eraseHeight(0),
      m_haveFocus(false), m_gotFocus(false) {

    viewport()->setFocusPolicy(QWidget::StrongFocus);
    viewport()->setMouseTracking(true);
    setMouseTracking(true);
    setFocus();
    viewport()->setBackgroundMode(QWidget::PaletteLight);
    installEventFilter(viewport());
    setFrameStyle(QFrame::NoFrame);
    resizeContentsMM(m_doc->pageLayout().width(),m_doc->pageLayout().height());
    connect(this, SIGNAL(contentsMoving(int, int)), this, SLOT(contentsMoving(int, int)));
}

void GCanvas::setRulers(Ruler *hruler, Ruler *vruler) {
    m_horizontal=hruler;
    m_vertical=vruler;
}

void GCanvas::showMousePos(bool show) {
    m_vertical->showMousePos(show);
    m_horizontal->showMousePos(show);
}

void GCanvas::resizeContentsMM(const double &x, const double &y) {
    GraphiteGlobal::self()->setZoom(m_view->zoom());
    double zoomedRes=GraphiteGlobal::self()->zoomedResolution();
    resizeContents(Graphite::double2Int(x*zoomedRes),
                   Graphite::double2Int(y*zoomedRes));
}

void GCanvas::contentsMouseMoveEvent(QMouseEvent *e) {
    m_vertical->setMousePos(e->x()-contentsX(), e->y()-contentsY());
    m_horizontal->setMousePos(e->x()-contentsX(), e->y()-contentsY());
    m_doc->mouseMoveEvent(e, m_view, m_gotFocus);
    m_gotFocus=false;
}

void GCanvas::viewportPaintEvent(QPaintEvent *e) {

    m_doc->setGlobalZoom(m_view->zoom());
    // If the contents have been moved these two members are !=0 and we
    // have to erase this rect. Note: 0,0,-2,200 deletes a rect at the right border, e.g.
    if(m_eraseWidth!=0 && m_eraseHeight!=0) {
#ifdef GRAPHITE_DEBUG_PAINTING
        kdDebug(37001) << "paintEvent: m_eraseWidth=" << m_eraseWidth << " m_eraseHeight=" << m_eraseHeight << endl;
#endif
        viewport()->erase(0, 0, m_eraseWidth, m_eraseHeight);
        m_eraseWidth=0;
        m_eraseHeight=0;
    }
    QPainter p(viewport());
#ifdef GRAPHITE_DEBUG_PAINTING
    p.setPen(Qt::green);
    p.drawRect(e->rect());
    p.setPen(Qt::black);
#endif // GRAPHITE_DEBUG_PAINTING
    p.setBrushOrigin(-contentsX(), -contentsY());
    p.setClipRect(e->rect());
    p.setClipping(true);
    // ###   1 - define the region which has to be
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
#ifdef GRAPHITE_DEBUG_PAINTING
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
                   << " y-offset=" << contentsY() << endl;
#endif // GRAPHITE_DEBUG_PAINTING
    p.translate(-contentsX(), -contentsY());
    QRect r(e->rect().left()+contentsX(), e->rect().top()+contentsY(),
            e->rect().width(), e->rect().height());
    m_doc->paintPageBorders(p, r);
    m_doc->paintContent(p, r);
    if(m_haveFocus)
        m_doc->paintSelection(p, r, m_view);
    p.end();
}

bool GCanvas::eventFilter(QObject *obj, QEvent *e) {

    if(e->type()==QEvent::Enter)
        showMousePos(true);
    else if(e->type()==QEvent::Leave)
        showMousePos(false);
    else if(e->type()==QEvent::FocusIn)
        m_haveFocus=m_gotFocus=true;
    else if(e->type()==QEvent::FocusOut)
        m_haveFocus=m_gotFocus=false;
    return QScrollView::eventFilter(obj, e);
}

void GCanvas::contentsMoving(int x, int y) {

    static int oldX=0;
    static int oldY=0;
    m_eraseWidth=oldX-x;
    m_eraseHeight=oldY-y;

    if(m_eraseHeight!=0 && m_eraseWidth!=0) {
        m_eraseHeight=0;
        m_eraseWidth=0;
    }
    else if(m_eraseWidth==0)
        m_eraseWidth=visibleWidth();
    else if(m_eraseHeight==0)
        m_eraseHeight=visibleHeight();
    oldX=x;
    oldY=y;
}

#include <gcanvas.moc>
