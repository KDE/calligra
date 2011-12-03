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

#include "KoConnectionShapeRouter.h"

#include <KoShape.h>

#include <QPolygonF>

KoConnectionShapeRouter::KoConnectionShapeRouter()
{
    m_router = new Avoid::Router(Avoid::OrthogonalRouting | Avoid::PolyLineRouting);
}

KoConnectionShapeRouter::~KoConnectionShapeRouter()
{
    delete m_obstacleHash;
    delete m_router;
}

void KoConnectionShapeRouter::addObstacle(KoShape *shape)
{
    Avoid::Polygon shapePoly = getObstaclePolygon(shape);
    Avoid::ShapeRef *shapeRef = new Avoid::ShapeRef(m_router, shapePoly);
    m_obstacleHash->insert(shape, shapeRef);
}

void KoConnectionShapeRouter::updateObstacle(KoShape *shape)
{
    Avoid::Polygon shapePoly = getObstaclePolygon(shape);
    Avoid::ShapeRef *shapeRef = m_obstacleHash->value(shape);
    m_router->moveShape(shapeRef, shapePoly);
}

void KoConnectionShapeRouter::removeObstacle(KoShape *shape)
{
    Avoid::ShapeRef *shapeRef = m_obstacleHash->value(shape);
    m_obstacleHash->remove(shape);
    m_router->deleteShape(shapeRef);
}

void KoConnectionShapeRouter::update()
{
    *m_router->processTransaction();
}

Avoid::Polygon KoConnectionShapeRouter::getObstaclePolygon(KoShape *shape)
{
    QPolygonF poly = shape->absoluteTransformation(0).map(shape->outline().toFillPolygon());
    Avoid::Polygon shapePoly(poly.size());
    for (i=0; i<poly.size(); i++) {
        shapePoly.ps[i] = Avoid::Point(poly[i].x(), poly[i].y());
    }
    return shapePoly;
}
