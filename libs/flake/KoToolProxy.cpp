/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006-2011 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "KoToolProxy.h"
#include "KoToolProxy_p.h"

#include "FlakeDebug.h"
#include "KoCanvasBase.h"
#include "KoCanvasController.h"
#include "KoInputDevice.h"
#include "KoOdf.h"
#include "KoPointerEvent.h"
#include "KoSelection.h"
#include "KoShapeController.h"
#include "KoShapeLayer.h"
#include "KoShapeManager.h"
#include "KoShapePaste.h"
#include "KoShapeRegistry.h"
#include "KoToolBase.h"
#include "KoToolManager_p.h"
#include "KoToolSelection.h"

#include <KoNetAccess.h>
#include <KoProperties.h>
#include <kundo2command.h>

#include <KLocalizedString>
#include <KMessageBox>

#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QTimer>
#include <QTouchEvent>
#include <QUrl>

KoToolProxyPrivate::KoToolProxyPrivate(KoToolProxy *p)
    : activeTool(nullptr)
    , tabletPressed(false)
    , hasSelection(false)
    , controller(nullptr)
    , parent(p)
{
    scrollTimer.setInterval(100);
    mouseLeaveWorkaround = false;
    multiClickCount = 0;
}

void KoToolProxyPrivate::timeout() // Auto scroll the canvas
{
    Q_ASSERT(controller);

    QPoint offset = QPoint(controller->canvasOffsetX(), controller->canvasOffsetY());
    QPoint origin = controller->canvas()->documentOrigin();
    QPoint viewPoint = widgetScrollPoint - origin - offset;

    QRectF mouseArea(viewPoint, QSizeF(10, 10));
    mouseArea.setTopLeft(mouseArea.center());

    controller->ensureVisible(mouseArea, true);

    QPoint newOffset = QPoint(controller->canvasOffsetX(), controller->canvasOffsetY());

    QPoint moved = offset - newOffset;
    if (moved.isNull())
        return;

    widgetScrollPoint += moved;

    QPointF documentPoint = parent->widgetToDocument(widgetScrollPoint);
    QMouseEvent event(QEvent::MouseMove, widgetScrollPoint, Qt::LeftButton, Qt::LeftButton, Qt::KeyboardModifiers());
    KoPointerEvent ev(&event, documentPoint);
    activeTool->mouseMoveEvent(&ev);
}

void KoToolProxyPrivate::checkAutoScroll(const KoPointerEvent &event)
{
    if (controller == nullptr)
        return;
    if (!activeTool)
        return;
    if (!activeTool->wantsAutoScroll())
        return;
    if (!event.isAccepted())
        return;
    if (event.buttons() != Qt::LeftButton)
        return;

    widgetScrollPoint = event.position().toPoint();

    if (!scrollTimer.isActive())
        scrollTimer.start();
}

void KoToolProxyPrivate::selectionChanged(bool newSelection)
{
    if (hasSelection == newSelection)
        return;
    hasSelection = newSelection;
    Q_EMIT parent->selectionChanged(hasSelection);
}

bool KoToolProxyPrivate::isActiveLayerEditable()
{
    if (!activeTool)
        return false;

    KoShapeManager *shapeManager = activeTool->canvas()->shapeManager();
    KoShapeLayer *activeLayer = shapeManager->selection()->activeLayer();
    if (activeLayer && !activeLayer->isEditable())
        return false;
    return true;
}

KoToolProxy::KoToolProxy(KoCanvasBase *canvas, QObject *parent)
    : QObject(parent)
    , d(new KoToolProxyPrivate(this))
{
    KoToolManager::instance()->priv()->registerToolProxy(this, canvas);

    connect(&d->scrollTimer, &QTimer::timeout, this, [this]() {
        d->timeout();
    });
}

KoToolProxy::~KoToolProxy()
{
    delete d;
}

void KoToolProxy::paint(QPainter &painter, const KoViewConverter &converter)
{
    if (d->activeTool)
        d->activeTool->paint(painter, converter);
}

void KoToolProxy::repaintDecorations()
{
    if (d->activeTool)
        d->activeTool->repaintDecorations();
}

