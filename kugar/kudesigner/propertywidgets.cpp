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

#include "propertywidget.h"

QString PLineEdit::value() const
{
    return text();
}

void PLineEdit::setValue(QString value)
{
    setText(value);
}

QString PComboBox::value() const
{
    return corresp[currentText()];
}

virtual void PComboBox::setValue(QString value);
{
    setCurrentText();
}
