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

#include <PropertyEditor.h>

#include <assert.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kcolorbtn.h>
#include <kfontdialog.h>

#include <qlabel.h>
#include <qlayout.h>
#include <qwidgetstack.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qhbuttongroup.h>
#include <qvbuttongroup.h>
#include <qvgroupbox.h>
#include <qradiobutton.h>
#include <qslider.h>
#include <qlayout.h>
#include <qhbox.h>
#include <qspinbox.h>

#include "GPage.h"
#include <GText.h>
#include <GPolygon.h>
#include <GOval.h>
#include <SetPropertyCmd.h>
#include <Arrow.h>
#include <LineStyle.h>
#include <PStateManager.h>
#include <units.h>
#include <GDocument.h>
#include <GObject.h>
#include <CommandHistory.h>
#include <Gradient.h>
#include <BrushCells.h>
#include <UnitBox.h>

#include <iostream.h>

#define SOLID_BOX    0
#define PATTERN_BOX  1
#define GRADIENT_BOX 2
#define NOFILL_BOX   3

PropertyEditor::PropertyEditor (CommandHistory* history, GDocument* doc,
                                QWidget* parent, const char* name)
:KDialogBase(KDialogBase::Tabbed, i18n("Properties"),
             KDialogBase::Ok | KDialogBase::Cancel, KDialogBase::Ok,parent, name)
,document(doc)
,isGlobal(false)
,haveTextObjects(false)
,haveLineObjects(false)
,haveEllipseObjects(false)
,haveRectangleObjects(false)
,haveFillObjects(false)
,haveArrows(false)
,leftArrows(0)
,rightArrows(0)
,roundnessSlider(0)
,gradient(0)
{
   for (int i = 0; i < 3; i++)
   {
      ellipseKind[i] = 0L;
      textAlign[i] = 0L;
   }

   isGlobal = document->activePage()->selectionIsEmpty ();
   if (isGlobal)
   {
      haveTextObjects = true;
      haveLineObjects = true;
      haveArrows=true;
      haveFillObjects=true;
      //TODO
      //haveEllipseObjects = true;
      //haveRectangleObjects = true;
   }
   else
   {
      for (QListIterator<GObject> it(document->activePage()->getSelection()); it.current(); ++it)
      {
         GObject* o = *it;
         if (o->inherits ("GText"))
            haveTextObjects = true;
         if (o->inherits ("GPolyline") || o->inherits ("GOval"))
         {
            haveLineObjects = true;
            haveFillObjects=true;
         };
         if ((o->isA("GBezier")) || (o->isA("GPolyline")))
         {
            haveArrows=true;
         };
         if (o->inherits("GOval"))
            haveEllipseObjects = true;
         if (o->inherits("GPolygon"))
         {
            GPolygon* polygon = (GPolygon *) o;
            if (polygon->isRectangle ())
               haveRectangleObjects = true;
         }
      }
   };

   cmdHistory = history;

   createInfoWidget(addPage(i18n("Info")));
   cout<<"afetr createInfoWidget()"<<endl;
   if (haveLineObjects)
      createOutlineWidget(addPage(i18n("Outline")));
   cout<<"after createOutlineWidget()"<<endl;
   if (haveFillObjects)
      createFillWidget(addPage(i18n("Fill")));
   cout<<"after createFillWidget()"<<endl;

   if (haveTextObjects)
      createFontWidget(addPage(i18n("Font")));
   cout<<"after createFontWidget()"<<endl;

   readProperties ();
   cout<<"after readProps()"<<endl;
}

