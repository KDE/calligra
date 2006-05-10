// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2002 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2004 Thorsten Zachmann <zachmann@kde.org>

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

#include "KPrSlideTransitionDia.h"

#include <kfiledialog.h>
#include <klocale.h>
#include <knuminput.h>
#include <kstandarddirs.h>
#include <kurlrequester.h>

#include <QCheckBox>
#include <QComboBox>
#include <qimage.h>
#include <QLabel>
#include <QPushButton>

#include "KPrBackground.h"
#include "KPrCanvas.h"
#include "KPrCommand.h"
#include "KPrDocument.h"
#include "KPrSoundPlayer.h"
#include "KPrView.h"
#include "KPrPage.h"
#include "KPrPageEffects.h"
#include "slidetransitionwidget.h"

KPrSlideTransitionDia::KPrSlideTransitionDia( QWidget *parent, const char *name, KPrView *view )
: KDialogBase( parent, name, true, i18n( "Slide Transition" ), User1|Ok|Cancel, Ok, true )
, m_dialog( new SlideTransitionWidget( this ) ), m_view( view ), m_soundPlayer(0), m_pageEffect( 0 )
{
    int pgnum = m_view->getCurrPgNum() - 1;
    KPrPage *page = m_view->kPresenterDoc()->pageList().at( pgnum );
    m_effect = page->getPageEffect();
    m_effectSpeed = page->getPageEffectSpeed();  
    m_soundEffect = page->getPageSoundEffect();
    m_soundFileName = page->getPageSoundFileName();
    m_slideTime = page->getPageTimer();

    // draw preview pix
    m_view->getCanvas()->drawPageInPix( m_pixmap, pgnum, 60 );
    int w = m_pixmap.width();
    int h = m_pixmap.height();

    if ( w > h )
    {
        h = 300 * h / w;
        w = 300;
    }
    else
    {
        w = 300 * w / h;
        h = 300;
    }

    const QImage img(m_pixmap.convertToImage().smoothScale( w, h ));
    m_pixmap.convertFromImage(img);

    m_dialog->previewPixmap->setPixmap( m_pixmap );
    m_dialog->previewPixmap->setFixedSize( w, h );
    m_target.resize( m_pixmap.size() );
    m_target.fill( Qt::black );

    // fill effect combo
    m_dialog->effectCombo->insertItem( i18n( "No Effect" ) );
    m_dialog->effectCombo->insertItem( i18n( "Close Horizontal" ) );
    m_dialog->effectCombo->insertItem( i18n( "Close Vertical" ) );
    m_dialog->effectCombo->insertItem( i18n( "Close From All Directions" ) );
    m_dialog->effectCombo->insertItem( i18n( "Open Horizontal" ) );
    m_dialog->effectCombo->insertItem( i18n( "Open Vertical" ) );
    m_dialog->effectCombo->insertItem( i18n( "Open From All Directions" ) );
    m_dialog->effectCombo->insertItem( i18n( "Interlocking Horizontal 1" ) );
    m_dialog->effectCombo->insertItem( i18n( "Interlocking Horizontal 2" ) );
    m_dialog->effectCombo->insertItem( i18n( "Interlocking Vertical 1" ) );
    m_dialog->effectCombo->insertItem( i18n( "Interlocking Vertical 2" ) );
    m_dialog->effectCombo->insertItem( i18n( "Surround 1" ) );
    m_dialog->effectCombo->insertItem( i18n( "Fly Away 1" ) );
    m_dialog->effectCombo->insertItem( i18n( "Blinds Horizontal" ) );
    m_dialog->effectCombo->insertItem( i18n( "Blinds Vertical" ) );
    m_dialog->effectCombo->insertItem( i18n( "Box In" ) );
    m_dialog->effectCombo->insertItem( i18n( "Box Out" ) );
    m_dialog->effectCombo->insertItem( i18n( "Checkerboard Across" ) );
    m_dialog->effectCombo->insertItem( i18n( "Checkerboard Down" ) );
    m_dialog->effectCombo->insertItem( i18n( "Cover Down" ) );
    m_dialog->effectCombo->insertItem( i18n( "Uncover Down" ) );
    m_dialog->effectCombo->insertItem( i18n( "Cover Up" ) );
    m_dialog->effectCombo->insertItem( i18n( "Uncover Up" ) );
    m_dialog->effectCombo->insertItem( i18n( "Cover Left" ) );
    m_dialog->effectCombo->insertItem( i18n( "Uncover Left" ) );
    m_dialog->effectCombo->insertItem( i18n( "Cover Right" ) );
    m_dialog->effectCombo->insertItem( i18n( "Uncover Right" ) );
    m_dialog->effectCombo->insertItem( i18n( "Cover Left-Up" ) );
    m_dialog->effectCombo->insertItem( i18n( "Uncover Left-Up" ) );
    m_dialog->effectCombo->insertItem( i18n( "Cover Left-Down" ) );
    m_dialog->effectCombo->insertItem( i18n( "Uncover Left-Down" ) );
    m_dialog->effectCombo->insertItem( i18n( "Cover Right-Up" ) );
    m_dialog->effectCombo->insertItem( i18n( "Uncover Right-Up" ) );
    //TODO change the strings to use Down instead of Bottome to make them consistent
    m_dialog->effectCombo->insertItem( i18n( "Cover Right-Bottom" ) );
    m_dialog->effectCombo->insertItem( i18n( "Uncover Right-Bottom" ) );
    m_dialog->effectCombo->insertItem( i18n( "Dissolve" ) );
    m_dialog->effectCombo->insertItem( i18n( "Strips Left-Up" ) );
    m_dialog->effectCombo->insertItem( i18n( "Strips Left-Down" ) );
    m_dialog->effectCombo->insertItem( i18n( "Strips Right-Up" ) );
    m_dialog->effectCombo->insertItem( i18n( "Strips Right-Down" ) );
    m_dialog->effectCombo->insertItem( i18n( "Melting" ) );
    m_dialog->effectCombo->insertItem( i18n( "Random Transition" ) );
    m_dialog->effectCombo->setCurrentItem( static_cast<int>( m_effect ) );

    // workaround, because Random Effect is always negative
    if( m_effect == PEF_RANDOM )
        m_dialog->effectCombo->setCurrentItem( m_dialog->effectCombo->count()-1 );

    connect( m_dialog->effectCombo, SIGNAL( activated( int ) ), this, SLOT( effectChanged( int ) ) );
    
    // setup speed combo
    m_dialog->speedCombo->insertItem( i18n("Slow") );
    m_dialog->speedCombo->insertItem( i18n("Medium") );
    m_dialog->speedCombo->insertItem( i18n("Fast") );

    m_dialog->speedCombo->setCurrentItem( m_effectSpeed );

    connect( m_dialog->speedCombo, SIGNAL( activated( int ) ), this, SLOT( effectChanged( int ) ) );

    // set up sound
    m_dialog->soundCheckBox->setChecked( m_soundEffect );
    m_dialog->soundRequester->setURL( m_soundFileName );
    soundEffectChanged();
    m_dialog->playButton->setIconSet( SmallIconSet( "player_play" ) );
    m_dialog->stopButton->setIconSet( SmallIconSet( "player_stop" ) );
    connect( m_dialog->playButton, SIGNAL( clicked() ), this, SLOT( playSound() ) );
    connect( m_dialog->stopButton, SIGNAL( clicked() ), this, SLOT( stopSound() ) );

    connect( m_dialog->soundCheckBox, SIGNAL( clicked()), this, SLOT( soundEffectChanged() ) );
    connect( m_dialog->soundRequester, SIGNAL( openFileDialog( KUrlRequester * ) ), this, SLOT( slotRequesterClicked( KUrlRequester * ) ) );
    connect( m_dialog->soundRequester, SIGNAL( textChanged( const QString& ) ), this, SLOT( slotSoundFileChanged( const QString& ) ) );

    // set up automatic tansition
    m_dialog->automaticTransitionInput->setRange( 1, 600, 1 );
    m_dialog->automaticTransitionInput->setValue( m_slideTime );

    // set up preview button
    connect( m_dialog->previewButton, SIGNAL( clicked() ), this, SLOT( preview() ) );

    setButtonText(KDialogBase::User1,i18n( "Apply &Global" ));

    connect( this, SIGNAL( okClicked() ), this, SLOT( slotOk() ) );
    connect( this, SIGNAL( user1Clicked() ), this, SLOT( slotUser1() ) );
    
    setMainWidget( m_dialog );
}


