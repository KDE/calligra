/* This file is part of the KDE project
   Copyright (C) 2005 Peter Simonsson <psn@linux.se>,

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
#include "addstencilsetdialog.h"

#include <klocale.h>

#include "kivioaddstencilsetpanel.h"

namespace Kivio {

AddStencilSetDialog::AddStencilSetDialog(QWidget *parent, const char *name)
  : KDialogBase(parent, name, false, i18n("Stencil Sets"), KDialogBase::Close)
{
  m_mainWidget = new AddStencilSetPanel(this);
  setMainWidget(m_mainWidget);
  setInitialSize(QSize(600, 450));

  connect(m_mainWidget, SIGNAL(addStencilSet(const QString&)), this, SIGNAL(addStencilSet(const QString&)));
}

AddStencilSetDialog::~AddStencilSetDialog()
{
}

void AddStencilSetDialog::updateList()
{
  m_mainWidget->updateList();
}

}

#include "addstencilsetdialog.moc"
