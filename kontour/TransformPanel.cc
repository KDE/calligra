/* -*- C++ -*-

  This file is part of Kontour.
  Copyright (C) 2001 Rob Buis (rwlbuis@wanadoo..nl)

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
#include "DuplicateCmd.h"

#include <qtabwidget.h>
#include <qgroupbox.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qwmatrix.h>

#include <klocale.h>
#include <knuminput.h>
#include <kcommand.h>
#include <kdebug.h>

const double deg2rad = 0.017453292519943295769; // pi/180

TransformPanel::TransformPanel(QWidget *parent, const char *name)
: QDockWindow(QDockWindow::InDock, parent, name)
{
  mTab = new QTabWidget(this);
  mTab->setTabShape(QTabWidget::Triangular);

  /* Translate */
  mTranslate = new QGroupBox(5, Qt::Vertical, mTab);
  mHoriz = new KIntNumInput(0, mTranslate);
  //connect(mHoriz, SIGNAL(valueChanged(int)), this, SLOT(slotTranslateChanged(int)));
  mHoriz->setRange(-1000, 1000, 1, false);
  mHoriz->setLabel(i18n("Horizontal"));
  mVert  = new KIntNumInput(0, mTranslate);
  //connect(mVert, SIGNAL(valueChanged(int)), this, SLOT(slotTranslateChanged(int)));
  mVert->setRange(-1000, 1000, 1, false);
  mVert->setLabel(i18n("Vertical"));
  QCheckBox *mTRelative = new QCheckBox(i18n("Relative"), mTranslate, "T");
  connect(mTRelative, SIGNAL(toggled(bool)), this, SLOT(slotRelativeToggled(bool)));
  QPushButton *but = new QPushButton(i18n("Duplicate"), mTranslate);
  connect(but, SIGNAL(clicked()), this, SLOT(slotDupPressed()));
  but = new QPushButton(i18n("Apply"), mTranslate);
  connect(but, SIGNAL(clicked()), this, SLOT(slotApplyPressed()));
  mTab->insertTab(mTranslate, i18n("T"));

  /* Rotate */
  mRotate = new QGroupBox(4, Qt::Vertical, mTab);
  mAngle = new KIntNumInput(0, mRotate);
  //connect(mHoriz, SIGNAL(valueChanged(int)), this, SLOT(slotHorizTranslateChanged(int)));
  mAngle->setRange(-360, 360, 1, false);
  mAngle->setLabel(i18n("Angle"));
  QCheckBox *mRRelative = new QCheckBox(i18n("Relative"), mRotate, "R");
  connect(mRRelative, SIGNAL(toggled(bool)), this, SLOT(slotRelativeToggled(bool)));
  but = new QPushButton(i18n("Duplicate"), mRotate);
  connect(but, SIGNAL(clicked()), this, SLOT(slotDupPressed()));
  but = new QPushButton(i18n("Apply"), mRotate);
  connect(but, SIGNAL(clicked()), this, SLOT(slotApplyPressed()));
  mTab->insertTab(mRotate, i18n("R"));

  /* Shear */
  mShear = new QGroupBox(5, Qt::Vertical, mTab);
  mShearAngleX = new KIntNumInput(0, mShear);
  mShearAngleX->setRange(-360, 360, 1, false);
  mShearAngleX->setLabel(i18n("Shear horizontal"));
  mShearAngleY = new KIntNumInput(0, mShear);
  mShearAngleY->setRange(-360, 360, 1, false);
  mShearAngleY->setLabel(i18n("Shear vertical"));
  QCheckBox *mSRelative = new QCheckBox(i18n("Relative"), mShear, "R");
  connect(mSRelative, SIGNAL(toggled(bool)), this, SLOT(slotRelativeToggled(bool)));
  but = new QPushButton(i18n("Duplicate"), mShear);
  connect(but, SIGNAL(clicked()), this, SLOT(slotDupPressed()));
  but = new QPushButton(i18n("Apply"), mShear);
  connect(but, SIGNAL(clicked()), this, SLOT(slotApplyPressed()));
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
    mHoriz->setValue(int(mHandle->rotCenter().x()));
    mVert->setValue(int(mHandle->rotCenter().y()));
  }
}

void TransformPanel::slotRelativeToggled(bool toggled)
{
  if(mTab->currentPage() == mTranslate)
  {
	mTRelative = toggled;
    if(toggled)
	{
	  mHoriz->setValue(0);
	  mVert->setValue(0);
	}
	else
	{
	  mHoriz->setValue(int(mHandle->rotCenter().x()));
	  mVert->setValue(int(mHandle->rotCenter().y()));
	}
  }
  else if(mTab->currentPage() == mRotate)
  {
	mRRelative = toggled;
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
	  if(!(mHoriz->value() == 0 && mVert->value() == 0))
        c->addCommand(new TranslateCmd(mPage->document(), double(mHoriz->value()), double(mVert->value())));
	}
	else
	{
	  if(mHoriz->value() != mHandle->rotCenter().x() || mVert->value() != mHandle->rotCenter().y())
        c->addCommand(new TranslateCmd(mPage->document(), double(mHoriz->value() - mHandle->rotCenter().x()), double(mVert->value()  - mHandle->rotCenter().y())));
    }
  else if(mTab->currentPage() == mRotate)
    c->addCommand(new RotateCmd(mPage->document(), mHandle->rotCenter(), mAngle->value()));
  else if(mTab->currentPage() == mShear)
    c->addCommand(new ShearCmd(mPage->document(), mHandle->rotCenter(), mShearAngleX->value() * deg2rad,
	                 mShearAngleY->value() * deg2rad));
  emit changeTransform(c);
}

void TransformPanel::slotApplyPressed()
{
  TransformationCmd *c = 0;
  if(mTab->currentPage() == mTranslate)
    // Handle only translates that really change the object
    if(mTRelative)
	{
	  if(!(mHoriz->value() == 0 && mVert->value() == 0))
        c = new TranslateCmd(mPage->document(), double(mHoriz->value()), double(mVert->value()));
	}
	else
	{
	  if(mHoriz->value() != mHandle->rotCenter().x() || mVert->value() != mHandle->rotCenter().y())
        c = new TranslateCmd(mPage->document(), double(mHoriz->value() - mHandle->rotCenter().x()),
	                                            double(mVert->value()  - mHandle->rotCenter().y()));
    }
  else if(mTab->currentPage() == mRotate)
    c = new RotateCmd(mPage->document(), mHandle->rotCenter(), mAngle->value());
  else if(mTab->currentPage() == mShear)
    c = new ShearCmd(mPage->document(), mHandle->rotCenter(), mShearAngleX->value() * deg2rad,
	                 mShearAngleY->value() * deg2rad);

  if(c)
    emit changeTransform(c);
}

#include "TransformPanel.moc"
