/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998-99 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

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

#include <qlabel.h>
#include <qlayout.h>
#include <qvgroupbox.h>
#include <qhbox.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <kcolorbutton.h>

#include <klocale.h>

#include <UnitBox.h>
#include <PStateManager.h>

#include "OptionDialog.h"
#include "GDocument.h"
#include "GPage.h"

OptionDialog::OptionDialog (GDocument *adoc,QWidget* parent, const char* name) :
    KDialogBase(KDialogBase::TreeList, i18n("Option"),
                Ok|Apply|Cancel, Ok,
                parent, name, true),doc(adoc),modified(false)
{
  QStringList list;
  createGeneralWidget(addPage(i18n("General")));
  createEditWidget(addPage(i18n("Edit")));
  list.clear();
  list << i18n("Document") << i18n("Grid");
  createGridWidget(addPage(list));
  list.clear();
  list << i18n("Document") << i18n("Background");
  createBGWidget(addPage(list));
  list.clear();
  list << i18n("Document") << i18n("Helplines") << i18n("Vertical");
  createVertLineWidget(addPage(list));
  list.clear();
  list << i18n("Document") << i18n("Helplines") << i18n("Horizontal");
  createHorizLineWidget(addPage(list));
  list.clear();

  horizLines = doc->horizHelplines();
  vertLines = doc->vertHelplines();
  initHelplinesLists();
}

void OptionDialog::createGeneralWidget (QWidget* parent)
{

    QGridLayout *layout=new QGridLayout(parent, 2, 2, KDialogBase::marginHint(), KDialogBase::spacingHint());
    QLabel *label = new QLabel(i18n("Unit:"), parent);
    layout->addWidget(label, 0, 0);

    unit = new QComboBox (parent);
    unit->insertItem (unitToString (UnitPoint));
    unit->insertItem (unitToString (UnitMillimeter));
    unit->insertItem (unitToString (UnitInch));
    unit->insertItem (unitToString (UnitPica));
    unit->insertItem (unitToString (UnitCentimeter));
    unit->insertItem (unitToString (UnitDidot));
    unit->insertItem (unitToString (UnitCicero));
    layout->addWidget(unit, 0, 1);
    layout->setRowStretch(1, 1);

    unit->setCurrentItem ((int)
                          PStateManager::instance ()->defaultMeasurementUnit ());
}

void OptionDialog::createEditWidget (QWidget* parent)
{

    QBoxLayout *layout=new QVBoxLayout(parent, KDialogBase::marginHint(), KDialogBase::spacingHint());
    QGroupBox *box = new QGroupBox(i18n("Duplicate Offset"), parent);
    layout->addWidget(box);

    QBoxLayout *vboxlayout=new QVBoxLayout(box, KDialogBase::marginHint(), KDialogBase::spacingHint());
    vboxlayout->addSpacing(box->fontMetrics().height()/2);
    QGridLayout *grid=new QGridLayout(vboxlayout, 2, 2);
    QLabel *label = new QLabel(i18n("Horizontal:"), box);
    grid->addWidget(label, 0, 0);

    horiz = new UnitBox(box);
    horiz->setRange (-1000.0, 1000.0);
    horiz->setStep (0.1);
    horiz->setEditable (true);
    grid->addWidget(horiz, 0, 1);

    label = new QLabel(i18n("Vertical:"), box);
    grid->addWidget(label, 1, 0);

    vert = new UnitBox(box);
    vert->setRange (-1000.0, 1000.0);
    vert->setStep (0.1);
    vert->setEditable (true);
    grid->addWidget(vert, 1, 1);

    box = new QGroupBox(i18n("Step Distance"), parent);
    layout->addWidget(box);

    vboxlayout=new QVBoxLayout(box, KDialogBase::marginHint(), KDialogBase::spacingHint());
    vboxlayout->addSpacing(box->fontMetrics().height()/2);
    grid=new QGridLayout(vboxlayout, 2, 2);
    label = new QLabel(i18n("Small step:"), box);
    grid->addWidget(label, 0, 0);

    smallStep = new UnitBox(box);
    smallStep->setRange (-1000.0, 1000.0);
    smallStep->setStep (0.1);
    smallStep->setEditable (true);
    grid->addWidget(smallStep, 0, 1);

    label = new QLabel(i18n("Big step:"), box);
    grid->addWidget(label, 1, 0);

    bigStep = new UnitBox(box);
    bigStep->setRange (-1000.0, 1000.0);
    bigStep->setStep (0.1);
    bigStep->setEditable (true);
    grid->addWidget(bigStep, 1, 1);

    PStateManager *psm = PStateManager::instance ();
    horiz->setValue (psm->duplicateXOffset ());
    vert->setValue (psm->duplicateYOffset ());
    smallStep->setValue (psm->smallStepSize ());
    bigStep->setValue (psm->bigStepSize ());
}

