/***************************************************************************
                          propertywidgets.cpp  -  description
                             -------------------
    begin                : 19.12.2002
    copyright            : (C) 2002 by Alexander Dymo
    email                : cloudtemple@mskat.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "propertywidgets.h"

QString PLineEdit::value() const
{
    return text();
}

void PLineEdit::setValue(const QString value)
{
    setText(value);
}


PComboBox::PComboBox ( std::map<QString, QString> *v_corresp, QWidget * parent, const char * name):
    QComboBox(parent, name), corresp(v_corresp)
{
    fillBox();
}

PComboBox::PComboBox ( std::map<QString, QString> *v_corresp,  bool rw, QWidget * parent, const char * name):
        QComboBox(rw, parent, name), corresp(v_corresp)
{
    fillBox();
}


void PComboBox::fillBox()
{
    for (std::map<QString, QString>::const_iterator it = corresp->begin(); it != corresp->end(); it++)
    {
        insertItem((*it).first);
        r_corresp[(*it).second] = (*it).first;
    }
}

QString PComboBox::value() const
{
    std::map<QString, QString>::const_iterator it = corresp->find(currentText());
    return (*it).second;
}

void PComboBox::setValue(const QString value)
{
    if (!value.isNull())
        setCurrentText(r_corresp[value]);    
}
