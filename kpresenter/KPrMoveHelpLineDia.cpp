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
#include <KoUnit.h>
#include <klineedit.h>
#include <knumvalidator.h>
#include <KoUnitWidgets.h>

#include "KPrMoveHelpLineDia.h"
#include "KPrDocument.h"


KPrMoveHelpLineDia::KPrMoveHelpLineDia( QWidget *parent, double value, double limitTop, double limitBottom,
                                        KPrDocument *_doc, const char *name)
    : KDialogBase( parent, name , true, "", Ok | Cancel | User1, Ok, true )
{
    m_doc=_doc;
    m_bRemoveLine = false;

    setButtonText( KDialogBase::User1, i18n("Remove") );
    setCaption( i18n("Change Help Line Position") );
    KVBox *page = makeVBoxMainWidget();
    new QLabel(i18n("Position:"), page);
    position= new KoUnitDoubleSpinBox( page, qMax(0.00, limitTop), qMax(0.00, limitBottom), 1, qMax(0.00, value));
    position->setUnit(m_doc->unit() );

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
    return position->value();
}


KPrInsertHelpLineDia::KPrInsertHelpLineDia( QWidget *parent, const KoRect & _pageRect,
                                            KPrDocument *_doc, const char *name)
    : KDialogBase( parent, name , true, "", Ok|Cancel, Ok, true )
{
    limitOfPage=_pageRect;
    m_doc=_doc;
    setCaption( i18n("Add New Help Line") );
    KVBox *page = makeVBoxMainWidget();
    QButtonGroup *group = new QButtonGroup( 1, QGroupBox::Horizontal,i18n("Orientation"), page );
    group->setRadioButtonExclusive( true );
    group->layout();
    m_rbHoriz = new QRadioButton( i18n("Horizontal"), group );
    m_rbVert = new QRadioButton( i18n("Vertical"), group );

    connect( group , SIGNAL( clicked( int) ), this, SLOT( slotRadioButtonClicked() ));

    new QLabel(i18n("Position:"), page);

    position= new KoUnitDoubleSpinBox( page,qMax(0.00, limitOfPage.top() ), qMax(0.00, limitOfPage.bottom()),1,0.00 );

    position->setUnit( m_doc->unit() );
    m_rbHoriz->setChecked( true );
    resize( 300,100 );
}

double KPrInsertHelpLineDia::newPosition() const
{
    return position->value();
}

bool KPrInsertHelpLineDia::addHorizontalHelpLine()
{
    return m_rbHoriz->isChecked();
}

void KPrInsertHelpLineDia::slotRadioButtonClicked()
{
    if ( m_rbHoriz->isChecked() )
    {
        position->setMinValue( qMax(0.00, limitOfPage.top() ) );
        position->setMaxValue( qMax(0.00, limitOfPage.bottom() ) );
    }
    else if ( m_rbVert->isChecked() )
    {
        position->setMinValue( qMax(0.00, limitOfPage.left()) );
        position->setMaxValue( qMax(0.00, limitOfPage.right()) );
    }
}

KPrInsertHelpPointDia::KPrInsertHelpPointDia( QWidget *parent, const KoRect & _pageRect,
                                              KPrDocument *_doc, double posX, double posY, const char *name)
    : KDialogBase( parent, name , true, "", Ok|Cancel| User1, Ok, true ),
      m_bRemovePoint( false )
{
    limitOfPage=_pageRect;
    m_doc=_doc;
    setButtonText( KDialogBase::User1, i18n("Remove") );
    setCaption( i18n("Add New Help Point") );
    KVBox *page = makeVBoxMainWidget();
    QLabel *lab=new QLabel(i18n("X position:"), page);
    positionX= new KoUnitDoubleSpinBox( page, qMax(0.00, limitOfPage.left()),qMax(0.00, limitOfPage.right()),1,qMax(0.00, posX) ) ;
    positionX->setUnit( m_doc->unit() );


    lab=new QLabel(i18n("Y position:"), page);
    positionY= new KoUnitDoubleSpinBox( page, qMax(0.00, limitOfPage.top()),qMax(0.00, limitOfPage.bottom()),1,  qMax(0.00, posY) );
    positionY->setUnit( m_doc->unit() );

    showButton( KDialogBase::User1, (posX!=0.0 || posY!=0.0) );

    connect( this, SIGNAL( user1Clicked() ), this ,SLOT( slotRemoveHelpPoint() ));

    resize( 300,100 );
}

KoPoint KPrInsertHelpPointDia::newPosition() const
{
    return KoPoint( positionX->value(),
                    positionY->value() );
}

void KPrInsertHelpPointDia::slotRemoveHelpPoint()
{
    m_bRemovePoint = true;
    KDialogBase::slotOk();
}

#include "KPrMoveHelpLineDia.moc"
