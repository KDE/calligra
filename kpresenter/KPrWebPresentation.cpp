// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
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

#include "KPrWebPresentation.h"

#include "KPrView.h"
#include "KPrDocument.h"
#include "KPrCanvas.h"
#include "KPrPage.h"

#include <kstandarddirs.h>
#include <unistd.h>
#include <sys/types.h>
#include <ktempfile.h>

#include <QFile>
#include <qtextstream.h>


#include <QLabel>
#include <QPushButton>
#include <qfileinfo.h>
#include <q3frame.h>
#include <QFont>
#include <qpixmap.h>
#include <qdatetime.h>
#include <QDir>
#include <q3header.h>
#include <qmatrix.h>
#include <qtextcodec.h>
#include <QRegExp>
#include <qimage.h>
#include <QLayout>

#include <QCheckBox>
//Added by qt3to4:
#include <Q3ValueList>
#include <Q3GridLayout>
#include <Q3PtrList>
#include <QResizeEvent>
#include <QCloseEvent>

#include <kdebug.h>
#include <klocale.h>
#include <kcolorbutton.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kbuttonbox.h>
#include <ksimpleconfig.h>
#include <kapplication.h>
#include <kprogressbar.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <kcharsets.h>
#include <kurlrequester.h>
#include <klineedit.h>
#include <k3listview.h>
#include <knuminput.h>
#include <kcombobox.h>
#include <kurl.h>
#include <kio/netaccess.h>
#include <kdialog.h>
#include <kvbox.h>

#include "KoDocumentInfo.h"


// Comes from koffice/filters/libexport/KWEFUtils.cc
static QString EscapeSgmlText(const QTextCodec* codec, const QString& strIn,
                              const bool quot = false , const bool apos = false )
{
    QString strReturn;
    QChar ch;

    for (uint i=0; i<strIn.length(); i++)
    {
        ch=strIn[i];
        switch (ch.unicode())
        {
        case 38: // &
        {
            strReturn+="&amp;";
            break;
        }
        case 60: // <
        {
            strReturn+="&lt;";
            break;
        }
        case 62: // >
        {
            strReturn+="&gt;";
            break;
        }
        case 34: // "
        {
            if (quot)
                strReturn+="&quot;";
            else
                strReturn+=ch;
            break;
        }
        case 39: // '
        {
            // NOTE:  HTML does not define &apos; by default (only XML/XHTML does)
            if (apos)
                strReturn+="&apos;";
            else
                strReturn+=ch;
            break;
        }
        default:
        {
            // verify that the character ch can be expressed in the
            //   encoding in which we will write the HTML file.
            if (codec)
            {
                if (!codec->canEncode(ch))
                {
                    strReturn+=QString("&#%1;").arg(ch.unicode());
                    break;
                }
            }
            strReturn+=ch;
            break;
        }
        }
    }

    return strReturn;
}

// Escape only if the encoding do not support the character
// Special SGML characters like < > & are supposed to be already escaped.
static QString EscapeEncodingOnly(const QTextCodec* codec, const QString& strIn)
{
    QString strReturn;
    QChar ch;

    for (uint i=0; i<strIn.length(); i++)
    {
        ch=strIn[i];
        if (codec)
        {
            if (!codec->canEncode(ch))
            {
                strReturn+=QString("&#%1;").arg(ch.unicode());
                continue;
            }
        }
        strReturn+=ch;
    }
    return strReturn;
}

KPrWebPresentation::KPrWebPresentation( KPrDocument *_doc, KPrView *_view )
    : config( QString::null ), xml( false )
{
    doc = _doc;
    view = _view;
    init();
}

KPrWebPresentation::KPrWebPresentation( const QString &_config, KPrDocument *_doc, KPrView *_view )
    : config( _config ), xml( false ), m_bWriteHeader( true ), m_bWriteFooter( true ), m_bLoopSlides( false )
{
    doc = _doc;
    view = _view;
    init();
    loadConfig();
}

KPrWebPresentation::KPrWebPresentation( const KPrWebPresentation &webPres )
    : config( webPres.config ), author( webPres.author ), title( webPres.title ), email( webPres.email ),
      slideInfos( webPres.slideInfos ), backColor( webPres.backColor ), titleColor( webPres.titleColor ),
      textColor( webPres.textColor ), path( webPres.path ), xml( webPres.xml),
      m_bWriteHeader( webPres.m_bWriteHeader ),
      m_bWriteFooter( webPres.m_bWriteFooter ), m_bLoopSlides( webPres.m_bLoopSlides ),
      timeBetweenSlides ( webPres.timeBetweenSlides ), zoom( webPres.zoom ), m_encoding( webPres.m_encoding )
{
    doc = webPres.doc;
    view = webPres.view;
}

void KPrWebPresentation::loadConfig()
{
    if ( config.isEmpty() )
        return;

    KSimpleConfig cfg( config );
    cfg.setGroup( "General" );

    author = cfg.readEntry( "Author", author );
    title = cfg.readEntry( "Title", title );
    email = cfg.readEntry( "EMail", email );
    unsigned int num = cfg.readNumEntry( "Slides", slideInfos.count() );
    //kDebug(33001) << "KPrWebPresentation::loadConfig num=" << num << endl;

    if ( num <= slideInfos.count() ) {
        for ( unsigned int i = 0; i < num; i++ )
        {
            QString key = QString::fromLatin1( "SlideTitle%1" ).arg( i );
            if ( cfg.hasKey( key ) )
            {
                // We'll assume that the selected pages haven't changed... Hmm.
                slideInfos[ i ].slideTitle = cfg.readEntry( key, QString() );
                kDebug(33001) << "KPrWebPresentation::loadConfig key=" << key << " data=" << slideInfos[i].slideTitle << endl;
            } else kDebug(33001) << " key not found " << key << endl;
        }
    }

    backColor = cfg.readColorEntry( "BackColor", &backColor );
    titleColor = cfg.readColorEntry( "TitleColor", &titleColor );
    textColor = cfg.readColorEntry( "TextColor", &textColor );
    path = cfg.readPathEntry( "Path", path );
    xml = cfg.readBoolEntry( "XML", xml );
    m_bWriteHeader = cfg.readBoolEntry( "WriteHeader", m_bWriteHeader );
    m_bWriteFooter = cfg.readBoolEntry( "WriteFooter", m_bWriteFooter );
    m_bLoopSlides = cfg.readBoolEntry( "LoopSlides", m_bLoopSlides );
    zoom = cfg.readNumEntry( "Zoom", zoom );
    timeBetweenSlides = cfg.readNumEntry("TimeBetweenSlides", timeBetweenSlides );
    m_encoding = cfg.readEntry( "Encoding", m_encoding );
}

