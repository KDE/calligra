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
#include "kspread_doc.h"
#include "kspread_table.h"
#include "kspread_map.h"
#include <qlayout.h>
#include <kapp.h>
#include <klocale.h>
#include <kbuttonbox.h>
#include <qbuttongroup.h>
#include <knumvalidator.h>
KSpreadseries::KSpreadseries( KSpreadView* parent, const char* name,const QPoint &_marker)
	: QDialog( parent, name )
{
  m_pView = parent;
  marker=_marker;


  setCaption( i18n("Series") );
  QGridLayout *grid1 = new QGridLayout(this,2,2,15,7);
  QButtonGroup* gb = new QButtonGroup( i18n("Linear series"), this );

  QGridLayout *grid2 = new QGridLayout(gb,2,4,15,7);
  
  column = new QRadioButton( i18n("Column"), gb );
  column->resize( column->sizeHint() );
  grid2->addWidget(column,0,0);
  
  row = new QRadioButton( i18n("Row"), gb );
  row->resize( row->sizeHint() );
  grid2->addWidget(row,1,0);

  column->setChecked(true);
  
  
  QLabel *tmplabel = new QLabel( i18n( "Start value" ), gb );
  tmplabel->resize( tmplabel->sizeHint() );
  grid2->addWidget(tmplabel,0,1);

  start=new QLineEdit(gb);
  start->resize( start->sizeHint() );
  grid2->addWidget(start,1,1);
  start->setValidator( new KIntValidator( start ) );
  
  tmplabel = new QLabel( i18n( "End value" ), gb );
  tmplabel->resize( tmplabel->sizeHint() );
  grid2->addWidget(tmplabel,0,2);

  end=new QLineEdit(gb);
  end->resize( end->sizeHint() );
  grid2->addWidget(end,1,2);
  end->setValidator( new KIntValidator( end ) );

  tmplabel = new QLabel( i18n( "Step" ), gb );
  tmplabel->resize( tmplabel->sizeHint() );
  grid2->addWidget(tmplabel,0,3);

  step=new QLineEdit(gb);
  step->resize( step->sizeHint() );
  grid2->addWidget(step,1,3);
  step->setValidator( new KIntValidator( step ) );

  KButtonBox *bb = new KButtonBox( this );
  bb->addStretch();
  m_pOk = bb->addButton( i18n("OK") );
  m_pOk->setDefault( TRUE );
  m_pClose = bb->addButton( i18n( "Close" ) );
  bb->layout();
  grid1->addWidget( bb,1,0 );
  grid2->setColStretch(0,20);
  grid2->activate();
  grid1->addWidget(gb,0,0);
  grid1->addRowSpacing(0,gb->height());

  grid1->activate();

  connect( m_pOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
  connect( m_pClose, SIGNAL( clicked() ), this, SLOT( slotClose() ) );
}


void KSpreadseries::slotOk()
{

Series mode=Column;
QString tmp;
KSpreadTable *m_pTable;
m_pTable=m_pView->activeTable();

if(column->isChecked())
	mode=Column;
else if(row->isChecked())
	mode=Row;
if(step->text().isEmpty()||start->text().isEmpty()||end->text().isEmpty())
	{
	QMessageBox::warning( 0L, i18n("Error"), i18n("Area text is empty!"),
			   i18n("Ok") );
	}
else
	{
	if(step->text().toInt()>=0)
		{
		int val_end=QMAX(end->text().toInt(),start->text().toInt());
		int val_start=QMIN(end->text().toInt(),start->text().toInt());
		m_pTable->setSeries( marker,val_start,val_end,step->text().toInt(),mode );

		KSpreadCell *cell = m_pTable->cellAt( marker.x(),marker.y()  );
		if ( cell->text() != 0L )
			m_pView->editWidget()->setText( cell->text() );
		else
			m_pView->editWidget()->setText( "" );
		accept();
		}
	else
		{
	 	QMessageBox::warning( 0L, i18n("Error"), i18n("Step is negative !"),
			   i18n("Ok") );
		}
	}
}


void KSpreadseries::slotClose()
{
reject();
}


#include "kspread_dlg_series.moc"
