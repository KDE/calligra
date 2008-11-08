/*
 * Kexi Report Plugin
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
#include "krshapedata.h"
#include <koproperty/Property.h>
#include <koproperty/Set.h>
#include <KoGlobal.h>
#include <kdebug.h>
#include <klocalizedstring.h>
#include <kglobalsettings.h>
#include <KoShapeRegistry.h>

KRShapeData::KRShapeData(QDomNode & element)
{
    createProperties();
    QDomNodeList nl = element.childNodes();
    QString n;
    QDomNode node;
    for (int i = 0; i < nl.count(); i++) {
        node = nl.item(i);
        n = node.nodeName();
        if (n == "name") {
            _name->setValue(node.firstChild().nodeValue());
        } else {
            kDebug() << "while parsing label element encountered unknow element: " << n;
        }
    }
}

void KRShapeData::createProperties()
{
    _set = new KoProperty::Set(0, "Shape");

    QStringList keys;

    keys << KoShapeRegistry::instance()->keys();
    _shapeType = new KoProperty::Property("ShapeType", keys, keys, "KoStarShape", "Shape Type");

    _set->addProperty(_name);
    _set->addProperty(_shapeType);
}

QRectF KRShapeData::_rect()
{
    QRectF r;
    r.setRect(_pos.toScene().x(), _pos.toScene().y(), _size.toScene().width(), _size.toScene().height());
    return r;
}

// RTTI
int KRShapeData::type() const
{
    return RTTI;
}
int KRShapeData::RTTI = KRObjectData::EntityShape;
KRShapeData * KRShapeData::toShape()
{
    return this;
}

