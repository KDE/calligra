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

#include "AlignmentDialog.h"
#include "AlignmentDialog.moc"

#include <stdio.h>

#include <klocale.h>
#include <kapp.h>
#include <kbuttonbox.h>
#include <kseparator.h>
#include <kiconloader.h>
#include <ktabctl.h>

#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qradiobutton.h>
#include <qcheckbox.h>

#include "GDocument.h"
#include "GObject.h"
#include "CommandHistory.h"

#define BUTTON_WIDTH  40
#define BUTTON_HEIGHT 40

AlignmentDialog::AlignmentDialog (QWidget* parent, const char* name) :
    QDialog (parent, name, true) {
  QPushButton* button;
  QWidget* widget;

  setCaption (i18n ("Alignment"));

  QVBoxLayout *vl = new QVBoxLayout (this, 2);

  // the tab control
  tabctl = new KTabCtl (this);
  connect (tabctl, SIGNAL(tabSelected(int)), this, SLOT(selectTab(int)));

  activeTab = 0;

  widget = createAlignmentWidget (tabctl);
  tabctl->addTab (widget, i18n ("Align"));

  widget = createDistributionWidget (tabctl);
  tabctl->addTab (widget, i18n ("Distribute"));

  vl->addWidget (tabctl, 1);

  // a separator
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
  adjustSize ();

  setMinimumSize (300, 300);
  setMaximumSize (350, 300);
}

QWidget* AlignmentDialog::createAlignmentWidget (QWidget* parent) {
  QWidget* w;
  QButtonGroup* group;
  QGroupBox* box;

#if NEWKDE
  KIconLoader* loader = KGlobal::iconLoader ();
#else
  KIconLoader* loader = kapp->getIconLoader ();
#endif

  w = new QWidget (parent);
  QGridLayout *layout = new QGridLayout (w, 2, 3, 10);

  group = new QButtonGroup (w, "Vertically");
  group->setTitle (i18n ("Vertically"));

  valignButton[0] = new QPushButton (group);
  valignButton[0]->setToggleButton (true);
  valignButton[0]->setPixmap (UserIcon ("atop"));
  valignButton[0]->setGeometry (20, 30,
				BUTTON_WIDTH, BUTTON_HEIGHT);


  valignButton[1] = new QPushButton (group);
  valignButton[1]->setToggleButton (true);
  valignButton[1]->setPixmap (UserIcon ("avcenter"));
  valignButton[1]->setGeometry (20, 30 + 1 * BUTTON_HEIGHT,
				BUTTON_WIDTH, BUTTON_HEIGHT);

  valignButton[2] = new QPushButton (group);
  valignButton[2]->setToggleButton (true);
  valignButton[2]->setPixmap (UserIcon ("abottom"));
  valignButton[2]->setGeometry (20, 30 + 2 * BUTTON_HEIGHT,
				BUTTON_WIDTH, BUTTON_HEIGHT);

  group->setExclusive (true);
#if NEWKDE
  layout->addMultiCellWidget (group, 0, 1, 0, 0, 0);
#else /*TB1999-10-23: Don't know whether the Qt 1.x version is needed */
  layout->addMultiCellWidget (group, 0, 1, 0, 0, AlignCenter);
#endif

  group = new QButtonGroup (w, "Horizontally");
  group->setTitle (i18n ("Horizontally"));

  halignButton[0] = new QPushButton (group);
  halignButton[0]->setToggleButton (true);
  halignButton[0]->setPixmap (UserIcon ("aleft"));
  halignButton[0]->setGeometry (20, 30, BUTTON_WIDTH,
				BUTTON_HEIGHT);

  halignButton[1] = new QPushButton (group);
  halignButton[1]->setToggleButton (true);
  halignButton[1]->setPixmap (UserIcon ("ahcenter"));
  halignButton[1]->setGeometry (20 + BUTTON_WIDTH, 30,
                           BUTTON_WIDTH, BUTTON_HEIGHT);

  halignButton[2] = new QPushButton (group);
  halignButton[2]->setToggleButton (true);
  halignButton[2]->setPixmap (UserIcon ("aright"));
  halignButton[2]->setGeometry (20 + 2 * BUTTON_WIDTH,
                           30, BUTTON_WIDTH, BUTTON_HEIGHT);

  group->setExclusive (true);
#if NEWKDE
  layout->addMultiCellWidget (group, 0, 0, 1, 2, 0);
#else /*TB1999-10-23: Don't know whether the Qt 1.x version is needed */
  layout->addMultiCellWidget (group, 0, 0, 1, 2, AlignCenter);
#endif

  box = new QGroupBox (w);
  gbutton = new QCheckBox (box);
  gbutton->setText (i18n ("To Grid"));
  gbutton->setFixedSize (gbutton->sizeHint ());
  gbutton->move (15, 20);

  cbutton = new QCheckBox (box);
  cbutton->setText (i18n ("Align To Center of Page"));
  cbutton->setFixedSize (cbutton->sizeHint ());
  cbutton->move (15, 45);

#if NEWKDE
  layout->addMultiCellWidget (box, 1, 1, 1, 2, 0);
#else /*TB1999-10-23: Don't know whether the Qt 1.x version is needed */
  layout->addMultiCellWidget (box, 1, 1, 1, 2, AlignCenter);
#endif

  layout->activate ();
  w->adjustSize ();
  return w;
}

