/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000 theKompany.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include "kivio_dlg_pageshow.h"
#include "kivio_view.h"
#include "kivio_canvas.h"
#include "kivio_tabbar.h"
#include "kivio_page.h"

#include <kapp.h>
#include <klocale.h>
#include <qstringlist.h>
#include <qlayout.h>
#include <kbuttonbox.h>
#include <qstrlist.h>
#include <qlist.h>


KivioPageShow::KivioPageShow( KivioView* parent, const char* name )
: QDialog( parent, name,TRUE )
{
  m_pView = parent;


  QVBoxLayout *lay1 = new QVBoxLayout( this );
  lay1->setMargin( 5 );
  lay1->setSpacing( 10 );
  list=new QListBox(this);
  lay1->addWidget( list );

  setCaption( i18n("Hidden pages") );

  KButtonBox *bb = new KButtonBox( this );
  bb->addStretch();
  m_pOk = bb->addButton( i18n("OK") );
  m_pOk->setDefault( TRUE );
  m_pClose = bb->addButton( i18n( "Close" ) );
  bb->layout();
  lay1->addWidget( bb );
  QString text;
  QStringList::Iterator it;
  QStringList tabsList=m_pView->tabBar()->listhide();
  for ( it = tabsList.begin(); it != tabsList.end(); ++it )
    	{
    	text=*it;
    	list->insertItem(text);
    	}
  if(!list->count())
  	m_pOk->setEnabled(false);
  connect( m_pOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
  connect( m_pClose, SIGNAL( clicked() ), this, SLOT( slotClose() ) );
  connect( list, SIGNAL(doubleClicked(QListBoxItem *)),this,SLOT(slotDoubleClicked(QListBoxItem *)));
  resize( 200, 150 );

}

void KivioPageShow::slotDoubleClicked(QListBoxItem *)
{
  slotOk();
}

void KivioPageShow::slotOk()
{
  QString text;
  if (list->currentItem()!=-1) {
    text=list->text(list->currentItem());
    m_pView->tabBar()->showPage(text);
  }
  accept();
}

void KivioPageShow::slotClose()
{
  reject();
}
#include "kivio_dlg_pageshow.moc"
