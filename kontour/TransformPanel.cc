/* -*- C++ -*-

  This file is part of Kontour.
  Copyright (C) 2001 Rob Buis (rwlbuis@wanadoo.nl)

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

#include "TransformPanel.h"

#include <qtabwidget.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qcheckbox.h>

#include <klocale.h>
#include <knuminput.h>
#include <kcommand.h>
#include <kdebug.h>
#include <kontour_global.h>

#include "kontour_view.h"
#include "GDocument.h"
#include "GPage.h"
#include "TranslateCmd.h"
#include "RotateCmd.h"
#include "ShearCmd.h"
#include "ScaleCmd.h"
#include "DuplicateCmd.h"

const double deg2rad = 0.017453292519943295769; // pi/180

TransformPanel::TransformPanel(KontourView *aView, QWidget *parent, const char *name):
QDockWindow(QDockWindow::InDock, parent, name)
{
  mView = aView;

  mTransformPanel = new QWidget(this);
  QGridLayout *mTransformLayout = new QGridLayout(mTransformPanel, 2, 2);
  mTab = new QTabWidget(mTransformPanel);
  mTab->setTabShape(QTabWidget::Triangular);

  mApplyBtn = new QPushButton(i18n("Apply"), mTransformPanel);
  connect(mApplyBtn, SIGNAL(clicked()), this, SLOT(slotApplyPressed()));
  mDuplicateBtn = new QPushButton(i18n("Duplicate"), mTransformPanel);
  connect(mDuplicateBtn, SIGNAL(clicked()), this, SLOT(slotDupPressed()));

  mTransformLayout->addMultiCellWidget(mTab, 0, 0, 0, 1);
  mTransformLayout->addWidget(mApplyBtn, 1, 0);
  mTransformLayout->addWidget(mDuplicateBtn, 1, 1);

  QGridLayout *mLayout;

  /* Translate */
  mTranslate = new QWidget(mTab);
  mLayout = new QGridLayout(mTranslate, 3, 2);
  QLabel *mHorizText = new QLabel(i18n("Horizontal:"), mTranslate);
  mHorizBox = new QSpinBox(-1000, 1000, 1, mTranslate);
  //connect(mHoriz, SIGNAL(valueChanged(int)), this, SLOT(slotTranslateChanged(int)));
  QLabel *mVertText = new QLabel(i18n("Vertical:"), mTranslate);
  mVertBox = new QSpinBox(-1000, 1000, 1, mTranslate);
  //connect(mVert, SIGNAL(valueChanged(int)), this, SLOT(slotTranslateChanged(int)));
  QCheckBox *mRelative = new QCheckBox(i18n("Relative"), mTranslate, "T");
  connect(mRelative, SIGNAL(toggled(bool)), this, SLOT(slotRelativeToggled(bool)));

  mLayout->addWidget(mHorizText, 0, 0);
  mLayout->addWidget(mHorizBox, 0, 1);
  mLayout->addWidget(mVertText, 1, 0);
  mLayout->addWidget(mVertBox, 1, 1);
  mLayout->addMultiCellWidget(mRelative, 2, 2, 0, 1);
  mTab->insertTab(mTranslate, i18n("T"));
  mTab->setTabToolTip(mTranslate, i18n("Translate"));

  /* Rotate */
  mRotate = new QWidget(mTab);
  mLayout = new QGridLayout(mRotate, 3, 2);
  QLabel *mAngleText = new QLabel(i18n("Angle"), mRotate);
  mAngleBox = new QSpinBox(-360, 360, 1, mRotate);
  //connect(mHoriz, SIGNAL(valueChanged(int)), this, SLOT(slotHorizTranslateChanged(int)));
  mRelative = new QCheckBox(i18n("Relative"), mRotate, "R");
  connect(mRelative, SIGNAL(toggled(bool)), this, SLOT(slotRelativeToggled(bool)));

  mLayout->addWidget(mAngleText, 0, 0);
  mLayout->addWidget(mAngleBox, 0, 1);
  mLayout->addMultiCellWidget(mRelative, 2, 2, 0, 1);
  mTab->insertTab(mRotate, i18n("R"));
  mTab->setTabToolTip(mRotate, i18n("Rotate"));

  /* Scale */
  mScale = new QWidget(mTab);
  mLayout = new QGridLayout(mScale, 3, 2);
  QLabel *mScaleXText = new QLabel(i18n("Horizontal:"), mScale);
  mScaleXBox = new QSpinBox(0, 1000, 1, mScale);
  mScaleXBox->setSuffix("%");
  QLabel *mScaleYText = new QLabel(i18n("Vertical:"), mScale);
  mScaleYBox = new QSpinBox(0, 1000, 1, mScale);
  mScaleYBox->setSuffix("%");
  QCheckBox *mSUniform = new QCheckBox(i18n("Uniform"), mScale, "S");
  //connect(mSUniform, SIGNAL(toggled(bool)), this, SLOT(slotRelativeToggled(bool)));

  mLayout->addWidget(mScaleXText, 0, 0);
  mLayout->addWidget(mScaleXBox, 0, 1);
  mLayout->addWidget(mScaleYText, 1, 0);
  mLayout->addWidget(mScaleYBox, 1, 1);
  mLayout->addMultiCellWidget(mSUniform, 2, 2, 0, 1);
  mTab->insertTab(mScale, i18n("S"));
  mTab->setTabToolTip(mScale, i18n("Scale"));

  /* Shear */
  mShear = new QWidget(mTab);
  mLayout = new QGridLayout(mShear, 3, 2);
  QLabel *mShearAngleXText = new QLabel(i18n("Horizontal:"), mShear);
  mShearAngleXBox = new QSpinBox(-89, 89, 1, mShear);
  QLabel *mShearAngleYText = new QLabel(i18n("Vertical:"), mShear);
  mShearAngleYBox = new QSpinBox(-89, 89, 1, mShear);
  QCheckBox *mSRelative = new QCheckBox(i18n("Relative"), mShear, "R");
  connect(mSRelative, SIGNAL(toggled(bool)), this, SLOT(slotRelativeToggled(bool)));

  mLayout->addWidget(mShearAngleXText, 0, 0);
  mLayout->addWidget(mShearAngleXBox, 0, 1);
  mLayout->addWidget(mShearAngleYText, 1, 0);
  mLayout->addWidget(mShearAngleYBox, 1, 1);
  mLayout->addMultiCellWidget(mSRelative, 2, 2, 0, 1);
  mTab->insertTab(mShear, i18n("S"));
  mTab->setTabToolTip(mShear, i18n("Shear"));

  setWidget(mTransformPanel);
  setCloseMode(QDockWindow::Always);
  setOpaqueMoving(true);
  setCaption(i18n("Transformation"));

  mTRelative = false;
  mRRelative = false;
}

