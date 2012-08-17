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

#include "kis_gl2_canvas.h"

#include <QApplication>

#include <kis_config.h>
#include <kis_config_notifier.h>
#include <kis_image.h>

#include <opengl/kis_opengl.h>

#include "kis_gl2_tilemanager.h"
#include "kis_gl2_texture_updater.h"
#include <kis_view2.h>
#include <kis_canvas2.h>

class KisGL2Canvas::Private
{
public:
    KisGL2TileManager *tileManager;

    KisGL2TextureUpdater *updater;
    GLuint imageTexture;

    static QGLWidget *shareWidget;
};

QGLWidget *KisGL2Canvas::Private::shareWidget = 0;

KisGL2Canvas::KisGL2Canvas(KisCanvas2 *canvas, KisCoordinatesConverter *coordinatesConverter, QWidget *parent)
    : QGLWidget(parent, KisGL2Canvas::shareWidget()), KisCanvasWidgetBase(canvas, coordinatesConverter), d(new Private)
{
    d->tileManager = new KisGL2TileManager(this);
}

KisGL2Canvas::~KisGL2Canvas()
{
    delete d;
}

QWidget* KisGL2Canvas::widget()
{
    return this;
}

bool KisGL2Canvas::callFocusNextPrevChild(bool next)
{
    return focusNextPrevChild(next);
}

void KisGL2Canvas::initialize(KisImageWSP image)
{
    const_cast<QGLContext*>(context())->makeCurrent();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    connect(KisConfigNotifier::instance(), SIGNAL(configChanged()), SLOT(configChanged()));
    configChanged();

    d->tileManager->initialize(image);

    int pixelCount = image->width() * image->height();
    quint8 *buffer = image->projection()->colorSpace()->allocPixelBuffer(pixelCount);
    image->projection()->readBytes(buffer, image->bounds());

    quint32 *rgba = reinterpret_cast<quint32*>(buffer);

    //Convert ARGB to RGBA
    for( int x = 0; x < pixelCount; ++x )
    {
        rgba[x] = ((rgba[x] << 16) & 0xff0000) | ((rgba[x] >> 16) & 0xff) | (rgba[x] & 0xff00ff00);
    }

    glGenTextures(1, &d->imageTexture);
    glBindTexture(GL_TEXTURE_2D, d->imageTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image->width(), image->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
    glBindTexture(GL_TEXTURE_2D, 0);

    d->updater = new KisGL2TextureUpdater(image, d->imageTexture);
    d->updater->start();
    d->updater->moveToThread(d->updater);
    connect(image, SIGNAL(sigImageUpdated(QRect)), d->updater, SLOT(imageUpdated(QRect)), Qt::QueuedConnection);
}

void KisGL2Canvas::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    d->tileManager->render(d->imageTexture);
}

void KisGL2Canvas::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    d->tileManager->resize(w, h);
}

QPoint KisGL2Canvas::translation() const
{
    return canvas()->documentOffset();
}

qreal KisGL2Canvas::rotation() const
{
    return canvas()->rotationAngle();
}

qreal KisGL2Canvas::scaling() const
{
    return canvas()->viewConverter()->zoom();
}

void KisGL2Canvas::configChanged()
{
    const KisConfig cfg;
    QColor clearColor = cfg.canvasBorderColor();

    glClearColor(clearColor.redF(), clearColor.greenF(), clearColor.blueF(), 1.0);
}

void KisGL2Canvas::update(const QRect& area)
{
    //d->tileManager->update(area);
    //paintGL();
}

uint KisGL2Canvas::framebufferTexture() const
{
    return d->tileManager->framebufferTexture();
}

QGLWidget* KisGL2Canvas::shareWidget()
{
    if(!KisGL2Canvas::Private::shareWidget) {
        KisGL2Canvas::Private::shareWidget = new QGLWidget();
    }

    return KisGL2Canvas::Private::shareWidget;
}

#include "kis_gl2_canvas.moc"
