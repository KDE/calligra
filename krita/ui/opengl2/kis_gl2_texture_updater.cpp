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

#include <KoColorSpace.h>
#include <KoColorSpaceRegistry.h>

#include <kis_image.h>

#include "kis_gl2_canvas.h"

class KisGL2TextureUpdater::Private
{
public:
    QGLPixelBuffer *pbuffer;

    QGLBuffer *currentBuffer;
    QGLBuffer *nextBuffer;

    KisImageWSP image;
    GLuint glTexture;

    QRect transferRect;
};

KisGL2TextureUpdater::KisGL2TextureUpdater(KisImageWSP image, uint texture, QObject* parent)
    : QThread(parent), d(new Private)
{
    d->image = image;
    d->glTexture = texture;
}

KisGL2TextureUpdater::~KisGL2TextureUpdater()
{
    delete d;
}

void KisGL2TextureUpdater::imageUpdated(const QRect& rect)
{
    d->currentBuffer->bind();
    glTexSubImage2D(GL_TEXTURE_2D, 0, d->transferRect.x(), d->transferRect.y(), d->transferRect.width(), d->transferRect.height(), GL_RGBA, GL_UNSIGNED_BYTE, 0);

    int pixelCount = rect.width() * rect.height();
    KoColorSpace *projectionColorSpace = d->image->projection()->colorSpace();

    quint8 *buffer = projectionColorSpace->allocPixelBuffer(pixelCount);
    d->image->projection()->readBytes(buffer, rect);

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

    //glTexSubImage2D(GL_TEXTURE_2D, 0, rect.x(), rect.y(), rect.width(), rect.height(), GL_RGBA, GL_UNSIGNED_BYTE, rgba);

    d->nextBuffer->bind();
    d->nextBuffer->allocate(rgba, pixelCount * sizeof(quint32));
    d->nextBuffer->release();

    qSwap(d->currentBuffer, d->nextBuffer);
    d->transferRect = rect;
}

void KisGL2TextureUpdater::run()
{
    d->pbuffer = new QGLPixelBuffer(1, 1, QGLFormat::defaultFormat(), KisGL2Canvas::shareWidget());
    d->pbuffer->makeCurrent();

    glBindTexture(GL_TEXTURE_2D, d->glTexture);

    d->currentBuffer = new QGLBuffer(QGLBuffer::PixelUnpackBuffer);
    d->currentBuffer->create();
    d->nextBuffer = new QGLBuffer(QGLBuffer::PixelUnpackBuffer);
    d->nextBuffer->create();

    exec();

    delete d->currentBuffer;
    delete d->nextBuffer;
    delete d->pbuffer;
}