KPrSlideTransitionDia::~KPrSlideTransitionDia()
{
    delete m_dialog;
}


void KPrSlideTransitionDia::effectChanged( int )
{
    preview();
}


void KPrSlideTransitionDia::preview()
{
    PageEffect effect = static_cast<PageEffect>( m_dialog->effectCombo->currentItem() );
    if ( m_dialog->effectCombo->currentText() == i18n( "Random Transition" ) )
        effect = PEF_RANDOM;

    EffectSpeed effectSpeed = static_cast<EffectSpeed>( m_dialog->speedCombo->currentItem() );
  
    if ( m_pageEffect )
    {
        m_pageEffectTimer.stop();
        QObject::disconnect( &m_pageEffectTimer, SIGNAL( timeout() ), this, SLOT( slotDoPageEffect() ) );

        m_pageEffect->finish();

        delete m_pageEffect;
        m_pageEffect = 0;
        m_dialog->previewPixmap->repaint();
    }

    m_pageEffect = new KPrPageEffects( m_dialog->previewPixmap, m_target, effect, effectSpeed );
    if ( m_pageEffect->doEffect() )
    {
        delete m_pageEffect;
        m_pageEffect = 0;
        m_dialog->previewPixmap->update();
    }
    else
    {
        connect( &m_pageEffectTimer, SIGNAL( timeout() ), SLOT( slotDoPageEffect() ) );
        m_pageEffectTimer.start( 50, true );
    }
}


