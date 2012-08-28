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

#include "kis_gl2_renderthread.h"

#include <GL/glew.h>

#include <QtCore>
#include <QGLFramebufferObject>
#include <QGLShaderProgram>
#include <QGLBuffer>
#include <QGLPixelBuffer>

#include <KStandardDirs>

#include <KoColorSpace.h>

#include <kis_image.h>
#include <kis_config.h>
#include <kis_config_notifier.h>

#include "kis_gl2_canvas.h"
#include "kis_gl2_texture_updater.h"

class KisGL2RenderThread::Private
{
public:
    Private() :
        canvas(0),
        framebuffer(0),
        checkerTexture(0),
        imageTexture(0),
        stop(false),
        newWidth(0),
        newHeight(0)
    { }

    void createImageTexture();
    void createCheckerTexture(int size, const QColor& color);
    void createShader();
    void createMesh();

    KisGL2Canvas *canvas;
    KisImageWSP image;
    KisGL2TextureUpdater *updater;

    QGLPixelBuffer *pbuffer;
    QGLFramebufferObject *framebuffer;
    QGLShaderProgram *shader;
    QGLBuffer *vertexBuffer;
    QGLBuffer *indexBuffer;

    int modelMatrixLocation;
    int viewMatrixLocation;
    int projectionMatrixLocation;
    int texture0Location;
    int textureScaleLocation;
    int vertexLocation;
    int uv0Location;

    GLuint checkerTexture;
    GLuint imageTexture;

    QElapsedTimer frameTimer;
    volatile bool stop;
    QEventLoop *eventLoop;

    volatile uint newWidth;
    volatile uint newHeight;

    GLuint texture;
};

KisGL2RenderThread::KisGL2RenderThread(KisGL2Canvas *canvas, KisImageWSP image)
    : QThread(), d(new Private)
{
    d->canvas = canvas;
    d->image = image;
}

KisGL2RenderThread::~KisGL2RenderThread()
{
    delete d;
}

void KisGL2RenderThread::initialize()
{
    d->pbuffer = new QGLPixelBuffer(1024, 1024, QGLFormat::defaultFormat(), KisGL2Canvas::shareWidget());
    d->pbuffer->makeCurrent();

    d->texture = d->pbuffer->generateDynamicTexture();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//    d->framebuffer = new QGLFramebufferObject(d->canvas->width(), d->canvas->height());

    d->createImageTexture();

    d->updater = new KisGL2TextureUpdater(d->image, d->imageTexture);
    d->updater->start();
    d->updater->moveToThread(d->updater);
    connect(d->image, SIGNAL(sigImageUpdated(QRect)), d->updater, SLOT(imageChanged(QRect)), Qt::QueuedConnection);

    d->createShader();
    d->createMesh();

    connect(KisConfigNotifier::instance(), SIGNAL(configChanged()), SLOT(configChanged()));
    configChanged();
}

uint KisGL2RenderThread::texture() const
{
    return d->texture;
}

void KisGL2RenderThread::render()
{
    //Bind the framebuffer
    //d->framebuffer->bind();

    //Clear it
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    d->shader->bind();

    //Set view/projection matrices
    QMatrix4x4 view;
    view.translate(-d->canvas->translation().x(), d->canvas->translation().y());
    view.rotate(d->canvas->rotation(), 0.f, 0.f, -1.f);
    view.scale(d->canvas->scaling(), d->canvas->scaling());
    d->shader->setUniformValue(d->viewMatrixLocation, view.transposed());

    QMatrix4x4 proj;
    proj.ortho(0, d->canvas->width(), -d->canvas->height(), 0, -10, 10);
    d->shader->setUniformValue(d->projectionMatrixLocation, proj.transposed());

    //Setup the geometry for rendering
    d->vertexBuffer->bind();
    d->indexBuffer->bind();
    d->shader->setAttributeBuffer(d->vertexLocation, GL_FLOAT, 0, 3);
    d->shader->enableAttributeArray(d->vertexLocation);
    d->shader->setAttributeBuffer(d->uv0Location, GL_FLOAT, 12 * sizeof(float), 2);
    d->shader->enableAttributeArray(d->uv0Location);

    d->shader->setUniformValue(d->texture0Location, 0);

    //Render the checker background
    QMatrix4x4 model;
    model.scale(d->image->width(), d->image->height());
    d->shader->setUniformValue(d->modelMatrixLocation, model.transposed());

    d->shader->setUniformValue(d->textureScaleLocation, QVector2D(d->image->width() / 64, d->image->height() / 64));
    glBindTexture(GL_TEXTURE_2D, d->checkerTexture);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    //Render the actual image
    d->shader->setUniformValue(d->textureScaleLocation, QVector2D(1.0f, 1.0f));
    glBindTexture(GL_TEXTURE_2D, d->imageTexture);

    if(d->canvas->scaling() > 2.0f) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    //d->framebuffer->release();

    d->shader->disableAttributeArray(d->uv0Location);
    d->shader->disableAttributeArray(d->vertexLocation);

    d->indexBuffer->release();
    d->vertexBuffer->release();
    d->shader->release();

    emit renderFinished();
}

void KisGL2RenderThread::resize(int width, int height)
{
    d->newWidth = width;
    d->newHeight = height;
}

void KisGL2RenderThread::stop()
{
    d->stop = true;
}

