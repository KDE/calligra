/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright 2001, 2002 Nicolas GOUTTE <goutte@kde.org>

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

#include <webpresentation.h>

#include <kpresenter_view.h>
#include "kprcanvas.h"

#include <kstandarddirs.h>
#include <unistd.h>
#include <sys/types.h>

#include <qfile.h>
#include <qtextstream.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qfileinfo.h>
#include <qframe.h>
#include <qfont.h>
#include <qpixmap.h>
#include <qdatetime.h>
#include <qdir.h>
#include <qheader.h>
#include <qwmatrix.h>
#include <qtextcodec.h>
#include <qregexp.h>
#include <qimage.h>
#include <qlayout.h>

#include <kdebug.h>
#include <klocale.h>
#include <kcolorbutton.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kbuttonbox.h>
#include <ksimpleconfig.h>
#include <kimageio.h>
#include <kapplication.h>
#include <kprogress.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <kcharsets.h>
#include <kurlrequester.h>
#include <klineedit.h>
#include <klistview.h>
#include <knuminput.h>
#include <kcombobox.h>
#include <kurl.h>
#include <kio/netaccess.h>
#include <kdialog.h>

#include "koDocumentInfo.h"


// Comes from koffice/filters/libexport/KWEFUtils.cc
static QString EscapeSgmlText(const QTextCodec* codec,
    const QString& strIn, const bool quot = false , const bool apos = false )
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

// Escape only if the ecoding do not support the character
// Speical SGML charcaters like < > & are supposed to be already escpaed.
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

/******************************************************************/
/* Class: KPWebPresentation                                       */
/******************************************************************/

/*================================================================*/
KPWebPresentation::KPWebPresentation( KPresenterDoc *_doc, KPresenterView *_view )
    : config( QString::null )
{
    doc = _doc;
    view = _view;
    init();
}

/*================================================================*/
KPWebPresentation::KPWebPresentation( const QString &_config, KPresenterDoc *_doc, KPresenterView *_view )
    : config( _config )
{
    doc = _doc;
    view = _view;
    init();
    loadConfig();
}

/*================================================================*/
KPWebPresentation::KPWebPresentation( const KPWebPresentation &webPres )
    : config( webPres.config ), author( webPres.author ), title( webPres.title ), email( webPres.email ),
      slideInfos( webPres.slideInfos ), backColor( webPres.backColor ), titleColor( webPres.titleColor ),
      textColor( webPres.textColor ), path( webPres.path ), imgFormat( webPres.imgFormat ), zoom( webPres.zoom ),
      m_encoding( webPres.m_encoding )
{
    doc = webPres.doc;
    view = webPres.view;
}

/*================================================================*/
void KPWebPresentation::loadConfig()
{
    if ( config.isEmpty() )
        return;

    KSimpleConfig cfg( config );
    cfg.setGroup( "General" );

    author = cfg.readEntry( "Author", author );
    title = cfg.readEntry( "Title", title );
    email = cfg.readEntry( "EMail", email );
    unsigned int num = cfg.readNumEntry( "Slides", slideInfos.count() );
    //kdDebug(33001) << "KPWebPresentation::loadConfig num=" << num << endl;

    if ( num <= slideInfos.count() ) {
        for ( unsigned int i = 0; i < num; i++ )
        {
            QString key = QString::fromLatin1( "SlideTitle%1" ).arg( i );
            if ( cfg.hasKey( key ) )
            {
                // We'll assume that the selected pages haven't changed... Hmm.
                slideInfos[ i ].slideTitle = cfg.readEntry( key );
                kdDebug(33001) << "KPWebPresentation::loadConfig key=" << key << " data=" << slideInfos[i].slideTitle << endl;
            } else kdDebug(33001) << " key not found " << key << endl;
        }
    }

    backColor = cfg.readColorEntry( "BackColor", &backColor );
    titleColor = cfg.readColorEntry( "TitleColor", &titleColor );
    textColor = cfg.readColorEntry( "TextColor", &textColor );
    path = cfg.readEntry( "Path", path );
    imgFormat = static_cast<ImageFormat>( cfg.readNumEntry( "ImageFormat", static_cast<int>( imgFormat ) ) );
    zoom = cfg.readNumEntry( "Zoom", zoom );
    m_encoding = cfg.readEntry( "Encoding", m_encoding );
}

/*================================================================*/
void KPWebPresentation::saveConfig()
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
    cfg.writeEntry( "Path", path );
    cfg.writeEntry( "ImageFormat", static_cast<int>( imgFormat ) );
    cfg.writeEntry( "Zoom", zoom );
    cfg.writeEntry( "Encoding", m_encoding );
}