void KPrSlideTransitionDia::slotDoPageEffect()
{
    if ( m_pageEffect->doEffect() )
    {
        m_pageEffectTimer.stop();
        QObject::disconnect( &m_pageEffectTimer, SIGNAL( timeout() ), this, SLOT( slotDoPageEffect() ) );
        delete m_pageEffect;
        m_pageEffect = 0;
        m_dialog->previewPixmap->update();
    }
    else
    {
        m_pageEffectTimer.start( 50, true );
    }
}


void KPrSlideTransitionDia::slotRequesterClicked( KUrlRequester * )
{
    QString filter( getSoundFileFilter() );
    m_dialog->soundRequester->fileDialog()->setFilter( filter );

    // find the first "sound"-resource that contains files
    QStringList soundDirs = KGlobal::dirs()->resourceDirs( "sound" );
    if ( !soundDirs.isEmpty() ) {
        KUrl soundURL;
        QDir dir;
        dir.setFilter( QDir::Files | QDir::Readable );
        QStringList::ConstIterator it = soundDirs.begin();
        while ( it != soundDirs.end() ) {
            dir = *it;
            if ( dir.isReadable() && dir.count() > 2 ) {
                soundURL.setPath( *it );
                m_dialog->soundRequester->fileDialog()->setURL( soundURL );
                break;
            }
            ++it;
        }
    }
}


QString KPrSlideTransitionDia::getSoundFileFilter()
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

    str = full + '|' + i18n( "All Supported Files" ) + '\n' + str;
    str += "\n*|" + i18n( "All Files" );

    return str;
}


void KPrSlideTransitionDia::soundEffectChanged()
{
    bool soundEffect = m_dialog->soundCheckBox->isChecked();

    m_dialog->soundLabel->setEnabled( soundEffect );
    m_dialog->soundRequester->setEnabled( soundEffect );

    if ( !m_dialog->soundRequester->url().isEmpty() ) {
        m_dialog->playButton->setEnabled( soundEffect );
        m_dialog->stopButton->setEnabled( soundEffect );
    }
    else {
        m_dialog->playButton->setEnabled( false );
        m_dialog->stopButton->setEnabled( false );
    }
}


