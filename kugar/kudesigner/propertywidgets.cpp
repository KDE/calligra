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
 
#include "propertywidgets.h"

QString PLineEdit::value() const
{
    return text();
}

void PLineEdit::setValue(const QString value)
{
    setText(value);
}


QString PSpinBox::value() const
{
    return cleanText();
}

void PSpinBox::setValue(const QString value)
{
    QSpinBox::setValue(value.toInt());
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

PFontCombo::PFontCombo (QWidget *parent, const char *name):
    KFontCombo(parent, name)
{
    setEditable(false);    
}

PFontCombo::PFontCombo (const QStringList &fonts, QWidget *parent, const char *name):
    KFontCombo(fonts, parent, name)
{
    setEditable(false);
}

QString PFontCombo::value() const
{
    return currentFont();
}

void PFontCombo::setValue(const QString value)
{
    setCurrentFont(value);    
}

PColorCombo::PColorCombo(QWidget *parent, const char *name):
    KColorCombo(parent, name)
{
    
}

QString PColorCombo::value() const
{
    return QString("%1,%2,%3").arg(color().red(), color().green(), color().blue());
}

void PColorCombo::setValue(const QString value)
{
    setColor(QColor(value.section(',', 0, 0).toInt(),
        value.section(',', 1, 1).toInt(),
        value.section(',', 2, 2).toInt()));
}

PSymbolCombo::PSymbolCombo(QWidget *parent, const char *name):
    QWidget(parent, name)
{
    l = new QHBoxLayout(this);

    edit = new QLineEdit(this);
    edit->setMaxLength(1);
    l->addWidget(edit);
    pbSelect = new QPushButton("...", this);
    l->addWidget(pbSelect);

    connect(pbSelect, SIGNAL(clicked()), this, SLOT(selectChar()));
}

QString PSymbolCombo::value() const
{
    if (!(edit->text().isNull()))
        return QString("%1").arg(edit->text().at(0).unicode());
    else
        return "";
}

void PSymbolCombo::setValue(const QString value)
{
    if (!(value.isNull()))
    {
        edit->setText(QChar(value.toInt()));
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

#include "propertywidgets.moc"
