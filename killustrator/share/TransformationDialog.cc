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

#include "TransformationDialog.h"
#include "TransformationDialog.moc"

#include <stdio.h>

#include <klocale.h>
#include <kapp.h>
#include <kbuttonbox.h>
#include <kseparator.h>
#include <kiconloader.h>

#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qradiobutton.h>
#include <qcheckbox.h>

#include "TranslateCmd.h"
#include "RotateCmd.h"
#include "ScaleCmd.h"
#include "InsertObjCmd.h"
#include "Handle.h" // for scaling directions
#include "GDocument.h"
#include "GObject.h"
#include "CommandHistory.h"
#include "FloatSpinBox.h"
#include "UnitBox.h"
#include "MyTabCtl.h"

TransformationDialog::TransformationDialog (CommandHistory* cmdHist,
					    QWidget* parent,
					    const char* name) :
  QDialog (parent, name, false) {
  document = 0L;
  history = cmdHist;
  setCaption (i18n ("Transform"));

  QVBoxLayout *vl = new QVBoxLayout (this, 2);

  // the tab control
  tabCtl = new MyTabCtl (this);

  widgets[0] = createPositionWidget (tabCtl);
  tabCtl->addTab (widgets[0], i18n ("Position"));

  widgets[1] = createDimensionWidget (tabCtl);
  tabCtl->addTab (widgets[1], i18n ("Dimension"));

  widgets[2] = createRotationWidget (tabCtl);
  tabCtl->addTab (widgets[2], i18n ("Rotation"));

  widgets[3] = createMirrorWidget (tabCtl);
  tabCtl->addTab (widgets[3], i18n ("Mirror"));

  vl->addWidget (tabCtl, 1);

  // a separator
  KSeparator* sep = new KSeparator (this);
  vl->addWidget (sep);

  // the standard buttons
  KButtonBox *bbox = new KButtonBox (this);
  QPushButton *button = bbox->addButton (i18n ("Close"));
  connect (button, SIGNAL(clicked ()), this, SLOT(accept ()));

  bbox->layout ();
  bbox->setMinimumSize (bbox->sizeHint () + QSize (20, 20));

  vl->addWidget (bbox);

  vl->activate ();

  setMinimumSize (330, 350);
  setMaximumSize (400, 350);
  resize (330, 350);
}

QWidget* TransformationDialog::createPositionWidget (QWidget* parent) {
  QWidget* w;
  QLabel* label;

  w = new QWidget (parent);
  label = new QLabel (w);
  label->setAlignment (AlignLeft | AlignVCenter);
  label->setText (i18n ("Horizontal:"));
  label->move (20, 20);

  horizPosition = new UnitBox (w);
  horizPosition->setRange (-1000.0, 1000.0);
  horizPosition->setStep (0.1);
  horizPosition->setEditable (true);
  horizPosition->move (90, 20);

  label = new QLabel (w);
  label->setAlignment (AlignLeft | AlignVCenter);
  label->setText (i18n ("Vertical:"));
  label->move (20, 50);

  vertPosition = new UnitBox (w);
  vertPosition->setRange (-1000.0, 1000.0);
  vertPosition->setStep (0.1);
  vertPosition->setEditable (true);
  vertPosition->move (90, 50);

  relativePosition = new QCheckBox (w);
  relativePosition->setText (i18n ("Relative Position"));
  relativePosition->setGeometry (20, 80, 150, 30);
  connect( relativePosition, SIGNAL(clicked()),
           this, SLOT(relativePositionSlot()) );

  applyBttn[0] = new QPushButton (w);
  applyBttn[0]->setText (i18n ("Apply"));
  connect (applyBttn[0], SIGNAL(clicked()), this, SLOT(applyPressed()));

  applyToDupBttn[0] = new QPushButton (w);
  applyToDupBttn[0]->setText (i18n ("Apply To Duplicate"));
  connect (applyToDupBttn[0], SIGNAL(clicked()),
	   this, SLOT(applyToDuplicatePressed()));

  int width = applyToDupBttn[0]->sizeHint ().width ();
  int height = applyToDupBttn[0]->sizeHint ().height ();

  applyBttn[0]->setGeometry (20, 120, width, height);
  applyToDupBttn[0]->setGeometry (20, 122 + height, width, height);

  w->adjustSize ();
  return w;
}