/*================================================================*/
void KPWebPresentation::initCreation( KProgress *progressBar )
{
    QString cmd;
    int p;

    QDir( path ).mkdir( path + "/html" );

    p = progressBar->progress();
    progressBar->setProgress( ++p );
    kapp->processEvents();

    QDir( path ).mkdir( path + "/pics" );

    p = progressBar->progress();
    progressBar->setProgress( ++p );
    kapp->processEvents();

    QString format = "." + imageFormat( imgFormat );

    const char *pics[] = { "home", "first", "next", "prev", "last", 0 };

    KURL srcurl, desturl;
    
    for ( uint index = 0; pics[ index ]; index ++ )    
    {
        QString filename( pics[ index ] + format );
        srcurl.setPath( locate( "slideshow", filename, KPresenterFactory::global() ) );
        desturl.setPath ( path + "/pics/" + filename );
        KIO::NetAccess::del( desturl ); // Copy does not remove existing destination file
        KIO::NetAccess::copy( srcurl, desturl );
        p = progressBar->progress();
        progressBar->setProgress( ++p );
        kapp->processEvents();
    }
}

/*================================================================*/
void KPWebPresentation::createSlidesPictures( KProgress *progressBar )
{
    if ( slideInfos.isEmpty() )
        return;
    QPixmap pix( 10, 10 );
    QString filename;
    QString format = imageFormat( imgFormat );
    int p;
    for ( unsigned int i = 0; i < slideInfos.count(); i++ ) {
        int pgNum = slideInfos[i].pageNumber;
        view->getCanvas()->drawPageInPix( pix, pgNum, zoom, true /*force real variable value*/ );
        filename = QString( "%1/pics/slide_%2.%3" ).arg( path ).arg( i + 1 ).arg( format );

        pix.save( filename, format.upper().latin1() );   //lukas: provide the option to choose image quality

        p = progressBar->progress();
        progressBar->setProgress( ++p );
        kapp->processEvents();
    }
}

/*================================================================*/
QString KPWebPresentation::escapeHtmlText( QTextCodec *codec, const QString& strText ) const
{
    // Escape quotes (needed in attributes)
    // Do not escape apostrophs (only allowed in XHTML!)
    return EscapeSgmlText( codec, strText, true, false );
}

/*================================================================*/
void KPWebPresentation::writeStartOfHeader(QTextStream& streamOut, QTextCodec *codec, const bool xhtml, const QString& subtitle)
{
    QString mimeName ( codec->mimeName() );
    if (xhtml)
    {   //Write out the XML declaration
        streamOut << "<?xml version=\"1.0\" encoding=\""
            << mimeName << "\"?>\n";
    }
    // write <!DOCTYPE
    streamOut << "<!DOCTYPE ";
    if (xhtml)
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
    if (xhtml)
    {
        // XHTML has an extra attribute defining its namespace (in the <html> opening tag)
        streamOut << " xmlns=\"http://www.w3.org/1999/xhtml\"";
    }
    streamOut << ">\n" << "<head>\n";

    // Declare what charset we are using
    streamOut << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=";
    streamOut << mimeName << '"' << (xhtml?" /":"") << ">\n" ;

    // Tell who we are (with the CVS revision number) in case we have a bug in our output!
    QString strVersion("$Revision$");
    // Eliminate the dollar signs
    //  (We don't want that the version number changes if the HTML file is itself put in a CVS storage.)
    streamOut << "<meta name=\"Generator\" content=\"KPresenter's Web Presentation "
        << strVersion.mid(10).replace("$","")
        << "\""<< (xhtml?" /":"") // X(HT)ML closes empty elements, HTML not!
        << ">\n";

    streamOut << "<title>"<< escapeHtmlText( codec, title ) << " - " << escapeHtmlText( codec, subtitle ) << "</title>\n";

    // ### TODO: transform documentinfo.xml into many <META> elements (at least the author!)
}