void KPrSlideTransitionDia::slotSoundFileChanged( const QString& text )
{
    m_dialog->playButton->setEnabled( !text.isEmpty() );
    m_dialog->stopButton->setEnabled( !text.isEmpty() );
}


void KPrSlideTransitionDia::slotOk()
{
    apply( false );
}


void KPrSlideTransitionDia::slotUser1()
{
    apply( true );
}


void KPrSlideTransitionDia::apply( bool global )
{
    PageEffect effect = static_cast<PageEffect>( m_dialog->effectCombo->currentItem() );
    if ( m_dialog->effectCombo->currentText() == i18n( "Random Transition" ) )
        effect = PEF_RANDOM;

    EffectSpeed effectSpeed = static_cast<EffectSpeed>( m_dialog->speedCombo->currentItem() );
    bool soundEffect = m_dialog->soundCheckBox->isChecked();
    QString soundFileName = m_dialog->soundRequester->url();
    int slideTime = m_dialog->automaticTransitionInput->value();
    
    if ( effect != m_effect ||
         effectSpeed != m_effectSpeed ||
         soundEffect != m_soundEffect ||
         soundFileName != m_soundFileName ||
         slideTime != m_slideTime )
    {
        KPrTransEffectCmd::PageEffectSettings newSettings;
    
        newSettings.pageEffect = effect;
        newSettings.effectSpeed = effectSpeed;
        newSettings.soundEffect = soundEffect;
        newSettings.soundFileName = soundFileName;
        newSettings.autoAdvance = /*TODO */ false;
        newSettings.slideTime = slideTime;

        // Collect info about current settings
        QValueVector<KPrTransEffectCmd::PageEffectSettings> oldSettings;
        KPrPage *page = 0;
        if ( global )
        {
            oldSettings.resize( m_view->kPresenterDoc()->getPageList().count() );
            int i = 0;
            for( QPtrListIterator<KPrPage> it( m_view->kPresenterDoc()->getPageList() ); *it; ++it, ++i )
            {
                oldSettings[i].pageEffect = it.current()->getPageEffect();
                oldSettings[i].effectSpeed = it.current()->getPageEffectSpeed();
                oldSettings[i].soundEffect = it.current()->getPageSoundEffect();
                oldSettings[i].soundFileName = it.current()->getPageSoundFileName();
                oldSettings[i].autoAdvance = /*TODO it.current()->getAutoAdvance() */ false;
                oldSettings[i].slideTime = it.current()->getPageTimer();
            }
        }
        else
        {
            int pgnum = m_view->getCurrPgNum() - 1;
            page = m_view->kPresenterDoc()->pageList().at( pgnum );
            oldSettings.resize( 1 );
            oldSettings[0].pageEffect = page->getPageEffect();
            oldSettings[0].effectSpeed = page->getPageEffectSpeed();
            oldSettings[0].soundEffect = page->getPageSoundEffect();
            oldSettings[0].soundFileName = page->getPageSoundFileName();
            oldSettings[0].autoAdvance = /*TODO page->getAutoAdvance() */ false;
            oldSettings[0].slideTime = page->getPageTimer();
        }
        KPrTransEffectCmd *transEffectCmd = new KPrTransEffectCmd( oldSettings, newSettings,
                                                             global ? 0 : page, m_view->kPresenterDoc() );
        transEffectCmd->execute();
        m_view->kPresenterDoc()->addCommand( transEffectCmd );
    }

    accept();
}


void KPrSlideTransitionDia::playSound()
{
    delete m_soundPlayer;

    m_soundPlayer = new KPrSoundPlayer( m_dialog->soundRequester->url() );
    m_soundPlayer->play();

    m_dialog->playButton->setEnabled( false );
    m_dialog->stopButton->setEnabled( true );
}


void KPrSlideTransitionDia::stopSound()
{
    if ( m_soundPlayer ) {
        m_soundPlayer->stop();
        delete m_soundPlayer;
        m_soundPlayer = 0;

        m_dialog->playButton->setEnabled( true );
        m_dialog->stopButton->setEnabled( false );
    }
}


#include "KPrSlideTransitionDia.moc"
