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
#include "reportentitybarcode.h"
#include "reportdesigner.h"

#include <qdom.h>
#include <qpainter.h>
#include <kdebug.h>
#include <klocalizedstring.h>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include "barcodepaint.h"

#include <koproperty/Property.h>
#include <koproperty/Set.h>
#include <koproperty/EditorView.h>
//
// class ReportEntityBarcode
//

void ReportEntityBarcode::init(QGraphicsScene * scene)
{
    setFlags(ItemIsSelectable | ItemIsMovable);

    if (scene)
        scene->addItem(this);

    connect(_set, SIGNAL(propertyChanged(KoProperty::Set &, KoProperty::Property &)), this, SLOT(propertyChanged(KoProperty::Set &, KoProperty::Property &)));

    setMaxLength(5);
    ReportRectEntity::init(&_pos, &_size, _set);
    setZValue(Z);
}
// methods (constructors)
ReportEntityBarcode::ReportEntityBarcode(ReportDesigner * rw, QGraphicsScene* scene)
        : ReportRectEntity(rw)
{
    init(scene);
    _size.setSceneSize(QSizeF(min_width_total*dpiX, min_height*dpiY));
    setSceneRect(_pos.toScene(), _size.toScene());

    _name->setValue(_rd->suggestEntityName("Barcode"));
}

ReportEntityBarcode::ReportEntityBarcode(QDomNode & element, ReportDesigner * rw, QGraphicsScene* scene) : ReportRectEntity(rw), KRBarcodeData(element)
{
    init(scene);
    setSceneRect(_pos.toScene(), _size.toScene());
}

ReportEntityBarcode* ReportEntityBarcode::clone()
{
    QDomDocument d;
    QDomElement e = d.createElement("clone");;
    QDomNode n;
    buildXML(d, e);
    n = e.firstChild();
    return new ReportEntityBarcode(n, designer(), 0);
}

// methods (deconstructor)
ReportEntityBarcode::~ReportEntityBarcode()
{}

QRect ReportEntityBarcode::getTextRect()
{
    QFont fnt = QFont();
    return QFontMetrics(fnt).boundingRect(int (x()), int (y()), 0, 0, 0, _controlSource->value().toString() + QObject::tr(":") + QObject::tr(" barcode"));
}

void ReportEntityBarcode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    // store any values we plan on changing so we can restore them
    QPen  p = painter->pen();

    painter->setBackground(Qt::white);


    //Draw a border so user knows the object edge
    painter->setPen(QPen(QColor(224, 224, 224)));
    painter->drawRect(rect());

    drawHandles(painter);

    if (_format->value().toString() == "3of9")
        render3of9(rect().toRect(), "3of9", alignment(), painter);
    else if (_format->value().toString() == "3of9+")
        renderExtended3of9(rect().toRect(), "3of9+", alignment(), painter);
    else if (_format->value().toString() == "128")
        renderCode128(rect().toRect(), "128", alignment(), painter);
    else if (_format->value().toString() == "upc-a")
        renderCodeUPCA(rect().toRect(), "123456789012", alignment(), painter);
    else if (_format->value().toString() == "upc-e")
        renderCodeUPCE(rect().toRect(), "12345678", alignment(), painter);
    else if (_format->value().toString() == "ean13")
        renderCodeEAN13(rect().toRect(), "123456789012", alignment(), painter);
    else if (_format->value().toString() == "ean8")
        renderCodeEAN8(rect().toRect(), "1234567", alignment(), painter);

    painter->setPen(Qt::black);
    painter->drawText(rect(), 0, _controlSource->value().toString() + QObject::tr(":") + QObject::tr(" barcode"));

    // restore an values before we started just in case
    painter->setPen(p);
}

void ReportEntityBarcode::buildXML(QDomDocument & doc, QDomElement & parent)
{
    //kdDebug() << "ReportEntityField::buildXML()");
    QDomElement entity = doc.createElement("barcode");

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

    // format
    QDomElement fmt = doc.createElement("format");
    fmt.appendChild(doc.createTextNode(_format->value().toString()));
    entity.appendChild(fmt);

    QDomElement maxl = doc.createElement("maxlength");
    maxl.appendChild(doc.createTextNode(QString::number(_maxLength->value().toInt())));
    entity.appendChild(maxl);

    // alignment
    entity.appendChild(doc.createElement(_hAlignment->value().toString()));

    // the field data
    QDomElement data = doc.createElement("data");
    QDomElement dcolumn = doc.createElement("controlsource");
    dcolumn.appendChild(doc.createTextNode(_controlSource->value().toString()));
    data.appendChild(dcolumn);
    entity.appendChild(data);

    parent.appendChild(entity);
}

void ReportEntityBarcode::propertyChanged(KoProperty::Set &s, KoProperty::Property &p)
{
    kDebug() << "ReportEntityBarcode::propertyChanged " << s.typeName() << ":" << p.name();

    //Handle Position
    if (p.name() == "Position") {
        _pos.setUnitPos(p.value().value<QPointF>());
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

void ReportEntityBarcode::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    _controlSource->setListData(_rd->fieldList(), _rd->fieldList());
    ReportRectEntity::mousePressEvent(event);
}
