/* This file is part of the KDE project
   Copyright (C) 1999-2004 Laurent Montel <montel@kde.org>
             (C) 2003 Norbert Andres <nandres@web.de>
             (C) 2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2002 John Dailey <dailey@vt.edu>
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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/


#include "kspread_dlg_special.h"
#include "kspread_canvas.h"
#include "kspread_doc.h"
#include "kspread_sheet.h"
#include "kspread_view.h"

#include <qlayout.h>
#include <klocale.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>

KSpreadspecial::KSpreadspecial( KSpreadView* parent, const char* name )
	: KDialogBase( parent, name, TRUE,i18n("Special Paste"),Ok|Cancel  )
{
    m_pView = parent;
    QWidget *page = new QWidget( this );
    setMainWidget(page);
    QVBoxLayout *lay1 = new QVBoxLayout( page, 0, spacingHint() );

    QButtonGroup *grp = new QButtonGroup( 1, QGroupBox::Horizontal, i18n( "Paste What" ),page );
    grp->setRadioButtonExclusive( TRUE );
    grp->layout();
    lay1->addWidget(grp);
    rb1 = new QRadioButton( i18n("Everything"), grp );
    rb2 = new QRadioButton( i18n("Text"), grp );
    rb3 = new QRadioButton( i18n("Format"), grp );
    rb10 = new QRadioButton( i18n("Comment"), grp );
    rb11 = new QRadioButton( i18n("Result"), grp );

    rb4 = new QRadioButton( i18n("Everything without border"), grp );
    rb1->setChecked(true);

    grp = new QButtonGroup( 1, QGroupBox::Horizontal, i18n("Operation"),page);
    grp->setRadioButtonExclusive( TRUE );
    grp->layout();
    lay1->addWidget(grp);


    rb5 = new QRadioButton( i18n("Overwrite"), grp );
    rb6 = new QRadioButton( i18n("Addition"), grp );
    rb7 = new QRadioButton( i18n("Subtraction"), grp );
    rb8 = new QRadioButton( i18n("Multiplication"), grp );
    rb9 = new QRadioButton( i18n("Division"), grp );
    rb5->setChecked(true);

    // cb = new QCheckBox(i18n("Transpose"),this);
    // cb->layout();
    // lay1->addWidget(cb);

    connect( this, SIGNAL( okClicked() ), this, SLOT( slotOk() ) );
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
    else if( rb11->isChecked() )
	sp = Result;

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

    m_pView->doc()->emitBeginOperation( false );
    m_pView->activeSheet()->paste( m_pView->selection(), true, sp, op );
    m_pView->slotUpdateView( m_pView->activeSheet() );
    accept();
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