/*================================================================*/
void KPWebPresentation::createSlidesHTML( KProgress *progressBar )
{
    QTextCodec *codec = KGlobal::charsets()->codecForName( m_encoding );
    QString format ( imageFormat( imgFormat ) );
    
    bool xhtml=false; // ### TODO: XHTML 1.0 support in dialog
    
    const QString brtag ( "<br" + QString(xhtml?" /":"") + ">" );

    for ( unsigned int i = 0; i < slideInfos.count(); i++ ) {
        
        unsigned int pgNum = i + 1;
        
        QFile file( QString( "%1/html/slide_%2.html" ).arg( path ).arg( pgNum ) );
        file.open( IO_WriteOnly );
        QTextStream streamOut( &file );
        streamOut.setCodec( codec );

        writeStartOfHeader( streamOut, codec, xhtml, slideInfos[ i ].slideTitle );

        // ### TODO: transform documentinfo.xml into many <META> elements (at least the author!)

        if ( i > 0 ) {
            streamOut <<  "<link rel=\"first\" href=\"slide_1.html\"" << (xhtml?" /":"") << ">\n";
            streamOut <<  "<link rel=\"prev\" href=\"slide_" << pgNum - 1 << ".html\"" << (xhtml?" /":"") << ">\n";
        }
        if ( i < slideInfos.count() - 1 ) {
            streamOut <<  "<link rel=\"next\" href=\"slide_" << pgNum + 1 << ".html\"" << (xhtml?" /":"") << ">\n";
            streamOut <<  "<link rel=\"last\" href=\"slide_" << slideInfos.count() << ".html\"" << (xhtml?" /":"") << ">\n";
        }
        streamOut <<  "<link rel=\"contents\" href=\"../index.html\"" << (xhtml?" /":"") << ">\n";

        streamOut << "</head>\n";
        streamOut << "<body bgcolor=\"" << backColor.name() << "\" text=\"" << textColor.name() << "\">\n";
        
        streamOut << "  <center>\n";
        
        if ( i > 0 )
            streamOut << "    <a href=\"slide_1.html\">";
        streamOut << "<img src=\"../pics/first." << format << "\" border=\"0\" alt=\"First\" title=\"First\"" << (xhtml?" /":"") << ">";
        if ( i > 0 )
            streamOut << "</a>";

        streamOut << "\n";
            
        if ( i > 0 )
            streamOut << "    <a href=\"slide_" << pgNum - 1 << ".html\">";
        streamOut << "<img src=\"../pics/prev." << format << "\" border=\"0\" alt=\"Previous\" title=\"Previous\"" << (xhtml?" /":"") << ">";
        if ( i > 0 )
            streamOut << "</a>";

        streamOut << "\n";
        
        if ( i < slideInfos.count() - 1 )
            streamOut << "    <a href=\"slide_" << pgNum + 1 << ".html\">";
        streamOut << "<img src=\"../pics/next." << format << "\" border=\"0\" alt=\"Next\" title=\"Next\"" << (xhtml?" /":"") << ">";;
        if ( i < slideInfos.count() - 1 )
            streamOut << "</a>";

        streamOut << "\n";
        
        if ( i < slideInfos.count() - 1 )
            streamOut << "    <a href=\"slide_" << slideInfos.count() << ".html\">";
        streamOut << "<img src=\"../pics/last." << format << "\" border=\"0\" alt=\"Last\" title=\"Last\"" << (xhtml?" /":"") << ">";;
        if ( i < slideInfos.count() - 1 )
            streamOut << "</a>";

        streamOut << "\n" << "    &nbsp; &nbsp; &nbsp; &nbsp;\n";

        streamOut << "    <a href=\"../index.html\">";
        streamOut << "<img src=\"../pics/home." << format << "\" border=\"0\" alt=\"Home\" title=\"Home\"" << (xhtml?" /":"") << ">";;
        streamOut << "</a>\n";

        streamOut << " </center>" << brtag << "<hr noshade=\"noshade\"" << (xhtml?" /":"") << ">\n"; // ### TODO: is noshade W3C?

        streamOut << "  <center>\n    <font color=\"" << escapeHtmlText( codec, titleColor.name() ) << "\">\n";
        streamOut << "    <b>" << escapeHtmlText( codec, title ) << "</b> - <i>" << escapeHtmlText( codec, slideInfos[ i ].slideTitle ) << "</i>\n";

        streamOut << "    </font>\n  </center><hr noshade=\"noshade\"" << (xhtml?" /":"") << ">" << brtag << "\n";

        streamOut << "  <center>\n    ";

        if ( i < slideInfos.count() - 1 )
            streamOut << "<a href=\"slide_" << pgNum + 1 << ".html\">";
        streamOut << "<img src=\"../pics/slide_" << pgNum << "." << format << "\" border=\"0\" alt=\"Slide " << pgNum << "\">";
        if ( i < slideInfos.count() - 1 )
            streamOut << "</a>";

        streamOut << "\n";
        
        streamOut << "    </center>" << brtag << "<hr noshade=\"noshade\"" << (xhtml?" /":"") << ">\n";

        QPtrList<KPrPage> _tmpList( doc->getPageList() );
        QString note ( escapeHtmlText( codec, _tmpList.at(i)->noteText() ) );
        if ( !note.isEmpty() ) {
            streamOut << "  <b>" << escapeHtmlText( codec, i18n( "Note" ) ) << "</b>\n";
            streamOut << " blockquote>\n";
            
            streamOut << note.replace( "\n", brtag );

            streamOut << "  </blockquote><hr noshade=\"noshade\"" << (xhtml?" /":"") << ">\n";
        }

        streamOut << "  <center>\n";
        
        QString htmlAuthor;
        if (email.isEmpty())
            htmlAuthor=escapeHtmlText( codec, author );
        else
            htmlAuthor=QString("<a href=\"mailto:%1\">%2</a>").arg( escapeHtmlText( codec, email )).arg( escapeHtmlText( codec, author ));
        streamOut << EscapeEncodingOnly ( codec, i18n( "Created on %1 by <i>%2</i> with <a href=\"http://www.koffice.org/kpresenter\">KPresenter</a>" ) 
            .arg( KGlobal::locale()->formatDate ( QDate::currentDate() ) ).arg( htmlAuthor ) );

        streamOut << "    </center><hr noshade=\"noshade\"" << (xhtml?" /":"") << ">\n";
        streamOut << "</body>\n</html>\n";

        file.close();

        int p = progressBar->progress();
        progressBar->setProgress( ++p );
        kapp->processEvents();
    }
}