QPointF KoToolProxy::widgetToDocument(const QPointF &widgetPoint) const
{
    QPoint offset = QPoint(d->controller->canvasOffsetX(), d->controller->canvasOffsetY());
    QPoint origin = d->controller->canvas()->documentOrigin();
    QPoint viewPoint = widgetPoint.toPoint() - origin - offset;

    return d->controller->canvas()->viewConverter()->viewToDocument(viewPoint);
}

KoCanvasBase *KoToolProxy::canvas() const
{
    return d->controller->canvas();
}

void KoToolProxy::touchEvent(QTouchEvent *event)
{
    QPointF point;
    QVector<KoTouchPoint> touchPoints;

    bool isPrimary = true;
    foreach (QTouchEvent::TouchPoint p, event->touchPoints()) {
        QPointF docPoint = widgetToDocument(p.screenPos());
        if (isPrimary) {
            point = docPoint;
            isPrimary = false;
        }
        KoTouchPoint touchPoint;
        touchPoint.touchPoint = p;
        touchPoint.point = point;
        touchPoint.lastPoint = widgetToDocument(p.lastNormalizedPos());
        touchPoints << touchPoint;
    }

    KoPointerEvent ev(event, point, touchPoints);

    KoInputDevice id;
    KoToolManager::instance()->priv()->switchInputDevice(id);

    switch (event->type()) {
    case QEvent::TouchBegin:
        ev.setTabletButton(Qt::LeftButton);
        if (d->activeTool) {
            if (d->activeTool->wantsTouch())
                d->activeTool->touchEvent(event);
            else
                d->activeTool->mousePressEvent(&ev);
        }
        break;
    case QEvent::TouchUpdate:
        ev.setTabletButton(Qt::LeftButton);
        if (d->activeTool) {
            if (d->activeTool->wantsTouch())
                d->activeTool->touchEvent(event);
            else
                d->activeTool->mouseMoveEvent(&ev);
        }
        break;
    case QEvent::TouchEnd:
        ev.setTabletButton(Qt::LeftButton);
        if (d->activeTool) {
            if (d->activeTool->wantsTouch())
                d->activeTool->touchEvent(event);
            else
                d->activeTool->mouseReleaseEvent(&ev);
        }
        break;
    default:; // ignore the rest
    }
    d->mouseLeaveWorkaround = true;
}

void KoToolProxy::tabletEvent(QTabletEvent *event, const QPointF &point)
{
    // don't process tablet events for stylus middle and right mouse button
    // they will be re-send as mouse events with the correct button. there is no possibility to get the button from the QTabletEvent.
    if (qFuzzyIsNull(event->pressure()) && d->tabletPressed == false && event->type() != QEvent::TabletMove) {
        // debugFlake<<"don't accept tablet event: "<< point;
        return;
    } else {
        // Accept the tablet events as they are useless to parent widgets and they will
        // get re-send as mouseevents if we don't accept them.
        // debugFlake<<"accept tablet event: "<< point;
        event->accept();
    }

    KoInputDevice id(event->device()->type(), event->pointerType(), event->uniqueId());
    KoToolManager::instance()->priv()->switchInputDevice(id);

    KoPointerEvent ev(event, point);
    switch (event->type()) {
    case QEvent::TabletPress:
        ev.setTabletButton(Qt::LeftButton);
        if (!d->tabletPressed && d->activeTool)
            d->activeTool->mousePressEvent(&ev);
        d->tabletPressed = true;
        break;
    case QEvent::TabletRelease:
        ev.setTabletButton(Qt::LeftButton);
        d->tabletPressed = false;
        d->scrollTimer.stop();
        if (d->activeTool)
            d->activeTool->mouseReleaseEvent(&ev);
        break;
    case QEvent::TabletMove:
        if (d->tabletPressed)
            ev.setTabletButton(Qt::LeftButton);
        if (d->activeTool)
            d->activeTool->mouseMoveEvent(&ev);
        d->checkAutoScroll(ev);
    default:; // ignore the rest.
    }

    d->mouseLeaveWorkaround = true;
}

