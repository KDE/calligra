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
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include <klocale.h>
#include <kcharselect.h>
 
#include <qlayout.h>
#include <qdialog.h>

#include "propertyeditor.h" 
#include "propertywidgets.h"

//class PLineEdit

PLineEdit::PLineEdit ( const PropertyEditor *editor, const QString pname, const QString value, QWidget * parent, const char * name ):
    QLineEdit(parent, name)
{
    setValue(value, false);
    setPName(pname);
    connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(updateProperty(const QString&)));
    connect(this, SIGNAL(propertyChanged(QString, QString)), editor, SLOT(emitPropertyChange(QString, QString)));
}

QString PLineEdit::value() const
{
    return text();
}

void PLineEdit::setValue(const QString value, bool emitChange)
{
    setText(value);
    if (emitChange)
        emit propertyChanged(pname(), value);
}

void PLineEdit::updateProperty(const QString& val)
{
    emit propertyChanged(pname(), val);
}

//class PSpinBox

PSpinBox::PSpinBox ( const PropertyEditor *editor, const QString pname, const QString value, QWidget * parent, const char * name ):
    QSpinBox(parent, name)
{
    setValue(value, false);
    setPName(pname);
    connect(this, SIGNAL(valueChanged(int)), this, SLOT(updateProperty(int)));
    connect(this, SIGNAL(propertyChanged(QString, QString)), editor, SLOT(emitPropertyChange(QString, QString)));
}

PSpinBox::PSpinBox ( const PropertyEditor *editor, const QString pname, const QString value, int minValue, int maxValue, int step, QWidget * parent, const char * name ):
    QSpinBox(minValue, maxValue, step, parent, name)
{
    setPName(pname);
    setValue(value, false);
    connect(this, SIGNAL(valueChanged(int)), this, SLOT(updateProperty(int)));
    connect(this, SIGNAL(propertyChanged(QString, QString)), editor, SLOT(emitPropertyChange(QString, QString)));
}

QString PSpinBox::value() const
{
    return cleanText();
}

void PSpinBox::setValue(const QString value, bool emitChange)
{
    QSpinBox::setValue(value.toInt());
    if (emitChange)
        emit propertyChanged(pname(), value);
}

void PSpinBox::updateProperty(int val)
{
    emit propertyChanged(pname(), QString("%1").arg(val));
}

//class PComboBox

PComboBox::PComboBox ( const PropertyEditor *editor, const QString pname, const QString value, std::map<QString, QString> *v_corresp, QWidget * parent, const char * name):
    QComboBox(parent, name), corresp(v_corresp)
{
    fillBox();
    setValue(value, false);
    setPName(pname);
    connect(this, SIGNAL(activated(int)), this, SLOT(updateProperty(int)));
    connect(this, SIGNAL(propertyChanged(QString, QString)), editor, SLOT(emitPropertyChange(QString, QString)));
}

PComboBox::PComboBox ( const PropertyEditor *editor, const QString pname, const QString value, std::map<QString, QString> *v_corresp,  bool rw, QWidget * parent, const char * name):
        QComboBox(rw, parent, name), corresp(v_corresp)
{
    fillBox();
    setValue(value, false);
    setPName(pname);
    connect(this, SIGNAL(activated(int)), this, SLOT(updateProperty(int)));
    connect(this, SIGNAL(propertyChanged(QString, QString)), editor, SLOT(emitPropertyChange(QString, QString)));
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

void PComboBox::setValue(const QString value, bool emitChange)
{
    if (!value.isNull())
    {
        setCurrentText(r_corresp[value]);
        if (emitChange)
            emit propertyChanged(pname(), value);
    }
}

void PComboBox::updateProperty(int val)
{
    emit propertyChanged(pname(), value());
}


//class PFontCombo

PFontCombo::PFontCombo (const PropertyEditor *editor, const QString pname, const QString value, QWidget *parent, const char *name):
    KFontCombo(parent, name)
{
    //AD: KFontCombo seems to have a bug: when it is not editable, the signals
    //activated(int) and textChanged(const QString &) are not emitted
//    setEditable(false);
    setPName(pname);
    setValue(value, false);
    connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(updateProperty(const QString&)));
    connect(this, SIGNAL(propertyChanged(QString, QString)), editor, SLOT(emitPropertyChange(QString, QString)));
}

PFontCombo::PFontCombo (const PropertyEditor *editor, const QString pname, const QString value, const QStringList &fonts, QWidget *parent, const char *name):
    KFontCombo(fonts, parent, name)
{
    //AD: KFontCombo seems to have a bug: when it is not editable, the signals
    //activated(int) and textChanged(const QString &) are not emitted
//    setEditable(false);
    setValue(value, false);
    setPName(pname);
    connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(updateProperty(const QString&)));
    connect(this, SIGNAL(propertyChanged(QString, QString)), editor, SLOT(emitPropertyChange(QString, QString)));
}

QString PFontCombo::value() const
{
    return currentFont();
}

void PFontCombo::setValue(const QString value, bool emitChange)
{
    setCurrentFont(value);
    if (emitChange)
        emit propertyChanged(pname(), value);
}

void PFontCombo::updateProperty(const QString &val)
{
    emit propertyChanged(pname(), val);
}


//class PColorCombo

PColorCombo::PColorCombo(const PropertyEditor *editor, const QString pname, const QString value, QWidget *parent, const char *name):
    KColorCombo(parent, name)
{
    setValue(value);
    setPName(pname);
    connect(this, SIGNAL(activated(int)), this, SLOT(updateProperty(int)));
    connect(this, SIGNAL(propertyChanged(QString, QString)), editor, SLOT(emitPropertyChange(QString, QString)));
}

