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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/


// Local
#include "KCPageLayout.h"

// Qt
#include <QLabel>
#include <QGroupBox>
#include <QLineEdit>
#include <QLayout>

// KDE
#include <knumvalidator.h>
//Added by qt3to4:
#include <klocale.h>

// KChart
#include "KChartPart.h"


namespace KChart
{

KCPageLayout::KCPageLayout( KChartPart* _part, QWidget* parent)
	: KDialog( parent )
{
    setButtons( Ok | Cancel | User1 | Apply );
    setDefaultButton( Ok );
    setCaption( i18n( "Page Layout" ) );

    part=_part;
#if 0
    QWidget *page = new QWidget( this );
#else
//      QGroupBox* page = new QGroupBox( i18n("Margins"), this );
#endif
    QWidget *page = new QWidget( this );
    setMainWidget(page);

    // FIXME: The following code is strange, since it is written to
    // use a grid layout with a standard QWidget.  However, with the
    // QGroupBox, it looks better, and since it actually works, there
    // is no immediate need for rewriting.  In the sake of clarity, it
    // should be done though, and we should use the layout
    // capabilities of the groupbox instead..
    setButtonText( KDialog::User1, i18n("&Reset") );

    QGridLayout *grid = new QGridLayout( page );
    grid->setMargin( KDialog::marginHint() );
    grid->setSpacing( KDialog::spacingHint() );

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

    connect( this, SIGNAL( okClicked() ),    this, SLOT( slotOk() ) );
    connect( this, SIGNAL( applyClicked() ), this, SLOT( slotApply() ) );
    connect( this, SIGNAL( user1Clicked() ), this ,SLOT( slotReset() ));
}

void KCPageLayout::init()
{
#if 0
    oldGlobalLeadingRight  = part->globalLeadingRight();
    oldGlobalLeadingLeft   = part->globalLeadingLeft();
    oldGlobalLeadingTop    = part->globalLeadingTop();
    oldGlobalLeadingBottom = part->globalLeadingBottom();
#endif

    slotReset();
}

void KCPageLayout::slotOk()
{
    slotApply();
    accept();
}

void KCPageLayout::slotApply()
{
#if 0
    part->setGlobalLeading( leftBorder->text().toInt(),topBorder->text().toInt() , rightBorder->text().toInt(), bottomBorder->text().toInt() );
    emit dataChanged();
#endif
}

void KCPageLayout::slotReset()
{
    rightBorder->setText(QString::number(oldGlobalLeadingRight));
    leftBorder->setText(QString::number(oldGlobalLeadingLeft));
    topBorder->setText(QString::number(oldGlobalLeadingTop));
    bottomBorder->setText(QString::number(oldGlobalLeadingBottom));
}

}  //KChart namespace

#include "KCPageLayout.moc"
