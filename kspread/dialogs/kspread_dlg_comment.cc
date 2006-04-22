/* This file is part of the KDE project
   Copyright (C) 2003 Norbert Andres <nandres@web.de>
             (C) 2002 Ariya Hidayat <ariya@kde.org>
             (C) 2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 1999-2002 Laurent Montel <montel@kde.org>
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

#include <q3multilineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
//Added by qt3to4:
#include <QVBoxLayout>

#include <klocale.h>
#include <kbuttonbox.h>

#include "kspread_canvas.h"
#include "kspread_doc.h"
#include "selection.h"
#include "kspread_sheet.h"
#include "kspread_view.h"

#include "kspread_dlg_comment.h"

using namespace KSpread;

CommentDialog::CommentDialog( View* parent, const char* name,const QPoint &_marker)
  : KDialogBase( KDialogBase::Tabbed, Qt::Dialog, parent, name,TRUE,i18n("Cell Comment"),Ok|Cancel )
{
    m_pView = parent;
    marker= _marker;
    QWidget *page = new QWidget( this );
    setMainWidget(page);
    QVBoxLayout *lay1 = new QVBoxLayout( page );
    lay1->setMargin(KDialogBase::marginHint());
    lay1->setSpacing(KDialogBase::spacingHint());

    multiLine = new Q3MultiLineEdit( page );
    lay1->addWidget(multiLine);

    multiLine->setFocus();


    Cell *cell = m_pView->activeSheet()->cellAt( m_pView->canvasWidget()->markerColumn(), m_pView->canvasWidget()->markerRow() );
    if(!cell->format()->comment(marker.x(),marker.y()).isEmpty())
      multiLine->setText(cell->format()->comment(marker.x(),marker.y()));

    connect( this, SIGNAL( okClicked() ), this, SLOT( slotOk() ) );
    connect(multiLine, SIGNAL(textChanged ()),this, SLOT(slotTextChanged()));

    slotTextChanged();

    resize( 400, height() );
}

void CommentDialog::slotTextChanged()
{
    enableButtonOK( !multiLine->text().isEmpty());
}

void CommentDialog::slotOk()
{
    m_pView->doc()->emitBeginOperation( false );
    m_pView->activeSheet()->setSelectionComment( m_pView->selectionInfo(),
                                                 multiLine->text().trimmed() );
    m_pView->slotUpdateView( m_pView->activeSheet(), *m_pView->selectionInfo() );
    // m_pView->doc()->emitEndOperation();
    accept();
}

#include "kspread_dlg_comment.moc"
