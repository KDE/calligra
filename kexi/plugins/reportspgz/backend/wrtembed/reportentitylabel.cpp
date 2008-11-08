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
    setFlags(ItemIsSelectable | ItemIsMovable);

    if (scene)
        scene->addItem(this);

    ReportRectEntity::init(&_pos, &_size, _set);
    kDebug() << getTextRect();


    connect(properties(), SIGNAL(propertyChanged(KoProperty::Set &, KoProperty::Property &)), this, SLOT(propertyChanged(KoProperty::Set &, KoProperty::Property &)));

    setZValue(Z);
}

// methods (constructors)
ReportEntityLabel::ReportEntityLabel(ReportDesigner* d, QGraphicsScene * scene)
        : ReportRectEntity(d)
{
    init(scene);
    setSceneRect(getTextRect());

    _name->setValue(_rd->suggestEntityName("Label"));

}

ReportEntityLabel::ReportEntityLabel(QDomNode & element, ReportDesigner * d, QGraphicsScene * s)
        : ReportRectEntity(d), KRLabelData(element)
{
    init(s);
    setSceneRect(_pos.toScene(), _size.toScene());
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
    return QFontMetrics(font()).boundingRect(x(), y(), 0, 0, textFlags(), _text->value().toString());
}


void ReportEntityLabel::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    // store any values we plan on changing so we can restore them
    QFont f = painter->font();
    QPen  p = painter->pen();

    painter->setFont(font());
    //painter->setBackgroundMode ( Qt::OpaqueMode );

    QColor bg = _bgColor->value().value<QColor>();
    bg.setAlpha(_bgOpacity->value().toInt());

    painter->setBackground(bg);
    painter->setPen(_fgColor->value().value<QColor>());

    painter->fillRect(QGraphicsRectItem::rect(), bg);
    painter->drawText(rect(), textFlags(), text());

    if ((Qt::PenStyle)_lnStyle->value().toInt() == Qt::NoPen || _lnWeight->value().toInt() <= 0) {
        painter->setPen(QPen(QColor(224, 224, 224)));
    } else {
        painter->setPen(QPen(_lnColor->value().value<QColor>(), _lnWeight->value().toInt(), (Qt::PenStyle)_lnStyle->value().toInt()));
    }

    painter->drawRect(QGraphicsRectItem::rect());

    painter->setBackgroundMode(Qt::TransparentMode);
    painter->setPen(_fgColor->value().value<QColor>());

    drawHandles(painter);

    // restore an values before we started just in case
    painter->setFont(f);
    painter->setPen(p);
}

void ReportEntityLabel::buildXML(QDomDocument & doc, QDomElement & parent)
{
    kDebug();
    //kdDebug() << "ReportEntityLabel::buildXML()");
    QDomElement entity = doc.createElement("label");

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

    // font info
    //buildXMLFont ( doc,entity,font() );

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

    // the text string
    QDomElement string = doc.createElement("string");
    string.appendChild(doc.createTextNode(text()));
    entity.appendChild(string);

    parent.appendChild(entity);
}

void ReportEntityLabel::propertyChanged(KoProperty::Set &s, KoProperty::Property &p)
{
    kDebug();
    //TODO KoProperty needs QPointF and QSizeF and need to sync property with actual size/pos
    if (p.name() == "Position") {
        //_pos.setUnitPos(p.value().value<QPointF>(), false);
    } else if (p.name() == "Size") {
        //_size.setUnitSize(p.value().value<QSizeF>());
    } else if (p.name() == "Name") {
        //For some reason p.oldValue returns an empty string
        if (!_rd->isEntityNameUnique(p.value().toString(), this)) {
            p.setValue(_oldName);
        } else {
            _oldName = p.value().toString();
        }
    }

    //setSceneRect(_pos.toScene(), _size.toScene());

    if (_rd) _rd->setModified(true);
    if (scene()) scene()->update();
}




