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

#include "PropertyEditor.h"
#include "PropertyEditor.moc"

#include <stdio.h>
#include <assert.h>
#include <klocale.h>
#include <kapp.h>
#include <kbuttonbox.h>
#include <kseparator.h>
#include <kiconloader.h>

#include <qpushbt.h>
#include <qlabel.h>
#include <qlayout.h>

#include "GText.h"
#include "GPolygon.h"
#include "GOval.h"
#include "SetPropertyCmd.h"
#include "Arrow.h"
#include "LineStyle.h"
#include "PStateManager.h"
#include "units.h"

PropertyEditor::PropertyEditor (CommandHistory* history, GDocument* doc, 
				QWidget* parent, const char* name) : 
    QDialog (parent, name, true) {
  QPushButton* button;
  QWidget* widget;

  leftArrows = rightArrows = 0L;
  roundnessSlider = 0L;
  for (int i = 0; i < 3; i++) {
    ellipseKind[i] = 0L;
    textAlign[i] = 0L;
  }

  document = doc;
  haveObjects = ! document->selectionIsEmpty ();
  haveTextObjects = false;
  haveLineObjects = false;
  haveEllipseObjects = false;
  haveRectangleObjects = false;

  text = "Text";

  if (haveObjects) {
    QListIterator<GObject> it (document->getSelection ());
    for (; it.current (); ++it) {
      if (it.current ()->isA ("GText")) {
	haveTextObjects = true;
	GText* tobj = (GText *) it.current ();
	text = tobj->line (0);
      }
      else if (it.current ()->isA ("GPolyline") ||
	       it.current ()->isA ("GBezier"))
	haveLineObjects = true;
      else if (it.current ()->isA ("GOval"))
	haveEllipseObjects = true;
      else if (it.current ()->isA ("GPolygon")) {
	GPolygon* polygon = (GPolygon *) it.current ();
	if (polygon->isRectangle ())
	  haveRectangleObjects = true;
      }
    }
  }
  cmdHistory = history;

  setCaption (i18n ("Properties"));

  QVBoxLayout *vl = new QVBoxLayout (this, 2);

  // the tab control 
  tabctl = new KTabCtl (this);

  widget = createInfoWidget (tabctl);
  tabctl->addTab (widget, i18n ("Info"));

  widget = createOutlineWidget (tabctl);
  tabctl->addTab (widget, i18n ("Outline"));

  widget = createFillWidget (tabctl);
  tabctl->addTab (widget, i18n ("Fill"));

  if (! haveObjects || haveTextObjects) {
    widget = createFontWidget (tabctl);
    tabctl->addTab (widget, i18n ("Font"));
  }

  vl->addWidget (tabctl, 1);

  // a separator
  KSeparator* sep = new KSeparator (this);
  vl->addWidget (sep);

  // the standard buttons
  KButtonBox *bbox = new KButtonBox (this);
  button = bbox->addButton (i18n ("Ok"));
  connect (button, SIGNAL (clicked ()), SLOT (applyPressed ()));
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
 
  setMinimumSize (430, 400);
  setMaximumSize (430, 400);

  readProperties ();
}

