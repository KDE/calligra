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
#include <kpresenter_sound_player.h>

#include <qlabel.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qheader.h>
#include <qstringlist.h>
#include <qdir.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

#include <klocale.h>
#include <kbuttonbox.h>
#include <knuminput.h>
#include <kglobal.h>
#include <kurlrequester.h>
#include <kurl.h>
#include <kstandarddirs.h>
#include <kfiledialog.h>

/******************************************************************/
/* class PgConfDia                                                */
/******************************************************************/

/*================================================================*/
PgConfDia::PgConfDia( QWidget* parent, const char* name,
                      bool infLoop, bool swMan, int pgNum, PageEffect pageEffect,
                      PresSpeed presSpeed, int pageTimer, bool soundEffect, QString fileName,
                      bool showPresentationDuration )
    : KDialogBase( parent, name, true, "",Ok|Cancel )
{
    soundPlayer = 0;

    QWidget *page = new QWidget( this );
    setMainWidget(page);
    QVBoxLayout *back = new QVBoxLayout( page, 0, spacingHint() );

    general = new QButtonGroup( 1, Qt::Horizontal, i18n( "General" ), page, "general" );
    general->setFrameStyle( QFrame::Box | QFrame::Sunken );

    infinitLoop = new QCheckBox( i18n( "&Infinite loop" ), general );
    infinitLoop->setChecked( infLoop );

    manualSwitch = new QCheckBox( i18n( "&Manual switch to next step" ), general );
    manualSwitch->setChecked( swMan );
    connect( manualSwitch, SIGNAL( clicked() ), this, SLOT( slotManualSwitch() ) );

    speedOfObject = new KIntNumInput( presSpeed, general );
    speedOfObject->setRange( 1, 10, 1 );
    speedOfObject->setLabel( i18n( "Effect speed of the object and page:" ) );

    presentationDuration = new QCheckBox( i18n( "&Show presentation duration" ), general );
    presentationDuration->setChecked( showPresentationDuration );

    back->addWidget(general);


    QGroupBox *grp = new QGroupBox( i18n( "Configuration Page %1" ).arg( pgNum ), page );
    back->addWidget( grp );
    QGridLayout *grid = new QGridLayout( grp, 4, 4, 15 );

    label2 = new QLabel( i18n( "Effect for changing to next page:" ), grp );
    label2->setAlignment( AlignVCenter );
    grid->addWidget( label2, 1, 0 );

    effectCombo = new QComboBox( false, grp );
    effectCombo->insertItem( i18n( "No Effect" ) );
    effectCombo->insertItem( i18n( "Close Horizontal" ) );
    effectCombo->insertItem( i18n( "Close Vertical" ) );
    effectCombo->insertItem( i18n( "Close From All Directions" ) );
    effectCombo->insertItem( i18n( "Open Horizontal" ) );
    effectCombo->insertItem( i18n( "Open Vertical" ) );
    effectCombo->insertItem( i18n( "Open From All Directions" ) );
    effectCombo->insertItem( i18n( "Interlocking Horizontal 1" ) );
    effectCombo->insertItem( i18n( "Interlocking Horizontal 2" ) );
    effectCombo->insertItem( i18n( "Interlocking Vertical 1" ) );
    effectCombo->insertItem( i18n( "Interlocking Vertical 2" ) );
    effectCombo->insertItem( i18n( "Surround 1" ) );
    effectCombo->insertItem( i18n( "Fly Away 1" ) );
    effectCombo->insertItem( i18n( "Blinds Horizontal" ) );
    effectCombo->insertItem( i18n( "Blinds Vertical" ) );
    effectCombo->insertItem( i18n( "Box In" ) );
    effectCombo->insertItem( i18n( "Box Out" ) );
    effectCombo->insertItem( i18n( "Checkerboard Across" ) );
    effectCombo->insertItem( i18n( "Checkerboard Down" ) );
    effectCombo->insertItem( i18n( "Cover Down" ) );
    effectCombo->insertItem( i18n( "Uncover Down" ) );
    effectCombo->insertItem( i18n( "Cover Up" ) );
    effectCombo->insertItem( i18n( "Uncover Up" ) );
    effectCombo->insertItem( i18n( "Cover Left" ) );
    effectCombo->insertItem( i18n( "Uncover Left" ) );
    effectCombo->insertItem( i18n( "Cover Right" ) );
    effectCombo->insertItem( i18n( "Uncover Right" ) );
    effectCombo->insertItem( i18n( "Cover Left-Up" ) );
    effectCombo->insertItem( i18n( "Uncover Left-Up" ) );
    effectCombo->insertItem( i18n( "Cover Left-Down" ) );
    effectCombo->insertItem( i18n( "Uncover Left-Down" ) );
    effectCombo->insertItem( i18n( "Cover Right-Up" ) );
    effectCombo->insertItem( i18n( "Uncover Right-Up" ) );
    effectCombo->insertItem( i18n( "Cover Right-Bottom" ) );
    effectCombo->insertItem( i18n( "Uncover Right-Bottom" ) );
    effectCombo->insertItem( i18n( "Dissolve" ) );
    effectCombo->insertItem( i18n( "Strips Left-Up" ) );
    effectCombo->insertItem( i18n( "Strips Left-Down" ) );
    effectCombo->insertItem( i18n( "Strips Right-Up" ) );
    effectCombo->insertItem( i18n( "Strips Right-Down" ) );
    effectCombo->insertItem( i18n( "Random Transition" ) );
    effectCombo->setCurrentItem( static_cast<int>( pageEffect ) );
    grid->addWidget( effectCombo, 1, 1 );

    // workaround, because Random Effect is always negative
    if( pageEffect == PEF_RANDOM )
      effectCombo->setCurrentItem( effectCombo->count() );

    connect( effectCombo, SIGNAL( activated( int ) ), this, SLOT( effectChanged( int ) ) );

    lTimer = new QLabel( i18n( "Timer of the page:" ), grp );
    lTimer->setAlignment( AlignVCenter );
    grid->addWidget( lTimer, 2, 0 );

    if ( swMan )
        lTimer->setEnabled( false );

    timerOfPage = new KIntNumInput( pageTimer, grp );
    timerOfPage->setRange( 1, 600, 1 );
    timerOfPage->setSuffix( i18n( " seconds" ) );
    grid->addWidget( timerOfPage, 2, 1 );

    if ( swMan )
        timerOfPage->setEnabled( false );


    // setup the Sound Effect stuff
    checkSoundEffect = new QCheckBox( i18n( "Sound effect" ), grp );
    checkSoundEffect->setChecked( soundEffect );
    QWhatsThis::add( checkSoundEffect, i18n("If you use a sound effect, please do not select No Effect.") );
    grid->addWidget( checkSoundEffect, 3, 0 );

    if ( static_cast<int>( pageEffect ) == 0 )
        checkSoundEffect->setEnabled( false );

    connect( checkSoundEffect, SIGNAL( clicked() ), this, SLOT( soundEffectChanged() ) );

    lSoundEffect = new QLabel( i18n( "File name:" ), grp );
    lSoundEffect->setAlignment( AlignVCenter );
    grid->addWidget( lSoundEffect, 4, 0 );

    requester = new KURLRequester( grp );
    requester->setURL( fileName );
    grid->addWidget( requester, 4, 1 );

    connect( requester, SIGNAL( openFileDialog( KURLRequester * ) ),
             this, SLOT( slotRequesterClicked( KURLRequester * ) ) );

    connect( requester, SIGNAL( textChanged( const QString& ) ),
             this, SLOT( slotSoundFileChanged( const QString& ) ) );

    buttonTestPlaySoundEffect = new QPushButton( grp );
    buttonTestPlaySoundEffect->setPixmap( BarIcon("1rightarrow", KIcon::SizeSmall) );
    QToolTip::add( buttonTestPlaySoundEffect, i18n("Play") );
    grid->addWidget( buttonTestPlaySoundEffect, 4, 2 );

    connect( buttonTestPlaySoundEffect, SIGNAL( clicked() ), this, SLOT( playSound() ) );

    buttonTestStopSoundEffect = new QPushButton( grp );
    buttonTestStopSoundEffect->setPixmap( BarIcon("player_stop", KIcon::SizeSmall) );
    QToolTip::add( buttonTestStopSoundEffect, i18n("Stop") );
    grid->addWidget( buttonTestStopSoundEffect, 4, 3 );

    connect( buttonTestStopSoundEffect, SIGNAL( clicked() ), this, SLOT( stopSound() ) );



    slides = new QButtonGroup( 1, Qt::Horizontal, this );
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


    connect( this, SIGNAL( okClicked() ), this, SLOT( confDiaOk() ) );
    connect( this, SIGNAL( okClicked() ), this, SLOT( accept() ) );


    soundEffectChanged();

    //presSlidesChanged( 0 );
}

