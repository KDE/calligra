// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright 2004 Brad Hards <bradh@frogmouth.net>
   Loosely based on webpresention.cc, which is:
     Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
     Copyright 2001, 2002 Nicolas GOUTTE <goutte@kde.org>
     Copyright 2002 Ariya Hidayat <ariya@kde.org>

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

#include "KPrMSPresentationSetup.h"

#include "KPrView.h"
#include "KPrDocument.h"

#include "KPrCanvas.h"
#include "KPrPage.h"

#include <kio/netaccess.h>
#include <ktempfile.h>

#include <QDir>
#include <q3frame.h>
#include <qfileinfo.h>
#include <q3hbox.h>
#include <QLabel>
#include <QLayout>
#include <qpixmap.h>
#include <qpainter.h>
#include <QPushButton>
#include <q3vbox.h>
#include <q3whatsthis.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <QResizeEvent>
#include <Q3VBoxLayout>

#include <kapplication.h>
#include <kbuttonbox.h>
#include <kcolorbutton.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <klineedit.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kprogressbar.h>
#include <kstandarddirs.h>
#include <kurlrequester.h>
#include <kstdguiitem.h>
#include <kpushbutton.h>
#include <ktoolinvocation.h>

KPrMSPresentation::KPrMSPresentation( KPrDocument *_doc, KPrView *_view )
{
    doc = _doc;
    view = _view;
    init();
}

KPrMSPresentation::KPrMSPresentation( const KPrMSPresentation &msPres )
    : title( msPres.title ),
      slideInfos( msPres.slideInfos ), backColour( msPres.backColour ),
      textColour( msPres.textColour ), path( msPres.path )
{
    doc = msPres.doc;
    view = msPres.view;
}

void KPrMSPresentation::initCreation( KProgress *progressBar )
{
    int p;

    // the title images, and the subsequent real images need to
    // be in a file under DCIM/, and they start at 101MSPJP, so
    // we create the DCIM file, and then iterate through looking
    // for the next "available" directory slot
    KUrl str(  path + "/DCIM"  );
    KIO::NetAccess::mkdir( str,( QWidget* )0L  );

    p = progressBar->progress();
    progressBar->setProgress( ++p );
    kapp->processEvents();

    for (int dirNum = 101; dirNum < 999; dirNum++) {
        slidePath = QString("/DCIM/%1MSPJP").arg(dirNum);
        if (! KIO::NetAccess::exists(( path + slidePath), true, ( QWidget* )0L) )
            break;
    }

    p = progressBar->progress();
    progressBar->setProgress( ++p );
    kapp->processEvents();

    str = (  path + slidePath );
    KIO::NetAccess::mkdir( str,( QWidget* )0L  );
    p = progressBar->progress();
    progressBar->setProgress( ++p );
    kapp->processEvents();

    // now do the SPP file directory
    str = (  path + "/MSSONY" );
    KIO::NetAccess::mkdir( str,( QWidget* )0L  );

    p = progressBar->progress();
    progressBar->setProgress( ++p );
    kapp->processEvents();
    str = (  path + "/MSSONY/PJ" );
    KIO::NetAccess::mkdir( str,( QWidget* )0L  );


    p = progressBar->progress();
    progressBar->setProgress( ++p );
    kapp->processEvents();

    // create the title slides
    QPixmap titleSlide( 1024, 768 );
    titleSlide.fill( backColour );
    QPainter painter( &titleSlide );

    //the second title is just blank, so create that now
    KTempFile tmp;
    QString filename = path + slidePath + "/SPJT0002.JPG";
    titleSlide.save( tmp.name(), "JPEG" );
    KIO::NetAccess::file_move( tmp.name(), filename, -1, true /*overwrite*/);

    p = progressBar->progress();
    progressBar->setProgress( ++p );
    kapp->processEvents();

    // and put the specified title string on the first slide
    QFont textFont( "SansSerif", 96 );
    painter.setFont( textFont );
    painter.setPen( textColour );
    painter.drawText( titleSlide.rect(), Qt::AlignCenter | Qt::TextWordWrap, title );
    filename = path + slidePath + "/SPJT0001.JPG";

    KTempFile tmp2;
    titleSlide.save( tmp2.name(), "JPEG" );
    KIO::NetAccess::file_move( tmp2.name(), filename, -1, true /*overwrite*/);

    p = progressBar->progress();
    progressBar->setProgress( ++p );
    kapp->processEvents();

}

