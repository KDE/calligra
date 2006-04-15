/* This file is part of the KDE project
   Copyright (C) 1999-2003 Laurent Montel <montel@kde.org>
             (C) 2003 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2003 Ariya Hidayat <ariya@kde.org>
             (C) 2003 Norbert Andres <nandres@web.de>
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

#include <qlabel.h>
#include <qlayout.h>
//Added by qt3to4:
#include <QVBoxLayout>

#include <klineedit.h>

#include "kspread_canvas.h"
#include "kspread_doc.h"
#include "kspread_locale.h"
#include "kspread_util.h"
#include "kspread_view.h"
#include "selection.h"

#include "kspread_dlg_goto.h"

using namespace KSpread;

GotoDialog::GotoDialog( View* parent, const char* name )
	: KDialogBase( parent, name, TRUE, i18n("Goto Cell"), Ok|Cancel )
{
  m_pView = parent;
  QWidget *page = new QWidget( this );
  setMainWidget(page);
  QVBoxLayout *lay1 = new QVBoxLayout( page );
  lay1->setMargin(KDialogBase::marginHint());
  lay1->setSpacing(KDialogBase::spacingHint());

  QLabel *label = new QLabel(i18n("Enter cell:"), page);
  lay1->addWidget(label);

  m_nameCell = new KLineEdit( page );
  lay1->addWidget(m_nameCell);

  m_nameCell->setFocus();
  enableButtonOK( false );

  connect( this, SIGNAL( okClicked() ), this, SLOT( slotOk() ) );
  connect( m_nameCell, SIGNAL(textChanged ( const QString & )),
           this, SLOT(textChanged ( const QString & )));
}

void GotoDialog::textChanged ( const QString &_text )
{
    enableButtonOK(!_text.isEmpty());
}

void GotoDialog::slotOk()
{
    m_pView->doc()->emitBeginOperation( false );

    QString tmp_upper;
    tmp_upper=m_nameCell->text().toUpper();
    Region region(m_pView, tmp_upper);
    if ( region.isValid() )
    {
      m_pView->selectionInfo()->initialize(region);
      accept();
    }
    else
    {
        m_nameCell->clear();
    }
    m_pView->slotUpdateView( m_pView->activeSheet() );
}

#include "kspread_dlg_goto.moc"
