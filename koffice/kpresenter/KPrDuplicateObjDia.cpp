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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <klocale.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qbuttongroup.h>

#include "KPrDuplicateObjDia.h"
#include "KPrPage.h"
#include <knuminput.h>
#include <qlineedit.h>
#include <KoUnit.h>
#include <knumvalidator.h>
#include "KPrDocument.h"
#include <kseparator.h>
#include <KoUnitWidgets.h>

KPrDuplicatObjDia::KPrDuplicatObjDia( QWidget *parent, KPrDocument * _doc, const char *name)
    : KDialogBase( parent, name, true, "", Ok | Cancel, Ok, true )
{
    m_doc=_doc;
    KoUnit::Unit unit = m_doc->unit();

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

    lab=new QLabel(i18n("Increase width:"), page);
    m_increaseX= new KoUnitDoubleSpinBox( page );
    m_increaseX->setMinValue(0.0);
    m_increaseX->setMaxValue(pageRect.width());
    m_increaseX->changeValue( 1 );
    m_increaseX->setUnit(unit);

    lab=new QLabel(i18n("Increase height:"), page);
    m_increaseY= new KoUnitDoubleSpinBox( page );
    m_increaseY->setMinValue(0.0);
    m_increaseY->setMaxValue(pageRect.height());
    m_increaseY->changeValue( 1 );
    m_increaseY->setUnit(unit);

    tmp=new KSeparator(page);
    lab=new QLabel(i18n("Move X:"), page);
    m_moveX= new KoUnitDoubleSpinBox( page );
    m_moveX->setMinValue(0.0);
    m_moveX->setMaxValue(pageRect.width());
    m_moveX->changeValue( 20.0 );
    m_moveX->setUnit(unit);


    lab=new QLabel(i18n("Move Y:"), page);
    m_moveY= new KoUnitDoubleSpinBox( page );
    m_moveY->setMinValue(0.0);
    m_moveY->setMaxValue(pageRect.height());
    m_moveY->changeValue( 20.0 );
    m_moveY->setUnit(unit);

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
    return QMAX(0, m_increaseX->value());
}

double KPrDuplicatObjDia::increaseY() const
{
    return QMAX(0, m_increaseY->value());
}

double KPrDuplicatObjDia::moveX() const
{
    return QMAX(0, m_moveX->value());
}

double KPrDuplicatObjDia::moveY() const
{
    return QMAX(0, m_moveY->value());
}


#include "KPrDuplicateObjDia.moc"
