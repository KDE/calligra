#include "PresentationViewPortShape.h"
#include <KoPathPoint.h>

PresentationViewPortShape::PresentationViewPortShape()
: m_cornerRadiusX(0)
, m_cornerRadiusY(0)
{
    QList<QPointF> handles;
    handles.push_back(QPointF(100, 0));
    handles.push_back(QPointF(100, 0));
    setHandles(handles);
    QSizeF size(100, 100);
    updatePath(size);
}

PresentationViewPortShape::~PresentationViewPortShape()
{

}

QString PresentationViewPortShape::pathShapeId() const
{
    return PresentationViewPortShapeId;
}

bool PresentationViewPortShape::loadOdf(const KoXmlElement& element, KoShapeLoadingContext& context)
{
    return true;
}

void PresentationViewPortShape::saveOdf(KoShapeSavingContext& context) const
{

}

qreal PresentationViewPortShape::cornerRadiusX() const
{
    return 0.0;
}

qreal PresentationViewPortShape::cornerRadiusY() const
{
    return 0.0;
}

void PresentationViewPortShape::setCornerRadiusX(qreal radius)
{

}

void PresentationViewPortShape::setCornerRadiusY(qreal radius)
{

}

void PresentationViewPortShape::moveHandleAction(int handleId, const QPointF& point, Qt::KeyboardModifiers modifiers)
{

}

void PresentationViewPortShape::updateHandles()
{
    QList<QPointF> handles;
    handles.append(QPointF(size().width() - m_cornerRadiusX/100.0 * 0.5 * size().width(), 0.0));
    handles.append(QPointF(size().width(), m_cornerRadiusY/100.0 * 0.5 * size().height()));
    setHandles(handles);
}
void PresentationViewPortShape::updatePath(const QSizeF& size)
{
    qreal rx = 0;
    qreal ry = 0;
    if (m_cornerRadiusX > 0 && m_cornerRadiusY > 0) {
        rx = size.width() / 200.0 * m_cornerRadiusX;
        ry = size.height() / 200.0 * m_cornerRadiusY;
    }

    qreal x2 = size.width() - rx;
    qreal y2 = size.height() - ry;

    QPointF curvePoints[12];

    int requiredCurvePointCount = 4;
    if (rx && m_cornerRadiusX < 100)
        requiredCurvePointCount += 2;
    if (ry && m_cornerRadiusY < 100)
        requiredCurvePointCount += 2;

    createPoints(requiredCurvePointCount);

    KoSubpath &points = *m_subpaths[0];

    int cp = 0;

    // first path starts and closes path
    points[cp]->setProperty(KoPathPoint::StartSubpath);
    points[cp]->setProperty(KoPathPoint::CloseSubpath);
    points[cp]->setPoint(QPointF(rx, 0));
    points[cp]->removeControlPoint1();
    points[cp]->removeControlPoint2();

    if (m_cornerRadiusX < 100 || m_cornerRadiusY == 0) {
        // end point of the top edge
        points[++cp]->setPoint(QPointF(x2, 0));
        points[cp]->removeControlPoint1();
        points[cp]->removeControlPoint2();
    }

    if (rx) {
        // the top right radius
        arcToCurve(rx, ry, 90, -90, points[cp]->point(), curvePoints);
        points[cp]->setControlPoint2(curvePoints[0]);
        points[++cp]->setControlPoint1(curvePoints[1]);
        points[cp]->setPoint(curvePoints[2]);
        points[cp]->removeControlPoint2();
    }

    if (m_cornerRadiusY < 100 || m_cornerRadiusX == 0) {
        // the right edge
        points[++cp]->setPoint(QPointF(size.width(), y2));
        points[cp]->removeControlPoint1();
        points[cp]->removeControlPoint2();
    }

    if (rx) {
        // the bottom right radius
        arcToCurve(rx, ry, 0, -90, points[cp]->point(), curvePoints);
        points[cp]->setControlPoint2(curvePoints[0]);
        points[++cp]->setControlPoint1(curvePoints[1]);
        points[cp]->setPoint(curvePoints[2]);
        points[cp]->removeControlPoint2();
    }

    if (m_cornerRadiusX < 100 || m_cornerRadiusY == 0) {
        // the bottom edge
        points[++cp]->setPoint(QPointF(rx, size.height()));
        points[cp]->removeControlPoint1();
        points[cp]->removeControlPoint2();
    }

    if (rx) {
        // the bottom left radius
        arcToCurve(rx, ry, 270, -90, points[cp]->point(), curvePoints);
        points[cp]->setControlPoint2(curvePoints[0]);
        points[++cp]->setControlPoint1(curvePoints[1]);
        points[cp]->setPoint(curvePoints[2]);
        points[cp]->removeControlPoint2();
    }

    if ((m_cornerRadiusY < 100 || m_cornerRadiusX == 0) && ry) {
        // the right edge
        points[++cp]->setPoint(QPointF(0, ry));
        points[cp]->removeControlPoint1();
        points[cp]->removeControlPoint2();
    }

    if (rx) {
        // the top left radius
        arcToCurve(rx, ry, 180, -90, points[cp]->point(), curvePoints);
        points[cp]->setControlPoint2(curvePoints[0]);
        points[0]->setControlPoint1(curvePoints[1]);
        points[0]->setPoint(curvePoints[2]);
    }

    // unset all stop/close path properties
    for (int i = 1; i < cp; ++i) {
        points[i]->unsetProperty(KoPathPoint::StopSubpath);
        points[i]->unsetProperty(KoPathPoint::CloseSubpath);
    }

    // last point stops and closes path
    points.last()->setProperty(KoPathPoint::StopSubpath);
    points.last()->setProperty(KoPathPoint::CloseSubpath);
}

void PresentationViewPortShape::createPoints(int requiredPointCount)
{
    if (m_subpaths.count() != 1) {
        clear();
        m_subpaths.append(new KoSubpath());
    }
    int currentPointCount = m_subpaths[0]->count();
    if (currentPointCount > requiredPointCount) {
        for (int i = 0; i < currentPointCount-requiredPointCount; ++i) {
            delete m_subpaths[0]->front();
            m_subpaths[0]->pop_front();
        }
    } else if (requiredPointCount > currentPointCount) {
        for (int i = 0; i < requiredPointCount-currentPointCount; ++i) {
            m_subpaths[0]->append(new KoPathPoint(this, QPointF()));
        }
    }
}