void PropertyEditor::createInfoWidget(QWidget* parent)
{

    QGridLayout *layout=new QGridLayout(parent, 7, 4, KDialogBase::marginHint(), KDialogBase::spacingHint());

    QLabel *label = new QLabel(i18n("Type:"), parent);
    layout->addWidget(label, 0, 0);

    infoLabel[0] = new QLabel(parent);
    layout->addWidget(infoLabel[0], 0, 2);

    label = new QLabel(i18n("Bounding Box"), parent);
    layout->addMultiCellWidget(label, 1, 1, 0, 2);

    label = new QLabel(i18n("X Position:"), parent);
    layout->addWidget(label, 2, 1);

    infoLabel[1] = new QLabel(parent);
    layout->addWidget(infoLabel[1], 2, 2);

    label = new QLabel(i18n("Y Position:"), parent);
    layout->addWidget(label, 3, 1);

    infoLabel[2] = new QLabel(parent);
    layout->addWidget(infoLabel[2], 3, 2);

    label = new QLabel(i18n("Width:"), parent);
    layout->addWidget(label, 4, 1);

    infoLabel[3] = new QLabel(parent);
    layout->addWidget(infoLabel[3], 4, 2);

    label = new QLabel(i18n("Height:"), parent);
    layout->addWidget(label, 5, 1);

    infoLabel[4] = new QLabel(parent);
    layout->addWidget(infoLabel[4], 5, 2);

    layout->setRowStretch(6, 1);
    layout->setColStretch(4, 1);
}

void PropertyEditor::createOutlineWidget (QWidget* parent)
{
   QGridLayout *layout=new QGridLayout(parent, 7, 2, KDialogBase::marginHint(), KDialogBase::spacingHint());
   int gridY(0);

   QLabel *label = new QLabel(i18n("Width:"), parent);
   layout->addWidget(label, gridY, 0);

   widthField = new UnitBox (parent);
   widthField->setRange (0.0, 20.0);
   widthField->setStep (0.1);
   widthField->setEditable (true);
   layout->addWidget(widthField, gridY, 1);
   gridY++;

   label = new QLabel(i18n("Color:"), parent);
   layout->addWidget(label, gridY, 0);

   penColorBttn = new KColorButton(parent);
   penColorBttn->setColor (Qt::white);
   layout->addWidget(penColorBttn, gridY, 1);
   gridY++;

   label = new QLabel(i18n("Style:"), parent);
   layout->addWidget(label, gridY, 0);

   penStyleField = new QComboBox(parent);
   layout->addWidget(penStyleField, gridY, 1);
   QIntDictIterator<LineStyle> lit = LineStyle::getLineStyles ();
   for (; lit.current (); ++lit)
   {
      LineStyle* style = lit.current ();
      penStyleField->insertItem (style->pixmap ());
   }
   penStyleField->setCurrentItem (1);
   gridY++;

   if (haveArrows)
   {
      label = new QLabel(i18n("Arrows:"), parent);
      QHBox *hbox=new QHBox(parent);
      leftArrows = new QComboBox (hbox);
      rightArrows = new QComboBox (hbox);
      QIntDictIterator<Arrow> iter = Arrow::getArrows ();
      QPixmap empty (50, 20);
      empty.fill (white);
      leftArrows->insertItem (empty);
      rightArrows->insertItem (empty);
      for (; iter.current (); ++iter)
      {
         Arrow* arrow = iter.current ();
         leftArrows->insertItem (arrow->leftPixmap ());
         rightArrows->insertItem (arrow->rightPixmap ());
      }
      layout->addWidget(label, gridY, 0);
      layout->addWidget(hbox, gridY, 1);
      gridY++;
   };

   if (haveEllipseObjects)
   {
      label = new QLabel(i18n("Shape:"), parent);
      layout->addWidget(label, gridY, 0);

      QButtonGroup *group = new QHButtonGroup(parent);
      group->setFrameStyle(QFrame::NoFrame);
      group->setExclusive (true);
      layout->addWidget(group, gridY, 1);

      ellipseKind[0] = new QPushButton(group);
      ellipseKind[0]->setToggleButton(true);
      ellipseKind[0]->setPixmap (SmallIcon ("ellipse1"));

      ellipseKind[1] = new QPushButton (group);
      ellipseKind[1]->setToggleButton (true);
      ellipseKind[1]->setPixmap (SmallIcon ("ellipse2"));

      ellipseKind[2] = new QPushButton (group);
      ellipseKind[2]->setToggleButton (true);
      ellipseKind[2]->setPixmap (SmallIcon ("ellipse3"));
      gridY++;
   };
   if (haveRectangleObjects)
   {
      label = new QLabel(i18n("Roundness:"), parent);
      layout->addWidget(label, gridY, 0);

      roundnessSlider = new QSlider(QSlider::Horizontal, parent);
      roundnessSlider->setRange (0, 100);
      roundnessSlider->setSteps (10, 50);
      layout->addWidget(roundnessSlider, gridY, 1);
      gridY++;
   };
   layout->setRowStretch(6, 1);
   layout->setColStretch(3, 1);
}

