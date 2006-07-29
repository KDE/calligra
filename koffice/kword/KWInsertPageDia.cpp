/* This file is part of the KDE project
   Copyright (C) 2002 Montel Laurent <lmontel@mandrakesoft.com>

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



#include "KWInsertPageDia.h"
#include <klocale.h>
#include <qradiobutton.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qbuttongroup.h>
#include <qvbox.h>

KWInsertPageDia::KWInsertPageDia( QWidget *parent, const char* name )
    : KDialogBase( parent, name,TRUE,i18n("Insert Page"),Ok|Cancel )
{
    QVBox *page = makeVBoxMainWidget();

    QButtonGroup *grp = new QButtonGroup( 1, QGroupBox::Horizontal, i18n( "Insert Page" ),page );
    grp->setRadioButtonExclusive( TRUE );
    grp->layout();
    m_after = new QRadioButton( i18n("After"), grp );
    m_before = new QRadioButton( i18n("Before"), grp );

    grp->setRadioButtonExclusive( TRUE );
    m_after->setChecked( true );
    resize( 200, 150 );
    setFocus();
}

InsertPagePos KWInsertPageDia::insertPagePos() const
{
    return m_after->isChecked() ? KW_INSERTPAGEAFTER : KW_INSERTPAGEBEFORE;
}


#include "KWInsertPageDia.moc"
