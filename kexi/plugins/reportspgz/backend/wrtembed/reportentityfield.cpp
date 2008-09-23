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
#include "reportentityfield.h"
#include "reportdesigner.h"

#include <qdom.h>
#include <qpainter.h>
#include <kdebug.h>
#include <klocalizedstring.h>
#include <QGraphicsScene>
#include <koproperty/editor.h>
#include <QGraphicsSceneMouseEvent>

//
// class ReportEntityField
//

void ReportEntityField::init(QGraphicsScene * scene)
{
    setFlags(ItemIsSelectable | ItemIsMovable);
    if (scene)
        scene->addItem(this);

    connect(_set, SIGNAL(propertyChanged(KoProperty::Set &, KoProperty::Property &)), this, SLOT(propertyChanged(KoProperty::Set &, KoProperty::Property &)));

    ReportRectEntity::init(&_pos, &_size, _set);
    setZValue(Z);
}

// methods (constructors)
ReportEntityField::ReportEntityField(ReportDesigner * rw, QGraphicsScene * scene)
        : ReportRectEntity(rw)
{
    init(scene);
    setSceneRect(getTextRect());

    _name->setValue(_rd->suggestEntityName("Field"));
}

ReportEntityField::ReportEntityField(QDomNode & element, ReportDesigner * d, QGraphicsScene * s)
        : KRFieldData(element), ReportRectEntity(d)
{
    init(s);
    setSceneRect(_pos.toScene(), _size.toScene());
}

ReportEntityField* ReportEntityField::clone()
{
    QDomDocument d;
    QDomElement e = d.createElement("clone");;
    QDomNode n;
    buildXML(d, e);
    n = e.firstChild();
    return new ReportEntityField(n, designer(), 0);
}

// methods (deconstructor)
ReportEntityField::~ReportEntityField()
{}

QRect ReportEntityField::getTextRect()
{
    return QFontMetrics(font()).boundingRect(int (x()), int (y()), 0, 0, textFlags(), _controlSource->value().toString() + QObject::tr(":") + QObject::tr((_trackTotal->value().toBool() ? " field total" : " field")));
}



void ReportEntityField::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget *widget)
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
    painter->drawText(rect(), textFlags(), _controlSource->value().toString() + QObject::tr(":") + QObject::tr((_trackTotal->value().toBool() ? " field total" : " field")));


    if ((Qt::PenStyle)_lnStyle->value().toInt() == Qt::NoPen || _lnWeight->value().toInt() <= 0) {
        painter->setPen(QPen(QColor(224, 224, 224)));
    } else {
        painter->setPen(QPen(_lnColor->value().value<QColor>(), _lnWeight->value().toInt(), (Qt::PenStyle)_lnStyle->value().toInt()));
    }

    painter->drawRect(rect());

    painter->setBackgroundMode(Qt::TransparentMode);

    drawHandles(painter);

    // restore an values before we started just in case
    painter->setFont(f);
    painter->setPen(p);
}

void ReportEntityField::buildXML(QDomDocument & doc, QDomElement & parent)
{
    //kdDebug() << "ReportEntityField::buildXML()");
    QDomElement entity = doc.createElement("field");

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

    // the field data
    QDomElement data = doc.createElement("data");
// QDomElement dquery = doc.createElement ( "query" );
// dquery.appendChild ( doc.createTextNode ( query() ) );
// data.appendChild ( dquery );
    QDomElement dcolumn = doc.createElement("controlsource");
    dcolumn.appendChild(doc.createTextNode(controlSource()));
    data.appendChild(dcolumn);
    entity.appendChild(data);

    if (_trackTotal) {
        QDomElement tracktotal = doc.createElement("tracktotal");
        if (_trackBuiltinFormat)
            tracktotal.setAttribute("builtin", "true");
        if (_useSubTotal)
            tracktotal.setAttribute("subtotal", "true");
        tracktotal.appendChild(doc.createTextNode(_trackTotalFormat->value().toString()));
        entity.appendChild(tracktotal);
    }

    parent.appendChild(entity);
}

void ReportEntityField::propertyChanged(KoProperty::Set &s, KoProperty::Property &p)
{
    kDebug() << "ReportEntityField::propertyChanged " << s.typeName() << ":" << p.name() << ":" << p.value();

    //Handle Position
    if (p.name() == "Position") {
        //TODO _pos.setUnitRect(p.value().value<QRect>() );
    }

    if (p.name() == "Name") {
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

void ReportEntityField::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    _controlSource->setListData(_rd->fieldList(), _rd->fieldList());
    ReportRectEntity::mousePressEvent(event);
}