void PropertyEditor::createFillWidget (QWidget* parent)
{
   QBoxLayout *layout=new QHBoxLayout(parent, KDialogBase::marginHint(), KDialogBase::spacingHint());
   QBoxLayout *left=new QVBoxLayout(layout);

   QButtonGroup* group = new QVButtonGroup(parent);
   group->setFrameStyle (QFrame::NoFrame);
   group->setExclusive (true);
   left->addWidget(group);

   QStringList msg;
   msg.append(i18n("Solid"));
   msg.append(i18n("Pattern"));
   msg.append(i18n("Gradient"));
   msg.append(i18n("No Fill"));
   for (int i = 0; i < 4; i++)
   {
      fillStyleBttn[i] = new QRadioButton(msg[i], group);
      connect (fillStyleBttn[i], SIGNAL(clicked()),
               this, SLOT(fillStyleChanged()));
   }
   left->addStretch(1);
   layout->addSpacing(KDialogBase::spacingHint());

   wstack = new QWidgetStack(parent);
   layout->addWidget(wstack, 1);

   // ------ Solid Fill ------
   QGroupBox *groupbox = new QVGroupBox(i18n("Solid Fill"), wstack);
   wstack->addWidget(groupbox, SOLID_BOX);
   QHBox *hbox=new QHBox(groupbox);
   QLabel *label=new QLabel(i18n("Color:"), hbox);
   fillSolidColor=new KColorButton(hbox);
   fillSolidColor->setColor(Qt::white);

    // ------ Pattern Fill ------
    groupbox = new QVGroupBox(i18n("Pattern Fill"), wstack);
    wstack->addWidget (groupbox, PATTERN_BOX);
    hbox=new QHBox(groupbox);
    label = new QLabel(i18n("Color:"), hbox);
    fillPatternColor=new KColorButton(hbox);
    fillPatternColor->setColor(Qt::white);
    connect(fillPatternColor, SIGNAL(changed(const QColor &)),
            this, SLOT(fillPatternColorChanged(const QColor &)));
    brushCells = new BrushCells(groupbox);

    // ------ No Fill ------
    groupbox = new QVGroupBox(i18n("No Fill"), wstack);
    wstack->addWidget (groupbox, NOFILL_BOX);

    // ------ Gradient Fill ------
    groupbox = new QGroupBox(i18n("Gradient Fill"), wstack);
    wstack->addWidget(groupbox, GRADIENT_BOX);
    QVBoxLayout *vbl=new QVBoxLayout(groupbox, KDialogBase::marginHint(), KDialogBase::spacingHint());
    vbl->addSpacing(wstack->fontMetrics().height()/2);
    QGridLayout *gradlayout=new QGridLayout(vbl, 5, 2);
    label = new QLabel(i18n("Color:"), groupbox);
    gradlayout->addWidget(label, 0, 0);
    fillColorBtn1=new KColorButton(groupbox);
    fillColorBtn1->setColor(Qt::white);
    connect (fillColorBtn1, SIGNAL(changed(const QColor&)),
             this, SLOT(gradientColorChanged(const QColor&)));
    gradlayout->addWidget(fillColorBtn1, 0, 1);
    label = new QLabel(i18n("Color:"), groupbox);
    gradlayout->addWidget(label, 1, 0);
    fillColorBtn2 = new KColorButton(groupbox);
    fillColorBtn2->setColor(Qt::white);
    connect (fillColorBtn2, SIGNAL(changed(const QColor&)),
             this, SLOT(gradientColorChanged(const QColor&)));
    gradlayout->addWidget(fillColorBtn2, 1, 1);

    label = new QLabel(i18n("Style:"), groupbox);
    gradlayout->addWidget(label, 2, 0);
    gradStyleCombo = new QComboBox(groupbox);
    gradlayout->addWidget(gradStyleCombo, 2, 1);
    gradStyleCombo->insertItem (i18n("Linear"));
    gradStyleCombo->insertItem (i18n("Radial"));
    gradStyleCombo->insertItem (i18n("Rectangular"));
    connect (gradStyleCombo, SIGNAL(activated(int)),
             this, SLOT(gradientStyleChanged(int)));
    label = new QLabel(i18n("Angle:"), groupbox);
    gradlayout->addWidget(label, 3, 0);
    gradientAngle = new QSpinBox(0,359,1, groupbox);
    gradientAngle->setSuffix(i18n("°"));
    gradlayout->addWidget(gradientAngle, 3, 1);
    connect (gradientAngle, SIGNAL(valueChanged(int)),
             this, SLOT(gradientAngleChanged(int)));
    QGroupBox *box = new QVGroupBox(groupbox);
    gradlayout->addMultiCellWidget(box, 4, 4, 0, 1);
    gradPreview = new PWidget(box);

    fillStyleBttn[0]->setChecked (true);
    wstack->raiseWidget (SOLID_BOX);
}