QWidget* TransformationDialog::createDimensionWidget (QWidget* parent) {
  QWidget* w;
  QButtonGroup* group;
  QLabel* label;

  w = new QWidget (parent);

  group = new QButtonGroup (w);
  group->setFrameStyle (QFrame::NoFrame);
  group->setExclusive (true);
  group->move (20, 10);

  absolute = new QRadioButton (group);
  absolute->setText (i18n ("Absolute"));
  absolute->setChecked (true);
  absolute->move (0, 0);
  connect (absolute, SIGNAL(clicked ()), this, SLOT(slotAbsScale ()));

  percent = new QRadioButton (group);
  percent->setText (i18n ("Percentage"));
  percent->move (0, 25);
  connect (percent, SIGNAL(clicked ()), this, SLOT(slotPercentScale ()));
  group->adjustSize ();

  label = new QLabel (w);
  label->setAlignment (AlignLeft | AlignVCenter);
  label->setText (i18n ("Horizontal:"));
  label->move (20, 65);

  horizDim = new UnitBox (w);
  horizDim->setRange (-1000.0, 1000.0);
  horizDim->setStep (0.1);
  horizDim->setEditable (true);
  horizDim->move (90, 65);
  connect (horizDim, SIGNAL(valueChanged (float)),
	   this, SLOT(updateProportionalDimension (float)));
  label = new QLabel (w);
  label->setAlignment (AlignLeft | AlignVCenter);
  label->setText (i18n ("Vertical:"));
  label->move (20, 95);

  vertDim = new UnitBox (w);
  vertDim->setRange (-1000.0, 1000.0);
  vertDim->setStep (0.1);
  vertDim->setEditable (true);
  vertDim->move (90, 95);
  connect (vertDim, SIGNAL(valueChanged (float)),
	   this, SLOT(updateProportionalDimension (float)));

  proportional = new QCheckBox (w);
  proportional->setText (i18n ("Proportional"));
  proportional->setGeometry (20, 125, 150, 30);

  applyBttn[1] = new QPushButton (w);
  applyBttn[1]->setText (i18n ("Apply"));
  connect (applyBttn[1], SIGNAL(clicked()), this, SLOT(applyPressed()));

  applyToDupBttn[1] = new QPushButton (w);
  applyToDupBttn[1]->setText (i18n ("Apply To Duplicate"));
  connect (applyToDupBttn[1], SIGNAL(clicked()),
	   this, SLOT(applyToDuplicatePressed()));

  int width = applyToDupBttn[1]->sizeHint ().width ();
  int height = applyToDupBttn[1]->sizeHint ().height ();

  applyBttn[1]->setGeometry (20, 165, width, height);
  applyToDupBttn[1]->setGeometry (20, 167 + height, width, height);

  w->adjustSize ();
  return w;
}