QWidget* PropertyEditor::createInfoWidget (QWidget* parent) {
  QWidget* w;
  QLabel* label;

  w = new QWidget (parent);

  label = new QLabel (w);
  label->setAlignment (AlignLeft | AlignVCenter);
  label->setText (i18n ("Type:"));
  label->setFixedHeight (label->sizeHint ().height ());
  label->move (10, 20);

  infoLabel[0] = new QLabel (w);
  infoLabel[0]->setAlignment (AlignLeft | AlignVCenter);
  infoLabel[0]->setText ("");
  infoLabel[0]->setFixedHeight (label->sizeHint ().height ());
  infoLabel[0]->move (150, 20);

  label = new QLabel (w);
  label->setAlignment (AlignLeft | AlignVCenter);
  label->setText (i18n ("Bounding Box"));
  label->setFixedHeight (label->sizeHint ().height ());
  label->move (10, 40);

  label = new QLabel (w);
  label->setAlignment (AlignRight | AlignVCenter);
  label->setText (i18n ("X Position:"));
  label->setFixedHeight (label->sizeHint ().height ());
  label->move (30, 60);

  infoLabel[1] = new QLabel (w);
  infoLabel[1]->setAlignment (AlignRight | AlignVCenter);
  infoLabel[1]->setText ("");
  infoLabel[1]->setFixedHeight (label->sizeHint ().height ());
  infoLabel[1]->move (150, 60);

  label = new QLabel (w);
  label->setAlignment (AlignRight | AlignVCenter);
  label->setText (i18n ("Y Position:"));
  label->setFixedHeight (label->sizeHint ().height ());
  label->move (30, 75);

  infoLabel[2] = new QLabel (w);
  infoLabel[2]->setAlignment (AlignRight | AlignVCenter);
  infoLabel[2]->setFixedHeight (label->sizeHint ().height ());
  infoLabel[2]->setText ("");
  infoLabel[2]->move (150, 75);

  label = new QLabel (w);
  label->setAlignment (AlignRight | AlignVCenter);
  label->setText (i18n ("Width:"));
  label->setFixedHeight (label->sizeHint ().height ());
  label->move (30, 90);

  infoLabel[3] = new QLabel (w);
  infoLabel[3]->setAlignment (AlignRight | AlignVCenter);
  infoLabel[3]->setText ("");
  infoLabel[3]->setFixedHeight (label->sizeHint ().height ());
  infoLabel[3]->move (150, 90);

  label = new QLabel (w);
  label->setAlignment (AlignRight | AlignVCenter);
  label->setText (i18n ("Height:"));
  label->setFixedHeight (label->sizeHint ().height ());
  label->move (30, 105);

  infoLabel[4] = new QLabel (w);
  infoLabel[4]->setAlignment (AlignRight | AlignVCenter);
  infoLabel[4]->setText ("");
  infoLabel[4]->setFixedHeight (label->sizeHint ().height ());
  infoLabel[4]->move (150, 105);

  return w;
}

