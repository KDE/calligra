/*
 *  Copyright (c) 2014 Shivaraman Aiyer<sra392@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "kis_perspective_gridng.h"
#include "kis_coordinates_converter.h"
#include "kis_debug.h"
#include <kis_canvas2.h>

#include <kglobal.h>
#include <QPen>
#include <QPainter>
#include <QPixmapCache>
#include <KoStore.h>

#include <math.h>
#include <limits>

struct KisPerspectiveGridNgHandle::Private {
    QList<KisPerspectiveGridNg*> assistants;
    char handle_type;
};

// squared distance from a point to a line
inline qreal distsqr(const QPointF& pt, const QLineF& line)
{
    // distance = |(p2 - p1) x (p1 - pt)| / |p2 - p1|

    // magnitude of (p2 - p1) x (p1 - pt)
    const qreal cross = (line.dx() * (line.y1() - pt.y()) - line.dy() * (line.x1() - pt.x()));

    return cross * cross / (line.dx() * line.dx() + line.dy() * line.dy());
}

KisPerspectiveGridNgHandle::KisPerspectiveGridNgHandle(double x, double y) : QPointF(x, y), d(new Private)
{
}
KisPerspectiveGridNgHandle::KisPerspectiveGridNgHandle(QPointF p) : QPointF(p), d(new Private)
{
}

KisPerspectiveGridNgHandle::KisPerspectiveGridNgHandle(const KisPerspectiveGridNgHandle& rhs)
    : QPointF(rhs)
    , KisShared()
    , d(new Private)
{
}

KisPerspectiveGridNgHandle& KisPerspectiveGridNgHandle::operator=(const QPointF &  pt)
{
    setX(pt.x());
    setY(pt.y());
    return *this;
}

void KisPerspectiveGridNgHandle::setType(char type)
{
    d->handle_type = type;
}

const char KisPerspectiveGridNgHandle::handleType() const
{
    return d->handle_type;
}


KisPerspectiveGridNgHandle::~KisPerspectiveGridNgHandle()
{
//    Q_ASSERT(d->assistants.empty());
//    delete d;
}

void KisPerspectiveGridNgHandle::registerAssistant(KisPerspectiveGridNg* assistant)
{
//    Q_ASSERT(!d->assistants.contains(assistant));
//    d->assistants.append(assistant);
}

void KisPerspectiveGridNgHandle::unregisterAssistant(KisPerspectiveGridNg* assistant)
{
//    d->assistants.removeOne(assistant);
//    Q_ASSERT(!d->assistants.contains(assistant));
}

bool KisPerspectiveGridNgHandle::containsAssistant(KisPerspectiveGridNg* assistant)
{
    return d->assistants.contains(assistant);
}

void KisPerspectiveGridNgHandle::mergeWith(KisPerspectiveGridNgHandleSP handle)
{
//    if(this->handleType()=='S' || handle.data()->handleType()== 'S')
//        return;
//    foreach(KisPerspectiveGridNg* assistant, handle->d->assistants) {
//        if (!assistant->handles().contains(this)) {
//            assistant->replaceHandle(handle, this);
//        }
//    }
}

QList<KisPerspectiveGridNgHandleSP> KisPerspectiveGridNgHandle::split()
{
    QList<KisPerspectiveGridNgHandleSP> newHandles;
//    foreach(KisPerspectiveGridNg* assistant, d->assistants) {
//        KisPerspectiveGridNgHandleSP newHandle(new KisPerspectiveGridNgHandle(*this));
//        newHandles.append(newHandle);
//        assistant->replaceHandle(this, newHandle);
//    }
    return newHandles;
}

void KisPerspectiveGridNgHandle::uncache()
{
    foreach(KisPerspectiveGridNg* assistant, d->assistants) {
        assistant->uncache();
    }
}


struct KisPerspectiveGridNg::Private {
    QString id;
    QString name;
    QList<KisPerspectiveGridNgHandleSP> handles,sideHandles;
    QPixmapCache::Key cached;
    QRect cachedRect; // relative to boundingRect().topLeft()
    KisPerspectiveGridNgHandleSP topLeft, bottomLeft, topRight, bottomRight, topMiddle, bottomMiddle, rightMiddle, leftMiddle, vanishingPointX,vanishingPointY,vanishingPointZ;
    struct TranslationInvariantTransform {
        qreal m11, m12, m21, m22;
        TranslationInvariantTransform() { }
        TranslationInvariantTransform(const QTransform& t) : m11(t.m11()), m12(t.m12()), m21(t.m21()), m22(t.m22()) { }
        bool operator==(const TranslationInvariantTransform& b) {
            return m11 == b.m11 && m12 == b.m12 && m21 == b.m21 && m22 == b.m22;
        }
    } cachedTransform;
};

KisPerspectiveGridNg::KisPerspectiveGridNg()
        : KisPerspectiveGridNg("perspectivegridng", i18n("Perspective GridNg"))
{
}

KisPerspectiveGridNg::KisPerspectiveGridNg(const QString& id, const QString& name) : d(new Private)
{
    d->id = id;
    d->name = name;
}

void KisPerspectiveGridNg::drawPath(QPainter& painter, const QPainterPath &path)
{
    painter.save();
    QPen pen_a(Qt::black, 2);
    pen_a.setCosmetic(true);
    painter.setPen(pen_a);
    painter.drawPath(path);
//    QPen pen_b(Qt::white, 0.9);
//    pen_b.setCosmetic(true);
//    painter.setPen(pen_b);
//    painter.drawPath(path);
    painter.restore();
}

void KisPerspectiveGridNg::drawPath(QPainter& painter, const QPainterPath &path, QColor colour)
{
    painter.save();
    QPen pen_a(colour, 2);
    pen_a.setCosmetic(true);
    painter.setPen(pen_a);
    painter.setBrush(QColor(0,0,0,125));
    painter.drawPath(path);

//    QPen pen_b(Qt::white, 0.9);
//    pen_b.setCosmetic(true);
//    painter.setPen(pen_b);
//    painter.drawPath(path);
    painter.restore();
}

//qreal KisPerspectiveGridNg::distance(const QPointF& pt) const
//{
//    QPolygonF poly;
//    QTransform transform;
//    if (!getTransform(poly, transform)) return 1.0;
//    bool invertible;
//    QTransform inverse = transform.inverted(&invertible);
//    if (!invertible) return 1.0;
//    if (inverse.m13() * pt.x() + inverse.m23() * pt.y() + inverse.m33() == 0.0) {
//        // point at infinity
//        return 0.0;
//    }
//    return localScale(transform, inverse.map(pt)) * inverseMaxLocalScale(transform);
//}

bool KisPerspectiveGridNg::getTransform(QPolygonF& poly, QTransform& transform) const
{
    if (m_cachedPolygon.size() != 0 && handles().size() == 4) {
        for (int i = 0; i <= 4; ++i) {
            if (i == 4) {
                poly = m_cachedPolygon;
                transform = m_cachedTransform;
                return m_cacheValid;
            }
            if (m_cachedPoints[i] != *handles()[i]) break;
        }
    }
    m_cachedPolygon.clear();
    m_cacheValid = false;
    if (!quad(poly)) {
        m_cachedPolygon = poly;
        return false;
    }
    if (!QTransform::squareToQuad(poly, transform)) {
        qWarning("Failed to create perspective mapping");
        return false;
    }
    for (int i = 0; i < 4; ++i) {
        m_cachedPoints[i] = *handles()[i];
    }
    m_cachedPolygon = poly;
    m_cachedTransform = transform;
    m_cacheValid = true;
    return true;
}


void KisPerspectiveGridNg::initHandles(QList<KisPerspectiveGridNgHandleSP> _handles)
{
    Q_ASSERT(d->handles.isEmpty());
    d->handles = _handles;
    foreach(KisPerspectiveGridNgHandleSP handle, _handles) {
        handle->registerAssistant(this);
    }
}

KisPerspectiveGridNg::~KisPerspectiveGridNg()
{
    foreach(KisPerspectiveGridNgHandleSP handle, d->handles) {
        handle->unregisterAssistant(this);
    }
    if(!d->sideHandles.isEmpty()) {
        foreach(KisPerspectiveGridNgHandleSP handle, d->sideHandles) {
            handle->unregisterAssistant(this);
        }
    }
    delete d;
}

const QString& KisPerspectiveGridNg::id() const
{
    return d->id;
}

const QString& KisPerspectiveGridNg::name() const
{
    return d->name;
}

QPointF KisPerspectiveGridNg::adjustPosition(const QPointF& pt, const QPointF& strokeBegin)
{
    return project(pt, strokeBegin);
}

void KisPerspectiveGridNg::endStroke()
{
    m_snapLine = QLineF();
}

QPointF KisPerspectiveGridNg::buttonPosition() const
{
    QPointF centroid(0, 0);
    for (int i = 0; i < 4; ++i) centroid += *handles()[i];
    return centroid * 0.25;
}



void KisPerspectiveGridNg::replaceHandle(KisPerspectiveGridNgHandleSP _handle, KisPerspectiveGridNgHandleSP _with)
{
    Q_ASSERT(d->handles.contains(_handle));
    d->handles.replace(d->handles.indexOf(_handle), _with);
    Q_ASSERT(!d->handles.contains(_handle));
    _handle->unregisterAssistant(this);
    _with->registerAssistant(this);
}

void KisPerspectiveGridNg::addHandle(KisPerspectiveGridNgHandleSP handle)
{
    Q_ASSERT(!d->handles.contains(handle));
    d->handles.append(handle);
    handle->registerAssistant(this);
    handle.data()->setType('H');
}

void KisPerspectiveGridNg::addSideHandle(KisPerspectiveGridNgHandleSP handle)
{
    Q_ASSERT(!d->sideHandles.contains(handle));
    d->sideHandles.append(handle);
    handle->registerAssistant(this);
    handle.data()->setType('S');
}

inline QPainterPath drawX(const QPointF& pt)
{
    QPainterPath path;
    path.moveTo(QPointF(pt.x() - 5.0, pt.y() - 5.0)); path.lineTo(QPointF(pt.x() + 5.0, pt.y() + 5.0));
    path.moveTo(QPointF(pt.x() - 5.0, pt.y() + 5.0)); path.lineTo(QPointF(pt.x() + 5.0, pt.y() - 5.0));
    return path;
}

void KisPerspectiveGridNg::drawAssistant(QPainter& gc, const QRectF& updateRect, const KisCoordinatesConverter* converter, bool useCache,KisCanvas2* canvas)
{
    gc.save();
    gc.resetTransform();
    QTransform initialTransform = converter->documentToWidgetTransform();
    QPolygonF poly;
    QTransform transform; // unused, but computed for caching purposes
    if (getTransform(poly, transform)) {
        // draw vanishing points
        QPointF intersection(0, 0);
        if (QLineF(poly[0], poly[1]).intersect(QLineF(poly[2], poly[3]), &intersection) != QLineF::NoIntersection) {
            drawPath(gc, drawX(initialTransform.map(intersection)));
        }
        if (QLineF(poly[1], poly[2]).intersect(QLineF(poly[3], poly[0]), &intersection) != QLineF::NoIntersection) {
            drawPath(gc, drawX(initialTransform.map(intersection)));
        }
    }
    gc.restore();

    Q_UNUSED(updateRect);
    Q_UNUSED(canvas);
    findHandleLocation();
    if (!useCache) {
        gc.save();
        drawCache(gc, converter);
        gc.restore();
        return;
    }
    const QRect bound = boundingRect();
    if (bound.isEmpty()) return;
    /*const QTransform */transform = converter->documentToWidgetTransform();
    const QRect widgetBound = transform.mapRect(bound);

    const QRect paintRect = transform.mapRect(bound).intersected(gc.viewport());
    if (paintRect.isEmpty()) return;

    QPixmap cached;
    if (!(QPixmapCache::find(d->cached, &cached) &&
          d->cachedTransform == transform &&
          d->cachedRect.translated(widgetBound.topLeft()).contains(paintRect))) {
        const QRect cacheRect = gc.viewport().adjusted(-100, -100, 100, 100).intersected(widgetBound);
        Q_ASSERT(!cacheRect.isEmpty());
        if (cached.isNull() || cached.size() != cacheRect.size()) {
            cached = QPixmap(cacheRect.size());
        }
        cached.fill(Qt::transparent);
        QPainter painter(&cached);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setWindow(cacheRect);
        drawCache(painter, converter);
        painter.end();
        d->cachedTransform = transform;
        d->cachedRect = cacheRect.translated(-widgetBound.topLeft());
        d->cached = QPixmapCache::insert(cached);
    }
    gc.drawPixmap(paintRect, cached, paintRect.translated(-widgetBound.topLeft() - d->cachedRect.topLeft()));
}



