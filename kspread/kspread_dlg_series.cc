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

  QGridLayout *grid1 = new QGridLayout(page,3,2,15,7);

  QButtonGroup* gb1 = new QButtonGroup( i18n("Mode"), page );
  QGridLayout *grid3 = new QGridLayout(gb1,2,2,15,7);
  column = new QRadioButton( i18n("Column"), gb1 );
  column->resize( column->sizeHint() );
  grid3->addWidget(column,0,0);

  row = new QRadioButton( i18n("Row"), gb1 );
  row->resize( row->sizeHint() );
  grid3->addWidget(row,1,0);

  column->setChecked(true);


  QButtonGroup* gb2 = new QButtonGroup( i18n("Type"), page );
  QGridLayout *grid4 = new QGridLayout(gb2,2,2,15,7);
  linear = new QRadioButton( i18n("Linear"), gb2 );
  linear->resize( linear->sizeHint() );
  grid4->addWidget(linear,0,0);

  geometric = new QRadioButton( i18n("Geometric"), gb2 );
  geometric->resize( geometric->sizeHint() );
  grid4->addWidget(geometric,1,0);

  linear->setChecked(true);


  QButtonGroup* gb = new QButtonGroup( i18n("Parameters"), page );
  QGridLayout *grid2 = new QGridLayout(gb,2,4,15,7);

  QLabel *tmplabel = new QLabel( i18n( "Start value" ), gb );
  tmplabel->resize( tmplabel->sizeHint() );
  grid2->addWidget(tmplabel,0,0);

  start=new QLineEdit(gb);
  start->resize( start->sizeHint() );
  grid2->addWidget(start,1,0);
  start->setValidator( new KFloatValidator( 0, 0, true, start ) );

  tmplabel = new QLabel( i18n( "End value" ), gb );
  tmplabel->resize( tmplabel->sizeHint() );
  grid2->addWidget(tmplabel,0,1);

  end=new QLineEdit(gb);
  end->resize( end->sizeHint() );
  grid2->addWidget(end,1,1);
  end->setValidator( new KFloatValidator( 0, 0, true, end ) );

  tmplabel = new QLabel( i18n( "Step" ), gb );
  tmplabel->resize( tmplabel->sizeHint() );
  grid2->addWidget(tmplabel,0,2);

  step=new QLineEdit(gb);
  step->resize( step->sizeHint() );
  grid2->addWidget(step,1,2);
  step->setValidator( new KFloatValidator( 0, 0, true, step ) );

  grid2->setColStretch(0,20);
  grid2->activate();

  grid1->addWidget(gb1,0,0);
  grid1->addRowSpacing(0,gb1->height());

  grid1->addWidget(gb2,0,1);
  grid1->addRowSpacing(0,gb2->height());

  grid1->addMultiCellWidget(gb,1,1,0,1);
  grid1->addRowSpacing(1,gb->height());

  grid1->activate();

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
      if ( dstart > dend )
      {
        KMessageBox::error( this, i18n("End value must be greater than the start value!") );
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
      else if (geometric->isChecked() && dstep <= 1)
      {
        KMessageBox::error( this, i18n("The step value must be greater than one. "
                                       "Otherwise the geometric series is infinite!") );
        step->setFocus();
        return;
      }
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