QWidget* PropertyEditor::createOutlineWidget (QWidget* parent) {
  QWidget* w;
  QLabel* label;

  w = new QWidget (parent);

  label = new QLabel (w);
  label->setAlignment (AlignLeft | AlignVCenter);
  label->setText (i18n ("Width:"));
//  label->setFixedHeight (label->sizeHint ().height ());
  label->move (10, 20);
  
  widthField = new FloatSpinBox (w);
  widthField->setFormatString ("%-3.3f");
  widthField->setEditable (true);
  widthField->setRange (0, 20);
  widthField->move (80, 20);

  label = new QLabel (w);
  label->setAlignment (AlignLeft | AlignVCenter);
  label->setText (i18n ("Color:"));
//  label->setFixedHeight (label->sizeHint ().height ());
  label->move (10, 60);
  
  penColorField = new ColorComboBox (w);
  penColorField->setColor (white);
  penColorField->move (80, 60);

  label = new QLabel (w);
  label->setAlignment (AlignLeft | AlignVCenter);
  label->setText (i18n ("Style:"));
//  label->setFixedHeight (label->sizeHint ().height ());
  label->move (10, 100);
  
  penStyleField = new KCombo (w);
  penStyleField->setGeometry (80, 100, 100, 30);
  QIntDictIterator<LineStyle> lit = LineStyle::getLineStyles ();
  for (; lit.current (); ++lit) {
    LineStyle* style = lit.current ();
    penStyleField->insertItem (style->pixmap ());
  }
  penStyleField->setCurrentItem (1);

  if (haveLineObjects || !haveObjects) {
    label = new QLabel (w);
    label->setAlignment (AlignLeft | AlignVCenter);
    label->setText (i18n ("Arrows:"));
    label->setGeometry (10, 140, 60, 30);
    leftArrows = new KCombo (w);
    leftArrows->setGeometry (80, 140, 60, 30);
    rightArrows = new KCombo (w);
    rightArrows->setGeometry (150, 140, 60, 30);
    QIntDictIterator<Arrow> iter = Arrow::getArrows ();
    QPixmap empty (50, 20);
    empty.fill (white);
    leftArrows->insertItem (empty);
    rightArrows->insertItem (empty);
    for (; iter.current (); ++iter) {
      Arrow* arrow = iter.current ();
      leftArrows->insertItem (arrow->leftPixmap ());
      rightArrows->insertItem (arrow->rightPixmap ());
    }
  }
  else if (haveEllipseObjects) {
#define BUTTON_WIDTH  40
#define BUTTON_HEIGHT 40
    label = new QLabel (w);
    label->setAlignment (AlignLeft | AlignVCenter);
    label->setText (i18n ("Shape:"));
    label->move (10, 140);

    QButtonGroup *group = new QButtonGroup (w);
    group->move (80, 140);

    KIconLoader* loader = kapp->getIconLoader ();

    ellipseKind[0] = new QPushButton (group);
    ellipseKind[0]->setToggleButton (true);
    ellipseKind[0]->setPixmap (loader->loadIcon ("ellipse1.xpm"));
    ellipseKind[0]->setGeometry (0, 0, 
				 BUTTON_WIDTH, BUTTON_HEIGHT);

    ellipseKind[1] = new QPushButton (group);
    ellipseKind[1]->setToggleButton (true);
    ellipseKind[1]->setPixmap (loader->loadIcon ("ellipse2.xpm"));
    ellipseKind[1]->setGeometry (1 * BUTTON_WIDTH, 0,
				 BUTTON_WIDTH, BUTTON_HEIGHT);

    ellipseKind[2] = new QPushButton (group);
    ellipseKind[2]->setToggleButton (true);
    ellipseKind[2]->setPixmap (loader->loadIcon ("ellipse3.xpm"));
    ellipseKind[2]->setGeometry (2 * BUTTON_WIDTH, 0, 
				 BUTTON_WIDTH, BUTTON_HEIGHT);
    group->adjustSize ();
    group->setExclusive (true);
  }
  else if (haveRectangleObjects) {
    label = new QLabel (w);
    label->setAlignment (AlignLeft | AlignVCenter);
    label->setText (i18n ("Roundness:"));
    label->move (10, 140);

    roundnessSlider = new KSlider (KSlider::Horizontal, w);
    roundnessSlider->setRange (0, 100);
    roundnessSlider->setSteps (10, 50);
    roundnessSlider->move (80, 140);
  }
  else if (haveTextObjects) {
    label = new QLabel (w);
    label->setAlignment (AlignLeft | AlignVCenter);
    label->setText (i18n ("Alignment:"));
    label->move (10, 140);

    QButtonGroup *group = new QButtonGroup (w);
    group->move (80, 140);

    KIconLoader* loader = kapp->getIconLoader ();

    textAlign[0] = new QPushButton (group);
    textAlign[0]->setToggleButton (true);
    textAlign[0]->setPixmap (loader->loadIcon ("tleftalign.xpm"));
    textAlign[0]->setGeometry (0, 0, 
				 BUTTON_WIDTH, BUTTON_HEIGHT);

    textAlign[1] = new QPushButton (group);
    textAlign[1]->setToggleButton (true);
    textAlign[1]->setPixmap (loader->loadIcon ("tcenteralign.xpm"));
    textAlign[1]->setGeometry (1 * BUTTON_WIDTH, 0,
				 BUTTON_WIDTH, BUTTON_HEIGHT);

    textAlign[2] = new QPushButton (group);
    textAlign[2]->setToggleButton (true);
    textAlign[2]->setPixmap (loader->loadIcon ("trightalign.xpm"));
    textAlign[2]->setGeometry (2 * BUTTON_WIDTH, 0, 
				 BUTTON_WIDTH, BUTTON_HEIGHT);
    group->adjustSize ();
    group->setExclusive (true);
  }
  w->adjustSize ();
  return w;
}

