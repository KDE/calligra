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
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/
#include <qstring.h>
#include <qwidget.h>

#include "propertywidgets.h"
#include "propertyeditor.h"
#include "property.h"

Property::Property(int type, QString name, QString description, QString value):
    m_type(type), m_name(name), m_description(description), m_value(value)
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

QString Property::description() const
{
    return m_description;
}

void Property::setDescription(QString description)
{
    m_description = description;
}

QWidget *Property::editorOfType(const PropertyEditor *editor)
{
    PSpinBox *s;
    PLineEdit *l;
    PFontCombo *f;
    PColorCombo *c;
    PSymbolCombo *y;
    PLineStyle *i;
    
    switch (type())
    {
        case IntegerValue:
            s = new PSpinBox(editor, name(), value(), 0, 10000, 1, 0);
            return s;

        case Color:
            c = new PColorCombo(editor, name(), value(), 0);
            return c;

        case FontName:
            f = new PFontCombo(editor, name(), value(), 0);
            return f;

        case Symbol:
            y = new PSymbolCombo(editor, name(), value(), 0);
            return y;

        case LineStyle:
            i = new PLineStyle(editor, name(), value(), 0);
            return i;

        case ValueFromList:
        case StringValue:
        default:
            l = new PLineEdit((PropertyEditor *)editor, name(), value(), (QWidget*)0);
            return l;
    }
    return 0;
}

DescriptionProperty::DescriptionProperty(QString name, std::map<QString, QString> v_correspList,
    QString description, QString value):
    Property(ValueFromList, name, description, value), correspList(v_correspList)
{
    
}


void DescriptionProperty::setCorrespList(std::map<QString, QString> list)
{
    correspList = list;
}

QWidget *DescriptionProperty::editorOfType(const PropertyEditor *editor)
{
    switch (type())
    {
        case ValueFromList:
            PComboBox *b = new PComboBox(editor, name(), value(), &correspList, false, 0, 0);
            return b;
    }
    return Property::editorOfType(editor);
}
