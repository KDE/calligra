/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998-1999 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2001-2002 Igor Janssen (rm@kde.org)

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

#include "OptionsDialog.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qvgroupbox.h>
#include <qhbox.h>
#include <qcheckbox.h>

#include <kcombobox.h>
#include <kcolorbutton.h>
#include <klocale.h>

#include "kontour_view.h"
#include "GDocument.h"
#include "GPage.h"
#include "Canvas.h"
#include "UnitBox.h"

OptionsDialog::OptionsDialog(KontourView *aView, GDocument *aGDoc, QWidget *parent, const char *name):
KDialogBase(KDialogBase::TreeList, i18n("Configure Kontour"), Ok|Apply|Cancel, Ok, parent, name, true)
{
  mView = aView;
  mGDoc = aGDoc;
  mDocModified = false;
  mViewUpdate = false;

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
  list << i18n("Document") << i18n("Page Layout");
  createPageLayoutWidget(addPage(list));
  list.clear();
  list << i18n("Document") << i18n("Helplines") << i18n("Vertical");
  createVertLineWidget(addPage(list));
  list.clear();
  list << i18n("Document") << i18n("Helplines") << i18n("Horizontal");
  createHorizLineWidget(addPage(list));
  list.clear();
  list << i18n("Plugins");

  horizLines = mGDoc->horizHelplines();
  vertLines = mGDoc->vertHelplines();

  initHelplinesLists();
}

void OptionsDialog::createGeneralWidget(QWidget *parent)
{
  QGridLayout *layout = new QGridLayout(parent, 3, 2, KDialogBase::marginHint(), KDialogBase::spacingHint());
  
  QLabel *mUnitLabel = new QLabel(i18n("Unit:"), parent);
  layout->addWidget(mUnitLabel, 0, 0);
  unit = new KComboBox(parent);
  unit->insertItem(unitToString(UnitPoint));
  unit->insertItem(unitToString(UnitMillimeter));
  unit->insertItem(unitToString(UnitInch));
  unit->insertItem(unitToString(UnitPica));
  unit->insertItem(unitToString(UnitCentimeter));
  unit->insertItem(unitToString(UnitDidot));
  unit->insertItem(unitToString(UnitCicero));
  layout->addWidget(unit, 0, 1);
  unit->setCurrentItem((int)mView->unit());

  QLabel *mWSColorLabel = new QLabel(i18n("Workspace color:"), parent);
  mWSColorButton = new KColorButton(parent);
  mWSColorButton->setColor(mView->workSpaceColor());
  connect(mWSColorButton, SIGNAL(changed(const QColor&)), this, SLOT(slotSetViewUpdate()));
  layout->addWidget(mWSColorLabel, 1, 0);
  layout->addWidget(mWSColorButton, 1, 1);

  layout->setRowStretch(2, 1);
}

void OptionsDialog::createEditWidget (QWidget* parent)
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
    horiz->setRange(-1000.0, 1000.0);
    horiz->setStep(0.1);
    grid->addWidget(horiz, 0, 1);

    label = new QLabel(i18n("Vertical:"), box);
    grid->addWidget(label, 1, 0);

    vert = new UnitBox(box);
    vert->setRange(-1000.0, 1000.0);
    vert->setStep(0.1);
    grid->addWidget(vert, 1, 1);

    box = new QGroupBox(i18n("Step Distance"), parent);
    layout->addWidget(box);

    vboxlayout=new QVBoxLayout(box, KDialogBase::marginHint(), KDialogBase::spacingHint());
    vboxlayout->addSpacing(box->fontMetrics().height()/2);
    grid=new QGridLayout(vboxlayout, 2, 2);
    label = new QLabel(i18n("Small step:"), box);
    grid->addWidget(label, 0, 0);

    smallStep = new UnitBox(box);
    smallStep->setRange(-1000.0, 1000.0);
    smallStep->setStep(0.1);
    grid->addWidget(smallStep, 0, 1);

    label = new QLabel(i18n("Big step:"), box);
    grid->addWidget(label, 1, 0);

    bigStep = new UnitBox(box);
    bigStep->setRange(-1000.0, 1000.0);
    bigStep->setStep(0.1);
    grid->addWidget(bigStep, 1, 1);

