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
#include <qwidgetstack.h>

#include "GText.h"
#include "GPolygon.h"
#include "GOval.h"
#include "SetPropertyCmd.h"
#include "Arrow.h"
#include "LineStyle.h"
#include "PStateManager.h"
#include "units.h"

#define SOLID_BOX    0
#define PATTERN_BOX  1
#define GRADIENT_BOX 2
#define NOFILL_BOX   3

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
  gradient = 0L;

  document = doc;
  haveObjects = ! document->selectionIsEmpty ();
  haveTextObjects = false;
  haveLineObjects = false;
  haveEllipseObjects = false;
  haveRectangleObjects = false;

  text = "Text";

  if (haveObjects) {
    for (list<GObject*>::iterator it = document->getSelection ().begin ();
	 it != document->getSelection ().end (); it++) {
      GObject* o = *it;
      if (o->isA ("GText")) {
	haveTextObjects = true;
	GText* tobj = (GText *) o;
	text = tobj->line (0);
      }
      else if (o->isA ("GPolyline") || o->isA ("GBezier"))
	haveLineObjects = true;
      else if (o->isA ("GOval"))
	haveEllipseObjects = true;
      else if (o->isA ("GPolygon")) {
	GPolygon* polygon = (GPolygon *) o;
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
  button = bbox->addButton (i18n ("OK"));
  connect (button, SIGNAL (clicked ()), SLOT (applyPressed ()));
  button = bbox->addButton (i18n ("Cancel"));
  connect (button, SIGNAL (clicked ()), SLOT (reject ()));
  bbox->addStretch (1);
  button = bbox->addButton (i18n ("Help"));
  connect (button, SIGNAL (clicked ()), SLOT (helpPressed ()));
  bbox->layout ();
  bbox->setMinimumSize (bbox->sizeHint () + QSize (20, 20));

  vl->addWidget (bbox);

  vl->activate ();
 
  setMinimumSize (430, 400);
  setMaximumSize (430, 400);
  adjustSize ();

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
  label->move (10, 20);
  
  widthField = new FloatSpinBox (w);
  widthField->setFormatString ("%-3.3f");
  widthField->setEditable (true);
  widthField->setRange (0, 20);
  widthField->move (80, 20);

  label = new QLabel (w);
  label->setAlignment (AlignLeft | AlignVCenter);
  label->setText (i18n ("Color:"));
  label->move (10, 60);
  
  penColorBttn = new KColorButton (w);
  penColorBttn->setColor (white);
  penColorBttn->move (80, 60);

  label = new QLabel (w);
  label->setAlignment (AlignLeft | AlignVCenter);
  label->setText (i18n ("Style:"));
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
    //    leftArrows->setGeometry (80, 140, 60, 30);
    leftArrows->setGeometry (80, 140, 75, 30);
    rightArrows = new KCombo (w);
    //    rightArrows->setGeometry (150, 140, 60, 30);
    rightArrows->setGeometry (165, 140, 75, 30);
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

  QButtonGroup* group = new QButtonGroup (w);
  group->setFrameStyle (QFrame::NoFrame);
  group->setExclusive (true);
  group->move (10, 10);

  const char *msg[] = { "Solid", "Pattern", "Gradient", "No Fill" };
  for (int i = 0; i < 4; i++) {
    fillStyleBttn[i] = new QRadioButton (group);
    fillStyleBttn[i]->setText (msg[i]);
    fillStyleBttn[i]->move (10, i * 30);
    connect (fillStyleBttn[i], SIGNAL(clicked()), 
	     this, SLOT(fillStyleChanged()));
  }
  group->adjustSize ();

  wstack = new QWidgetStack (w);
  wstack->setGeometry (100, 10, 300, 300);

  // ------ Solid Fill ------
  QGroupBox *box = new QGroupBox (wstack);
  box->setGeometry (0, 0, 300, 300);
  box->setTitle (i18n ("Solid Fill"));
  wstack->addWidget (box, SOLID_BOX);
  label = new QLabel (box);

  label->setAlignment (AlignLeft | AlignVCenter);
  label->setText (i18n ("Color:"));
  label->move (10, 15);
  solidColorBttn = new KColorButton (box);
  solidColorBttn->setColor (white);
  solidColorBttn->move (80, 15);

  // ------ Pattern Fill ------
  box = new QGroupBox (group);
  box->setGeometry (0, 0, 300, 300);
  box->setTitle (i18n ("Pattern Fill"));
  wstack->addWidget (box, PATTERN_BOX);
  label = new QLabel (box);
  label->setAlignment (AlignLeft | AlignVCenter);
  label->setText (i18n ("Color:"));
  label->move (10, 15);
  patternColorBttn = new KColorButton (box);
  patternColorBttn->setColor (white);
  patternColorBttn->move (80, 15);
  brushCells = new BrushCells (box);
  brushCells->move (10, 60);

  // ------ No Fill ------
  box = new QGroupBox (group);
  box->setGeometry (0, 0, 300, 300);
  box->setTitle (i18n ("No Fill"));
  wstack->addWidget (box, NOFILL_BOX);

#if 0
  // ------ Tile Fill ------
  box = new QGroupBox (group);
  box->setGeometry (0, 0, 300, 300);
  box->setTitle (i18n ("Tile Fill"));
  wstack->addWidget (box, TILE_BOX);
#endif

  // ------ Gradient Fill ------
  box = new QGroupBox (group);
  box->setGeometry (0, 0, 300, 300);
  box->setTitle (i18n ("Gradient Fill"));
  wstack->addWidget (box, GRADIENT_BOX);
  label = new QLabel (box);
  label->setAlignment (AlignLeft | AlignVCenter);
  label->setText (i18n ("Color:"));
  label->move (10, 15);
  gradColor1Bttn = new KColorButton (box);
  gradColor1Bttn->setColor (white);
  gradColor1Bttn->move (80, 15);
  connect (gradColor1Bttn, SIGNAL(changed(const QColor&)),
	   this, SLOT(gradientColorChanged(const QColor&)));

  label = new QLabel (box);
  label->setAlignment (AlignLeft | AlignVCenter);
  label->setText (i18n ("Color:"));
  label->move (10, 50);
  gradColor2Bttn = new KColorButton (box);
  gradColor2Bttn->setColor (white);
  gradColor2Bttn->move (80, 50);
  connect (gradColor2Bttn, SIGNAL(changed(const QColor&)),
	   this, SLOT(gradientColorChanged(const QColor&)));

  label = new QLabel (box);
  label->setAlignment (AlignLeft | AlignVCenter);
  label->setText (i18n ("Style:"));
  label->move (10, 85);
  gradStyleCombo = new KCombo (box);
  gradStyleCombo->move (80, 85);
  gradStyleCombo->insertItem (i18n ("Horizontal"));
  gradStyleCombo->insertItem (i18n ("Vertical"));
  gradStyleCombo->insertItem (i18n ("Radial"));
  gradStyleCombo->insertItem (i18n ("Rectangular"));
  //  gradStyleCombo->insertItem (i18n ("Diagonal 1"));
  //  gradStyleCombo->insertItem (i18n ("Diagonal 2"));
  connect (gradStyleCombo, SIGNAL(activated(int)), 
	   this, SLOT(gradientStyleChanged(int)));
  QFrame* frame = new QFrame (box);
  frame->setLineWidth (1);
  frame->setFrameStyle (QFrame::Panel | QFrame::Sunken);
  frame->move (10, 130);
  gradPreview = new QLabel (frame);
  gradPreview->move (2, 2);
  gradPreview->resize (170, 150);
  gradPreview->setAutoResize (false);
  frame->resize (174, 154);

  fillStyleBttn[0]->setChecked (true);
  wstack->raiseWidget (SOLID_BOX);

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
    oinfo.color = penColorBttn->color ();
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

    finfo.mask = GObject::FillInfo::FillStyle;
    if (fillStyleBttn[SOLID_BOX]->isChecked ()) {
      finfo.fstyle = GObject::FillInfo::SolidFill;
      finfo.color = solidColorBttn->color ();
      finfo.mask |= GObject::FillInfo::Color;
    }
    else if (fillStyleBttn[GRADIENT_BOX]->isChecked ()) {
      finfo.fstyle = GObject::FillInfo::GradientFill;
      finfo.gradient = *gradient;
      finfo.mask |= GObject::FillInfo::GradientInfo;
    }
    else if (fillStyleBttn[NOFILL_BOX]->isChecked ())
      finfo.fstyle = GObject::FillInfo::NoFill;
    else if (fillStyleBttn[PATTERN_BOX]->isChecked ()) {
      finfo.fstyle = GObject::FillInfo::PatternFill;
      finfo.pattern = brushCells->brushStyle ();
      finfo.color = patternColorBttn->color ();
      finfo.mask |= (GObject::FillInfo::Color | GObject::FillInfo::Pattern);
    }
    else
      finfo.fstyle = GObject::FillInfo::SolidFill;

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
      GObject* object = document->getSelection ().front ();
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
      penColorBttn->setColor (oInfo.color);
      penStyleField->setCurrentItem (oInfo.style);
      if (object->isA ("GPolyline") || object->isA ("GBezier")) {
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
      switch (object->getFillStyle ()) {
      case GObject::FillInfo::NoFill:
	fillStyleBttn[NOFILL_BOX]->setChecked (true);
	wstack->raiseWidget (NOFILL_BOX);
	break;
      case GObject::FillInfo::SolidFill:
	fillStyleBttn[SOLID_BOX]->setChecked (true);
	solidColorBttn->setColor (object->getFillColor ());
	wstack->raiseWidget (SOLID_BOX);
	break;
      case GObject::FillInfo::GradientFill:
	{
	  Gradient g = object->getFillGradient ();
	  fillStyleBttn[GRADIENT_BOX]->setChecked (true);
	  gradColor1Bttn->setColor (g.getColor1 ());
	  gradColor2Bttn->setColor (g.getColor2 ());
	  gradStyleCombo->setCurrentItem ((int) g.getStyle ());
	  updateGradient ();
	  wstack->raiseWidget (GRADIENT_BOX);
	}
	break;
      case GObject::FillInfo::PatternFill:
	fillStyleBttn[PATTERN_BOX]->setChecked (true);
	patternColorBttn->setColor (object->getFillColor ());
	brushCells->selectBrush (object->getFillPattern ());
	wstack->raiseWidget (PATTERN_BOX);
	break;
      default:
	break;
      }
      
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
      penColorBttn->setColor (oInfo.color);

      // Fill tab
      GObject::FillInfo fInfo = GObject::getDefaultFillInfo ();
      solidColorBttn->setColor (fInfo.color);
      
      // Font tab
      if (!haveObjects || haveTextObjects) {
	GText::TextInfo tInfo = GText::getDefaultTextInfo ();
	fontSelector->setFont (tInfo.font);
      }
    }
}

void PropertyEditor::fillStyleChanged () {
  for (int i = 0; i < 5; i++) {
    if ((QRadioButton *) sender () == fillStyleBttn[i]) {
      if (i == GRADIENT_BOX)
	updateGradient ();
      wstack->raiseWidget (i);
      break;
    }
  }
}

void PropertyEditor::gradientColorChanged (const QColor&) {
  updateGradient ();
}

void PropertyEditor::gradientStyleChanged (int) {
  updateGradient ();
}

void PropertyEditor::updateGradient () {
  static Gradient::Style styles[] = { 
    Gradient::Horizontal, Gradient::Vertical, Gradient::Radial, 
    Gradient::Rectangular, Gradient::Diagonal1, Gradient::Diagonal2 
  };

  if (gradient == 0L) {
      gradient = new Gradient (gradColor1Bttn->color (), 
			       gradColor2Bttn->color (), Gradient::Horizontal);
      gradient->setStyle (styles[gradStyleCombo->currentItem ()]);
  }
  else {
    gradient->setColor1 (gradColor1Bttn->color ());
    gradient->setColor2 (gradColor2Bttn->color ());
    gradient->setStyle (styles[gradStyleCombo->currentItem ()]);
  }
  gradPreview->setPixmap (gradient->createPixmap (gradPreview->width (),
						  gradPreview->height ()));

}

int PropertyEditor::edit (CommandHistory* history, GDocument* doc) {
  PropertyEditor dialog (history, doc, 0L, "Properties");
  
  return dialog.exec ();
}
  