void KPrMSPresentation::createSlidesPictures( KProgress *progressBar )
{
    if ( slideInfos.isEmpty() )
        return;
    QString filename;
    int p;
    for ( unsigned int i = 0; i < slideInfos.count(); i++ ) {
        int pgNum = slideInfos[i].pageNumber;
        filename.sprintf("/SPJP%04i.JPG", i+3);

        KTempFile tmp;

        view->getCanvas()->exportPage( pgNum, 1023, 767,
                                       tmp.name(), "JPEG" );

        KIO::NetAccess::file_move( tmp.name(), ( path + slidePath + filename ), -1, true /*overwrite*/);

        p = progressBar->progress();
        progressBar->setProgress( ++p );
        kapp->processEvents();
    }
}

void KPrMSPresentation::createIndexFile( KProgress *progressBar )
{
    int p;
    KTempFile sppFile;

    QString filenameStore = (path + "/MSSONY/PJ/" + title + ".SPP");

    QDataStream sppStream( sppFile.file() );
    sppStream.setByteOrder(QDataStream::LittleEndian);
    p = progressBar->progress();
    progressBar->setProgress( ++p );
    kapp->processEvents();

    // We are doing little endian
    sppStream << (quint32)0x00505053; // SPP magic header
    sppStream << (quint32)0x00000000; // four null bytes
    sppStream << (quint32)0x30303130; // version(?) 0100
    sppStream << (quint32)0x00000000; // more nulls
    sppStream << (quint32)(slideInfos.count());

    // DCIM path 1, 68 bytes null padded
    char buff[68];
    strncpy( buff, QString("%1").arg(slidePath).ascii(), 67 );
    buff[67] = 0x00;
    sppStream.writeRawBytes( buff, 68 );
    sppStream << (quint32)0x00000001; // fixed value
    sppStream << (quint32)0x00000005; // fixed value
    sppStream << (quint32)0x00000000; // more nulls
    sppStream << (quint32)0x00000000; // more nulls
    sppStream << (quint32)0x00000000; // more nulls
    sppStream << (quint32)0x00000000; // more nulls
    sppStream << (quint32)0x00000000; // more nulls
    sppStream << (quint32)0x00000000; // more nulls
    p = progressBar->progress();
    progressBar->setProgress( ++p );
    kapp->processEvents();

    // title 1, 16 bytes null padded
    strncpy( buff, "SPJT0001.JPG", 15 );
    buff[15] = 0x00;
    sppStream.writeRawBytes( buff, 16 );

    // title 2, 16 bytes null padded
    strncpy( buff, "SPJT0002.JPG", 15 );
    buff[15] = 0x00;
    sppStream.writeRawBytes( buff, 16 );


    // type face (?), 44 bytes null padded
    strncpy( buff, "MS Sans Serif", 43 );
    buff[44] = 0x00;
    sppStream.writeRawBytes( buff, 44 );

    //not really sure what this is about
    sppStream << (quint32)0xffff0000;
    sppStream << (quint32)0xffff00ff;
    sppStream << (quint32)0xffff00ff;
    sppStream << (quint32)0x000000ff;
    sppStream << (quint32)0x00000002;
    for (int i = 0; i < (296/4); i++) {
        sppStream << (quint32)0x00000000;
    }
    p = progressBar->progress();
    progressBar->setProgress( ++p );
    kapp->processEvents();

    // Add in the slide filenames
    QString filename;
    for ( unsigned int i = 0; i < slideInfos.count(); i++ ) {
        filename.sprintf("SPJP%04i.JPG", i+3);
        strncpy( buff, filename.ascii(), 63 );
        buff[64] = 0x00;
        sppStream.writeRawBytes( buff, 64 );
        p = progressBar->progress();
        progressBar->setProgress( ++p );
        kapp->processEvents();
    }

    // OK, now we need to fill to 16384 bytes
    // the logic is 16384 bytes total, lead in is 512 bytes, and there
    // is 64 bytes for each real slide
    for(unsigned int i = 0;  i < (16384-512-64*(slideInfos.count()))/4; i++) {
        sppStream << (quint32)0x00000000;
    }

    p = progressBar->progress();
    progressBar->setProgress( ++p );
    kapp->processEvents();

    sppFile.close();
    KIO::NetAccess::file_move( sppFile.name(), filenameStore, -1, true /*overwrite*/);
}

