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
#include "GPage.h"
#include "TranslateCmd.h"
#include "RotateCmd.h"
#include "ShearCmd.h"
#include "ScaleCmd.h"
#include "DuplicateCmd.h"

#include <qtabwidget.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qgroupbox.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qwmatrix.h>

#include <klocale.h>
#include <knuminput.h>
#include <kcommand.h>
#include <kdebug.h>
#include <kontour_global.h>

const double deg2rad = 0.017453292519943295769; // pi/180

TransformPanel::TransformPanel(QWidget *parent, const char *name):
QDockWindow(QDockWindow::InDock, parent, name)
{
  mTab = new QTabWidget(this);
  mTab->setTabShape(QTabWidget::Triangular);

  QGridLayout *mLayout;
  QPushButton *mDuplicateBtn;
  QPushButton *mApplyBtn;

  /* Translate */
  mTranslate = new QWidget(mTab);
  mLayout = new QGridLayout(mTranslate, 4, 2);
  QLabel *mHorizText = new QLabel(i18n("Horizontal"), mTranslate);
  mHorizBox = new QSpinBox(-1000, 1000, 1, mTranslate);
  //connect(mHoriz, SIGNAL(valueChanged(int)), this, SLOT(slotTranslateChanged(int)));
  QLabel *mVertText = new QLabel(i18n("Vertical"), mTranslate);
  mVertBox = new QSpinBox(-1000, 1000, 1, mTranslate);
  //connect(mVert, SIGNAL(valueChanged(int)), this, SLOT(slotTranslateChanged(int)));
  QCheckBox *mRelative = new QCheckBox(i18n("Relative"), mTranslate, "T");
  connect(mRelative, SIGNAL(toggled(bool)), this, SLOT(slotRelativeToggled(bool)));
  mDuplicateBtn = new QPushButton(i18n("Duplicate"), mTranslate);
  connect(mDuplicateBtn, SIGNAL(clicked()), this, SLOT(slotDupPressed()));
  mApplyBtn = new QPushButton(i18n("Apply"), mTranslate);
  connect(mApplyBtn, SIGNAL(clicked()), this, SLOT(slotApplyPressed()));

  mLayout->addWidget(mHorizText, 0, 0);
  mLayout->addWidget(mHorizBox, 0, 1);
  mLayout->addWidget(mVertText, 1, 0);
  mLayout->addWidget(mVertBox, 1, 1);
  mLayout->addMultiCellWidget(mRelative, 2, 2, 0, 1);
  mLayout->addWidget(mDuplicateBtn, 3, 0);
  mLayout->addWidget(mApplyBtn, 3, 1);
  mTab->insertTab(mTranslate, i18n("T"));

  /* Rotate */
  mRotate = new QWidget(mTab);
  mLayout = new QGridLayout(mRotate, 3, 2);
  QLabel *mAngleText = new QLabel(i18n("Angle"), mRotate);
  mAngleBox = new QSpinBox(-360, 360, 1, mRotate);
  //connect(mHoriz, SIGNAL(valueChanged(int)), this, SLOT(slotHorizTranslateChanged(int)));
  mRelative = new QCheckBox(i18n("Relative"), mRotate, "R");
  connect(mRelative, SIGNAL(toggled(bool)), this, SLOT(slotRelativeToggled(bool)));
  mDuplicateBtn = new QPushButton(i18n("Duplicate"), mRotate);
  connect(mDuplicateBtn, SIGNAL(clicked()), this, SLOT(slotDupPressed()));
  mApplyBtn = new QPushButton(i18n("Apply"), mRotate);
  connect(mApplyBtn, SIGNAL(clicked()), this, SLOT(slotApplyPressed()));

  mLayout->addWidget(mAngleText, 0, 0);
  mLayout->addWidget(mAngleBox, 0, 1);
  mLayout->addMultiCellWidget(mRelative, 1, 1, 0, 1);
  mLayout->addWidget(mDuplicateBtn, 2, 0);
  mLayout->addWidget(mApplyBtn, 2, 1);
  mTab->insertTab(mRotate, i18n("R"));

  /* Scale */
  mScale = new QWidget(mTab);
  mLayout = new QGridLayout(mScale, 4, 2);
  QLabel *mScaleXText = new QLabel(i18n("Scale horizontal"), mScale);
  mScaleX = new QSpinBox(0, 1000, 1, mScale);
  mScaleX->setSuffix("%");
  QLabel *mScaleYText = new QLabel(i18n("Scale vertical"), mScale);
  mScaleY = new QSpinBox(0, 1000, 1, mScale);
  mScaleY->setSuffix("%");
  QCheckBox *mSUniform = new QCheckBox(i18n("Uniform"), mScale, "S");
  //connect(mSUniform, SIGNAL(toggled(bool)), this, SLOT(slotRelativeToggled(bool)));
  mDuplicateBtn = new QPushButton(i18n("Duplicate"), mScale);
  connect(mDuplicateBtn, SIGNAL(clicked()), this, SLOT(slotDupPressed()));
  mApplyBtn = new QPushButton(i18n("Apply"), mScale);
  connect(mApplyBtn, SIGNAL(clicked()), this, SLOT(slotApplyPressed()));

  mLayout->addWidget(mScaleXText, 0, 0);
  mLayout->addWidget(mScaleX, 0, 1);
  mLayout->addWidget(mScaleYText, 1, 0);
  mLayout->addWidget(mScaleY, 1, 1);
  mLayout->addMultiCellWidget(mSUniform, 2, 2, 0, 1);
  mLayout->addWidget(mDuplicateBtn, 3, 0);
  mLayout->addWidget(mApplyBtn, 3, 1);
  mTab->insertTab(mScale, i18n("S"));

  /* Shear */
  mShear = new QWidget(mTab);
  mLayout = new QGridLayout(mShear, 4, 2);
  QLabel *mShearAngleXText = new QLabel(i18n("Shear horizontal"), mShear);
  mShearAngleXBox = new QSpinBox(-89, 89, 1, mShear);
  QLabel *mShearAngleYText = new QLabel(i18n("Shear vertical"), mShear);
  mShearAngleYBox = new QSpinBox(-89, 89, 1, mShear);
  QCheckBox *mSRelative = new QCheckBox(i18n("Relative"), mShear, "R");
  connect(mSRelative, SIGNAL(toggled(bool)), this, SLOT(slotRelativeToggled(bool)));
  mDuplicateBtn = new QPushButton(i18n("Duplicate"), mShear);
  connect(mDuplicateBtn, SIGNAL(clicked()), this, SLOT(slotDupPressed()));
  mApplyBtn = new QPushButton(i18n("Apply"), mShear);
  connect(mApplyBtn, SIGNAL(clicked()), this, SLOT(slotApplyPressed()));

  mLayout->addWidget(mShearAngleXText, 0, 0);
  mLayout->addWidget(mShearAngleXBox, 0, 1);
  mLayout->addWidget(mShearAngleYText, 1, 0);
  mLayout->addWidget(mShearAngleYBox, 1, 1);
  mLayout->addMultiCellWidget(mSRelative, 2, 2, 0, 1);
  mLayout->addWidget(mDuplicateBtn, 3, 0);
  mLayout->addWidget(mApplyBtn, 3, 1);
  mTab->insertTab(mShear, i18n("S"));

  setWidget(mTab);
  setCaption(i18n("Transform"));

  mTRelative = false;
  mRRelative = false;
}