void KisGL2RenderThread::configChanged()
{
    const KisConfig cfg;

    QColor clearColor = cfg.canvasBorderColor();
    glClearColor(clearColor.redF(), clearColor.greenF(), clearColor.blueF(), 1.0);

    d->createCheckerTexture(cfg.checkSize(), cfg.checkersColor());
}

void KisGL2RenderThread::run()
{
    initialize();

    d->eventLoop = new QEventLoop();

    int timePerFrame = 1000 / 60;
    int remainder = 0;
    d->frameTimer.start();
    forever {
        d->eventLoop->processEvents();
        render();
        d->pbuffer->updateDynamicTexture(d->texture);

        if(d->stop)
            break;

        if(d->newWidth != 0 && d->newHeight != 0) {
//            delete d->framebuffer;
//            d->framebuffer = new QGLFramebufferObject(d->newWidth, d->newHeight);
            glDeleteTextures(1, &d->texture);
            d->pbuffer->doneCurrent();
            delete d->pbuffer;
            d->pbuffer = new QGLPixelBuffer(d->newWidth, d->newHeight, QGLFormat::defaultFormat(), KisGL2Canvas::shareWidget());
            d->pbuffer->makeCurrent();
            d->texture = d->pbuffer->generateDynamicTexture();

            d->newWidth = 0;
            d->newHeight = 0;
        }

        remainder = timePerFrame - d->frameTimer.elapsed();
        if(remainder > 0) {
            msleep(remainder);
        }

        d->frameTimer.restart();
    }

    d->updater->stop();
    d->updater->wait();
    delete d->updater;

    //Clean up all the resources once we're done.
    glDeleteTextures(1, &d->imageTexture);
    glDeleteTextures(1, &d->checkerTexture);

    delete d->vertexBuffer;
    delete d->indexBuffer;
    delete d->shader;

    delete d->pbuffer;
}

void KisGL2RenderThread::Private::createImageTexture()
{
    int pixelCount = image->width() * image->height();
    quint8 *buffer = image->projection()->colorSpace()->allocPixelBuffer(pixelCount);
    image->projection()->readBytes(buffer, image->bounds());

    quint32 *rgba = reinterpret_cast<quint32*>(buffer);

    //Convert ARGB to RGBA
    for( int x = 0; x < pixelCount; ++x )
    {
        rgba[x] = ((rgba[x] << 16) & 0xff0000) | ((rgba[x] >> 16) & 0xff) | (rgba[x] & 0xff00ff00);
    }

    glGenTextures(1, &imageTexture);
    glBindTexture(GL_TEXTURE_2D, imageTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image->width(), image->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void KisGL2RenderThread::Private::createCheckerTexture(int size, const QColor& color)
{
    int halfSize = size / 2;

    if(checkerTexture != 0) {
        glDeleteTextures(1, &checkerTexture);
    }

    QImage checkers(size, size, QImage::Format_ARGB32);
    QPainter painter;
    painter.begin(&checkers);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(Qt::white));
    painter.drawRect(0, 0, size, size);
    painter.setBrush(QBrush(color));
    painter.drawRect(0, 0, halfSize, halfSize);
    painter.drawRect(halfSize, halfSize, halfSize, halfSize);
    painter.end();

    checkerTexture = pbuffer->bindTexture(checkers);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void KisGL2RenderThread::Private::createShader()
{
    shader = new QGLShaderProgram();
    shader->addShaderFromSourceFile(QGLShader::Vertex, KGlobal::dirs()->findResource("data", "krita/shaders/gl2.vert"));
    shader->addShaderFromSourceFile(QGLShader::Fragment, KGlobal::dirs()->findResource("data", "krita/shaders/gl2.frag"));
    shader->link();

    modelMatrixLocation = shader->uniformLocation("modelMatrix");
    viewMatrixLocation = shader->uniformLocation("viewMatrix");
    projectionMatrixLocation = shader->uniformLocation("projectionMatrix");
    texture0Location = shader->uniformLocation("texture0");
    textureScaleLocation = shader->uniformLocation("textureScale");

    vertexLocation = shader->attributeLocation("vertex");
    uv0Location = shader->attributeLocation("uv0");
}

void KisGL2RenderThread::Private::createMesh()
{
    vertexBuffer = new QGLBuffer(QGLBuffer::VertexBuffer);
    vertexBuffer->create();
    vertexBuffer->bind();

    QVector<float> vertices;
    vertices << 0.0f <<  0.0f << 0.0f;
    vertices << 1.0f <<  0.0f << 0.0f;
    vertices << 1.0f << -1.0f << 0.0f;
    vertices << 0.0f << -1.0f << 0.0f;
    int vertSize = sizeof(float) * vertices.count();

    QVector<float> uvs;
    uvs << 0.f << 0.f;
    uvs << 1.f << 0.f;
    uvs << 1.f << 1.f;
    uvs << 0.f << 1.f;
    int uvSize = sizeof(float) * uvs.count();

    vertexBuffer->allocate(vertSize + uvSize);
    vertexBuffer->write(0, reinterpret_cast<void*>(vertices.data()), vertSize);
    vertexBuffer->write(vertSize, reinterpret_cast<void*>(uvs.data()), uvSize);
    vertexBuffer->release();

    indexBuffer = new QGLBuffer(QGLBuffer::IndexBuffer);
    indexBuffer->create();
    indexBuffer->bind();

    QVector<uint> indices;
    indices << 0 << 1 << 2 << 0 << 2 << 3;
    indexBuffer->allocate(reinterpret_cast<void*>(indices.data()), indices.size() * sizeof(uint));
    indexBuffer->release();
}
