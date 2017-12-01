/* This file is part of the KDE project
 * Copyright (C) 2001-2007 by OpenMFG, LLC (info@openmfg.com)
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 * Copyright (C) 2016 by Dag Andersen <danders@get2net.dk>
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
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "PlanReportDesignerItemText.h"

#include "KReportDesignerItemBase.h"
#include "KReportDesigner.h"
#include "KReportLineStyle.h"

#include <KPropertySet>
#include <QDomDocument>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>


void PlanReportDesignerItemText::init(QGraphicsScene *scene, KReportDesigner *d)
{
    //setFlags(ItemIsSelectable | ItemIsMovable);
    if (scene)
        scene->addItem(this);

    connect(propertySet(), SIGNAL(propertyChanged(KPropertySet&,KProperty&)),
            this, SLOT(slotPropertyChanged(KPropertySet&,KProperty&)));

    KReportDesignerItemRectBase::init(&m_pos, &m_size, m_set, d);

    m_controlSource->setListData(m_reportDesigner->fieldKeys(), m_reportDesigner->fieldNames());
    setZValue(Z);

    updateRenderText(m_controlSource->value().toString(), m_itemValue->value().toString(),
                     QLatin1String("textarea"));
}

PlanReportDesignerItemText::PlanReportDesignerItemText(KReportDesigner * rw, QGraphicsScene * scene, const QPointF &pos)
        : KReportDesignerItemRectBase(rw)
{
    Q_UNUSED(pos);
    init(scene, rw);
    setSceneRect(properRect(*rw, getTextRect().width(), getTextRect().height()));
    m_name->setValue(m_reportDesigner->suggestEntityName(typeName()));
}

PlanReportDesignerItemText::PlanReportDesignerItemText(const QDomNode & element, KReportDesigner * d, QGraphicsScene * s)
        : PlanReportItemText(element), KReportDesignerItemRectBase(d)
{
    init(s, d);
    setSceneRect(m_pos.toScene(), m_size.toScene());
}

PlanReportDesignerItemText* PlanReportDesignerItemText::clone()
{
    QDomDocument d;
    QDomElement e = d.createElement(QLatin1String("clone"));
    QDomNode n;
    buildXML(&d, &e);
    n = e.firstChild();
    return new PlanReportDesignerItemText(n, designer(), 0);
}

PlanReportDesignerItemText::~PlanReportDesignerItemText()
{}

QRect PlanReportDesignerItemText::getTextRect() const
{
    return QFontMetrics(font()).boundingRect(int (x()), int (y()), 0, 0, textFlags(), m_renderText);
}

void PlanReportDesignerItemText::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget)

    // store any values we plan on changing so we can restore them
    QFont f = painter->font();
    QPen  p = painter->pen();

    painter->setFont(font());
    painter->setBackgroundMode(Qt::TransparentMode);

    QColor bg = m_backgroundColor->value().value<QColor>();
    bg.setAlphaF(m_backgroundOpacity->value().toReal()*0.01);

    painter->setPen(m_foregroundColor->value().value<QColor>());

    painter->fillRect(rect(),  bg);
    painter->drawText(rect(), textFlags(), m_renderText);

    if ((Qt::PenStyle)m_lineStyle->value().toInt() == Qt::NoPen || m_lineWeight->value().toInt() <= 0) {
        painter->setPen(QPen(Qt::lightGray));
    } else {
        painter->setPen(QPen(m_lineColor->value().value<QColor>(), m_lineWeight->value().toInt(), (Qt::PenStyle)m_lineStyle->value().toInt()));
    }
    painter->drawRect(rect());

    painter->setPen(m_foregroundColor->value().value<QColor>());

    drawHandles(painter);

    // restore an values before we started just in case
    painter->setFont(f);
    painter->setPen(p);
}

void PlanReportDesignerItemText::buildXML(QDomDocument *doc, QDomElement *parent)
{
    //kreportpluginDebug();
    QDomElement entity = doc->createElement(QLatin1String("report:") + typeName());

    // properties
    addPropertyAsAttribute(&entity, m_name);
    addPropertyAsAttribute(&entity, m_controlSource);
    addPropertyAsAttribute(&entity, m_verticalAlignment);
    addPropertyAsAttribute(&entity, m_horizontalAlignment);
    entity.setAttribute(QLatin1String("report:bottom-padding"), m_bottomPadding);
    entity.setAttribute(QLatin1String("report:z-index"), zValue());
    addPropertyAsAttribute(&entity, m_itemValue);

    // bounding rect
    buildXMLRect(doc, &entity, &m_pos, &m_size);

    //text style info
    buildXMLTextStyle(doc, &entity, textStyle());

    //Line Style
    buildXMLLineStyle(doc, &entity, lineStyle());

    parent->appendChild(entity);
}

void PlanReportDesignerItemText::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    m_controlSource->setListData(m_reportDesigner->fieldKeys(), m_reportDesigner->fieldNames());
    KReportDesignerItemRectBase::mousePressEvent(event);
}


void PlanReportDesignerItemText::slotPropertyChanged(KPropertySet &s, KProperty &p)
{
    Q_UNUSED(s);

    if (p.name() == "position") {
        m_pos.setUnitPos(p.value().toPointF(), KReportPosition::DontUpdateProperty);
    } else if (p.name() == "size") {
        m_size.setUnitSize(p.value().toSizeF(), KReportSize::DontUpdateProperty);
    } else if (p.name() == "name") {
        //For some reason p.oldValue returns an empty string
        if (!m_reportDesigner->isEntityNameUnique(p.value().toString(), this)) {
            p.setValue(m_oldName);
        } else {
            m_oldName = p.value().toString();
        }
    }

    setSceneRect(m_pos.toScene(), m_size.toScene(), DontUpdateProperty);
    if (m_reportDesigner)
        m_reportDesigner->setModified(true);
    if (scene())
        scene()->update();

    updateRenderText(m_controlSource->value().toString(), m_itemValue->value().toString(),
                     QLatin1String("textarea"));
}