/*Background*/
void OptionDialog::createBGWidget(QWidget* parent)
{
  QBoxLayout *layout=new QHBoxLayout(parent, KDialogBase::marginHint(), KDialogBase::spacingHint());
  QLabel* clabel = new QLabel(i18n("Background Color"), parent);
  bgbutton = new KColorButton(parent);
  connect (bgbutton, SIGNAL(changed (const QColor&)), this, SLOT(colorChanged(const QColor&)));
  bgbutton->setColor(doc->activePage()->bgColor());
  layout->addWidget(clabel);
  layout->addWidget(bgbutton);
}

void OptionDialog::colorChanged(const QColor&)
{
  modified = true;
}

/*Grid*/

void OptionDialog::createGridWidget (QWidget* parent)
{
  QGridLayout *layout=new QGridLayout(parent, 3, 2, KDialogBase::marginHint(), KDialogBase::spacingHint());

  QGroupBox *box=new QGroupBox(i18n("Distance"), parent);
  layout->addMultiCellWidget(box, 0, 0, 0, 1);

  QBoxLayout *vboxlayout=new QVBoxLayout(box, KDialogBase::marginHint(), KDialogBase::spacingHint());
  vboxlayout->addSpacing(box->fontMetrics().height()/2);
  QGridLayout *grid=new QGridLayout(vboxlayout, 2, 2);
  QLabel* label = new QLabel(i18n("Horizontally"), box);
  grid->addWidget(label, 0, 0);

  hspinbox = new UnitBox(box);
  hspinbox->setFormatString ("%-3.3f");
  hspinbox->setEditable (true);
  hspinbox->setRange (0, 1000);
  grid->addWidget(hspinbox, 0, 1);

  label = new QLabel(i18n("Vertically"), box);
  grid->addWidget(label, 1, 0);

  vspinbox = new UnitBox (box);
  vspinbox->setFormatString ("%-3.3f");
  vspinbox->setEditable (true);
  vspinbox->setRange (0, 1000);
  grid->addWidget(vspinbox, 1, 1);

  hspinbox->setValue(doc->horizGridDistance());
  vspinbox->setValue(doc->vertGridDistance());

  gbutton = new QCheckBox(i18n("Snap To Grid"), parent);
  gbutton->setDown(doc->snapToGrid());
  layout->addWidget(gbutton, 1, 0);

  sbutton = new QCheckBox(i18n("Show Grid"), parent);
  sbutton->setDown(doc->showGrid());
  layout->addWidget(sbutton, 1, 1);

  cbutton = new KColorButton(parent);
  cbutton->setColor(doc->gridColor());
  QLabel* clabel = new QLabel(i18n("Grid Color"), parent);
  layout->addWidget(cbutton, 2, 1);
  layout->addWidget(clabel, 2, 0);
}