void PropertyEditor::createFontWidget (QWidget* parent)
{
   QVBoxLayout *layout=new QVBoxLayout(parent, KDialogBase::marginHint(), KDialogBase::spacingHint());
   fontChooser = new KFontChooser(parent);
   layout->addWidget(fontChooser);

   QButtonGroup *group = new QHButtonGroup(parent);
   group->setFrameStyle(QFrame::NoFrame);
   layout->addWidget(group);

   QLabel *label = new QLabel(i18n("Color:"), group);
   penColorBttn = new KColorButton(group);
   penColorBttn->setColor (Qt::white);

   group = new QHButtonGroup(parent);
   group->setFrameStyle(QFrame::NoFrame);
   group->setExclusive (true);
   layout->addWidget(group);

   label = new QLabel(i18n("Alignment:"), group);

   textAlign[0] = new QPushButton (group);
   textAlign[0]->setToggleButton (true);
   textAlign[0]->setPixmap (SmallIcon ("tleftalign"));

   textAlign[1] = new QPushButton (group);
   textAlign[1]->setToggleButton (true);
   textAlign[1]->setPixmap (SmallIcon ("tcenteralign"));

   textAlign[2] = new QPushButton (group);
   textAlign[2]->setToggleButton (true);
   textAlign[2]->setPixmap (SmallIcon ("trightalign"));
}