QWidget* TransformationDialog::createRotationWidget (QWidget* parent) {
  QWidget* w;
  QLabel* label;
  QGroupBox* box;

  w = new QWidget (parent);

  label = new QLabel (w);
  label->setAlignment (AlignLeft | AlignVCenter);
  label->setText (i18n ("Angle:"));
  label->setFixedHeight (label->sizeHint ().height ());
  label->move (20, 20);

  rotAngle = new FloatSpinBox (w);
  rotAngle->setRange (-360.0, +360.0);
  horizPosition->setStep (0.1);
  //  rotAngle->setEditable (true);
  rotAngle->move (90, 20);

  box = new QGroupBox (w);
  box->setTitle (i18n ("Center"));
  box->move (20, 50);

  label = new QLabel (box);
  label->setAlignment (AlignLeft | AlignVCenter);
  label->setText (i18n ("Horizontal:"));
  label->move (20, 20);

  horizRotCenter = new UnitBox (box);
  horizRotCenter->setRange (-1000.0, 1000.0);
  horizRotCenter->setStep (0.1);
  horizRotCenter->setEditable (true);
  horizRotCenter->move (90, 20);

  label = new QLabel (box);
  label->setAlignment (AlignLeft | AlignVCenter);
  label->setText (i18n ("Vertical:"));
  label->move (20, 50);

  vertRotCenter = new UnitBox (box);
  vertRotCenter->setRange (-1000.0, 1000.0);
  vertRotCenter->setStep (0.1);
  vertRotCenter->setEditable (true);
  vertRotCenter->move (90, 50);

  relativeRotCenter = new QCheckBox (box);
  relativeRotCenter->setText (i18n ("Relative Position"));
  relativeRotCenter->setGeometry (20, 80, 150, 20);
  box->adjustSize ();
  connect( relativeRotCenter, SIGNAL(clicked()),
           this, SLOT(relativeRotCenterSlot()) );
  
  applyBttn[2] = new QPushButton (w);
  applyBttn[2]->setText (i18n ("Apply"));
  connect (applyBttn[2], SIGNAL(clicked()), this, SLOT(applyPressed()));

  applyToDupBttn[2] = new QPushButton (w);
  applyToDupBttn[2]->setText (i18n ("Apply To Duplicate"));
  connect (applyToDupBttn[2], SIGNAL(clicked()),
	   this, SLOT(applyToDuplicatePressed()));

  int width = applyToDupBttn[2]->sizeHint ().width ();
  int height = applyToDupBttn[2]->sizeHint ().height ();

  applyBttn[2]->setGeometry (20, 180, width, height);
  applyToDupBttn[2]->setGeometry (20, 182 + height, width, height);

  w->adjustSize ();
  return w;
}

QWidget* TransformationDialog::createMirrorWidget (QWidget* parent) {
  QWidget* w;
  QButtonGroup* group;
  w = new QWidget (parent);

  group = new QButtonGroup (w);
  group->setFrameStyle (QFrame::NoFrame);
  group->setExclusive (true);

  horizMirror = new QPushButton (group);
  horizMirror->setToggleButton (true);
  horizMirror->setPixmap (UserIcon ("hmirror"));
  horizMirror->setGeometry (20, 20, 40, 40);

  vertMirror = new QPushButton (group);
  vertMirror->setToggleButton (true);
  vertMirror->setPixmap (UserIcon ("vmirror"));
  vertMirror->setGeometry (65, 20, 40, 40);

  group->adjustSize ();

  applyBttn[3] = new QPushButton (w);
  applyBttn[3]->setText (i18n ("Apply"));
  connect (applyBttn[3], SIGNAL(clicked()), this, SLOT(applyPressed()));

  applyToDupBttn[3] = new QPushButton (w);
  applyToDupBttn[3]->setText (i18n ("Apply To Duplicate"));
  connect (applyToDupBttn[3], SIGNAL(clicked()),
	   this, SLOT(applyToDuplicatePressed()));

  int width = applyToDupBttn[3]->sizeHint ().width ();
  int height = applyToDupBttn[3]->sizeHint ().height ();

  applyBttn[3]->setGeometry (20, 110, width, height);
  applyToDupBttn[3]->setGeometry (20, 112 + height, width, height);

  w->adjustSize ();
  return w;
}

void TransformationDialog::relativePositionSlot(){
  if( relativePosition->isChecked()){
     horizPosition->setValue(0.0);
     vertPosition-> setValue(0.0);
  }
  else
  {
     Rect r = document->boundingBoxForSelection ();
     horizPosition->setValue (r.left ());
     vertPosition->setValue (r.top ());
  }
}

void TransformationDialog::relativeRotCenterSlot(){
  if( relativeRotCenter->isChecked()){
     horizRotCenter->setValue(0.0);
     vertRotCenter-> setValue(0.0);
  }
  else
  {
     Rect r = document->boundingBoxForSelection ();
     horizRotCenter->setValue (r.center ().x ());
     vertRotCenter->setValue (r.center ().y ());
  }
}