/*    PStateManager *psm = PStateManager::instance ();
    horiz->setValue (psm->duplicateXOffset ());
    vert->setValue (psm->duplicateYOffset ());
    smallStep->setValue (psm->smallStepSize ());
    bigStep->setValue (psm->bigStepSize ());*/
}

// Background
void OptionsDialog::createBGWidget(QWidget *parent)
{
  QBoxLayout *layout = new QHBoxLayout(parent, KDialogBase::marginHint(), KDialogBase::spacingHint());
  QLabel* clabel = new QLabel(i18n("Background color:"), parent);
  bgbutton = new KColorButton(parent);
  connect(bgbutton, SIGNAL(changed(const QColor &)), this, SLOT(slotSetDocModified()));
  connect(bgbutton, SIGNAL(changed(const QColor &)), this, SLOT(slotSetViewUpdate()));
  bgbutton->setColor(mGDoc->activePage()->bgColor());
  layout->addWidget(clabel);
  layout->addWidget(bgbutton);
}

void OptionsDialog::createPageLayoutWidget(QWidget *parent)
{
  KoHeadFoot headFoot;
  mPageLayout = new KoPageLayoutDia(parent, 0L, mGDoc->activePage()->pageLayout(), headFoot, FORMAT_AND_BORDERS, KoUnit::U_PT);
}

// Grid
void OptionsDialog::createGridWidget(QWidget *parent)
{
  QGridLayout *layout = new QGridLayout(parent, 3, 2, KDialogBase::marginHint(), KDialogBase::spacingHint());

  QGroupBox *box = new QGroupBox(i18n("Distance"), parent);
  layout->addMultiCellWidget(box, 0, 0, 0, 1);

  QBoxLayout *vboxlayout = new QVBoxLayout(box, KDialogBase::marginHint(), KDialogBase::spacingHint());
  vboxlayout->addSpacing(box->fontMetrics().height() / 2);

  QGridLayout *grid = new QGridLayout(vboxlayout, 2, 2);

  QLabel *label = new QLabel(i18n("Horizontally"), box);
  grid->addWidget(label, 0, 0);
  hspinbox = new UnitBox(box);
  hspinbox->setPrecision(3);
  hspinbox->setRange(0, 1000);
  connect(hspinbox, SIGNAL(valueChanged(double)), this, SLOT(slotSetDocModified()));
  connect(hspinbox, SIGNAL(valueChanged(double)), this, SLOT(slotSetViewUpdate()));
  grid->addWidget(hspinbox, 0, 1);

  label = new QLabel(i18n("Vertically"), box);
  grid->addWidget(label, 1, 0);
  vspinbox = new UnitBox(box);
  vspinbox->setPrecision(3);
  vspinbox->setRange(0, 1000);
  connect(vspinbox, SIGNAL(valueChanged(double)), this, SLOT(slotSetDocModified()));
  connect(vspinbox, SIGNAL(valueChanged(double)), this, SLOT(slotSetViewUpdate()));
  grid->addWidget(vspinbox, 1, 1);

  hspinbox->setValue(mGDoc->xGrid());
  vspinbox->setValue(mGDoc->yGrid());

  gbutton = new QCheckBox(i18n("Snap to grid"), parent);
  gbutton->setChecked(mGDoc->snapToGrid());
  connect(gbutton, SIGNAL(stateChanged(int)), this, SLOT(slotSetDocModified()));
  layout->addWidget(gbutton, 1, 0);

  sbutton = new QCheckBox(i18n("Show grid"), parent);
  sbutton->setChecked(mGDoc->showGrid());
  connect(sbutton, SIGNAL(stateChanged(int)), this, SLOT(slotSetDocModified()));
  connect(sbutton, SIGNAL(stateChanged(int)), this, SLOT(slotSetViewUpdate()));
  layout->addWidget(sbutton, 1, 1);

  cbutton = new KColorButton(parent);
  cbutton->setColor(mGDoc->gridColor());
  QLabel* clabel = new QLabel(i18n("Grid color:"), parent);
  connect(cbutton, SIGNAL(changed(const QColor&)), this, SLOT(slotSetDocModified()));
  connect(cbutton, SIGNAL(changed(const QColor&)), this, SLOT(slotSetViewUpdate()));
  layout->addWidget(cbutton, 2, 1);
  layout->addWidget(clabel, 2, 0);
}