/*================================================================*/
void KPWebPresentation::createMainPage( KProgress *progressBar )
{
    QString html;

    QTextCodec *codec = KGlobal::charsets()->codecForName( m_encoding );

    bool xhtml=false; // ### TODO: XHTML 1.0 support in dialog
    
    const QString brtag ( "<br" + QString(xhtml?" /":"") + ">" );

    QFile file( QString( "%1/index.html" ).arg( path ) );
    file.open( IO_WriteOnly );
    QTextStream streamOut( &file );
    streamOut.setCodec( codec );
        
    writeStartOfHeader( streamOut, codec, xhtml, i18n("Table of Contents") );
    
    streamOut << "</head>\n";
    streamOut << "<body bgcolor=\"" << backColor.name() << "\" text=\"" << textColor.name() << "\">\n";

    streamOut << "<font color=\"" << titleColor.name() << "\">\n";
    streamOut << brtag << "<center><h1>" << title << "</h1></center>\n";
    streamOut << "</font>\n";

    streamOut << brtag << brtag << "<center><h3><a href=\"html/slide_1.html\">";
    streamOut << i18n("Click here to start the Slideshow");
    streamOut << "</a></h3></center>" << brtag << "\n";

    streamOut << "<hr noshade=\"noshade\"" << (xhtml?" /":"") << ">" << "\n";

    streamOut << brtag << brtag << "\n<b>" << i18n("Table of Contents") << "</b>" << brtag << "\n";
    streamOut << "<ol>\n";

    for ( unsigned int i = 0; i < slideInfos.count(); i++ )
        streamOut << "  <li><a href=\"html/slide_" << i+1 << ".html\">" << slideInfos[ i ].slideTitle << "</a></li>\n";

    streamOut << "</ol>\n";

    streamOut << "<hr noshade=\"noshade\"" << (xhtml?" /":"") << ">" << "\n";
    
    QString htmlAuthor;
    if (email.isEmpty())
        htmlAuthor=escapeHtmlText( codec, author );
    else
        htmlAuthor=QString("<a href=\"mailto:%1\">%2</a>").arg( escapeHtmlText( codec, email )).arg( escapeHtmlText( codec, author ));
    streamOut << EscapeEncodingOnly ( codec, i18n( "Created on %1 by <i>%2</i> with <a href=\"http://www.koffice.org/kpresenter\">KPresenter</a>" ) 
        .arg( KGlobal::locale()->formatDate ( QDate::currentDate() ) ).arg( htmlAuthor ) );

    streamOut << "</body>\n</html>\n";
    file.close();

    progressBar->setProgress( progressBar->totalSteps() );
    kapp->processEvents();
}

/*================================================================*/
void KPWebPresentation::init()
{

    KoDocumentInfo * info = doc->documentInfo();
    KoDocumentInfoAuthor * authorPage = static_cast<KoDocumentInfoAuthor *>(info->page( "author" ));
    if ( !authorPage )
        kdWarning() << "Author information not found in documentInfo !" << endl;
    else
    {
        author = authorPage->fullName();
        email = authorPage->email();
    }

    title = i18n("Slideshow");
    kdDebug(33001) << "KPWebPresentation::init : " << doc->getPageNums() << " pages." << endl;
    for ( unsigned int i = 0; i < doc->getPageNums(); i++ )
    {
        if ( doc->isSlideSelected( i ) )
        {
            SlideInfo info;
            info.pageNumber = i;
            info.slideTitle = doc->pageList().at(i)->pageTitle( i18n( "Slide %1" ).arg( i+1 ) );
            slideInfos.append( info );
        }
    }
    if ( slideInfos.isEmpty() )
        kdWarning() << "No slides selected!" << endl;
    backColor = Qt::white;
    textColor = Qt::black;
    titleColor = Qt::red;
    // ### TODO: make PNG the default. All modern user agents should know PNG nowadays.
    if (KImageIO::canWrite("JPEG"))
        imgFormat = JPEG;
    else
        imgFormat = PNG;

    path = KGlobalSettings::documentPath() + "www";

    zoom = 100;
    m_encoding = QTextCodec::codecForLocale()->name();
}