QString PColorCombo::value() const
{
    return QString("%1,%2,%3").arg(color().red()).arg(color().green()).arg(color().blue());
}

void PColorCombo::setValue(const QString value, bool emitChange)
{
    setColor(QColor(value.section(',', 0, 0).toInt(),
        value.section(',', 1, 1).toInt(),
        value.section(',', 2, 2).toInt()));
    if (emitChange)
        emit propertyChanged(pname(), value);
}

void PColorCombo::updateProperty(int val)
{
    emit propertyChanged(pname(), value());
}


//class PSymbolCombo

PSymbolCombo::PSymbolCombo(const PropertyEditor *editor, const QString pname, const QString value, QWidget *parent, const char *name):
    QWidget(parent, name)
{
    l = new QHBoxLayout(this);

    edit = new QLineEdit(this);
    edit->setMaxLength(1);
    l->addWidget(edit);
    pbSelect = new QPushButton("...", this);
    l->addWidget(pbSelect);

    connect(pbSelect, SIGNAL(clicked()), this, SLOT(selectChar()));
    setValue(value);
    setPName(pname);
    connect(edit, SIGNAL(textChanged(const QString&)), this, SLOT(updateProperty(const QString&)));
    connect(this, SIGNAL(propertyChanged(QString, QString)), editor, SLOT(emitPropertyChange(QString, QString)));
}

QString PSymbolCombo::value() const
{
    if (!(edit->text().isNull()))
        return QString("%1").arg(edit->text().at(0).unicode());
    else
        return "";
}

void PSymbolCombo::setValue(const QString value, bool emitChange)
{
    if (!(value.isNull()))
    {
        edit->setText(QChar(value.toInt()));
        if (emitChange)
            emit propertyChanged(pname(), value);
    }
}

void PSymbolCombo::selectChar()
{
    QDialog* dia = new QDialog(this, "select_dialog", true);
    QVBoxLayout *dv = new QVBoxLayout(dia, 2);

    KCharSelect *select = new KCharSelect(dia, "select_char");
    dv->addWidget(select);

    QHBoxLayout *dh = new QHBoxLayout(dv, 6);
    QPushButton *pbOk = new QPushButton(i18n("Ok"), dia);
    QPushButton *pbCancel = new QPushButton(i18n("Cancel"), dia);
    QSpacerItem *si = new QSpacerItem(30, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(pbOk, SIGNAL(clicked()), dia, SLOT(accept()));
    connect(pbCancel, SIGNAL(clicked()), dia, SLOT(reject()));

    dh->addItem(si);
    dh->addWidget(pbOk);
    dh->addWidget(pbCancel);

    if (!(edit->text().isNull()))
        select->setChar(edit->text().at(0));

    if (dia->exec() == QDialog::Accepted)
    {
        edit->setText(select->chr());
    }
    delete dia;
}

void PSymbolCombo::updateProperty(const QString& val)
{
    emit propertyChanged(pname(), val);
}



//class PLineStyle

PLineStyle::PLineStyle(const PropertyEditor *editor, const QString pname, const QString value, QWidget *parent, const char *name):
    QComboBox(parent, name)
{
    const char *nopen[]={
    "48 16 1 1",
    ". c None",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................"};
    insertItem(QPixmap(nopen));

    const char *solid[]={
    "48 16 2 1",
    ". c None",
    "# c #000000",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    ".###########################################....",
    ".###########################################....",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................"};
    insertItem(QPixmap(solid));

    const char *dash[]={
    "48 16 2 1",
    ". c None",
    "# c #000000",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    ".#########..#########..#########..##########....",
    ".#########..#########..#########..##########....",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................"};
    insertItem(QPixmap(dash));

    const char *dashdot[]={
    "48 16 2 1",
    ". c None",
    "# c #000000",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    ".#########..##..#########..##..#########..##....",
    ".#########..##..#########..##..#########..##....",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................"};
    insertItem(QPixmap(dashdot));

    const char *dashdotdot[]={
    "48 16 2 1",
    ". c None",
    "# c #000000",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    ".#########..##..##..#########..##..##..#####....",
    ".#########..##..##..#########..##..##..#####....",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................"};
    insertItem(QPixmap(dashdotdot));

    setValue(value, false);
    setPName(pname);
    connect(this, SIGNAL(activated(int)), this, SLOT(updateProperty(int)));
    connect(this, SIGNAL(propertyChanged(QString, QString)), editor, SLOT(emitPropertyChange(QString, QString)));
}

QString PLineStyle::value() const
{
    return QString("%1").arg(currentItem());
}

void PLineStyle::setValue(const QString value, bool emitChange)
{
    if (value == "0") { setCurrentItem(0);         if (emitChange)
            emit propertyChanged(pname(), value);
 return; };
    if (value == "1") { setCurrentItem(1);         if (emitChange)
            emit propertyChanged(pname(), value);
 return; };
    if (value == "2") { setCurrentItem(2);         if (emitChange)
            emit propertyChanged(pname(), value);
 return; };
    if (value == "3") { setCurrentItem(3);         if (emitChange)
            emit propertyChanged(pname(), value);
 return; };
    if (value == "4") { setCurrentItem(4);         if (emitChange)
            emit propertyChanged(pname(), value);
 return; };
    if (value == "5") { setCurrentItem(5);          if (emitChange)
            emit propertyChanged(pname(), value);
 return; };
}

void PLineStyle::updateProperty(int val)
{
    emit propertyChanged(pname(), value());
}


#include "propertywidgets.moc"
