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

#include "kspread_dlg_assistant2.h"
#include "kspread_view.h"
#include "kspread_canvas.h"
#include "kspread_doc.h"
#include "kspread_util.h"
#include "kspread_tabbar.h"
#include "kspread_table.h"

#include <qlayout.h>
#include <kapp.h>
#include <klocale.h>
#include "kspread_dlg_create.h"


KSpreadassistant2::KSpreadassistant2( KSpreadcreate* parent, const char* name)
	: QDialog( parent, name )
{

  m_pCreate=parent;
  m_pView=m_pCreate->view();

  m_pCreate->hide();

  // m_pView->canvasWidget()->setEditorActivate(false);
  setCaption( i18n("Function") );
  QHBoxLayout *lay1 = new QHBoxLayout( this );
  lay1->setMargin( 5 );
  lay1->setSpacing( 10 );

  m_pRef = new QLineEdit( this );
  lay1->addWidget(m_pRef);


  m_pClose = new QPushButton( i18n("Ok"), this );
  lay1->addWidget(m_pClose);

  connect( m_pClose, SIGNAL( clicked() ), this, SLOT( slotClose() ) );

  connect( m_pView, SIGNAL( sig_selectionChanged( KSpreadTable*, const QRect& ) ),
	   this, SLOT( slotSelectionChanged( KSpreadTable*, const QRect& ) ) );

  m_pView->canvasWidget()->startChoose();
}

KSpreadassistant2::~KSpreadassistant2()
{
    m_pView->canvasWidget()->endChoose();
}
				
void KSpreadassistant2::slotClose()
{
    give_range();
    m_pCreate->show();

    delete this;
}

void KSpreadassistant2::give_range()
{
    QString _tabname;
    QString tmp;
    int p = m_pRef->text().find( "!" );
    _tabname=m_pRef->text().left(p);

    if( m_pCreate->tableName() != _tabname )
    {
	//change table
	tmp= m_pRef->text();
    }
    else
    {
	tmp= m_pRef->text().right(m_pRef->text().length()-(p+1));
    }
    m_pCreate->setText(tmp);
}

void KSpreadassistant2::slotSelectionChanged( KSpreadTable* _table, const QRect& _selection )
{
    QString tmp;
  if ( _selection.left() == 0 || _selection.top() == 0 ||
       _selection.right() == 0 || _selection.bottom() == 0 )
  	{
    	
    	int dx=m_pView->canvasWidget()->markerColumn();
    	int dy=m_pView->canvasWidget()->markerRow();
    	tmp=tmp.setNum(dy);
    	tmp=_table->tableName()+"!"+util_columnLabel(dx)+tmp;
    	m_pRef->setText(tmp);
  	}
  else
  {
      if( m_pCreate->paramCount() == 5 )
      {
   		//When you have 5 params you can add others params
   		//=>creat range
   		QString area = util_rangeName( _table, _selection );
		m_pRef->setText( area );
  		m_pRef->setSelection( 0, area.length() );
  		}
  	else
  		{
  		int dx=_selection.right();
    		int dy=_selection.bottom();
    		tmp=tmp.setNum(dy);
    		tmp=_table->tableName()+"!"+util_columnLabel(dx)+tmp;
    		m_pRef->setText(tmp);
  		}
  	}
}

#include "kspread_dlg_assistant2.moc"