void KisPerspectiveGridNg::drawCache(QPainter& gc, const KisCoordinatesConverter *converter)
{
    gc.setTransform(converter->documentToWidgetTransform());
    QPolygonF poly;
    QTransform transform;
    if (!getTransform(poly, transform)) {
        // color red for an invalid transform, but not for an incomplete one
        if(handles().size() == 4)
        {
            gc.setPen(QColor(255, 0, 0, 125));
            gc.drawPolygon(poly);
        } else {
            QPainterPath path;
            path.addPolygon(poly);
            drawPath(gc, path);
        }
    } else {
        gc.setPen(QColor(0, 0, 0, 125));
        gc.setTransform(transform, true);
        QPainterPath path;
        for (int y = 0; y <= 8;)
        {
            path.moveTo(QPointF(0.0, y * 0.125));
            path.lineTo(QPointF(1.0, y * 0.125));
            y+=8;
        }
        for (int x = 0; x <= 8; )
        {
            path.moveTo(QPointF(x * 0.125, 0.0));
            path.lineTo(QPointF(x * 0.125, 1.0));
            x+=8;
        }

        drawPath(gc, path);
    }
}


void KisPerspectiveGridNg::uncache()
{
    d->cached = QPixmapCache::Key();
}

QRect KisPerspectiveGridNg::boundingRect() const
{
    QRectF r;
    foreach (KisPerspectiveGridNgHandleSP h, handles()) {
        r = r.united(QRectF(*h, QSizeF(1,1)));
    }
    return r.adjusted(-2, -2, 2, 2).toAlignedRect();
}