/******************************************************************/
/* Class: KPWebPresentationWizard                                 */
/******************************************************************/

/*================================================================*/
KPWebPresentationWizard::KPWebPresentationWizard( const QString &_config, KPresenterDoc *_doc,
                                                  KPresenterView *_view )
    : KWizard( 0, "", false ), config( _config ), webPres( config, _doc, _view )
{
    doc = _doc;
    view = _view;

    setupPage1();
    setupPage2();
    setupPage3();

    connect( nextButton(), SIGNAL( clicked() ), this, SLOT( pageChanged() ) );
    connect( backButton(), SIGNAL( clicked() ), this, SLOT( pageChanged() ) );
    connect( finishButton(), SIGNAL( clicked() ), this, SLOT( finish() ) );
}

/*================================================================*/
KPWebPresentationWizard::~KPWebPresentationWizard()
{
    view->enableWebPres();
}

/*================================================================*/
void KPWebPresentationWizard::createWebPresentation( const QString &_config, KPresenterDoc *_doc,
                                                     KPresenterView *_view )
{
    KPWebPresentationWizard *dlg = new KPWebPresentationWizard( _config, _doc, _view );

    dlg->setCaption( i18n( "Create HTML Slideshow" ) );
    dlg->resize( 640, 350 );
    dlg->show();
}

/*================================================================*/
void KPWebPresentationWizard::setupPage1()
{
    page1 = new QHBox( this );
    page1->setSpacing( 5 );
    page1->setMargin( 5 );

    QLabel* sidebar = new QLabel( page1 );
    sidebar->setMinimumSize( 106, 318 );
    sidebar->setMaximumSize( 106, 318 );
    sidebar->setFrameShape( QFrame::Panel );
    sidebar->setFrameShadow( QFrame::Sunken );
    sidebar->setPixmap(locate("data", "kpresenter/pics/webslideshow-sidebar.png"));

    QWidget* canvas = new QWidget( page1 );
    QGridLayout *layout = new QGridLayout( canvas, 6, 2, 
        KDialog::marginHint(), KDialog::spacingHint() );

    QLabel *helptext = new QLabel( canvas );
    helptext->setAlignment( Qt::WordBreak | Qt::AlignVCenter| Qt::AlignLeft );
    helptext->setText( i18n( "Enter your name, email address and "
                             "the title of the web presentation. "
                             "Also enter the path into which the "
                             "web presentation should be created. "
                             "(This must be a directory)." ) );
    layout->addMultiCellWidget( helptext, 0, 0, 0, 1 );

    QLabel *label1 = new QLabel( i18n("Author:"), canvas );
    label1->setAlignment( Qt::AlignVCenter | Qt::AlignRight );
    layout->addWidget( label1, 1, 0 );

    QLabel *label2 = new QLabel( i18n("Title:"), canvas );
    label2->setAlignment( Qt::AlignVCenter | Qt::AlignRight );
    layout->addWidget( label2, 2, 0 );

    QLabel *label3 = new QLabel( i18n("Email address:"), canvas );
    label3->setAlignment( Qt::AlignVCenter | Qt::AlignRight );
    layout->addWidget( label3, 3, 0 );

    QLabel *label4 = new QLabel( i18n("Path:"), canvas );
    label4->setAlignment( Qt::AlignVCenter | Qt::AlignRight );
    layout->addWidget( label4, 4, 0 );

    author = new KLineEdit( webPres.getAuthor(), canvas );
    layout->addWidget( author, 1, 1 );

    title = new KLineEdit( webPres.getTitle(), canvas );
    layout->addWidget( title, 2, 1 );

    email = new KLineEdit( webPres.getEmail(), canvas );
    layout->addWidget( email, 3, 1 );

    path=new KURLRequester( canvas );
    path->setMode( KFile::Directory);
    path->lineEdit()->setText(webPres.getPath());
    layout->addWidget( path, 4, 1 );

    QSpacerItem* spacer = new QSpacerItem( 1, 50 );
    layout->addMultiCell( spacer, 5, 5, 0, 1 );

    connect(path, SIGNAL(textChanged(const QString&)),
           this,SLOT(slotChoosePath(const QString&)));
    connect(path, SIGNAL(urlSelected( const QString& )),
           this,SLOT(slotChoosePath(const QString&)));

    addPage( page1, i18n( "Step 1: General Information" ) );

    setHelpEnabled(page1, false);  //doesn't do anything currently

}

