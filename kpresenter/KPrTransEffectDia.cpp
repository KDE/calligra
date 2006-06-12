/* This file is part of the KDE project
   Copyright (C) 2002 Ariya Hidayat <ariya@kde.org>

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

#include "KPrTransEffectDia.h"

#include "global.h"
#include "KPrBackground.h"
#include "KPrObject.h"
#include "KPrTextObject.h"
#include "KPrView.h"
#include "KPrDocument.h"
#include "KPrCanvas.h"
#include "KPrPage.h"
#include "KPrPageEffects.h"
#include "KPrSoundPlayer.h"

#include <qsplitter.h>
#include <q3header.h>
#include <qmatrix.h>

#include <QLayout>
#include <qimage.h>
#include <QPushButton>
#include <QCheckBox>
#include <qslider.h>
#include <QToolTip>
#include <q3whatsthis.h>
#include <q3frame.h>
#include <QLabel>
#include <QComboBox>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <QPixmap>
#include <Q3VBoxLayout>

#include <kdebug.h>
#include <kurlrequester.h>
#include <klocale.h>
#include <knuminput.h>
#include <kiconloader.h>
#include <kurl.h>
#include <kstandarddirs.h>
#include <kfiledialog.h>
#include <kvbox.h>


KPrEffectPreview::KPrEffectPreview( QWidget *parent, KPrDocument *_doc, KPrView *_view )
    : QLabel( parent ), doc( _doc ), view( _view ), m_pageEffect( 0 )
{
    setFrameStyle( StyledPanel | Sunken );
}

void KPrEffectPreview::setPixmap( const QPixmap& pixmap )
{
    // find the right size
    QRect rect = pixmap.rect();
    int w = rect.width();
    int h = rect.height();
    if ( w > h ) {
        w = 297;
        h = 210;
    }
    else if ( w < h ) {
        w = 210;
        h = 297;
    }
    else if ( w == h ) {
        w = 297;
        h = 297;
    }

    setMinimumSize( w, h );

    // create smaller pixmap for preview
    const QImage img( pixmap.convertToImage().smoothScale( w, h, Qt::IgnoreAspectRatio ) );
    m_pixmap.convertFromImage( img );

    QLabel::setPixmap( m_pixmap );
}

void KPrEffectPreview::run( PageEffect effect, EffectSpeed speed )
{
    QRect rect = m_pixmap.rect();
    m_target.resize( rect.size() );
    m_target.fill( Qt::black );

    //kPchangePages( this, m_pixmap, m_target, effect, speed );
    if ( m_pageEffect )
    {
        m_pageEffectTimer.stop();
        QObject::disconnect( &m_pageEffectTimer, SIGNAL( timeout() ), this, SLOT( slotDoPageEffect() ) );

        m_pageEffect->finish();

        delete m_pageEffect;
        m_pageEffect = 0;
        QLabel::repaint();
    }

    m_pageEffect = new KPrPageEffects( this, m_target, effect, speed );
    if ( m_pageEffect->doEffect() )
    {
        delete m_pageEffect;
        m_pageEffect = 0;
        QLabel::update();
    }
    else
    {
        connect( &m_pageEffectTimer, SIGNAL( timeout() ), SLOT( slotDoPageEffect() ) );
        m_pageEffectTimer.start( 50, true );
    }

    //QLabel::update();
}


void KPrEffectPreview::slotDoPageEffect()
{
    if ( m_pageEffect->doEffect() )
    {
        m_pageEffectTimer.stop();
        QObject::disconnect( &m_pageEffectTimer, SIGNAL( timeout() ), this, SLOT( slotDoPageEffect() ) );
        delete m_pageEffect;
        m_pageEffect = 0;
        QLabel::update();
    }
    else
    {
        m_pageEffectTimer.start( 50, true );
    }
}


KPrTransEffectDia::KPrTransEffectDia( QWidget *parent, const char *name,
                                    KPrDocument *_doc, KPrView *_view )
    : KDialogBase( parent, name, true, "", KDialogBase::User1|Ok|Cancel ),
      doc( _doc ), view( _view ), soundPlayer( 0 )
{
    enableButtonSeparator( true );

    QWidget *page = new QWidget( this );
    setMainWidget(page);

    Q3BoxLayout *topLayout = new Q3HBoxLayout( page, KDialog::marginHint(), KDialog::spacingHint() );
    QWidget* leftpart = new QWidget( page );
    topLayout->addWidget( leftpart );
    QWidget* rightpart = new QWidget( page );
    topLayout->addWidget( rightpart );

    // right-side of the dialog, for showing effect preview

    Q3VBoxLayout *rightlayout = new Q3VBoxLayout( rightpart, KDialog::marginHint(), KDialog::spacingHint() );
    rightlayout->setAutoAdd( true );

    effectPreview = new KPrEffectPreview( rightpart, doc, view );

    int pgnum = view->getCurrPgNum() - 1;  // getCurrPgNum() is 1-based
    KPrPage* pg = doc->pageList().at( pgnum );

    // pixmap for effect preview
    QRect rect= pg->getZoomPageRect();
    QPixmap pix( rect.size() );
    pix.fill( Qt::white );
    view->getCanvas()->drawPageInPix( pix, pgnum, 100 );
    effectPreview->setPixmap( pix );

    pageEffect = pg->getPageEffect();
    speed = pg->getPageEffectSpeed();

    Q3VBoxLayout *leftlayout = new Q3VBoxLayout( leftpart, KDialog::marginHint(), KDialog::spacingHint() );
    leftlayout->setAutoAdd( true );

    new QLabel( i18n("Effect:"), leftpart );

    effectList = new Q3ListBox( leftpart );
    effectList->insertItem( i18n( "No Effect" ) );
    effectList->insertItem( i18n( "Close Horizontal" ) );
    effectList->insertItem( i18n( "Close Vertical" ) );
    effectList->insertItem( i18n( "Close From All Directions" ) );
    effectList->insertItem( i18n( "Open Horizontal" ) );
    effectList->insertItem( i18n( "Open Vertical" ) );
    effectList->insertItem( i18n( "Open From All Directions" ) );
    effectList->insertItem( i18n( "Interlocking Horizontal 1" ) );
    effectList->insertItem( i18n( "Interlocking Horizontal 2" ) );
    effectList->insertItem( i18n( "Interlocking Vertical 1" ) );
    effectList->insertItem( i18n( "Interlocking Vertical 2" ) );
    effectList->insertItem( i18n( "Surround 1" ) );
    effectList->insertItem( i18n( "Fly Away 1" ) );
    effectList->insertItem( i18n( "Blinds Horizontal" ) );
    effectList->insertItem( i18n( "Blinds Vertical" ) );
    effectList->insertItem( i18n( "Box In" ) );
    effectList->insertItem( i18n( "Box Out" ) );
    effectList->insertItem( i18n( "Checkerboard Across" ) );
    effectList->insertItem( i18n( "Checkerboard Down" ) );
    effectList->insertItem( i18n( "Cover Down" ) );
    effectList->insertItem( i18n( "Uncover Down" ) );
    effectList->insertItem( i18n( "Cover Up" ) );
    effectList->insertItem( i18n( "Uncover Up" ) );
    effectList->insertItem( i18n( "Cover Left" ) );
    effectList->insertItem( i18n( "Uncover Left" ) );
    effectList->insertItem( i18n( "Cover Right" ) );
    effectList->insertItem( i18n( "Uncover Right" ) );
    effectList->insertItem( i18n( "Cover Left-Up" ) );
    effectList->insertItem( i18n( "Uncover Left-Up" ) );
    effectList->insertItem( i18n( "Cover Left-Down" ) );
    effectList->insertItem( i18n( "Uncover Left-Down" ) );
    effectList->insertItem( i18n( "Cover Right-Up" ) );
    effectList->insertItem( i18n( "Uncover Right-Up" ) );
    effectList->insertItem( i18n( "Cover Right-Bottom" ) );
    effectList->insertItem( i18n( "Uncover Right-Bottom" ) );
    effectList->insertItem( i18n( "Dissolve" ) );
    effectList->insertItem( i18n( "Strips Left-Up" ) );
    effectList->insertItem( i18n( "Strips Left-Down" ) );
    effectList->insertItem( i18n( "Strips Right-Up" ) );
    effectList->insertItem( i18n( "Strips Right-Down" ) );
    effectList->insertItem( i18n( "Melting" ) );
    effectList->insertItem( i18n( "Random Transition" ) );
    effectList->setCurrentItem( static_cast<int>( pageEffect ) );

    // workaround, because Random Effect is always negative
    if( pageEffect == PEF_RANDOM )
        effectList->setCurrentItem( effectList->count()-1 );

    connect( effectList, SIGNAL(highlighted(int)), this, SLOT(effectChanged(int)) );
    connect( effectList, SIGNAL( doubleClicked ( Q3ListBoxItem *) ), this, SLOT( effectChanged()) );

    new QLabel( i18n("Speed:"), leftpart );

    QWidget* sp = new QWidget( leftpart );
    Q3BoxLayout* speedLayout = new Q3HBoxLayout( sp, KDialog::marginHint(), KDialog::spacingHint() );
    speedLayout->setAutoAdd( true );

    speedCombo = new QComboBox( sp );
    speedCombo->insertItem(i18n("Slow") );
    speedCombo->insertItem(i18n("Medium") );
    speedCombo->insertItem(i18n("Fast") );


    speedCombo->setCurrentItem( speed );

    connect( speedCombo, SIGNAL(activated(int)), this, SLOT(speedChanged(int)) );


    QWidget* previewgrp = new QWidget( leftpart );
    Q3BoxLayout* previewLayout = new Q3HBoxLayout( previewgrp, KDialog::marginHint(), KDialog::spacingHint() );
    previewLayout->setAutoAdd( true );

    automaticPreview = new QCheckBox( i18n( "Automatic preview" ), previewgrp );
    automaticPreview->setChecked( true );

    QWidget* previewspacer = new QWidget( previewgrp );
    previewspacer->setSizePolicy( QSizePolicy( QSizePolicy::Expanding,
                                               QSizePolicy::Expanding ) );

    previewButton = new QPushButton( previewgrp );
    previewButton->setText( i18n("Preview") );
    connect( previewButton, SIGNAL(clicked()), this, SLOT(preview()) );

    Q3Frame* line = new Q3Frame( leftpart );
    line->setFrameStyle( Q3Frame::HLine | Q3Frame::Sunken );

    soundFileName = pg->getPageSoundFileName();
    soundEffect = pg->getPageSoundEffect();

    checkSoundEffect = new QCheckBox( i18n( "Sound effect" ), leftpart );
    checkSoundEffect->setChecked( soundEffect );
    connect( checkSoundEffect, SIGNAL( clicked() ), this, SLOT( soundEffectChanged() ) );

    QWidget* soundgrp = new QWidget( leftpart );
    Q3BoxLayout* soundLayout = new Q3HBoxLayout( soundgrp, KDialog::marginHint(), KDialog::spacingHint() );
    soundLayout->setAutoAdd( true );

    lSoundEffect = new QLabel( i18n( "File name:" ), soundgrp );
    requester = new KUrlRequester( soundgrp );
    requester->setUrl( soundFileName );
    connect( requester, SIGNAL( openFileDialog( KUrlRequester * ) ),
             this, SLOT( slotRequesterClicked( KUrlRequester * ) ) );
    connect( requester, SIGNAL( textChanged( const QString& ) ),
             this, SLOT( slotSoundFileChanged( const QString& ) ) );

    buttonTestPlaySoundEffect = new QPushButton( soundgrp );
    buttonTestPlaySoundEffect->setPixmap( BarIcon("1rightarrow", K3Icon::SizeSmall) );
    buttonTestPlaySoundEffect->setToolTip( i18n("Play") );

    connect( buttonTestPlaySoundEffect, SIGNAL( clicked() ), this, SLOT( playSound() ) );

    buttonTestStopSoundEffect = new QPushButton( soundgrp );
    buttonTestStopSoundEffect->setPixmap( BarIcon("player_stop", K3Icon::SizeSmall) );
    buttonTestStopSoundEffect->setToolTip( i18n("Stop") );

    connect( buttonTestStopSoundEffect, SIGNAL( clicked() ), this, SLOT( stopSound() ) );

    soundEffect = pg->getPageSoundEffect();
    setButtonText(KDialogBase::User1,i18n( "Apply &Global" ));

    slideTime = pg->getPageTimer();

    new QLabel( i18n("Automatically advance to the next slide after:"), rightpart );

    timeSlider = new KIntNumInput( slideTime, rightpart );
    timeSlider->setRange( 1, 600, 1 );
    timeSlider->setSuffix( i18n( " seconds" ) );
    connect( timeSlider, SIGNAL(valueChanged(int)), this, SLOT(timeChanged(int)) );

    QWidget* rspacer = new QWidget( rightpart );
    rspacer->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding ) );

    QWidget* lspacer = new QWidget( leftpart );
    lspacer->setMinimumSize( 10, spacingHint() );

    soundEffectChanged();
}

void KPrTransEffectDia::preview()
{
    if( pageEffect==PEF_NONE)
        return;
    effectPreview->run( pageEffect, speed );
}

void KPrTransEffectDia::effectChanged()
{
    effectChanged( effectList->currentItem() );
}

void KPrTransEffectDia::effectChanged( int index )
{
    if( effectList->currentText() == i18n( "Random Transition" ) )
        pageEffect = PEF_RANDOM;
    else
        pageEffect =  static_cast<PageEffect>( index );

    if( automaticPreview->isChecked() ) preview();
}

void KPrTransEffectDia::speedChanged( int value )
{
    speed = static_cast<EffectSpeed>(value);
}

void KPrTransEffectDia::timeChanged( int value )
{
    if( value <= 0 ) value = 1;
    slideTime = value;
}

void KPrTransEffectDia::soundEffectChanged()
{
    soundEffect = checkSoundEffect->isChecked();

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

static QString getSoundFileFilter()
{
    QStringList fileList;
    fileList << "wav" << "au" << "mp3" << "mp1" << "mp2" << "mpg" << "dat"
             << "mpeg" << "ogg" << "cdda" << "cda " << "vcd" << "null";
    fileList.sort();

    bool comma = false;
    QString full, str;
    QStringList::ConstIterator end( fileList.end() );
    for ( QStringList::ConstIterator it = fileList.begin(); it != end; ++it ) {
        if ( comma )
            str += '\n';
        comma = true;
        str +=  i18n( "*.%1|%2 Files" , *it , (*it).upper() );

        full += QString( "*.") + (*it) + ' ';
    }

    str = full + '|' + i18n( "All Supported Files" ) + '\n' + str;
    str += "\n*|" + i18n( "All Files" );

    return str;
}

void KPrTransEffectDia::slotRequesterClicked( KUrlRequester * )
{
    QString filter = getSoundFileFilter();
    requester->fileDialog()->setFilter( filter );

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
                requester->fileDialog()->setURL( soundURL );
                break;
            }
            ++it;
        }
    }
}

void KPrTransEffectDia::slotSoundFileChanged( const QString& text )
{
    soundFileName = text;

    buttonTestPlaySoundEffect->setEnabled( !text.isEmpty() );
    buttonTestStopSoundEffect->setEnabled( !text.isEmpty() );
}

void KPrTransEffectDia::playSound()
{
    delete soundPlayer;
    soundPlayer = new KPrSoundPlayer( requester->url() );
    soundPlayer->play();

    buttonTestPlaySoundEffect->setEnabled( false );
    buttonTestStopSoundEffect->setEnabled( true );
}

void KPrTransEffectDia::stopSound()
{
    if ( soundPlayer ) {
        soundPlayer->stop();
        delete soundPlayer;
        soundPlayer = 0;

        buttonTestPlaySoundEffect->setEnabled( true );
        buttonTestStopSoundEffect->setEnabled( false );
    }
}

void KPrTransEffectDia::slotOk()
{
    // TODO: only if changed. And pass flags for which settings changed
    emit apply( false );
}

void KPrTransEffectDia::slotUser1()
{
    // TODO: only if changed. And pass flags for which settings changed
    emit apply( true );
}

#include "KPrTransEffectDia.moc"
