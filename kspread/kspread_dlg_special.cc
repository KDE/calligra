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
#include "kspread_global.h"

#include <qlayout.h>
#include <kapp.h>
#include <klocale.h>
#include <kbuttonbox.h>
#include <qbuttongroup.h>

KSpreadspecial::KSpreadspecial( KSpreadView* parent, const char* name )
	: QDialog( parent, name, TRUE )
{
    m_pView = parent;

    setCaption( i18n("Special Paste") );
    QVBoxLayout *lay1 = new QVBoxLayout( this );
    lay1->setMargin( 5 );
    lay1->setSpacing( 10 );

    QButtonGroup *grp = new QButtonGroup( 1, QGroupBox::Horizontal, i18n( "Paste what:" ),this );
    grp->setRadioButtonExclusive( TRUE );
    grp->layout();
    lay1->addWidget(grp);
    rb1 = new QRadioButton( i18n("Everything"), grp );
    rb2 = new QRadioButton( i18n("Text"), grp );
    rb3 = new QRadioButton( i18n("Format"), grp );
    rb10 = new QRadioButton( i18n("Comment"), grp );
    rb4 = new QRadioButton( i18n("Everything without border"), grp );
    rb1->setChecked(true);

    grp = new QButtonGroup( 1, QGroupBox::Horizontal, i18n("Operation"),this);
    grp->setRadioButtonExclusive( TRUE );
    grp->layout();
    lay1->addWidget(grp);


    rb5 = new QRadioButton( i18n("Overwrite"), grp );
    rb6 = new QRadioButton( i18n("Addition"), grp );
    rb7 = new QRadioButton( i18n("Substraction"), grp );
    rb8 = new QRadioButton( i18n("Multiplication"), grp );
    rb9 = new QRadioButton( i18n("Division"), grp );
    rb5->setChecked(true);

    // cb = new QCheckBox(i18n("Transpose"),this);
    // cb->layout();
    // lay1->addWidget(cb);

    KButtonBox *bb = new KButtonBox( this );
    bb->addStretch();
    m_pOk = bb->addButton( i18n("OK") );
    m_pOk->setDefault( TRUE );
    m_pClose = bb->addButton( i18n( "Close" ) );
    bb->layout();
    lay1->addWidget( bb );

    connect( m_pOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
    connect( m_pClose, SIGNAL( clicked() ), this, SLOT( slotClose() ) );
    connect( rb3, SIGNAL( toggled( bool ) ), this, SLOT( slotToggled( bool ) ) );
    connect( rb10, SIGNAL( toggled( bool ) ), this, SLOT( slotToggled( bool ) ) );
}

void KSpreadspecial::slotOk()
{
    PasteMode sp = Normal;
    Operation op = OverWrite;

    /* if( rb1->isChecked() )
	sp = cb->isChecked() ? NormalAndTranspose : Normal;
    else if( rb2->isChecked() )
	sp = cb->isChecked() ? TextAndTranspose : Text;
    else if( rb3->isChecked() )
	sp = cb->isChecked() ? FormatAndTranspose : Format;
    else if( rb4->isChecked() )
    sp = cb->isChecked() ? NoBorderAndTranspose : NoBorder; */

    if( rb1->isChecked() )
	sp = Normal;
    else if( rb2->isChecked() )
	sp = Text;
    else if( rb3->isChecked() )
	sp = Format;
    else if( rb4->isChecked() )
	sp = NoBorder;
    else if( rb10->isChecked() )
	sp = Comment;

    if( rb5->isChecked() )
	op = OverWrite;
    if( rb6->isChecked() )
	op = Add;
    if( rb7->isChecked() )
	op = Sub;
    if( rb8->isChecked() )
	op = Mul;
    if( rb9->isChecked() )
	op = Div;

    m_pView->activeTable()->paste( QPoint(  m_pView->canvasWidget()->markerColumn(),
					    m_pView->canvasWidget()->markerRow() ),true, sp, op );
    accept();
}

void KSpreadspecial::slotClose()
{
    reject();
}

void KSpreadspecial::slotToggled( bool b )
{
    rb5->setEnabled( !b );
    rb6->setEnabled( !b );
    rb7->setEnabled( !b );
    rb8->setEnabled( !b );
    rb9->setEnabled( !b );
}

#include "kspread_dlg_special.moc"
