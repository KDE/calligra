/* This file is part of the KDE project
   Copyright (C) 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#ifndef KSPREAD_SOLVER_DIALOG
#define KSPREAD_SOLVER_DIALOG

#include <kdialog.h>

#include "ui_SolverMainWidget.h"
#include "ui_SolverDetailsWidget.h"

class QCloseEvent;
class QWidget;

namespace KSpread
{
class View;

namespace Plugins
{

class SolverDialog : public KDialog, public Ui::Solver, public Ui::SolverDetails
{
  Q_OBJECT

public:
  SolverDialog( View* view, QWidget* parent );

protected:
  void closeEvent( QCloseEvent* );

protected Q_SLOTS:
  void finishDialog();
};

} // namespace Plugins
} // namespace KSpread

#endif