/*Helplines*/
void OptionDialog::createHorizLineWidget(QWidget* parent)
{

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
    addHorizHelpLine = new QPushButton (i18n("Add"), parent);
    connect (addHorizHelpLine, SIGNAL(clicked ()), this, SLOT(addHorizLine ()));
    right->addWidget(addHorizHelpLine);

    updateHorizHelpLine = new QPushButton(i18n("Update"), parent);
    connect (updateHorizHelpLine, SIGNAL(clicked ()), this, SLOT(updateHorizLine ()));
    right->addWidget(updateHorizHelpLine);

    delHorizHelpLine = new QPushButton(i18n("Delete"), parent);
    connect (delHorizHelpLine, SIGNAL(clicked ()), this, SLOT(deleteHorizLine ()));
    right->addWidget(delHorizHelpLine);
    right->addStretch();
}

void OptionDialog::createVertLineWidget(QWidget* parent)
{
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
    layout->addSpacing(KDialogBase::spacingHint() * 2);

    QBoxLayout *right=new QVBoxLayout(layout);
    addVertHelpLine = new QPushButton(i18n("Add"), parent);
    connect (addVertHelpLine, SIGNAL(clicked ()), this, SLOT(addVertLine ()));
    right->addWidget(addVertHelpLine);

    updateVertHelpLine = new QPushButton(i18n("Update"), parent);
    connect (updateVertHelpLine, SIGNAL(clicked ()), this, SLOT(updateVertLine ()));
    right->addWidget(updateVertHelpLine);

    delVertHelpLine = new QPushButton(i18n("Delete"), parent);
    connect (delVertHelpLine, SIGNAL(clicked ()), this, SLOT(deleteVertLine ()));
    right->addWidget(delVertHelpLine);
    right->addStretch();
}

void OptionDialog::initHelplinesLists()
{
  QValueList<float>::Iterator i;
  QString buf;
  MeasurementUnit unit = PStateManager::instance ()->defaultMeasurementUnit ();

  for (i = horizLines.begin (); i != horizLines.end (); ++i)
  {
    buf=QString::number(cvtPtToUnit (unit, *i), 'f', 3);
    buf+=" ";
    buf+=unitToString (unit);
    horizList->insertItem (buf);
  }
  if(!horizLines.isEmpty())
    horizValue->setValue(horizLines[0]);
  else
  {
      updateHorizHelpLine->setEnabled(false);
      delHorizHelpLine->setEnabled(false);
  }

  for (i = vertLines.begin (); i != vertLines.end (); ++i)
  {
    buf=QString::number(cvtPtToUnit (unit, *i), 'f', 3);
    buf+=" ";
    buf+=unitToString (unit);
    vertList->insertItem (buf);
  }
  if(!vertLines.isEmpty())
    vertValue->setValue(vertLines[0]);
  else
  {
      updateVertHelpLine->setEnabled(false);
      delVertHelpLine->setEnabled(false);
  }
}

void OptionDialog::addHorizLine()
{
  float value = horizValue->getValue ();
  horizLines.append(value);
  MeasurementUnit unit = PStateManager::instance ()->defaultMeasurementUnit ();
  QString buf=QString::number(cvtPtToUnit (unit, value), 'f', 3);
  buf+=" ";
  buf+=unitToString (unit);
  horizList->insertItem (buf);
  updateHorizHelpLine->setEnabled(true);
  delHorizHelpLine->setEnabled(true);
  modified = true;
}

void OptionDialog::updateHorizLine()
{
  if(horizLines.isEmpty())
    return;
  int idx = horizList->currentItem ();
  if (idx != -1)
  {
    float value = horizValue->getValue ();
    MeasurementUnit unit = PStateManager::instance ()->defaultMeasurementUnit ();
    QString buf=QString::number(cvtPtToUnit (unit, value), 'f', 3);
    buf+=" ";
    buf+=unitToString (unit);
    horizList->blockSignals(true);
    horizList->changeItem (buf, idx);
    horizList->blockSignals(false);
    horizLines[idx] = value;
  }
}

void OptionDialog::deleteHorizLine()
{
  if(horizLines.isEmpty())
    return;
  int idx = horizList->currentItem ();
  if (idx != -1)
  {
    horizLines.remove(horizLines.at(idx));
    horizList->removeItem(idx);
    modified = true;
    if(horizLines.isEmpty())
    {
        updateHorizHelpLine->setEnabled(false);
        delHorizHelpLine->setEnabled(false);
    }
  }

}