QByteArray KisPerspectiveGridNg::saveXml(QMap<KisPerspectiveGridNgHandleSP, int> &handleMap)
{
        QByteArray data;
        QXmlStreamWriter xml(&data);
            xml.writeStartDocument();
            xml.writeStartElement("assistant");
            xml.writeAttribute("type",d->id);
            xml.writeStartElement("handles");
            foreach(const KisPerspectiveGridNgHandleSP handle, d->handles) {
                int id = handleMap.size();
                if (!handleMap.contains(handle)){
                    handleMap.insert(handle, id);
                }
                id = handleMap.value(handle);
                xml.writeStartElement("handle");
                xml.writeAttribute("id", QString::number(id));
                xml.writeAttribute("x", QString::number(double(handle->x()), 'f', 3));
                xml.writeAttribute("y", QString::number(double(handle->y()), 'f', 3));
                xml.writeEndElement();
            }
            xml.writeEndElement();
            xml.writeEndElement();
            xml.writeEndDocument();
            return data;
}

void KisPerspectiveGridNg::loadXml(KoStore* store, QMap<int, KisPerspectiveGridNgHandleSP> &handleMap, QString path)
{
    int id;
    double x,y ;
    store->open(path);
    QByteArray data = store->read(store->size());
    QXmlStreamReader xml(data);
    while (!xml.atEnd()) {
        switch (xml.readNext()) {
        case QXmlStreamReader::StartElement:
            if (xml.name() == "handle") {
                QString strId = xml.attributes().value("id").toString(),
                strX = xml.attributes().value("x").toString(),
                strY = xml.attributes().value("y").toString();
                if (!strId.isEmpty() && !strX.isEmpty() && !strY.isEmpty()) {
                    id = strId.toInt();
                    x = strX.toDouble();
                    y = strY.toDouble();
                    if (!handleMap.contains(id)) {
                        handleMap.insert(id, new KisPerspectiveGridNgHandle(x, y));
                    }
                }
                addHandle(handleMap.value(id));
            }
            break;
        default:
            break;
        }
    }
    store->close();
}

