/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999 Boris Wedl <boris.wedl@kfunigraz.ac.at>

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


#include "kspread_dlg_tabname.h"
#include "kspread_view.h"
#include "kspread_canvas.h"
#include "kspread_doc.h"
#include "kspread_util.h"
#include "kspread_map.h"

#include <kapp.h>
#include <klocale.h>
#include <qmessagebox.h>
#include <qlayout.h>

KSpreadTableName::KSpreadTableName( KSpreadView* _parent, const char* _name, QString& _tableName )
	: QDialog( _parent, _name, true )
{
   m_pView = _parent;

   setCaption( i18n("Change Table Name") );

   QLabel* tmpQLabel;

   QGridLayout* grid = new QGridLayout( this, 2, 2, 10 );

   tmpQLabel = new QLabel( this, "Label_1" );
   tmpQLabel->setText( i18n("Table Name") );
   grid->addWidget( tmpQLabel, 0, 0 );

   m_pTableName = new QLineEdit( this );
   m_pTableName->setText( _tableName );
   m_pTableName->setFocus();
   grid->addWidget( m_pTableName, 1, 0 );

   m_pOk = new QPushButton( i18n("Ok"), this );
   grid->addWidget( m_pOk, 0, 1 );
   m_pClose = new QPushButton( i18n("Cancel"), this );
   grid->addWidget( m_pClose, 1, 1 );

   connect( m_pOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
   connect( m_pClose, SIGNAL( clicked() ), this, SLOT( slotClose() ) );
   connect( m_pTableName, SIGNAL( returnPressed() ), this, SLOT( slotOk() ) );
}

void KSpreadTableName::slotOk()
{
   QString txt = (m_pTableName->text()).stripWhiteSpace();
   if ( txt.isEmpty() )
   {
      QApplication::beep();
      QMessageBox::information( this, i18n("Change table name"), i18n("Table name cannot be empty."), i18n("OK") );
      return;
   }
   if ( KSpreadTable *tbl = m_pView->doc()->map()->findTable( txt ) )
   {
      if ( tbl != m_pView->activeTable() )
      {
         QApplication::beep();
         QMessageBox::information( this, i18n("Change table name"), i18n("A table with this name already exists."), i18n("OK") );
         return;
      }
   }

   accept();
}

void KSpreadTableName::slotClose()
{
   reject();
}

#include "kspread_dlg_tabname.moc"
