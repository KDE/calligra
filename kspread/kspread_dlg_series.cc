/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999, 2000 Montel Laurent <montell@club-internet.fr>
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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/


#include "kspread_dlg_series.h"
#include "kspread_canvas.h"
#include "kspread_table.h"
#include <qlayout.h>
#include <klocale.h>
#include <qlabel.h>

#include <qbuttongroup.h>
#include <qgroupbox.h>
#include <kmessagebox.h>
#include <knumvalidator.h>

#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qlineedit.h>


KSpreadSeriesDlg::KSpreadSeriesDlg( KSpreadView* parent, const char* name,const QPoint &_marker)
  : KDialogBase( parent, name,TRUE,i18n("Series"),Ok|Cancel )
{
  m_pView = parent;
  marker=_marker;
  QWidget *page = new QWidget( this );
  setMainWidget(page);

  QBoxLayout *grid1 = new QHBoxLayout(page);
  grid1->setSpacing( spacingHint() );

  QButtonGroup* gb1 = new QButtonGroup( 2, Qt::Vertical, 
    i18n("Insert values"), page );
  column = new QRadioButton( i18n("Vertical"), gb1 );
  row = new QRadioButton( i18n("Horizontal"), gb1 );

  column->setChecked(true);

  QButtonGroup* gb2 = new QButtonGroup( 2, Qt::Vertical, 
    i18n("Type"), page );
  linear = new QRadioButton( i18n("Linear (2,4,6,...)"), gb2 );
  geometric = new QRadioButton( i18n("Geometric (2,4,8,...)"), gb2 );

  linear->setChecked(true);

  QGroupBox* gb = new QGroupBox( 1, Qt::Vertical, i18n("Parameters"), page );
  QWidget *params = new QWidget( gb );
  QGridLayout *params_layout = new QGridLayout( params, 3, 2 );
  params_layout->setSpacing( spacingHint() );
  params_layout->setAutoAdd( true );

  new QLabel( i18n( "Start value:" ), params );
  start=new QLineEdit( params );
  start->setValidator( new KFloatValidator( 0, 0, true, start ) );

  new QLabel( i18n( "Stop value:" ), params );
  end=new QLineEdit( params );
  end->setValidator( new KFloatValidator( 0, 0, true, end ) );

  new QLabel( i18n( "Step value:" ), params );
  step=new QLineEdit( params );
  step->setValidator( new KFloatValidator( 0, 0, true, step ) );

  grid1->addWidget(gb);

  grid1->addWidget(gb1);
  grid1->addWidget(gb2);

  start->setFocus();

  connect( this, SIGNAL( okClicked() ), this, SLOT( slotOk() ) );
}


void KSpreadSeriesDlg::slotOk()
{

  Series mode=Column;
  Series type=Linear;
  QString tmp;
  double dstep, dend, dstart;
  KSpreadTable * m_pTable;
  m_pTable = m_pView->activeTable();

  if(column->isChecked())
    mode = Column;
  else if(row->isChecked())
    mode = Row;

  if (linear->isChecked())
    type = Linear;
  else if (geometric->isChecked())
    type = Geometric;

  if (step->text().isEmpty() || start->text().isEmpty() || end->text().isEmpty())
  {
    KMessageBox::error( this, i18n("Area text is empty!") );
    return;
  }
  else
  {
    bool ok;
    dstart = KGlobal::locale()->readNumber(start->text(), &ok);
    if (!ok)
    {
      KMessageBox::error(this, i18n("The start value is not a valid number!"));
      start->setFocus();
      return;
    }

    dend   = KGlobal::locale()->readNumber(end->text(),   &ok);
    if (!ok)
    {
      KMessageBox::error(this, i18n("The end value is not a valid number!"));
      end->setFocus();
      return;
    }

    dstep  = KGlobal::locale()->readNumber(step->text(),  &ok);
    if (!ok)
    {
      KMessageBox::error(this, i18n("The step value is not a valid number!"));
      step->setFocus();
      return;
    }

    if ( type == Geometric )
    {
      if  ( dstart < 0 || dend < 0 )
      {
        KMessageBox::error( this, i18n("End and start value must be positive!") );
        return;
      }
      if ( dstart > dend && dstep >= 1)
      {
        KMessageBox::error( this, i18n("End value must be greater than the start value or the step must be less than '1'!") );
        return;
      }
    }

    if (dstep >= 0)
    {
      if (linear->isChecked() && dstep == 0)
      {
        KMessageBox::error( this, i18n("The step value must be greater than zero. "
                                       "Otherwise the linear series is infinite!") );
        step->setFocus();
        return;
      }
      /*      else if (geometric->isChecked() && dstep <= 1)
      {
        KMessageBox::error( this, i18n("The step value must be greater than one. "
                                       "Otherwise the geometric series is infinite!") );
        step->setFocus();
        return;
      }
      */
      else if ( type == Linear && dend < dstart )
      {
        KMessageBox::error( this, 
                            i18n("If the start value is greater than the end value the step must be less than zero!") );
        return;
      }
    }
    else if (type != Linear)      
    {
      KMessageBox::error( this, i18n("Step is negative!") );
      return;
    }
    else
    {
      if (dstart <= dend)
      {
        KMessageBox::error( this, 
                            i18n("If the step is negative, the start value must be greater then the end value!") );
        return;        
      }
    } 
  }

  //        double val_end = QMAX(dend, dstart);
  //        double val_start = QMIN(dend, dstart);
  m_pTable->setSeries( marker, dstart, dend, dstep, mode, type );
  
  KSpreadCell * cell = m_pTable->cellAt( marker.x(), marker.y() );
  if ( cell->text() != 0L )
    m_pView->editWidget()->setText( cell->text() );
  else
    m_pView->editWidget()->setText( "" );
  
  accept();
}


#include "kspread_dlg_series.moc"