QWidget* AlignmentDialog::createDistributionWidget (QWidget* parent) {
  QWidget* w;
  QButtonGroup* group;

#if NEWKDE
  KIconLoader* loader = KGlobal::iconLoader ();
#else
  KIconLoader* loader = kapp->getIconLoader ();
#endif

  w = new QWidget (parent);
  QGridLayout *layout = new QGridLayout (w, 2, 3, 10);

  group = new QButtonGroup (w, "Vertically");
  group->setTitle (i18n ("Vertically"));

  vdistButton[0] = new QPushButton (group);
  vdistButton[0]->setToggleButton (true);
  vdistButton[0]->setPixmap (UserIcon ("dtop"));
  vdistButton[0]->setGeometry (20, 30, BUTTON_WIDTH, BUTTON_HEIGHT);


  vdistButton[1] = new QPushButton (group);
  vdistButton[1]->setToggleButton (true);
  vdistButton[1]->setPixmap (UserIcon ("dvcenter"));
  vdistButton[1]->setGeometry (20, 30 + 1 * BUTTON_HEIGHT,
			       BUTTON_WIDTH, BUTTON_HEIGHT);

  vdistButton[2] = new QPushButton (group);
  vdistButton[2]->setToggleButton (true);
  vdistButton[2]->setPixmap (UserIcon ("dvdist"));
  vdistButton[2]->setGeometry (20, 30 + 2 * BUTTON_HEIGHT,
			       BUTTON_WIDTH, BUTTON_HEIGHT);

  vdistButton[3] = new QPushButton (group);
  vdistButton[3]->setToggleButton (true);
  vdistButton[3]->setPixmap (UserIcon ("dbottom"));
  vdistButton[3]->setGeometry (20, 30 + 3 * BUTTON_HEIGHT,
			       BUTTON_WIDTH, BUTTON_HEIGHT);

  group->setExclusive (true);
  layout->addMultiCellWidget (group, 0, 1, 0, 0, AlignCenter);
#if NEWKDE
  layout->addMultiCellWidget (group, 0, 1, 0, 0, 0);
#else /*TB1999-10-23: Don't know whether the Qt 1.x version is needed */
  layout->addMultiCellWidget (group, 0, 1, 0, 0, AlignCenter);
#endif

  group = new QButtonGroup (w, "Horizontally");
  group->setTitle (i18n ("Horizontally"));

  hdistButton[0] = new QPushButton (group);
  hdistButton[0]->setToggleButton (true);
  hdistButton[0]->setPixmap (UserIcon ("dleft"));
  hdistButton[0]->setGeometry (20, 30, BUTTON_WIDTH,
				BUTTON_HEIGHT);

  hdistButton[1] = new QPushButton (group);
  hdistButton[1]->setToggleButton (true);
  hdistButton[1]->setPixmap (UserIcon ("dhcenter"));
  hdistButton[1]->setGeometry (20 + BUTTON_WIDTH, 30,
                           BUTTON_WIDTH, BUTTON_HEIGHT);

  hdistButton[2] = new QPushButton (group);
  hdistButton[2]->setToggleButton (true);
  hdistButton[2]->setPixmap (UserIcon ("dhdist"));
  hdistButton[2]->setGeometry (20 + 2 * BUTTON_WIDTH,
			       30, BUTTON_WIDTH, BUTTON_HEIGHT);

  hdistButton[3] = new QPushButton (group);
  hdistButton[3]->setToggleButton (true);
  hdistButton[3]->setPixmap (UserIcon ("dright"));
  hdistButton[3]->setGeometry (20 + 3 * BUTTON_WIDTH,
                           30, BUTTON_WIDTH, BUTTON_HEIGHT);

  group->setExclusive (true);
#if NEWKDE
  layout->addMultiCellWidget (group, 0, 0, 1, 2, 0);
#else /*TB1999-10-23: Don't know whether the Qt 1.x version is needed */
  layout->addMultiCellWidget (group, 0, 0, 1, 2, AlignCenter);
#endif

  group = new QButtonGroup (w);
  group->setTitle (i18n ("Distribute at"));

  sbutton = new QRadioButton (group);
  sbutton->setText (i18n ("Selection"));
  sbutton->move (15, 20);
  sbutton->setChecked (true);

  pbutton = new QRadioButton (group);
  pbutton->setText (i18n ("Page"));
  pbutton->move (15, 45);

  layout->addMultiCellWidget (group, 1, 1, 1, 2, AlignCenter);
#if NEWKDE
  layout->addMultiCellWidget (group, 1, 1, 1, 2, 0);
#else /*TB1999-10-23: Don't know whether the Qt 1.x version is needed */
  layout->addMultiCellWidget (group, 1, 1, 1, 2, AlignCenter);
#endif

  layout->activate ();
  w->adjustSize ();
  return w;
}