void TransformPanel::setContext(const QWMatrix &m, GPage *p)
{
  mPage = p;
  mHandle = &(p->handle());
  if(!mTRelative)
  {
    mHorizBox->setValue(int(mHandle->rotCenter().x()));
    mVertBox->setValue(int(mHandle->rotCenter().y()));
  }
  mShearAngleXBox->setValue(0);
  mShearAngleYBox->setValue(0);
  mScaleX->setValue(100);
  mScaleY->setValue(100);
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
	  mHorizBox->setValue(int(mHandle->rotCenter().x()));
	  mVertBox->setValue(int(mHandle->rotCenter().y()));
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

void TransformPanel::slotDupPressed()
{
  KMacroCommand *c = new KMacroCommand(i18n("Transform Duplicate"));
  c->addCommand(new DuplicateCmd(mPage->document()));
  if(mTab->currentPage() == mTranslate)
    // Handle only translates that really change the object
    if(mTRelative)
	{
	  if(!(mHorizBox->value() == 0 && mVertBox->value() == 0))
        c->addCommand(new TranslateCmd(mPage->document(), double(mHorizBox->value()), double(mVertBox->value())));
	}
	else
	{
	  if(mHorizBox->value() != mHandle->rotCenter().x() || mVertBox->value() != mHandle->rotCenter().y())
        c->addCommand(new TranslateCmd(mPage->document(), double(mHorizBox->value() - mHandle->rotCenter().x()), double(mVertBox->value()  - mHandle->rotCenter().y())));
    }
  else if(mTab->currentPage() == mRotate)
    c->addCommand(new RotateCmd(mPage->document(), mHandle->rotCenter(), mAngleBox->value()));
  else if(mTab->currentPage() == mShear)
    c->addCommand(new ShearCmd(mPage->document(), mHandle->rotCenter(), mShearAngleXBox->value() * deg2rad,
	                 mShearAngleYBox->value() * deg2rad));
  else if(mTab->currentPage() == mScale)
    c->addCommand(new ScaleCmd(mPage->document(), Kontour::HPos_Center, mScaleX->value() / 100.0, mScaleY->value() / 100.0, mPage->boundingBoxForSelection()));
  emit changeTransform(c);
}

void TransformPanel::slotApplyPressed()
{
  TransformationCmd *c = 0;
  if(mTab->currentPage() == mTranslate)
    // Handle only translates that really change the object
    if(mTRelative)
	{
	  if(!(mHorizBox->value() == 0 && mVertBox->value() == 0))
        c = new TranslateCmd(mPage->document(), double(mHorizBox->value()), double(mVertBox->value()));
	}
	else
	{
	  if(mHorizBox->value() != mHandle->rotCenter().x() || mVertBox->value() != mHandle->rotCenter().y())
        c = new TranslateCmd(mPage->document(), double(mHorizBox->value() - mHandle->rotCenter().x()),
	                                            double(mVertBox->value()  - mHandle->rotCenter().y()));
    }
  else if(mTab->currentPage() == mRotate)
    c = new RotateCmd(mPage->document(), mHandle->rotCenter(), mAngleBox->value());
  else if(mTab->currentPage() == mShear)
    c = new ShearCmd(mPage->document(), mHandle->rotCenter(), mShearAngleXBox->value() * deg2rad,
	                 mShearAngleYBox->value() * deg2rad);
  else if(mTab->currentPage() == mScale)
    c = new ScaleCmd(mPage->document(), Kontour::HPos_Center, mScaleX->value() / 100.0, mScaleY->value() / 100.0, mPage->boundingBoxForSelection());

  if(c)
    emit changeTransform(c);
}

#include "TransformPanel.moc"