void PropertyEditor::applyPressed ()
{
   // Outline
   GObject::OutlineInfo oinfo;
   GObject::FillInfo finfo;
   GText::TextInfo tinfo;
   if (haveLineObjects)
   {

      oinfo.width = widthField->getValue ();
      oinfo.color = penColorBttn->color ();
      oinfo.style = (Qt::PenStyle) penStyleField->currentItem ();
      if (leftArrows != 0L && rightArrows != 0L)
      {
         oinfo.startArrowId = leftArrows->currentItem ();
         oinfo.endArrowId = rightArrows->currentItem ();
      };
      if (haveEllipseObjects)
      {
         if (ellipseKind[1]->isOn ())
            oinfo.shape = GObject::OutlineInfo::ArcShape;
         else if (ellipseKind[2]->isOn ())
            oinfo.shape = GObject::OutlineInfo::PieShape;
         else
            oinfo.shape = GObject::OutlineInfo::DefaultShape;
      };
      if (haveRectangleObjects)
      {
         oinfo.roundness = roundnessSlider->value ();
      }
      oinfo.mask = GObject::OutlineInfo::Color | GObject::OutlineInfo::Style |
         GObject::OutlineInfo::Width | GObject::OutlineInfo::Custom;
   };
   if (haveFillObjects)
   {
      // Fill

      finfo.mask = GObject::FillInfo::FillStyle;
      if (fillStyleBttn[SOLID_BOX]->isChecked ())
      {
         finfo.fstyle = GObject::FillInfo::SolidFill;
         finfo.color = fillSolidColor->color ();
         finfo.mask |= GObject::FillInfo::Color;
      }
      else if (fillStyleBttn[GRADIENT_BOX]->isChecked ())
      {
         finfo.fstyle = GObject::FillInfo::GradientFill;
         finfo.gradient = *gradient;
         finfo.mask |= GObject::FillInfo::GradientInfo;
      }
      else if (fillStyleBttn[NOFILL_BOX]->isChecked ())
         finfo.fstyle = GObject::FillInfo::NoFill;
      else if (fillStyleBttn[PATTERN_BOX]->isChecked ())
      {
         finfo.fstyle = GObject::FillInfo::PatternFill;
         finfo.pattern = brushCells->brushStyle ();
         finfo.color = fillPatternColor->color ();
         finfo.mask |= (GObject::FillInfo::Color | GObject::FillInfo::Pattern);
      }
      else
         finfo.fstyle = GObject::FillInfo::SolidFill;
   };

   // Font
   if ((haveTextObjects)&& (!isGlobal))
   {
      tinfo.mask = GText::TextInfo::Font | GText::TextInfo::Align;
      tinfo.font = fontChooser->font ();
      if (textAlign[0]->isOn ())
         tinfo.align = GText::TextInfo::AlignLeft;
      else if (textAlign[1]->isOn ())
         tinfo.align = GText::TextInfo::AlignCenter;
      else if (textAlign[2]->isOn ())
         tinfo.align = GText::TextInfo::AlignRight;
   }
   if (!isGlobal)
   {
      SetPropertyCmd* cmd = 0L;
      if (haveTextObjects)
         cmd = new SetPropertyCmd (document, oinfo, finfo, tinfo);
      else
         cmd = new SetPropertyCmd (document, oinfo, finfo);

      cmdHistory->addCommand (cmd, true);
   }
   else
   {
      // set default values
      tinfo.mask = GText::TextInfo::Font;
      tinfo.font = fontChooser->font ();

      GObject::setDefaultOutlineInfo (oinfo);
      GObject::setDefaultFillInfo (finfo);
      GText::setDefaultTextInfo (tinfo);
   }
}