void KisPerspectiveGridNg::saveXmlList(QDomDocument& doc, QDomElement& assistantsElement,int count)
{
    if (d->id == "ellipse"){
        QDomElement assistantElement = doc.createElement("assistant");
        assistantElement.setAttribute("type", "ellipse");
        assistantElement.setAttribute("filename", QString("ellipse%1.assistant").arg(count));
        assistantsElement.appendChild(assistantElement);
    }
    else if (d->id == "spline"){
        QDomElement assistantElement = doc.createElement("assistant");
        assistantElement.setAttribute("type", "spline");
        assistantElement.setAttribute("filename", QString("spline%1.assistant").arg(count));
        assistantsElement.appendChild(assistantElement);
    }
    else if (d->id == "perspective"){
        QDomElement assistantElement = doc.createElement("assistant");
        assistantElement.setAttribute("type", "perspective");
        assistantElement.setAttribute("filename", QString("perspective%1.assistant").arg(count));
        assistantsElement.appendChild(assistantElement);
    }
    else if (d->id == "ruler"){
        QDomElement assistantElement = doc.createElement("assistant");
        assistantElement.setAttribute("type", "ruler");
        assistantElement.setAttribute("filename", QString("ruler%1.assistant").arg(count));
        assistantsElement.appendChild(assistantElement);
    }
}

