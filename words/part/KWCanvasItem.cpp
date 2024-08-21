/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2010 Boudewijn Rempt <boud@valdyas.org>
 * SPDX-FileCopyrightText: 2002-2006 David Faure <faure@kde.org>
 * SPDX-FileCopyrightText: 2005-2006 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

// words includes
#include "KWCanvasItem.h"
#include "KWGui.h"
#include "KWPage.h"
#include "KWViewMode.h"

// calligra libs includes
#include <KoCanvasController.h>
#include <KoGridData.h>
#include <KoPointerEvent.h>
#include <KoShape.h>
#include <KoToolProxy.h>
#include <KoZoomHandler.h>

// Qt includes
#include <QBrush>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOptionGraphicsItem>

KWCanvasItem::KWCanvasItem(const QString &viewMode, KWDocument *document)
    : QGraphicsWidget(nullptr)
    , KWCanvasBase(document, this)
{
    setAttribute(Qt::WA_OpaquePaintEvent, true);
    setFocusPolicy(Qt::StrongFocus);
    connect(document, &KWDocument::pageSetupChanged, this, &KWCanvasItem::pageSetupChanged);
    m_viewConverter = new KoZoomHandler();
    m_viewMode = KWViewMode::create(viewMode, document);
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
    resourceManager()->setResource(Words::CurrentPageCount, m_document->pageCount());
    Q_EMIT documentSize(m_viewMode->contentsSize());
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
    m_toolProxy->mouseMoveEvent(&me, m_viewMode->viewToDocument(e->pos() + m_documentOffset, m_viewConverter));
    e->setAccepted(me.isAccepted());
}

void KWCanvasItem::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    QMouseEvent me(e->type(), e->pos().toPoint(), e->button(), e->buttons(), e->modifiers());
    m_toolProxy->mousePressEvent(&me, m_viewMode->viewToDocument(e->pos() + m_documentOffset, m_viewConverter));
    if (!me.isAccepted() && me.button() == Qt::RightButton) {
        // XXX: Port to graphicsitem!
        // m_view->popupContextMenu(e->globalPos(), m_toolProxy->popupActionList());
        me.setAccepted(true);
    }
    e->setAccepted(me.isAccepted());
}

void KWCanvasItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
    QMouseEvent me(e->type(), e->pos().toPoint(), e->button(), e->buttons(), e->modifiers());
    m_toolProxy->mouseReleaseEvent(&me, m_viewMode->viewToDocument(e->pos() + m_documentOffset, m_viewConverter));
    e->setAccepted(me.isAccepted());
}

void KWCanvasItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *e)
{
    QMouseEvent me(e->type(), e->pos().toPoint(), e->button(), e->buttons(), e->modifiers());
    m_toolProxy->mouseDoubleClickEvent(&me, m_viewMode->viewToDocument(e->pos() + m_documentOffset, m_viewConverter));
    e->setAccepted(me.isAccepted());
}

void KWCanvasItem::keyPressEvent(QKeyEvent *e)
{
    m_toolProxy->keyPressEvent(e);
    if (!e->isAccepted()) {
        if (e->key() == Qt::Key_Backtab || (e->key() == Qt::Key_Tab && (e->modifiers() & Qt::ShiftModifier)))
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
    m_toolProxy->keyReleaseEvent(e);
}

void KWCanvasItem::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    QWheelEvent ev(event->pos().toPoint(),
                   event->screenPos(),
                   event->pixelDelta(),
                   event->pixelDelta(),
                   event->buttons(),
                   event->modifiers(),
                   event->phase(),
                   event->isInverted());
    m_toolProxy->wheelEvent(&ev, m_viewMode->viewToDocument(event->pos() + m_documentOffset, m_viewConverter));
    event->setAccepted(ev.isAccepted());
}

void KWCanvasItem::inputMethodEvent(QInputMethodEvent *event)
{
    m_toolProxy->inputMethodEvent(event);
}

void KWCanvasItem::updateInputMethodInfo()
{
    updateMicroFocus();
}

void KWCanvasItem::updateCanvas(const QRectF &rc)
{
    Q_UNUSED(rc)
    Q_EMIT canvasUpdated();
}

void KWCanvasItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    painter->fillRect(option->exposedRect, QColor::fromRgb(232, 233, 234));
    KWCanvasBase::paint(*painter, option->exposedRect);
}

void KWCanvasItem::setCursor(const QCursor &cursor)
{
    QGraphicsWidget::setCursor(cursor);
}
