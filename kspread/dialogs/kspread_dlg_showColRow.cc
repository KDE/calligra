/* This file is part of the KDE project
   Copyright (C) 2003 Norbert Andres <nandres@web.de>
             (C) 2000-2002 Laurent Montel <montel@kde.org>
             (C) 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2002 John Dailey <dailey@vt.edu>

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
#include "kspread_doc.h"
#include "kspread_view.h"
#include "kspread_sheet.h"
#include "kspread_util.h"
#include <qlabel.h>
#include <qlayout.h>
#include <klocale.h>

KSpreadShowColRow::KSpreadShowColRow( KSpreadView* parent, const char* name,ShowColRow _type )
	: KDialogBase( parent, name,TRUE,"",Ok|Cancel )
{
  m_pView = parent;
  typeShow=_type;

  QWidget *page = new QWidget( this );
  setMainWidget(page);
  QVBoxLayout *lay1 = new QVBoxLayout( page, 0, spacingHint() );

  QLabel *label = new QLabel( page );

  if(_type==Column) {
        setCaption( i18n("Show Columns") );
        label->setText(i18n("Select hidden columns to show:"));
  }
  else if(_type==Row) {
        setCaption( i18n("Show Rows") );
        label->setText(i18n("Select hidden rows to show:"));
  }

  list=new QListBox(page);

  lay1->addWidget( label );
  lay1->addWidget( list );

  bool showColNumber=m_pView->activeSheet()->getShowColumnNumber();
  if(_type==Column)
        {
        ColumnFormat *col=m_pView->activeSheet()->firstCol();

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
	      listCol+=i18n("Column: %1").arg(KSpreadCell::columnName(*it));
	    else
	      listCol+=i18n("Column: %1").arg(text.setNum(*it));
	  }
        list->insertStringList(listCol);
        }
  else if(_type==Row)
        {
        RowFormat *row=m_pView->activeSheet()->firstRow();

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
      enableButtonOK(false);

  //selection multiple
  list->setSelectionMode(QListBox::Multi);
  connect( this, SIGNAL( okClicked() ), this, SLOT( slotOk() ) );
  connect( list, SIGNAL(doubleClicked(QListBoxItem *)),this,SLOT(slotDoubleClicked(QListBoxItem *)));
  resize( 200, 150 );
  setFocus();
}

void KSpreadShowColRow::slotDoubleClicked(QListBoxItem *)
{
    slotOk();
}

void KSpreadShowColRow::slotOk()
{
  m_pView->doc()->emitBeginOperation( false );

  QValueList<int>listSelected;
  for(unsigned int i=0;i<list->count();i++)
    {
      if(list->isSelected(i))
	listSelected.append(*listInt.at(i));
    }
  if( typeShow==Column)
    {
      if(listSelected.count()!=0)
	m_pView->activeSheet()->showColumn(0,-1,listSelected);
    }
  if( typeShow==Row)
    {
      if(listSelected.count()!=0)
	m_pView->activeSheet()->showRow(0,-1,listSelected);
    }

  m_pView->slotUpdateView( m_pView->activeSheet() );
  accept();
}

#include "kspread_dlg_showColRow.moc"
