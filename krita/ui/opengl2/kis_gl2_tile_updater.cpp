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
    QRect tileArea = m_tile->area();
    int pixelCount = tileArea.width() * tileArea.height();
    KoColorSpace *projectionColorSpace = m_image->projection()->colorSpace();

    m_buffer = projectionColorSpace->allocPixelBuffer(pixelCount);
    m_image->projection()->readBytes(m_buffer, m_tile->area());

    const KoColorSpace *framebufferColorSpace = KoColorSpaceRegistry::instance()->colorSpace(RGBAColorModelID.id(), Float32BitsColorDepthID.id(), QString());
    quint8 *dest = framebufferColorSpace->allocPixelBuffer(pixelCount);
    projectionColorSpace->convertPixelsTo(m_buffer,
        dest,
        framebufferColorSpace,
        pixelCount,
        KoColorConversionTransformation::IntentRelativeColorimetric,
        KoColorConversionTransformation::BlackpointCompensation | KoColorConversionTransformation::NoOptimization);

    m_context->makeCurrent();
    glBindTexture(GL_TEXTURE_2D, m_tile->glTexture());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, tileArea.width(), tileArea.height(), 0, GL_RGBA32F_ARB, GL_FLOAT, dest);
    m_context->doneCurrent();
}
