/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999, 2000, 2001  Montel Laurent <lmontel@mandrakesoft.com>

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



#include "kspread_dlg_reference.h"
#include "kspread_view.h"
#include "kspread_canvas.h"
#include "kspread_table.h"
#include "kspread_doc.h"
#include "kspread_util.h"
#include "kspread_map.h"
#include <kapp.h>
#include <qstringlist.h>
#include <qlayout.h>
#include <kbuttonbox.h>
#include <kmessagebox.h>


KSpreadreference::KSpreadreference( KSpreadView* parent, const char* name )
	: QDialog( parent, name,TRUE )
{
  m_pView = parent;
  QVBoxLayout *lay1 = new QVBoxLayout( this );
  lay1->setMargin( 5 );
  lay1->setSpacing( 10 );
  list=new QListBox(this);
  lay1->addWidget( list );

  setCaption( i18n("Area Name") );

  rangeName=new QLabel(this);
  lay1->addWidget(rangeName);

  m_pRemove=new QPushButton(i18n("Remove"),this);
  lay1->addWidget( m_pRemove );
  //m_pRemove->setEnabled(false);
  KButtonBox *bb = new KButtonBox( this );
  bb->addStretch();
  m_pOk = bb->addButton( i18n("OK") );
  m_pOk->setDefault( TRUE );
  m_pClose = bb->addButton( i18n( "Close" ) );
  bb->layout();
  lay1->addWidget( bb );
  QString text;
  QValueList<Reference>::Iterator it;
  QValueList<Reference> area=m_pView->doc()->listArea();
  for ( it = area.begin(); it != area.end(); ++it )
    	{
    	text=(*it).ref_name;
    	list->insertItem(text);
    	}
  if(!list->count())
  	{
  	m_pOk->setEnabled(false);
  	m_pRemove->setEnabled(false);
  	}
  connect( m_pOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
  connect( m_pClose, SIGNAL( clicked() ), this, SLOT( slotClose() ) );
  connect( m_pRemove, SIGNAL( clicked() ), this, SLOT( slotRemove() ) );
  connect( list, SIGNAL(doubleClicked(QListBoxItem *)),this,SLOT(slotDoubleClicked(QListBoxItem *)));
  connect( list, SIGNAL(highlighted ( QListBoxItem * ) ),this,SLOT(slotHighlighted(QListBoxItem * )));
  rangeName->setText(i18n("area: %1").arg(""));

  resize( 250, 200 );

}

void KSpreadreference::slotHighlighted(QListBoxItem * )
{
  QString tmp=list->text(list->currentItem());
  QString tmpName;
  QValueList<Reference>::Iterator it;
  QValueList<Reference> area=m_pView->doc()->listArea();
  for ( it = area.begin(); it != area.end(); ++it )
    {
      if((*it).ref_name==tmp)
	{
	  tmpName=util_rangeName( m_pView->doc()->map()->findTable( (*it).table_name), (*it). rect );
	  break;
	}
    }

  tmpName=i18n("area: %1").arg(tmpName);

  rangeName->setText(tmpName);
}

void KSpreadreference::slotDoubleClicked(QListBoxItem *)
{
    slotOk();
}

void KSpreadreference::slotRemove()
{
  if(list->currentItem()==-1)
    return;
  int ret = KMessageBox::warningYesNo( this, i18n("Do you want really remove this area name?"));
  if(ret==4) // reponse = No
    return;

  QString textRemove;
  if( list->currentItem()!=-1)
        {
	  QString textRemove=list->text(list->currentItem());
	  m_pView->doc()->removeArea(textRemove );

	  list->clear();
	  QString text;
	  QValueList<Reference>::Iterator it;
	  QValueList<Reference> area=m_pView->doc()->listArea();
	  for ( it = area.begin(); it != area.end(); ++it )
	    {
	      text=(*it).ref_name;
	      list->insertItem(text);
	    }
	  KSpreadTable *tbl;

	  for ( tbl = m_pView->doc()->map()->firstTable(); tbl != 0L; tbl = m_pView->doc()->map()->nextTable() )
	    {
	      tbl->refreshRemoveAreaName(textRemove);
	    }
	}
  if(!list->count())
    {
      m_pOk->setEnabled(false);
      m_pRemove->setEnabled(false);
    }
}

void KSpreadreference::slotOk()
{
  QString text;
  if(list->currentItem()!=-1)
	{
	int index=list->currentItem();
        text=list->text(index);
        QValueList<Reference> area=m_pView->doc()->listArea();

        if(m_pView->activeTable()->tableName()!=area[ index ].table_name)
                {
                KSpreadTable *table=m_pView->doc()->map()->findTable(area[ index ].table_name);
                if(table)
                        table->setActiveTable();
                }

        m_pView->canvasWidget()->gotoLocation(KSpreadPoint( m_pView->activeTable()->tableName()
                +"!"+util_cellName(area[ index ].rect.left() ,area[ index ].rect.top()  ), m_pView->doc()->map() ) );
        m_pView->activeTable()->setSelection(area[ index ].rect , m_pView->canvasWidget() );
        }
  accept();
}

void KSpreadreference::slotClose()
{
  reject();
}


#include "kspread_dlg_reference.moc"