QWidget* PropertyEditor::createFillWidget (QWidget* parent) {
  QWidget* w;
  QLabel* label;

  w = new QWidget (parent);

  label = new QLabel (w);
  label->setAlignment (AlignLeft | AlignVCenter);
  label->setText (i18n ("Color:"));
  label->move (10, 20);
  
  fillColorField = new ColorComboBox (w);
  fillColorField->setColor (white);
  fillColorField->move (80, 20);

  w->adjustSize ();
  return w;
}

QWidget* PropertyEditor::createFontWidget (QWidget* parent) {
  QWidget* w;

  w = new QWidget (parent);
  fontSelector = new FontSelector (w, 0L, text);
  fontSelector->move (10, 20);
  w->adjustSize ();
  return w;
}

void PropertyEditor::applyPressed () {
    // Outline
    GObject::OutlineInfo oinfo;

    oinfo.width = widthField->getValue ();
    oinfo.color = penColorField->getColor ();
    oinfo.style = (PenStyle) penStyleField->currentItem ();
    if (leftArrows != 0L && rightArrows != 0L) {
      oinfo.startArrowId = leftArrows->currentItem ();
      oinfo.endArrowId = rightArrows->currentItem ();
    }
    else if (ellipseKind[0] != 0L) {
      if (ellipseKind[1]->isOn ())
	oinfo.shape = GObject::OutlineInfo::ArcShape;
      else if (ellipseKind[2]->isOn ())
	oinfo.shape = GObject::OutlineInfo::PieShape;
      else
	oinfo.shape = GObject::OutlineInfo::DefaultShape;
    }
    else if (roundnessSlider != 0L) {
      oinfo.roundness = roundnessSlider->value ();
    }
    oinfo.mask = GObject::OutlineInfo::Color | GObject::OutlineInfo::Style |
                GObject::OutlineInfo::Width | GObject::OutlineInfo::Custom;

    // Fill
    GObject::FillInfo finfo;

    finfo.color = fillColorField->getColor ();
    finfo.style = SolidPattern;
    finfo.mask = GObject::FillInfo::Color | GObject::FillInfo::Style;

    SetPropertyCmd* cmd = 0L;

    // Font
    if (haveObjects) {
      if (haveTextObjects) {
	GText::TextInfo tinfo;
	tinfo.mask = GText::TextInfo::Font | GText::TextInfo::Align;
	tinfo.font = fontSelector->font ();
	if (textAlign[0]->isOn ())
	  tinfo.align = GText::TextInfo::AlignLeft;
	else if (textAlign[1]->isOn ())
	  tinfo.align = GText::TextInfo::AlignCenter;
	else if (textAlign[2]->isOn ())
	  tinfo.align = GText::TextInfo::AlignRight;
	cmd = new SetPropertyCmd (document, oinfo, finfo, tinfo);
      }
      else 
	cmd = new SetPropertyCmd (document, oinfo, finfo);

      cmdHistory->addCommand (cmd, true);
    }
    else {
      // set default values
      GText::TextInfo tinfo;
      tinfo.mask = GText::TextInfo::Font;
      tinfo.font = fontSelector->font ();

      GObject::setDefaultOutlineInfo (oinfo);
      GObject::setDefaultFillInfo (finfo);
      GText::setDefaultTextInfo (tinfo);
    }
    accept ();
}

void PropertyEditor::helpPressed () {
}

