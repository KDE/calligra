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
#include <knuminput.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <koUnit.h>
#include <klineedit.h>
#include <knumvalidator.h>

#include "kprhelplinedia.h"
#include "kpresenter_doc.h"


KPrMoveHelpLineDia::KPrMoveHelpLineDia( QWidget *parent, double value, double limitTop, double limitBottom,
                                        KPresenterDoc *_doc, const char *name)
    : KDialogBase( parent, name , true, "", Ok | Cancel | User1, Ok, true )
{
    m_doc=_doc;
    m_bRemoveLine = false;

    setButtonText( KDialogBase::User1, i18n("Remove") );
    setCaption( i18n("Change Help Line Position") );
    QVBox *page = makeVBoxMainWidget();
    new QLabel(i18n("Position (%1):").arg(m_doc->getUnitName()), page);
    position= new KDoubleNumInput( page );

    position->setValue( KoUnit::toUserValue( QMAX(0.00, value), m_doc->getUnit() ) );
    position->setRange ( KoUnit::toUserValue(QMAX(0.00, limitTop),m_doc->getUnit() ),
                         KoUnit::toUserValue(QMAX(0.00, limitBottom), m_doc->getUnit() ), 1, false);

    connect( this, SIGNAL( user1Clicked() ), this ,SLOT( slotRemoveHelpLine() ));
    resize( 300,100 );
}

void KPrMoveHelpLineDia::slotRemoveHelpLine()
{
    m_bRemoveLine = true;
    KDialogBase::slotOk();
}

double KPrMoveHelpLineDia::newPosition() const
{
    return KoUnit::fromUserValue(  position->value(), m_doc->getUnit() );
}


KPrInsertHelpLineDia::KPrInsertHelpLineDia( QWidget *parent, const KoRect & _pageRect,
                                            KPresenterDoc *_doc, const char *name)
    : KDialogBase( parent, name , true, "", Ok|Cancel, Ok, true )
{
    limitOfPage=_pageRect;
    m_doc=_doc;
    setCaption( i18n("Add New Help Line") );
    QVBox *page = makeVBoxMainWidget();
    QButtonGroup *group = new QButtonGroup( 1, QGroupBox::Horizontal,i18n("Orientation"), page );
    group->setRadioButtonExclusive( TRUE );
    group->layout();
    m_rbHoriz = new QRadioButton( i18n("Horizontal"), group );
    m_rbVert = new QRadioButton( i18n("Vertical"), group );

    connect( group , SIGNAL( clicked( int) ), this, SLOT( slotRadioButtonClicked() ));

    new QLabel(i18n("Position (%1):").arg(m_doc->getUnitName()), page);

    position= new KDoubleNumInput( page );

    position->setValue( KoUnit::toUserValue(0.00, m_doc->getUnit() ) );
    position->setRange ( KoUnit::toUserValue(QMAX(0.00, limitOfPage.top()),m_doc->getUnit() ),
                         KoUnit::toUserValue(QMAX(0.00, limitOfPage.bottom()), m_doc->getUnit() ), 1, false);

    m_rbHoriz->setChecked( true );
    resize( 300,100 );
}

double KPrInsertHelpLineDia::newPosition() const
{
    return KoUnit::fromUserValue(  position->value(), m_doc->getUnit() );
}

bool KPrInsertHelpLineDia::addHorizontalHelpLine()
{
    return m_rbHoriz->isChecked();
}

void KPrInsertHelpLineDia::slotRadioButtonClicked()
{
    if ( m_rbHoriz->isChecked() )
        position->setRange ( KoUnit::toUserValue(QMAX(0.00, limitOfPage.top()),m_doc->getUnit() ),
                             KoUnit::toUserValue(QMAX(0.00, limitOfPage.bottom()), m_doc->getUnit() ), 1, false);
    else if ( m_rbVert->isChecked() )
        position->setRange ( KoUnit::toUserValue(QMAX(0.00, limitOfPage.left()),m_doc->getUnit() ),
                             KoUnit::toUserValue(QMAX(0.00, limitOfPage.right()), m_doc->getUnit() ), 1, false);
}


KPrInsertHelpPointDia::KPrInsertHelpPointDia( QWidget *parent, const KoRect & _pageRect,
                                              KPresenterDoc *_doc, double posX, double posY, const char *name)
    : KDialogBase( parent, name , true, "", Ok|Cancel| User1, Ok, true ),
      m_bRemovePoint( false )
{
    limitOfPage=_pageRect;
    m_doc=_doc;
    setButtonText( KDialogBase::User1, i18n("Remove") );
    setCaption( i18n("Add New Help Point") );
    QVBox *page = makeVBoxMainWidget();
    QLabel *lab=new QLabel(i18n("X position (%1):").arg(m_doc->getUnitName()), page);
    positionX= new KDoubleNumInput( page );

    positionX->setValue( KoUnit::toUserValue( QMAX(0.00, posX), m_doc->getUnit() ) );
    positionX->setRange ( KoUnit::toUserValue(QMAX(0.00, limitOfPage.left()),m_doc->getUnit() ),
                          KoUnit::toUserValue(QMAX(0.00, limitOfPage.right()), m_doc->getUnit() ), 1, false);


    lab=new QLabel(i18n("Y position (%1):").arg(m_doc->getUnitName()), page);
    positionY= new KDoubleNumInput( page );

    positionY->setValue( KoUnit::toUserValue( QMAX(0.00, posY), m_doc->getUnit() ) );
    positionY->setRange ( KoUnit::toUserValue(QMAX(0.00, limitOfPage.top()),m_doc->getUnit() ),
                          KoUnit::toUserValue(QMAX(0.00, limitOfPage.bottom()), m_doc->getUnit() ), 1, false);

    showButton( KDialogBase::User1, (posX!=0.0 || posY!=0.0) );

    connect( this, SIGNAL( user1Clicked() ), this ,SLOT( slotRemoveHelpPoint() ));

    resize( 300,100 );
}

KoPoint KPrInsertHelpPointDia::newPosition() const
{
    return KoPoint( KoUnit::fromUserValue(  positionX->value(), m_doc->getUnit() ),
                    KoUnit::fromUserValue(  positionY->value(), m_doc->getUnit() ));
}

void KPrInsertHelpPointDia::slotRemoveHelpPoint()
{
    m_bRemovePoint = true;
    KDialogBase::slotOk();
}

#include "kprhelplinedia.moc"
