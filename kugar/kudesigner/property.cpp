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

#include "propertywidgets.h"
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
    if ((type() < prop.type()) && (name() < prop.name()))
        return true;
    else
        return false;
}

QString Property::name() const
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

QString Property::value() const
{
    return m_value;
}

void Property::setValue(QString value)
    {
    m_value = value;
}


QWidget *Property::editorOfType()
{
    switch (type())
    {
        case IntegerValue:
            return new PLineEdit(0);

        case LineStyle:
            return new PLineEdit(0);

        case Color:
            return new PLineEdit(0);

        case FontName:
            return new PLineEdit(0);

        case ValueFromList:
        case StringValue:
        case Symbol:
        default:
            PLineEdit *l = new PLineEdit(0);
            l->setValue(value());
            return l;
    }
    return 0;
}

DescriptionProperty::DescriptionProperty(QString name, QString value, std::map<QString, QString> v_correspList):
    Property(ValueFromList, name, value), correspList(v_correspList)
{
    
}


void DescriptionProperty::setCorrespList(std::map<QString, QString> list)
{
    correspList = list;
}

QWidget *DescriptionProperty::editorOfType()
{
    switch (type())
    {
        case ValueFromList:
            PComboBox *b = new PComboBox(&correspList, false, 0, 0);
            b->setValue(value());
            return b;
    }
    return Property::editorOfType();;
}
