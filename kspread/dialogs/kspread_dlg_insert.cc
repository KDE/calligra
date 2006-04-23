/* This file is part of the KDE project
   Copyright (C) 2003 Norbert Andres <nandres@web.de>
             (C) 1999-2002 Laurent Montel <montel@kde.org>
             (C) 2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 2000-2001 Werner Trobin <trobin@kde.org>
             (C) 2000 David Faure <faure@kde.org>
             (C) 1999 Stephan Kulow <coolo@kde.org>
             (C) 1998-2000 Torben Weis <weis@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <q3buttongroup.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qlayout.h>
//Added by qt3to4:
#include <QVBoxLayout>

#include <kbuttonbox.h>
#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>

#include "kspread_doc.h"
#include "kspread_sheet.h"
#include "kspread_view.h"

#include "kspread_dlg_insert.h"

using namespace KSpread;

InsertDialog::InsertDialog( View* parent, const char* name,const QRect &_rect,Mode _mode)
  : KDialogBase( KDialogBase::Tabbed, Qt::Dialog, parent, name, true,"",Ok|Cancel )
{
  m_pView = parent;
  rect=_rect;
  insRem=_mode;

  QWidget *page = new QWidget( this );
  setMainWidget(page);
  QVBoxLayout *lay1 = new QVBoxLayout( page );
  lay1->setMargin(KDialogBase::marginHint());
  lay1->setSpacing(KDialogBase::spacingHint());

  Q3ButtonGroup *grp = new Q3ButtonGroup( 1, Qt::Horizontal, i18n("Insert"),page);
  grp->setRadioButtonExclusive( true );
  grp->layout();
  lay1->addWidget(grp);
  if( insRem==Insert)
  {
    rb1 = new QRadioButton( i18n("Move towards right"), grp );
    rb2 = new QRadioButton( i18n("Move towards bottom"), grp );
    rb3 = new QRadioButton( i18n("Insert rows"), grp );
    rb4 = new QRadioButton( i18n("Insert columns"), grp );
    setWindowTitle( i18n("Insert Cells") );
  }
  else if(insRem==Remove)
  {
    grp->setTitle(i18n("Remove"));
    rb1 = new QRadioButton( i18n("Move towards left"), grp );
    rb2 = new QRadioButton( i18n("Move towards top"), grp );
    rb3 = new QRadioButton( i18n("Remove rows"), grp );
    rb4 = new QRadioButton( i18n("Remove columns"), grp );
    setWindowTitle( i18n("Remove Cells") );
  }
  else
    kDebug(36001) << "Error in kspread_dlg_InsertDialog" << endl;

  rb1->setChecked(true);


  connect( this, SIGNAL( okClicked() ), this, SLOT( slotOk() ) );
}

void InsertDialog::slotOk()
{
    m_pView->doc()->emitBeginOperation( false );
    if( rb1->isChecked() )
    {
	if( insRem == Insert )
        {
	    if ( !m_pView->activeSheet()->shiftRow( rect ) )
		KMessageBox::error( this, i18n("The row is full. Cannot move cells to the right.") );
	}
	else if( insRem == Remove )
        {
	    m_pView->activeSheet()->unshiftRow(rect);
	}
    }
    else if( rb2->isChecked() )
    {
	if( insRem == Insert )
        {
	    if ( !m_pView->activeSheet()->shiftColumn( rect ) )
		KMessageBox::error( this, i18n("The column is full. Cannot move cells towards the bottom.") );
	}
	else if( insRem == Remove )
        {
	    m_pView->activeSheet()->unshiftColumn( rect );
	}
    }
    else if( rb3->isChecked() )
    {
	if( insRem == Insert )
        {
	    if ( !m_pView->activeSheet()->insertRow( rect.top(),(rect.bottom()-rect.top() ) ) )
		KMessageBox::error( this, i18n("The row is full. Cannot move cells to the right.") );
	}
	else if( insRem == Remove )
        {
	    m_pView->activeSheet()->removeRow( rect.top(),(rect.bottom()-rect.top() ) );
	}
    }
    else if( rb4->isChecked() )
    {
	if( insRem == Insert )
        {
	    if ( !m_pView->activeSheet()->insertColumn( rect.left(),(rect.right()-rect.left() )) )
		KMessageBox::error( this, i18n("The column is full. Cannot move cells towards the bottom.") );
	}
	else if( insRem == Remove )
        {
	    m_pView->activeSheet()->removeColumn( rect.left(),(rect.right()-rect.left() ) );
	}
    }
    else
    {
	kDebug(36001) << "Error in kspread_dlg_InsertDialog" << endl;
    }

    m_pView->updateEditWidget();

    m_pView->slotUpdateView( m_pView->activeSheet() );
    accept();
}

#include "kspread_dlg_insert.moc"
