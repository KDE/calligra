/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999, 2000 Montel Laurent <montell@club-internet.fr>

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
#include "kspread_table.h"
#include <qlayout.h>
#include <qlabel.h>
#include <kapp.h>
#include <klocale.h>
#include <kbuttonbox.h>
#include <kmessagebox.h>

KSpreadReplaceDlg::KSpreadReplaceDlg( KSpreadView* parent, const char* name,const QPoint &_marker)
	: QDialog( parent, name,TRUE )
{
  m_pView = parent;
  marker= _marker;
  setCaption( i18n("Replace text") );
  QVBoxLayout *lay1 = new QVBoxLayout( this );
  lay1->setMargin( 5 );
  lay1->setSpacing( 10 );

  QLabel* tmpQLabel;
  tmpQLabel = new QLabel( this);


  lay1->addWidget(tmpQLabel);
  tmpQLabel->setText(i18n("Find"));

  l_find = new QLineEdit( this );
  lay1->addWidget(l_find);


  tmpQLabel = new QLabel( this);
  lay1->addWidget(tmpQLabel);
  tmpQLabel->setText(i18n("Replace with"));
  l_replace = new QLineEdit( this );
  lay1->addWidget(l_replace);
  l_find->setFocus();

  sensitive=new QCheckBox(i18n("Case Sensitive"),this);
  lay1->addWidget(sensitive);

  wholeWordOnly=new QCheckBox(i18n( "Find Whole Words only" ),this);
  lay1->addWidget(wholeWordOnly);

  KButtonBox *bb = new KButtonBox( this );
  bb->addStretch();
  m_pOk = bb->addButton( i18n("Replace") );
  m_pOk->setDefault( TRUE );
  m_pClose = bb->addButton( i18n( "Close" ) );
  bb->layout();
  lay1->addWidget( bb);

  connect( m_pOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
  connect( m_pClose, SIGNAL( clicked() ), this, SLOT( slotClose() ) );
}

void KSpreadReplaceDlg::slotOk()
{
    // Search == Replace ?
    if ( l_replace->text() == l_find->text() )
    {
	accept();
	return;
    }

    // Nothing to find ?
    if ( l_find->text().isEmpty() )
    {
	 KMessageBox::error( this, i18n("You must enter some text to search for.") );
	 return;
    }

    // Do the replacement.
    if( !( m_pView->activeTable()->replace( marker,l_find->text(),
					    l_replace->text(),
					    sensitive->isChecked(),
					    wholeWordOnly->isChecked() ) ) )
    {
	KMessageBox::information( this, i18n("No text was replaced.") );
    }

    // Refresh the editWidget
    KSpreadCell *cell = m_pView->activeTable()->cellAt( m_pView->canvasWidget()->markerColumn(),
							m_pView->canvasWidget()->markerRow() );
    if ( cell->text() != 0L )
	m_pView->editWidget()->setText( cell->text() );
    else
	m_pView->editWidget()->setText( "" );

    accept();
}


void KSpreadReplaceDlg::slotClose()
{
    reject();
}

#include "kspread_dlg_replace.moc"