void TransformationDialog::applyToDuplicatePressed () {
  if (document == 0L || document->selectionIsEmpty ())
    return;

  int buttonId = -1;

  for (int i = 0; i < 4; i++)
    if (sender () == applyToDupBttn[i]) {
      buttonId = i;
      break;
    }
  switch (buttonId) {
  case 0:
    translate (true);
    break;
  case 1:
    scale (true);
    break;
  case 2:
    rotate (true);
    break;
  case 3:
    mirror (true);
    break;
  default:
    return;
  }
}

void TransformationDialog::applyPressed () {
  if (document == 0L || document->selectionIsEmpty ())
    return;

  int buttonId = -1;

  for (int i = 0; i < 4; i++)
    if (sender () == applyBttn[i]) {
      buttonId = i;
      break;
    }
  switch (buttonId) {
  case 0:
    translate (false);
    break;
  case 1:
    scale (false);
    break;
  case 2:
    rotate (false);
    break;
  case 3:
    mirror (false);
    break;
  default:
    return;
  }
}

void TransformationDialog::translate (bool onDuplicate) {
  float xval, yval;

  xval = horizPosition->getValue ();
  yval = vertPosition->getValue ();

  if (! relativePosition->isChecked ()) {
    // the given values are relative to the current bounding box
    Rect r = document->boundingBoxForSelection ();
    xval -= r.left ();
    yval -= r.top ();
  }
  if (onDuplicate) {
    QList<GObject> duplicates;
    duplicates.setAutoDelete (false);

    for (list<GObject*>::iterator it = document->getSelection ().begin ();
	 it != document->getSelection ().end (); it++) {
      GObject* obj = (*it)->copy ();
      QWMatrix m;
      m.translate (xval, yval);
      obj->transform (m, true);
      duplicates.append (obj);
    }
    InsertObjCmd* cmd = new InsertObjCmd (document, duplicates);
    history->addCommand (cmd, true);
  }
  else {
    TranslateCmd* cmd = new TranslateCmd (document, xval, yval);
    history->addCommand (cmd, true);
  }
}

void TransformationDialog::scale (bool onDuplicate) {
  float xval, yval;

  xval = horizDim->getValue ();
  yval = vertDim->getValue ();

  if (percent->isChecked ()) {
    // the given values are percentage values
    xval /= 100.0;
    yval /= 100.0;
  }
  else {
    // the values are absolute values, so compute the scaling factors
    Rect r = document->boundingBoxForSelection ();
    xval /= r.width ();
    yval /= r.height ();
  }
  if (onDuplicate) {
    Rect box = document->boundingBoxForSelection ();
    float xoff = box.x (), yoff = box.y ();

    QList<GObject> duplicates;
    duplicates.setAutoDelete (false);

    for (list<GObject*>::iterator it = document->getSelection ().begin ();
	 it != document->getSelection ().end (); it++) {
      GObject* obj = (*it)->copy ();
      QWMatrix m1, m2, m3;

      m1.translate (-xoff, -yoff);
      m2.scale (xval, yval);
      m3.translate (xoff, yoff);

      obj->transform (m1);
      obj->transform (m2);
      obj->transform (m3, true);
      duplicates.append (obj);
    }
    InsertObjCmd* cmd = new InsertObjCmd (document, duplicates);
    history->addCommand (cmd, true);
  }
  else {
    ScaleCmd* cmd = new ScaleCmd (document, Handle_Right | Handle_Bottom,
				 xval, yval);
    history->addCommand (cmd, true);
  }
}

void TransformationDialog::rotate (bool onDuplicate) {
  float xcenter, ycenter, angle;

  xcenter = horizRotCenter->getValue ();
  ycenter = vertRotCenter->getValue ();
  angle = rotAngle->getValue ();

  if (relativeRotCenter->isChecked ()) {
    // the given values are relative to the current bounding box
    Rect r = document->boundingBoxForSelection ();
    xcenter += r.left ();
    ycenter += r.top ();
  }
  if (onDuplicate) {
    QList<GObject> duplicates;
    duplicates.setAutoDelete (false);

    for (list<GObject*>::iterator it = document->getSelection ().begin ();
	 it != document->getSelection ().end (); it++) {
      GObject* obj = (*it)->copy ();
      QWMatrix m1, m2, m3;
      m1.translate (-xcenter, -ycenter);
      m2.rotate (angle);
      m3.translate (xcenter, ycenter);
      obj->transform (m1);
      obj->transform (m2);
      obj->transform (m3, true);
      duplicates.append (obj);
    }
    InsertObjCmd* cmd = new InsertObjCmd (document, duplicates);
    history->addCommand (cmd, true);
  }
  else {
    RotateCmd* cmd = new RotateCmd (document, Coord (xcenter, ycenter),
				    angle);
    history->addCommand (cmd, true);
  }
}

