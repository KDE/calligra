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

#include "EllipseConfigDialog.h"
#include "EllipseConfigDialog.moc"

#include <stdio.h>

#include <klocale.h>
#include <kapp.h>
#include <kbuttonbox.h>
#include <kseparator.h>

#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qframe.h>

EllipseConfigDialog::EllipseConfigDialog (QWidget* parent, const char* name) : 
    QDialog (parent, name, true) {
  QPushButton* button;
  QWidget* widget;

  setCaption (i18n ("Setup Ellipse Tool"));

  QVBoxLayout *vl = new QVBoxLayout (this, 2);

  widget = createWidget (this);
  vl->addWidget (widget);

  KSeparator* sep = new KSeparator (this);
  vl->addWidget (sep);

  // the standard buttons
  KButtonBox *bbox = new KButtonBox (this);
  button = bbox->addButton (i18n ("OK"));
  connect (button, SIGNAL (clicked ()), SLOT (accept ()));
  button = bbox->addButton (i18n ("Cancel"));
  connect (button, SIGNAL (clicked ()), SLOT (reject ()));
  bbox->addStretch (1);
  button = bbox->addButton (i18n ("Help"));
  connect (button, SIGNAL (clicked ()), SLOT (helpPressed ()));
  bbox->layout ();
  bbox->setMinimumSize (bbox->sizeHint ());

  vl->addWidget (bbox);
  vl->activate ();
 
  setMinimumSize (250, 100);
  setMaximumSize (300, 150);
}

QWidget* EllipseConfigDialog::createWidget (QWidget* parent) {
  QWidget* w;
  QButtonGroup* group;

  w = new QWidget (parent);

  group = new QButtonGroup (w);
  group->setTitle (i18n ("Draw ellipse"));
  group->setGeometry (10, 10, 160, 65);
  
  radiusButton = new QRadioButton (group);
  radiusButton->setText (i18n ("between points"));
  radiusButton->move (10, 20);
  radiusButton->setFixedSize (radiusButton->sizeHint ());

  diameterButton = new QRadioButton (group);
  diameterButton->setText (i18n ("around fixed center"));
  diameterButton->move (10, 40);
  diameterButton->setFixedSize (diameterButton->sizeHint ());

  w->setMinimumSize (200, 90);
  w->setMaximumSize (200, 90);
  return w;
}

void EllipseConfigDialog::helpPressed () {
}

void EllipseConfigDialog::setupTool (OvalTool* tool) {
  EllipseConfigDialog dialog;

  if (tool->aroundFixedCenter ())
    dialog.diameterButton->setChecked (true);
  else
    dialog.radiusButton->setChecked (true);
      
  int result = dialog.exec ();
  if (result == Accepted) {
    tool->aroundFixedCenter (dialog.diameterButton->isChecked ());
  }
}
  
