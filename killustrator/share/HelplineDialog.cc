/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as
  published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include <HelplineDialog.h>

#include <qlistbox.h>
#include <qpushbutton.h>
#include <qlayout.h>

#include <klocale.h>
#include <kdebug.h>

#include <Canvas.h>
#include <UnitBox.h>
#include <units.h>
#include <PStateManager.h>

HelplineDialog::HelplineDialog (Canvas* c, QWidget* parent,
                                const char* name) : KDialogBase(KDialogBase::Tabbed,
                                                                i18n("Setup Helplines"),
                                                                KDialogBase::Ok | KDialogBase::Cancel,
                                                                KDialogBase::Ok, parent, name, true) {
  canvas = c;
  horizLines = canvas->getHorizHelplines ();
  vertLines = canvas->getVertHelplines ();

  createHorizLineWidget(addPage(i18n("Horizontal")));
  createVertLineWidget(addPage(i18n("Vertical")));
  initLists ();
}

void HelplineDialog::createHorizLineWidget (QWidget* parent) {

    QBoxLayout *layout=new QHBoxLayout(parent, KDialogBase::marginHint(), KDialogBase::spacingHint());
    QBoxLayout *left=new QVBoxLayout(layout);

    horizValue = new UnitBox (parent);
    horizValue->setRange (-1000.0, 1000.0);
    horizValue->setStep (0.1);
    horizValue->setEditable (true);
    horizValue->setValue(0.0);
    left->addWidget(horizValue);

    horizList = new QListBox (parent);
    horizList->setMultiSelection (false);
    connect (horizList, SIGNAL(highlighted(int)),
             this, SLOT(horizLineSelected(int)));
    left->addWidget(horizList);
    layout->addSpacing(KDialogBase::spacingHint()*2);

    QBoxLayout *right=new QVBoxLayout(layout);
    QPushButton *button = new QPushButton (i18n("Add"), parent);
    connect (button, SIGNAL(clicked ()), this, SLOT(addHorizLine ()));
    right->addWidget(button);

    button = new QPushButton(i18n("Update"), parent);
    connect (button, SIGNAL(clicked ()), this, SLOT(updateHorizLine ()));
    right->addWidget(button);

    button = new QPushButton(i18n("Delete"), parent);
    connect (button, SIGNAL(clicked ()), this, SLOT(deleteHorizLine ()));
    right->addWidget(button);
    right->addStretch();
}

void HelplineDialog::createVertLineWidget (QWidget* parent) {

    QBoxLayout *layout=new QHBoxLayout(parent, KDialogBase::marginHint(), KDialogBase::spacingHint());
    QBoxLayout *left=new QVBoxLayout(layout);

    vertValue = new UnitBox (parent);
    vertValue->setRange (-1000.0, 1000.0);
    vertValue->setStep (0.1);
    vertValue->setEditable (true);
    vertValue->setValue(0.0);
    left->addWidget(vertValue);

    vertList = new QListBox(parent);
    vertList->setMultiSelection (false);
    connect (vertList, SIGNAL(highlighted (int)),
             this, SLOT(vertLineSelected(int)));
    left->addWidget(vertList);
    layout->addSpacing(KDialogBase::spacingHint()*2);

    QBoxLayout *right=new QVBoxLayout(layout);
    QPushButton *button = new QPushButton(i18n("Add"), parent);
    connect (button, SIGNAL(clicked ()), this, SLOT(addVertLine ()));
    right->addWidget(button);

    button = new QPushButton(i18n("Update"), parent);
    connect (button, SIGNAL(clicked ()), this, SLOT(updateVertLine ()));
    right->addWidget(button);

    button = new QPushButton(i18n("Delete"), parent);
    connect (button, SIGNAL(clicked ()), this, SLOT(deleteVertLine ()));
    right->addWidget(button);
    right->addStretch();
}

void HelplineDialog::applyPressed ()
{
    canvas->setHorizHelplines (horizLines);
    canvas->setVertHelplines (vertLines);
    canvas->saveGridProperties();
}