void KPrWebPresentation::saveConfig()
{
    KSimpleConfig cfg( config );
    cfg.setGroup( "General" );

    cfg.writeEntry( "Author", author );
    cfg.writeEntry( "Title", title );
    cfg.writeEntry( "EMail", email );
    cfg.writeEntry( "Slides", slideInfos.count() );

    for ( unsigned int i = 0; i < slideInfos.count(); i++ )
        cfg.writeEntry( QString::fromLatin1( "SlideTitle%1" ).arg( i ), slideInfos[ i ].slideTitle );

    cfg.writeEntry( "BackColor", backColor );
    cfg.writeEntry( "TitleColor", titleColor );
    cfg.writeEntry( "TextColor", textColor );
    cfg.writePathEntry( "Path", path );
    cfg.writeEntry( "XML", xml );
    cfg.writeEntry( "WriteHeader", m_bWriteHeader );
    cfg.writeEntry( "WriteFooter", m_bWriteFooter );
    cfg.writeEntry( "LoopSlides", m_bLoopSlides );
    cfg.writeEntry( "Zoom", zoom );
    cfg.writeEntry( "TimeBetweenSlides", timeBetweenSlides );
    cfg.writeEntry( "Encoding", m_encoding );
}

void KPrWebPresentation::initCreation( KProgressBar *progressBar )
{
    QString cmd;
    int p;
    KUrl str(  path + "/html"  );
    KIO::NetAccess::mkdir( str,( QWidget* )0L  );

    p = progressBar->value();
    progressBar->setValue( ++p );
    kapp->processEvents();

    str = path + "/pics";
    KIO::NetAccess::mkdir( str,( QWidget* )0L );

    p = progressBar->value();
    progressBar->setValue( ++p );
    kapp->processEvents();

    const char *pics[] = { "home", "first", "next", "prev", "last", 0 };

    KUrl srcurl, desturl;

    for ( uint index = 0; pics[ index ]; index ++ )
    {
        QString filename = pics[ index ];
        filename += ".png";
        srcurl.setPath( KStandardDirs::locate( "slideshow", filename, KPrFactory::global() ) );
        desturl = path;
        desturl.addPath( "/pics/" + filename );
        KIO::NetAccess::file_copy( srcurl, desturl, -1, true /*overwrite*/);
        p = progressBar->value();
        progressBar->setValue( ++p );
        kapp->processEvents();
    }
}

void KPrWebPresentation::createSlidesPictures( KProgressBar *progressBar )
{
    if ( slideInfos.isEmpty() )
        return;
    QPixmap pix( 10, 10 );
    QString filename;
    int p;
    for ( unsigned int i = 0; i < slideInfos.count(); i++ ) {
        int pgNum = slideInfos[i].pageNumber;
        view->getCanvas()->drawPageInPix( pix, pgNum, zoom, true /*force real variable value*/ );
        filename = QString( "%1/pics/slide_%2.png" ).arg( path ).arg( i + 1 );

        KTempFile tmp;
        pix.save( tmp.name(), "PNG" );

        KIO::NetAccess::file_move( tmp.name(), filename, -1, true /*overwrite*/);

        p = progressBar->value();
        progressBar->setValue( ++p );
        kapp->processEvents();
    }
}

QString KPrWebPresentation::escapeHtmlText( QTextCodec *codec, const QString& strText ) const
{
    // Escape quotes (needed in attributes)
    // Do not escape apostrophs (only allowed in XHTML!)
    return EscapeSgmlText( codec, strText, true, false );
}

void KPrWebPresentation::writeStartOfHeader(QTextStream& streamOut, QTextCodec *codec, const QString& subtitle, const QString& next)
{
    QString mimeName ( codec->mimeName() );
    if ( isXML() )
    {   //Write out the XML declaration
        streamOut << "<?xml version=\"1.0\" encoding=\""
                  << mimeName << "\"?>\n";
    }
    // write <!DOCTYPE
    streamOut << "<!DOCTYPE ";
    if ( isXML() )
    {
        streamOut << "html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\"";
        streamOut << " \"DTD/xhtml1-transitional.dtd\">\n";
    }
    else
    {
        streamOut << "HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\"";
        streamOut << " \"http://www.w3.org/TR/html4/loose.dtd\">\n";
    }
    streamOut << "<html";
    if ( isXML() )
    {
        // XHTML has an extra attribute defining its namespace (in the <html> opening tag)
        streamOut << " xmlns=\"http://www.w3.org/1999/xhtml\"";
    }
    streamOut << ">\n" << "<head>\n";

    // Declare what charset we are using
    streamOut << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=";
    streamOut << mimeName << '"' << ( isXML() ?" /":"") << ">\n" ;

    // Tell who we are (with the CVS revision number) in case we have a bug in our output!
    QString strVersion("$Revision$");
    // Eliminate the dollar signs
    //  (We don't want that the version number changes if the HTML file is itself put in a CVS storage.)
    streamOut << "<meta name=\"Generator\" content=\"KPresenter's Web Presentation "
              << strVersion.mid(10).replace("$","")
              << "\""<< ( isXML() ?" /":"") // X(HT)ML closes empty elements, HTML not!
              << ">\n";

    // Load the next slide after time elapsed
    if ( (timeBetweenSlides > 0) && ( ! next.isNull() ) )
    {
        streamOut << "<meta http-equiv=\"refresh\" content=\""
                  << timeBetweenSlides
                  << ";url=" << next
                  << "\">\n";
    }

    streamOut << "<title>"<< escapeHtmlText( codec, title ) << " - " << escapeHtmlText( codec, subtitle ) << "</title>\n";

    // ### TODO: transform documentinfo.xml into many <META> elements (at least the author!)
}

