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
#include "kspread_tabbar.h"
#include <qlayout.h>
#include <klocale.h>
#include <qlistbox.h>


KSpreadshow::KSpreadshow( KSpreadView* parent, const char* name )
	: KDialogBase( parent, name,TRUE,i18n("Select Hidden Sheet to Show"),Ok|Cancel )
{
  m_pView = parent;
  QWidget *page = new QWidget( this );
  setMainWidget(page);
  QVBoxLayout *lay1 = new QVBoxLayout( page, 0, spacingHint() );


  list=new QListBox(page);
  lay1->addWidget( list );

  QString text;
  QStringList::Iterator it;
  QStringList tabsList=m_pView->tabBar()->listhide();
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

}

void KSpreadshow::slotDoubleClicked(QListBoxItem *)
{
    slotOk();
}



void KSpreadshow::slotOk()
{
  QString text;
  if(list->currentItem()!=-1)
	{
	text=list->text(list->currentItem());
        m_pView->tabBar()->showTable(text);
        }
  accept();
}

#include "kspread_dlg_show.moc"
