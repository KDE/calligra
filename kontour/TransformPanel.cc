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

#include <qtabwidget.h>
#include <qgroupbox.h>
#include <qcheckbox.h>
#include <qspinbox.h>

#include <klocale.h>
#include <knuminput.h>

TransformPanel::TransformPanel(QWidget *parent, const char *name) : QDockWindow(QDockWindow::InDock, parent, name)
{
  m_tab = new QTabWidget(this);
  m_tab->setTabShape(QTabWidget::Triangular);
  QGroupBox *mTranslate = new QGroupBox(2, Qt::Vertical, m_tab);
  KIntNumInput *mHoriz = new KIntNumInput(0, mTranslate);
  mHoriz->setRange(-1000, 1000, 1, false);
  mHoriz->setLabel(i18n("Horizontal"));
  KIntNumInput *mVert  = new KIntNumInput(0.0, mTranslate);
  mVert->setRange(-1000, 1000, 1, false);
  mVert->setLabel(i18n("Vertical"));
  m_tab->insertTab(mTranslate, i18n("T"));
  setWidget(m_tab);
  setCaption(i18n("Transform"));
}

#include "TransformPanel.moc"
