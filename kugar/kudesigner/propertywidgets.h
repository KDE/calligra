/***************************************************************************
                          propertywidgets.h  -  description
                             -------------------
    begin                : 19.12.2002
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

#include <map>
 
#include <qlineedit.h>
#include <qcombobox.h>

class PropertyWidget{
public:
    virtual QString value() const = 0;
    virtual void setValue(QString value)=0;
};

class PLineEdit: public QLineEdit, PropertyWidget{
public:
    PLineEdit ( QWidget * parent, const char * name = 0 ):
        QLineEdit(parent, name) {};
    PLineEdit ( const QString & contents, QWidget * parent, const char * name = 0 ):
        QLineEdit(contents, parent, name) {};

    virtual QString value() const;
    virtual void setValue(QString value);
};

class PComboBox: public QComboBox, PropertyWidget{
public:
    PComboBox ( std::map<QString, QString> v_corresp, QWidget * parent = 0, const char * name = 0 ):
        QComboBox(parent, name), corresp(v_corresp) {}
    PComboBox ( std::map<QString, QString> v_corresp,  bool rw, QWidget * parent = 0, const char * name = 0 ):
        QComboBox(rw, parent, name), corresp(v_corresp) {}

    virtual QString value() const;
    virtual void setValue(QString value);

private:
    /** map<description, value>*/
    std::map<QString, QString> corresp;
};

