/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999,2000,2001 Montel Laurent <lmontel@mandrakesoft.com>

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



#include "kspread_dlg_show.h"
#include "kspread_view.h"
#include "kspread_doc.h"
#include "kspread_map.h"
#include "kspread_tabbar.h"
#include "kspread_undo.h"
#include <qlayout.h>
#include <klocale.h>
#include <qlistbox.h>
#include <qlabel.h>

KSpreadshow::KSpreadshow( KSpreadView* parent, const char* name )
	: KDialogBase( parent, name,TRUE,i18n("Show Sheet"),Ok|Cancel )
{
  m_pView = parent;
  QWidget *page = new QWidget( this );
  setMainWidget(page);
  QVBoxLayout *lay1 = new QVBoxLayout( page, 0, spacingHint() );

  QLabel *label = new QLabel( i18n("Select hidden sheets to show:"), page );
  lay1->addWidget( label );

  list=new QListBox(page);
  lay1->addWidget( list );

  list->setSelectionMode(QListBox::Multi);
  QString text;
  QStringList::Iterator it;
  QStringList tabsList=m_pView->doc()->map()->hiddenSheets();
  for ( it = tabsList.begin(); it != tabsList.end(); ++it )
    	{
    	text=*it;
    	list->insertItem(text);
    	}
  if(!list->count())
  	enableButtonOK(false);
  connect( this, SIGNAL( okClicked() ), this, SLOT( slotOk() ) );
  connect( list, SIGNAL(doubleClicked(QListBoxItem *)),this,SLOT(slotDoubleClicked(QListBoxItem *)));
  resize( 200, 150 );
  setFocus();
}

void KSpreadshow::slotDoubleClicked(QListBoxItem *)
{
    slotOk();
}



void KSpreadshow::slotOk()
{
    m_pView->doc()->emitBeginOperation( false );

    QStringList listTable;

    for (int i=0; i < list->numRows(); i++)
    {
        if (list->isSelected(i))
        {
            listTable.append( list->text(i));
        }
    }

    //m_pView->tabBar()->showTable(listTable);

    if ( listTable.count()==0 )
        return;

    KSpreadSheet *table;
    KSpreadMacroUndoAction *macroUndo=new KSpreadMacroUndoAction( m_pView->doc(),i18n("Show Table"));
    for ( QStringList::Iterator it = listTable.begin(); it != listTable.end(); ++it )
    {
        table=m_pView->doc()->map()->findTable( *it );
        if ( !m_pView->doc()->undoLocked() )
        {
            KSpreadUndoShowTable* undo = new KSpreadUndoShowTable( m_pView->doc(), table );
            macroUndo->addCommand( undo );
        }
        table->hideTable(false);
    }
    m_pView->doc()->addCommand( macroUndo );

    m_pView->slotUpdateView( m_pView->activeTable() );
    accept();
}

#include "kspread_dlg_show.moc"
