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

#include <koproperty/EditorView.h>
#include <KoGlobal.h>
#include "reportscene.h"

//
// class ReportEntityLine
//
void ReportEntityLine::init(QGraphicsScene* s, ReportDesigner *r)
{
    m_reportDesigner = r;
    setPos(0, 0);
    setUnit(r->pageUnit());

#if QT_VERSION >= 0x040600
    setFlags(ItemIsSelectable | ItemIsMovable | ItemSendsGeometryChanges);
#else
    setFlags(ItemIsSelectable | ItemIsMovable);
#endif

    setPen(QPen(Qt::black, 5));
    setAcceptsHoverEvents(true);

    if (s)
        s->addItem(this);

    connect(m_set, SIGNAL(propertyChanged(KoProperty::Set &, KoProperty::Property &)),
            this, SLOT(slotPropertyChanged(KoProperty::Set &, KoProperty::Property &)));

    setZValue(Z);
}

ReportEntityLine::ReportEntityLine(ReportDesigner * d, QGraphicsScene * scene)
        : ReportEntity(d)
{
    init(scene, d);

    m_name->setValue(m_reportDesigner->suggestEntityName("line"));
}

ReportEntityLine::ReportEntityLine(QDomNode & entity, ReportDesigner * d, QGraphicsScene * scene)
        : KRLineData(entity), ReportEntity(d)
{
    init(scene, d);
    setLine(m_start.toScene().x(), m_start.toScene().y(), m_end.toScene().x(), m_end.toScene().y());
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
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setRenderHint(QPainter::Antialiasing, true);
    QPen p = painter->pen();
    painter->setPen(QPen(m_lineColor->value().value<QColor>(), m_lineWeight->value().toInt(), (Qt::PenStyle)m_lineStyle->value().toInt()));
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
    QDomElement entity = doc.createElement("report:line");

    qreal sx, sy, ex, ey;

    sx = m_start.toPoint().x();
    sy = m_start.toPoint().y();
    ex = m_end.toPoint().x();
    ey = m_end.toPoint().y();

    // properties
    addPropertyAsAttribute(&entity, m_name);
    entity.setAttribute("report:z-index", zValue());
    entity.setAttribute("report:xstart", QString::number(sx));
    entity.setAttribute("report:ystart", QString::number(sy));
    entity.setAttribute("report:xend", QString::number(ex));
    entity.setAttribute("report:yend", QString::number(ey));

    buildXMLLineStyle(doc, entity, lineStyle());

    parent.appendChild(entity);
}

void ReportEntityLine::slotPropertyChanged(KoProperty::Set &s, KoProperty::Property &p)
{
    Q_UNUSED(s);

    kDebug() << p.name();
    if (p.name() == "Start" || p.name() == "End") {
        setLine ( m_start.toScene().x(), m_start.toScene().y(), m_end.toScene().x(), m_end.toScene().y() );
    } else if (p.name() == "Name") {
        //For some reason p.oldValue returns an empty string
        if (!m_reportDesigner->isEntityNameUnique(p.value().toString(), this)) {
            p.setValue(m_oldName);
        } else {
            m_oldName = p.value().toString();
        }
    }
    if (m_reportDesigner) m_reportDesigner->setModified(true);
    if (scene()) scene()->update();
}

void ReportEntityLine::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    m_reportDesigner->changeSet(m_set);
    setSelected(true);
    QGraphicsLineItem::mousePressEvent(event);
}

QVariant ReportEntityLine::itemChange(GraphicsItemChange change, const QVariant &value)
{
        //Update Properties
    m_start.setScenePos(mapToScene(line().p1()), true);
    m_end.setScenePos(mapToScene(line().p2()), true);
    return QGraphicsItem::itemChange(change, value);
}

void ReportEntityLine::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
    //Update Properties
    //m_start.setScenePos(mapToScene(line().p1()), true);
    //m_end.setScenePos(mapToScene(line().p2()), true);
    QGraphicsLineItem::mouseReleaseEvent(event);
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

    switch (m_grabAction) {
    case 1:
	m_start.setScenePos(QPointF(x,y));
        //setLine(QLineF(x, y, line().x2(), line().y2()));
        break;
    case 2:
	m_end.setScenePos(QPointF(x,y));
        //setLine(QLineF(line().x1(), line().y1(), x, y));
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
    //m_grabAction = 0;
    if (isSelected()) {
        m_grabAction = grabHandle(event->pos());
        switch (m_grabAction) {
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

void ReportEntityLine::setUnit(KoUnit u)
{
    m_start.setUnit(u);
    m_end.setUnit(u);
}
