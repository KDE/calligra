/* This file is part of the KDE project
 *
 * Copyright (C) 2010 Boudewijn Rempt <boud@valdyas.org>
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
#include "KWCanvasItem.h"
#include "KWGui.h"
#include "KWViewMode.h"
#include "KWPage.h"

// koffice libs includes
#include <KoShapeManager.h>
#include <KoPointerEvent.h>
#include <KoToolManager.h>
#include <KoCanvasController.h>
#include <KoToolProxy.h>
#include <KoGridData.h>
#include <KoShape.h>
#include <KoZoomHandler.h>

// KDE + Qt includes
#include <KDebug>
#include <QBrush>
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include <QStyleOptionGraphicsItem>


KWCanvasItem::KWCanvasItem(const QString &viewMode, KWDocument *document)
        : QGraphicsWidget(0),
        KWCanvasBase(document, this)
{
    setAttribute(Qt::WA_OpaquePaintEvent, true);
    setAttribute(Qt::WA_InputMethodEnabled, true);
    setFocusPolicy(Qt::StrongFocus);
    connect(document, SIGNAL(pageSetupChanged()), this, SLOT(pageSetupChanged()));
    m_viewConverter = new KoZoomHandler();
    m_viewMode = KWViewMode::create(viewMode, document, this);
}

KWCanvasItem::~KWCanvasItem()
{
    delete m_viewConverter;
}

void KWCanvasItem::pageSetupChanged()
{
    m_viewMode->pageSetupChanged();
    updateSize();
}

void KWCanvasItem::updateSize()
{
    resourceManager()->setResource(KWord::CurrentPageCount, m_document->pageCount());
    emit documentSize(m_viewMode->contentsSize());
}

void KWCanvasItem::setDocumentOffset(const QPoint &offset)
{
    m_documentOffset = offset;
}

bool KWCanvasItem::snapToGrid() const
{
    return false;
}

void KWCanvasItem::mouseMoveEvent(QGraphicsSceneMouseEvent *e)
{
    QMouseEvent me(e->type(), e->pos().toPoint(), e->button(), e->buttons(), e->modifiers());
    m_toolProxy->mouseMoveEvent(&me, m_viewMode->viewToDocument(e->pos() + m_documentOffset));
    e->setAccepted(me.isAccepted());
}

void KWCanvasItem::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    QMouseEvent me(e->type(), e->pos().toPoint(), e->button(), e->buttons(), e->modifiers());
    m_toolProxy->mousePressEvent(&me, m_viewMode->viewToDocument(e->pos() + m_documentOffset));
    if (!me.isAccepted() && me.button() == Qt::RightButton) {
        // XXX: Port to graphicsitem!
        //m_view->popupContextMenu(e->globalPos(), m_toolProxy->popupActionList());
        me.setAccepted(true);
    }
    e->setAccepted(me.isAccepted());
}

void KWCanvasItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
    QMouseEvent me(e->type(), e->pos().toPoint(), e->button(), e->buttons(), e->modifiers());
    m_toolProxy->mouseReleaseEvent(&me, m_viewMode->viewToDocument(e->pos() + m_documentOffset));
    e->setAccepted(me.isAccepted());
}

void KWCanvasItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *e)
{
    QMouseEvent me(e->type(), e->pos().toPoint(), e->button(), e->buttons(), e->modifiers());
    m_toolProxy->mouseDoubleClickEvent(&me, m_viewMode->viewToDocument(e->pos() + m_documentOffset));
    e->setAccepted(me.isAccepted());
}

void KWCanvasItem::keyPressEvent(QKeyEvent *e)
{
    m_toolProxy->keyPressEvent(e);
    if (! e->isAccepted()) {
        if (e->key() == Qt::Key_Backtab
                || (e->key() == Qt::Key_Tab && (e->modifiers() & Qt::ShiftModifier)))
            focusNextPrevChild(false);
        else if (e->key() == Qt::Key_Tab)
            focusNextPrevChild(true);
    }
}

QVariant KWCanvasItem::inputMethodQuery(Qt::InputMethodQuery query) const
{
    return m_toolProxy->inputMethodQuery(query, *(viewConverter()));
}

void KWCanvasItem::keyReleaseEvent(QKeyEvent *e)
{
#ifndef NDEBUG
    // Debug keys
    if ((e->modifiers() & (Qt::AltModifier | Qt::ControlModifier | Qt::ShiftModifier))) {
        if (e->key() == Qt::Key_F) {
            document()->printDebug();
            e->accept();
            return;
        }
        if (e->key() == Qt::Key_M) {
            const QDateTime dtMark(QDateTime::currentDateTime());
            kDebug(32001) << "Developer mark:" << dtMark.toString("yyyy-MM-dd hh:mm:ss,zzz");
            e->accept();
            return;
        }
    }
#endif
    m_toolProxy->keyReleaseEvent(e);
}

void KWCanvasItem::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    QWheelEvent ev(event->pos().toPoint(), event->delta(), event->buttons(), event->modifiers(), event->orientation());
    m_toolProxy->wheelEvent(&ev, m_viewMode->viewToDocument(event->pos() + m_documentOffset));
    event->setAccepted(ev.isAccepted());
}

void KWCanvasItem::inputMethodEvent(QInputMethodEvent *event)
{
    m_toolProxy->inputMethodEvent(event);
}

void KWCanvasItem::updateInputMethodInfo()
{
#if QT_VERSION  >= 0x040700
    updateMicroFocus();
#endif
}

void KWCanvasItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    painter->fillRect(option->exposedRect, Qt::darkGray);
    KWCanvasBase::paint(*painter, option->exposedRect);
}

QCursor KWCanvasItem::setCursor(const QCursor &cursor)
{
    QCursor oldCursor = QGraphicsWidget::cursor();
    QGraphicsWidget::setCursor(cursor);
    return oldCursor;
}
