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
#include "PageNameDialog.h"
#include "GDocument.h"
#include "KIllustrator_view.h"

#include <kbuttonbox.h>
#include <kapp.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <qlayout.h>

PageNameDialog::PageNameDialog( KIllustratorView* _parent, const char* _name, QString& _pageName )
: QDialog( _parent, _name, true )
{
   m_pView = _parent;
   m_PageName=_pageName;
   setCaption( i18n("Change Page Name") );

   QLabel* tmpQLabel;
   QVBoxLayout *lay1 = new QVBoxLayout( this );
   lay1->setMargin( 5 );
   lay1->setSpacing( 10 );

   tmpQLabel = new QLabel( this, "Label_1" );
   tmpQLabel->setText( i18n("Page Name") );
   lay1->addWidget( tmpQLabel );

   m_pPageName = new QLineEdit( this );
   m_pPageName->setText( _pageName );
   m_pPageName->setFocus();
   lay1->addWidget( m_pPageName );

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

void PageNameDialog::slotOk()
{
   QString txt = (m_pPageName->text()).stripWhiteSpace();
   if ( txt.isEmpty() )
   {
      QApplication::beep();
      KMessageBox::information( this, i18n("Page name cannot be empty."), i18n("Change page name"));
      return;
   }
/*   if ( GPage *tbl = m_pView->activeDocument()->findPage( txt ) )
   {
      if ( tbl != m_pView->activeDocument()->activePage() )
      {
         QApplication::beep();
         KMessageBox::information( this, i18n("A page with this name already exists."), i18n("Change page name") );
         m_pPageName->setText(m_PageName);
         return;
      }
   }*/

   accept();
}

void PageNameDialog::slotClose()
{
  reject();
}

#include "PageNameDialog.moc"