/*================================================================*/
void KPWebPresentationWizard::setupPage2()
{
    page2 = new QHBox( this );
    page2->setSpacing( 6 );
    page2->setMargin( 6 );

    QLabel* sidebar = new QLabel( page2 );
    sidebar->setMinimumSize( 106, 318 );
    sidebar->setMaximumSize( 106, 318 );
    sidebar->setFrameShape( QFrame::Panel );
    sidebar->setFrameShadow( QFrame::Sunken );
    sidebar->setPixmap(locate("data", "kpresenter/pics/webslideshow-sidebar.png"));

    QVBox *canvas = new QVBox( page2 );

    QLabel *helptext = new QLabel( canvas );
    helptext->setAlignment( Qt::WordBreak | Qt::AlignVCenter| Qt::AlignLeft );
    QString help = i18n("Here you can configure the style "
                        "of the web pages (colors). You also "
                        "need to specify the picture format "
                        "which should be used for the slides.\n\n"
                        "BMP is a picture format with a bad "
                        "compression, but is supported by "
                        "old web browsers.");

    if ( KImageIO::canWrite( "PNG" ) )
    {
        help += "\n";
        help += i18n("PNG is a very optimized and well "
                     "compressed format, but may not be "
                     "supported by some old web browsers.");
    }

    if ( KImageIO::canWrite( "JPEG" ) )
    {
        help += "\n";
        help += i18n("JPEG is a picture format with quite a good "
                     "compression and which is supported by "
                     "all web browsers.");
    }

    help += "\n\n";
    help += i18n( "Finally you can also specify the zoom for the slides." );
    helptext->setText(help);

    QHBox *row1 = new QHBox( canvas );
    QHBox *row2 = new QHBox( canvas );
    QHBox *row3 = new QHBox( canvas );
    QHBox *row4 = new QHBox( canvas );
    QHBox *row5 = new QHBox( canvas );
    QHBox *row6 = new QHBox( canvas );

    QLabel *label1 = new QLabel( i18n("Text color:"), row1 );
    label1->setAlignment( Qt::AlignVCenter );
    QLabel *label2 = new QLabel( i18n("Title color:"), row2 );
    label2->setAlignment( Qt::AlignVCenter );
    QLabel *label3 = new QLabel( i18n("Background color:"), row3 );
    label3->setAlignment( Qt::AlignVCenter );
    QLabel *label4 = new QLabel( i18n("Picture format:"), row4 );
    label4->setAlignment( Qt::AlignVCenter );
    QLabel *label5 = new QLabel( i18n("Zoom:"), row5 );
    label5->setAlignment( Qt::AlignVCenter );
    QLabel *label6 = new QLabel( i18n( "Default encoding:" ), row6 );
    label6->setAlignment( Qt::AlignVCenter );

    textColor = new KColorButton( webPres.getTextColor(), row1 );
    titleColor = new KColorButton( webPres.getTitleColor(), row2 );
    backColor = new KColorButton( webPres.getBackColor(), row3 );
    format = new KComboBox( false, row4 );
    format->insertItem( "BMP", -1 );
    format->insertItem( "PNG", -1 );
    if ( KImageIO::canWrite( "JPEG" ) )
        format->insertItem( "JPEG", -1 );
    format->setCurrentItem( static_cast<int>( webPres.getImageFormat() ) );

    zoom = new KIntNumInput( webPres.getZoom(), row5 );
    zoom->setSuffix( " %" );
    zoom->setRange( 1, 1000, 1 );

    encoding = new KComboBox( false, row6 );
    QStringList _strList = KGlobal::charsets()->availableEncodingNames();
    encoding->insertStringList( _strList );
    QString _name = webPres.getEncoding();
    encoding->setCurrentItem( _strList.findIndex( _name.lower() ) );


    addPage( page2, i18n( "Step 2: Choose Style" ) );

    setHelpEnabled(page2, false);  //doesn't do anything currently
}

