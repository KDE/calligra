/* This file is part of the KDE project
   Copyright (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 1999-2001 Laurent Montel <montel@kde.org>
             (C) 1998-1999 Torben Weis <weis@kde.org>

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
#include <qlabel.h>
#include <qlineedit.h>
//Added by qt3to4:
#include <QVBoxLayout>
#include <Q3ValueList>

#include <kmessagebox.h>

#include "kspread_doc.h"
#include "kspread_locale.h"
#include "kspread_sheet.h"
#include "kspread_view.h"
#include "selection.h"

#include "kspread_dlg_area.h"

using namespace KSpread;

AreaDialog::AreaDialog( View * parent, const char * name, const QPoint & _marker )
  : KDialogBase( parent, name, TRUE, i18n("Area Name"), Ok | Cancel )
{
  m_pView  = parent;
  m_marker = _marker;

  QWidget * page = new QWidget( this );
  setMainWidget(page);
  QVBoxLayout * lay1 = new QVBoxLayout( page );
  lay1->setMargin(KDialogBase::marginHint());
  lay1->setSpacing(KDialogBase::spacingHint());

  QLabel * label = new QLabel( i18n("Enter the area name:"), page );
  lay1->addWidget( label );

  m_areaName = new QLineEdit(page);
  m_areaName->setMinimumWidth( m_areaName->sizeHint().width() * 3 );

  lay1->addWidget( m_areaName );
  m_areaName->setFocus();
  connect ( m_areaName, SIGNAL(textChanged ( const QString & )), this, SLOT(slotAreaNamechanged( const QString &)));
  connect( this, SIGNAL( okClicked() ), this, SLOT( slotOk() ) );
  enableButtonOK(!m_areaName->text().isEmpty());

}

void AreaDialog::slotAreaNamechanged( const QString & text)
{
  enableButtonOK(!text.isEmpty());
}

void AreaDialog::slotOk()
{
  QString tmp(m_areaName->text());
  if( !tmp.isEmpty() )
  {
    tmp = tmp.toLower();

    QRect rect( m_pView->selectionInfo()->selection() );
    bool newName = true;
    QList<Reference>::Iterator it;
    QList<Reference> area = m_pView->doc()->listArea();
    for ( it = area.begin(); it != area.end(); ++it )
    {
      if(tmp == (*it).ref_name)
        newName = false;
    }
    if (newName)
    {
      m_pView->doc()->emitBeginOperation( false );
      m_pView->doc()->addAreaName(rect, tmp, m_pView->activeSheet()->sheetName());
      m_pView->slotUpdateView( m_pView->activeSheet() );
      accept();
    }
    else
      KMessageBox::error( this, i18n("This name is already used."));
  }
  else
  {
    KMessageBox::error( this, i18n("Area text is empty.") );
  }
}

#include "kspread_dlg_area.moc"
