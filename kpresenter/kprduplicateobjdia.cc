/* This file is part of the KDE project
   Copyright (C)  2002 Montel Laurent <lmontel@mandrakesoft.com>

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

#include <klocale.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qbuttongroup.h>

#include "kprduplicateobjdia.h"
#include <knuminput.h>


KPrDuplicatObjDia::KPrDuplicatObjDia( QWidget *parent,const char *name)
    : KDialogBase( parent, name , true, "", Ok | Cancel | User1, Ok, true )
{
    setButtonText( KDialogBase::User1, i18n("Reset") );
    setCaption( i18n("Duplicate Object") );

    QVBox *page = makeVBoxMainWidget();
    QLabel *lab=new QLabel(i18n("Number of copy:"), page);

    m_nbCopy = new KIntNumInput( 1, page );
    m_nbCopy->setRange( 1 , 10, 1);

    lab=new QLabel(i18n("Rotation Angle:"), page);

    m_rotation = new KDoubleNumInput( page, "customInput" );

    connect( this, SIGNAL( user1Clicked() ), this ,SLOT( slotReset() ));
    resize( 200,100 );
}

void KPrDuplicatObjDia::slotReset()
{
    m_nbCopy->setValue( 1 );
    m_rotation->setValue( 0.0 );
}

int KPrDuplicatObjDia::nbCopy() const
{
    return m_nbCopy->value();
}

double KPrDuplicatObjDia::angle() const
{
    return m_rotation->value();
}

#include "kprduplicateobjdia.moc"