void KisPerspectiveGridNg::findHandleLocation() {
    QList<KisPerspectiveGridNgHandleSP> hHandlesList;
    QList<KisPerspectiveGridNgHandleSP> vHandlesList;
    uint vHole = 0,hHole = 0;
    KisPerspectiveGridNgHandleSP oppHandle;
    if (d->handles.size() == 4) {
        //get the handle opposite to the first handle
        oppHandle = oppHandleOne();
        //Sorting handles into two list, X sorted and Y sorted into hHandlesList and vHandlesList respectively.
        foreach(const KisPerspectiveGridNgHandleSP handle,d->handles) {
            hHandlesList.append(handle);
            hHole = hHandlesList.size() - 1;
            vHandlesList.append(handle);
            vHole = vHandlesList.size() - 1;
            /*
             sort handles on the basis of X-coordinate
             */
            while(hHole > 0 && hHandlesList.at(hHole -1).data()->x() > handle.data()->x()) {
                hHandlesList.swap(hHole-1, hHole);
                hHole = hHole - 1;
            }
            /*
             sort handles on the basis of Y-coordinate
             */
            while(vHole > 0 && vHandlesList.at(vHole -1).data()->y() > handle.data()->y()) {
                vHandlesList.swap(vHole-1, vHole);
                vHole = vHole - 1;
            }
        }

        /*
         give the handles their respective positions
         */
        if(vHandlesList.at(0).data()->x() > vHandlesList.at(1).data()->x()) {
//            qDebug() << "Shiva: VCount:" vHandlesList.count();
            d->topLeft = vHandlesList.at(1);
            d->topRight= vHandlesList.at(0);
        }
        else {
            d->topLeft = vHandlesList.at(0);
            d->topRight = vHandlesList.at(1);
        }
        if(vHandlesList.at(2).data()->x() > vHandlesList.at(3).data()->x()) {
            d->bottomLeft = vHandlesList.at(3);
            d->bottomRight = vHandlesList.at(2);
        }
        else {
            d->bottomLeft= vHandlesList.at(2);
            d->bottomRight = vHandlesList.at(3);
        }

        /*
         find if the handles that should be opposite are actually oppositely positioned
         */
        if (( (d->topLeft == d->handles.at(0).data() && d->bottomRight == oppHandle) ||
            (d->topLeft == oppHandle && d->bottomRight == d->handles.at(0).data()) ||
            (d->topRight == d->handles.at(0).data() && d->bottomLeft == oppHandle) ||
            (d->topRight == oppHandle && d->bottomLeft == d->handles.at(0).data()) ) )
        {}
        else {
            if(hHandlesList.at(0).data()->y() > hHandlesList.at(1).data()->y()) {
                d->topLeft = hHandlesList.at(1);
                d->bottomLeft= hHandlesList.at(0);
            }
            else {
                d->topLeft = hHandlesList.at(0);
                d->bottomLeft = hHandlesList.at(1);
            }
            if(hHandlesList.at(2).data()->y() > hHandlesList.at(3).data()->y()) {
                d->topRight = hHandlesList.at(3);
                d->bottomRight = hHandlesList.at(2);
            }
            else {
                d->topRight= hHandlesList.at(2);
                d->bottomRight = hHandlesList.at(3);
            }

        }
        /*
         Setting the middle handles as needed
         */
//        if(!d->bottomMiddle && !d->topMiddle && !d->leftMiddle && !d->rightMiddle) {
//            d->bottomMiddle = new KisPerspectiveGridNgHandle((d->bottomLeft.data()->x() + d->bottomRight.data()->x())*0.5,
//                                                             (d->bottomLeft.data()->y() + d->bottomRight.data()->y())*0.5);
//            d->topMiddle = new KisPerspectiveGridNgHandle((d->topLeft.data()->x() + d->topRight.data()->x())*0.5,
//                                                             (d->topLeft.data()->y() + d->topRight.data()->y())*0.5);
//            d->rightMiddle= new KisPerspectiveGridNgHandle((d->topRight.data()->x() + d->bottomRight.data()->x())*0.5,
//                                                             (d->topRight.data()->y() + d->bottomRight.data()->y())*0.5);
//            d->leftMiddle= new KisPerspectiveGridNgHandle((d->bottomLeft.data()->x() + d->topLeft.data()->x())*0.5,
//                                                             (d->bottomLeft.data()->y() + d->topLeft.data()->y())*0.5);
//            addSideHandle(d->rightMiddle.data());
//            addSideHandle(d->leftMiddle.data());
//            addSideHandle(d->bottomMiddle.data());
//            addSideHandle(d->topMiddle.data());
//        }
//        else
//        {
//            d->bottomMiddle.data()->operator =(QPointF((d->bottomLeft.data()->x() + d->bottomRight.data()->x())*0.5,
//                                                             (d->bottomLeft.data()->y() + d->bottomRight.data()->y())*0.5));
//            d->topMiddle.data()->operator =(QPointF((d->topLeft.data()->x() + d->topRight.data()->x())*0.5,
//                                                             (d->topLeft.data()->y() + d->topRight.data()->y())*0.5));
//            d->rightMiddle.data()->operator =(QPointF((d->topRight.data()->x() + d->bottomRight.data()->x())*0.5,
//                                                             (d->topRight.data()->y() + d->bottomRight.data()->y())*0.5));
//            d->leftMiddle.data()->operator =(QPointF((d->bottomLeft.data()->x() + d->topLeft.data()->x())*0.5,
//                                                             (d->bottomLeft.data()->y() + d->topLeft.data()->y())*0.5));
//        }

        if(!d->vanishingPointX && !d->vanishingPointY && !d->vanishingPointZ){
            d->vanishingPointZ = new KisPerspectiveGridNgHandle((d->topLeft.data()->x() + d->topRight.data()->x())*0.5,
                                                                (d->topLeft.data()->y() + d->topRight.data()->y())*0.5 - 16);
            d->vanishingPointY= new KisPerspectiveGridNgHandle((d->topRight.data()->x() + d->bottomRight.data()->x())*0.5 + 16,
                                                                (d->topRight.data()->y() + d->bottomRight.data()->y())*0.5);
            d->vanishingPointX= new KisPerspectiveGridNgHandle((d->bottomLeft.data()->x() + d->topLeft.data()->x())*0.5,
                                                                (d->bottomLeft.data()->y() + d->topLeft.data()->y())*0.5);
            d->vanishingPointX.data()->setType('v');
            d->vanishingPointY.data()->setType('v');
            d->vanishingPointZ.data()->setType('v');
        }

    }
}