void TransformPanel::slotUpdate()
{
  if(mView->activeDocument()->activePage()->selectionIsEmpty())
    mTransformPanel->setEnabled(false);
  else
  {
    mTransformPanel->setEnabled(true);
    if(mTRelative)
    {
      mHorizBox->setValue(0);
      mVertBox->setValue(0);
    }
    else
    {
      mHorizBox->setValue(static_cast<int>(mView->activeDocument()->activePage()->handle().rotCenter().x()));
      mVertBox->setValue(static_cast<int>(mView->activeDocument()->activePage()->handle().rotCenter().y()));
    }
    mShearAngleXBox->setValue(0);
    mShearAngleYBox->setValue(0);
    mScaleXBox->setValue(100);
    mScaleYBox->setValue(100);
  }
}

void TransformPanel::slotRelativeToggled(bool toggled)
{
  if(mTab->currentPage() == mTranslate)
  {
    mTRelative = toggled;
    if(toggled)
    {
      mHorizBox->setValue(0);
      mVertBox->setValue(0);
    }
    else
    {
      mHorizBox->setValue(static_cast<int>(mView->activeDocument()->activePage()->handle().rotCenter().x()));
      mVertBox->setValue(static_cast<int>(mView->activeDocument()->activePage()->handle().rotCenter().y()));
    }
  }
  else if(mTab->currentPage() == mRotate)
  {
    mRRelative = toggled;
    if(toggled)
    {
      mAngleBox->setValue(0);
    }
  }
}

