/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Page Configuration Dialog                              */
/******************************************************************/

#include "pgconfdia.h"
#include "pgconfdia.moc"
#include "kpresenter_doc.h"

#include <qlabel.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qvbox.h>
#include <qlistview.h>
#include <qradiobutton.h>
#include <qchkbox.h>
#include <qcombo.h>
#include <qheader.h>

#include <klocale.h>
#include <kapp.h>
#include <kbuttonbox.h>

/******************************************************************/
/* class PgConfDia                                                */
/******************************************************************/

/*================================================================*/
PgConfDia::PgConfDia( QWidget* parent, KPresenterDoc *doc, const char* name,
                      bool infLoop, bool swMan, int pgNum, PageEffect pageEffect,
                      PresSpeed presSpeed, PresentSlides presSlides,
                      const QMap<int,bool> &selectedSlides )
    : QDialog( parent, name, true )
{
    back = new QVBox( this );
    back->setMargin( 10 );
    back->setSpacing( 5 );

    general = new QButtonGroup( 1, Qt::Horizontal, i18n( "General" ), back, "general" );
    general->setFrameStyle( QFrame::Box | QFrame::Sunken );

    infinitLoop = new QCheckBox( i18n( "&Infinite Loop" ), general );
    infinitLoop->setChecked( infLoop );

    manualSwitch = new QCheckBox( i18n( "&Manual switch to next step" ), general );
    manualSwitch->setChecked( swMan );

    label4 = new QLabel( i18n( "Speed of the presentation:" ), general );

    speedCombo = new QComboBox( false, general );
    speedCombo->insertItem( i18n( "Slow" ) );
    speedCombo->insertItem( i18n( "Normal" ) );
    speedCombo->insertItem( i18n( "Fast" ) );
    speedCombo->setCurrentItem( static_cast<int>( presSpeed ) );

    page = new QButtonGroup( 1, Qt::Horizontal, i18n( "Page Configuration" ), back, "page" );
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

    slides = new QButtonGroup( 1, Qt::Horizontal, back );
    slides->setCaption( i18n( "Show slides in presentation" ) );

    slidesAll = new QRadioButton( i18n( "&All slides" ), slides );
    slidesCurrent = new QRadioButton( i18n( "&Current slide" ), slides );
    slidesSelected = new QRadioButton( i18n( "&Selected slides" ), slides );

    switch ( presSlides ) {
    case PS_ALL:
        slidesAll->setChecked( true );
        break;
    case PS_CURRENT:
        slidesCurrent->setChecked( true );
        break;
    case PS_SELECTED:
        slidesSelected->setChecked( true );
        break;
    }

    lSlides = new QListView( slides );
    lSlides->addColumn( i18n( "Slide Nr." ) );
    lSlides->addColumn( i18n( "Slide Title" ) );
    lSlides->header()->setMovingEnabled( false );
    lSlides->setSorting( -1 );
    
    for ( int i = doc->getPageNums() - 1; i >= 0; --i ) {
        QCheckListItem *item = new QCheckListItem( lSlides, "", QCheckListItem::CheckBox );
        item->setText( 0, QString( "%1" ).arg( i + 1 ) );
        item->setText( 1, doc->getPageTitle( i, i18n( "Slide %1" ).arg( i +1 ) ) );
        if ( selectedSlides.contains( i ) )
            item->setOn( *( selectedSlides.find( i ) ) );
    }

    connect( slides, SIGNAL( clicked( int ) ),
             this, SLOT( presSlidesChanged( int ) ) );

    KButtonBox *bb = new KButtonBox( back );

    bb->addStretch();
    okBut = bb->addButton( i18n( "OK" ) );
    okBut->setDefault( true );
    cancelBut = bb->addButton( i18n( "Cancel" ) );
    bb->layout();

    bb->setMaximumHeight( okBut->sizeHint().height() + 5 );

    connect( okBut, SIGNAL( clicked() ), this, SLOT( confDiaOk() ) );
    connect( cancelBut, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( okBut, SIGNAL( clicked() ), this, SLOT( accept() ) );

    resize( 530, 550 );

    presSlidesChanged( 0 );
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
void PgConfDia::resizeEvent( QResizeEvent *e )
{
    QDialog::resizeEvent( e );
    back->resize( size() );
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
    return static_cast<PresSpeed>( speedCombo->currentItem() );
}

/*================================================================*/
PresentSlides PgConfDia::getPresentSlides()
{
    if ( slidesAll->isChecked() )
        return PS_ALL;
    else if ( slidesCurrent->isChecked() )
        return PS_CURRENT;
    else if ( slidesSelected->isChecked() )
        return PS_SELECTED;

    return PS_ALL;
}

/*================================================================*/
QMap<int,bool> PgConfDia::getSelectedSlides()
{
    QMap<int,bool> m;
    QListViewItemIterator it( lSlides );
    for ( ; it.current(); ++it )
        m.insert( it.current()->text( 0 ).toInt() - 1,
                  dynamic_cast<QCheckListItem*>( it.current() )->isOn() );

    return m;
}
