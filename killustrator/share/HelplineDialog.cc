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

#include <kapp.h>
#include <klocale.h>
#include <kseparator.h>
#include <kbuttonbox.h>
#include <qpushbt.h>
#include <qlabel.h>
#include <qlayout.h>
#include "HelplineDialog.h"
#include "HelplineDialog.moc"
#include "units.h"
#include "PStateManager.h"

HelplineDialog::HelplineDialog (Canvas* c, QWidget* parent, 
				const char* name) : 
    QTabDialog (parent, name, true) {
  QWidget* widget;

  canvas = c;
  setCaption (i18n ("Setup Helplines"));

  horizLines = canvas->getHorizHelplines ();
  vertLines = canvas->getVertHelplines ();

  widget = createHorizLineWidget (this);
  addTab (widget, i18n ("Horizontal"));
  widget = createVertLineWidget (this);
  addTab (widget, i18n ("Vertical"));

  initLists ();

  setOkButton (i18n ("OK"));
  setCancelButton (i18n ("Cancel"));

  connect (this, SIGNAL(applyButtonPressed ()), this, SLOT(applyPressed ()));

  adjustSize ();
 
  setMinimumSize (300, 250);
  setMaximumSize (300, 250);
}

QWidget* HelplineDialog::createHorizLineWidget (QWidget* parent) {
  QWidget* w;
  QPushButton* button;
  
  w = new QWidget (parent);

  horizValue = new UnitBox (w);
  horizValue->setRange (-1000.0, 1000.0);
  horizValue->setStep (0.1);
  horizValue->setEditable (true);
  horizValue->move (10, 20);

  horizList = new QListBox (w);
  horizList->setFixedVisibleLines (6);
  horizList->move (10, 60);
  horizList->setMultiSelection (false);
  connect (horizList, SIGNAL(highlighted (int)), 
	   this, SLOT(horizLineSelected(int)));

  button = new QPushButton (w);
  button->setText (i18n ("Add"));
  button->move (180, 20);
  connect (button, SIGNAL(clicked ()), this, SLOT(addHorizLine ()));

  button = new QPushButton (w);
  button->setText (i18n ("Update"));
  button->move (180, 60);
  connect (button, SIGNAL(clicked ()), this, SLOT(updateHorizLine ()));

  button = new QPushButton (w);
  button->setText (i18n ("Delete"));
  button->move (180, 100);
  connect (button, SIGNAL(clicked ()), this, SLOT(deleteHorizLine ()));

  return w;
}

QWidget* HelplineDialog::createVertLineWidget (QWidget* parent) {
  QWidget* w;
  QPushButton* button;
  
  w = new QWidget (parent);

  vertValue = new UnitBox (w);
  vertValue->setRange (-1000.0, 1000.0);
  vertValue->setStep (0.1);
  vertValue->setEditable (true);
  vertValue->move (10, 20);

  vertList = new QListBox (w);
  vertList->setFixedVisibleLines (6);
  vertList->move (10, 60);
  vertList->setMultiSelection (false);
  connect (vertList, SIGNAL(highlighted (int)), 
	   this, SLOT(vertLineSelected(int)));

  button = new QPushButton (w);
  button->setText (i18n ("Add"));
  button->move (180, 20);
  connect (button, SIGNAL(clicked ()), this, SLOT(addVertLine ()));

  button = new QPushButton (w);
  button->setText (i18n ("Update"));
  button->move (180, 60);
  connect (button, SIGNAL(clicked ()), this, SLOT(updateVertLine ()));

  button = new QPushButton (w);
  button->setText (i18n ("Delete"));
  button->move (180, 100);
  connect (button, SIGNAL(clicked ()), this, SLOT(deleteVertLine ()));

  return w;
}

void HelplineDialog::applyPressed () {
  canvas->setHorizHelplines (horizLines);
  canvas->setVertHelplines (vertLines);
  accept ();
}

void HelplineDialog::helpPressed () {
}

void HelplineDialog::initLists () {
  vector<float>::iterator i;
  char buf[20];
  MeasurementUnit unit = 
    PStateManager::instance ()->defaultMeasurementUnit ();

  for (i = horizLines.begin (); i != horizLines.end (); i++) {
    float value = *i;
    sprintf (buf, "%.3f %s", cvtPtToUnit (unit, value), unitToString (unit));
    horizList->insertItem (buf);
  }

  for (i = vertLines.begin (); i != vertLines.end (); i++) {
    float value = *i;
    sprintf (buf, "%.3f %s", cvtPtToUnit (unit, value), unitToString (unit));
    vertList->insertItem (buf);
  }
}

void HelplineDialog::addHorizLine () {
  float value = horizValue->getValue ();
  horizLines.push_back (value);
  char buf[20];
  MeasurementUnit unit = 
    PStateManager::instance ()->defaultMeasurementUnit ();
  sprintf (buf, "%.3f %s", cvtPtToUnit (unit, value), unitToString (unit));
  horizList->insertItem (buf);
}

void HelplineDialog::updateHorizLine () {
  int idx = horizList->currentItem ();
  if (idx != -1) {
    float value = horizValue->getValue ();
    char buf[20];
    MeasurementUnit unit = 
      PStateManager::instance ()->defaultMeasurementUnit ();
    sprintf (buf, "%.3f %s", cvtPtToUnit (unit, value), unitToString (unit));
    horizList->changeItem (buf, idx);
    horizLines[idx] = value;
  }
}

void HelplineDialog::deleteHorizLine () {
  int idx = horizList->currentItem ();
  if (idx != -1) {
    horizList->removeItem (idx);
    vector<float>::iterator it = horizLines.begin ();
    advance (it, idx);
    horizLines.erase (it);
  }
}

void HelplineDialog::addVertLine () {
  float value = vertValue->getValue ();
  vertLines.push_back (value);
  char buf[20];
  MeasurementUnit unit = 
    PStateManager::instance ()->defaultMeasurementUnit ();
  sprintf (buf, "%.3f %s", cvtPtToUnit (unit, value), unitToString (unit));
  vertList->insertItem (buf);
}

void HelplineDialog::updateVertLine () {
  int idx = vertList->currentItem ();
  if (idx != -1) {
    float value = vertValue->getValue ();
    char buf[20];
    MeasurementUnit unit = 
      PStateManager::instance ()->defaultMeasurementUnit ();
    sprintf (buf, "%.3f %s", cvtPtToUnit (unit, value), unitToString (unit));
    vertList->changeItem (buf, idx);
    vertLines[idx] = value;
  }
}

void HelplineDialog::deleteVertLine () {
  int idx = vertList->currentItem ();
  if (idx != -1) {
    vertList->removeItem (idx);
    vector<float>::iterator it = vertLines.begin ();
    advance (it, idx);
    vertLines.erase (it);
  }
}

void HelplineDialog::horizLineSelected (int idx) {
    vector<float>::iterator it = horizLines.begin ();
    advance (it, idx);
    horizValue->setValue (*it);
}

void HelplineDialog::vertLineSelected (int idx) {
    vector<float>::iterator it = vertLines.begin ();
    advance (it, idx);
    vertValue->setValue (*it);
}

void HelplineDialog::setup (Canvas *c) {
  HelplineDialog *dialog = new HelplineDialog (c, 0L);
  dialog->exec ();
  delete dialog;
}  
