/***************************************************************************
                          property.cpp  -  description
                             -------------------
    begin                : 08.12.2002
    copyright            : (C) 2002 by Alexander Dymo
    email                : cloudtemple@mksat.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <qstring.h>
#include <qwidget.h>
#include <qlineedit.h>
#include "property.h"

Property::Property(int type, QString name, QString value):
    m_type(type), m_name(name), m_value(value)
{
}

Property::~Property()
{
}

bool Property::operator<(const Property &prop) const
{
    if (type() < prop.type())
        return true;
    else
        return false;
}

QString Property::name()
{
    return m_name;
}

void Property::setName(QString name)
{
    m_name = name;
}

int Property::type() const
{
    return m_type;
}

void Property::setType(int type)
{
    m_type = type;
}

QString Property::value()
{
    return m_value;
}

void Property::setValue(QString value)
{
    m_value = value;
}


QWidget *Property::editorOfType(int type, QWidget *parent)
{
    switch (type)
    {
        case IntegerValue:
            return new QLineEdit(parent);
            break;

        case CheckedValue:
        case LineStyle:
            return new QLineEdit(parent);
            break;

        case Color:
            return new QLineEdit(parent);
            break;

        case FontName:
            return new QLineEdit(parent);
            break;

        case StringValue:
        case Symbol:
        default:
            return new QLineEdit(parent);
    }
}
