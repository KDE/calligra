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

#include <qprinter.h>

#include "kspread_dlg_series.h"
#include "kspread_view.h"
#include "kspread_canvas.h"
#include "kspread_table.h"
#include <qlayout.h>
#include <qlabel.h>
#include <kapp.h>
#include <kbuttonbox.h>
#include <qbuttongroup.h>
#include <kmessagebox.h>
#include <knumvalidator.h>

KSpreadSeriesDlg::KSpreadSeriesDlg( KSpreadView* parent, const char* name,const QPoint &_marker)
        : QDialog( parent, name,TRUE )
{
  m_pView = parent;
  marker=_marker;

  setCaption( i18n("Series") );
  QGridLayout *grid1 = new QGridLayout(this,3,2,15,7);

  QButtonGroup* gb1 = new QButtonGroup( i18n("Mode"), this );
  QGridLayout *grid3 = new QGridLayout(gb1,2,2,15,7);
  column = new QRadioButton( i18n("Column"), gb1 );
  column->resize( column->sizeHint() );
  grid3->addWidget(column,0,0);

  row = new QRadioButton( i18n("Row"), gb1 );
  row->resize( row->sizeHint() );
  grid3->addWidget(row,1,0);

  column->setChecked(true);


  QButtonGroup* gb2 = new QButtonGroup( i18n("Type"), this );
  QGridLayout *grid4 = new QGridLayout(gb2,2,2,15,7);
  linear = new QRadioButton( i18n("Linear"), gb2 );
  linear->resize( linear->sizeHint() );
  grid4->addWidget(linear,0,0);

  geometric = new QRadioButton( i18n("Geometric"), gb2 );
  geometric->resize( geometric->sizeHint() );
  grid4->addWidget(geometric,1,0);

  linear->setChecked(true);


  QButtonGroup* gb = new QButtonGroup( i18n("Parameters"), this );
  QGridLayout *grid2 = new QGridLayout(gb,2,4,15,7);

  QLabel *tmplabel = new QLabel( i18n( "Start value" ), gb );
  tmplabel->resize( tmplabel->sizeHint() );
  grid2->addWidget(tmplabel,0,0);

  start=new QLineEdit(gb);
  start->resize( start->sizeHint() );
  grid2->addWidget(start,1,0);
  start->setValidator( new KIntValidator( start ) );

  tmplabel = new QLabel( i18n( "End value" ), gb );
  tmplabel->resize( tmplabel->sizeHint() );
  grid2->addWidget(tmplabel,0,1);

  end=new QLineEdit(gb);
  end->resize( end->sizeHint() );
  grid2->addWidget(end,1,1);
  end->setValidator( new KIntValidator( end ) );

  tmplabel = new QLabel( i18n( "Step" ), gb );
  tmplabel->resize( tmplabel->sizeHint() );
  grid2->addWidget(tmplabel,0,2);

  step=new QLineEdit(gb);
  step->resize( step->sizeHint() );
  grid2->addWidget(step,1,2);
  step->setValidator( new KIntValidator( step ) );


  KButtonBox *bb = new KButtonBox( this );
  bb->addStretch();
  m_pOk = bb->addButton( i18n("OK") );
  m_pOk->setDefault( TRUE );
  m_pClose = bb->addButton( i18n( "Close" ) );
  bb->layout();
  grid1->addWidget( bb,2,1 );
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

  connect( m_pOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
  connect( m_pClose, SIGNAL( clicked() ), this, SLOT( slotClose() ) );
}


void KSpreadSeriesDlg::slotOk()
{

Series mode=Column;
Series type=Linear;
QString tmp;
KSpreadTable *m_pTable;
m_pTable=m_pView->activeTable();

if(column->isChecked())
        mode=Column;
else if(row->isChecked())
        mode=Row;

if(linear->isChecked())
        type=Linear;
else if(geometric->isChecked())
        type=Geometric;

if(step->text().isEmpty()||start->text().isEmpty()||end->text().isEmpty())
        {
        KMessageBox::error( this, i18n("Area text is empty!") );
        }
else
        {
        if(step->text().toInt()>=0)
                {
                if(linear->isChecked() &&step->text().toInt() ==0)
                {
                KMessageBox::error( this, i18n("Sorry, but the step value has to be greater than 0.\n"
                "Otherwise the linear series is infinite!") );
                step->setFocus();
                }
                else if(geometric->isChecked() &&step->text().toInt() <=1)
                {
                KMessageBox::error( this, i18n("Sorry, but the step value has to be greater than one\n"
                "Otherwise the geometric series is infinite!") );
                step->setFocus();
                }
                else
                {
                        int val_end=QMAX(end->text().toInt(),start->text().toInt());
                        int val_start=QMIN(end->text().toInt(),start->text().toInt());
                        m_pTable->setSeries( marker,val_start,val_end,step->text().toInt(),mode,type );

                        KSpreadCell *cell = m_pTable->cellAt( marker.x(),marker.y()  );
                        if ( cell->text() != 0L )
                                m_pView->editWidget()->setText( cell->text() );
                        else
                                m_pView->editWidget()->setText( "" );
                        accept();
                }

                }
        else
                {
                KMessageBox::error( this, i18n("Step is negative !") );
                }
        }
}


void KSpreadSeriesDlg::slotClose()
{
reject();
}


#include "kspread_dlg_series.moc"