void AlignmentDialog::helpPressed () {
}

HorizAlignment AlignmentDialog::getHorizAlignment () {
  HorizAlignment result = HAlign_None;

  if (halignButton[0]->isOn ())
    result = HAlign_Left;
  else if (halignButton[1]->isOn ())
    result = HAlign_Center;
  else if (halignButton[2]->isOn ())
    result = HAlign_Right;

  return result;
}

VertAlignment AlignmentDialog::getVertAlignment () {
  VertAlignment result = VAlign_None;

  if (valignButton[0]->isOn ())
    result = VAlign_Top;
  else if (valignButton[1]->isOn ())
    result = VAlign_Center;
  else if (valignButton[2]->isOn ())
    result = VAlign_Bottom;

  return result;
}

bool AlignmentDialog::centerToPage () {
  return cbutton->isOn ();
}

bool AlignmentDialog::snapToGrid () {
  return gbutton->isOn ();
}

HorizDistribution AlignmentDialog::getHorizDistribution () {
  HorizDistribution result = HDistrib_None;

  if (hdistButton[0]->isOn ())
    result = HDistrib_Left;
  if (hdistButton[1]->isOn ())
    result = HDistrib_Center;
  if (hdistButton[2]->isOn ())
    result = HDistrib_Distance;
  if (hdistButton[3]->isOn ())
    result = HDistrib_Right;

  return result;
}

VertDistribution AlignmentDialog::getVertDistribution () {
  VertDistribution result = VDistrib_None;

  if (vdistButton[0]->isOn ())
    result = VDistrib_Top;
  if (vdistButton[1]->isOn ())
    result = VDistrib_Center;
  if (vdistButton[2]->isOn ())
    result = VDistrib_Distance;
  if (vdistButton[3]->isOn ())
    result = VDistrib_Bottom;

  return result;
}

DistributionMode AlignmentDialog::getDistributionMode () {
  return (sbutton->isOn () ? DMode_AtSelection : DMode_AtPage);
}

AlignmentMode AlignmentDialog::getMode () {
  return (activeTab == 0 ? AMode_Align : AMode_Distribute);
}

void AlignmentDialog::selectTab (int t) {
  activeTab = t;
}

void AlignmentDialog::alignSelection (GDocument* doc,
				      CommandHistory* history) {
  if (! doc->selectionIsEmpty ()) {
    AlignmentDialog dialog (0L, "Alignment");

    int result = dialog.exec ();
    if (result == Accepted) {
      if (dialog.getMode () == AMode_Align) {
        AlignCmd *cmd = new AlignCmd (doc, dialog.getHorizAlignment (),
				      dialog.getVertAlignment (),
				      dialog.centerToPage (),
				      dialog.snapToGrid ());
        history->addCommand (cmd, true);
      }
      else {
        DistributeCmd *cmd =
	  new DistributeCmd (doc, dialog.getHorizDistribution (),
			     dialog.getVertDistribution (),
			     dialog.getDistributionMode ());
        history->addCommand (cmd, true);
      }
    }
  }
}

#undef BUTTON_WIDTH
#undef BUTTON_HEIGHT
