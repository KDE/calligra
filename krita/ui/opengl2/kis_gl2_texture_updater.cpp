/* This file is part of the KDE project
 * Copyright (C) 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
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

#include "kis_gl2_texture_updater.h"

#include <GL/glew.h>

#include <QGLPixelBuffer>
#include <QGLBuffer>
#include <QTimer>
#include <QEventLoop>

#include <KoColorSpace.h>
#include <KoColorSpaceRegistry.h>

#include <kis_image.h>

#include "kis_gl2_canvas.h"

class KisGL2TextureUpdater::Private
{
public:
    Private() : stop(false) { }

    QGLPixelBuffer *pbuffer;

    QGLBuffer *currentBuffer;
    QGLBuffer *nextBuffer;

    KisImageWSP image;
    GLuint glTexture;

    QRect transferRect;
    QRect changedRect;

    QEventLoop *loop;

    bool stop;
};

KisGL2TextureUpdater::KisGL2TextureUpdater(KisImageWSP image, uint texture, QObject* parent)
    : QThread(parent), d(new Private)
{
    d->image = image;
    d->glTexture = texture;
}

KisGL2TextureUpdater::~KisGL2TextureUpdater()
{
    d->stop = true;
    delete d;
}

void KisGL2TextureUpdater::imageUpdated(const QRect& rect)
{
    d->changedRect = d->changedRect.united(rect);

    if(d->changedRect.width() >= 256 || d->changedRect.height() >= 256) {
        timeout();
    }
}

void KisGL2TextureUpdater::timeout()
{
//    if(!d->transferRect.isEmpty()) {
//        d->currentBuffer->bind();
//        glTexSubImage2D(GL_TEXTURE_2D, 0, d->transferRect.x(), d->transferRect.y(), d->transferRect.width(), d->transferRect.height(), GL_RGBA, GL_UNSIGNED_BYTE, 0);
//        d->currentBuffer->release();
//    }

    if(d->changedRect.isEmpty()) {
//        d->transferRect = QRect();
        return;
    }

    int pixelCount = d->changedRect.width() * d->changedRect.height();
    KoColorSpace *projectionColorSpace = d->image->projection()->colorSpace();

    quint8 *buffer = projectionColorSpace->allocPixelBuffer(pixelCount);
    d->image->projection()->readBytes(buffer, d->changedRect);

    const KoColorSpace *framebufferColorSpace = KoColorSpaceRegistry::instance()->rgb8();
    quint8 *dest = framebufferColorSpace->allocPixelBuffer(pixelCount);
    projectionColorSpace->convertPixelsTo(buffer,
        dest,
        framebufferColorSpace,
        pixelCount,
        KoColorConversionTransformation::IntentRelativeColorimetric,
        KoColorConversionTransformation::BlackpointCompensation | KoColorConversionTransformation::NoOptimization);

    quint32 *rgba = reinterpret_cast<quint32*>(dest);

    //Convert ARGB to RGBA
    for( int x = 0; x < pixelCount; ++x )
    {
        rgba[x] = ((rgba[x] << 16) & 0xff0000) | ((rgba[x] >> 16) & 0xff) | (rgba[x] & 0xff00ff00);
    }

//    d->nextBuffer->bind();
//    d->nextBuffer->allocate(rgba, pixelCount * sizeof(quint32));

////    void* vid = d->nextBuffer->map(QGLBuffer::WriteOnly);
////    memcpy(vid, rgba, pixelCount * sizeof(quint32));
////    d->nextBuffer->unmap();

//    d->nextBuffer->release();

//    qSwap(d->currentBuffer, d->nextBuffer);

//    d->transferRect = d->changedRect;

    glTexSubImage2D(GL_TEXTURE_2D, 0, d->changedRect.x(), d->changedRect.y(), d->changedRect.width(), d->changedRect.height(), GL_RGBA, GL_UNSIGNED_BYTE, rgba);
    d->changedRect = QRect();
}

void KisGL2TextureUpdater::run()
{
    d->pbuffer = new QGLPixelBuffer(1, 1, QGLFormat::defaultFormat(), KisGL2Canvas::shareWidget());
    d->pbuffer->makeCurrent();

    glBindTexture(GL_TEXTURE_2D, d->glTexture);

    d->currentBuffer = new QGLBuffer(QGLBuffer::PixelUnpackBuffer);
    d->currentBuffer->setUsagePattern(QGLBuffer::DynamicDraw);
    d->currentBuffer->create();
    d->nextBuffer = new QGLBuffer(QGLBuffer::PixelUnpackBuffer);
    d->nextBuffer->setUsagePattern(QGLBuffer::DynamicDraw);
    d->nextBuffer->create();

    d->loop = new QEventLoop();

    forever {
        d->loop->processEvents();
        timeout();

        if(d->stop)
            break;

        msleep(2);
    }

    delete d->currentBuffer;
    delete d->nextBuffer;
    delete d->pbuffer;
}
