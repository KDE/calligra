/* This file is part of the KDE project
   Copyright (C) 2003 Norbert Andres <nandres@web.de>
             (C) 2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 2000-2002 Laurent Montel <montel@kde.org>

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

#include <qlayout.h>
//Added by qt3to4:
#include <QVBoxLayout>
#include <klocale.h>
#include <kbuttonbox.h>
#include <q3buttongroup.h>
#include <kdebug.h>
#include <qradiobutton.h>
#include <qcheckbox.h>

#include "kspread_dlg_pasteinsert.h"
#include "kspread_canvas.h"
#include "kspread_doc.h"
#include "kspread_sheet.h"
#include "kspread_view.h"
#include "selection.h"

using namespace KSpread;

PasteInsertDialog::PasteInsertDialog( View* parent, const char* name,const QRect &_rect)
  : KDialogBase( KDialogBase::Tabbed, Qt::Dialog, parent, name, true,i18n("Paste Inserting Cells"),Ok|Cancel )
{
  m_pView = parent;
  rect=_rect;

  QWidget *page = new QWidget( this );
  setMainWidget(page);
  QVBoxLayout *lay1 = new QVBoxLayout( page );
  lay1->setMargin(KDialogBase::marginHint());
  lay1->setSpacing(KDialogBase::spacingHint());

  Q3ButtonGroup *grp = new Q3ButtonGroup( 1, Qt::Horizontal, i18n("Insert"),page);
  grp->setRadioButtonExclusive( true );
  grp->layout();
  lay1->addWidget(grp);
  rb1 = new QRadioButton( i18n("Move towards right"), grp );
  rb2 = new QRadioButton( i18n("Move towards bottom"), grp );
  rb1->setChecked(true);

  connect( this, SIGNAL( okClicked() ), this, SLOT( slotOk() ) );
}

void PasteInsertDialog::slotOk()
{
    m_pView->doc()->emitBeginOperation( false );
    if( rb1->isChecked() )
      m_pView->activeSheet()->paste( m_pView->selectionInfo()->lastRange(),
                                     true, Paste::Normal, Paste::OverWrite,
                                     true, -1 );
    else if( rb2->isChecked() )
      m_pView->activeSheet()->paste( m_pView->selectionInfo()->lastRange(),
                                     true, Paste::Normal, Paste::OverWrite,
                                     true, +1 );

    m_pView->slotUpdateView( m_pView->activeSheet() );
    accept();
}

#include "kspread_dlg_pasteinsert.moc"
