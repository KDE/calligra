
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
#include "kivio_dlg_layername.h"
#include "kivio_view.h"
#include "kivio_canvas.h"
#include "kivio_doc.h"
#include "kivio_map.h"

#include <kbuttonbox.h>
#include <kapp.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <qlayout.h>

KivioLayerName::KivioLayerName( KivioView* _parent, const char* _name, QString& _layerName )
: QDialog( _parent, _name, true )
{
   m_pView = _parent;

   setCaption( i18n("Change Layer Name") );

   QLabel* tmpQLabel;
   QVBoxLayout *lay1 = new QVBoxLayout( this );
   lay1->setMargin( 5 );
   lay1->setSpacing( 10 );

   tmpQLabel = new QLabel( this, "Label_1" );
   tmpQLabel->setText( i18n("Layer Name") );
   lay1->addWidget( tmpQLabel );

   m_pLayerName = new QLineEdit( this );
   m_pLayerName->setText( _layerName );
   m_pLayerName->setFocus();
   lay1->addWidget( m_pLayerName );

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

void KivioLayerName::slotOk()
{
   QString txt = (m_pLayerName->text()); //.stripWhiteSpace();
   if ( txt.isEmpty() )
   {
      QApplication::beep();
      KMessageBox::information( this, i18n("Layer name cannot be empty."), i18n("Change Layer Name"));
      return;
   }

   accept();
}

void KivioLayerName::slotClose()
{
  reject();
}

#include "kivio_dlg_layername.moc"
