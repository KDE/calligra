/* This file is part of the KDE project
   Copyright (C) 2003 Peter Simonsson <psn@linux.se>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "kivioaligndialog.h"
#include "kivioaligndialog.moc"

#include <qradiobutton.h>
#include <QCheckBox>

#include <klocale.h>

#include "kivio_page.h"
#include "aligndialog.h"

KivioAlignDialog::KivioAlignDialog(QWidget* parent, const char* name)
 : KDialogBase(parent, name, true, i18n("Align & Distribute"), Ok|Cancel, Ok)
{
  m_view = new KivioAlignDialogWidget(this);
  setMainWidget(m_view);
}

AlignData KivioAlignDialog::align()
{
  AlignData ad;

  ad.centerOfPage = m_view->aCenterPage->isChecked();
  ad.v = AlignData::None;

  if (m_view->avtop->isChecked())
    ad.v = AlignData::Top;
  if (m_view->avcenter->isChecked())
    ad.v = AlignData::Center;
  if (m_view->avbottom->isChecked())
    ad.v = AlignData::Bottom;

  ad.h = AlignData::None;
  if (m_view->ahleft->isChecked())
    ad.h = AlignData::Left;
  if (m_view->ahcenter->isChecked())
    ad.h = AlignData::Center;
  if (m_view->ahright->isChecked())
    ad.h = AlignData::Right;

  return ad;
}

DistributeData KivioAlignDialog::distribute()
{
  DistributeData dd;

  if (m_view->dselection->isChecked())
    dd.extent = DistributeData::Selection;
  if (m_view->dpage->isChecked())
    dd.extent = DistributeData::Page;

  dd.v = DistributeData::None;
  if (m_view->dvtop->isChecked())
    dd.v = DistributeData::Top;
  if (m_view->dvcenter->isChecked())
    dd.v = DistributeData::Center;
  if (m_view->dvbottom->isChecked())
    dd.v = DistributeData::Bottom;
  if (m_view->dvspacing->isChecked())
    dd.v = DistributeData::Spacing;

  dd.h = DistributeData::None;
  if (m_view->dhleft->isChecked())
    dd.h = DistributeData::Left;
  if (m_view->dhcenter->isChecked())
    dd.h = DistributeData::Center;
  if (m_view->dhright->isChecked())
    dd.h = DistributeData::Right;
  if (m_view->dhspacing->isChecked())
    dd.h = DistributeData::Spacing;

  return dd;
}
