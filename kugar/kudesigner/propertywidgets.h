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

class PropertyEditor;

class PropertyWidget{
public:
    virtual ~PropertyWidget() {}
    
    virtual QString value() const = 0;
    virtual void setValue(const QString value, bool emitChange=true) = 0;

    virtual QString pname() const { return m_name; }
    virtual void setPName(const QString pname) { m_name = pname; }
private:
    QString m_name;
};

class PLineEdit: public QLineEdit, public PropertyWidget{
    Q_OBJECT
public:
    PLineEdit ( const PropertyEditor *editor, QString name, QString value, QWidget * parent, const char * name = 0 );

    virtual QString value() const;
    virtual void setValue(const QString value, bool emitChange=true);
signals:
    void propertyChanged(QString name, QString newValue);
private slots:
    void updateProperty(const QString &val);
};

class PSpinBox: public QSpinBox, PropertyWidget{
    Q_OBJECT
public:
    PSpinBox ( const PropertyEditor *editor, const QString name, const QString value, QWidget * parent = 0, const char * name = 0 );
    PSpinBox ( const PropertyEditor *editor, const QString name, const QString value, int minValue, int maxValue, int step = 1, QWidget * parent = 0, const char * name = 0 );

    virtual QString value() const;
    virtual void setValue(const QString value, bool emitChange=true);
signals:
    void propertyChanged(QString name, QString newValue);
private slots:
    void updateProperty(int val);
};

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

class PFontCombo: public KFontCombo, PropertyWidget{
    Q_OBJECT
public:
    PFontCombo (const PropertyEditor *editor, const QString name, const QString value, QWidget *parent, const char *name=0);
    PFontCombo (const PropertyEditor *editor, const QString name, const QString value, const QStringList &fonts, QWidget *parent, const char *name=0);

    virtual QString value() const;
    virtual void setValue(const QString value, bool emitChange=true);
signals:
    void propertyChanged(QString name, QString newValue);
private slots:
    void updateProperty(int val);
};

class PColorCombo: public KColorCombo, PropertyWidget{
    Q_OBJECT
public:
    PColorCombo(const PropertyEditor *editor, const QString name, const QString value, QWidget *parent, const char *name = 0);

    virtual QString value() const;
    virtual void setValue(const QString value, bool emitChange=true);
signals:
    void propertyChanged(QString name, QString newValue);
private slots:
    void updateProperty(int val);
};

class QHBoxLayout;

class PSymbolCombo: public QWidget, PropertyWidget{
    Q_OBJECT
public:
    PSymbolCombo(const PropertyEditor *editor, const QString name, const QString value, QWidget *parent = 0, const char *name = 0);

    virtual QString value() const;
    virtual void setValue(const QString value, bool emitChange=true);

public slots:
    void selectChar();

private:
    QLineEdit *edit;
    QPushButton *pbSelect;
    QHBoxLayout *l;
signals:
    void propertyChanged(QString name, QString newValue);
private slots:
    void updateProperty(const QString &val);
};


class PLineStyle: public QComboBox, PropertyWidget{
    Q_OBJECT
public:
    PLineStyle(const PropertyEditor *editor, const QString name, const QString value, QWidget *parent = 0, const char *name = 0);

    virtual QString value() const;
    virtual void setValue(const QString value, bool emitChange=true);
signals:
    void propertyChanged(QString name, QString newValue);
private slots:
    void updateProperty(int val);
};

#endif
