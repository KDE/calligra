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

#include "kis_gl2_tile_updater.h"

#include <QGLContext>

#include <kis_image.h>
#include "kis_gl2_tile.h"
#include <KoColorSpace.h>
#include <KoColorModelStandardIds.h>
#include <KoColorSpaceRegistry.h>
#include <GL/gl.h>

KisGL2TileUpdater::KisGL2TileUpdater(const KisImageWSP image, const KisGL2Tile* tile, QGLContext* context)
    : m_image(image), m_tile(tile), m_context(context)
{

}

KisGL2TileUpdater::~KisGL2TileUpdater()
{

}

void KisGL2TileUpdater::run()
{
    QElapsedTimer timer;
    timer.start();

    QRect tileArea = m_tile->area();
    int pixelCount = tileArea.width() * tileArea.height();
    KoColorSpace *projectionColorSpace = m_image->projection()->colorSpace();

    quint8 *buffer = projectionColorSpace->allocPixelBuffer(pixelCount);
    m_image->projection()->readBytes(buffer, m_tile->area());

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

    //qDebug() << "Getting and converting data took" << timer.elapsed() << "msec";
    timer.restart();

    //m_context->makeCurrent();

    glBindTexture(GL_TEXTURE_2D, m_tile->glTexture());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tileArea.width(), tileArea.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);

    //qDebug() << "Uploading to GL took" << timer.elapsed() << "msec";
}