void KPrWebPresentation::createSlidesHTML( KProgressBar *progressBar )
{
    QTextCodec *codec = KGlobal::charsets()->codecForName( m_encoding );

    const QString brtag ( "<br" + QString(isXML()?" /":"") + ">" );

    for ( unsigned int i = 0; i < slideInfos.count(); i++ ) {

        unsigned int pgNum = i + 1; // pgquiles # elpauer . org - I think this is a bug, seems to be an overflow if we have max_unsigned_int slides
        KTempFile tmp;
        QString dest= QString( "%1/html/slide_%2.html" ).arg( path ).arg( pgNum );
        QString next= QString( "slide_%2.html" ).arg( pgNum<slideInfos.count() ? pgNum+1 : (m_bLoopSlides ? 1 : pgNum ) ); // Ugly, but it works

        QFile file( tmp.name() );
        file.open( QIODevice::WriteOnly );
        QTextStream streamOut( &file );
        streamOut.setCodec( codec );

        writeStartOfHeader( streamOut, codec, slideInfos[ i ].slideTitle, next );

        // ### TODO: transform documentinfo.xml into many <META> elements (at least the author!)

        if ( i > 0 ) {
            streamOut <<  "<link rel=\"first\" href=\"slide_1.html\"" << ( isXML() ?" /":"") << ">\n";
            streamOut <<  "<link rel=\"prev\" href=\"slide_" << pgNum - 1 << ".html\"" << ( isXML() ?" /":"") << ">\n";
        }
        if ( i < slideInfos.count() - 1 ) {
            streamOut <<  "<link rel=\"next\" href=\"slide_" << pgNum + 1 << ".html\"" << ( isXML() ?" /":"") << ">\n";
            streamOut <<  "<link rel=\"last\" href=\"slide_" << slideInfos.count() << ".html\"" << ( isXML() ?" /":"") << ">\n";
        }
        streamOut <<  "<link rel=\"contents\" href=\"../index.html\"" << ( isXML() ?" /":"") << ">\n";

        streamOut << "</head>\n";
        streamOut << "<body bgcolor=\"" << backColor.name() << "\" text=\"" << textColor.name() << "\">\n";

        if (m_bWriteHeader) {
            streamOut << "  <center>\n";

            if ( i > 0 )
                streamOut << "    <a href=\"slide_1.html\">";
                streamOut << "<img src=\"../pics/first.png\" border=\"0\" alt=\"" << i18n( "First" )
                              << "\" title=\"" << i18n( "First" ) << "\"" << ( isXML() ?" /":"") << ">";
            if ( i > 0 )
                streamOut << "</a>";

            streamOut << "\n";

            if ( i > 0 )
                streamOut << "    <a href=\"slide_" << pgNum - 1 << ".html\">";
                streamOut << "<img src=\"../pics/prev.png\" border=\"0\" alt=\"" << i18n( "Previous" )
                              << "\" title=\"" << i18n( "Previous" ) << "\"" << ( isXML() ?" /":"") << ">";
            if ( i > 0 )
                streamOut << "</a>";

            streamOut << "\n";

            if ( (m_bLoopSlides) || (i < slideInfos.count() - 1 ) )
                streamOut << "    <a href=\"" << next << "\">";
                streamOut << "<img src=\"../pics/next.png\" border=\"0\" alt=\"" << i18n( "Next" )
                          << "\" title=\"" << i18n( "Next" ) << "\"" << ( isXML() ?" /":"") << ">";
            if ( (m_bLoopSlides) || (i < slideInfos.count() - 1 ) )
                streamOut << "</a>";

            streamOut << "\n";

            if ( i < slideInfos.count() - 1 )
                streamOut << "    <a href=\"slide_" << slideInfos.count() << ".html\">";
                streamOut << "<img src=\"../pics/last.png\" border=\"0\" alt=\"" << i18n( "Last" )
                      << "\" title=\"" << i18n( "Last" ) << "\"" << ( isXML() ?" /":"") << ">";
            if ( i < slideInfos.count() - 1 )
                streamOut << "</a>";

            streamOut << "\n" << "    &nbsp; &nbsp; &nbsp; &nbsp;\n";

            streamOut << "    <a href=\"../index.html\">";
            streamOut << "<img src=\"../pics/home.png\" border=\"0\" alt=\"" << i18n( "Home" )
                      << "\" title=\"" << i18n( "Home" ) << "\"" << ( isXML() ?" /":"") << ">";
            streamOut << "</a>\n";

            streamOut << " </center>" << brtag << "<hr noshade=\"noshade\"" << ( isXML() ?" /":"") << ">\n"; // ### TODO: is noshade W3C?

            streamOut << "  <center>\n    <font color=\"" << escapeHtmlText( codec, titleColor.name() ) << "\">\n";
            streamOut << "    <b>" << escapeHtmlText( codec, title ) << "</b> - <i>" << escapeHtmlText( codec, slideInfos[ i ].slideTitle ) << "</i>\n";

            streamOut << "    </font>\n  </center>\n";

            streamOut << "<hr noshade=\"noshade\"" << ( isXML() ?" /":"") << ">" << brtag << "\n";
	}

        streamOut << "  <center>\n    ";

	if ( (m_bLoopSlides) || (i < slideInfos.count() - 1) )
            streamOut << "<a href=\"" << next << "\">";

	    streamOut << "<img src=\"../pics/slide_" << pgNum << ".png\" border=\"0\" alt=\""
                      << i18n( "Slide %1", pgNum ) << "\"" << ( isXML() ?" /":"") << ">";

	    if ( i < slideInfos.count() - 1 )
                streamOut << "</a>";

            streamOut << "\n";

            streamOut << "    </center>\n";

	if (m_bWriteFooter) {
	    	streamOut << brtag << "<hr noshade=\"noshade\"" << ( isXML() ?" /":"") << ">\n";

            Q3PtrList<KPrPage> _tmpList( doc->getPageList() );
            QString note ( escapeHtmlText( codec, _tmpList.at(i)->noteText() ) );
            if ( !note.isEmpty() ) {
                streamOut << "  <b>" << escapeHtmlText( codec, i18n( "Note" ) ) << "</b>\n";
                streamOut << " <blockquote>\n";

                streamOut << note.replace( "\n", brtag );

                streamOut << "  </blockquote><hr noshade=\"noshade\"" << ( isXML() ?" /":"") << ">\n";
            }

            streamOut << "  <center>\n";

            QString htmlAuthor;
            if (email.isEmpty())
                htmlAuthor=escapeHtmlText( codec, author );
            else
                htmlAuthor=QString("<a href=\"mailto:%1\">%2</a>").arg( escapeHtmlText( codec, email )).arg( escapeHtmlText( codec, author ));
            streamOut << EscapeEncodingOnly ( codec, i18n( "Created on %1 by <i>%2</i> with <a href=\"http://www.koffice.org/kpresenter\">KPresenter</a>" )
                                          .arg( KGlobal::locale()->formatDate ( QDate::currentDate() ) ).arg( htmlAuthor ) );

            streamOut << "    </center><hr noshade=\"noshade\"" << ( isXML() ?" /":"") << ">\n";
        }

        streamOut << "</body>\n</html>\n";

        file.close();

        KIO::NetAccess::file_move( tmp.name(), dest, -1, true /*overwrite*/);

        int p = progressBar->value();
        progressBar->setValue( ++p );
        kapp->processEvents();
    }
}