void OptionDialog::addVertLine()
{
  float value = vertValue->getValue ();
  vertLines.append(value);
  MeasurementUnit unit = PStateManager::instance ()->defaultMeasurementUnit ();
  QString buf=QString::number(cvtPtToUnit (unit, value), 'f', 3);
  buf+=" ";
  buf+=unitToString (unit);
  vertList->insertItem (buf);
  delVertHelpLine->setEnabled(true);
  updateVertHelpLine->setEnabled(true);
  modified = true;
}

void OptionDialog::updateVertLine()
{
  if(vertLines.isEmpty())
    return;
  int idx = vertList->currentItem ();
  if (idx != -1)
  {
    float value = vertValue->getValue ();
    MeasurementUnit unit = PStateManager::instance ()->defaultMeasurementUnit ();
    QString buf=QString::number(cvtPtToUnit (unit, value), 'f', 3);
    buf+=" ";
    buf+=unitToString (unit);
    vertList->blockSignals(true);
    vertList->changeItem (buf, idx);
    vertList->blockSignals(false);
    vertLines[idx] = value;
  }
}

void OptionDialog::deleteVertLine()
{
  if(vertLines.isEmpty())
    return;
  int idx = vertList->currentItem ();
  if(idx != -1)
  {
    vertLines.remove(vertLines.at(idx));
    vertList->removeItem (idx);
    modified = true;
    if(vertLines.isEmpty())
    {
          delVertHelpLine->setEnabled(false);
          updateVertHelpLine->setEnabled(false);
    }
  }
}

void OptionDialog::horizLineSelected(int idx)
{
  if(!horizLines.isEmpty())
    horizValue->setValue(*horizLines.at(idx));
}

void OptionDialog::vertLineSelected(int idx)
{
  if(!vertLines.isEmpty())
    vertValue->setValue (*vertLines.at(idx));
}

/**/
void OptionDialog::slotApply()
{
  /*Document settings*/

  /*Background*/
  doc->activePage()->bgColor(bgbutton->color());
  /*Grid*/
  doc->setGridDistance(hspinbox->getValue(), vspinbox->getValue());
  doc->showGrid(sbutton->isOn());
  doc->snapToGrid(gbutton->isOn());
  doc->gridColor(cbutton->color());
  /*Helplines*/
  doc->setHorizHelplines(horizLines);
  doc->setVertHelplines(vertLines);

  if(modified)
    doc->setModified();

  doc->emitChanged();
}

void OptionDialog::slotOk()
{
  slotApply();
  KDialogBase::slotOk();
}

int OptionDialog::setup (GDocument *adoc)
{

   OptionDialog dialog (adoc, 0L, "Options");

   int res=dialog.exec();
   if(res == QDialog::Accepted)
   {
      int selection = dialog.unit->currentItem ();
      PStateManager* psm = PStateManager::instance ();
      switch (selection)
      {
      case 0:
         psm->setDefaultMeasurementUnit (UnitPoint);
         break;
      case 1:
         psm->setDefaultMeasurementUnit (UnitMillimeter);
         break;
      case 2:
         psm->setDefaultMeasurementUnit (UnitInch);
                break;
      case 3:
         psm->setDefaultMeasurementUnit (UnitPica);
         break;
      case 4:
         psm->setDefaultMeasurementUnit (UnitCentimeter);
         break;
      case 5:
         psm->setDefaultMeasurementUnit (UnitDidot);
         break;
      case 6:
         psm->setDefaultMeasurementUnit (UnitCicero);
         break;
      default:
         break;
      }
      psm->setStepSizes (dialog.smallStep->getValue (),dialog.bigStep->getValue ());
      psm->setDuplicateOffsets (dialog.horiz->getValue (),dialog.vert->getValue ());
   }
   return res;
}

#include <OptionDialog.moc>
