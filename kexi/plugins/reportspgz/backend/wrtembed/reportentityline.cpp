/*
 * OpenRPT report writer and rendering engine
 * Copyright (C) 2001-2007 by OpenMFG, LLC
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * Please contact info@openmfg.com with any questions on this license.
 */

#include "reportentities.h"
#include "reportentityline.h"
#include "reportdesigner.h"

#include <qdom.h>
#include <qpainter.h>
#include <kdebug.h>
#include <klocalizedstring.h>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

#include <koproperty/editor.h>
#include <KoGlobal.h>
#include "reportscene.h"

//
// class ReportEntityLine
//
void ReportEntityLine::init(QGraphicsScene* s, ReportDesigner *r)
{
    _rd = r;
    setPos(0, 0);

    setFlags(ItemIsSelectable | ItemIsMovable);
    setPen(QPen(Qt::black, 5));
    setAcceptsHoverEvents(true);

    if (s)
        s->addItem(this);

    connect(_set, SIGNAL(propertyChanged(KoProperty::Set &, KoProperty::Property &)), this, SLOT(propertyChanged(KoProperty::Set &, KoProperty::Property &)));

    setZValue(Z);
}

ReportEntityLine::ReportEntityLine(ReportDesigner * d, QGraphicsScene * scene)
        : ReportEntity(d)
{
    init(scene, d);

    _name->setValue(_rd->suggestEntityName("Line"));
}

ReportEntityLine::ReportEntityLine(QDomNode & entity, ReportDesigner * d, QGraphicsScene * scene)
        : ReportEntity(d), KRLineData(entity)
{
    init(scene, d);
    setLine(_start.toScene().x(), _start.toScene().y(), _end.toScene().x(), _end.toScene().y());
}

ReportEntityLine* ReportEntityLine::clone()
{
    QDomDocument d;
    QDomElement e = d.createElement("clone");;
    QDomNode n;
    buildXML(d, e);
    n = e.firstChild();
    return new ReportEntityLine(n, designer(), 0);
}



void ReportEntityLine::paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                             QWidget *widget)
{
    //Q3CanvasLine::drawShape(painter);
    painter->setRenderHint(QPainter::Antialiasing, true);
    QPen p = painter->pen();
    painter->setPen(QPen(_lnColor->value().value<QColor>(), _lnWeight->value().toInt(), (Qt::PenStyle)_lnStyle->value().toInt()));
    painter->drawLine(line());
    if (isSelected()) {

        // draw a selected border for visual purposes
        painter->setPen(QPen(QColor(128, 128, 255), 0, Qt::DotLine));
        QPointF pt = line().p1();
        painter->fillRect(pt.x(), pt.y() - 2, 5, 5, QColor(128, 128, 255));
        pt = line().p2();
        painter->fillRect(pt.x() - 4, pt.y() - 2, 5, 5, QColor(128, 128, 255));

        painter->setPen(p);
    }
}

void ReportEntityLine::buildXML(QDomDocument & doc, QDomElement & parent)
{
    QDomElement entity = doc.createElement("line");

    qreal sx, sy, ex, ey;

    sx = _start.toPoint().x();
    sy = _start.toPoint().y();
    ex = _end.toPoint().x();
    ey = _end.toPoint().y();

    QDomElement e;

    e = doc.createElement("xstart");
    e.appendChild(doc.createTextNode(QString::number((int) sx)));
    entity.appendChild(e);

    e = doc.createElement("ystart");
    e.appendChild(doc.createTextNode(QString::number((int) sy)));
    entity.appendChild(e);

    e = doc.createElement("xend");
    e.appendChild(doc.createTextNode(QString::number((int) ex)));
    entity.appendChild(e);

    e = doc.createElement("yend");
    e.appendChild(doc.createTextNode(QString::number((int) ey)));
    entity.appendChild(e);

    // name
    QDomElement n = doc.createElement("name");
    n.appendChild(doc.createTextNode(entityName()));
    entity.appendChild(n);

    // z
    QDomElement z = doc.createElement("zvalue");
    z.appendChild(doc.createTextNode(QString::number(zValue())));
    entity.appendChild(z);

    buildXMLLineStyle(doc, entity, lineStyle());

    parent.appendChild(entity);
}

void ReportEntityLine::propertyChanged(KoProperty::Set &s, KoProperty::Property &p)
{
    //TODO KoProperty does not support QPointF
    if (p.name() == "Start") {
        //setLine ( _start.toScene().x(), _start.toScene().y(), line().p1().x(), line().p1().y() );
    } else if (p.name() == "End") {
        //setLine ( line().p2().x(), line().p2().y(),_end.toScene().x(), _end.toScene().y() );
    } else if (p.name() == "Name") {
        //For some reason p.oldValue returns an empty string
        if (!_rd->isEntityNameUnique(p.value().toString(), this)) {
            p.setValue(_oldName);
        } else {
            _oldName = p.value().toString();
        }
    }
    if (_rd) _rd->setModified(true);
    if (scene()) scene()->update();
}

void ReportEntityLine::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    _rd->changeSet(_set);
    setSelected(true);
}

QVariant ReportEntityLine::itemChange(GraphicsItemChange change, const QVariant &value)
{
    return QGraphicsItem::itemChange(change, value);
}

void ReportEntityLine::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
    int x;
    int y;

    QPointF p = mapToItem(this, dynamic_cast<ReportScene*>(scene())->gridPoint(event->scenePos()));

    x = p.x();
    y = p.y();

    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x > scene()->width()) x = scene()->width();
    if (y > scene()->height()) y = scene()->height();
    //x = event->pos().x();
    //y = event->pos().y();

    switch (_grabAction) {
    case 1:
        setLine(QLineF(x, y, line().x2(), line().y2()));
        break;
    case 2:
        setLine(QLineF(line().x1(), line().y1(), x, y));
        break;
    default:
        QPointF d = mapToItem(this, dynamic_cast<ReportScene*>(scene())->gridPoint(event->scenePos())) - mapToItem(this, dynamic_cast<ReportScene*>(scene())->gridPoint(event->lastScenePos()));

        if (((line().p1() + d).x() >= 0) &&
                ((line().p2() + d).x() >= 0) &&
                ((line().p1() + d).y() >= 0) &&
                ((line().p2() + d).y() >= 0)  &&
                ((line().p1() + d).x() <= scene()->width()) &&
                ((line().p2() + d).x() <= scene()->width()) &&
                ((line().p1() + d).y() <= scene()->height()) &&
                ((line().p2() + d).y() <= scene()->height()))
            setLine(QLineF(line().p1() + d, line().p2() + d));
        break;
    }
    //Update Properties
    _start.setScenePos(mapToScene(line().p1()), true);
    _end.setScenePos(mapToScene(line().p2()), true);

}

int ReportEntityLine::grabHandle(QPointF pos)
{
    QRectF r = boundingRect();

    QPointF center = r.center();

    if (QRectF(line().p1().x(), line().p1().y() - 2, 5, 5).contains(pos)) {
        // we are over point 1
        return 1;
    } else if (QRectF(line().p2().x() - 4, line().p2().y() - 2, 5, 5).contains(pos)) {
        // we are over point 2
        return 2;
    } else {
        return 0;
    }

}

void ReportEntityLine::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{
    _grabAction = 0;
    if (isSelected()) {
        _grabAction = grabHandle(event->pos());
        switch (_grabAction) {
        case 1: //Point 1
            setCursor(Qt::SizeAllCursor);
            break;
        case 2: //Point 2
            setCursor(Qt::SizeAllCursor);
            break;
        default:
            unsetCursor();
        }
    }
}


