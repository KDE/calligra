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

#include "kspread_dlg_assistant.h"
#include "kspread_view.h"
#include "kspread_canvas.h"
#include "kspread_doc.h"
#include "kspread_util.h"
#include "kspread_tabbar.h"
#include "kspread_table.h"

#include <qlayout.h>
#include <kapp.h>
#include <klocale.h>


KSpreadassistant::KSpreadassistant( KSpreadView* parent, const char* name,QString _formula)
	: QDialog( parent, name )
{
  m_pView = parent;

  formula=_formula;
  tabname=m_pView->activeTable()->tableName();


  table = m_pView->activeTable();
  dx = m_pView->canvasWidget()->markerColumn();
  dy = m_pView->canvasWidget()->markerRow();
  // m_pView->canvasWidget()->setEditorActivate(false);

  setCaption( i18n("Function") );
  QVBoxLayout *lay1 = new QVBoxLayout( this );
  lay1->setMargin( 5 );
  lay1->setSpacing( 10 );
  QGridLayout *lay2 = new QGridLayout( lay1,2,2 );
  lay2->setSpacing( 5 );

  QLabel* tmpQLabel;
  tmpQLabel = new QLabel( this, "Label_1" );


  lay2->addWidget(tmpQLabel,0,0);
  tmpQLabel->setText(formula );

  m_pRef = new QLineEdit( this );
  lay2->addWidget(m_pRef,1,0);


  m_pOk = new QPushButton( i18n("Ok"), this );

  lay2->addWidget(m_pOk,0,1);
  m_pClose = new QPushButton( i18n("Cancel"), this );
  lay2->addWidget(m_pClose,1,1);

  connect( m_pOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
  connect( m_pClose, SIGNAL( clicked() ), this, SLOT( slotClose() ) );

  connect( m_pView, SIGNAL( sig_selectionChanged( KSpreadTable*, const QRect& ) ),
	   this, SLOT( slotSelectionChanged( KSpreadTable*, const QRect& ) ) );
}


void KSpreadassistant::slotOk()
{

if(m_pView->activeTable()->tableName() != tabname)
	{
	m_pView->tabBar()->setActiveTab(tabname);
	
	m_pView->changeTable( tabname );
	}
	
table->unselect();
table->setText( dy, dx , m_pRef->text() );
m_pView->canvasWidget()->hideMarker();
m_pView->canvasWidget()->setMarkerColumn(dx);
m_pView->canvasWidget()->setMarkerRow(dy);
m_pView->editWidget()->setText( m_pRef->text() );
m_pView->canvasWidget()->showMarker();



m_pView->editWidget()->setFocus();
m_pView->editWidget()->setActivate(true);
accept();
}

void KSpreadassistant::slotClose()
{
if(m_pView->activeTable()->tableName() != tabname)
	{
	m_pView->tabBar()->setActiveTab(tabname);
	m_pView->changeTable( tabname );
	}


table->unselect();
table->setText( dy, dx , m_pRef->text() );
m_pView->canvasWidget()->hideMarker();
m_pView->canvasWidget()->setMarkerColumn(dx);
m_pView->canvasWidget()->setMarkerRow(dy);
if ( m_pRef->text() != 0L )
    m_pView->editWidget()->setText( formula );
else
    m_pView->editWidget()->setText( "" );



m_pView->canvasWidget()->showMarker();
m_pView->editWidget()->setFocus();
//reactivate the combobox
m_pView->editWidget()->setActivate(true);
reject();
}



void KSpreadassistant::slotSelectionChanged( KSpreadTable* _table, const QRect& _selection )
{
  if ( _selection.left() == 0 || _selection.top() == 0 ||
       _selection.right() == 0 || _selection.bottom() == 0 )
  {
    formula=formula+"(:)";
    m_pRef->setText( formula );
    return;
  }

  QString area = util_rangeName( _table, _selection );
  area=formula+"("+area+")";
  m_pRef->setText( area );
  m_pRef->setSelection( 0, area.length() );
}

#include "kspread_dlg_assistant.moc"
