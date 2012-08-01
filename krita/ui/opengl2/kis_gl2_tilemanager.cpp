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

#include <QtCore>
#include <QGLFramebufferObject>
#include <QThreadPool>

#include <kis_image.h>

#include "kis_gl2_tile.h"
#include "kis_gl2_canvas.h"
#include "kis_gl2_tile_updater.h"

class KisGL2TileManager::Private
{
public:
    void createTiles(const QRect &size);

    KisGL2Canvas *canvas;
    QGLContext *context;
    QGLFramebufferObject *framebuffer;
    QList<KisGL2Tile*> tiles;
};

KisGL2TileManager::KisGL2TileManager(KisGL2Canvas* parent)
    : QObject(parent), d(new Private)
{
    d->canvas = parent;

    //Hack because QtOpenGL does not provide a non-const getter
    d->context = const_cast<QGLContext*>(parent->context());
}

KisGL2TileManager::~KisGL2TileManager()
{
    delete d;
}

void KisGL2TileManager::initialize(KisImageWSP image)
{
    d->framebuffer = new QGLFramebufferObject(d->canvas->width(), d->canvas->height());

    QRect imageSize = image->bounds();
    d->createTiles(imageSize);

    update(image, imageSize);
}

void KisGL2TileManager::update(KisImageWSP image, const QRect& area)
{
    QList<KisGL2Tile*> updateTiles;
    //Find the list of tiles within the changed area
    foreach(KisGL2Tile* tile, d->tiles) {
        if(area.intersects(tile->area())) {
            updateTiles.append(tile);
        }
    }

    //Make sure we clear the current context so we can reuse the context in the updater
    d->context->doneCurrent();
    //Update the tiles that need updating
    foreach(KisGL2Tile* tile, updateTiles) {
        KisGL2TileUpdater* updater = new KisGL2TileUpdater(image, tile, d->context);
        QThreadPool::globalInstance()->start(updater);
    }

    //Wait until we are done updating textures, then make sure we switch the context to the current thread again.
    QThreadPool::globalInstance()->waitForDone();
    d->context->makeCurrent();
}

void KisGL2TileManager::render()
{
    //Bind the Framebuffer
    d->framebuffer->bind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Render the background

    //Render all tiles

    //Release the framebuffer
    d->framebuffer->release();

    //Render the framebuffer
    //d->framebuffer->
}

void KisGL2TileManager::Private::createTiles(const QRect& size)
{
    int cols = qCeil(size.width() / KIS_GL2_TILE_SIZE);
    int rows = qCeil(size.height() / KIS_GL2_TILE_SIZE);

    QRect area;
    for(int r = 0; r < rows; ++r) {
        for(int c = 0; c < cols; ++c) {
            area.setRect(c * KIS_GL2_TILE_SIZE, r * KIS_GL2_TILE_SIZE, KIS_GL2_TILE_SIZE, KIS_GL2_TILE_SIZE);
            KisGL2Tile *tile = new KisGL2Tile(area);
            tiles.append(tile);
        }
    }
}
