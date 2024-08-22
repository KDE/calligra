/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2002-2006 David Faure <faure@kde.org>
 * SPDX-FileCopyrightText: 2005-2006 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2009 Inge Wallin <inge@lysator.liu.se>
 * SPDX-FileCopyrightText: 2010 Boudewijn Rempt <boud@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

// Own
#include "KWCanvas.h"

// words includes
#include "KWGui.h"
#include "KWPage.h"
#include "KWView.h"
#include "KWViewMode.h"

// calligra libs includes
#include <KoAnnotationLayoutManager.h>
#include <KoCanvasController.h>
#include <KoGridData.h>
#include <KoPointerEvent.h>
#include <KoToolProxy.h>

// Qt includes
#include <QBrush>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QPainterPath>

KWCanvas::KWCanvas(const QString &viewMode, KWDocument *document, KWView *view, KWGui *parent)
    : QWidget(parent)
    , KWCanvasBase(document, this)
    , m_view(view)
{
    setAttribute(Qt::WA_OpaquePaintEvent, true);
    setAttribute(Qt::WA_InputMethodEnabled, true);
    setFocusPolicy(Qt::StrongFocus);
    connect(document, &KWDocument::pageSetupChanged, this, &KWCanvas::pageSetupChanged);

    auto effect = new QGraphicsDropShadowEffect(this);
    effect->setBlurRadius(8);
    effect->setOffset(0);
    effect->setColor(QColor(0, 0, 0, 255));
    setGraphicsEffect(effect);
    if (m_view != nullptr) {
        m_viewConverter = m_view->viewConverter();
    }
    m_viewMode = KWViewMode::create(viewMode, document);
}

KWCanvas::~KWCanvas() = default;

void KWCanvas::pageSetupChanged()
{
    m_viewMode->pageSetupChanged();
    updateSize();
}

void KWCanvas::updateSize()
{
    resourceManager()->setResource(Words::CurrentPageCount, m_document->pageCount());
    QSizeF canvasSize = m_viewMode->contentsSize();
    if (showAnnotations()) {
        canvasSize += QSize(AnnotationAreaWidth, 0.0);
    }
    Q_EMIT documentSize(canvasSize);
}

void KWCanvas::setDocumentOffset(const QPoint &offset)
{
    m_documentOffset = offset;
}

bool KWCanvas::snapToGrid() const
{
    return m_view->snapToGrid();
}

QPointF KWCanvas::viewToDocument(const QPointF &viewPoint) const
{
    return m_viewMode->viewToDocument(viewPoint, m_viewConverter);
}

void KWCanvas::contextMenuEvent(QContextMenuEvent *e)
{
    m_view->popupContextMenu(e->globalPos(), m_toolProxy->popupActionList());
    e->setAccepted(true);
}

void KWCanvas::mouseMoveEvent(QMouseEvent *e)
{
    m_view->viewMouseMoveEvent(e);
    m_toolProxy->mouseMoveEvent(e, m_viewMode->viewToDocument(e->pos() + m_documentOffset, m_viewConverter));
}

void KWCanvas::mousePressEvent(QMouseEvent *e)
{
    m_toolProxy->mousePressEvent(e, m_viewMode->viewToDocument(e->pos() + m_documentOffset, m_viewConverter));
}

void KWCanvas::mouseReleaseEvent(QMouseEvent *e)
{
    m_toolProxy->mouseReleaseEvent(e, m_viewMode->viewToDocument(e->pos() + m_documentOffset, m_viewConverter));
}

void KWCanvas::mouseDoubleClickEvent(QMouseEvent *e)
{
    m_toolProxy->mouseDoubleClickEvent(e, m_viewMode->viewToDocument(e->pos() + m_documentOffset, m_viewConverter));
}

bool KWCanvas::event(QEvent *e)
{
    if (toolProxy()) {
        toolProxy()->processEvent(e);
    }
    return QWidget::event(e);
}

void KWCanvas::keyPressEvent(QKeyEvent *e)
{
    m_toolProxy->keyPressEvent(e);
    if (!e->isAccepted()) {
        if (e->key() == Qt::Key_Backtab || (e->key() == Qt::Key_Tab && (e->modifiers() & Qt::ShiftModifier)))
            focusNextPrevChild(false);
        else if (e->key() == Qt::Key_Tab)
            focusNextPrevChild(true);
        else if (e->key() == Qt::Key_PageUp)
            m_view->goToPreviousPage(e->modifiers());
        else if (e->key() == Qt::Key_PageDown)
            m_view->goToNextPage(e->modifiers());
    }
    if (e->key() == Qt::Key_Escape)
        m_view->exitFullscreenMode();
}

QVariant KWCanvas::inputMethodQuery(Qt::InputMethodQuery query) const
{
    if (query == Qt::ImCursorRectangle) {
        // We may get a query after canvasController() has been deleted.
        // See ~KoCanvasControllerWidget()
        const auto controller = canvasController();
        if (!controller) {
            return QVariant();
        }
        QRectF rect = (m_toolProxy->inputMethodQuery(query, *(viewConverter())).toRectF()).toRect();
        rect = m_viewMode->documentToView(viewConverter()->viewToDocument(rect), viewConverter());
        QPointF scroll(controller->scrollBarValue());
        if (canvasWidget()->layoutDirection() == Qt::RightToLeft) {
            scroll.setX(-scroll.x());
        }
        rect.translate(documentOrigin() - scroll);
        return rect.toRect();
    }
    return m_toolProxy->inputMethodQuery(query, *(viewConverter()));
}

void KWCanvas::keyReleaseEvent(QKeyEvent *e)
{
    m_toolProxy->keyReleaseEvent(e);
}

void KWCanvas::tabletEvent(QTabletEvent *e)
{
    m_toolProxy->tabletEvent(e, m_viewMode->viewToDocument(e->pos() + m_documentOffset, m_viewConverter));
}

void KWCanvas::wheelEvent(QWheelEvent *e)
{
    m_toolProxy->wheelEvent(e, m_viewMode->viewToDocument(e->position() + m_documentOffset, m_viewConverter));
}

void KWCanvas::inputMethodEvent(QInputMethodEvent *event)
{
    m_toolProxy->inputMethodEvent(event);
}

void KWCanvas::paintEvent(QPaintEvent *ev)
{
    QPainter painter(this);
    painter.eraseRect(ev->rect());
    paint(painter, ev->rect()); // In KWCanvasBase

    painter.end();
}

void KWCanvas::setCursor(const QCursor &cursor)
{
    QWidget::setCursor(cursor);
}

void KWCanvas::updateInputMethodInfo()
{
    updateMicroFocus();
}
