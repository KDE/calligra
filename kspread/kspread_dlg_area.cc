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



#include "kspread_dlg_area.h"
#include "kspread_view.h"
#include "kspread_table.h"
#include "kspread_doc.h"

#include <kapp.h>
#include <klocale.h>
#include <qstringlist.h>
#include <qlayout.h>
#include <kbuttonbox.h>

#include <qlineedit.h>
#include <qlabel.h>

#include <kmessagebox.h>

KSpreadarea::KSpreadarea( KSpreadView* parent, const char* name,const QPoint &_marker )
	: QDialog( parent, name,TRUE )
{
  m_pView = parent;
  marker=_marker;

  QVBoxLayout *lay1 = new QVBoxLayout( this );
  lay1->setMargin( 5 );
  lay1->setSpacing( 10 );

  areaName=new QLineEdit(this);
  lay1->addWidget( areaName );

  setCaption( i18n("Area Name") );

  KButtonBox *bb = new KButtonBox( this );
  bb->addStretch();
  m_pOk = bb->addButton( i18n("OK") );
  m_pOk->setDefault( TRUE );
  m_pClose = bb->addButton( i18n( "Close" ) );
  bb->layout();
  lay1->addWidget( bb );
  areaName->setFocus();
  connect( m_pOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
  connect( m_pClose, SIGNAL( clicked() ), this, SLOT( slotClose() ) );
}



void KSpreadarea::slotOk()
{
  if( !areaName->text().isEmpty())
  {
        QRect rect( m_pView->activeTable()->selectionRect() );
        if ( rect.left() == 0 || rect.top() == 0 ||
                rect.right() == 0 || rect.bottom() == 0 )
                {
                rect.setCoords( marker.x(), marker.y(), marker.x(),marker.y() );
                }
        bool newName=true;
        QValueList<Reference>::Iterator it;
        QValueList<Reference> area=m_pView->doc()->listArea();
        for ( it = area.begin(); it != area.end(); ++it )
    	        {
    	        if(areaName->text()==(*it).ref_name)
                        newName=false;
    	        }
        if(newName)
                {
                m_pView->doc()->addAreaName(rect,areaName->text(),m_pView->activeTable()->name());
                accept();
                }
        else
                KMessageBox::error( this, i18n("This name is already used."));
  }
  else
  {
        KMessageBox::error( this, i18n("Area text is empty!") );
  }
}

void KSpreadarea::slotClose()
{

  reject();
}


#include "kspread_dlg_area.moc"
