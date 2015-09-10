/* This file is part of the KDE project
 * Copyright (C) Boudewijn Rempt <boud@valdyas.org>, (C) 2013
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "TouchDeclarativeView.h"

#include <QWidget>
#include <QSurfaceFormat>
#include <QResizeEvent>
#include <QApplication>
#include <QQuickItem>

#include <gemini/ViewModeSwitchEvent.h>

TouchDeclarativeView::TouchDeclarativeView(QWindow *parent)
    : QQuickView(parent)
    , m_drawCanvas(false)
    , m_canvasWidget(0)
    , m_GLInitialized(false)
{
//     setCacheMode(QQuickView::CacheNone);
//     setViewportUpdateMode(QQuickView::FullViewportUpdate);

//     setFlag(Qt::WA_AcceptTouchEvents);
//     setAttribute(Qt::WA_OpaquePaintEvent);
//     setAttribute(Qt::WA_NoSystemBackground);
//     viewport()->setAttribute(Qt::WA_OpaquePaintEvent);
//     viewport()->setAttribute(Qt::WA_NoSystemBackground);
//     installEventFilter(this);
}

TouchDeclarativeView::TouchDeclarativeView(const QUrl &url, QWindow *parent)
    : QQuickView(url, parent)
    , m_drawCanvas(false)
    , m_canvasWidget(0)
    , m_GLInitialized(false)
{
//     setCacheMode(QQuickView::CacheNone);
//     setViewportUpdateMode(QQuickView::FullViewportUpdate);

//     QSurfaceFormat format;
//     format.setSampleBuffers(true);
//     setFormat(format);

//     setAttribute(Qt::WA_AcceptTouchEvents);
//     setAttribute(Qt::WA_OpaquePaintEvent);
//     setAttribute(Qt::WA_NoSystemBackground);
//     viewport()->setAttribute(Qt::WA_OpaquePaintEvent);
//     viewport()->setAttribute(Qt::WA_NoSystemBackground);
//     installEventFilter(this);
}

TouchDeclarativeView::~TouchDeclarativeView()
{
    m_canvasWidget = 0;
}

QWidget* TouchDeclarativeView::canvasWidget() const
{
    return m_canvasWidget.data();
}

void TouchDeclarativeView::setCanvasWidget(QWidget *canvasWidget)
{
    m_canvasWidget = canvasWidget;// qobject_cast<KisOpenGLCanvas2*>(canvasWidget);
    connect(m_canvasWidget, SIGNAL(destroyed(QObject*)), this, SLOT(resetInitialized()));
    emit canvasWidgetChanged();
}

void TouchDeclarativeView::resetInitialized()
{
    m_GLInitialized = false;
}

bool TouchDeclarativeView::drawCanvas() const
{
    return m_drawCanvas;
}

void TouchDeclarativeView::setDrawCanvas(bool drawCanvas)
{
    if (m_drawCanvas != drawCanvas) {
        m_drawCanvas = drawCanvas;
        emit drawCanvasChanged();
    }
}

// void TouchDeclarativeView::drawBackground(QPainter *painter, const QRectF &rect)
// {
// 
//     if (painter->paintEngine()->type() != QPaintEngine::OpenGL2) {
//         qWarning("OpenGLScene: drawBackground needs a "
//                  "QGLWidget to be set as viewport on the "
//                  "graphics view");
//         return;
//     }
// 
//     if (m_drawCanvas && m_canvasWidget) {
//         if (!m_GLInitialized) {
//             //m_canvasWidget->initializeCheckerShader();
//             //m_canvasWidget->initializeDisplayShader();
//             m_GLInitialized = true;
//         }
//         //m_canvasWidget->renderCanvasGL();
//         //m_canvasWidget->renderDecorations(painter);
//     }
//     else {
//         QQuickView::drawBackground(painter, rect);
//     }
// 
// }


// void TouchDeclarativeView::resizeEvent(QResizeEvent *event)
// {
//     if (m_canvasWidget) {
//         //m_canvasWidget->coordinatesConverter()->setCanvasWidgetSize(event->size());
//     }
// 
//     QQuickView::resizeEvent(event);
// }
// 
// bool TouchDeclarativeView::event( QEvent* event )
// {
//     switch(static_cast<int>(event->type())) {
//         case QEvent::TabletPress:
//         case QEvent::TabletMove:
//         case QEvent::TabletRelease:
//             break;
//         case ViewModeSwitchEvent::AboutToSwitchViewModeEvent:
//         case ViewModeSwitchEvent::SwitchedToTouchModeEvent: {
//             // If we don't have a canvas widget yet, we don't really have anywhere to send those events
//             // so... let's just not
//             //if (m_canvasWidget.data())
// //             {
//                 //QGraphicsScene is silly and will not forward unknown events to its items, so emulate that
//                 //functionality.
// //                 QList<QGraphicsItem*> items = scene()->items();
// //                 Q_FOREACH(QGraphicsItem* item, items) {
// //                    if (item == m_sketchView || qobject_cast<TouchView*>((item))) {
// //                        if (item != m_sketchView)
// //                            m_sketchView = item;
// //                         scene()->sendEvent(item, event);
// //                        break;
// //                    }
// //                 }
// //             }
//             sendEvent(contentItem(), event);
//             break;
//         }
//         default:
//             break;
//     }
//     return QQuickView::event( event );
// }
// 
// bool TouchDeclarativeView::eventFilter(QObject* watched, QEvent* e)
// {
//     switch(static_cast<int>(e->type())) {
// //         case KisTabletEvent::TabletMoveEx:
// //         case KisTabletEvent::TabletPressEx:
// //         case KisTabletEvent::TabletReleaseEx: {
// //             if (m_canvasWidget.data())
// //             {
// //                 //QGraphicsScene is silly and will not forward unknown events to its items, so emulate that
// //                 //functionality.s
// //                 KisTabletEvent* ev = static_cast<KisTabletEvent*>(e);
// //                 QList<QGraphicsItem*> items = scene()->items(ev->pos());
// //                 Q_FOREACH(QGraphicsItem* item, items)
// //                 {
// //                     if(scene()->sendEvent(item, e))
// //                         return true;
// //                 }
// //             }
// //             break;
// //         }
//         default:
//             break;
//     }
// 
// 
//     return QQuickView::eventFilter(watched, e);
// }
