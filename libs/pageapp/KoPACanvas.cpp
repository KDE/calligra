/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2007 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoPACanvas.h"

#include <KoCanvasController.h>
#include <KoPageLayout.h>
#include <KoToolProxy.h>
#include <KoZoomHandler.h>

#include "KoPADocument.h"
#include "KoPAView.h"

#include <kxmlguifactory.h>

#include <QGraphicsDropShadowEffect>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>

KoPACanvas::KoPACanvas(KoPAViewBase *view, KoPADocument *doc, QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
    , KoPACanvasBase(doc)
{
    setView(view);
    setFocusPolicy(Qt::StrongFocus);
    auto effect = new QGraphicsDropShadowEffect(this);
    effect->setBlurRadius(8);
    effect->setOffset(0);
    effect->setColor(QColor(0, 0, 0, 255));
    setGraphicsEffect(effect);
    // this is much faster than painting it in the paintevent
    setAutoFillBackground(true);
    updateSize();
    setAttribute(Qt::WA_InputMethodEnabled, true);
}

void KoPACanvas::repaint()
{
    update();
}

QWidget *KoPACanvas::canvasWidget()
{
    return this;
}

const QWidget *KoPACanvas::canvasWidget() const
{
    return this;
}

void KoPACanvas::updateSize()
{
    QSize size;

    if (koPAView()->activePage()) {
        KoPageLayout pageLayout = koPAView()->viewMode()->activePageLayout();
        size.setWidth(qRound(koPAView()->zoomHandler()->zoomItX(pageLayout.width)));
        size.setHeight(qRound(koPAView()->zoomHandler()->zoomItX(pageLayout.height)));
    }

    Q_EMIT documentSize(size);
}

void KoPACanvas::updateCanvas(const QRectF &rc)
{
    QRect clipRect(viewToWidget(viewConverter()->documentToView(rc).toRect()));
    clipRect.adjust(-2, -2, 2, 2); // Resize to fit anti-aliasing
    clipRect.moveTopLeft(clipRect.topLeft() - documentOffset());
    update(clipRect);

    Q_EMIT canvasUpdated();
}

bool KoPACanvas::event(QEvent *e)
{
    if (toolProxy()) {
        if (e->type() == QEvent::TouchBegin || e->type() == QEvent::TouchUpdate || e->type() == QEvent::TouchEnd) {
            toolProxy()->touchEvent(dynamic_cast<QTouchEvent *>(e));
        }
        toolProxy()->processEvent(e);
    }
    return QWidget::event(e);
}

void KoPACanvas::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    paint(painter, event->rect());
    painter.end();
}

void KoPACanvas::tabletEvent(QTabletEvent *event)
{
    koPAView()->viewMode()->tabletEvent(event, viewConverter()->viewToDocument(widgetToView(event->pos() + documentOffset())));
}

void KoPACanvas::mousePressEvent(QMouseEvent *event)
{
    koPAView()->viewMode()->mousePressEvent(event, viewConverter()->viewToDocument(widgetToView(event->pos() + documentOffset())));

    if (!event->isAccepted() && event->button() == Qt::RightButton) {
        showContextMenu(event->globalPos(), toolProxy()->popupActionList());
    }

    event->setAccepted(true);
}

void KoPACanvas::mouseDoubleClickEvent(QMouseEvent *event)
{
    koPAView()->viewMode()->mouseDoubleClickEvent(event, viewConverter()->viewToDocument(widgetToView(event->pos() + documentOffset())));
}

void KoPACanvas::mouseMoveEvent(QMouseEvent *event)
{
    koPAView()->viewMode()->mouseMoveEvent(event, viewConverter()->viewToDocument(widgetToView(event->pos() + documentOffset())));
}

void KoPACanvas::mouseReleaseEvent(QMouseEvent *event)
{
    koPAView()->viewMode()->mouseReleaseEvent(event, viewConverter()->viewToDocument(widgetToView(event->pos() + documentOffset())));
}

void KoPACanvas::keyPressEvent(QKeyEvent *event)
{
    koPAView()->viewMode()->keyPressEvent(event);
    if (!event->isAccepted()) {
        if (event->key() == Qt::Key_Backtab || (event->key() == Qt::Key_Tab && (event->modifiers() & Qt::ShiftModifier)))
            focusNextPrevChild(false);
        else if (event->key() == Qt::Key_Tab)
            focusNextPrevChild(true);
    }
}

void KoPACanvas::keyReleaseEvent(QKeyEvent *event)
{
    koPAView()->viewMode()->keyReleaseEvent(event);
}

void KoPACanvas::wheelEvent(QWheelEvent *event)
{
    koPAView()->viewMode()->wheelEvent(event, viewConverter()->viewToDocument(widgetToView(event->position().toPoint() + documentOffset())));
}

void KoPACanvas::closeEvent(QCloseEvent *event)
{
    koPAView()->viewMode()->closeEvent(event);
}

void KoPACanvas::updateInputMethodInfo()
{
    updateMicroFocus();
}

QVariant KoPACanvas::inputMethodQuery(Qt::InputMethodQuery query) const
{
    if (query == Qt::ImCursorRectangle) {
        // We may get a query after canvasController() has been deleted.
        // See ~KoCanvasControllerWidget()
        const auto controller = canvasController();
        if (!controller) {
            return QVariant();
        }
        QRectF rect = (toolProxy()->inputMethodQuery(query, *(viewConverter())).toRectF()).toRect();
        QPointF scroll(controller->scrollBarValue());
        if (controller->canvasMode() == KoCanvasController::Spreadsheet && canvasWidget()->layoutDirection() == Qt::RightToLeft) {
            scroll.setX(-scroll.x());
        }
        rect.translate(documentOrigin() - scroll);
        return rect.toRect();
    }
    return toolProxy()->inputMethodQuery(query, *(viewConverter()));
}

void KoPACanvas::inputMethodEvent(QInputMethodEvent *event)
{
    toolProxy()->inputMethodEvent(event);
}

void KoPACanvas::resizeEvent(QResizeEvent *event)
{
    Q_EMIT sizeChanged(event->size());
}

void KoPACanvas::showContextMenu(const QPoint &globalPos, const QList<QAction *> &actionList)
{
    KoPAView *view = dynamic_cast<KoPAView *>(koPAView());
    if (!view || !view->factory())
        return;

    view->unplugActionList("toolproxy_action_list");
    view->plugActionList("toolproxy_action_list", actionList);

    QMenu *menu = dynamic_cast<QMenu *>(view->factory()->container("default_canvas_popup", view));

    if (menu)
        menu->exec(globalPos);
}

void KoPACanvas::setCursor(const QCursor &cursor)
{
    QWidget::setCursor(cursor);
}