QPointF KisPerspectiveGridNg::project(const QPointF& pt, const QPointF& strokeBegin)
{
    const static QPointF nullPoint(std::numeric_limits<qreal>::quiet_NaN(), std::numeric_limits<qreal>::quiet_NaN());
    Q_ASSERT(handles().size() == 4);
    if (m_snapLine.isNull()) {
        QPolygonF poly;
        QTransform transform;
        if (!getTransform(poly, transform)) return nullPoint;
        // avoid problems with multiple assistants: only snap if starting in the grid
        if (!poly.containsPoint(strokeBegin, Qt::OddEvenFill)) return nullPoint;

        const qreal
            dx = pt.x() - strokeBegin.x(),
            dy = pt.y() - strokeBegin.y();
        if (dx * dx + dy * dy < 4.0) {
            // allow some movement before snapping
            return strokeBegin;
        }

        // construct transformation
        bool invertible;
        const QTransform inverse = transform.inverted(&invertible);
        if (!invertible) return nullPoint; // shouldn't happen

        // figure out which direction to go
        const QPointF start = inverse.map(strokeBegin);
        const QLineF
            verticalLine = QLineF(strokeBegin, transform.map(start + QPointF(0, 1))),
            horizontalLine = QLineF(strokeBegin, transform.map(start + QPointF(1, 0)));
        // determine whether the horizontal or vertical line is closer to the point
        m_snapLine = distsqr(pt, verticalLine) < distsqr(pt, horizontalLine) ? verticalLine : horizontalLine;
    }

    // snap to line
    const qreal
        dx = m_snapLine.dx(),
        dy = m_snapLine.dy(),
        dx2 = dx * dx,
        dy2 = dy * dy,
        invsqrlen = 1.0 / (dx2 + dy2);
    QPointF r(dx2 * pt.x() + dy2 * m_snapLine.x1() + dx * dy * (pt.y() - m_snapLine.y1()),
              dx2 * m_snapLine.y1() + dy2 * pt.y() + dx * dy * (pt.x() - m_snapLine.x1()));
    r *= invsqrlen;
    return r;
    return QPointF(0,0);
}

