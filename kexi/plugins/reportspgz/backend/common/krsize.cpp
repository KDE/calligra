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
#include "krsize.h"

#include "krsize.h"
#include <kdebug.h>

KRSize::KRSize(const KoUnit& unit)
{
    _unit = unit;
    //TODO When KoProperty can handle QSize
    _property = new KoProperty::Property("Size", toScene().toSize(), "Size", "Size");
}


KRSize::~KRSize()
{
}

void KRSize::setSceneSize(const QSizeF& s)
{
    qreal w, h;

    w = INCH_TO_POINT(s.width() / KoGlobal::dpiX());
    h = INCH_TO_POINT(s.height() / KoGlobal::dpiY());
    _pointSize.setWidth(w);
    _pointSize.setHeight(h);
    _property->setValue(toUnit().toSize());
}

void KRSize::setUnitSize(const QSizeF& s)
{
    qreal w, h;
    w = _unit.fromUserValue(s.width());
    h = _unit.fromUserValue(s.height());
    _pointSize.setWidth(w);
    _pointSize.setHeight(h);
    _property->setValue(toUnit().toSize());
}

void KRSize::setPointSize(const QSizeF& s)
{
    _pointSize.setWidth(s.width());
    _pointSize.setHeight(s.height());
    _property->setValue(toUnit().toSize());

}

void KRSize::setUnit(KoUnit u)
{
    _unit = u;
    _property->setValue(toUnit().toSize());
}

QSizeF KRSize::toPoint()
{
    return _pointSize;
}

QSizeF KRSize::toScene()
{
    qreal w, h;
    w = POINT_TO_INCH(_pointSize.width()) * KoGlobal::dpiX();
    h = POINT_TO_INCH(_pointSize.height()) * KoGlobal::dpiY();
    return QSizeF(w, h);
}

QSizeF KRSize::toUnit()
{
    qreal w, h;
    w = _unit.toUserValue(_pointSize.width());
    h = _unit.toUserValue(_pointSize.height());

    return QSizeF(w, h);
}
