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

TransformPanel::TransformPanel(QWidget *parent, const char *name) : QDockWindow(QDockWindow::InDock, parent, name)
{
  m_tab = new QTabWidget(this);
  m_tab->setTabShape(QTabWidget::Triangular);

  /* Translate */
  mTranslate = new QGroupBox(5, Qt::Vertical, m_tab);
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
  m_tab->insertTab(mTranslate, i18n("T"));

  /* Rotate */
  mRotate = new QGroupBox(4, Qt::Vertical, m_tab);
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
  m_tab->insertTab(mRotate, i18n("R"));

  setWidget(m_tab);
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
  if(m_tab->currentPage() == mTranslate)
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
  else if(m_tab->currentPage() == mRotate)
  {
	mRRelative = toggled;
  }
}

void TransformPanel::slotDupPressed()
{
  KMacroCommand *c = new KMacroCommand(i18n("Transform Duplicate"));
  c->addCommand(new DuplicateCmd(mPage->document()));
  if(m_tab->currentPage() == mTranslate)
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
  else if(m_tab->currentPage() == mRotate)
    c->addCommand(new RotateCmd(mPage->document(), mHandle->rotCenter(), mAngle->value()));
  emit changeTransform(c);
}

void TransformPanel::slotApplyPressed()
{
  TransformationCmd *c = 0;
  if(m_tab->currentPage() == mTranslate)
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
  else if(m_tab->currentPage() == mRotate)
    c = new RotateCmd(mPage->document(), mHandle->rotCenter(), mAngle->value());

  if(c)
    emit changeTransform(c);
}

#include "TransformPanel.moc"