void KoToolProxy::mousePressEvent(KoPointerEvent *ev)
{
    d->mouseLeaveWorkaround = false;
    KoInputDevice id;
    KoToolManager::instance()->priv()->switchInputDevice(id);
    d->mouseDownPoint = ev->position().toPoint();

    if (d->tabletPressed) // refuse to send a press unless there was a release first.
        return;

    QPointF globalPoint = ev->globalPosition();
    if (d->multiClickGlobalPoint != globalPoint) {
        if (qAbs(globalPoint.x() - d->multiClickGlobalPoint.x()) > 5 || qAbs(globalPoint.y() - d->multiClickGlobalPoint.y()) > 5) {
            d->multiClickCount = 0;
        }
        d->multiClickGlobalPoint = globalPoint;
    }

    if (d->multiClickCount && d->multiClickTimeStamp.elapsed() < QApplication::doubleClickInterval() && d->multiClickButton == ev->button()) {
        // One more multiclick;
        d->multiClickCount++;
    } else {
        d->multiClickTimeStamp.start();
        d->multiClickCount = 1;
        d->multiClickButton = ev->button();
    }

    if (d->activeTool) {
        switch (d->multiClickCount) {
        case 0:
        case 1:
            d->activeTool->mousePressEvent(ev);
            break;
        case 2:
            d->activeTool->mouseDoubleClickEvent(ev);
            break;
        case 3:
        default:
            d->activeTool->mouseTripleClickEvent(ev);
            break;
        }
    } else {
        d->multiClickCount = 0;
        ev->ignore();
    }
}

void KoToolProxy::mousePressEvent(QMouseEvent *event, const QPointF &point)
{
    KoPointerEvent ev(event, point);
    mousePressEvent(&ev);
}

void KoToolProxy::mouseDoubleClickEvent(QMouseEvent *event, const QPointF &point)
{
    KoPointerEvent ev(event, point);
    mouseDoubleClickEvent(&ev);
}

void KoToolProxy::mouseDoubleClickEvent(KoPointerEvent *event)
{
    // let us handle it as any other mousepress (where we then detect multi clicks
    mousePressEvent(event);
    if (!event->isAccepted() && d->activeTool)
        d->activeTool->canvas()->shapeManager()->suggestChangeTool(event);
}

void KoToolProxy::mouseMoveEvent(QMouseEvent *event, const QPointF &point)
{
    if (d->mouseLeaveWorkaround) {
        d->mouseLeaveWorkaround = false;
        return;
    }
    KoInputDevice id;
    KoToolManager::instance()->priv()->switchInputDevice(id);
    if (d->activeTool == nullptr) {
        event->ignore();
        return;
    }

    KoPointerEvent ev(event, point);
    d->activeTool->mouseMoveEvent(&ev);

    d->checkAutoScroll(ev);
}

void KoToolProxy::mouseMoveEvent(KoPointerEvent *event)
{
    if (d->mouseLeaveWorkaround) {
        d->mouseLeaveWorkaround = false;
        return;
    }
    KoInputDevice id;
    KoToolManager::instance()->priv()->switchInputDevice(id);
    if (d->activeTool == nullptr) {
        event->ignore();
        return;
    }

    d->activeTool->mouseMoveEvent(event);

    d->checkAutoScroll(*event);
}

void KoToolProxy::mouseReleaseEvent(QMouseEvent *event, const QPointF &point)
{
    d->mouseLeaveWorkaround = false;
    KoInputDevice id;
    KoToolManager::instance()->priv()->switchInputDevice(id);
    d->scrollTimer.stop();

    KoPointerEvent ev(event, point);
    if (d->activeTool) {
        d->activeTool->mouseReleaseEvent(&ev);

        if (!event->isAccepted() && event->button() == Qt::LeftButton && event->modifiers() == 0 && qAbs(d->mouseDownPoint.x() - event->x()) < 5
            && qAbs(d->mouseDownPoint.y() - event->y()) < 5) {
            // we potentially will change the selection
            Q_ASSERT(d->activeTool->canvas());
            KoShapeManager *manager = d->activeTool->canvas()->shapeManager();
            Q_ASSERT(manager);
            // only change the selection if that will not lead to losing a complex selection
            if (manager->selection()->count() <= 1) {
                KoShape *shape = manager->shapeAt(point);
                if (shape && !manager->selection()->isSelected(shape)) { // make the clicked shape the active one
                    manager->selection()->deselectAll();
                    manager->selection()->select(shape);
                    QList<KoShape *> shapes;
                    shapes << shape;
                    QString tool = KoToolManager::instance()->preferredToolForSelection(shapes);
                    KoToolManager::instance()->switchToolRequested(tool);
                }
            }
        }
    } else {
        event->ignore();
    }
}