void KPrWebPresentation::createMainPage( KProgressBar *progressBar )
{
    QTextCodec *codec = KGlobal::charsets()->codecForName( m_encoding );
    KTempFile tmp;
    QString dest = QString( "%1/index.html" ).arg( path );
    QFile file( tmp.name() );
    file.open( QIODevice::WriteOnly );
    QTextStream streamOut( &file );
    streamOut.setCodec( codec );

    writeStartOfHeader( streamOut, codec, i18n("Table of Contents"), QString() );
    streamOut << "</head>\n";

    streamOut << "<body bgcolor=\"" << backColor.name() << "\" text=\"" << textColor.name() << "\">\n";

    streamOut << "<h1 align=\"center\"><font color=\"" << titleColor.name()
              << "\">" << title << "</font></h1>";

    streamOut << "<p align=\"center\"><a href=\"html/slide_1.html\">";
    streamOut << i18n("Click here to start the Slideshow");
    streamOut << "</a></p>\n";

    streamOut << "<p><b>" << i18n("Table of Contents") << "</b></p>\n";

    // create list of slides (with proper link)
    streamOut << "<ol>\n";
    for ( unsigned int i = 0; i < slideInfos.count(); i++ )
        streamOut << "  <li><a href=\"html/slide_" << i+1 << ".html\">" << slideInfos[ i ].slideTitle << "</a></li>\n";
    streamOut << "</ol>\n";

    // footer: author name, e-mail
    QString htmlAuthor = email.isEmpty() ? escapeHtmlText( codec, author ) :
                         QString("<a href=\"mailto:%1\">%2</a>").arg( escapeHtmlText( codec, email )).arg( escapeHtmlText( codec, author ));
    streamOut << EscapeEncodingOnly ( codec, i18n( "Created on %1 by <i>%2</i> with <a href=\"http://www.koffice.org/kpresenter\">KPresenter</a>"
                                      , KGlobal::locale()->formatDate ( QDate::currentDate() ), htmlAuthor ) );

    streamOut << "</body>\n</html>\n";
    file.close();

    KIO::NetAccess::file_move( tmp.name(), dest, -1, true /*overwrite*/);


    progressBar->setValue( progressBar->maximum() );
    kapp->processEvents();
}

void KPrWebPresentation::init()
{

    KoDocumentInfo * info = doc->documentInfo();
    if ( !info )
        kWarning() << "Author information not found in document Info !" << endl;
    else
    {
        author = info->authorInfo( "creator" );
        email = info->authorInfo( "email" );
    }

    title = i18n("Slideshow");
    kDebug(33001) << "KPrWebPresentation::init : " << doc->getPageNums() << " pages." << endl;
    for ( unsigned int i = 0; i < doc->getPageNums(); i++ )
    {
        if ( doc->isSlideSelected( i ) )
        {
            SlideInfo info;
            info.pageNumber = i;
            info.slideTitle = doc->pageList().at(i)->pageTitle();
            slideInfos.append( info );
        }
    }
    if ( slideInfos.isEmpty() )
        kWarning() << "No slides selected!" << endl;
    backColor = Qt::white;
    textColor = Qt::black;
    titleColor = Qt::red;

    path = KGlobalSettings::documentPath() + "www";

    zoom = 100;

    timeBetweenSlides = 0;

    m_encoding = QTextCodec::codecForLocale()->name();
}

KPrWebPresentationWizard::KPrWebPresentationWizard( const QString &_config, KPrDocument *_doc,
                                                  KPrView *_view )
    : K3Wizard( 0, "", false ), config( _config ), webPres( config, _doc, _view )
{
    doc = _doc;
    view = _view;

    setupPage1();
    setupPage2();
    setupPage3();
    setupPage4();
    setupPage5();

    connect( nextButton(), SIGNAL( clicked() ), this, SLOT( pageChanged() ) );
    connect( backButton(), SIGNAL( clicked() ), this, SLOT( pageChanged() ) );
    connect( finishButton(), SIGNAL( clicked() ), this, SLOT( finish() ) );
}

KPrWebPresentationWizard::~KPrWebPresentationWizard()
{
    view->enableWebPres();
}

void KPrWebPresentationWizard::createWebPresentation( const QString &_config, KPrDocument *_doc,
                                                     KPrView *_view )
{
    KPrWebPresentationWizard *dlg = new KPrWebPresentationWizard( _config, _doc, _view );

    dlg->setCaption( i18n( "Create HTML Slideshow Wizard" ) );
    dlg->show();
}

