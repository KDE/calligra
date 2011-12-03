/* This file is part of the KDE project
   Copyright (C) 2011 Yue Liu <yue.liu@mail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KOCONNECTIONSHAPEROUTER_H
#define KOCONNECTIONSHAPEROUTER_H

#include "libavoid/libavoid.h"

#include <QHash>

class KoShape;

class KoConnectionShapeRouter
{
public:
    ///add an obstacle shape to the router
    void addObstacle(KoShape *shape);
    ///update an obstacle shape in the router
    void updateObstacle(KoShape *shape);
    ///delete an obstacle shape from the router
    void removeObstacle(KoShape *shape);

    ///update the connection shape routing
    void update();

private:
    Avoid::Polygon getObstaclePolygon(KoShape *shape);
    Avoid::router *m_router;
    QHash<KoShape *, Avoid::ShapeRef *> *m_obstacleHash;
};

#endif /* KOCONNECTIONSHAPEROUTER_H */
