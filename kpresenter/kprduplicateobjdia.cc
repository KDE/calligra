// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
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
#include "kprpage.h"
#include <knuminput.h>
#include <qlineedit.h>
#include <koUnit.h>
#include <knumvalidator.h>
#include "kpresenter_doc.h"
#include <kseparator.h>

KPrDuplicatObjDia::KPrDuplicatObjDia( QWidget *parent, KPresenterDoc * _doc, const char *name)
    : KDialogBase( parent, name, true, "", Ok | Cancel, Ok, true )
{
    m_doc=_doc;
    KoUnit::Unit unit = m_doc->getUnit();

    KoRect pageRect = m_doc->masterPage()->getPageRect();

    setCaption( i18n("Duplicate Object") );

    QVBox *page = makeVBoxMainWidget();
    QLabel *lab=new QLabel(i18n("Number of copies:"), page);

    m_nbCopy = new KIntNumInput( 1, page );
    m_nbCopy->setRange( 1 , 10, 1);

    KSeparator *tmp=new KSeparator(page);
    lab=new QLabel(i18n("Rotation angle:"), page);

    m_rotation = new KDoubleNumInput( page, "customInput" );
    m_rotation->setRange(0.0, 360, 1, false);

    tmp=new KSeparator(page);

    lab=new QLabel(i18n("Increase width (%1):").arg(m_doc->getUnitName()), page);
    m_increaseX= new KDoubleNumInput( page );
    m_increaseX->setRange( 0.0, KoUnit::toUserValue( pageRect.width(), unit ), KoUnit::toUserValue( 1, unit ), false);

    lab=new QLabel(i18n("Increase height (%1):").arg(m_doc->getUnitName()), page);
    m_increaseY= new KDoubleNumInput( page );
    m_increaseY->setRange( 0.0, KoUnit::toUserValue( pageRect.height(), unit ), KoUnit::toUserValue( 1, unit ), false);


    tmp=new KSeparator(page);
    lab=new QLabel(i18n("Move X (%1):").arg(m_doc->getUnitName()), page);
    m_moveX= new KDoubleNumInput( page );
    m_moveX->setRange( 0.0, KoUnit::toUserValue( pageRect.width(), unit ), KoUnit::toUserValue( 1, unit ) , false);
    m_moveX->setValue( KoUnit::toUserValue( 20.0, unit ) );

    lab=new QLabel(i18n("Move Y (%1):").arg(m_doc->getUnitName()), page);
    m_moveY= new KDoubleNumInput( page );
    m_moveY->setRange( 0.0, KoUnit::toUserValue( pageRect.height(), unit ), KoUnit::toUserValue( 1, unit ), false);
    m_moveY->setValue( KoUnit::toUserValue( 20.0, unit ) );

    resize( 200,100 );
}

int KPrDuplicatObjDia::nbCopy() const
{
    return m_nbCopy->value();
}

double KPrDuplicatObjDia::angle() const
{
    return m_rotation->value();
}

double KPrDuplicatObjDia::increaseX() const
{
    return QMAX(0, KoUnit::fromUserValue( m_increaseX->value(), m_doc->getUnit() ));
}

double KPrDuplicatObjDia::increaseY() const
{
    return QMAX(0, KoUnit::fromUserValue( m_increaseY->value(), m_doc->getUnit() ));
}

double KPrDuplicatObjDia::moveX() const
{
    return QMAX(0, KoUnit::fromUserValue( m_moveX->value(), m_doc->getUnit() ));
}

double KPrDuplicatObjDia::moveY() const
{
    return QMAX(0, KoUnit::fromUserValue( m_moveY->value(), m_doc->getUnit() ));
}


#include "kprduplicateobjdia.moc"