void KPrWebPresentationWizard::setupPage1()
{
    page1 = new KHBox( this );
    page1->setWhatsThis( i18n("This page allows you to specify some of the key"
                                 " values for how your presentation will be shown"
                                 " in HTML. Select individual items for more help"
                                 " on what they do.") );
    page1->setSpacing( KDialog::spacingHint() );
    page1->setMargin( KDialog::marginHint() );

    QLabel* sidebar = new QLabel( page1 );
    sidebar->setMinimumSize( 106, 318 );
    sidebar->setMaximumSize( 106, 318 );
    sidebar->setFrameShape( Q3Frame::Panel );
    sidebar->setFrameShadow( Q3Frame::Sunken );
    sidebar->setPixmap(KStandardDirs::locate("data", "kpresenter/pics/webslideshow-sidebar.png"));

    QWidget* canvas = new QWidget( page1 );
    Q3GridLayout *layout = new Q3GridLayout( canvas, 7, 2,
                                           KDialog::marginHint(), KDialog::spacingHint() );

    QLabel *helptext = new QLabel( canvas );
    helptext->setAlignment( Qt::TextWordWrap | Qt::AlignTop| Qt::AlignLeft );
    helptext->setText( i18n( "Enter your name, email address and "
                             "the title of the web presentation. "
                             "Also enter the output directory where the "
                             "web presentation should be saved. " ) );
    layout->addMultiCellWidget( helptext, 0, 0, 0, 1 );

    layout->addMultiCell( new QSpacerItem( 1, 50 ), 1, 1, 0, 1 );

    QLabel *label1 = new QLabel( i18n("Author:"), canvas );
    label1->setAlignment( Qt::AlignVCenter | Qt::AlignRight );
    label1->setWhatsThis( i18n("This is where you enter the name of the person or "
                                  "organization that should be named as the author of "
                                  "the presentation.") );
    layout->addWidget( label1, 2, 0 );

    QLabel *label2 = new QLabel( i18n("Title:"), canvas );
    label2->setAlignment( Qt::AlignVCenter | Qt::AlignRight );
    label2->setWhatsThis( i18n("This is where you enter the title of the overall "
                                  "presentation." ) );
    layout->addWidget( label2, 3, 0 );

    QLabel *label3 = new QLabel( i18n("Email address:"), canvas );
    label3->setAlignment( Qt::AlignVCenter | Qt::AlignRight );
    label3->setWhatsThis( i18n("This is where you enter the email address of the "
                                  "person or organization that is responsible for "
                                  "the presentation.") );
    layout->addWidget( label3, 4, 0 );

    QLabel *label4 = new QLabel( i18n("Path:"), canvas );
    label4->setAlignment( Qt::AlignVCenter | Qt::AlignRight );
    label4->setWhatsThis( i18n("The value entered for the path is the directory "
                                  "where the presentation will be saved. If it does "
                                  "not exist, you'll be asked if you want to create "
                                  "the directory or abort the creation.") );
    layout->addWidget( label4, 5, 0 );

    author = new KLineEdit( webPres.getAuthor(), canvas );
    author->setWhatsThis( i18n("This is where you enter the name of the person or "
                                  "organization that should be named as the author of "
                                  "the presentation.") );
    layout->addWidget( author, 2, 1 );

    title = new KLineEdit( webPres.getTitle(), canvas );
    title->setWhatsThis( i18n("This is where you enter the title of the overall "
                                 "presentation." ) );
    layout->addWidget( title, 3, 1 );

    email = new KLineEdit( webPres.getEmail(), canvas );
    email->setWhatsThis( i18n("This is where you enter the email address of the "
                                 "person or organization that is responsible for "
                                 "the presentation.") );
    layout->addWidget( email, 4, 1 );

    path=new KUrlRequester( canvas );
    path->setMode( KFile::Directory);
    path->lineEdit()->setText(webPres.getPath());
    path->setWhatsThis( i18n("The value entered for the path is the directory "
                                "where the presentation will be saved. If it does "
                                "not exist, you'll be asked if you want to create "
                                "the directory or abort the creation.") );
    layout->addWidget( path, 5, 1 );

    QSpacerItem* spacer = new QSpacerItem( 1, 10,
                                           QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout->addMultiCell( spacer, 6, 6, 0, 1 );

    connect(path, SIGNAL(textChanged(const QString&)),
            this,SLOT(slotChoosePath(const QString&)));
    connect(path, SIGNAL(urlSelected( const QString& )),
            this,SLOT(slotChoosePath(const QString&)));

    addPage( page1, i18n( "Step 1: General Information" ) );

    setHelpEnabled(page1, false);  //doesn't do anything currently
}

void KPrWebPresentationWizard::setupPage2()
{
    page2 = new KHBox( this );
    page2->setWhatsThis( i18n("This page allows you to specify how the HTML "
                                 "for your presentation will be displayed. Select "
                                 "individual items for more help on what they do.") );
    page2->setSpacing( KDialog::spacingHint() );
    page2->setMargin( KDialog::marginHint() );

    QLabel* sidebar = new QLabel( page2 );
    sidebar->setMinimumSize( 106, 318 );
    sidebar->setMaximumSize( 106, 318 );
    sidebar->setFrameShape( Q3Frame::Panel );
    sidebar->setFrameShadow( Q3Frame::Sunken );
    sidebar->setPixmap(KStandardDirs::locate("data", "kpresenter/pics/webslideshow-sidebar.png"));

    QWidget* canvas = new QWidget( page2 );
    Q3GridLayout *layout = new Q3GridLayout( canvas, 6, 2,
                                           KDialog::marginHint(), KDialog::spacingHint() );

    QLabel *helptext = new QLabel( canvas );
    helptext->setAlignment( Qt::TextWordWrap | Qt::AlignVCenter| Qt::AlignLeft );
    QString help = i18n("Here you can configure the style of the web pages.");
    help += i18n( "You can also specify the zoom for the slides." );
    helptext->setText(help);

    layout->addMultiCellWidget( helptext, 0, 0, 0, 1 );

    layout->addMultiCell( new QSpacerItem( 1, 50 ), 1, 1, 0, 1 );

    QLabel *label1 = new QLabel( i18n("Zoom:"), canvas );
    label1->setAlignment( Qt::AlignVCenter | Qt::AlignRight );
    label1->setWhatsThis( i18n( "This selection allows you to specify "
                                   "the size of the slide image." ) );
    layout->addWidget( label1, 2, 0 );

    QLabel *label2 = new QLabel( i18n( "Encoding:" ), canvas );
    label2->setAlignment( Qt::AlignVCenter | Qt::AlignRight );

    layout->addWidget( label2, 3, 0 );

    QLabel *label3 = new QLabel( i18n( "Document type:" ), canvas );
    label3->setAlignment( Qt::AlignVCenter | Qt::AlignRight );
    layout->addWidget( label3, 4, 0 );

    zoom = new KIntNumInput( webPres.getZoom(), canvas );
    zoom->setWhatsThis( i18n( "This selection allows you to specify "
                                 "the size of the slide image." ) );
    layout->addWidget( zoom, 2, 1 );
    zoom->setSuffix( " %" );
    zoom->setRange( 25, 1000, 5 );

    encoding = new KComboBox( false, canvas );
    layout->addWidget( encoding, 3, 1 );

    // Fill encoding combo
    // Stolen from kdelibs/kate/part/katedialogs.cpp
    QStringList encodings(KGlobal::charsets()->descriptiveEncodingNames());
    int idx = 0;
    for (uint i = 0; i < encodings.count(); i++)
    {
      bool found = false;
      QTextCodec *codecForEnc = KGlobal::charsets()->codecForName(KGlobal::charsets()->encodingForName(encodings[i]), found);
      if (found)
      {
        encoding->addItem(encodings[i]);

        if ( codecForEnc->name() == webPres.getEncoding() )
          encoding->setCurrentIndex(idx);
        idx++;
      }
    }

    doctype = new KComboBox( false, canvas );
    layout->addWidget( doctype, 4, 1 );
    doctype->addItem( "HTML 4.01" );
    doctype->addItem( "XHTML 1.0" );

    doctype->setCurrentIndex( webPres.isXML() ? 1 : 0 );

    QSpacerItem* spacer = new QSpacerItem( 1, 10,
                                           QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout->addMultiCell( spacer, 5, 5, 0, 1 );

    addPage( page2, i18n( "Step 2: Configure HTML" ) );

    setHelpEnabled(page2, false);  //doesn't do anything currently
}

void KPrWebPresentationWizard::setupPage3()
{
    page3 = new KHBox( this );
    page3->setWhatsThis( i18n("This page allows you to specify the colors for "
                                 "your presentation display. Select individual "
                                 "items for more help on what they do.") );

    page3->setSpacing( KDialog::spacingHint() );
    page3->setMargin( KDialog::marginHint() );

    QLabel* sidebar = new QLabel( page3 );
    sidebar->setMinimumSize( 106, 318 );
    sidebar->setMaximumSize( 106, 318 );
    sidebar->setFrameShape( Q3Frame::Panel );
    sidebar->setFrameShadow( Q3Frame::Sunken );
    sidebar->setPixmap(KStandardDirs::locate("data", "kpresenter/pics/webslideshow-sidebar.png"));

    QWidget* canvas = new QWidget( page3 );
    Q3GridLayout *layout = new Q3GridLayout( canvas, 6, 2,
                                           KDialog::marginHint(), KDialog::spacingHint() );

    QLabel *helptext = new QLabel( canvas );
    helptext->setAlignment( Qt::TextWordWrap | Qt::AlignVCenter| Qt::AlignLeft );
    helptext->setText( i18n( "Now you can customize the colors of the web pages." ) );
    layout->addMultiCellWidget( helptext, 0, 0, 0, 1 );

    layout->addMultiCell( new QSpacerItem( 1, 50 ), 1, 1, 0, 1 );

    QLabel *label1 = new QLabel( i18n("Text color:"), canvas );
    label1->setAlignment( Qt::AlignVCenter | Qt::AlignRight );
    layout->addWidget( label1, 2, 0 );

    QLabel *label2 = new QLabel( i18n("Title color:"), canvas );
    label2->setAlignment( Qt::AlignVCenter | Qt::AlignRight );
    layout->addWidget( label2, 3, 0 );

    QLabel *label3 = new QLabel( i18n("Background color:"), canvas );
    label3->setAlignment( Qt::AlignVCenter | Qt::AlignRight );
    layout->addWidget( label3, 4, 0 );

    textColor = new KColorButton( webPres.getTextColor(), canvas );
    layout->addWidget( textColor, 2, 1 );

    titleColor = new KColorButton( webPres.getTitleColor(), canvas );
    layout->addWidget( titleColor, 3, 1 );

    backColor = new KColorButton( webPres.getBackColor(), canvas );
    layout->addWidget( backColor, 4, 1 );

    QSpacerItem* spacer = new QSpacerItem( 1, 10,
                                           QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout->addMultiCell( spacer, 5, 5, 0, 1 );

    addPage( page3, i18n( "Step 3: Customize Colors" ) );

    setHelpEnabled(page3, false);  //doesn't do anything currently
}

void KPrWebPresentationWizard::setupPage4()
{
    page4 = new KHBox( this );
    page4->setWhatsThis( i18n("This page allows you to modify the titles of "
                                 "each slide, if required. You normally do not need "
                                 "to do this, but it is available if required.") );
    page4->setSpacing( KDialog::spacingHint() );
    page4->setMargin( KDialog::marginHint() );

    QLabel* sidebar = new QLabel( page4 );
    sidebar->setMinimumSize( 106, 318 );
    sidebar->setMaximumSize( 106, 318 );
    sidebar->setFrameShape( Q3Frame::Panel );
    sidebar->setFrameShadow( Q3Frame::Sunken );
    sidebar->setPixmap(KStandardDirs::locate("data", "kpresenter/pics/webslideshow-sidebar.png"));

    QWidget* canvas = new QWidget( page4 );
    Q3GridLayout *layout = new Q3GridLayout( canvas, 3, 2,
                                           KDialog::marginHint(), KDialog::spacingHint() );

    QLabel *helptext = new QLabel( canvas );
    helptext->setAlignment( Qt::TextWordWrap | Qt::AlignVCenter| Qt::AlignLeft );
    helptext->setText( i18n( "Here you can specify titles for "
                             "each slide. Click on a slide in "
                             "the list and then enter the title "
                             "in the textbox below. If you "
                             "click on a title, KPresenter "
                             "mainview will display the slide.") );

    layout->addMultiCellWidget( helptext, 0, 0, 0, 1 );

    QLabel *label = new QLabel( i18n( "Slide title:" ), canvas );
    label->setAlignment( Qt::AlignVCenter | Qt::AlignRight );
    layout->addWidget( label, 1, 0 );

    slideTitle = new KLineEdit( canvas );
    layout->addWidget( slideTitle, 1, 1 );
    connect( slideTitle, SIGNAL( textChanged( const QString & ) ), this,
             SLOT( slideTitleChanged( const QString & ) ) );

    slideTitles = new K3ListView( canvas );
    layout->addMultiCellWidget( slideTitles, 2, 2, 0, 1 );
    slideTitles->addColumn( i18n( "No." ) );
    slideTitles->addColumn( i18n( "Slide Title" ) );
    connect( slideTitles, SIGNAL( selectionChanged( Q3ListViewItem * ) ), this,
             SLOT( slideTitleChanged( Q3ListViewItem * ) ) );
    slideTitles->setSorting( -1 );
    slideTitles->setAllColumnsShowFocus( true );
    slideTitles->setResizeMode( Q3ListView::LastColumn );
    slideTitles->header()->setMovingEnabled( false );

    Q3ValueList<KPrWebPresentation::SlideInfo> infos = webPres.getSlideInfos();
    for ( int i = infos.count() - 1; i >= 0; --i ) {
        K3ListViewItem *item = new K3ListViewItem( slideTitles );
        item->setText( 0, QString::number( i + 1 ) );
        //kDebug(33001) << "KPrWebPresentationWizard::setupPage3 " << infos[ i ].slideTitle << endl;
        item->setText( 1, infos[ i ].slideTitle );
    }

    slideTitles->setSelected( slideTitles->firstChild(), true );

    addPage( page4, i18n( "Step 4: Customize Slide Titles" ) );

    setHelpEnabled(page4, false);  //doesn't do anything currently
}

void KPrWebPresentationWizard::setupPage5()
{
    page5 = new KHBox( this );
    page5->setWhatsThis( i18n("This page allows you to specify some options for "
                                 "presentations which run unattended, such as time "
                                 "elapsed before advancing to the next slide, looping "
                                 "and the presence of headers. If you do not want "
                                 "an unattended presentation, just leave defaults unchanged.") );
    page5->setSpacing( KDialog::spacingHint() );
    page5->setMargin( KDialog::marginHint() );

    QLabel* sidebar = new QLabel( page5 );
    sidebar->setMinimumSize( 106, 318 );
    sidebar->setMaximumSize( 106, 318 );
    sidebar->setFrameShape( Q3Frame::Panel );
    sidebar->setFrameShadow( Q3Frame::Sunken );
    sidebar->setPixmap(KStandardDirs::locate("data", "kpresenter/pics/webslideshow-sidebar.png"));

    QWidget* canvas = new QWidget( page5 );
    Q3GridLayout *layout = new Q3GridLayout( canvas, 6, 2,
                                           KDialog::marginHint(), KDialog::spacingHint() );

    QLabel *helptext = new QLabel( canvas );
    helptext->setAlignment( Qt::TextWordWrap | Qt::AlignVCenter| Qt::AlignLeft );
    QString help = i18n("Here you can configure some options for unattended "
                        "presentations, such as time elapsed before automatically advance to "
                        "the next slide, looping and the presence of headers.");
    helptext->setText(help);

    layout->addMultiCellWidget( helptext, 0, 0, 0, 1 );

    layout->addMultiCell( new QSpacerItem( 1, 50 ), 1, 1, 0, 1 );

    QLabel *label1 = new QLabel( i18n("Advance after:"), canvas );
    label1->setAlignment( Qt::AlignVCenter | Qt::AlignRight );
    label1->setWhatsThis( i18n( "This selection allows you to specify "
                                   "the time between slides." ) );
    layout->addWidget( label1, 2, 0 );

    timeBetweenSlides = new KIntNumInput( webPres.getTimeBetweenSlides(), canvas );
    timeBetweenSlides->setSpecialValueText(i18n( "Disabled" ));
    timeBetweenSlides->setWhatsThis( i18n( "This selection allows you to specify "
                                 "the time between slides." ) );
    layout->addWidget( timeBetweenSlides, 2, 1 );
    timeBetweenSlides->setSuffix( " seconds" );
    timeBetweenSlides->setRange( 0, 900, 1 );

    layout->addMultiCell( new QSpacerItem( 1, 10 ), 1, 1, 0, 1 );

    writeHeader=new QCheckBox( i18n("Write header to the slides"), canvas);
    writeHeader->setWhatsThis( i18n( "This checkbox allows you to specify if you "
                                       "want to write the navigation buttons on top "
                                       "of the slide." ) );
    writeHeader->setChecked( webPres.wantHeader() );
    layout->addWidget( writeHeader, 3, 1);

    writeFooter=new QCheckBox( i18n("Write footer to the slides"), canvas);
    writeFooter->setWhatsThis( i18n( "This checkbox allows you to specify if you "
                                       "want to write an imprint consisting on the author "
                                       "and the software used to create these slides." ) );
    writeFooter->setChecked( webPres.wantFooter() );
    layout->addWidget( writeFooter, 4, 1);

    loopSlides=new QCheckBox( i18n("Loop presentation"), canvas);
    loopSlides->setWhatsThis( i18n( "This checkbox allows you to specify if you "
                                       "want the presentation to start again once "
                                       "the latest slide is reached." ) );
    loopSlides->setChecked( webPres.wantLoopSlides() );
    layout->addWidget( loopSlides, 5, 1);

    QSpacerItem* spacer = new QSpacerItem( 1, 10,
                                           QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout->addMultiCell( spacer, 5, 5, 0, 1 );

    addPage( page5, i18n( "Step 5: Options for Unattended Presentations" ) );

    setHelpEnabled(page5, false);  //doesn't do anything currently

    setFinish( page5, true );
}

void KPrWebPresentationWizard::finish()
{
    webPres.setAuthor( author->text() );
    webPres.setEMail( email->text() );
    webPres.setTitle( title->text() );

    Q3ListViewItemIterator it( slideTitles );
    for ( ; it.current(); ++it )
        webPres.setSlideTitle( it.current()->text( 0 ).toInt() - 1, it.current()->text( 1 ) );

    webPres.setBackColor( backColor->color() );
    webPres.setTitleColor( titleColor->color() );
    webPres.setTextColor( textColor->color() );
    webPres.setPath( path->lineEdit()->text() );
    webPres.setZoom( zoom->value() );
    webPres.setTimeBetweenSlides( timeBetweenSlides->value() );
    webPres.setWriteHeader( writeHeader->isChecked() );
    webPres.setWriteFooter( writeFooter->isChecked() );
    webPres.setLoopSlides( loopSlides->isChecked() );
    webPres.setXML( doctype->currentIndex() != 0 );
    bool found = false;
    QTextCodec *codecForEnc = KGlobal::charsets()->codecForName(KGlobal::charsets()->encodingForName(encoding->currentText()), found);
    if ( found )
    {
        webPres.setEncoding( codecForEnc->name() );
    }

    close();
    KPrWebPresentationCreateDialog::createWebPresentation( doc, view, webPres );
}

void KPrWebPresentationWizard::pageChanged()
{
    if ( currentPage() != page5 )
    {
        QString pathname = path->lineEdit()->text();

        // path doesn't exist. ask user if it should be created.
        if ( !KIO::NetAccess::exists( pathname, true/*write*/,this ) )
        {
            QString msg = i18n( "<qt>The directory <b>%1</b> does not exist.<br>"
                                "Do you want create it?</qt>" );
            if( KMessageBox::questionYesNo( this, msg.arg( pathname ),
                                            i18n( "Directory Not Found" ) )
                == KMessageBox::Yes)
            {
                bool ok = KIO::NetAccess::mkdir( pathname, this );
                if( !ok )
                {
                    KMessageBox::sorry( this,
                                        i18n( "Cannot create directory." ) );
                    // go back to first step
                    showPage( page1 );
                    path->setFocus();
                }

            }
            else
            {
                // go back to first step
                showPage( page1 );
                path->setFocus();
            }
        }
    } else
        finishButton()->setEnabled( true );
}

void KPrWebPresentationWizard::slideTitleChanged( const QString &s )
{
    if ( slideTitles->currentItem() )
        slideTitles->currentItem()->setText( 1, s );
}

void KPrWebPresentationWizard::slideTitleChanged( Q3ListViewItem *i )
{
    if ( !i ) return;

    slideTitle->setText( i->text( 1 ) );
    view->skipToPage( i->text( 0 ).toInt() - 1 );
}

void KPrWebPresentationWizard::closeEvent( QCloseEvent *e )
{
    view->enableWebPres();
    K3Wizard::closeEvent( e );
}

void KPrWebPresentationWizard::slotChoosePath(const QString &text)
{
    webPres.setPath(text);
}

KPrWebPresentationCreateDialog::KPrWebPresentationCreateDialog( KPrDocument *_doc, KPrView *_view,
                                                              const KPrWebPresentation &_webPres )
    : QDialog( 0, "", false ), webPres( _webPres )
{
    doc = _doc;
    view = _view;

    setupGUI();
}

KPrWebPresentationCreateDialog::~KPrWebPresentationCreateDialog()
{
    view->enableWebPres();
}

void KPrWebPresentationCreateDialog::createWebPresentation( KPrDocument *_doc, KPrView *_view,
                                                           const KPrWebPresentation &_webPres )
{
    KPrWebPresentationCreateDialog *dlg = new KPrWebPresentationCreateDialog( _doc, _view, _webPres );

    dlg->setCaption( i18n( "Create HTML Slideshow" ) );
    dlg->resize( 400, 300 );
    dlg->show();
    dlg->start();
}

void KPrWebPresentationCreateDialog::start()
{
    setCursor( Qt::waitCursor );
    initCreation();
    createSlidesPictures();
    createSlidesHTML();
    createMainPage();
    setCursor( Qt::arrowCursor );

    bDone->setEnabled( true );
    bSave->setEnabled( true );
}

void KPrWebPresentationCreateDialog::initCreation()
{
    QFont f = step1->font(), f2 = step1->font();
    f.setBold( true );
    step1->setFont( f );

    progressBar->setValue( 0 );
    progressBar->setMaximum( webPres.initSteps() );

    webPres.initCreation( progressBar );

    step1->setFont( f2 );
    progressBar->setValue( progressBar->maximum() );
}

void KPrWebPresentationCreateDialog::createSlidesPictures()
{
    QFont f = step2->font(), f2 = f;
    f.setBold( true );
    step2->setFont( f );

    progressBar->setValue( 0 );
    if ( webPres.slides1Steps() > 0 )
    {
        progressBar->setMaximum( webPres.slides1Steps() );
        webPres.createSlidesPictures( progressBar );
    }

    step2->setFont( f2 );
    progressBar->setValue( progressBar->maximum() );
}

void KPrWebPresentationCreateDialog::createSlidesHTML()
{
    QFont f = step3->font(), f2 = step3->font();
    f.setBold( true );
    step3->setFont( f );

    progressBar->setValue( 0 );
    if ( webPres.slides1Steps() > 0 )
    {
        progressBar->setMaximum( webPres.slides1Steps() );
        webPres.createSlidesHTML( progressBar );
    }

    step3->setFont( f2 );
    progressBar->setValue( progressBar->maximum() );
}

void KPrWebPresentationCreateDialog::createMainPage()
{
    QFont f = step4->font(), f2 = step4->font();
    f.setBold( true );
    step4->setFont( f );

    progressBar->setValue( 0 );
    progressBar->setMaximum( webPres.slides1Steps() );

    webPres.createMainPage( progressBar );

    step4->setFont( f2 );
    progressBar->setValue( progressBar->maximum() );
}

void KPrWebPresentationCreateDialog::setupGUI()
{
    back = new KVBox( this );
    back->setMargin( KDialog::marginHint() );

    Q3Frame *line;

    line = new Q3Frame( back );
    line->setFrameStyle( Q3Frame::HLine | Q3Frame::Sunken );
    line->setMaximumHeight( 20 );

    step1 = new QLabel( i18n( "Initialize (create file structure, etc.)" ), back );
    step2 = new QLabel( i18n( "Create Pictures of the Slides" ), back );
    step3 = new QLabel( i18n( "Create HTML Pages for the Slides" ), back );
    step4 = new QLabel( i18n( "Create Main Page (Table of Contents)" ), back );
    step5 = new QLabel( i18n( "Options for Unattended Presentations" ), back);

    line = new Q3Frame( back );
    line->setFrameStyle( Q3Frame::HLine | Q3Frame::Sunken );
    line->setMaximumHeight( 20 );

    progressBar = new KProgressBar( back );

    line = new Q3Frame( back );
    line->setFrameStyle( Q3Frame::HLine | Q3Frame::Sunken );
    line->setMaximumHeight( 20 );

    KButtonBox *bb = new KButtonBox( back );
    bSave = bb->addButton( i18n( "Save Configuration..." ) );
    bb->addStretch();
    bDone = bb->addButton( i18n( "Done" ) );

    bSave->setEnabled( false );
    bDone->setEnabled( false );

    connect( bDone, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( bSave, SIGNAL( clicked() ), this, SLOT( saveConfig() ) );
}

void KPrWebPresentationCreateDialog::resizeEvent( QResizeEvent *e )
{
    QDialog::resizeEvent( e );
    back->resize( size() );
}

void KPrWebPresentationCreateDialog::saveConfig()
{
    QString filename = webPres.getConfig();
    if ( QFileInfo( filename ).exists() )
        filename = QFileInfo( filename ).absoluteFilePath();
    else
        filename = QString::null;

    KFileDialog fd (filename, i18n("*.kpweb|KPresenter Web-Presentation (*.kpweb)"), this
                    );
    fd.setCaption (i18n ("Save Web Presentation Configuration"));
    fd.setOperationMode (KFileDialog::Saving);
    fd.setMode (KFile::File | KFile::LocalOnly);

    if (fd.exec ())
    {
        webPres.setConfig( fd.selectedFile () );
        webPres.saveConfig();
    }
}

#include "KPrGradient.h"
#include "KPrWebPresentation.moc"
