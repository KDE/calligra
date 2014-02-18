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

struct KisPerspectiveGridNgHandle::Private {
    QList<KisPerspectiveGridNg*> assistants;
    char handle_type;
};

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

char KisPerspectiveGridNgHandle::handleType()
{
    return d->handle_type;
}


KisPerspectiveGridNgHandle::~KisPerspectiveGridNgHandle()
{
    Q_ASSERT(d->assistants.empty());
    delete d;
}

void KisPerspectiveGridNgHandle::registerAssistant(KisPerspectiveGridNg* assistant)
{
    Q_ASSERT(!d->assistants.contains(assistant));
    d->assistants.append(assistant);
}

void KisPerspectiveGridNgHandle::unregisterAssistant(KisPerspectiveGridNg* assistant)
{
    d->assistants.removeOne(assistant);
    Q_ASSERT(!d->assistants.contains(assistant));
}

bool KisPerspectiveGridNgHandle::containsAssistant(KisPerspectiveGridNg* assistant)
{
    return d->assistants.contains(assistant);
}

void KisPerspectiveGridNgHandle::mergeWith(KisPerspectiveGridNgHandleSP handle)
{
    if(this->handleType()=='S' || handle.data()->handleType()== 'S')
        return;
    foreach(KisPerspectiveGridNg* assistant, handle->d->assistants) {
        if (!assistant->handles().contains(this)) {
            assistant->replaceHandle(handle, this);
        }
    }
}

QList<KisPerspectiveGridNgHandleSP> KisPerspectiveGridNgHandle::split()
{
    QList<KisPerspectiveGridNgHandleSP> newHandles;
    foreach(KisPerspectiveGridNg* assistant, d->assistants) {
        KisPerspectiveGridNgHandleSP newHandle(new KisPerspectiveGridNgHandle(*this));
        newHandles.append(newHandle);
        assistant->replaceHandle(this, newHandle);
    }
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
    KisPerspectiveGridNgHandleSP topLeft, bottomLeft, topRight, bottomRight, topMiddle, bottomMiddle, rightMiddle, leftMiddle;
    struct TranslationInvariantTransform {
        qreal m11, m12, m21, m22;
        TranslationInvariantTransform() { }
        TranslationInvariantTransform(const QTransform& t) : m11(t.m11()), m12(t.m12()), m21(t.m21()), m22(t.m22()) { }
        bool operator==(const TranslationInvariantTransform& b) {
            return m11 == b.m11 && m12 == b.m12 && m21 == b.m21 && m22 == b.m22;
        }
    } cachedTransform;
};

KisPerspectiveGridNg::KisPerspectiveGridNg(const QString& id, const QString& name) : d(new Private)
{
    d->id = id;
    d->name = name;
}

void KisPerspectiveGridNg::drawPath(QPainter& painter, const QPainterPath &path)
{
    painter.save();
    QPen pen_a(QColor(0, 0, 0, 100), 2);
    pen_a.setCosmetic(true);
    painter.setPen(pen_a);
    painter.drawPath(path);
    QPen pen_b(Qt::white, 0.9);
    pen_b.setCosmetic(true);
    painter.setPen(pen_b);
    painter.drawPath(path);
    painter.restore();
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

void KisPerspectiveGridNg::drawAssistant(QPainter& gc, const QRectF& updateRect, const KisCoordinatesConverter* converter, bool useCache,KisCanvas2* canvas)
{
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
    const QTransform transform = converter->documentToWidgetTransform();
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
    if (d->handles.size() == 4 && d->id == "perspective") {
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
        if(!d->bottomMiddle && !d->topMiddle && !d->leftMiddle && !d->rightMiddle) {
            d->bottomMiddle = new KisPerspectiveGridNgHandle((d->bottomLeft.data()->x() + d->bottomRight.data()->x())*0.5,
                                                             (d->bottomLeft.data()->y() + d->bottomRight.data()->y())*0.5);
            d->topMiddle = new KisPerspectiveGridNgHandle((d->topLeft.data()->x() + d->topRight.data()->x())*0.5,
                                                             (d->topLeft.data()->y() + d->topRight.data()->y())*0.5);
            d->rightMiddle= new KisPerspectiveGridNgHandle((d->topRight.data()->x() + d->bottomRight.data()->x())*0.5,
                                                             (d->topRight.data()->y() + d->bottomRight.data()->y())*0.5);
            d->leftMiddle= new KisPerspectiveGridNgHandle((d->bottomLeft.data()->x() + d->topLeft.data()->x())*0.5,
                                                             (d->bottomLeft.data()->y() + d->topLeft.data()->y())*0.5);
            addSideHandle(d->rightMiddle.data());
            addSideHandle(d->leftMiddle.data());
            addSideHandle(d->bottomMiddle.data());
            addSideHandle(d->topMiddle.data());
        }
        else
        {
            d->bottomMiddle.data()->operator =(QPointF((d->bottomLeft.data()->x() + d->bottomRight.data()->x())*0.5,
                                                             (d->bottomLeft.data()->y() + d->bottomRight.data()->y())*0.5));
            d->topMiddle.data()->operator =(QPointF((d->topLeft.data()->x() + d->topRight.data()->x())*0.5,
                                                             (d->topLeft.data()->y() + d->topRight.data()->y())*0.5));
            d->rightMiddle.data()->operator =(QPointF((d->topRight.data()->x() + d->bottomRight.data()->x())*0.5,
                                                             (d->topRight.data()->y() + d->bottomRight.data()->y())*0.5));
            d->leftMiddle.data()->operator =(QPointF((d->bottomLeft.data()->x() + d->topLeft.data()->x())*0.5,
                                                             (d->bottomLeft.data()->y() + d->topLeft.data()->y())*0.5));
        }

    }
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