// perpendicular dot product
inline qreal pdot(const QPointF& a, const QPointF& b)
{
    return a.x() * b.y() - a.y() * b.x();
}

template <typename T> int sign(T a)
{
    return (a > 0) - (a < 0);
}


bool KisPerspectiveGridNg::quad(QPolygonF& poly) const
{
    for (int i = 0; i < handles().size(); ++i)
        poly.push_back(*handles()[i]);
    if (handles().size() != 4) {
        return false;
    }
    int sum = 0;
    int signs[4];
    for (int i = 0; i < 4; ++i) {
        int j = (i == 3) ? 0 : (i + 1);
        int k = (j == 3) ? 0 : (j + 1);
        signs[i] = sign(pdot(poly[j] - poly[i], poly[k] - poly[j]));
        sum += signs[i];
    }
    if (sum == 0) {
        // complex (crossed)
        for (int i = 0; i < 4; ++i) {
            int j = (i == 3) ? 0 : (i + 1);
            if (signs[i] * signs[j] == -1) {
                // opposite signs: uncross
                qSwap(poly[i], poly[j]);
                return true;
            }
        }
        // okay, maybe it's just a line
        return false;
    } else if (sum != 4 && sum != -4) {
        // concave, or a triangle
        if (sum == 2 || sum == -2) {
            // concave, let's return a triangle instead
            for (int i = 0; i < 4; ++i) {
                int j = (i == 3) ? 0 : (i + 1);
                if (signs[i] != sign(sum)) {
                    // wrong sign: drop the inside node
                    poly.remove(j);
                    return false;
                }
            }
        }
        return false;
    }
    // convex
    return true;
}



KisPerspectiveGridNgHandleSP KisPerspectiveGridNg::oppHandleOne()
{
    QPointF intersection(0,0);
    if((QLineF(d->handles.at(0).data()->toPoint(),d->handles.at(1).data()->toPoint()).intersect(QLineF(d->handles.at(2).data()->toPoint(),d->handles.at(3).data()->toPoint()), &intersection) != QLineF::NoIntersection)
            && (QLineF(d->handles.at(0).data()->toPoint(),d->handles.at(1).data()->toPoint()).intersect(QLineF(d->handles.at(2).data()->toPoint(),d->handles.at(3).data()->toPoint()), &intersection) != QLineF::UnboundedIntersection))
    {
        return d->handles.at(1);
    }
    else if((QLineF(d->handles.at(0).data()->toPoint(),d->handles.at(2).data()->toPoint()).intersect(QLineF(d->handles.at(1).data()->toPoint(),d->handles.at(3).data()->toPoint()), &intersection) != QLineF::NoIntersection)
            && (QLineF(d->handles.at(0).data()->toPoint(),d->handles.at(2).data()->toPoint()).intersect(QLineF(d->handles.at(1).data()->toPoint(),d->handles.at(3).data()->toPoint()), &intersection) != QLineF::UnboundedIntersection))
    {
        return d->handles.at(2);
    }
    else
    {
        return d->handles.at(3);
    }
            //return d->handles.at(0);//TODO: comment this line while running
}

KisPerspectiveGridNgHandleSP KisPerspectiveGridNg::topLeft()
{
    return d->topLeft;
}

const KisPerspectiveGridNgHandleSP KisPerspectiveGridNg::topLeft() const
{
    return d->topLeft;
}

KisPerspectiveGridNgHandleSP KisPerspectiveGridNg::bottomLeft()
{
    return d->bottomLeft;
}

