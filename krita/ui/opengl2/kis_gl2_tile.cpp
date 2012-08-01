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

#include "kis_gl2_tile.h"

#include <GL/gl.h>

#include <QGLBuffer>

QGLBuffer* KisGL2Tile::m_vertexBuffer = 0;

KisGL2Tile::KisGL2Tile(const QRect& area)
    : m_area(area), m_glTexture(0)
{
    glGenTextures(1, &m_glTexture);
    glBindTexture(GL_TEXTURE_2D, m_glTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, KIS_GL2_TILE_SIZE, KIS_GL2_TILE_SIZE, 0, GL_RGBA8, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

KisGL2Tile::~KisGL2Tile()
{
    glDeleteTextures(1, &m_glTexture);
}

QRect KisGL2Tile::area() const
{
    return m_area;
}

unsigned int KisGL2Tile::glTexture() const
{
    return m_glTexture;
}

const QGLBuffer* KisGL2Tile::tileVertexBuffer()
{
    if(!m_vertexBuffer) {
        m_vertexBuffer = new QGLBuffer(QGLBuffer::VertexBuffer);
    }
    return m_vertexBuffer;
}
