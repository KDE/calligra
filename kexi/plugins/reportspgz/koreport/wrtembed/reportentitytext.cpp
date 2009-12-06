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
#include "reportentitytext.h"
#include "reportdesigner.h"

#include <qdom.h>
#include <qpainter.h>
#include <kdebug.h>
#include <klocalizedstring.h>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

#include <koproperty/Property.h>
#include <koproperty/Set.h>
#include <koproperty/EditorView.h>
//
// class ReportEntityText
//
// methods (constructors)

void ReportEntityText::init(QGraphicsScene * scene)
{
    //setFlags(ItemIsSelectable | ItemIsMovable);
    if (scene)
        scene->addItem(this);

    connect(properties(), SIGNAL(propertyChanged(KoProperty::Set &, KoProperty::Property &)),
        this, SLOT(slotPropertyChanged(KoProperty::Set &, KoProperty::Property &)));

    ReportRectEntity::init(&m_pos, &m_size, m_set);

    m_controlSource->setListData(m_reportDesigner->fieldList(), m_reportDesigner->fieldList());
    setZValue(Z);
}

ReportEntityText::ReportEntityText(ReportDesigner * rw, QGraphicsScene * scene)
        : ReportRectEntity(rw)
{
    init(scene);
    setSceneRect(getTextRect());

    m_name->setValue(m_reportDesigner->suggestEntityName("Text"));
}

ReportEntityText::ReportEntityText(QDomNode & element, ReportDesigner * d, QGraphicsScene * s)
        : KRTextData(element), ReportRectEntity(d)
{
    init(s);
    setSceneRect(m_pos.toScene(), m_size.toScene());
}

ReportEntityText* ReportEntityText::clone()
{
    QDomDocument d;
    QDomElement e = d.createElement("clone");;
    QDomNode n;
    buildXML(d, e);
    n = e.firstChild();
    return new ReportEntityText(n, designer(), 0);
}

ReportEntityText::~ReportEntityText()
{}

QRect ReportEntityText::getTextRect()
{
    return QFontMetrics(font()).boundingRect(int (x()), int (y()), 0, 0, textFlags(), column() + QObject::tr(":") + QObject::tr(" textarea"));
}

void ReportEntityText::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget)
    
    // store any values we plan on changing so we can restore them
    QFont f = painter->font();
    QPen  p = painter->pen();

    painter->setFont(font());
    painter->setBackgroundMode(Qt::OpaqueMode);
    painter->setBackground(m_backgroundColor->value().value<QColor>());
    painter->setPen(m_foregroundColor->value().value<QColor>());
    painter->fillRect(rect(),  m_backgroundColor->value().value<QColor>());
    painter->drawText(rect(), textFlags(), column() + QObject::tr(":") + QObject::tr(" textarea"));

    if ((Qt::PenStyle)m_lineStyle->value().toInt() == Qt::NoPen || m_lineWeight->value().toInt() <= 0) {
        painter->setPen(QPen(QColor(224, 224, 224)));
    } else {
        painter->setPen(QPen(m_lineColor->value().value<QColor>(), m_lineWeight->value().toInt(), (Qt::PenStyle)m_lineStyle->value().toInt()));
    }
    painter->drawRect(rect());

    painter->setBackgroundMode(Qt::TransparentMode);
    painter->setPen(m_foregroundColor->value().value<QColor>());

    drawHandles(painter);

    // restore an values before we started just in case
    painter->setFont(f);
    painter->setPen(p);
}

void ReportEntityText::buildXML(QDomDocument & doc, QDomElement & parent)
{
    //kdDebug() << "ReportEntityText::buildXML()");
    QDomElement entity = doc.createElement("text");

    // bounding rect
    buildXMLRect(doc, entity, pointRect());

    // name
    QDomElement n = doc.createElement("name");
    n.appendChild(doc.createTextNode(entityName()));
    entity.appendChild(n);

    // z
    QDomElement z = doc.createElement("zvalue");
    z.appendChild(doc.createTextNode(QString::number(zValue())));
    entity.appendChild(z);

    // bottompad
    QDomElement bottompad = doc.createElement("bottompad");
    qreal h = m_bottomPadding * 100.0;
    bottompad.appendChild(doc.createTextNode(QString::number((int) h)));
    entity.appendChild(bottompad);

    //text style info
    buildXMLTextStyle(doc, entity, textStyle());

    //Line Style
    buildXMLLineStyle(doc, entity, lineStyle());

    // text alignment
    int align = textFlags();
    // horizontal
    if ((align & Qt::AlignRight) == Qt::AlignRight)
        entity.appendChild(doc.createElement("right"));
    else if ((align & Qt::AlignHCenter) == Qt::AlignHCenter)
        entity.appendChild(doc.createElement("hcenter"));
    else // Qt::AlignLeft
        entity.appendChild(doc.createElement("left"));
    // vertical
    if ((align & Qt::AlignBottom) == Qt::AlignBottom)
        entity.appendChild(doc.createElement("bottom"));
    else if ((align & Qt::AlignVCenter) == Qt::AlignVCenter)
        entity.appendChild(doc.createElement("vcenter"));
    else // Qt::AlignTop
        entity.appendChild(doc.createElement("top"));

    // the field data
    QDomElement data = doc.createElement("data");

    QDomElement dcolumn = doc.createElement("controlsource");
    dcolumn.appendChild(doc.createTextNode(column()));
    data.appendChild(dcolumn);
    entity.appendChild(data);

    parent.appendChild(entity);
}

void ReportEntityText::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    m_controlSource->setListData(m_reportDesigner->fieldList(), m_reportDesigner->fieldList());
    ReportRectEntity::mousePressEvent(event);
}


void ReportEntityText::slotPropertyChanged(KoProperty::Set &s, KoProperty::Property &p)
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

    if (m_reportDesigner)
        m_reportDesigner->setModified(true);
    if (scene())
        scene()->update();
}
