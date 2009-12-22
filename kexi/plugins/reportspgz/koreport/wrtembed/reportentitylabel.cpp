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
#include "reportentitylabel.h"
#include "reportdesigner.h"

#include <qdom.h>
#include <qpainter.h>
#include <kdebug.h>
#include <klocalizedstring.h>
#include <koproperty/EditorView.h>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

//
// class ReportEntityLabel
//

void ReportEntityLabel::init(QGraphicsScene * scene)
{
    if (scene)
        scene->addItem(this);

    ReportRectEntity::init(&m_pos, &m_size, m_set);
    kDebug() << getTextRect();

    connect(properties(), SIGNAL(propertyChanged(KoProperty::Set &, KoProperty::Property &)),
            this, SLOT(slotPropertyChanged(KoProperty::Set &, KoProperty::Property &)));

    setZValue(Z);
}

// methods (constructors)
ReportEntityLabel::ReportEntityLabel(ReportDesigner* d, QGraphicsScene * scene)
        : ReportRectEntity(d)
{
    init(scene);
    setSceneRect(getTextRect());

    m_name->setValue(m_reportDesigner->suggestEntityName("label"));
}

ReportEntityLabel::ReportEntityLabel(QDomNode & element, ReportDesigner * d, QGraphicsScene * s)
        : ReportRectEntity(d), KRLabelData(element)
{
    init(s);
    setSceneRect(m_pos.toScene(), m_size.toScene());
}

ReportEntityLabel* ReportEntityLabel::clone()
{
    QDomDocument d;
    QDomElement e = d.createElement("clone");;
    QDomNode n;
    buildXML(d, e);
    n = e.firstChild();
    return new ReportEntityLabel(n, designer(), 0);
}

// methods (deconstructor)
ReportEntityLabel::~ReportEntityLabel()
{}

QRectF ReportEntityLabel::getTextRect()
{
    return QFontMetrics(font()).boundingRect(x(), y(), 0, 0, textFlags(), m_text->value().toString());
}

void ReportEntityLabel::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // store any values we plan on changing so we can restore them
    QFont f = painter->font();
    QPen  p = painter->pen();

    painter->setFont(font());

    QColor bg = m_backgroundColor->value().value<QColor>();
    bg.setAlpha((m_backgroundOpacity->value().toInt() / 100) * 255);

    painter->setBackground(bg);
    painter->setPen(m_foregroundColor->value().value<QColor>());

    painter->fillRect(QGraphicsRectItem::rect(), bg);
    painter->drawText(rect(), textFlags(), text());

    if ((Qt::PenStyle)m_lineStyle->value().toInt() == Qt::NoPen || m_lineWeight->value().toInt() <= 0) {
        painter->setPen(QPen(QColor(224, 224, 224)));
    } else {
        painter->setPen(QPen(m_lineColor->value().value<QColor>(), m_lineWeight->value().toInt(), (Qt::PenStyle)m_lineStyle->value().toInt()));
    }

    painter->drawRect(QGraphicsRectItem::rect());

    painter->setBackgroundMode(Qt::TransparentMode);
    painter->setPen(m_foregroundColor->value().value<QColor>());

    drawHandles(painter);

    // restore an values before we started just in case
    painter->setFont(f);
    painter->setPen(p);
}

void ReportEntityLabel::buildXML(QDomDocument & doc, QDomElement & parent)
{
    kDebug();

    QDomElement entity = doc.createElement("report:label");

    // properties
    addPropertyAsAttribute(&entity, m_name);
    addPropertyAsAttribute(&entity, m_text);
    addPropertyAsAttribute(&entity, m_verticalAlignment);
    addPropertyAsAttribute(&entity, m_horizontalAlignment);
    entity.setAttribute("report:z-index", zValue());

    // bounding rect
    buildXMLRect(doc, entity, pointRect());

    //text style info
    buildXMLTextStyle(doc, entity, textStyle());

    //Line Style
    buildXMLLineStyle(doc, entity, lineStyle());

    parent.appendChild(entity);
}

void ReportEntityLabel::slotPropertyChanged(KoProperty::Set &s, KoProperty::Property &p)
{
    Q_UNUSED(s);

    //TODO KoProperty needs QPointF and QSizeF and need to sync property with actual size/pos
    if (p.name() == "Position") {
        //_pos.setUnitPos(p.value().value<QPointF>(), false);
    } else if (p.name() == "Size") {
        //_size.setUnitSize(p.value().value<QSizeF>());
    } else if (p.name() == "Name") {
        //For some reason p.oldValue returns an empty string
        if (!m_reportDesigner->isEntityNameUnique(p.value().toString(), this)) {
            p.setValue(m_oldName);
        } else {
            m_oldName = p.value().toString();
        }
    }

    //setSceneRect(_pos.toScene(), _size.toScene());

    if (m_reportDesigner) m_reportDesigner->setModified(true);
    if (scene()) scene()->update();
}




