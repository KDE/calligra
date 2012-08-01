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

#ifndef KIS_GL2_TILE_H
#define KIS_GL2_TILE_H

#include <QRectF>

#define KIS_GL2_TILE_SIZE 256

class QGLShaderProgram;
class QGLBuffer;
class KisGL2Tile
{
public:
    KisGL2Tile(const QRect &area);
    virtual ~KisGL2Tile();

    QRect area() const;
    uint glTexture() const;

    void render(QGLShaderProgram* shader, int location);

    static QGLBuffer* tileVertexBuffer();

private:
    QRect m_area;
    uint m_glTexture;

    static QGLBuffer* m_vertexBuffer;
};

#endif // KIS_GL2_TILE_H
