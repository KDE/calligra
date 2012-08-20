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

#include "kis_gl2_tilemanager.h"

#include <GL/glew.h>

#include <QtCore>
#include <QGLFramebufferObject>
#include <QThreadPool>
#include <QGLShaderProgram>
#include <QGLBuffer>

#include <KStandardDirs>

#include <kis_image.h>

#include "kis_gl2_canvas.h"
#include "kis_gl2_tile.h"
#include "kis_gl2_tile_updater.h"

class KisGL2TileManager::Private
{
public:
    Private() : canvas(0), context(0), framebuffer(0) { }

    void createTiles(const QRect &size);

    KisGL2Canvas *canvas;
    KisImageWSP image;
    QGLContext *context;
    QGLFramebufferObject *framebuffer;
    QList<KisGL2Tile*> tiles;


    QGLShaderProgram* shader;
    int modelMatrixLocation;
    int viewMatrixLocation;
    int projectionMatrixLocation;
    int texture0Location;
    int textureScaleLocation;
    int vertexLocation;
    int uv0Location;

    int checkerTexture;

    QGLBuffer *vertexBuffer;
    QGLBuffer *indexBuffer;

    QElapsedTimer timer;
};

KisGL2TileManager::KisGL2TileManager(KisGL2Canvas* parent)
    : QObject(parent), d(new Private)
{
    d->canvas = parent;
}

KisGL2TileManager::~KisGL2TileManager()
{
    delete d;
}

void KisGL2TileManager::initialize(KisImageWSP image)
{
    //Hack because QtOpenGL does not provide a non-const getter
    d->context = const_cast<QGLContext*>(d->canvas->context());

    d->context->makeCurrent();

    d->framebuffer = new QGLFramebufferObject(d->canvas->width(), d->canvas->height());

    d->image = image;

//     QRect imageSize = image->bounds();
//     d->createTiles(imageSize);
//
//     update(imageSize);

    QImage checkers(64, 64, QImage::Format_ARGB32);
    QPainter painter;
    painter.begin(&checkers);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(Qt::white));
    painter.drawRect(0, 0, 64, 64);
    painter.setBrush(QBrush(Qt::gray));
    painter.drawRect(0, 0, 32, 32);
    painter.drawRect(32, 32, 32, 32);
    painter.end();
    d->checkerTexture = d->context->bindTexture(checkers);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);

    d->shader = new QGLShaderProgram(this);
    d->shader->addShaderFromSourceFile(QGLShader::Vertex, KGlobal::dirs()->findResource("data", "krita/shaders/gl2.vert"));
    d->shader->addShaderFromSourceFile(QGLShader::Fragment, KGlobal::dirs()->findResource("data", "krita/shaders/gl2.frag"));
    d->shader->link();

    d->modelMatrixLocation = d->shader->uniformLocation("modelMatrix");
    d->viewMatrixLocation = d->shader->uniformLocation("viewMatrix");
    d->projectionMatrixLocation = d->shader->uniformLocation("projectionMatrix");
    d->texture0Location = d->shader->uniformLocation("texture0");
    d->textureScaleLocation = d->shader->uniformLocation("textureScale");
    d->vertexLocation = d->shader->attributeLocation("vertex");
    d->uv0Location = d->shader->attributeLocation("uv0");

    d->vertexBuffer = new QGLBuffer(QGLBuffer::VertexBuffer);
    d->vertexBuffer->create();
    d->vertexBuffer->bind();

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

    d->vertexBuffer->allocate(vertSize + uvSize);
    d->vertexBuffer->write(0, reinterpret_cast<void*>(vertices.data()), vertSize);
    d->vertexBuffer->write(vertSize, reinterpret_cast<void*>(uvs.data()), uvSize);
    d->vertexBuffer->release();

    d->indexBuffer = new QGLBuffer(QGLBuffer::IndexBuffer);
    d->indexBuffer->create();
    d->indexBuffer->bind();

    QVector<uint> indices;
    indices << 0 << 1 << 2 << 0 << 2 << 3;
    d->indexBuffer->allocate(reinterpret_cast<void*>(indices.data()), indices.size() * sizeof(uint));
    d->indexBuffer->release();

    d->timer.start();
}

