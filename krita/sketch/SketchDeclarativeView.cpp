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
#include "SketchDeclarativeView.h"

#include <opengl/kis_opengl.h>
#include <QWidget>
#include <QGLWidget>
#include <QGLFramebufferObject>
#include <QDebug>
#include <QResizeEvent>
#include <opengl/kis_opengl_canvas2.h>

SketchDeclarativeView::SketchDeclarativeView(QWidget *parent)
    : QDeclarativeView(parent)
    , m_canvasWidget(0)
    , m_GLInitialized(false)
{
    setCacheMode(QGraphicsView::CacheNone);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
}

SketchDeclarativeView::SketchDeclarativeView(const QUrl &url, QWidget *parent)
    : QDeclarativeView(url, parent)
    , m_canvasWidget(0)
    , m_GLInitialized(false)
{
    setCacheMode(QGraphicsView::CacheNone);
}

SketchDeclarativeView::~SketchDeclarativeView()
{
    m_canvasWidget = 0;
}

void SketchDeclarativeView::setCanvasWidget(QWidget *canvasWidget)
{
    m_canvasWidget = qobject_cast<KisOpenGLCanvas2*>(canvasWidget);
}

bool SketchDeclarativeView::drawCanvas() const
{
    return m_drawCanvas;
}

void SketchDeclarativeView::setDrawCanvas(bool drawCanvas)
{
    if (m_drawCanvas != drawCanvas) {
        m_drawCanvas = drawCanvas;
        emit drawCanvasChanged();
    }
}

void SketchDeclarativeView::drawBackground(QPainter *painter, const QRectF &rect)
{

    if (painter->paintEngine()->type() != QPaintEngine::OpenGL2) {
        qWarning("OpenGLScene: drawBackground needs a "
                 "QGLWidget to be set as viewport on the "
                 "graphics view");
        return;
    }

    if (m_drawCanvas && m_canvasWidget) {
        if (!m_GLInitialized) {
            m_canvasWidget->initializeCheckerShader();
            m_canvasWidget->initializeDisplayShader();
            m_GLInitialized = true;
        }
        m_canvasWidget->renderCanvas();
        m_canvasWidget->renderDecorations(painter);
    }
    else {
        QDeclarativeView::drawBackground(painter, rect);
    }

}
