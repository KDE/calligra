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

#include "kspread_dlg_create.h"
#include "kspread_view.h"
#include "kspread_canvas.h"
#include "kspread_doc.h"
#include "kspread_table.h"
#include <qlayout.h>
#include <kapp.h>
#include <klocale.h>



KSpreadcreate::KSpreadcreate( KSpreadView* parent, const char* name,int param)
	: QDialog( 0L, name )
{
  m_pView = parent;
  setCaption( i18n(name) );
  QVBoxLayout *lay1 = new QVBoxLayout( this );
  lay1->setMargin( 5 );
  lay1->setSpacing( 10 );

  QLabel* tmpQLabel;
  tmpQLabel = new QLabel( this);


  lay1->addWidget(tmpQLabel);
  tmpQLabel->setText(i18n("First parameter"));

  f_param = new QLineEdit( this );
  lay1->addWidget(f_param);



  tmpQLabel = new QLabel( this);
  lay1->addWidget(tmpQLabel);
  tmpQLabel->setText(i18n("Second parameter"));
  s_param = new QLineEdit( this );
  lay1->addWidget(s_param);

  QHBoxLayout *lay2 = new QHBoxLayout( lay1);
  lay2->setSpacing( 5 );

  m_pOk = new QPushButton( i18n("OK"), this );
  lay2->addWidget(m_pOk);
  m_pClose = new QPushButton( i18n("Close"), this );
  lay2->addWidget(m_pClose);
  connect( m_pOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
  connect( m_pClose, SIGNAL( clicked() ), this, SLOT( slotClose() ) );
  resize(300,200);
}


void KSpreadcreate::slotOk()
{
accept();
}


void KSpreadcreate::slotClose()
{
reject();
}

#include "kspread_dlg_create.moc"