void KPrMSPresentation::init()
{
    title = i18n("Slideshow");

    for ( unsigned int i = 0; i < doc->getPageNums(); i++ )
    {
        if ( doc->isSlideSelected( i ) )
        {
            SlideInfo info;
            info.pageNumber = i;
            slideInfos.append( info );
        }
    }
    if ( slideInfos.isEmpty() )
        kWarning() << "No slides selected!" << endl;
    backColour = Qt::black;
    textColour = Qt::white;

    path = KGlobalSettings::documentPath();
}

KPrMSPresentationSetup::KPrMSPresentationSetup( KPrDocument *_doc, KPrView *_view )
    : QDialog( 0, "", false ), msPres(  _doc, _view )
{
    doc = _doc;
    view = _view;


    QLabel *helptext = new QLabel( this );
    helptext->setAlignment( Qt::TextWordWrap | Qt::AlignTop| Qt::AlignLeft );
    helptext->setText( i18n( "Please enter the directory where the memory stick "
                             "presentation should be saved. Please also enter a "
                             "title for the slideshow presentation. " ) );

    QLabel *lable2 = new QLabel( i18n("Path:"), this );
    lable2->setAlignment( Qt::AlignVCenter | Qt::AlignRight );
    path=new KUrlRequester( this );
    path->setMode( KFile::Directory);
    path->lineEdit()->setText(msPres.getPath());
    lable2->setBuddy(path);
    Q3HBoxLayout *pathLayout = new Q3HBoxLayout;
    pathLayout->addWidget(lable2);
    pathLayout->addWidget(path);

    connect( path, SIGNAL( textChanged(const QString&) ),
             this, SLOT( slotChoosePath(const QString&) ) );
    connect( path, SIGNAL( urlSelected( const QString&) ),
             this, SLOT( slotChoosePath(const QString&) ) );


    QLabel *lable1 = new QLabel( i18n("Title:"), this, "lable1" );
    lable1->setAlignment( Qt::AlignVCenter | Qt::AlignRight );
    // TODO - check if there is already a presentation with this title, and
    // add a number after it until there isn't.
    title = new KLineEdit( msPres.getTitle(), this );
    lable1->setBuddy( title );
    Q3HBoxLayout *titleLayout = new Q3HBoxLayout;
    titleLayout->addWidget( lable1 );
    titleLayout->addWidget( title );

    Q3HBox *moreBox = new Q3HBox( this );
    QPushButton *showColourButton = new QPushButton( i18n("&Set Colors"), moreBox );
    showColourButton->setToggleButton( true );
    connect( showColourButton, SIGNAL( toggled(bool) ), this, SLOT( showColourGroup(bool) ) );

    Q3VBoxLayout *topLayout = new Q3VBoxLayout;
    topLayout->addWidget( helptext );
    topLayout->addSpacing( 10 );
    topLayout->addLayout( pathLayout );
    topLayout->addLayout( titleLayout );
    topLayout->addWidget( moreBox );

    colourGroup = new Q3GroupBox( 2, Qt::Vertical,
                                            i18n("Preliminary Slides"),
                                            this , "colourBox" );
    Q3WhatsThis::add( colourGroup,
                     i18n( "This section allows you to set the colors for "
                           "the preliminary slides; it does not affect the "
                           "presentation in any way, and it is normal to "
                           "leave these set to the default.") );
    Q3HBox *textColourLayout = new Q3HBox( colourGroup );
    QLabel *lable3 = new QLabel( i18n("Text color:"), textColourLayout );
    lable3->setAlignment( Qt::AlignVCenter | Qt::AlignRight );
    textColour = new KColorButton( msPres.getTextColour(), textColourLayout );
    lable3->setBuddy( textColour );

    Q3HBox *backgroundColourLayout = new Q3HBox( colourGroup );
    QLabel *lable4 = new QLabel( i18n("Background color:"), backgroundColourLayout );
    lable4->setAlignment( Qt::AlignVCenter | Qt::AlignRight );
    backColour = new KColorButton( msPres.getBackColour(), backgroundColourLayout );
    lable4->setBuddy( backColour );
    colourGroup->setHidden( true );

    Q3HBox *buttonLayout = new Q3HBox( this );
    KPushButton *helpButton = new KPushButton( KStdGuiItem::help(), buttonLayout );
    Q3WhatsThis::add( helpButton,
                     i18n( "Selecting this button will take you to the KPresenter "
                           "documentation that provides more information on how "
                           "to use the Memory Stick export function. ") );
    KPushButton *createButton = new KPushButton( KStdGuiItem::ok(), buttonLayout );
    Q3WhatsThis::add( createButton,
                     i18n( "Selecting this button will proceed to generating "
                           "the presentation in the special Sony format." ) );
    KPushButton *cancelButton = new KPushButton( KStdGuiItem::cancel(), buttonLayout );
    Q3WhatsThis::add( cancelButton,
                     i18n( "Selecting this button will cancel out of the "
                           "generation of the presentation, and return "
                           "to the normal KPresenter view. No files will "
                           "be affected." ) );

    mainLayout = new Q3VBoxLayout( this );
    mainLayout->setMargin(11);
    mainLayout->setSpacing(6);
    mainLayout->addLayout(topLayout);
    mainLayout->addSpacing( 10 );
    mainLayout->addWidget(colourGroup);
    mainLayout->addWidget(buttonLayout);
    mainLayout->setResizeMode( QLayout::Fixed );
    mainLayout->setGeometry( QRect( 0, 0, 300, 220 ) );

    connect( helpButton, SIGNAL( clicked() ), this, SLOT ( helpMe() ) );
    connect( cancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( createButton, SIGNAL( clicked() ), this, SLOT( finish() ) );
}

KPrMSPresentationSetup::~KPrMSPresentationSetup()
{
    view->enableMSPres();
}

void KPrMSPresentationSetup::createMSPresentation( KPrDocument *_doc, KPrView *_view )
{
    KPrMSPresentationSetup *dlg = new KPrMSPresentationSetup( _doc, _view );

    dlg->setCaption( i18n( "Create Memory Stick Slideshow" ) );
    dlg->exec();
    delete dlg;
}

void KPrMSPresentationSetup::showColourGroup(bool on)
{
    if (on) {
        colourGroup->setHidden( false );
        mainLayout->setGeometry( QRect(0, 0, 300, 220 ) );
    } else {
        colourGroup->setHidden( true );
        mainLayout->setGeometry( QRect(0, 0, 300, 320 ) );
    }
}

void KPrMSPresentationSetup::helpMe()
{
    KToolInvocation::invokeHelp("ms-export");
}

void KPrMSPresentationSetup::finish()
{
    msPres.setTitle( title->text() );

    msPres.setBackColour( backColour->color() );
    msPres.setTextColour( textColour->color() );
    msPres.setPath( path->lineEdit()->text() );

    // Things to sanity check:
    // 1. that the directory exists. If not, offer to create it.
    // 2. that the directory is writable.
    // 3. that the .spp file doesn't already exist. If it does, offer to overwrite it.
    // If we fail to create a directory, or the user tells us not to bother creating it,
    // or the user doesn't want to overwrite it, go back.
    QString pathname = path->lineEdit()->text();

    // path doesn't exist. ask user if it should be created.
    if ( !KIO::NetAccess::exists(pathname, false, this) ) {
        QString msg = i18n( "<qt>The directory <b>%1</b> does not exist.<br>"
                            "Do you want create it?</qt>" );
        if( KMessageBox::questionYesNo( this, msg.arg( pathname ),
                                        i18n( "Directory Not Found" ) )
            == KMessageBox::Yes)
            {
                // we are trying to create the directory
                QDir dir;
                bool ok = KIO::NetAccess::mkdir( pathname,this );
                if( !ok ) {
                    // then directory couldn't be created
                    KMessageBox::sorry( this,
                                        i18n( "Cannot create directory." ) );
                    path->setFocus();
                    return;
                }
            }
        else {
            // user preferred not to create directory
            path->setFocus();
            return;
        }
    }

    QString sppFile( pathname + "/MSSONY/PJ/" + title->text() + ".SPP" );
    if (KIO::NetAccess::exists(sppFile, false, this ) ) {
        if ( KMessageBox::warningYesNo( 0,
                                   i18n( "You are about to overwrite an existing index "
                                         "file : %1.\n "
                                         "Do you want to proceed?", sppFile ),
                                   i18n( "Overwrite Presentation" ) )
             == KMessageBox::No) {
            path->setFocus();
            return;
        }
    }

    close();

    KPrMSPresentationCreateDialog::createMSPresentation( doc, view, msPres );

}

void KPrMSPresentationSetup::slotChoosePath(const QString &text)
{
    msPres.setPath(text);
}




KPrMSPresentationCreateDialog::KPrMSPresentationCreateDialog( KPrDocument *_doc, KPrView *_view,
                                                            const KPrMSPresentation &_msPres )
    : QDialog( 0, "", false ), msPres( _msPres )
{
    doc = _doc;
    view = _view;

    setupGUI();
}

KPrMSPresentationCreateDialog::~KPrMSPresentationCreateDialog()
{
    view->enableMSPres();
}

void KPrMSPresentationCreateDialog::createMSPresentation( KPrDocument *_doc, KPrView *_view,
                                                         const KPrMSPresentation &_msPres )
{
    KPrMSPresentationCreateDialog *dlg = new KPrMSPresentationCreateDialog( _doc, _view, _msPres );

    dlg->setCaption( i18n( "Create Memory Stick Slideshow" ) );
    dlg->resize( 400, 250 );
    dlg->show();
    dlg->start();
}

void KPrMSPresentationCreateDialog::start()
{
    setCursor( Qt::waitCursor );
    initCreation();
    createSlidesPictures();
    createIndexFile();
    setCursor( Qt::arrowCursor );

    bDone->setEnabled( true );
}

void KPrMSPresentationCreateDialog::initCreation()
{
    QFont f = step1->font(), f2 = step1->font();
    f.setBold( true );
    step1->setFont( f );

    progressBar->setProgress( 0 );
    progressBar->setTotalSteps( msPres.initSteps() );

    msPres.initCreation( progressBar );

    step1->setFont( f2 );
    progressBar->setProgress( progressBar->totalSteps() );
}

void KPrMSPresentationCreateDialog::createSlidesPictures()
{
    QFont f = step2->font(), f2 = f;
    f.setBold( true );
    step2->setFont( f );

    progressBar->setProgress( 0 );
    if ( msPres.slidesSteps() > 0 )
    {
        progressBar->setTotalSteps( msPres.slidesSteps() );
        msPres.createSlidesPictures( progressBar );
    }

    step2->setFont( f2 );
    progressBar->setProgress( progressBar->totalSteps() );
}

void KPrMSPresentationCreateDialog::createIndexFile()
{
    QFont f = step3->font(), f2 = f;
    f.setBold( true );
    step3->setFont( f );

    progressBar->setProgress( 0 );
    if ( msPres.indexFileSteps() > 0 )
    {
        progressBar->setTotalSteps( msPres.indexFileSteps() );
        msPres.createIndexFile( progressBar );
    }

    step3->setFont( f2 );
    progressBar->setProgress( progressBar->totalSteps() );
}

void KPrMSPresentationCreateDialog::setupGUI()
{
    back = new Q3VBox( this );
    back->setMargin( KDialog::marginHint() );

    Q3Frame *line;

    step1 = new QLabel( i18n( "Create directory structure" ), back );
    step2 = new QLabel( i18n( "Create pictures of the slides" ), back );
    step3 = new QLabel( i18n( "Create index file" ), back );

    line = new Q3Frame( back );
    line->setFrameStyle( Q3Frame::HLine | Q3Frame::Sunken );
    line->setMaximumHeight( 20 );

    progressBar = new KProgress( back );

    line = new Q3Frame( back );
    line->setFrameStyle( Q3Frame::HLine | Q3Frame::Sunken );
    line->setMaximumHeight( 20 );

    KButtonBox *bb = new KButtonBox( back );
    bb->addStretch();
    bDone = bb->addButton( i18n( "Done" ) );

    bDone->setEnabled( false );

    connect( bDone, SIGNAL( clicked() ), this, SLOT( accept() ) );
}

void KPrMSPresentationCreateDialog::resizeEvent( QResizeEvent *e )
{
    QDialog::resizeEvent( e );
    back->resize( size() );
}

#include "KPrGradient.h"
#include "KPrMSPresentationSetup.moc"