// Helplines
void OptionsDialog::createHorizLineWidget(QWidget* parent)
{

    QBoxLayout *layout=new QHBoxLayout(parent, KDialogBase::marginHint(), KDialogBase::spacingHint());
    QBoxLayout *left=new QVBoxLayout(layout);

    horizValue = new UnitBox (parent);
    horizValue->setRange(-1000.0, 1000.0);
    horizValue->setStep(0.1);
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

void OptionsDialog::createVertLineWidget(QWidget *parent)
{
  QBoxLayout *layout = new QHBoxLayout(parent, KDialogBase::marginHint(), KDialogBase::spacingHint());
  QBoxLayout *left = new QVBoxLayout(layout);

  vertValue = new UnitBox(parent);
  vertValue->setRange(-1000.0, 1000.0);
  vertValue->setStep(0.1);
  vertValue->setValue(0.0);
  left->addWidget(vertValue);

  vertList = new QListBox(parent);
  vertList->setMultiSelection(false);
  connect(vertList, SIGNAL(highlighted(int)), this, SLOT(vertLineSelected(int)));
  left->addWidget(vertList);
  layout->addSpacing(KDialogBase::spacingHint() * 2);

  QBoxLayout *right = new QVBoxLayout(layout);
  addVertHelpLine = new QPushButton(i18n("Add"), parent);
  connect(addVertHelpLine, SIGNAL(clicked()), this, SLOT(addVertLine()));
  right->addWidget(addVertHelpLine);

  updateVertHelpLine = new QPushButton(i18n("Update"), parent);
  connect(updateVertHelpLine, SIGNAL(clicked()), this, SLOT(updateVertLine()));
  right->addWidget(updateVertHelpLine);

  delVertHelpLine = new QPushButton(i18n("Delete"), parent);
  connect(delVertHelpLine, SIGNAL(clicked()), this, SLOT(deleteVertLine()));
  right->addWidget(delVertHelpLine);
  right->addStretch();
}

void OptionsDialog::initHelplinesLists()
{
  QValueList<double>::Iterator i;
  QString buf;
  MeasurementUnit unit;// = PStateManager::instance ()->defaultMeasurementUnit ();

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

void OptionsDialog::addHorizLine()
{
  double value = horizValue->getValue ();
  horizLines.append(value);
  MeasurementUnit unit;// = PStateManager::instance ()->defaultMeasurementUnit ();
  QString buf=QString::number(cvtPtToUnit (unit, value), 'f', 3);
  buf+=" ";
  buf+=unitToString (unit);
  horizList->insertItem (buf);
  updateHorizHelpLine->setEnabled(true);
  delHorizHelpLine->setEnabled(true);
  mDocModified = true;
}

void OptionsDialog::updateHorizLine()
{
  if(horizLines.isEmpty())
    return;
  int idx = horizList->currentItem ();
  if (idx != -1)
  {
    double value = horizValue->getValue ();
    MeasurementUnit unit;// = PStateManager::instance ()->defaultMeasurementUnit ();
    QString buf=QString::number(cvtPtToUnit (unit, value), 'f', 3);
    buf+=" ";
    buf+=unitToString (unit);
    horizList->blockSignals(true);
    horizList->changeItem (buf, idx);
    horizList->blockSignals(false);
    horizLines[idx] = value;
  }
}

void OptionsDialog::deleteHorizLine()
{
  if(horizLines.isEmpty())
    return;
  int idx = horizList->currentItem ();
  if (idx != -1)
  {
    horizLines.remove(horizLines.at(idx));
    horizList->removeItem(idx);
    mDocModified = true;
    if(horizLines.isEmpty())
    {
        updateHorizHelpLine->setEnabled(false);
        delHorizHelpLine->setEnabled(false);
    }
  }

}

void OptionsDialog::addVertLine()
{
  double value = vertValue->getValue ();
  vertLines.append(value);
  MeasurementUnit unit;// = PStateManager::instance ()->defaultMeasurementUnit ();
  QString buf=QString::number(cvtPtToUnit (unit, value), 'f', 3);
  buf+=" ";
  buf+=unitToString (unit);
  vertList->insertItem (buf);
  delVertHelpLine->setEnabled(true);
  updateVertHelpLine->setEnabled(true);
  mDocModified = true;
}

void OptionsDialog::updateVertLine()
{
  if(vertLines.isEmpty())
    return;
  int idx = vertList->currentItem ();
  if (idx != -1)
  {
    double value = vertValue->getValue ();
    MeasurementUnit unit;// = PStateManager::instance ()->defaultMeasurementUnit ();
    QString buf=QString::number(cvtPtToUnit (unit, value), 'f', 3);
    buf+=" ";
    buf+=unitToString (unit);
    vertList->blockSignals(true);
    vertList->changeItem (buf, idx);
    vertList->blockSignals(false);
    vertLines[idx] = value;
  }
}

void OptionsDialog::deleteVertLine()
{
  if(vertLines.isEmpty())
    return;
  int idx = vertList->currentItem ();
  if(idx != -1)
  {
    vertLines.remove(vertLines.at(idx));
    vertList->removeItem (idx);
    mDocModified = true;
    if(vertLines.isEmpty())
    {
      delVertHelpLine->setEnabled(false);
      updateVertHelpLine->setEnabled(false);
    }
  }
}

void OptionsDialog::horizLineSelected(int idx)
{
  if(!horizLines.isEmpty())
    horizValue->setValue(*horizLines.at(idx));
}

void OptionsDialog::vertLineSelected(int idx)
{
  if(!vertLines.isEmpty())
    vertValue->setValue (*vertLines.at(idx));
}

/**/
void OptionsDialog::slotSetDocModified()
{
  mDocModified = true;
}

void OptionsDialog::slotSetViewUpdate()
{
  mViewUpdate = true;
}

void OptionsDialog::slotApply()
{
  /* Units settings */
  int u = unit->currentItem();
  switch(u)
  {
  case 0:
    mView->unit(UnitPoint);
    break;
  case 1:
    mView->unit(UnitMillimeter);
    break;
  case 2:
    mView->unit(UnitInch);
    break;
  case 3:
    mView->unit(UnitPica);
    break;
  case 4:
    mView->unit(UnitCentimeter);
    break;
  case 5:
    mView->unit(UnitDidot);
    break;
  case 6:
    mView->unit(UnitCicero);
    break;
  }

  mView->workSpaceColor(mWSColorButton->color());
  
  if(mViewUpdate)
    mView->canvas()->repaint();

  /*Document settings*/

  /*Background*/
  mGDoc->activePage()->bgColor(bgbutton->color());
  /*Grid*/
  mGDoc->setGridDistance(hspinbox->getValue(), vspinbox->getValue());
  mGDoc->showGrid(sbutton->isOn());
  mGDoc->snapToGrid(gbutton->isOn());
  mGDoc->gridColor(cbutton->color());
  /*Helplines*/
  mGDoc->horizHelplines(horizLines);
  mGDoc->vertHelplines(vertLines);

  if(mDocModified)
  {
    mGDoc->setModified();
    mDocModified = false;
  }

//  mGDoc->emitChanged();
}

void OptionsDialog::slotOk()
{
  slotApply();
  KDialogBase::slotOk();
}

#include "OptionsDialog.moc"
