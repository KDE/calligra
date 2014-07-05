/* This file is part of the KDE project
 * Copyright (C) 2014 Wenchao Li <manchiu.lee.9@gmail.com>
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

#include "VariableLineShape.h"

#include "KoMarker.h"
#include "KoPathPoint.h"

#include <KoColorBackground.h>
#include <QPainter>
#include <klocale.h>
#include <kdebug.h>

#ifndef QT_NO_DEBUG
#include <qnumeric.h> // for qIsNaN
static bool qIsNaNPoint(const QPointF &p) {
        return qIsNaN(p.x()) || qIsNaN(p.y());
}
#endif

VariableLineShape::VariableLineShape()
: m_widthPercentage(100)
{
    setFillRule(Qt::WindingFill);
}

VariableLineShape::~VariableLineShape()
{
}

bool VariableLineShape::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    return true;
}

void VariableLineShape::saveOdf(KoShapeSavingContext & context) const
{
}

void VariableLineShape::initDefaultPointWidth()
{
    foreach (KoSubpath * subpath, m_subpaths){
        foreach (KoPathPoint *currPoint, *subpath) {
            m_pointsWidth.insert(currPoint, 50.0);
        }
    }
}

qreal VariableLineShape::getPointStokeWidth(KoPathPoint *pathPoint)
{
    return m_pointsWidth[pathPoint];
}

QPainterPath VariableLineShape::createVariableWidthOutline() const
{
    QPainterPath path;
    foreach(KoSubpath * subpath, m_subpaths) {
        KoPathPoint * lastPoint = subpath->first();
        bool activeCP = false;
        foreach(KoPathPoint * currPoint, *subpath) {
            KoPathPoint::PointProperties currProperties = currPoint->properties();
            if (currPoint == subpath->first()) {
                if (currProperties & KoPathPoint::StartSubpath) {
                    Q_ASSERT(!qIsNaNPoint(currPoint->point()));
                    path.moveTo(currPoint->point());
                }
            } else if (currPoint == subpath->last()) {
                path.moveTo(lastPoint->point().x() - m_pointsWidth[currPoint]/2, lastPoint->point().y());
                path.lineTo(currPoint->point().x(), currPoint->point().y());
                path.moveTo(lastPoint->point().x() + m_pointsWidth[currPoint]/2, lastPoint->point().y());
                path.lineTo(currPoint->point().x(), currPoint->point().y());
            }else {
                Q_ASSERT(!qIsNaNPoint(currPoint->point()));
                if (lastPoint != subpath->first()) {
                    path.moveTo(lastPoint->point().x() - m_pointsWidth[lastPoint]/2, lastPoint->point().y());
                    path.lineTo(currPoint->point().x() - m_pointsWidth[currPoint]/2, currPoint->point().y());
                    path.moveTo(lastPoint->point().x() + m_pointsWidth[lastPoint]/2, lastPoint->point().y());
                    path.lineTo(currPoint->point().x() + m_pointsWidth[currPoint]/2, currPoint->point().y());
                } else {
                //path.lineTo(currPoint->point());
                    path.lineTo(currPoint->point().x() - m_pointsWidth[currPoint]/2, currPoint->point().y());
                    path.moveTo(lastPoint->point());
                    path.lineTo(currPoint->point().x() + m_pointsWidth[currPoint]/2, currPoint->point().y());
                }
            }
            if (currProperties & KoPathPoint::CloseSubpath && currProperties & KoPathPoint::StopSubpath) {
                // add curve when there is a curve on the way to the first point
                KoPathPoint * firstPoint = subpath->first();
                Q_ASSERT(!qIsNaNPoint(firstPoint->point()));
                if (currPoint->activeControlPoint2() && firstPoint->activeControlPoint1()) {
                    path.cubicTo(
                        currPoint->controlPoint2(),
                        firstPoint->controlPoint1(),
                        firstPoint->point());
                }
                else if (currPoint->activeControlPoint2() || firstPoint->activeControlPoint1()) {
                    Q_ASSERT(!qIsNaNPoint(currPoint->point()));
                    Q_ASSERT(!qIsNaNPoint(currPoint->controlPoint1()));
                    path.quadTo(
                        currPoint->activeControlPoint2() ? currPoint->controlPoint2() : firstPoint->controlPoint1(),
                        firstPoint->point());
                }
                path.closeSubpath();
            }

            if (currPoint->activeControlPoint2()) {
                activeCP = true;
            } else {
                activeCP = false;
            }
            lastPoint = currPoint;
        }
    }

    return path;
}

QPainterPath VariableLineShape::pathStroke(const QPen &pen) const
{
    if (m_subpaths.isEmpty()) {
        return QPainterPath();
    }
    QPainterPath pathOutline;

    QPainterPathStroker stroker;
    stroker.setWidth(0);
    stroker.setJoinStyle(Qt::MiterJoin);

    QPair<KoPathSegment, KoPathSegment> firstSegments;
    QPair<KoPathSegment, KoPathSegment> lastSegments;

    KoPathPoint *firstPoint = 0;
    KoPathPoint *lastPoint = 0;
    KoPathPoint *secondPoint = 0;
    KoPathPoint *preLastPoint = 0;

    KoSubpath *firstSubpath = m_subpaths.first();
    bool twoPointPath = subpathPointCount(0) == 2;
    bool closedPath = isClosedSubpath(0);

    static qreal shortenFactor = 0.7;

    KoMarkerData mdStart = markerData(KoMarkerData::MarkerStart);
    KoMarkerData mdEnd = markerData(KoMarkerData::MarkerEnd);
    if (mdStart.marker() && !closedPath) {
        QPainterPath markerPath = mdStart.marker()->path(mdStart.width(pen.widthF()));

        KoPathSegment firstSegment = segmentByIndex(KoPathPointIndex(0, 0));
        if (firstSegment.isValid()) {
            QRectF pathBoundingRect = markerPath.boundingRect();
            qreal shortenLength = pathBoundingRect.height() * shortenFactor;
            kDebug(30006) << "length" << firstSegment.length() << shortenLength;
            qreal t = firstSegment.paramAtLength(shortenLength);
            firstSegments = firstSegment.splitAt(t);
            // transform the marker so that it goes from the first point of the first segment to the second point of the first segment
            QPointF startPoint = firstSegments.first.first()->point();
            QPointF newStartPoint = firstSegments.first.second()->point();
            QLineF vector(newStartPoint, startPoint);
            qreal angle = -vector.angle() + 90;
            QTransform transform;
            transform.translate(startPoint.x(), startPoint.y())
                     .rotate(angle)
                     .translate(-pathBoundingRect.width() / 2.0, 0);

            markerPath = transform.map(markerPath);
            QPainterPath startOutline = stroker.createStroke(markerPath);
            startOutline = startOutline.united(markerPath);
            pathOutline.addPath(startOutline);
            firstPoint = firstSubpath->first();
            if (firstPoint->properties() & KoPathPoint::StartSubpath) {
                firstSegments.second.first()->setProperty(KoPathPoint::StartSubpath);
            }
            kDebug(30006) << "start marker" << angle << startPoint << newStartPoint << firstPoint->point();

            if (!twoPointPath) {
                if (firstSegment.second()->activeControlPoint2()) {
                    firstSegments.second.second()->setControlPoint2(firstSegment.second()->controlPoint2());
                }
                secondPoint = (*firstSubpath)[1];
            }
            else if (!mdEnd.marker()) {
                // in case it is two point path with no end marker we need to modify the last point via the secondPoint
                secondPoint = (*firstSubpath)[1];
            }
        }
    }
    if (mdEnd.marker() && !closedPath) {
        QPainterPath markerPath = mdEnd.marker()->path(mdEnd.width(pen.widthF()));

        KoPathSegment lastSegment;

        /*
         * if the path consits only of 2 point and it it has an marker on both ends
         * use the firstSegments.second as that is the path that needs to be shortened
         */
        if (twoPointPath && firstPoint) {
            lastSegment = firstSegments.second;
        }
        else {
            lastSegment = segmentByIndex(KoPathPointIndex(0, firstSubpath->count() - 2));
        }

        if (lastSegment.isValid()) {
            QRectF pathBoundingRect = markerPath.boundingRect();
            qreal shortenLength = lastSegment.length() - pathBoundingRect.height() * shortenFactor;
            qreal t = lastSegment.paramAtLength(shortenLength);
            lastSegments = lastSegment.splitAt(t);
            // transform the marker so that it goes from the last point of the first segment to the previous point of the last segment
            QPointF startPoint = lastSegments.second.second()->point();
            QPointF newStartPoint = lastSegments.second.first()->point();
            QLineF vector(newStartPoint, startPoint);
            qreal angle = -vector.angle() + 90;
            QTransform transform;
            transform.translate(startPoint.x(), startPoint.y()).rotate(angle).translate(-pathBoundingRect.width() / 2.0, 0);

            markerPath = transform.map(markerPath);
            QPainterPath endOutline = stroker.createStroke(markerPath);
            endOutline = endOutline.united(markerPath);
            pathOutline.addPath(endOutline);
            lastPoint = firstSubpath->last();
            kDebug(30006) << "end marker" << angle << startPoint << newStartPoint << lastPoint->point();
            if (twoPointPath) {
                if (firstSegments.second.isValid()) {
                    if (lastSegments.first.first()->activeControlPoint2()) {
                        firstSegments.second.first()->setControlPoint2(lastSegments.first.first()->controlPoint2());
                    }
                }
                else {
                    // if there is no start marker we need the first point needs to be changed via the preLastPoint
                    // the flag needs to be set so the moveTo is done
                    lastSegments.first.first()->setProperty(KoPathPoint::StartSubpath);
                    preLastPoint = (*firstSubpath)[firstSubpath->count()-2];
                }
            }
            else {
                if (lastSegment.first()->activeControlPoint1()) {
                    lastSegments.first.first()->setControlPoint1(lastSegment.first()->controlPoint1());
                }
                preLastPoint = (*firstSubpath)[firstSubpath->count()-2];
            }
        }
    }

    stroker.setWidth(pen.widthF());
    stroker.setJoinStyle(pen.joinStyle());
    stroker.setMiterLimit(pen.miterLimit());
    stroker.setCapStyle(pen.capStyle());
    stroker.setDashOffset(pen.dashOffset());
    stroker.setDashPattern(pen.dashPattern());

    // shortent the path to make it look nice
    // replace the point temporarily in case there is an arrow
    // BE AWARE: this changes the content of the path so that outline give the correct values.
    if (firstPoint) {
        firstSubpath->first() = firstSegments.second.first();
        if (secondPoint) {
            (*firstSubpath)[1] = firstSegments.second.second();
        }
    }
    if (lastPoint) {
        if (preLastPoint) {
            (*firstSubpath)[firstSubpath->count() - 2] = lastSegments.first.first();
        }
        firstSubpath->last() = lastSegments.first.second();
    }

    QPainterPath path = stroker.createStroke(createVariableWidthOutline());

    if (firstPoint) {
        firstSubpath->first() = firstPoint;
        if (secondPoint) {
            (*firstSubpath)[1] = secondPoint;
        }
    }
    if (lastPoint) {
        if (preLastPoint) {
            (*firstSubpath)[firstSubpath->count() - 2] = preLastPoint;
        }
        firstSubpath->last() = lastPoint;
    }

    pathOutline.addPath(path);
    pathOutline.setFillRule(Qt::WindingFill);

    return pathOutline;
}  

qreal VariableLineShape::widthPercentage() const
{
    return m_widthPercentage;
}

QString VariableLineShape::pathShapeId() const
{
    return VariableLineShapeId;
}
