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
#include "krscriptfield.h"

namespace Scripting
{
Field::Field(KRFieldData *f)
{
    _field = f;
}


Field::~Field()
{
}

QString Field::source()
{
    return _field->controlSource();
}

void Field::setSource(const QString& s)
{
    _field->setColumn(s);
}

int Field::horizontalAlignment()
{
    QString a = _field->_hAlignment->value().toString();

    if (a.toLower() == "left") {
        return -1;
    } else if (a.toLower() == "center") {
        return 0;
    } else if (a.toLower() == "right") {
        return 1;
    }
    return -1;
}
void Field::setHorizonalAlignment(int a)
{
    switch (a) {
    case -1:
        _field->_hAlignment->setValue("Left");
        break;
    case 0:
        _field->_hAlignment->setValue("Center");
        break;
    case 1:
        _field->_hAlignment->setValue("Right");
        break;
    default:
        _field->_hAlignment->setValue("Left");
        break;
    }
}

int Field::verticalAlignment()
{
    QString a = _field->_hAlignment->value().toString();

    if (a.toLower() == "top") {
        return -1;
    } else if (a.toLower() == "middle") {
        return 0;
    } else if (a.toLower() == "bottom") {
        return 1;
    }
    return -1;
}
void Field::setVerticalAlignment(int a)
{
    switch (a) {
    case -1:
        _field->_hAlignment->setValue("Top");
        break;
    case 0:
        _field->_hAlignment->setValue("Middle");
        break;
    case 1:
        _field->_hAlignment->setValue("Bottom");
        break;
    default:
        _field->_hAlignment->setValue("Top");
        break;
    }
}

QColor Field::backgroundColor()
{
    return _field->_bgColor->value().value<QColor>();
}
void Field::setBackgroundColor(const QColor& c)
{
    _field->_bgColor->setValue(c);
}

QColor Field::foregroundColor()
{
    return _field->_fgColor->value().value<QColor>();
}
void Field::setForegroundColor(const QColor& c)
{
    _field->_fgColor->setValue(c);
}

int Field::backgroundOpacity()
{
    return _field->_bgOpacity->value().toInt();
}
void Field::setBackgroundOpacity(int o)
{
    _field->_bgOpacity->setValue(o);
}

QColor Field::lineColor()
{
    return _field->_lnColor->value().value<QColor>();
}
void Field::setLineColor(const QColor& c)
{
    _field->_lnColor->setValue(c);
}

int Field::lineWeight()
{
    return _field->_lnWeight->value().toInt();
}
void Field::setLineWeight(int w)
{
    _field->_lnWeight->setValue(w);
}

int Field::lineStyle()
{
    return _field->_lnStyle->value().toInt();
}
void Field::setLineStyle(int s)
{
    if (s < 0 || s > 5) {
        s = 1;
    }
    _field->_lnStyle->setValue(s);
}

QPointF Field::position()
{
    return _field->_pos.toPoint();
}
void Field::setPosition(const QPointF &p)
{
    _field->_pos.setPointPos(p);
}

QSizeF Field::size()
{
    return _field->_size.toPoint();
}
void Field::setSize(const QSizeF &s)
{
    _field->_size.setPointSize(s);
}
}
