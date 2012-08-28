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
#include <QTimer>
#include <QGLBuffer>
#include <QGLShaderProgram>

#include <kstandarddirs.h>

#include <kis_config.h>
#include <kis_config_notifier.h>
#include <kis_image.h>
#include <kis_view2.h>
#include <kis_canvas2.h>

#include "kis_gl2_renderthread.h"

class KisGL2Canvas::Private
{
public:
    void createShader();
    void createMesh();

    KisGL2RenderThread *renderer;

    KisImageWSP image;

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

    static QGLWidget *shareWidget;
};

QGLWidget *KisGL2Canvas::Private::shareWidget = 0;

KisGL2Canvas::KisGL2Canvas(KisCanvas2* canvas, KisCoordinatesConverter* coordinatesConverter, KisImageWSP image, QWidget* parent)
    : QGLWidget(parent, KisGL2Canvas::shareWidget()), KisCanvasWidgetBase(canvas, coordinatesConverter), d(new Private)
{
    d->image = image;
}

KisGL2Canvas::~KisGL2Canvas()
{
    d->renderer->stop();
    d->renderer->wait();
    delete d->renderer;
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

void KisGL2Canvas::initializeGL()
{
    d->renderer = new KisGL2RenderThread(this, d->image);
    d->renderer->start();
    d->renderer->moveToThread(d->renderer);
    connect(d->renderer, SIGNAL(renderFinished()), this, SLOT(update()), Qt::QueuedConnection);

    d->createShader();
    d->createMesh();
}

void KisGL2Canvas::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    d->shader->bind();

    QMatrix4x4 model;
    model.scale(1, -1);
    d->shader->setUniformValue(d->modelMatrixLocation, model.transposed());

    //Set view/projection matrices
    QMatrix4x4 view;
    d->shader->setUniformValue(d->viewMatrixLocation, view.transposed());

    QMatrix4x4 proj;
    proj.ortho(0, 1, 0, 1, -1, 1);
    d->shader->setUniformValue(d->projectionMatrixLocation, proj.transposed());

    //Setup the geometry for rendering
    d->vertexBuffer->bind();
    d->indexBuffer->bind();
    d->shader->setAttributeBuffer(d->vertexLocation, GL_FLOAT, 0, 3);
    d->shader->enableAttributeArray(d->vertexLocation);
    d->shader->setAttributeBuffer(d->uv0Location, GL_FLOAT, 12 * sizeof(float), 2);
    d->shader->enableAttributeArray(d->uv0Location);

    glBindTexture(GL_TEXTURE_2D, d->renderer->texture());
    d->shader->setUniformValue(d->texture0Location, 0);

    d->shader->setUniformValue(d->textureScaleLocation, QVector2D(1.0f, 1.0f));

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    d->shader->disableAttributeArray(d->uv0Location);
    d->shader->disableAttributeArray(d->vertexLocation);

    d->indexBuffer->release();
    d->vertexBuffer->release();
    d->shader->release();
}

void KisGL2Canvas::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    d->renderer->resize(w, h);
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

QGLWidget* KisGL2Canvas::shareWidget()
{
    if(!KisGL2Canvas::Private::shareWidget) {
        KisGL2Canvas::Private::shareWidget = new QGLWidget();
    }

    return KisGL2Canvas::Private::shareWidget;
}

void KisGL2Canvas::Private::createShader()
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

void KisGL2Canvas::Private::createMesh()
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

#include "kis_gl2_canvas.moc"
