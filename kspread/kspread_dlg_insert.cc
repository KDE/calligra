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

#include "kspread_dlg_insert.h"
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
#include <kdebug.h>

KSpreadinsert::KSpreadinsert( KSpreadView* parent, const char* name,const QPoint &_marker,Mode _mode)
	: QDialog( parent, name,TRUE )
{
  m_pView = parent;
  marker=_marker;
  insRem=_mode;


  QVBoxLayout *lay1 = new QVBoxLayout( this );
  lay1->setMargin( 5 );
  lay1->setSpacing( 10 );

  QButtonGroup *grp = new QButtonGroup( 1, QGroupBox::Horizontal, i18n("Insert"),this);
  grp->setRadioButtonExclusive( TRUE );
  grp->layout();
  lay1->addWidget(grp);
  if( insRem==Insert)
  {
    rb1 = new QRadioButton( i18n("Move towards right"), grp );
    rb2 = new QRadioButton( i18n("Move towards bottom"), grp );
    rb3 = new QRadioButton( i18n("Insert row"), grp );
    rb4 = new QRadioButton( i18n("Insert column"), grp );
    setCaption( i18n("Insert cell") );
  }
  else if(insRem==Remove)
  {
    grp->setTitle(i18n("Remove"));
    rb1 = new QRadioButton( i18n("Move towards left"), grp );
    rb2 = new QRadioButton( i18n("Move towards top"), grp );
    rb3 = new QRadioButton( i18n("Remove row"), grp );
    rb4 = new QRadioButton( i18n("Remove column"), grp );
    setCaption( i18n("Remove cell") );
  }
  else
    kdDebug(36001) << "Error in kspread_dlg_insert" << endl;

  rb1->setChecked(true);


  KButtonBox *bb = new KButtonBox( this );
  bb->addStretch();
  m_pOk = bb->addButton( i18n("OK") );
  m_pOk->setDefault( TRUE );
  m_pClose = bb->addButton( i18n( "Close" ) );
  bb->layout();
  lay1->addWidget( bb );

  connect( m_pOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
  connect( m_pClose, SIGNAL( clicked() ), this, SLOT( slotClose() ) );
}


void KSpreadinsert::slotOk()
{
  if(rb1->isChecked())
  {
    if(insRem==Insert)
    {
      m_pView->activeTable()->insertRightCell(marker);
      refresh(insertCellColumn);
    }
    else if(insRem==Remove)
    {
      m_pView->activeTable()->removeLeftCell(marker);
      refresh(removeCellColumn);
    }
  }
  else if(rb2->isChecked())
  {
    if(insRem==Insert)
    {
      m_pView->activeTable()->insertBottomCell(marker);
      refresh(insertCellRow);
    }
    else if(insRem==Remove)
    {
      m_pView->activeTable()->removeTopCell(marker);
      refresh(removeCellRow);
    }
  }
  else if(rb3->isChecked())
  {
    if(insRem==Insert)
    {
      m_pView->activeTable()->insertRow(marker.y());
      refresh(inserRow);
    }
    else if(insRem==Remove)
    {
      m_pView->activeTable()->deleteRow( marker.y());
      refresh(removeRow);
    }
  }
  else if(rb4->isChecked())
  {
    if(insRem==Insert)
    {
      m_pView->activeTable()->insertColumn(marker.x());
      refresh(insertColumn);
    }
    else if(insRem==Remove)
    {
      m_pView->activeTable()->deleteColumn( marker.x());
      refresh(removeColumn);
    }
  }
  else
  {
    kdDebug(36001) << "Error in kspread_dlg_insert" << endl;
  }
  accept();
}

void KSpreadinsert::refresh(Type_mode insertOrRemove)
{
  KSpreadTable *m_pTable;
  m_pTable=m_pView->activeTable();
  KSpreadTable *tbl;

  for ( tbl = m_pView->doc()->map()->firstTable(); tbl != 0L; tbl = m_pView->doc()->map()->nextTable() )
    tbl->recalc(true);
  QListIterator<KSpreadTable> it( m_pTable->map()->tableList() );
  if(insertOrRemove==insertColumn)
  {
    for( ; it.current(); ++it )
      it.current()->changeNameCellRef(marker.x(),KSpreadTable::ColumnInsert,m_pTable->name());
  }
  else if(insertOrRemove==inserRow)
  {
    for( ; it.current(); ++it )
      it.current()->changeNameCellRef(marker.y(),KSpreadTable::RowInsert,m_pTable->name());
  }
  else if(insertOrRemove==removeRow)
  {
    for( ; it.current(); ++it )
      it.current()->changeNameCellRef(marker.y(),KSpreadTable::RowRemove,m_pTable->name());
  }
  else if(insertOrRemove==removeColumn)
  {
    for( ; it.current(); ++it )
      it.current()->changeNameCellRef(marker.x(),KSpreadTable::ColumnRemove,m_pTable->name());  		
  }
  else if(insertOrRemove==insertCellColumn)
  {
    for( ; it.current(); ++it )
      it.current()->changeNameCellRef2(marker,KSpreadTable::ColumnInsert,m_pTable->name());          	
  }
  else if(insertOrRemove==insertCellRow)
  {
    for( ; it.current(); ++it )
      it.current()->changeNameCellRef2(marker,KSpreadTable::RowInsert,m_pTable->name());
  }
  else if(insertOrRemove==removeCellColumn)
  {
    for( ; it.current(); ++it )
      it.current()->changeNameCellRef2(marker,KSpreadTable::ColumnRemove,m_pTable->name());
  }
  else if(insertOrRemove==removeCellRow)
  {
    for( ; it.current(); ++it )
      it.current()->changeNameCellRef2(marker,KSpreadTable::RowRemove,m_pTable->name());
  }


  KSpreadCell *cell = m_pTable->cellAt( marker.x(),marker.y()  );
  if ( cell->text() != 0L )
    m_pView->editWidget()->setText( cell->text() );
  else
    m_pView->editWidget()->setText( "" );
}

void KSpreadinsert::slotClose()
{
  reject();
}


#include "kspread_dlg_insert.moc"
