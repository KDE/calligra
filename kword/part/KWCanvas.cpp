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

// koffice libs includes
#include "KoShapeManager.h"
#include "KoPointerEvent.h"
#include "KoTool.h"

// KDE + Qt includes
#include "kdebug.h"
#include <QBrush>
#include <QPainter>
#include <QSize>

// #define DEBUG_REPAINT


KWCanvas::KWCanvas(const QString& viewMode, KWDocument *document, KWView *view, KWGui *parent )
    : QWidget(parent),
    m_document( document ),
    m_viewMode(0)
{
    m_tool = 0;
    m_view = view;
    m_shapeManager = new KoShapeManager(this);
    m_viewMode = KWViewMode::create(viewMode, this);
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking( true );

    connect(document, SIGNAL(pageAdded(KWPage*)), this, SLOT(pageSetupChanged()));
    connect(document, SIGNAL(pageRemoved(KWPage*)), this, SLOT(pageSetupChanged()));
}

void KWCanvas::pageSetupChanged() {
    m_viewMode->pageSetupChanged();
    updateSize();
}

KWCanvas::~KWCanvas()
{
    delete m_shapeManager;
    m_shapeManager = 0;
}

void KWCanvas::updateSize() {
    QSize size = m_viewMode->contentsSize();
    setMinimumSize(size.width(), size.height());
}

void KWCanvas::gridSize(double *horizontal, double *vertical) const {
    *horizontal = m_document->gridX();
    *vertical = m_document->gridY();
}

bool KWCanvas::snapToGrid() const {
    return m_document->snapToGrid();
}

void KWCanvas::addCommand(KCommand *command, bool execute) {
    m_document->addCommand(command, execute);
}

void KWCanvas::updateCanvas(const QRectF& rc) {
    QRectF zoomedRect = m_viewMode->documentToView(rc);
    QList<KWViewMode::ViewMap> map = m_viewMode->clipRectToDocument(zoomedRect.toRect());
    foreach(KWViewMode::ViewMap vm, map) {
        vm.clipRect.adjust(-2, -2, 2, 2); // grow for anti-aliasing
        QRect finalClip((int)(vm.clipRect.x() + vm.distance.x()),
                (int)(vm.clipRect.y() + vm.distance.y()),
                vm.clipRect.width(), vm.clipRect.height());
        update(finalClip);
    }
}

KoViewConverter *KWCanvas::viewConverter() {
    return m_view->viewConverter();
}

void KWCanvas::mouseMoveEvent(QMouseEvent *e) {
    KoPointerEvent ev(e, QPointF( m_viewMode->viewToDocument(e->pos()) ));

    m_tool->mouseMoveEvent( &ev );
}

void KWCanvas::mousePressEvent(QMouseEvent *e) {
    KoPointerEvent ev(e, QPointF( m_viewMode->viewToDocument(e->pos()) ));

    m_tool->mousePressEvent( &ev );
}

void KWCanvas::mouseReleaseEvent(QMouseEvent *e) {
    KoPointerEvent ev(e, QPointF( m_viewMode->viewToDocument(e->pos()) ));

    m_tool->mouseReleaseEvent( &ev );
}

void KWCanvas::keyPressEvent( QKeyEvent *e ) {
    m_tool->keyPressEvent(e);
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
    m_tool->keyReleaseEvent(e);
}

#ifdef DEBUG_REPAINT
# include <stdlib.h>
#endif
void KWCanvas::paintEvent(QPaintEvent * ev) {
    QPainter painter( this );

    if(m_viewMode->hasPages()) {
        QList<KWViewMode::ViewMap> map = m_viewMode->clipRectToDocument(ev->rect());
        foreach(KWViewMode::ViewMap vm, map) {
            painter.save();
            painter.translate(vm.distance.x(), vm.distance.y());
            painter.setClipRect(vm.clipRect);
            QColor color = Qt::white; // TODO paper background
#ifdef DEBUG_REPAINT
    color = QColor(random()%255, random()%255, random()%255);
#endif
            painter.fillRect(vm.clipRect, QBrush(color));
            painter.setRenderHint(QPainter::Antialiasing);
            m_shapeManager->paint( painter, *(viewConverter()), false );
            m_tool->paint( painter, *(viewConverter()) );
            painter.restore();
        }
    }
    else {
        // TODO paint the main-text-flake directly
        kWarning() << "Non-page paiting not implemented yet!\n";
    }

    painter.end();
}

#include "KWCanvas.moc"
