/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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

#include <kapp.h>
#include <klocale.h>

#include <list>

KSpreadassistant::KSpreadassistant( KSpreadView* parent, const char* name,QString _formula)
	: QDialog( 0L, name )
{
  m_pView = parent;

  formula=_formula;

  table = m_pView->activeTable();
  dx = m_pView->canvasWidget()->markerColumn();
  dy = m_pView->canvasWidget()->markerRow();

  setCaption( i18n("Function") );

  QLabel* tmpQLabel;
  tmpQLabel = new QLabel( this, "Label_1" );
  tmpQLabel->setGeometry( 10, 20, 150, 30 );
  tmpQLabel->setText(formula );

  m_pRef = new QLineEdit( this );
  m_pRef->setGeometry( 10, 50, 150, 30 );

  m_pOk = new QPushButton( i18n("Ok"), this );
  m_pOk->setGeometry( 180, 20, 100, 30 );

  m_pClose = new QPushButton( i18n("Cancel"), this );
  m_pClose->setGeometry( 180, 60, 100, 30 );

  connect( m_pOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
  connect( m_pClose, SIGNAL( clicked() ), this, SLOT( slotClose() ) );

  connect( m_pView, SIGNAL( sig_selectionChanged( KSpreadTable*, const QRect& ) ),
	   this, SLOT( slotSelectionChanged( KSpreadTable*, const QRect& ) ) );
}


void KSpreadassistant::slotOk()
{


table->setText( dy, dx , m_pRef->text() );
//m_pView->canvasWidget()->setMarkerColumn(dx);
//m_pView->canvasWidget()->setMarkerRow(dy);

accept();
}

void KSpreadassistant::slotClose()
{
  reject();
}



void KSpreadassistant::slotSelectionChanged( KSpreadTable* _table, const QRect& _selection )
{
  if ( _selection.left() == 0 || _selection.top() == 0 ||
       _selection.right() == 0 || _selection.bottom() == 0 )
  {
    m_pRef->setText( "" );
    return;
  }

  QString area = util_rangeName( _table, _selection );
  area=formula+"("+area+")";
  m_pRef->setText( area );
  m_pRef->setSelection( 0, area.length() );
}

#include "kspread_dlg_assistant.moc"
