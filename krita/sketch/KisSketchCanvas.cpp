/* This file is part of the KDE project
 *
 * Copyright (c) 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
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

#include "KisSketchCanvas.h"

#include <QApplication>
#include <QTimer>
#include <QGLBuffer>
#include <QGLShaderProgram>

#include <kstandarddirs.h>

#include <kis_config.h>
#include <kis_config_notifier.h>
#include <kis_image.h>
#include <kis_view2.h>
#include <kis_canvas2.h>

#include <opengl2/kis_gl2_renderthread.h>

class KisSketchCanvas::Private
{
public:
    Private() : renderer(0), newWidth(0), newHeight(0) { }

    KisGL2RenderThread *renderer;
    KisImageWSP image;

    QTimer *resizeTimer;
    uint newWidth;
    uint newHeight;
};

KisSketchCanvas::KisSketchCanvas(KisCanvas2* canvas, KisCoordinatesConverter* coordinatesConverter, QWidget* parent)
    : QWidget(parent), KisCanvasWidgetBase(canvas, coordinatesConverter), d(new Private)
{
    d->image = canvas->view()->image();
    d->resizeTimer = new QTimer(this);
    d->resizeTimer->setInterval(500);
    d->resizeTimer->setSingleShot(true);
    connect(d->resizeTimer, SIGNAL(timeout()), this, SLOT(resizeImpl()));
}

KisSketchCanvas::~KisSketchCanvas()
{
    d->renderer->stop();
    d->renderer->wait();
    delete d->renderer;
    delete d;
}

QWidget* KisSketchCanvas::widget()
{
    return this;
}

bool KisSketchCanvas::callFocusNextPrevChild(bool next)
{
    return focusNextPrevChild(next);
}

void KisSketchCanvas::initialize()
{
    d->renderer = new KisGL2RenderThread(width(), height(), canvas(), d->image);
    d->renderer->start();
    d->renderer->moveToThread(d->renderer);
    connect(d->renderer, SIGNAL(renderFinished()), SIGNAL(renderFinished()));
}

void KisSketchCanvas::resize(int w, int h)
{
    glViewport(0, 0, w, h);

    d->newWidth = w;
    d->newHeight = h;

    d->resizeTimer->start();
}

uint KisSketchCanvas::texture() const
{
    if (d->renderer) {
        return d->renderer->texture();
    }
    return 0;
}

void KisSketchCanvas::resizeImpl()
{
    if (d->renderer) {
        d->renderer->stop();
        d->renderer->wait();
        delete d->renderer;

        d->renderer = new KisGL2RenderThread(d->newWidth, d->newHeight, canvas(), d->image);
        d->renderer->start();
        d->renderer->moveToThread(d->renderer);
        connect(d->renderer, SIGNAL(renderFinished()), SIGNAL(renderFinished()));
    }
}


#include "KisSketchCanvas.moc"
