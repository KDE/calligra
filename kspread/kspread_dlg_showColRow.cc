/* This file is part of the KDE project
   Copyright (C) 2000 Montel Laurent <lmontel@mandrakesoft.com>

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



#include "kspread_dlg_showColRow.h"
#include "kspread_view.h"
#include "kspread_canvas.h"
#include "kspread_tabbar.h"
#include "kspread_table.h"
#include "kspread_util.h"
#include <kapp.h>
#include <klocale.h>
#include <qstringlist.h>
#include <qlayout.h>
#include <kbuttonbox.h>
#include <qstrlist.h>
#include <qlist.h>
#include <qtl.h>

KSpreadShowColRow::KSpreadShowColRow( KSpreadView* parent, const char* name,ShowColRow _type )
	: QDialog( parent, name,TRUE )
{
  m_pView = parent;
  typeShow=_type;

  QVBoxLayout *lay1 = new QVBoxLayout( this );
  lay1->setMargin( 5 );
  lay1->setSpacing( 10 );
  list=new QListBox(this);
  lay1->addWidget( list );
  if(_type==Column)
        setCaption( i18n("Name of the hidden column:") );
  else if(_type==Row)
        setCaption( i18n("Number of the hidden row:") );

  KButtonBox *bb = new KButtonBox( this );
  bb->addStretch();
  m_pOk = bb->addButton( i18n("OK") );
  m_pOk->setDefault( TRUE );
  m_pClose = bb->addButton( i18n( "Close" ) );
  bb->layout();
  lay1->addWidget( bb );
  bool showColNumber=m_pView->activeTable()->getShowColumnNumber();
  if(_type==Column)
        {
        ColumnLayout *col=m_pView->activeTable()->firstCol();

        QString text;
        QStringList listCol;
        for( ; col; col = col->next() )
	  {
	    if(col->isHide())
	      listInt.append(col->column());
	  }
        qHeapSort(listInt);
        QValueList<int>::Iterator it;
        for( it = listInt.begin(); it != listInt.end(); ++it )
	  {
	    if(!showColNumber)
	      listCol+=i18n("Column: %1").arg(util_columnLabel(*it));
	    else
	      listCol+=i18n("Column: %1").arg(text.setNum(*it));
	  }
        list->insertStringList(listCol);
        }
  else if(_type==Row)
        {
        RowLayout *row=m_pView->activeTable()->firstRow();

        QString text;
        QStringList listRow;
        for( ; row; row = row->next() )
	  {
	    if(row->isHide())
	      listInt.append(row->row());
	  }
        qHeapSort(listInt);
        QValueList<int>::Iterator it;
        for( it = listInt.begin(); it != listInt.end(); ++it )
	  listRow+=i18n("Row: %1").arg(text.setNum(*it));

        list->insertStringList(listRow);
        }

  if(!list->count())
      m_pOk->setEnabled(false);

  //selection multiple
  list->setSelectionMode(QListBox::Multi);
  connect( m_pOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
  connect( m_pClose, SIGNAL( clicked() ), this, SLOT( slotClose() ) );
  connect( list, SIGNAL(doubleClicked(QListBoxItem *)),this,SLOT(slotDoubleClicked(QListBoxItem *)));
  resize( 200, 150 );

}

void KSpreadShowColRow::slotDoubleClicked(QListBoxItem *)
{
    slotOk();
}

void KSpreadShowColRow::slotOk()
{

  QValueList<int>listSelected;
  for(unsigned int i=0;i<list->count();i++)
    {
      if(list->isSelected(i))
	listSelected.append(*listInt.at(i));
    }
  if( typeShow==Column)
    {
      if(listSelected.count()!=0)
	m_pView->activeTable()->showColumn(0,-1,listSelected);
    }
  if( typeShow==Row)
    {
      if(listSelected.count()!=0)
	m_pView->activeTable()->showRow(0,-1,listSelected);
    } 
  accept();
}

void KSpreadShowColRow::slotClose()
{
  reject();
}

#include "kspread_dlg_showColRow.moc"