void PropertyEditor::readProperties () {
    char buf[25];
    PStateManager *psm = PStateManager::instance ();
    MeasurementUnit munit = psm->defaultMeasurementUnit ();
    const char* ustr = unitToString (munit);
    
    if (document->selectionCount () == 1) {
      QListIterator<GObject> it (document->getSelection ());
      GObject* object = it.current ();

      // Info tab
      Rect boundingBox = object->boundingBox ();
      infoLabel[0]->setText (object->typeName ());
      sprintf (buf, "%5.2f %s", cvtPtToUnit (munit, boundingBox.left ()),
	       ustr);
      infoLabel[1]->setText (buf);     
      sprintf (buf, "%5.2f %s", cvtPtToUnit (munit, boundingBox.top ()),
	       ustr);
      infoLabel[2]->setText (buf);     
      sprintf (buf, "%5.2f %s", cvtPtToUnit (munit, boundingBox.width ()),
	       ustr);
      infoLabel[3]->setText (buf);     
      sprintf (buf, "%5.2f %s", cvtPtToUnit (munit, boundingBox.height ()),
	       ustr);
      infoLabel[4]->setText (buf);     
    
      // Outline tab
      GObject::OutlineInfo oInfo = object->getOutlineInfo ();
      widthField->setValue (oInfo.width);
      penColorField->setColor (oInfo.color);
      penStyleField->setCurrentItem (oInfo.style);
      if (object->isA ("GPolyline")) {
	leftArrows->setCurrentItem (oInfo.startArrowId);
	rightArrows->setCurrentItem (oInfo.endArrowId);
      }
      else if (object->isA ("GPolygon")) {
	GPolygon* polygon = (GPolygon *) object;
	if (polygon->isRectangle ())
	  roundnessSlider->setValue (oInfo.roundness);
      }
      else if (object->isA ("GOval")) {
	switch (oInfo.shape) {
	case GObject::OutlineInfo::ArcShape:
	  ellipseKind[1]->setOn (true);
	  break;
	case GObject::OutlineInfo::PieShape:
	  ellipseKind[2]->setOn (true);
	  break;
	default:
	  ellipseKind[0]->setOn (true);
	  break;
	}
      }
      
      // Fill tab
      fillColorField->setColor (object->getFillColor ());
      
      // Font tab
      if (object->isA ("GText")) {
	GText* tobj = (GText *) object;
	GText::TextInfo tInfo = tobj->getTextInfo ();
	fontSelector->setFont (tInfo.font);
	switch (tInfo.align) {
	case GText::TextInfo::AlignCenter:
	  textAlign[1]->setOn (true);
	  break;
	case GText::TextInfo::AlignRight:
	  textAlign[2]->setOn (true);
	  break;
	default:
	  textAlign[0]->setOn (true);
	  break;
	}
      }
    }
    else {
      // more objects ar no objects - use default values
      // Info tab
      Rect boundingBox = document->boundingBoxForSelection ();
      if (! haveObjects)
	infoLabel[0]->setText (i18n ("no selection"));
      else
	infoLabel[0]->setText (i18n ("multiple selection"));
      sprintf (buf, "%5.2f %s", cvtPtToUnit (munit, boundingBox.left ()),
	       ustr);
      infoLabel[1]->setText (buf);     
      sprintf (buf, "%5.2f %s", cvtPtToUnit (munit, boundingBox.top ()),
	       ustr);
      infoLabel[2]->setText (buf);     
      sprintf (buf, "%5.2f %s", cvtPtToUnit (munit, boundingBox.width ()),
	       ustr);
      infoLabel[3]->setText (buf);     
      sprintf (buf, "%5.2f %s", cvtPtToUnit (munit, boundingBox.height ()),
	       ustr);
      infoLabel[4]->setText (buf);     
    
      // Outline tab
      GObject::OutlineInfo oInfo = GObject::getDefaultOutlineInfo ();
      widthField->setValue (oInfo.width);
      penColorField->setColor (oInfo.color);

      // Fill tab
      GObject::FillInfo fInfo = GObject::getDefaultFillInfo ();
      fillColorField->setColor (fInfo.color);
      
      // Font tab
      if (!haveObjects || haveTextObjects) {
	GText::TextInfo tInfo = GText::getDefaultTextInfo ();
	fontSelector->setFont (tInfo.font);
      }
    }
}

int PropertyEditor::edit (CommandHistory* history, GDocument* doc) {
  PropertyEditor dialog (history, doc, 0L, "Properties");
  
  return dialog.exec ();
}
  
