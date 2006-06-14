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

#include <kspread_editors.h>

#include "SolverDialog.h"

using namespace KSpread::Plugins;

SolverDialog::SolverDialog( View* view, QWidget* parent )
  : KDialog( parent )
{
    setCaption( i18n("Function Optimizer") );
    setButtons( KDialog::Ok|KDialog::Cancel|KDialog::Details );
  setAttribute( Qt::WA_DeleteOnClose );

  QWidget* widget = new QWidget( this );
  Ui::Solver::setupUi( widget );
  setMainWidget( widget );
  setModal( false );
  function->setView( view );
  function->setDialog( this );
  parameters->setView( view );
  parameters->setDialog( this );

  QWidget* detailsWidget = new QWidget( this );
  Ui::SolverDetails::setupUi( detailsWidget );
  setDetailsWidget( detailsWidget );

  connect( this, SIGNAL( okClicked() ),
           this, SLOT( finishDialog() ) );
  connect( this, SIGNAL( cancelClicked() ),
           this, SLOT( finishDialog() ) );
}

void SolverDialog::finishDialog()
{
  deleteLater();
}

#include "SolverDialog.moc"
