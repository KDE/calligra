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

#include <pgconfdia.h>
#include <kpresenter_doc.h>

#include <qlabel.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qlayout.h>
#include <qcheckbox.h>

#include <klocale.h>
#include <kbuttonbox.h>
#include <kglobal.h>

/******************************************************************/
/* class PgConfDia                                                */
/******************************************************************/

/*================================================================*/
PgConfDia::PgConfDia( QWidget* parent, const char* name,
                      bool infLoop, bool swMan, bool showPresentationDuration )
    : KDialogBase( parent, name, true, "",Ok|Cancel )
{
    QWidget *page = new QWidget( this );
    setMainWidget(page);
    
    QVBoxLayout *back = new QVBoxLayout( page, 0, spacingHint() );

    general = new QButtonGroup( 1, Qt::Horizontal, i18n( "General" ), page, "general" );
    general->setFrameStyle( QFrame::Box | QFrame::Sunken );

    infiniteLoop = new QCheckBox( i18n( "&Infinite loop" ), general );
    infiniteLoop->setChecked( infLoop );

    manualSwitch = new QCheckBox( i18n( "&Manual switch to next step" ), general );
    manualSwitch->setChecked( swMan );
    connect( manualSwitch, SIGNAL( clicked() ), this, SLOT( slotManualSwitch() ) );

    presentationDuration = new QCheckBox( i18n( "&Show presentation duration" ), general );
    presentationDuration->setChecked( showPresentationDuration );

    back->addWidget(general);

#if 0
    slides = new QButtonGroup( 1, Qt::Horizontal, page );
    slides->setCaption( i18n( "Show Slides in Presentation" ) );

    slidesAll = new QRadioButton( i18n( "&All slides" ), slides );
    slidesCurrent = new QRadioButton( i18n( "&Current slide" ), slides );
    slidesSelected = new QRadioButton( i18n( "&Selected slides" ), slides );

    lSlides = new QListView( slides );
    lSlides->addColumn( i18n( "Slide No." ) );
    lSlides->addColumn( i18n( "Slide Title" ) );
    lSlides->header()->setMovingEnabled( false );
    lSlides->setSorting( -1 );

    back->addWidget(slides);

    slides->hide();

    connect( slides, SIGNAL( clicked( int ) ),
             this, SLOT( presSlidesChanged( int ) ) );
#endif

    connect( this, SIGNAL( okClicked() ), this, SLOT( confDiaOk() ) );
    connect( this, SIGNAL( okClicked() ), this, SLOT( accept() ) );

    //presSlidesChanged( 0 );
}

/*================================================================*/
PgConfDia::~PgConfDia()
{
}

/*================================================================*/
#if 0
void PgConfDia::presSlidesChanged( int )
{
    if ( slidesSelected->isChecked() )
        lSlides->setEnabled( true );
    else
        lSlides->setEnabled( false );
}
#endif

/*================================================================*/
bool PgConfDia::getInfiniteLoop() const
{
    return infiniteLoop->isChecked();
}

/*================================================================*/
bool PgConfDia::getManualSwitch() const
{
    return manualSwitch->isChecked();
}

/*================================================================*/
bool PgConfDia::getPresentationDuration() const
{
    return presentationDuration->isChecked();
}

#include <pgconfdia.moc>