void TransformPanel::slotApplyPressed()
{
  TransformationCmd *cmd = 0L;
  if(mTab->currentPage() == mTranslate)
    if(mTRelative)
    {
      if(mHorizBox->value() != 0 || mVertBox->value() != 0)
        cmd = new TranslateCmd(mView->activeDocument(), mHorizBox->value(), mVertBox->value());
    }
    else
    {
      double x = mView->activeDocument()->activePage()->handle().rotCenter().x();
      double y = mView->activeDocument()->activePage()->handle().rotCenter().y();
      if(mHorizBox->value() != static_cast<int>(x) || mVertBox->value() != static_cast<int>(y))
        cmd = new TranslateCmd(mView->activeDocument(), mHorizBox->value() - x, mVertBox->value()  - y);
    }
  else if(mTab->currentPage() == mRotate)
    cmd = new RotateCmd(mView->activeDocument(), mView->activeDocument()->activePage()->handle().rotCenter(), mAngleBox->value());
  else if(mTab->currentPage() == mShear)
    cmd = new ShearCmd(mView->activeDocument(), mView->activeDocument()->activePage()->handle().rotCenter(), mShearAngleXBox->value() * deg2rad, mShearAngleYBox->value() * deg2rad);
  else if(mTab->currentPage() == mScale)
    cmd = new ScaleCmd(mView->activeDocument(), Kontour::HPosCenter, mScaleXBox->value() / 100.0, mScaleYBox->value() / 100.0, mView->activeDocument()->activePage()->boundingBoxForSelection());

  if(cmd)
  {
    KontourDocument *doc = (KontourDocument *)mView->koDocument();
    doc->history()->addCommand(cmd);
  }
}

void TransformPanel::slotDupPressed()
{
  TransformationCmd *tcmd = 0L;
  if(mTab->currentPage() == mTranslate)
    if(mTRelative)
    {
      if(mHorizBox->value() != 0 || mVertBox->value() != 0)
        tcmd = new TranslateCmd(mView->activeDocument(), mHorizBox->value(), mVertBox->value());
    }
    else
    {
      double x = mView->activeDocument()->activePage()->handle().rotCenter().x();
      double y = mView->activeDocument()->activePage()->handle().rotCenter().y();
      if(mHorizBox->value() != static_cast<int>(x) || mVertBox->value() != static_cast<int>(y))
        tcmd = new TranslateCmd(mView->activeDocument(), mHorizBox->value() - x, mVertBox->value()  - y);
    }
  else if(mTab->currentPage() == mRotate)
    tcmd = new RotateCmd(mView->activeDocument(), mView->activeDocument()->activePage()->handle().rotCenter(), mAngleBox->value());
  else if(mTab->currentPage() == mShear)
    tcmd = new ShearCmd(mView->activeDocument(), mView->activeDocument()->activePage()->handle().rotCenter(), mShearAngleXBox->value() * deg2rad, mShearAngleYBox->value() * deg2rad);
  else if(mTab->currentPage() == mScale)
    tcmd = new ScaleCmd(mView->activeDocument(), Kontour::HPosCenter, mScaleXBox->value() / 100.0, mScaleYBox->value() / 100.0, mView->activeDocument()->activePage()->boundingBoxForSelection());

  if(tcmd)
  {
    KMacroCommand *cmd = new KMacroCommand(i18n("Transform Duplicate"));
    cmd->addCommand(new DuplicateCmd(mView->activeDocument()));
    cmd->addCommand(tcmd);
    KontourDocument *doc = (KontourDocument *)mView->koDocument();
    doc->history()->addCommand(cmd);
  }
}

#include "TransformPanel.moc"