void KoToolProxy::mouseReleaseEvent(KoPointerEvent *event)
{
    d->mouseLeaveWorkaround = false;
    KoInputDevice id;
    KoToolManager::instance()->priv()->switchInputDevice(id);
    d->scrollTimer.stop();

    if (d->activeTool) {
        d->activeTool->mouseReleaseEvent(event);

        if (!event->isAccepted() && event->button() == Qt::LeftButton && event->modifiers() == 0 && qAbs(d->mouseDownPoint.x() - event->x()) < 5
            && qAbs(d->mouseDownPoint.y() - event->y()) < 5) {
            // we potentially will change the selection
            Q_ASSERT(d->activeTool->canvas());
            KoShapeManager *manager = d->activeTool->canvas()->shapeManager();
            Q_ASSERT(manager);
            // only change the selection if that will not lead to losing a complex selection
            if (manager->selection()->count() <= 1) {
                KoShape *shape = manager->shapeAt(event->point);
                if (shape && !manager->selection()->isSelected(shape)) { // make the clicked shape the active one
                    manager->selection()->deselectAll();
                    manager->selection()->select(shape);
                    QList<KoShape *> shapes;
                    shapes << shape;
                    QString tool = KoToolManager::instance()->preferredToolForSelection(shapes);
                    KoToolManager::instance()->switchToolRequested(tool);
                }
            }
        }
    } else {
        event->ignore();
    }
}

void KoToolProxy::shortcutOverrideEvent(QKeyEvent *event)
{
    if (d->activeTool)
        d->activeTool->shortcutOverrideEvent(event);
    else
        event->ignore();
}

void KoToolProxy::keyPressEvent(QKeyEvent *event)
{
    if (d->activeTool)
        d->activeTool->keyPressEvent(event);
    else
        event->ignore();
}

void KoToolProxy::keyReleaseEvent(QKeyEvent *event)
{
    if (d->activeTool)
        d->activeTool->keyReleaseEvent(event);
    else
        event->ignore();
}

void KoToolProxy::wheelEvent(QWheelEvent *event, const QPointF &point)
{
    KoPointerEvent ev(event, point);
    if (d->activeTool)
        d->activeTool->wheelEvent(&ev);
    else
        event->ignore();
}

void KoToolProxy::wheelEvent(KoPointerEvent *event)
{
    if (d->activeTool)
        d->activeTool->wheelEvent(event);
    else
        event->ignore();
}

QVariant KoToolProxy::inputMethodQuery(Qt::InputMethodQuery query, const KoViewConverter &converter) const
{
    if (d->activeTool)
        return d->activeTool->inputMethodQuery(query, converter);
    return QVariant();
}

void KoToolProxy::inputMethodEvent(QInputMethodEvent *event)
{
    if (d->activeTool)
        d->activeTool->inputMethodEvent(event);
}

void KoToolProxy::setActiveTool(KoToolBase *tool)
{
    if (d->activeTool)
        disconnect(d->activeTool, &KoToolBase::selectionChanged, this, nullptr);
    d->activeTool = tool;
    if (tool) {
        connect(d->activeTool, &KoToolBase::selectionChanged, this, [this](bool v) {
            d->selectionChanged(v);
        });
        d->selectionChanged(hasSelection());
        Q_EMIT toolChanged(tool->toolId());
    }
}

void KoToolProxyPrivate::setCanvasController(KoCanvasController *c)
{
    controller = c;
}

QHash<QString, QAction *> KoToolProxy::actions() const
{
    return d->activeTool ? d->activeTool->actions() : QHash<QString, QAction *>();
}

bool KoToolProxy::hasSelection() const
{
    return d->activeTool ? d->activeTool->hasSelection() : false;
}

void KoToolProxy::cut()
{
    // TODO maybe move checking the active layer to KoPasteController ?
    if (d->activeTool && d->isActiveLayerEditable())
        d->activeTool->cut();
}

void KoToolProxy::copy() const
{
    if (d->activeTool)
        d->activeTool->copy();
}

