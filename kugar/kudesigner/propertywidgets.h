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
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/
#ifndef PROPERTY_WIDGETS_H
#define PROPERTY_WIDGETS_H
 
#include <map>
 
#include <qlineedit.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qpushbutton.h>

#include <kfontcombo.h>
#include <kcolorcombo.h>

class PropertyWidget{
public:
    virtual QString value() const = 0;
    virtual void setValue(const QString value)=0;
};

class PLineEdit: public QLineEdit, PropertyWidget{
public:
    PLineEdit ( QWidget * parent, const char * name = 0 ):
        QLineEdit(parent, name) {};
    PLineEdit ( const QString & contents, QWidget * parent, const char * name = 0 ):
        QLineEdit(contents, parent, name) {};

    virtual QString value() const;
    virtual void setValue(const QString value);
};

class PSpinBox: public QSpinBox, PropertyWidget{
public:
    PSpinBox ( QWidget * parent = 0, const char * name = 0 ):
        QSpinBox(parent, name) {}
    PSpinBox ( int minValue, int maxValue, int step = 1, QWidget * parent = 0, const char * name = 0 ):
        QSpinBox(minValue, maxValue, step, parent, name) {}

    virtual QString value() const;
    virtual void setValue(const QString value);
};

class PComboBox: public QComboBox, PropertyWidget{
public:
    PComboBox ( std::map<QString, QString> *v_corresp, QWidget * parent = 0, const char * name = 0 );
    PComboBox ( std::map<QString, QString> *v_corresp,  bool rw, QWidget * parent = 0, const char * name = 0 );
 
    virtual QString value() const;
    virtual void setValue(const QString value);

private:
    /** map<description, value>*/
    std::map<QString, QString> *corresp;
    /** map<value, description>*/
    std::map<QString, QString> r_corresp;

protected:
    virtual void fillBox();
};

class PFontCombo: public KFontCombo, PropertyWidget{
public:
    PFontCombo (QWidget *parent, const char *name=0);
    PFontCombo (const QStringList &fonts, QWidget *parent, const char *name=0);

    virtual QString value() const;
    virtual void setValue(const QString value);
};

class PColorCombo: public KColorCombo, PropertyWidget{
public:
    PColorCombo(QWidget *parent, const char *name = 0);
    
    virtual QString value() const;
    virtual void setValue(const QString value);
};

class QHBoxLayout;

class PSymbolCombo: public QWidget, PropertyWidget{
    Q_OBJECT
public:
    PSymbolCombo(QWidget *parent = 0, const char *name = 0);

    virtual QString value() const;
    virtual void setValue(const QString value);

public slots:
    void selectChar();

private:
    QLineEdit *edit;
    QPushButton *pbSelect;
    QHBoxLayout *l;
};

#endif
