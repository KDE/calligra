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

#include "kspread_dlg_special.h"
#include "kspread_view.h"
#include "kspread_canvas.h"
#include "kspread_doc.h"
#include "kspread_table.h"
#include <qlayout.h>
#include <kapp.h>
#include <klocale.h>
#include <kbuttonbox.h>
#include <qbuttongroup.h>

KSpreadspecial::KSpreadspecial( KSpreadView* parent, const char* name)
	: QDialog( 0L, name )
{
  m_pView = parent;

  setCaption( i18n("Special Paste") );
  QVBoxLayout *lay1 = new QVBoxLayout( this );
  lay1->setMargin( 5 );
  lay1->setSpacing( 10 );

  QButtonGroup *grp = new QButtonGroup( 1, QGroupBox::Horizontal, "Special Paste",this);
  grp->setRadioButtonExclusive( TRUE );
  grp->layout();
  lay1->addWidget(grp);
  rb1 = new QRadioButton( i18n("All"), grp );
  rb2 = new QRadioButton( i18n("Formula"), grp );
  rb3 = new QRadioButton( i18n("Format"), grp );
  rb4 = new QRadioButton( i18n("All without border"), grp );
  rb5 = new QRadioButton( i18n("Create link"), grp );
  rb1->setChecked(true);
  cb=new QCheckBox(i18n("Transpose"),this);
  cb->layout();
  lay1->addWidget(cb);
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


void KSpreadspecial::slotOk()
{
 KSpreadTable::Special_paste sp;
if(rb1->isChecked())
	{
	if(cb->isChecked())
		{
		sp=KSpreadTable::ALL_trans;
		}
	else
		{
		sp=KSpreadTable::ALL;
		}
	}
if(rb2->isChecked())
	{
	if(cb->isChecked())
		{
		sp=KSpreadTable::Formula_trans;
		}
	else
		{
		sp=KSpreadTable::Formula;
		}
	}
if(rb3->isChecked())
	{
	if(cb->isChecked())
		{
		sp=KSpreadTable::Format_trans;
		}
	else
		{
		sp=KSpreadTable::Format;
		}
	}
if(rb4->isChecked())
	{
	if(cb->isChecked())
		{
		sp=KSpreadTable::Wborder_trans;
		}
	else
		{
		sp=KSpreadTable::Wborder;
		}
	}
if(rb5->isChecked())
	{
	if(cb->isChecked())
		{
		sp=KSpreadTable::Link_trans;
		}
	else
		{
		sp=KSpreadTable::Link;
		}
	}
m_pView->activeTable()->paste( QPoint(  m_pView->canvasWidget()->markerColumn(),  m_pView->canvasWidget()->markerRow() ) ,sp);
accept();
}

void KSpreadspecial::slotClose()
{
reject();
}




#include "kspread_dlg_special.moc"