void TransformationDialog::mirror (bool onDuplicate) {
  float sx = 1, sy = 1;

  if (horizMirror->isOn ())
    sx = -1;
  else if (vertMirror->isOn ())
    sy = -1;
  if (onDuplicate) {
    Rect box = document->boundingBoxForSelection ();
    float xoff = box.x (), yoff = box.y ();

    QList<GObject> duplicates;
    duplicates.setAutoDelete (false);

    for (list<GObject*>::iterator it = document->getSelection ().begin ();
	 it != document->getSelection ().end (); it++) {
      GObject* obj = (*it)->copy ();
      QWMatrix m1, m2, m3;

      m1.translate (-xoff, -yoff);
      m2.scale (sx, sy);
      m3.translate (xoff, yoff);

      obj->transform (m1);
      obj->transform (m2);
      obj->transform (m3, true);
      duplicates.append (obj);
    }
    InsertObjCmd* cmd = new InsertObjCmd (document, duplicates);
    history->addCommand (cmd, true);
  }
  else {
    ScaleCmd* cmd = new ScaleCmd (document, Handle_Right | Handle_Bottom,
				  sx, sy);
    history->addCommand (cmd, true);
  }
}

void TransformationDialog::setDocument (GDocument* doc) {
  document = doc;
  update ();
}

void TransformationDialog::showTab (int id) {
  show ();
  tabCtl->showPage (id);
  raise ();
}

void TransformationDialog::update () {
  if (sender () != 0L)
    document = (GDocument *) sender ();

  Rect r = document->boundingBoxForSelection ();

  // position
  horizPosition->setValue (r.left ());
  vertPosition->setValue (r.top ());
  relativePosition->setChecked (false);

  // dimension
  absolute->setChecked (true);
  horizDim->setValue (r.width ());
  vertDim->setValue (r.height ());
  dimRatio = r.width () / r.height ();
  selWidth = r.width ();
  selHeight = r.height ();

  // rotation
  rotAngle->setValue (90.0);
  horizRotCenter->setValue (r.center ().x ());
  vertRotCenter->setValue (r.center ().y ());
  relativeRotCenter->setChecked (false);
  setActiveWindow ();
  raise ();
}

void TransformationDialog::updateProportionalDimension (float /*value*/) {
  if (proportional->isChecked ()) {
    if (sender () == horizDim) {
    debug("updateProportionalDimension--Horizontal");
      if (percent->isChecked ()){
        debug("updateProportionalDimension--Percental");
	vertDim->setValue (horizDim->getValue ());}
      else {
        debug("updateProportionalDimension--Non-Percental");
	float h = horizDim->getValue ();
	vertDim->setValue (h / dimRatio);
      }
    }
    else if (sender () == vertDim) {
      debug("updateProportionalDimension--vertical");
      if (percent->isChecked ()){
      debug("updateProportionalDimension--percental");
	horizDim->setValue (vertDim->getValue ());}
      else {
        debug("updateProportionalDimension--non-percental");
	float v = vertDim->getValue ();
	horizDim->setValue (v * dimRatio);
      }
    }
  }
}

void TransformationDialog::slotAbsScale () {
  horizDim->enableUnits (true);
  horizDim->setValue (selWidth);
  vertDim->enableUnits (true);
  vertDim->setValue (selHeight);
}

void TransformationDialog::slotPercentScale () {
  horizDim->enableUnits (false);
  horizDim->setValue (100.0);
  vertDim->enableUnits (false);
  vertDim->setValue (100.0);
}
