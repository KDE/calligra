/* This file is part of the KDE project
   Copyright (C) 2002  Montel Laurent <lmontel@mandrakesoft.com>
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


#include "kchartPageLayout.h"
#include "kchartPageLayout.moc"
#include "kchart_params.h"
#include <knumvalidator.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <klocale.h>
#include <qlabel.h>

namespace KChart
{

KChartPageLayout::KChartPageLayout( KChartParams* _params, QWidget* parent, const char* name )
	: KDialogBase( parent, name, TRUE,i18n("Page Layout"),KDialogBase::Ok | KDialogBase::Cancel | KDialogBase::User1 | KDialogBase::Apply , KDialogBase::Ok,true )
{
    params=_params;
    QWidget *page = new QWidget( this );
    setMainWidget(page);

    setButtonText( KDialogBase::User1, i18n("&Reset") );

    QGridLayout *grid = new QGridLayout(page, 4, 2, KDialog::marginHint(), KDialog::spacingHint());

    QLabel *lab=new QLabel(i18n("Left:"),page);
    grid->addWidget(lab,0,0);

    leftBorder=new QLineEdit(page);
    leftBorder->setValidator( new KIntValidator( 0,9999,leftBorder ) );
    grid->addWidget(leftBorder,1,0);

    lab=new QLabel(i18n("Right:"),page);
    grid->addWidget(lab,0,1);

    rightBorder=new QLineEdit(page);
    rightBorder->setValidator( new KIntValidator( 0,9999,rightBorder ) );
    grid->addWidget(rightBorder,1,1);

    lab=new QLabel(i18n("Top:"),page);
    grid->addWidget(lab,2,0);

    topBorder=new QLineEdit(page);
    topBorder->setValidator( new KIntValidator( 0,9999,topBorder ) );
    grid->addWidget(topBorder,3,0);

    lab=new QLabel(i18n("Bottom:"),page);
    grid->addWidget(lab,2,1);

    bottomBorder=new QLineEdit(page);
    bottomBorder->setValidator( new KIntValidator( 0,9999,bottomBorder ) );
    grid->addWidget(bottomBorder,3,1);

    init();
    connect( this, SIGNAL( okClicked() ), this, SLOT( slotOk() ) );
    connect( this, SIGNAL( applyClicked() ), this, SLOT( slotApply() ) );
    connect( this, SIGNAL( user1Clicked() ), this ,SLOT( slotReset() ));

}

void KChartPageLayout::init()
{
    oldGlobalLeadingRight = params->globalLeadingRight();
    oldGlobalLeadingLeft = params->globalLeadingLeft();
    oldGlobalLeadingTop = params->globalLeadingTop();
    oldGlobalLeadingBottom = params->globalLeadingBottom();
    slotReset();
}

void KChartPageLayout::slotOk()
{
    slotApply();
    accept();
}

void KChartPageLayout::slotApply()
{
    params->setGlobalLeading( leftBorder->text().toInt(),topBorder->text().toInt() , rightBorder->text().toInt(), bottomBorder->text().toInt() );
    emit dataChanged();
}

void KChartPageLayout::slotReset()
{
    rightBorder->setText(QString::number(oldGlobalLeadingRight));
    leftBorder->setText(QString::number(oldGlobalLeadingLeft));
    topBorder->setText(QString::number(oldGlobalLeadingTop));
    bottomBorder->setText(QString::number(oldGlobalLeadingBottom));
}

}  //KChart namespace
