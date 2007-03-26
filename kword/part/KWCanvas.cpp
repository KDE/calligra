/* This file is part of the KDE project
 * Copyright (C) 2002-2006 David Faure <faure@kde.org>
 * Copyright (C) 2005-2006 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

// kword includes
#include "KWCanvas.h"
#include "KWGui.h"
#include "KWView.h"
#include "KWViewMode.h"
#include "KWPage.h"

// koffice libs includes
#include "KoShapeManager.h"
#include "KoPointerEvent.h"
#include "KoToolManager.h"
#include "KoToolProxy.h"

// KDE + Qt includes
#include "kdebug.h"
#include <QBrush>
#include <QPainter>
#include <QSize>
#include <QPainterPath>

// #define DEBUG_REPAINT


KWCanvas::KWCanvas(const QString& viewMode, KWDocument *document, KWView *view, KWGui *parent )
    : QWidget(parent),
    KoCanvasBase( document ),
    m_document( document ),
    m_view(view),
    m_viewMode(0)
{
    m_shapeManager = new KoShapeManager(this);
    m_viewMode = KWViewMode::create(viewMode, this);
    setFocusPolicy(Qt::StrongFocus);

    connect(document, SIGNAL(pageAdded(KWPage*)), this, SLOT(pageSetupChanged()));
    connect(document, SIGNAL(pageRemoved(KWPage*)), this, SLOT(pageSetupChanged()));

    m_toolProxy = new KoToolProxy(this, this);
    setAttribute(Qt::WA_OpaquePaintEvent, true);
}

KWCanvas::~KWCanvas()
{
    delete m_shapeManager;
    m_shapeManager = 0;
    m_toolProxy = 0;
}

void KWCanvas::pageSetupChanged() {
    m_viewMode->pageSetupChanged();
    updateSize();
}

void KWCanvas::updateSize() {
    QSize size = m_viewMode->contentsSize();
    emit documentSize(size);
}

void KWCanvas::setDocumentOffset(const QPoint &offset) {
    m_documentOffset = offset;
}

void KWCanvas::gridSize(double *horizontal, double *vertical) const {
    *horizontal = m_document->gridData().gridX();
    *vertical = m_document->gridData().gridY();
}

bool KWCanvas::snapToGrid() const {
    return m_view->snapToGrid();
}

void KWCanvas::addCommand(QUndoCommand *command) {
    m_document->addCommand(command);
}

void KWCanvas::updateCanvas(const QRectF& rc) {
    QRectF zoomedRect = m_viewMode->documentToView(rc);
    QList<KWViewMode::ViewMap> map = m_viewMode->clipRectToDocument(zoomedRect.toRect());
    foreach(KWViewMode::ViewMap vm, map) {
        vm.clipRect.adjust(-2, -2, 2, 2); // grow for anti-aliasing
        QRect finalClip((int)(vm.clipRect.x() + vm.distance.x() - m_documentOffset.x()),
                (int)(vm.clipRect.y() + vm.distance.y() - m_documentOffset.y()),
                vm.clipRect.width(), vm.clipRect.height());
        update(finalClip);
    }
}

KoViewConverter *KWCanvas::viewConverter() {
    return m_view->viewConverter();
}

void KWCanvas::clipToDocument(const KoShape *shape, QPointF &move) const {
    Q_ASSERT(shape);
    QPointF absPos = shape->absolutePosition();
    double y = qMax(0.1, absPos.y() + move.y());
    KWPage *page = m_document->pageManager()->page(QPointF(absPos.x(), y));
    Q_ASSERT(page);
    QRectF pageRect (page->rect().adjusted(5, 5, -5, -5));
    QRectF movedRect = pageRect;
    QPainterPath path (shape->transformationMatrix(0).map(shape->outline()));

    movedRect.moveLeft(pageRect.x() - move.x());
    if(! path.intersects(movedRect))
        move.setX(0);
    movedRect.moveTopLeft(pageRect.topLeft() - move);
    if(path.intersects(movedRect))
        return;

    move.setY(0);
}

void KWCanvas::mouseMoveEvent(QMouseEvent *e) {
    m_toolProxy->mouseMoveEvent( e, m_viewMode->viewToDocument(e->pos() + m_documentOffset) );
}

void KWCanvas::mousePressEvent(QMouseEvent *e) {
    m_toolProxy->mousePressEvent( e, m_viewMode->viewToDocument(e->pos() + m_documentOffset) );
}

void KWCanvas::mouseReleaseEvent(QMouseEvent *e) {
    m_toolProxy->mouseReleaseEvent( e, m_viewMode->viewToDocument(e->pos() + m_documentOffset) );
}

void KWCanvas::mouseDoubleClickEvent(QMouseEvent *e) {
    m_toolProxy->mouseDoubleClickEvent( e, m_viewMode->viewToDocument(e->pos() + m_documentOffset) );
}

void KWCanvas::keyPressEvent( QKeyEvent *e ) {
    m_toolProxy->keyPressEvent(e);
}

void KWCanvas::keyReleaseEvent (QKeyEvent *e) {
#ifndef NDEBUG
    // Debug keys
    if ( ( e->modifiers() & Qt::ControlModifier ) && ( e->modifiers() & Qt::ShiftModifier ) ) {
        if(e->key() == Qt::Key_F) {
            document()->printDebug();
            e->accept();
            return;
        }
        if(e->key() == Qt::Key_P) {
            //printRTDebug( 0 );
            e->accept();
            return;
        }
        if(e->key() == Qt::Key_V) {
            //printRTDebug( 1 );
            e->accept();
            return;
        }
        if(e->key() == Qt::Key_S) {
            //m_doc->printStyleDebug();
            e->accept();
            return;
        }
        if(e->key() == Qt::Key_M) {
            //const QDateTime dtMark ( QDateTime::currentDateTime() );
            //kDebug(32002) << "Developer mark: " << dtMark.toString("yyyy-MM-dd hh:mm:ss,zzz") << endl;
            e->accept();
            return;
        }
        // For some reason 'T' doesn't work (maybe kxkb)
    }
#endif
    m_toolProxy->keyReleaseEvent(e);
}

void KWCanvas::tabletEvent( QTabletEvent *e )
{
    m_toolProxy->tabletEvent( e, m_viewMode->viewToDocument(e->pos() + m_documentOffset) );
}

void KWCanvas::wheelEvent( QWheelEvent *e )
{
    m_toolProxy->wheelEvent( e, m_viewMode->viewToDocument(e->pos() + m_documentOffset) );
}

bool KWCanvas::event (QEvent *event) {
    // we should forward tabs, and let tools decide if they should be used or ignored.
    // if the tool ignores it, it will move focus.
    if(event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*> (event);
        if(keyEvent->key() == Qt::Key_Backtab)
            return true;
        if(keyEvent->key() == Qt::Key_Tab && event->type() == QEvent::KeyPress) {
            // we loose key-release events, which I think is not an issue.
            keyPressEvent(keyEvent);
            return true;
        }
    }
    return QWidget::event(event);
}

#ifdef DEBUG_REPAINT
# include <stdlib.h>
#endif
void KWCanvas::paintEvent(QPaintEvent * ev) {
    QPainter painter( this );
    painter.translate(-m_documentOffset);
    painter.eraseRect(ev->rect());

    if(m_viewMode->hasPages()) {
        QList<KWViewMode::ViewMap> map = m_viewMode->clipRectToDocument(ev->rect().translated(m_documentOffset));
        foreach(KWViewMode::ViewMap vm, map) {
            painter.save();
            painter.translate(vm.distance.x(), vm.distance.y());
            vm.clipRect = vm.clipRect.adjusted(-1, -1, 1, 1);
            painter.setClipRect(vm.clipRect);
            QColor color = Qt::white; // TODO paper background
#ifdef DEBUG_REPAINT
    color = QColor(random()%255, random()%255, random()%255);
#endif
            painter.fillRect(vm.clipRect, QBrush(color));
            painter.setRenderHint(QPainter::Antialiasing);
            m_shapeManager->paint( painter, *(viewConverter()), false );
            m_toolProxy->paint( painter, *(viewConverter()) );
            painter.restore();
        }
    }
    else {
        // TODO paint the main-text-flake directly
        kWarning() << "Non-page painting not implemented yet!\n";
    }

    painter.end();
}

#include "KWCanvas.moc"
