/* This file is part of the KDE project
   Copyright (C) 2002 Alexander Dymo <cloudtemple@mksat.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/
#ifndef PCOMBOBOX_H
#define PCOMBOBOX_H

#include <map>

#include <qcombobox.h>

#include "propertywidget.h"

class PropertyEditor;

class PComboBox: public QComboBox, PropertyWidget{
    Q_OBJECT
public:
    PComboBox ( const PropertyEditor *editor, const QString name, const QString value, std::map<QString, QString> *v_corresp, QWidget * parent = 0, const char * name = 0 );
    PComboBox ( const PropertyEditor *editor, const QString name, const QString value, std::map<QString, QString> *v_corresp,  bool rw, QWidget * parent = 0, const char * name = 0 );

    virtual QString value() const;
    virtual void setValue(const QString value, bool emitChange=true);

private:
    /** map<description, value>*/
    std::map<QString, QString> *corresp;
    /** map<value, description>*/
    std::map<QString, QString> r_corresp;

protected:
    virtual void fillBox();
signals:
    void propertyChanged(QString name, QString newValue);
private slots:
    void updateProperty(int val);
};

#endif
