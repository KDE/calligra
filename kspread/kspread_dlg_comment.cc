/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999, 2000, 2001  Montel Laurent <lmontel@mandrakesoft.com>

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

#include "kspread_dlg_comment.h"
#include "kspread_view.h"
#include "kspread_canvas.h"
#include "kspread_table.h"
#include <qlayout.h>
#include <kapp.h>
#include <kbuttonbox.h>

KSpreadComment::KSpreadComment( KSpreadView* parent, const char* name,const QPoint &_marker)
	: QDialog( parent, name,TRUE )
{
    m_pView = parent;
    marker= _marker;

    setCaption( i18n("Cell comment") );

    QVBoxLayout *lay1 = new QVBoxLayout( this );
    lay1->setMargin( 5 );
    lay1->setSpacing( 10 );

    multiLine = new QMultiLineEdit( this );
    lay1->addWidget(multiLine);

    multiLine->setFocus();

    KButtonBox *bb = new KButtonBox( this );
    bb->addStretch();
    m_pOk = bb->addButton( i18n("OK") );
    m_pOk->setDefault( TRUE );
    m_pClose = bb->addButton( i18n( "Close" ) );
    bb->layout();
    lay1->addWidget( bb);

    KSpreadCell *cell = m_pView->activeTable()->cellAt( m_pView->canvasWidget()->markerColumn(), m_pView->canvasWidget()->markerRow() );
    if(!cell->comment(marker.x(),marker.y()).isEmpty())
        multiLine->setText(cell->comment(marker.x(),marker.y()));

    connect( m_pOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
    connect( m_pClose, SIGNAL( clicked() ), this, SLOT( slotClose() ) );
}

void KSpreadComment::slotOk()
{
    m_pView->activeTable()->setSelectionComment(marker,multiLine->text().stripWhiteSpace() );
    accept();
}


void KSpreadComment::slotClose()
{
    reject();
}

#include "kspread_dlg_comment.moc"