/*================================================================*/
void KPWebPresentationWizard::setupPage3()
{
    page3 = new QHBox( this );
    page3->setSpacing( 5 );
    page3->setMargin( 5 );

    QLabel* sidebar = new QLabel( page3 );
    sidebar->setMinimumSize( 106, 318 );
    sidebar->setMaximumSize( 106, 318 );
    sidebar->setFrameShape( QFrame::Panel );
    sidebar->setFrameShadow( QFrame::Sunken );
    sidebar->setPixmap(locate("data", "kpresenter/pics/webslideshow-sidebar.png"));

    QVBox *canvas = new QVBox( page3 );

    QLabel *helptext = new QLabel( canvas );
    helptext->setAlignment( Qt::WordBreak | Qt::AlignVCenter| Qt::AlignLeft );
    helptext->setText( i18n( "Here you can specify titles for"
                             "each slide. Click on a slide in "
                             "the list and then enter the title "
                             "in the textbox below. If you "
                             "click on a title, the KPresenter "
                             "mainview will scroll to this "
                             "slide, so it can be seen." ) );

    QHBox *row = new QHBox( canvas );
    QLabel *label = new QLabel( i18n( "Slide title:" ), row );
    label->setAlignment( Qt::AlignVCenter );
    label->setMinimumWidth( label->sizeHint().width() );
    label->setMaximumWidth( label->sizeHint().width() );

    slideTitle = new KLineEdit( row );
    connect( slideTitle, SIGNAL( textChanged( const QString & ) ), this,
             SLOT( slideTitleChanged( const QString & ) ) );

    slideTitles = new KListView( canvas );
    slideTitles->addColumn( i18n( "Slide No." ) );
    slideTitles->addColumn( i18n( "Slide Title" ) );
    connect( slideTitles, SIGNAL( selectionChanged( QListViewItem * ) ), this,
             SLOT( slideTitleChanged( QListViewItem * ) ) );
    slideTitles->setSorting( -1 );
    slideTitles->setAllColumnsShowFocus( true );
    slideTitles->setResizeMode( QListView::LastColumn );
    slideTitles->header()->setMovingEnabled( false );

    QValueList<KPWebPresentation::SlideInfo> infos = webPres.getSlideInfos();
    for ( int i = infos.count() - 1; i >= 0; --i ) {
        KListViewItem *item = new KListViewItem( slideTitles );
        item->setText( 0, QString::number( i + 1 ) );
        //kdDebug(33001) << "KPWebPresentationWizard::setupPage3 " << infos[ i ].slideTitle << endl;
        item->setText( 1, infos[ i ].slideTitle );
    }

    slideTitles->setSelected( slideTitles->firstChild(), true );

    addPage( page3, i18n( "Step 3: Customize Slide Titles" ) );

    setHelpEnabled(page3, false);  //doesn't do anything currently

    setFinish( page3, true );
}


/*================================================================*/
void KPWebPresentationWizard::finish()
{
    webPres.setAuthor( author->text() );
    webPres.setEMail( email->text() );
    webPres.setTitle( title->text() );

    QListViewItemIterator it( slideTitles );
    for ( ; it.current(); ++it )
        webPres.setSlideTitle( it.current()->text( 0 ).toInt() - 1, it.current()->text( 1 ) );

    webPres.setBackColor( backColor->color() );
    webPres.setTitleColor( titleColor->color() );
    webPres.setTextColor( textColor->color() );
    webPres.setImageFormat( static_cast<KPWebPresentation::ImageFormat>( format->currentItem() ) );
    webPres.setPath( path->lineEdit()->text() );
    webPres.setZoom( zoom->value() );
    webPres.setEncoding( encoding->currentText() );

    close();
    KPWebPresentationCreateDialog::createWebPresentation( doc, view, webPres );
}


/*================================================================*/
bool KPWebPresentationWizard::isPathValid() const
{
    QFileInfo fi( path->lineEdit()->text() );

    if ( fi.exists() && fi.isDir() )
        return true;

    return false;
}

/*================================================================*/
void KPWebPresentationWizard::pageChanged()
{
    if ( currentPage() != page3 ) {
        if ( !isPathValid() ) {
            KMessageBox::error( this,
                                i18n( "The path you entered is not a valid directory!\n"
                                      "Please correct this." ),
                                i18n( "Invalid Path" ) );
            showPage( page1 );
            path->setFocus();
        }
    } else
        finishButton()->setEnabled( true );
}

/*================================================================*/
void KPWebPresentationWizard::slideTitleChanged( const QString &s )
{
    if ( slideTitles->currentItem() )
        slideTitles->currentItem()->setText( 1, s );
}

/*================================================================*/
void KPWebPresentationWizard::slideTitleChanged( QListViewItem *i )
{
    if ( !i ) return;

    slideTitle->setText( i->text( 1 ) );
    view->skipToPage( i->text( 0 ).toInt() - 1 );
}

/*================================================================*/
void KPWebPresentationWizard::closeEvent( QCloseEvent *e )
{
    view->enableWebPres();
    QWizard::closeEvent( e );
}

void KPWebPresentationWizard::slotChoosePath(const QString &text)
{
    webPres.setPath(text);
}

/******************************************************************/
/* Class: KPWebPresentationCreateDialog                           */
/******************************************************************/

/*================================================================*/
KPWebPresentationCreateDialog::KPWebPresentationCreateDialog( KPresenterDoc *_doc, KPresenterView *_view,
                                                              const KPWebPresentation &_webPres )
    : QDialog( 0, "", false ), webPres( _webPres )
{
    doc = _doc;
    view = _view;

    setupGUI();
}