void HelplineDialog::initLists () {
    QValueList<float>::Iterator i;
    QString buf;
    MeasurementUnit unit =
        PStateManager::instance ()->defaultMeasurementUnit ();

    for (i = horizLines.begin (); i != horizLines.end (); ++i) {
        buf=QString::number(cvtPtToUnit (unit, *i), 'f', 3);
        buf+=" ";
        buf+=unitToString (unit);
        horizList->insertItem (buf);
    }
    if(!horizLines.isEmpty())
        horizValue->setValue(horizLines[0]);

    for (i = vertLines.begin (); i != vertLines.end (); ++i) {
        buf=QString::number(cvtPtToUnit (unit, *i), 'f', 3);
        buf+=" ";
        buf+=unitToString (unit);
        vertList->insertItem (buf);
    }
    if(!vertLines.isEmpty())
        vertValue->setValue(vertLines[0]);
}

void HelplineDialog::addHorizLine () {
  float value = horizValue->getValue ();
  horizLines.append(value);
  MeasurementUnit unit = PStateManager::instance ()->defaultMeasurementUnit ();
  QString buf=QString::number(cvtPtToUnit (unit, value), 'f', 3);
  buf+=" ";
  buf+=unitToString (unit);
  horizList->insertItem (buf);
}

void HelplineDialog::updateHorizLine () {
    if(horizLines.isEmpty())
        return;
    int idx = horizList->currentItem ();
    if (idx != -1) {
        float value = horizValue->getValue ();
        MeasurementUnit unit =
            PStateManager::instance ()->defaultMeasurementUnit ();
        QString buf=QString::number(cvtPtToUnit (unit, value), 'f', 3);
        buf+=" ";
        buf+=unitToString (unit);
        horizList->blockSignals(true);
        horizList->changeItem (buf, idx);
        horizList->blockSignals(false);
        horizLines[idx] = value;
    }
}

void HelplineDialog::deleteHorizLine () {
    if(horizLines.isEmpty())
        return;
    int idx = horizList->currentItem ();
    if (idx != -1) {
        horizLines.remove(horizLines.at(idx));
        horizList->removeItem(idx);
    }
}

void HelplineDialog::addVertLine () {
    float value = vertValue->getValue ();
    vertLines.append(value);
    MeasurementUnit unit =
        PStateManager::instance ()->defaultMeasurementUnit ();
    QString buf=QString::number(cvtPtToUnit (unit, value), 'f', 3);
    buf+=" ";
    buf+=unitToString (unit);
    vertList->insertItem (buf);
}

void HelplineDialog::updateVertLine () {
    if(vertLines.isEmpty())
        return;
    int idx = vertList->currentItem ();
    if (idx != -1) {
        float value = vertValue->getValue ();
        MeasurementUnit unit =
            PStateManager::instance ()->defaultMeasurementUnit ();
        QString buf=QString::number(cvtPtToUnit (unit, value), 'f', 3);
        buf+=" ";
        buf+=unitToString (unit);
        vertList->blockSignals(true);
        vertList->changeItem (buf, idx);
        vertList->blockSignals(false);
        vertLines[idx] = value;
    }
}

void HelplineDialog::deleteVertLine () {
    if(vertLines.isEmpty())
        return;
    int idx = vertList->currentItem ();
    if (idx != -1) {
        vertLines.remove(vertLines.at(idx));
        vertList->removeItem (idx);
    }
}

void HelplineDialog::horizLineSelected (int idx) {
    if(!horizLines.isEmpty())
        horizValue->setValue(*horizLines.at(idx));
}

void HelplineDialog::vertLineSelected (int idx) {
    if(!vertLines.isEmpty())
        vertValue->setValue (*vertLines.at(idx));
}

void HelplineDialog::setup (Canvas *c)
{
  HelplineDialog *dialog = new HelplineDialog (c, 0L);
  if(dialog->exec()==Accepted)
      dialog->applyPressed();
  delete dialog;
}

#include <HelplineDialog.moc>
