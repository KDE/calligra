/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>
   Copyright (C) 2000,2001 Montel Laurent <lmontel@mandrakesoft.com>
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

#include "kspread_dlg_pasteinsert.h"
#include "kspread_view.h"
#include "kspread_canvas.h"
#include "kspread_table.h"

#include <qlayout.h>
#include <kapp.h>
#include <kbuttonbox.h>
#include <qbuttongroup.h>
#include <kdebug.h>

KSpreadpasteinsert::KSpreadpasteinsert( KSpreadView* parent, const char* name,const QRect &_rect)
	: QDialog( parent, name, TRUE )
{
  m_pView = parent;
  rect=_rect;

  QVBoxLayout *lay1 = new QVBoxLayout( this );
  lay1->setMargin( 5 );
  lay1->setSpacing( 10 );

  QButtonGroup *grp = new QButtonGroup( 1, QGroupBox::Horizontal, i18n("Insert"),this);
  grp->setRadioButtonExclusive( TRUE );
  grp->layout();
  lay1->addWidget(grp);
  rb1 = new QRadioButton( i18n("Move towards right"), grp );
  rb2 = new QRadioButton( i18n("Move towards bottom"), grp );
  setCaption( i18n("Paste inserting cell(s)") );
  rb1->setChecked(true);

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

void KSpreadpasteinsert::slotOk()
{
    if( rb1->isChecked() )
        m_pView->activeTable()->paste( QPoint( m_pView->canvasWidget()->markerColumn(), m_pView->canvasWidget()->markerRow() ) ,true, Normal,OverWrite,true,-1);
    else if( rb2->isChecked() )
        m_pView->activeTable()->paste( QPoint( m_pView->canvasWidget()->markerColumn(), m_pView->canvasWidget()->markerRow() ) ,true, Normal,OverWrite,true,+1);

    accept();
}

void KSpreadpasteinsert::slotClose()
{
  reject();
}


#include "kspread_dlg_pasteinsert.moc"