void KisGL2TileManager::update(const QRect& area)
{
//     qDebug() << "Updating" << area;
//
//     QList<KisGL2Tile*> updateTiles;
//     //Find the list of tiles within the changed area
// //     foreach(KisGL2Tile* tile, d->tiles) {
// //         if(area.intersects(tile->area())) {
// //             updateTiles.append(tile);
// //         }
// //     }
//
// //     qDebug() << "Updating" << updateTiles.count() << "tiles";
//
//     //Make sure we clear the current context so we can reuse the context in the updater
//     QElapsedTimer timer;
//     timer.start();
//
//     //d->context->doneCurrent();
//
//
//     //Update the tiles that need updating
// //     foreach(KisGL2Tile* tile, updateTiles) {
// //         KisGL2TileUpdater* updater = new KisGL2TileUpdater(d->image, tile, d->context);
// //         //QThreadPool::globalInstance()->start(updater);
// //         updater->run();
// //         delete updater;
// //     }
//
//
//
//     //Wait until we are done updating textures, then make sure we switch the context to the current thread again.
//     //QThreadPool::globalInstance()->waitForDone();
//     //d->context->makeCurrent();
//
//     qDebug() << "Updating took" << timer.elapsed() << "msec";
}

void KisGL2TileManager::render(uint texture)
{
    //qDebug() << "Time since last frame:" << d->timer.elapsed();
    //d->timer.restart();
	d->context = const_cast<QGLContext*>(d->canvas->context());

    d->context->makeCurrent();

    //Bind the framebuffer
    d->framebuffer->bind();

    //Clear it
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    d->shader->bind();

    QMatrix4x4 view;
    view.translate(-d->canvas->translation().x(), d->canvas->translation().y());
    view.rotate(d->canvas->rotation(), 0.f, 0.f, -1.f);
    view.scale(d->canvas->scaling(), d->canvas->scaling());
    d->shader->setUniformValue(d->viewMatrixLocation, view.transposed());

    QMatrix4x4 proj;
    qreal w = d->canvas->width();
    qreal h = d->canvas->height();
    proj.ortho(0, w, -h, 0, -10, 10);
    d->shader->setUniformValue(d->projectionMatrixLocation, proj.transposed());

    d->vertexBuffer->bind();
    d->indexBuffer->bind();
    d->shader->setAttributeBuffer(d->vertexLocation, GL_FLOAT, 0, 3);
    d->shader->enableAttributeArray(d->vertexLocation);
    d->shader->setAttributeBuffer(d->uv0Location, GL_FLOAT, 12 * sizeof(float), 2);
    d->shader->enableAttributeArray(d->uv0Location);

    d->shader->setUniformValue(d->texture0Location, 0);

    //Render the background
    QMatrix4x4 model;
    //model.translate(-d->image->width() / 2, -d->image->height() / 2);
    model.scale(d->image->width(), d->image->height());
    d->shader->setUniformValue(d->modelMatrixLocation, model.transposed());

    d->shader->setUniformValue(d->textureScaleLocation, QVector2D(d->image->width() / 64, d->image->height() / 64));
    glBindTexture(GL_TEXTURE_2D, d->checkerTexture);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    d->shader->setUniformValue(d->textureScaleLocation, QVector2D(1.0f, 1.0f));
    glBindTexture(GL_TEXTURE_2D, texture);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


    //d->shader->setUniformValue(d->textureScaleLocation, QVector2D(1.f, 1.f));

    //Render all tiles
//     foreach(KisGL2Tile* tile, d->tiles) {
//         //if(
//         tile->render(d->shader, d->modelMatrixLocation);
//     }

    d->framebuffer->release();

    model.setToIdentity();
    model.scale(1.0f, -1.0f);
    d->shader->setUniformValue(d->modelMatrixLocation, model);

    view.setToIdentity();
    d->shader->setUniformValue(d->viewMatrixLocation, view);

    proj.setToIdentity();
    proj.ortho(0, 1, 0, 1, -1, 1);
    d->shader->setUniformValue(d->projectionMatrixLocation, proj.transposed());

    glBindTexture(GL_TEXTURE_2D, d->framebuffer->texture());
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    d->shader->disableAttributeArray(d->uv0Location);
    d->shader->disableAttributeArray(d->vertexLocation);

    d->indexBuffer->release();
    d->vertexBuffer->release();
    d->shader->release();
}

void KisGL2TileManager::resize(int width, int height)
{
    if(d->framebuffer)
        delete d->framebuffer;

    d->framebuffer = new QGLFramebufferObject(width, height);
}

uint KisGL2TileManager::framebufferTexture() const
{
    if(d->framebuffer) {
        return d->framebuffer->texture();
    }

    return 0;
}

void KisGL2TileManager::Private::createTiles(const QRect& size)
{
    int cols = qCeil(size.width() / KIS_GL2_TILE_SIZE) + 1;
    int rows = qCeil(size.height() / KIS_GL2_TILE_SIZE) + 1;

    QRect area;
    for(int r = 0; r < rows; ++r) {
        for(int c = 0; c < cols; ++c) {
            area.setRect(c * KIS_GL2_TILE_SIZE, r * KIS_GL2_TILE_SIZE, KIS_GL2_TILE_SIZE, KIS_GL2_TILE_SIZE);
            KisGL2Tile *tile = new KisGL2Tile(area);
            tiles.append(tile);
        }
    }
}
