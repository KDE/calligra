/* This file is part of the KDE project
   Copyright (C)  2001 Montel Laurent <lmontel@mandrakesoft.com>

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

#include <kapp.h>
#include <qdialog.h>
#include <klocale.h>

#include <qlayout.h>
#include <kapp.h>
#include <kbuttonbox.h>

#include <qbuttongroup.h>
#include <qradiobutton.h>

#include "kwChangeCaseDia.h"
#include "kwview.h"
#include "defs.h"


KWChangeCaseDia::KWChangeCaseDia( QWidget *parent, const char *name )
    : QDialog( parent, name, TRUE )
{

    setCaption( i18n("Change case: ") );
    QVBoxLayout *lay1 = new QVBoxLayout( this );
    lay1->setMargin( 5 );
    lay1->setSpacing( 10 );

    QButtonGroup *grp = new QButtonGroup( 1, QGroupBox::Horizontal, i18n( "Case:" ),this );
    grp->setRadioButtonExclusive( TRUE );
    grp->layout();
    lay1->addWidget(grp);
    m_upperCase=new QRadioButton( i18n("Uppercase"), grp );
    m_lowerCase=new QRadioButton( i18n("Lowercase"), grp );

    m_titleCase=new QRadioButton( i18n("Title Case"), grp );

    m_upperCase->setChecked(true);
    grp->setRadioButtonExclusive( TRUE );

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


void KWChangeCaseDia::slotOk()
{
    accept();
}

void KWChangeCaseDia::slotClose()
{
    reject();
}

TypeOfCase KWChangeCaseDia::getTypeOfCase()
{
    TypeOfCase type=UpperCase;
    if( m_upperCase->isChecked())
        type=UpperCase;
    else if(m_lowerCase->isChecked())
        type=LowerCase;
    else if(m_titleCase->isChecked())
        type=TitleCase;
    return type;
}

#include "kwChangeCaseDia.moc"