void PropertyEditor::readProperties ()
{
   cerr<<"1"<<endl;
   QString buf;
   PStateManager *psm = PStateManager::instance ();
   MeasurementUnit munit = psm->defaultMeasurementUnit ();
   QString ustr=" ";
   ustr+=unitToString (munit);

   if (document->activePage()->selectionCount () == 1)
   {
      cerr<<"2"<<endl;
      GObject* object = document->activePage()->getSelection().first();
      // Info tab
      Rect boundingBox = object->boundingBox ();
      infoLabel[0]->setText (QString (object->typeName ()));
      buf.sprintf ("%5.2f", cvtPtToUnit (munit, boundingBox.left ()));
      buf+=ustr;
      infoLabel[1]->setText (buf);
      buf.sprintf ("%5.2f", cvtPtToUnit (munit, boundingBox.top ()));
      buf+=ustr;
      infoLabel[2]->setText (buf);
      buf.sprintf ("%5.2f", cvtPtToUnit (munit, boundingBox.width ()));
      buf+=ustr;
      infoLabel[3]->setText (buf);
      buf.sprintf ("%5.2f", cvtPtToUnit (munit, boundingBox.height ()));
      buf+=ustr;
      infoLabel[4]->setText (buf);

      GObject::OutlineInfo oInfo = object->getOutlineInfo ();
      if (haveLineObjects)
      {
         cerr<<"3"<<endl;
         // Outline tab
         //GObject::OutlineInfo oInfo = object->getOutlineInfo ();
         widthField->setValue (oInfo.width);
         penColorBttn->setColor (oInfo.color);
         penStyleField->setCurrentItem((int)oInfo.style);
         if (haveArrows)
         {
            leftArrows->setCurrentItem (oInfo.startArrowId);
            rightArrows->setCurrentItem (oInfo.endArrowId);
         };
cerr<<"4"<<endl;
         if (haveRectangleObjects)
         {
            roundnessSlider->setValue (static_cast<int>(oInfo.roundness)); // #### FIXME
         };
cerr<<"5"<<endl;
         if (haveEllipseObjects)
         {
            switch (oInfo.shape)
            {
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
      };
cerr<<"6"<<endl;
      if (haveFillObjects)
      {
cerr<<"7"<<endl;
         // Fill tab
         fillSolidColor->setColor (object->getFillColor ());
         fillPatternColor->setColor (object->getFillColor ());
         fillColorBtn1->setColor(object->getFillColor());
         fillColorBtn2->setColor(object->getFillColor());
         switch (object->getFillStyle ())
         {
         case GObject::FillInfo::NoFill:
            fillStyleBttn[NOFILL_BOX]->setChecked (true);
            wstack->raiseWidget (NOFILL_BOX);
            break;
         case GObject::FillInfo::SolidFill:
            fillStyleBttn[SOLID_BOX]->setChecked (true);
            wstack->raiseWidget (SOLID_BOX);
            break;
         case GObject::FillInfo::GradientFill:
            {
               Gradient g = object->getFillGradient ();
               fillStyleBttn[GRADIENT_BOX]->setChecked (true);
               fillColorBtn1->setColor (g.getColor1 ());
               fillColorBtn2->setColor (g.getColor2 ());
               gradStyleCombo->setCurrentItem ((int) g.getStyle ());
               gradientAngle->setEnabled(((int) g.getStyle ()== 0)?true:false);
               gradientAngle->setValue(g.getAngle());
               updateGradient ();
               wstack->raiseWidget (GRADIENT_BOX);
            }
            break;
         case GObject::FillInfo::PatternFill:
            fillStyleBttn[PATTERN_BOX]->setChecked (true);
            brushCells->setColor( object->getFillColor () );
            brushCells->selectBrush (object->getFillPattern ());
            wstack->raiseWidget (PATTERN_BOX);
            break;
         default:
            break;
         }
      };
cerr<<"8"<<endl;
      if (haveTextObjects)
      {
cerr<<"9"<<endl;
         penColorBttn->setColor (oInfo.color);
         GText* tobj = (GText *) object;
         GText::TextInfo tInfo = tobj->getTextInfo ();
         fontChooser->setFont (tInfo.font);
         switch (tInfo.align)
         {
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
      };
   }
   else
   {
      // more objects or no objects - use default values
      // Info tab
      Rect boundingBox = document->activePage()->boundingBoxForSelection ();
      if (isGlobal)
         infoLabel[0]->setText (i18n ("no selection"));
      else
         infoLabel[0]->setText (i18n ("multiple selection"));
      buf.sprintf ("%5.2f", cvtPtToUnit (munit, boundingBox.left ()));
      buf+=ustr;
      infoLabel[1]->setText (buf);
      buf.sprintf ("%5.2f", cvtPtToUnit (munit, boundingBox.top ()));
      buf+=ustr;
      infoLabel[2]->setText (buf);
      buf.sprintf ("%5.2f", cvtPtToUnit (munit, boundingBox.width ()));
      buf+=ustr;
      infoLabel[3]->setText (buf);
      buf.sprintf ("%5.2f", cvtPtToUnit (munit, boundingBox.height ()));
      buf+=ustr;
      infoLabel[4]->setText (buf);

      if (haveLineObjects)
      {
         // Outline tab
         GObject::OutlineInfo oInfo = GObject::getDefaultOutlineInfo ();
         widthField->setValue (oInfo.width);
         penColorBttn->setColor (oInfo.color);
         penStyleField->setCurrentItem(oInfo.style);
         if(leftArrows)
            leftArrows->setCurrentItem (oInfo.startArrowId);
         if(rightArrows)
            rightArrows->setCurrentItem (oInfo.endArrowId);
      };
      if (haveFillObjects)
      {
         // Fill tab
         GObject::FillInfo fInfo = GObject::getDefaultFillInfo ();
         fillSolidColor->setColor(fInfo.color);
         fillPatternColor->setColor(fInfo.color);
         fillColorBtn1->setColor(fInfo.gradient.getColor1());
         fillColorBtn2->setColor(fInfo.gradient.getColor2());
         switch(fInfo.fstyle) {
         case GObject::FillInfo::NoFill:
            fillStyleBttn[NOFILL_BOX]->setChecked (true);
            wstack->raiseWidget (NOFILL_BOX);
            break;
         case GObject::FillInfo::SolidFill:
            fillStyleBttn[SOLID_BOX]->setChecked (true);
            wstack->raiseWidget (SOLID_BOX);
            break;
         case GObject::FillInfo::GradientFill:
            {
               fillStyleBttn[GRADIENT_BOX]->setChecked (true);
               gradStyleCombo->setCurrentItem ((int)fInfo.gradient.getStyle ());
               gradientAngle->setEnabled(((int)fInfo.gradient.getStyle ()== 0)?true:false);
               gradientAngle->setValue(fInfo.gradient.getAngle());
               updateGradient();
               wstack->raiseWidget (GRADIENT_BOX);
            }
            break;
         case GObject::FillInfo::PatternFill:
            fillStyleBttn[PATTERN_BOX]->setChecked (true);
            brushCells->setColor(fInfo.color);
            brushCells->selectBrush (fInfo.pattern);
            wstack->raiseWidget (PATTERN_BOX);
            break;
         default:
            break;
         }
      };
      // Font tab
      if (haveTextObjects)
      {
         GText::TextInfo tInfo = GText::getDefaultTextInfo ();
         fontChooser->setFont (tInfo.font);
      }
      if(isGlobal)
      {
         if(fillStyleBttn[GRADIENT_BOX]->isChecked())
         {
            fillStyleBttn[NOFILL_BOX]->setChecked(true);
            wstack->raiseWidget(NOFILL_BOX);
         }
         fillStyleBttn[GRADIENT_BOX]->setEnabled(false);
      }
   }
cerr<<"10"<<endl;
}

void PropertyEditor::fillStyleChanged()
{
   for (int i = 0; i < 5; i++)
   {
      if ((QRadioButton *) sender () == fillStyleBttn[i])
      {
            if (i == GRADIENT_BOX)
                updateGradient ();
            else if(i == PATTERN_BOX)
                brushCells->setColor(fillPatternColor->color());
            wstack->raiseWidget (i);
        }
    }
}

void PropertyEditor::fillPatternColorChanged(const QColor& color)
{
    brushCells->setColor(color);
}

void PropertyEditor::gradientColorChanged (const QColor&)
{
    updateGradient ();
}

void PropertyEditor::gradientAngleChanged (int a)
{
    gradient->setAngle(a);
    updateGradient();
}

void PropertyEditor::gradientStyleChanged (int i)
{
    if(i == 0)
        gradientAngle->setEnabled(true);
    else
        gradientAngle->setEnabled(false);
    updateGradient ();
}

void PropertyEditor::updateGradient ()
{
    static Gradient::Style styles[] = {Gradient::Linear, Gradient::Radial,Gradient::Rectangular};

    if (gradient == 0L) {
        gradient = new Gradient (fillColorBtn1->color (),
                                 fillColorBtn2->color (), Gradient::Linear,0);
        gradient->setStyle (styles[gradStyleCombo->currentItem ()]);
    }
    else {
        gradient->setColor1 (fillColorBtn1->color ());
        gradient->setColor2 (fillColorBtn2->color ());
        gradient->setStyle  (styles[gradStyleCombo->currentItem ()]);
    }
    gradPreview->setBackgroundPixmap (gradient->createPixmap (gradPreview->width (),
                                                    gradPreview->height ()));
}

int PropertyEditor::edit (CommandHistory* history, GDocument* doc)
{
   PropertyEditor dialog (history, doc, 0L, "Properties");
   int res=dialog.exec();
   if(res==QDialog::Accepted)
      dialog.applyPressed();
   return res;
}

#include <PropertyEditor.moc>
