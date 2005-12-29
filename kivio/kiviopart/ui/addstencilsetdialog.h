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
#ifndef KIVIOADDSTENCILSETDIALOG_H
#define KIVIOADDSTENCILSETDIALOG_H

#include <kdialogbase.h>

namespace Kivio {

class AddStencilSetPanel;

class AddStencilSetDialog : public KDialogBase
{
  Q_OBJECT
  public:
    AddStencilSetDialog(QWidget *parent = 0, const char *name = 0);
    ~AddStencilSetDialog();

  public slots:
    void updateList();

  signals:
    void addStencilSet(const QString&);

  private:
    AddStencilSetPanel* m_mainWidget;
};

}

#endif
