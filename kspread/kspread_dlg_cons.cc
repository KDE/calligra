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

#include "kspread_dlg_cons.h"
#include "kspread_view.h"

#include <kapp.h>

KSpreadConsolidate::KSpreadConsolidate( KSpreadView* parent, const char* name )
	: QDialog( 0L, name )
{
  m_pView = parent;
  
  setCaption( i18n("Consolidate") );

  QLabel* tmpQLabel;
  tmpQLabel = new QLabel( this, "Label_1" );
  tmpQLabel->setGeometry( 10, 20, 150, 20 );
  tmpQLabel->setText( i18n("Function") );

  m_pFunction = new QComboBox( this );
  m_pFunction->setGeometry( 10, 40, 150, 30 );
  m_idSumme = 0; m_pFunction->insertItem( i18n("sum"), m_idSumme );
  
  tmpQLabel = new QLabel( this, "Label_1" );
  tmpQLabel->setGeometry( 10, 80, 150, 30 );
  tmpQLabel->setText( i18n("Reference") );
  
  m_pRef = new QLineEdit( this );
  m_pRef->setGeometry( 10, 110, 150, 30 );
  
  tmpQLabel = new QLabel( this, "Label_1" );
  tmpQLabel->setGeometry( 10, 140, 150, 30 );
  tmpQLabel->setText( i18n("Entered References") );

  m_pRefs = new QListBox( this );
  m_pRefs->setGeometry( 10, 170, 150, 80 );
  
  m_pOk = new QPushButton( i18n("Ok"), this );
  m_pOk->setGeometry( 180, 20, 100, 30 );
  m_pClose = new QPushButton( i18n("Close"), this );
  m_pClose->setGeometry( 180, 60, 100, 30 );
  
  m_pAdd = new QPushButton( i18n("Add"), this );
  m_pAdd->setGeometry( 180, 120, 100, 30 );
  m_pRemove = new QPushButton( i18n("Remove"), this );
  m_pRemove->setGeometry( 180, 160, 100, 30 );

  connect( m_pOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
  connect( m_pClose, SIGNAL( clicked() ), this, SLOT( slotClose() ) );
  connect( m_pAdd, SIGNAL( clicked() ), this, SLOT( slotAdd() ) );
  connect( m_pRemove, SIGNAL( clicked() ), this, SLOT( slotRemove() ) );
}

void KSpreadConsolidate::slotOk()
{
}

void KSpreadConsolidate::slotClose()
{
}

void KSpreadConsolidate::slotAdd()
{
  QString txt = m_pRef->text();
  if ( !txt.isEmpty() )
    m_pRefs->insertItem( txt );
}

void KSpreadConsolidate::slotRemove()
{
  int i = m_pRefs->currentItem();
  if ( i < 0 )
    return;
  
  m_pRefs->removeItem( i );
}

QStrList KSpreadConsolidate::refs()
{
  QStrList list;
  int c = m_pRefs->count();
  
  for( int i = 0; i < c; i++ )
    list.append( m_pRefs->text( i ) );
  
  return list;
}

#include "kspread_dlg_cons.moc"
