/* This file is part of the KDE project
   Copyright (C) 2002-2004 Ariya Hidayat <ariya@kde.org>
             (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 1999-2002 Laurent Montel <montel@kde.org>
             (C) 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2000-2001 Werner Trobin <trobin@kde.org>
             (C) 1998-2000 Torben Weis <weis@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/


#include "kspread_dlg_series.h"
#include "kspread_doc.h"
#include "kspread_editors.h"
#include "kspread_sheet.h"
#include "kspread_view.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QGridLayout>
#include <klocale.h>
#include <QLabel>

#include <q3buttongroup.h>

#include <kmessagebox.h>
#include <knumvalidator.h>

#include <QRadioButton>
#include <QCheckBox>
#include <QLineEdit>

#include <knuminput.h>

using namespace KSpread;

SeriesDlg::SeriesDlg( View* parent, const char* name,const QPoint &_marker)
  : KDialog( parent )
{
  setCaption( i18n("Series") );
  setButtons( Ok|Cancel );
  setModal( true );
  setObjectName( name );

  m_pView = parent;
  marker=_marker;
  QWidget *page = new QWidget();
  setMainWidget( page );

  QHBoxLayout *grid1 = new QHBoxLayout(page);
  grid1->setSpacing( spacingHint() );

  Q3ButtonGroup* gb1 = new Q3ButtonGroup( 2, Qt::Vertical,
    i18n("Insert Values"), page );
  column = new QRadioButton( i18n("Vertical"), gb1 );
  column->setWhatsThis( i18n("Insert the series vertically, one below the other") );
  row = new QRadioButton( i18n("Horizontal"), gb1 );
  row->setWhatsThis( i18n("Insert the series horizontally, from left to right") );

  column->setChecked(true);

  Q3ButtonGroup* gb2 = new Q3ButtonGroup( 2, Qt::Vertical,
    i18n("Type"), page );
  linear = new QRadioButton( i18n("Linear (2,4,6,...)"), gb2 );
  linear->setWhatsThis( i18n("Generate a series from 'start' to 'end' and for each step add "
     "the value provided in step. This creates a series where each value "
     "is 'step' larger than the value before it.") );
  geometric = new QRadioButton( i18n("Geometric (2,4,8,...)"), gb2 );
  geometric->setWhatsThis( i18n("Generate a series from 'start' to 'end' and for each step multiply "
     "the value with the value provided in step. Using a step of 5 produces a list like: "
      "5, 25, 125, 625 since 5 multiplied by 5 (step) equals 25, and that multiplied by 5 equals 125, "
      "which multiplied by the same step-value of 5 equals 625.") );

  linear->setChecked(true);

  QGroupBox* gb = new QGroupBox( /*1, Qt::Vertical, */i18n("Parameters"), page );
  QWidget *params = new QWidget( gb );
  QGridLayout *params_layout = new QGridLayout( params );
  params_layout->setSpacing( spacingHint() );

  QLabel* label = new QLabel( i18n( "Start value:" ), params );
  params_layout->addWidget(label);
  start=new KDoubleNumInput(-999999.999, 999999.99, 0.0, params, 1.0, 3);
  params_layout->addWidget(start);

  label = new QLabel( i18n( "Stop value:" ), params );
  params_layout->addWidget(label);
  end=new KDoubleNumInput(-999999.999, 999999.99, 0.0, params, 1.0, 3);
  params_layout->addWidget(end);

  label = new QLabel( i18n( "Step value:" ), params );
  params_layout->addWidget(label);
  step=new KDoubleNumInput(-999999.999, 999999.99, 0.0, params, 1.0, 3);
  params_layout->addWidget(step);

  grid1->addWidget(gb);

  grid1->addWidget(gb1);
  grid1->addWidget(gb2);

  start->setFocus();

  connect( this, SIGNAL( okClicked() ), this, SLOT( slotOk() ) );
}


void SeriesDlg::slotOk()
{

  Series mode=Column;  //same as Vertical
  Series type=Linear;  // same as Horizontal
  QString tmp;
  double dstep, dend, dstart;
  Sheet * m_pSheet;
  m_pSheet = m_pView->activeSheet();

  if(column->isChecked())
    mode = Column;
  else if(row->isChecked())
    mode = Row;

  if (linear->isChecked())
    type = Linear;
  else if (geometric->isChecked())
    type = Geometric;

  dstart = start->value();
  dend= end->value();
  dstep = step->value();
  if ( type == Geometric )
  {
      if  ( dstart < 0 || dend < 0 )
      {
          KMessageBox::error( this, i18n("End and start value must be positive.") );
          return;
      }
      if ( dstart > dend && dstep >= 1)
      {
        KMessageBox::error( this, i18n("End value must be greater than the start value or the step must be less than '1'.") );
        return;
      }
      if ( dstart == 0 || dend == 0 || dstep == 0)
      {
        KMessageBox::error( this, i18n("None of the Start, Stop or Step values may be equal to zero.") );
        return;
      }
      if ( dstep == 1)
      {
        KMessageBox::error( this, i18n("Step value must be different from 1") );
        return;
      }
  }

  if (dstep >= 0)
  {
      if (linear->isChecked() && dstep == 0)
      {
          KMessageBox::error( this, i18n("The step value must be greater than zero; "
                                         "otherwise, the linear series is infinite.") );
          step->setFocus();
          return;
      }
      /*      else if (geometric->isChecked() && dstep <= 1)
              {
              KMessageBox::error( this, i18n("The step value must be greater than one; "
                                       "otherwise, the geometric series is infinite.") );
                                       step->setFocus();
                                       return;
                                       }
      */
      else if ( type == Linear && dend < dstart )
      {
          KMessageBox::error( this,
                              i18n("If the start value is greater than the end value the step must be less than zero.") );
          return;
      }
  }
  else if (type != Linear)
  {
      KMessageBox::error( this, i18n("Step is negative.") );
      return;
  }
  else
  {
      if (dstart <= dend)
      {
        KMessageBox::error( this,
                            i18n("If the step is negative, the start value must be greater then the end value.") );
        return;
      }
  }

  //        double val_end = qMax(dend, dstart);
  //        double val_start = qMin(dend, dstart);
  m_pView->doc()->emitBeginOperation( false );

  m_pSheet->setSeries( marker, dstart, dend, dstep, mode, type );

  Cell * cell = m_pSheet->cellAt( marker.x(), marker.y() );
  if ( cell->text() != 0 )
    m_pView->editWidget()->setText( cell->text() );
  else
    m_pView->editWidget()->setText( "" );

  m_pView->slotUpdateView( m_pView->activeSheet() );
  accept();
}


#include "kspread_dlg_series.moc"
