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

#include "kspread_dlg_replace.h"
#include "kspread_view.h"
#include "kspread_canvas.h"
#include "kspread_doc.h"
#include "kspread_tabbar.h"
#include <qlayout.h>
#include <kapp.h>
#include <klocale.h>



KSpreadreplace::KSpreadreplace( KSpreadView* parent, const char* name)
	: QDialog( 0L, name )
{
  m_pView = parent;

  setCaption( i18n("Replace") );
  QVBoxLayout *lay1 = new QVBoxLayout( this );
  lay1->setMargin( 5 );
  lay1->setSpacing( 10 );
  QGridLayout *lay2 = new QGridLayout( lay1,4,2 );
  lay2->setSpacing( 5 );

  QLabel* tmpQLabel;
  tmpQLabel = new QLabel( this);


  lay2->addWidget(tmpQLabel,0,0);
  tmpQLabel->setText(i18n("Find"));

  l_find = new QLineEdit( this );
  lay2->addWidget(l_find,1,0);


  m_pOk = new QPushButton( i18n("Replace"), this );

  lay2->addWidget(m_pOk,2,1);
  m_pClose = new QPushButton( i18n("Close"), this );
  lay2->addWidget(m_pClose,3,1);
  tmpQLabel = new QLabel( this);
  lay2->addWidget(tmpQLabel,2,0);
  tmpQLabel->setText(i18n("Replace with"));
  l_replace = new QLineEdit( this );
  lay2->addWidget(l_replace,3,0);


  connect( m_pOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
  connect( m_pClose, SIGNAL( clicked() ), this, SLOT( slotClose() ) );

}


void KSpreadreplace::slotOk()
{
accept();
}

void KSpreadreplace::slotClose()
{
reject();
}

#include "kspread_dlg_replace.moc"
