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

#include <GL/glew.h>

#include <QGLBuffer>
#include <QGLShader>

#include <Eigen/Geometry>

using namespace Eigen;

QGLBuffer* KisGL2Tile::m_vertexBuffer = 0;

KisGL2Tile::KisGL2Tile(const QRect& area)
    : m_area(area), m_glTexture(0)
{
    glGenTextures(1, &m_glTexture);
    glBindTexture(GL_TEXTURE_2D, m_glTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, KIS_GL2_TILE_SIZE, KIS_GL2_TILE_SIZE, 0, GL_RGBA8, GL_UNSIGNED_BYTE, 0);
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

void KisGL2Tile::render(QGLShaderProgram* shader, int location)
{
    QMatrix4x4 mat;
    mat.translate(m_area.x(), -m_area.y());
    mat.scale(m_area.width(), m_area.height());

    shader->setUniformValue(location, mat.transposed());
    glBindTexture(GL_TEXTURE_2D, m_glTexture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

QGLBuffer* KisGL2Tile::tileVertexBuffer()
{
    if(!m_vertexBuffer) {
        m_vertexBuffer = new QGLBuffer(QGLBuffer::VertexBuffer);
        m_vertexBuffer->create();
        m_vertexBuffer->bind();

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

        m_vertexBuffer->allocate(vertSize + uvSize);
        m_vertexBuffer->write(0, reinterpret_cast<void*>(vertices.data()), vertSize);
        m_vertexBuffer->write(vertSize, reinterpret_cast<void*>(uvs.data()), uvSize);
        m_vertexBuffer->release();
    }
    return m_vertexBuffer;
}
