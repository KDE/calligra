/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include "pgconfdia.h"
#include "pgconfdia.moc"
#include "kpresenter_doc.h"

#include <qlabel.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qheader.h>
#include <qspinbox.h>

#include <klocale.h>
#include <kapp.h>
#include <kbuttonbox.h>

/******************************************************************/
/* class PgConfDia                                                */
/******************************************************************/

/*================================================================*/
PgConfDia::PgConfDia( QWidget* parent, KPresenterDoc *, const char* name,
                      bool infLoop, bool swMan, int pgNum, PageEffect pageEffect,
                      PresSpeed presSpeed )
    : QDialog( parent, name, true )
{
    QVBoxLayout *back = new QVBoxLayout( this );
    back->setMargin( 5 );
    back->setSpacing( 5 );

    general = new QButtonGroup( 1, Qt::Horizontal, i18n( "General" ), this, "general" );
    general->setFrameStyle( QFrame::Box | QFrame::Sunken );

    infinitLoop = new QCheckBox( i18n( "&Infinite Loop" ), general );
    infinitLoop->setChecked( infLoop );

    manualSwitch = new QCheckBox( i18n( "&Manual switch to next step" ), general );
    manualSwitch->setChecked( swMan );

    label4 = new QLabel( i18n( "Speed of the presentation:" ), general );

    speedSpinBox = new QSpinBox( general, "speedSpinBox" );
    speedSpinBox->setValue( presSpeed );

    back->addWidget(general);

    page = new QButtonGroup( 1, Qt::Horizontal, i18n( "Page Configuration" ), this, "page" );
    page->setFrameStyle( QFrame::Box | QFrame::Sunken );

    label1 = new QLabel( i18n( "Page number: %1" ).arg( pgNum ), page );

    label2 = new QLabel( i18n( "Effect for changing to next page:" ), page );

    effectCombo = new QComboBox( false, page );
    effectCombo->insertItem( i18n( "No effect" ) );
    effectCombo->insertItem( i18n( "Close horizontal" ) );
    effectCombo->insertItem( i18n( "Close vertical" ) );
    effectCombo->insertItem( i18n( "Close from all directions" ) );
    effectCombo->insertItem( i18n( "Open horizontal" ) );
    effectCombo->insertItem( i18n( "Open vertical" ) );
    effectCombo->insertItem( i18n( "Open from all directions" ) );
    effectCombo->insertItem( i18n( "Interlocking horizontal 1" ) );
    effectCombo->insertItem( i18n( "Interlocking horizontal 2" ) );
    effectCombo->insertItem( i18n( "Interlocking vertical 1" ) );
    effectCombo->insertItem( i18n( "Interlocking vertical 2" ) );
    effectCombo->insertItem( i18n( "Surround 1" ) );
    effectCombo->insertItem( i18n( "Fly away 1" ) );
    effectCombo->setCurrentItem( static_cast<int>( pageEffect ) );

    back->addWidget(page);
    
    slides = new QButtonGroup( 1, Qt::Horizontal, this );
    slides->setCaption( i18n( "Show slides in presentation" ) );

    slidesAll = new QRadioButton( i18n( "&All slides" ), slides );
    slidesCurrent = new QRadioButton( i18n( "&Current slide" ), slides );
    slidesSelected = new QRadioButton( i18n( "&Selected slides" ), slides );

    lSlides = new QListView( slides );
    lSlides->addColumn( i18n( "Slide Nr." ) );
    lSlides->addColumn( i18n( "Slide Title" ) );
    lSlides->header()->setMovingEnabled( false );
    lSlides->setSorting( -1 );

    back->addWidget(slides);

    slides->hide();

    connect( slides, SIGNAL( clicked( int ) ),
             this, SLOT( presSlidesChanged( int ) ) );

    KButtonBox *bb = new KButtonBox( this );

    bb->addStretch();
    okBut = bb->addButton( i18n( "OK" ) );
    okBut->setDefault( true );
    cancelBut = bb->addButton( i18n( "Cancel" ) );
    bb->layout();

    bb->setMaximumHeight( okBut->sizeHint().height() + 5 );

    connect( okBut, SIGNAL( clicked() ), this, SLOT( confDiaOk() ) );
    connect( cancelBut, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( okBut, SIGNAL( clicked() ), this, SLOT( accept() ) );

    back->addWidget(bb);

    //presSlidesChanged( 0 );
}

/*================================================================*/
void PgConfDia::presSlidesChanged( int )
{
    if ( slidesSelected->isChecked() )
        lSlides->setEnabled( true );
    else
        lSlides->setEnabled( false );
}

/*================================================================*/
bool PgConfDia::getInfinitLoop()
{
    return infinitLoop->isChecked();
}

/*================================================================*/
bool PgConfDia::getManualSwitch()
{
    return manualSwitch->isChecked();
}

/*================================================================*/
PageEffect PgConfDia::getPageEffect()
{
    return static_cast<PageEffect>( effectCombo->currentItem() );
}

/*================================================================*/
PresSpeed PgConfDia::getPresSpeed()
{
    return speedSpinBox->value();
}

