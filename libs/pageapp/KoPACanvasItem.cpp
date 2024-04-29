/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2007 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2010 Boudewijn Rempt <boud@valdyas.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoPACanvasItem.h"

#include "KoPADocument.h"
#include "KoPAPageBase.h"
#include "KoPAView.h"

#include <KoPageLayout.h>
#include <KoToolProxy.h>
#include <KoZoomHandler.h>

#include <kxmlguifactory.h>

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneResizeEvent>
#include <QGraphicsSceneWheelEvent>
#include <QMenu>
#include <QStyleOptionGraphicsItem>

KoPACanvasItem::KoPACanvasItem(KoPADocument *doc)
    : QGraphicsWidget()
    , KoPACanvasBase(doc)
{
    setFocusPolicy(Qt::StrongFocus);
    // this is much faster than painting it in the paintevent
    setAutoFillBackground(true);
}

void KoPACanvasItem::repaint()
{
    update();
}

void KoPACanvasItem::updateSize()
{
    QSize size;

    if (koPAView()->activePage()) {
        KoPageLayout pageLayout = koPAView()->activePage()->pageLayout();
        size.setWidth(qRound(koPAView()->zoomHandler()->zoomItX(pageLayout.width)));
        size.setHeight(qRound(koPAView()->zoomHandler()->zoomItX(pageLayout.height)));
    }
    Q_EMIT documentSize(size);
}

void KoPACanvasItem::updateCanvas(const QRectF &rc)
{
    QRect clipRect(viewToWidget(viewConverter()->documentToView(rc).toRect()));
    clipRect.adjust(-2, -2, 2, 2); // Resize to fit anti-aliasing
    clipRect.moveTopLeft(clipRect.topLeft() - documentOffset());
    update(clipRect);

    Q_EMIT canvasUpdated();
}

void KoPACanvasItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)
    KoPACanvasBase::paint(*painter, option->exposedRect);
}

bool KoPACanvasItem::event(QEvent *ev)
{
    if (ev->type() == QEvent::ShortcutOverride) {
        QKeyEvent *kev = static_cast<QKeyEvent *>(ev);
        koPAView()->viewMode()->shortcutOverrideEvent(kev);
    }
    return QGraphicsWidget::event(ev);
}

void KoPACanvasItem::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    QMouseEvent me(e->type(), e->pos().toPoint(), e->button(), e->buttons(), e->modifiers());
    Q_ASSERT(koPAView());
    Q_ASSERT(koPAView()->viewMode());
    Q_ASSERT(viewConverter());
    koPAView()->viewMode()->mousePressEvent(&me, viewConverter()->viewToDocument(widgetToView(me.pos() + documentOffset())));

    if (!me.isAccepted() && me.button() == Qt::RightButton) {
        showContextMenu(me.pos(), toolProxy()->popupActionList());
        e->setAccepted(true);
    }
}

void KoPACanvasItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *e)
{
    QMouseEvent me(e->type(), e->pos().toPoint(), e->button(), e->buttons(), e->modifiers());
    koPAView()->viewMode()->mouseDoubleClickEvent(&me, viewConverter()->viewToDocument(widgetToView(me.pos() + documentOffset())));
}

void KoPACanvasItem::mouseMoveEvent(QGraphicsSceneMouseEvent *e)
{
    QMouseEvent me(e->type(), e->pos().toPoint(), e->button(), e->buttons(), e->modifiers());
    koPAView()->viewMode()->mouseMoveEvent(&me, viewConverter()->viewToDocument(widgetToView(me.pos() + documentOffset())));
}

void KoPACanvasItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
    QMouseEvent me(e->type(), e->pos().toPoint(), e->button(), e->buttons(), e->modifiers());
    koPAView()->viewMode()->mouseReleaseEvent(&me, viewConverter()->viewToDocument(widgetToView(me.pos() + documentOffset())));
}

void KoPACanvasItem::keyPressEvent(QKeyEvent *event)
{
    koPAView()->viewMode()->keyPressEvent(event);
    if (!event->isAccepted()) {
        if (event->key() == Qt::Key_Backtab || (event->key() == Qt::Key_Tab && (event->modifiers() & Qt::ShiftModifier)))
            focusNextPrevChild(false);
        else if (event->key() == Qt::Key_Tab)
            focusNextPrevChild(true);
    }
}

void KoPACanvasItem::keyReleaseEvent(QKeyEvent *event)
{
    koPAView()->viewMode()->keyReleaseEvent(event);
}

void KoPACanvasItem::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    QWheelEvent ev(event->pos().toPoint(),
                   event->screenPos(),
                   event->pixelDelta(),
                   event->pixelDelta(),
                   event->buttons(),
                   event->modifiers(),
                   event->phase(),
                   event->isInverted());
    koPAView()->viewMode()->wheelEvent(&ev, viewConverter()->viewToDocument(widgetToView(ev.position().toPoint() + documentOffset())));
}

void KoPACanvasItem::closeEvent(QCloseEvent *event)
{
    koPAView()->viewMode()->closeEvent(event);
}

void KoPACanvasItem::updateInputMethodInfo()
{
    updateMicroFocus();
}

QVariant KoPACanvasItem::inputMethodQuery(Qt::InputMethodQuery query) const
{
    return toolProxy()->inputMethodQuery(query, *(viewConverter()));
}

void KoPACanvasItem::inputMethodEvent(QInputMethodEvent *event)
{
    toolProxy()->inputMethodEvent(event);
}

void KoPACanvasItem::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    Q_EMIT sizeChanged(event->newSize().toSize());
}

void KoPACanvasItem::showContextMenu(const QPoint &globalPos, const QList<QAction *> &actionList)
{
    KoPAView *view = dynamic_cast<KoPAView *>(koPAView());
    if (!view)
        return;

    view->unplugActionList("toolproxy_action_list");
    view->plugActionList("toolproxy_action_list", actionList);
    if (!view->factory())
        return;

    QMenu *menu = dynamic_cast<QMenu *>(view->factory()->container("default_canvas_popup", view));

    if (menu)
        menu->exec(globalPos);
}

void KoPACanvasItem::setCursor(const QCursor &cursor)
{
    QGraphicsWidget::setCursor(cursor);
}