/*================================================================*/
PgConfDia::~PgConfDia()
{
    stopSound();
    delete soundPlayer;
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
bool PgConfDia::getInfinitLoop() const
{
    return infinitLoop->isChecked();
}

/*================================================================*/
bool PgConfDia::getManualSwitch() const
{
    return manualSwitch->isChecked();
}

/*================================================================*/
PageEffect PgConfDia::getPageEffect() const
{
    if( effectCombo->currentText() == i18n( "Random Transition" ) )
      return PEF_RANDOM;
    else
      return static_cast<PageEffect>( effectCombo->currentItem() );
}

/*================================================================*/
PresSpeed PgConfDia::getPresSpeed() const
{
    return speedOfObject->value();
}

/*================================================================*/
int PgConfDia::getPageTimer() const
{
    return timerOfPage->value();
}

/*================================================================*/
bool PgConfDia::getPageSoundEffect() const
{
    return checkSoundEffect->isChecked();
}

/*================================================================*/
QString PgConfDia::getPageSoundFileName() const
{
    return requester->url();
}

bool PgConfDia::getPresentationDuration() const
{
    return presentationDuration->isChecked();
}

/*================================================================*/
void PgConfDia::slotManualSwitch()
{
    if ( manualSwitch->isChecked() ) {
        lTimer->setEnabled( false );
        timerOfPage->setEnabled( false );
    }
    else {
        lTimer->setEnabled( true );
        timerOfPage->setEnabled( true );
    }
}

/*================================================================*/
void PgConfDia::effectChanged( int num )
{
    if ( num == 0 ) {
        checkSoundEffect->setEnabled( false );
        lSoundEffect->setEnabled( false );
        requester->setEnabled( false );
        buttonTestPlaySoundEffect->setEnabled( false );
        buttonTestStopSoundEffect->setEnabled( false );
    }
    else {
        checkSoundEffect->setEnabled( true );
        soundEffectChanged();
    }
}

/*================================================================*/
void PgConfDia::soundEffectChanged()
{
    lSoundEffect->setEnabled( checkSoundEffect->isChecked() );
    requester->setEnabled( checkSoundEffect->isChecked() );

    if ( !requester->url().isEmpty() ) {
        buttonTestPlaySoundEffect->setEnabled( checkSoundEffect->isChecked() );
        buttonTestStopSoundEffect->setEnabled( checkSoundEffect->isChecked() );
    }
    else {
        buttonTestPlaySoundEffect->setEnabled( false );
        buttonTestStopSoundEffect->setEnabled( false );
    }
}

/*================================================================*/
void PgConfDia::slotRequesterClicked( KURLRequester *requester )
{
    QString filter = getSoundFileFilter();
    requester->fileDialog()->setFilter( filter );

    // find the first "sound"-resource that contains files
    QStringList soundDirs = KGlobal::dirs()->resourceDirs( "sound" );
    if ( !soundDirs.isEmpty() ) {
	KURL soundURL;
	QDir dir;
	dir.setFilter( QDir::Files | QDir::Readable );
	QStringList::ConstIterator it = soundDirs.begin();
	while ( it != soundDirs.end() ) {
	    dir = *it;
	    if ( dir.isReadable() && dir.count() > 2 ) {
		soundURL.setPath( *it );
		requester->fileDialog()->setURL( soundURL );
		break;
	    }
	    ++it;
	}
    }
}

/*================================================================*/
void PgConfDia::slotSoundFileChanged( const QString &text )
{
    buttonTestPlaySoundEffect->setEnabled( !text.isEmpty() );
    buttonTestStopSoundEffect->setEnabled( !text.isEmpty() );
}

/*================================================================*/
void PgConfDia::playSound()
{
    delete soundPlayer;
    soundPlayer = new KPresenterSoundPlayer( requester->url() );
    soundPlayer->play();

    buttonTestPlaySoundEffect->setEnabled( false );
    buttonTestStopSoundEffect->setEnabled( true );
}

/*================================================================*/
void PgConfDia::stopSound()
{
    if ( soundPlayer ) {
        soundPlayer->stop();
        delete soundPlayer;
        soundPlayer = 0;

        buttonTestPlaySoundEffect->setEnabled( true );
        buttonTestStopSoundEffect->setEnabled( false );
    }
}

/*================================================================*/
QString PgConfDia::getSoundFileFilter() const
{
    QStringList fileList;
    fileList << "wav" << "au" << "mp3" << "mp1" << "mp2" << "mpg" << "dat"
             << "mpeg" << "ogg" << "cdda" << "cda " << "vcd" << "null";
    fileList.sort();

    bool comma = false;
    QString full, str;
    for ( QStringList::ConstIterator it = fileList.begin(); it != fileList.end(); ++it ) {
        if ( comma )
            str += '\n';
        comma = true;
        str += QString( i18n( "*.%1|%2 Files" ) ).arg( *it ).arg( (*it).upper() );

        full += QString( "*.") + (*it) + ' ';
    }

    str = full + '|' + i18n( "All supported files" ) + '\n' + str;
    str += "\n*|" + i18n( "All files" );

    return str;
}

#include <pgconfdia.moc>