/*================================================================*/
KPWebPresentationCreateDialog::~KPWebPresentationCreateDialog()
{
    view->enableWebPres();
}

/*================================================================*/
void KPWebPresentationCreateDialog::createWebPresentation( KPresenterDoc *_doc, KPresenterView *_view,
                                                           const KPWebPresentation &_webPres )
{
    KPWebPresentationCreateDialog *dlg = new KPWebPresentationCreateDialog( _doc, _view, _webPres );

    dlg->setCaption( i18n( "Create HTML Slideshow" ) );
    dlg->resize( 400, 300 );
    dlg->show();
    dlg->start();
}

/*================================================================*/
void KPWebPresentationCreateDialog::start()
{
    setCursor( waitCursor );
    initCreation();
    createSlidesPictures();
    createSlidesHTML();
    createMainPage();
    setCursor( arrowCursor );

    bDone->setEnabled( true );
    bSave->setEnabled( true );
}

/*================================================================*/
void KPWebPresentationCreateDialog::initCreation()
{
    QFont f = step1->font(), f2 = step1->font();
    f.setBold( true );
    step1->setFont( f );

    progressBar->setProgress( 0 );
    progressBar->setTotalSteps( webPres.initSteps() );

    webPres.initCreation( progressBar );

    step1->setFont( f2 );
    progressBar->setProgress( progressBar->totalSteps() );
}

/*================================================================*/
void KPWebPresentationCreateDialog::createSlidesPictures()
{
    QFont f = step2->font(), f2 = f;
    f.setBold( true );
    step2->setFont( f );

    progressBar->setProgress( 0 );
    if ( webPres.slides1Steps() > 0 )
    {
        progressBar->setTotalSteps( webPres.slides1Steps() );
        webPres.createSlidesPictures( progressBar );
    }

    step2->setFont( f2 );
    progressBar->setProgress( progressBar->totalSteps() );
}

/*================================================================*/
void KPWebPresentationCreateDialog::createSlidesHTML()
{
    QFont f = step3->font(), f2 = step3->font();
    f.setBold( true );
    step3->setFont( f );

    progressBar->setProgress( 0 );
    if ( webPres.slides1Steps() > 0 )
    {
        progressBar->setTotalSteps( webPres.slides1Steps() );
        webPres.createSlidesHTML( progressBar );
    }

    step3->setFont( f2 );
    progressBar->setProgress( progressBar->totalSteps() );
}

/*================================================================*/
void KPWebPresentationCreateDialog::createMainPage()
{
    QFont f = step4->font(), f2 = step4->font();
    f.setBold( true );
    step4->setFont( f );

    progressBar->setProgress( 0 );
    progressBar->setTotalSteps( webPres.slides1Steps() );

    webPres.createMainPage( progressBar );

    step4->setFont( f2 );
    progressBar->setProgress( progressBar->totalSteps() );
}

/*================================================================*/
void KPWebPresentationCreateDialog::setupGUI()
{
    back = new QVBox( this );
    back->setMargin( 10 );

    QFrame *line;

    line = new QFrame( back );
    line->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    line->setMaximumHeight( 20 );

    step1 = new QLabel( i18n( "Initialize (create file structure, etc.)" ), back );
    step2 = new QLabel( i18n( "Create Pictures of the Slides" ), back );
    step3 = new QLabel( i18n( "Create HTML pages for the slides" ), back );
    step4 = new QLabel( i18n( "Create Main Page (Table of Contents)" ), back );

    line = new QFrame( back );
    line->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    line->setMaximumHeight( 20 );

    progressBar = new KProgress( back );

    line = new QFrame( back );
    line->setFrameStyle( QFrame::HLine | QFrame::Sunken );
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

/*================================================================*/
void KPWebPresentationCreateDialog::resizeEvent( QResizeEvent *e )
{
    QDialog::resizeEvent( e );
    back->resize( size() );
}

/*================================================================*/
void KPWebPresentationCreateDialog::saveConfig()
{
    QString filename = webPres.getConfig();
    if ( QFileInfo( filename ).exists() )
        filename = QFileInfo( filename ).absFilePath();
    else
        filename = QString::null;

    KURL url = KFileDialog::getOpenURL( filename, i18n("*.kpweb|KPresenter Web-Presentation (*.kpweb)") );

    if( url.isEmpty() )
      return;

    // ### TODO: use KIO::NetAccess for remote files (floppy: is remote!)
    if( !url.isLocalFile() )
    {
      KMessageBox::sorry( 0L, i18n( "Only local files are currently supported." ) );
      return;
    }

    filename = url.path();

    if ( !filename.isEmpty() ) {
        webPres.setConfig( filename );
        webPres.saveConfig();
    }
}

#include <webpresentation.moc>
