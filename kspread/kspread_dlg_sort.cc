/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999 Montel Laurent <montell@club-internet.fr>

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

#include "kspread_dlg_sort.h"
#include "kspread_view.h"
#include "kspread_doc.h"
#include "kspread_table.h"
#include "kspread_util.h"
#include <qlayout.h>
#include <kapp.h>
#include <klocale.h>
#include <qrect.h>
#include <kdebug.h>


KSpreadsort::KSpreadsort( KSpreadView* parent, const char* name)
	: QDialog( parent, name,TRUE )
{
  m_pView = parent;
  setCaption( i18n("Sort") );
  QVBoxLayout *lay1 = new QVBoxLayout( this );
  lay1->setMargin( 5 );
  lay1->setSpacing( 10 );
  QGridLayout *lay2 = new QGridLayout( lay1,3,2 );
  lay2->setSpacing( 15 );

  QButtonGroup *grp = new QButtonGroup( 1, QGroupBox::Horizontal, i18n("Sort by"),this);
  grp->setRadioButtonExclusive( TRUE );
  grp->layout();
  lay2->addWidget(grp,0,0);
  rb_row = new QRadioButton( i18n("Row"), grp );
  rb_column = new QRadioButton( i18n("Column"), grp );


  combo=new QComboBox(this);
  lay2->addWidget(combo,0,1);

  decrease=new QCheckBox(i18n("Decrease mode"),this);
  lay2->addWidget(decrease,1,0);

  m_pOk = new QPushButton( i18n("Sort"), this );

  lay2->addWidget(m_pOk,2,0);
  m_pClose = new QPushButton( i18n("Close"), this );
  lay2->addWidget(m_pClose,2,1);
  init();

  connect( m_pOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
  connect( m_pClose, SIGNAL( clicked() ), this, SLOT( slotClose() ) );
  connect( grp, SIGNAL(pressed(int)),this,SLOT(slotpress(int)));
}

void KSpreadsort::init()
{
  r=QRect( m_pView->activeTable()-> selectionRect() );

  // Selection is only one row
  if(r.top()==r.bottom())
  {
    rb_column->setEnabled(false);
    rb_row->setChecked(true);
  }
  // only one column
  else if(r.left()==r.right())
  {
    rb_row->setEnabled(false);
    rb_column->setChecked(true);
  }
  else
  {
    rb_column->setChecked(true);
  }

  for(int i=r.left();i<=r.right();i++)
    list_column += i18n("Column %1").arg(util_columnLabel(i));

  for(int i=r.top();i<=r.bottom();i++)
    list_row += i18n("Row %1").arg(i);

  if ( rb_row->isChecked() )
    slotpress(0);
  else
    slotpress(1);
}

void KSpreadsort::slotpress(int id)
{
switch(id)
	{
	case 0 :
		combo->clear();
		combo->insertStringList(list_row);
		break;
	case 1 :
		combo->clear();
		combo->insertStringList(list_column);
		break;
	default :
	        kdDebug(36001) << "Error in signal : pressed(int id)" << endl;
		break;
	}

}

void KSpreadsort::slotOk()
{
  if( rb_row->isChecked())
  {
    if(!decrease->isChecked())
    {
      m_pView->activeTable()->sortByRow(combo->currentItem()+r.top());
    }
    else
    {
      m_pView->activeTable()->sortByRow(combo->currentItem()+r.top(),KSpreadTable::Decrease);
    }

  }
  else if(rb_column->isChecked())
  {
    if(!decrease->isChecked())
    {
      m_pView->activeTable()->sortByColumn(combo->currentItem()+r.left());
    }
    else
    {
      m_pView->activeTable()->sortByColumn(combo->currentItem()+r.left(),KSpreadTable::Decrease);
    }
  }
  else
  {
    kdDebug(36001) << "Err in radiobutton" << endl;
  }

  accept();
}


void KSpreadsort::slotClose()
{
    reject();
}

#include "kspread_dlg_sort.moc"
