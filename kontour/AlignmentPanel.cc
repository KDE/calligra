/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2002 Igor Jansen (rm@kde.org)

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

#include "AlignmentPanel.h"

#include <qtabwidget.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qvbuttongroup.h>
#include <qhbuttongroup.h>
#include <qvgroupbox.h>
#include <qcheckbox.h>
#include <qradiobutton.h>

#include <klocale.h>
#include <kiconloader.h>

AlignmentPanel::AlignmentPanel(KontourView *aView, QWidget *parent, const char *name):
QDockWindow(QDockWindow::InDock, parent, name)
{
  mView = aView;

  mTab = new QTabWidget(this);
  mTab->setTabShape(QTabWidget::Triangular);
  QGridLayout *layout;

  QWidget *mAlign = new QWidget(mTab);

  layout = new QGridLayout(mAlign, 2, 2);

  QButtonGroup *group = new QVButtonGroup(i18n("Vertically"), mAlign, "Vertically");
  group->setExclusive(true);

  valignButton[0] = new QPushButton(group);
  valignButton[0]->setToggleButton(true);
  valignButton[0]->setPixmap(SmallIcon("atop"));

  valignButton[1] = new QPushButton(group);
  valignButton[1]->setToggleButton(true);
  valignButton[1]->setPixmap(SmallIcon("avcenter"));

  valignButton[2] = new QPushButton(group);
  valignButton[2]->setToggleButton(true);
  valignButton[2]->setPixmap(SmallIcon("abottom"));

  layout->addMultiCellWidget(group, 0, 1, 0, 0);

  group = new QHButtonGroup(i18n("Horizontally"), mAlign, "Horizontally");
  group->setExclusive(true);

  halignButton[0] = new QPushButton(group);
  halignButton[0]->setToggleButton(true);
  halignButton[0]->setPixmap(SmallIcon("aleft"));

  halignButton[1] = new QPushButton(group);
  halignButton[1]->setToggleButton(true);
  halignButton[1]->setPixmap(SmallIcon("ahcenter"));

  halignButton[2] = new QPushButton(group);
  halignButton[2]->setToggleButton(true);
  halignButton[2]->setPixmap(SmallIcon("aright"));

  layout->addWidget(group, 0, 1);

  QGroupBox *box = new QVGroupBox(mAlign);

  gbutton = new QCheckBox(i18n("To Grid"), box);
  cbutton = new QCheckBox(i18n("Align To Center of Page"), box);
  layout->addWidget(box, 1, 1);

  mTab->insertTab(mAlign, i18n("Align"));

  QWidget *mDistr = new QWidget(mTab);
  layout = new QGridLayout(mDistr, 2, 2);

  group = new QVButtonGroup(i18n("Vertically"), mDistr, "Vertically");
  group->setExclusive(true);
  layout->addMultiCellWidget(group, 0, 1, 0, 0);

  vdistButton[0] = new QPushButton(group);
  vdistButton[0]->setToggleButton(true);
  vdistButton[0]->setPixmap(SmallIcon("dtop"));

  vdistButton[1] = new QPushButton(group);
  vdistButton[1]->setToggleButton(true);
  vdistButton[1]->setPixmap(SmallIcon("dvcenter"));

  vdistButton[2] = new QPushButton(group);
  vdistButton[2]->setToggleButton(true);
  vdistButton[2]->setPixmap(SmallIcon("dvdist"));

  vdistButton[3] = new QPushButton(group);
  vdistButton[3]->setToggleButton(true);
  vdistButton[3]->setPixmap(SmallIcon("dbottom"));

  group = new QHButtonGroup(i18n("Horizontally"), mDistr, "Horizontally");
  group->setExclusive(true);
  layout->addWidget(group, 0, 1);

  hdistButton[0] = new QPushButton(group);
  hdistButton[0]->setToggleButton(true);
  hdistButton[0]->setPixmap(SmallIcon("dleft"));

  hdistButton[1] = new QPushButton(group);
  hdistButton[1]->setToggleButton(true);
  hdistButton[1]->setPixmap(SmallIcon("dhcenter"));

  hdistButton[2] = new QPushButton(group);
  hdistButton[2]->setToggleButton(true);
  hdistButton[2]->setPixmap(SmallIcon("dhdist"));

  hdistButton[3] = new QPushButton(group);
  hdistButton[3]->setToggleButton(true);
  hdistButton[3]->setPixmap(SmallIcon("dright"));

  group = new QVButtonGroup(i18n("Distribute at"), mDistr);
  layout->addWidget(group, 1, 1);

  sbutton = new QRadioButton(i18n("Selection"), group);
  sbutton->setChecked(true);
  pbutton = new QRadioButton(i18n("Page"), group);

  mTab->insertTab(mDistr, i18n("Distribute"));

  setWidget(mTab);
  setCaption(i18n("Alignment"));
}

#include "AlignmentPanel.moc"