const KisPerspectiveGridNgHandleSP KisPerspectiveGridNg::bottomLeft() const
{
    return d->bottomLeft;
}

KisPerspectiveGridNgHandleSP KisPerspectiveGridNg::topRight()
{
    return d->topRight;
}

const KisPerspectiveGridNgHandleSP KisPerspectiveGridNg::topRight() const
{
    return d->topRight;
}

KisPerspectiveGridNgHandleSP KisPerspectiveGridNg::bottomRight()
{
    return d->bottomRight;
}

const KisPerspectiveGridNgHandleSP KisPerspectiveGridNg::bottomRight() const
{
    return d->bottomRight;
}

KisPerspectiveGridNgHandleSP KisPerspectiveGridNg::topMiddle()
{
    return d->topMiddle;
}

const KisPerspectiveGridNgHandleSP KisPerspectiveGridNg::topMiddle() const
{
    return d->topMiddle;
}

KisPerspectiveGridNgHandleSP KisPerspectiveGridNg::bottomMiddle()
{
    return d->bottomMiddle;
}

const KisPerspectiveGridNgHandleSP KisPerspectiveGridNg::bottomMiddle() const
{
    return d->bottomMiddle;
}

KisPerspectiveGridNgHandleSP KisPerspectiveGridNg::rightMiddle()
{
    return d->rightMiddle;
}

const KisPerspectiveGridNgHandleSP KisPerspectiveGridNg::rightMiddle() const
{
    return d->rightMiddle;
}

KisPerspectiveGridNgHandleSP KisPerspectiveGridNg::leftMiddle()
{
    return d->leftMiddle;
}

const KisPerspectiveGridNgHandleSP KisPerspectiveGridNg::leftMiddle() const
{
    return d->leftMiddle;
}

KisPerspectiveGridNgHandleSP KisPerspectiveGridNg::vanishingPointX()
{
    return d->vanishingPointX;
}

const KisPerspectiveGridNgHandleSP KisPerspectiveGridNg::vanishingPointX() const
{
    return d->vanishingPointX;
}

KisPerspectiveGridNgHandleSP KisPerspectiveGridNg::vanishingPointY()
{
    return d->vanishingPointY;
}

const KisPerspectiveGridNgHandleSP KisPerspectiveGridNg::vanishingPointY() const
{
    return d->vanishingPointY;
}

KisPerspectiveGridNgHandleSP KisPerspectiveGridNg::vanishingPointZ()
{
    return d->vanishingPointZ;
}

const KisPerspectiveGridNgHandleSP KisPerspectiveGridNg::vanishingPointZ() const
{
    return d->vanishingPointZ;
}

const QList<KisPerspectiveGridNgHandleSP>& KisPerspectiveGridNg::handles() const
{
    return d->handles;
}

QList<KisPerspectiveGridNgHandleSP> KisPerspectiveGridNg::handles()
{
    return d->handles;
}

const QList<KisPerspectiveGridNgHandleSP>& KisPerspectiveGridNg::sideHandles() const
{
    return d->sideHandles;
}

QList<KisPerspectiveGridNgHandleSP> KisPerspectiveGridNg::sideHandles()
{
    return d->sideHandles;
}

KisPerspectiveGridNgFactory::KisPerspectiveGridNgFactory()
{
}

KisPerspectiveGridNgFactory::~KisPerspectiveGridNgFactory()
{
}

QString KisPerspectiveGridNgFactory::id() const
{
    return "perspectivegridng";
}

QString KisPerspectiveGridNgFactory::name() const
{
    return i18n("PerspectiveGridNg");
}


KisPerspectiveGridNg* KisPerspectiveGridNgFactory::createPerspectiveGridNg() const
{
    return new KisPerspectiveGridNg;
}

KisPerspectiveGridNgFactoryRegistry::KisPerspectiveGridNgFactoryRegistry()
{
}

KisPerspectiveGridNgFactoryRegistry::~KisPerspectiveGridNgFactoryRegistry()
{
    foreach(const QString &id, keys()) {
        delete get(id);
    }
    dbgRegistry << "deleting KisPerspectiveGridNgFactoryRegistry ";
}

KisPerspectiveGridNgFactoryRegistry* KisPerspectiveGridNgFactoryRegistry::instance()
{
    K_GLOBAL_STATIC(KisPerspectiveGridNgFactoryRegistry, s_instance);
    return s_instance;
}




