/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>
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

#include "kspread_dlg_insert.h"
#include "kspread_view.h"
#include "kspread_canvas.h"
#include "kspread_doc.h"
#include "kspread_table.h"
#include "kspread_map.h"
#include <qlayout.h>
#include <kapp.h>
#include <klocale.h>
#include <kbuttonbox.h>
#include <qbuttongroup.h>
#include <kdebug.h>
#include <kmessagebox.h>

KSpreadinsert::KSpreadinsert( KSpreadView* parent, const char* name,const QRect &_rect,Mode _mode)
	: QDialog( parent, name, TRUE )
{
  m_pView = parent;
  rect=_rect;
  insRem=_mode;


  QVBoxLayout *lay1 = new QVBoxLayout( this );
  lay1->setMargin( 5 );
  lay1->setSpacing( 10 );

  QButtonGroup *grp = new QButtonGroup( 1, QGroupBox::Horizontal, i18n("Insert"),this);
  grp->setRadioButtonExclusive( TRUE );
  grp->layout();
  lay1->addWidget(grp);
  if( insRem==Insert)
  {
    rb1 = new QRadioButton( i18n("Move towards right"), grp );
    rb2 = new QRadioButton( i18n("Move towards bottom"), grp );
    rb3 = new QRadioButton( i18n("Insert row(s)"), grp );
    rb4 = new QRadioButton( i18n("Insert column(s)"), grp );
    setCaption( i18n("Insert cell") );
  }
  else if(insRem==Remove)
  {
    grp->setTitle(i18n("Remove"));
    rb1 = new QRadioButton( i18n("Move towards left"), grp );
    rb2 = new QRadioButton( i18n("Move towards top"), grp );
    rb3 = new QRadioButton( i18n("Remove row(s)"), grp );
    rb4 = new QRadioButton( i18n("Remove column(s)"), grp );
    setCaption( i18n("Remove cell") );
  }
  else
    kdDebug(36001) << "Error in kspread_dlg_insert" << endl;

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

void KSpreadinsert::slotOk()
{
    if( rb1->isChecked() )
    {
	if( insRem == Insert )
        {
	    if ( !m_pView->activeTable()->shiftRow( rect ) )
		KMessageBox::error( this, i18n("The row is full. Can not move cells to the right.") );
	}
	else if( insRem == Remove )
        {
	    m_pView->activeTable()->unshiftRow(rect);
	}
    }
    else if( rb2->isChecked() )
    {
	if( insRem == Insert )
        {
	    if ( !m_pView->activeTable()->shiftColumn( rect ) )
		KMessageBox::error( this, i18n("The column is full. Can not move cells towards the bottom.") );
	}
	else if( insRem == Remove )
        {
	    m_pView->activeTable()->unshiftColumn( rect );
	}
    }
    else if( rb3->isChecked() )
    {
	if( insRem == Insert )
        {
	    if ( !m_pView->activeTable()->insertRow( rect.top(),(rect.bottom()-rect.top() ) ) )
		KMessageBox::error( this, i18n("The row is full. Can not move cells to the right.") );
	}
	else if( insRem == Remove )
        {
	    m_pView->activeTable()->removeRow( rect.top(),(rect.bottom()-rect.top() ) );
	}
    }
    else if( rb4->isChecked() )
    {
	if( insRem == Insert )
        {
	    if ( !m_pView->activeTable()->insertColumn( rect.left(),(rect.right()-rect.left() )) )
		KMessageBox::error( this, i18n("The column is full. Can not move cells towards the bottom.") );
	}
	else if( insRem == Remove )
        {
	    m_pView->activeTable()->removeColumn( rect.left(),(rect.right()-rect.left() ) );
	}
    }
    else
    {
	kdDebug(36001) << "Error in kspread_dlg_insert" << endl;
    }

    m_pView->updateEditWidget();

    accept();
}

void KSpreadinsert::slotClose()
{
  reject();
}


#include "kspread_dlg_insert.moc"