bool KoToolProxy::paste()
{
    bool success = false;
    KoCanvasBase *canvas = d->controller->canvas();

    // TODO maybe move checking the active layer to KoPasteController ?
    if (d->activeTool && d->isActiveLayerEditable())
        success = d->activeTool->paste();

    if (!success) {
        const QMimeData *data = QApplication::clipboard()->mimeData();

        if (data->hasFormat(KoOdf::mimeType(KoOdf::Text))) {
            KoShapeManager *shapeManager = canvas->shapeManager();
            KoShapePaste paste(canvas, shapeManager->selection()->activeLayer());
            success = paste.paste(KoOdf::Text, data);
            if (success) {
                shapeManager->selection()->deselectAll();
                foreach (KoShape *shape, paste.pastedShapes()) {
                    shapeManager->selection()->select(shape);
                }
            }
        }
    }

    if (!success) {
        const QMimeData *data = QApplication::clipboard()->mimeData();

        QVector<QImage> imageList;

        QImage image = QApplication::clipboard()->image();

        if (!image.isNull()) {
            imageList << image;
        } else if (data->hasUrls()) {
            QList<QUrl> urls = QApplication::clipboard()->mimeData()->urls();
            foreach (const QUrl &url, urls) {
                QImage image;
                // make sure we download the files before inserting them
                if (!url.isLocalFile()) {
                    QString tmpFile;
                    if (KIO::NetAccess::download(url, tmpFile, canvas->canvasWidget())) {
                        image.load(tmpFile);
                        KIO::NetAccess::removeTempFile(tmpFile);
                    } else {
                        KMessageBox::error(canvas->canvasWidget(), KIO::NetAccess::lastErrorString());
                    }
                } else {
                    image.load(url.toLocalFile());
                }
                if (!image.isNull()) {
                    imageList << image;
                }
            }
        }

        KoShapeFactoryBase *factory = KoShapeRegistry::instance()->value("PictureShape");
        QWidget *canvasWidget = canvas->canvasWidget();
        const KoViewConverter *converter = canvas->viewConverter();
        if (imageList.length() > 0 && factory && canvasWidget) {
            KUndo2Command *cmd = new KUndo2Command(kundo2_i18n("Paste Image"));
            foreach (const QImage &image, imageList) {
                if (!image.isNull()) {
                    QPointF p = converter->viewToDocument(canvasWidget->mapFromGlobal(QCursor::pos()) + canvas->canvasController()->documentOffset()
                                                          - canvasWidget->pos());
                    KoProperties params;
                    params.setProperty("qimage", image);

                    KoShape *shape = factory->createShape(&params, canvas->shapeController()->resourceManager());
                    shape->setPosition(p);

                    // add shape to the document
                    canvas->shapeController()->addShapeDirect(shape, cmd);

                    success = true;
                }
            }
            canvas->addCommand(cmd);
        }
    }
    return success;
}

void KoToolProxy::dragMoveEvent(QDragMoveEvent *event, const QPointF &point)
{
    if (d->activeTool)
        d->activeTool->dragMoveEvent(event, point);
}

void KoToolProxy::dragLeaveEvent(QDragLeaveEvent *event)
{
    if (d->activeTool)
        d->activeTool->dragLeaveEvent(event);
}

void KoToolProxy::dropEvent(QDropEvent *event, const QPointF &point)
{
    if (d->activeTool)
        d->activeTool->dropEvent(event, point);
}

QStringList KoToolProxy::supportedPasteMimeTypes() const
{
    if (d->activeTool)
        return d->activeTool->supportedPasteMimeTypes();

    return QStringList();
}

QList<QAction *> KoToolProxy::popupActionList() const
{
    if (d->activeTool)
        return d->activeTool->popupActionList();
    return QList<QAction *>();
}

void KoToolProxy::deleteSelection()
{
    if (d->activeTool)
        return d->activeTool->deleteSelection();
}

void KoToolProxy::processEvent(QEvent *e)
{
    if (e->type() == QEvent::ShortcutOverride) {
        QKeyEvent *kev = static_cast<QKeyEvent *>(e);
        if (d->activeTool && d->activeTool->isInTextMode() && kev->modifiers() == Qt::NoModifier) {
            e->accept();
        }
        shortcutOverrideEvent(kev);
    }
}

KoToolProxyPrivate *KoToolProxy::priv()
{
    return d;
}

// have to include this because of Q_PRIVATE_SLOT
#include "moc_KoToolProxy.cpp"
