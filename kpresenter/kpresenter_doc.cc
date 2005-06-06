/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2005 Thorsten Zachmann <zachmann@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kpresenter_doc.h"
#include "kpresenter_view.h"
#include "kprcanvas.h"
#include "kprpage.h"
#include "kpobject.h"
#include "kplineobject.h"
#include "kprectobject.h"
#include "kpellipseobject.h"
#include "kpautoformobject.h"
#include "kptextobject.h"
#include "kprtextdocument.h"
#include "kppixmapobject.h"
#include "kppieobject.h"
#include "kppartobject.h"
#include "kpgroupobject.h"
#include "kprcommand.h"
#include "insertpagedia.h"
#include "kpfreehandobject.h"
#include "kppolylineobject.h"
#include "kpquadricbeziercurveobject.h"
#include "kpcubicbeziercurveobject.h"
#include "kppolygonobject.h"
#include "kpclosedlineobject.h"

#include <qpopupmenu.h>
#include <qclipboard.h>
#include <qregexp.h>
#include <qfileinfo.h>
#include <qdom.h>
#include <qdict.h>

#include <kodom.h>
#include <koxmlns.h>

#include <kurl.h>
#include <kdebug.h>
#include <koGlobal.h>
#include <kapplication.h>
#include <kurldrag.h>
#include <ktempfile.h>
#include <klocale.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <kprocess.h>
#include <kio/netaccess.h>

#include <koTemplateChooseDia.h>
#include <koRuler.h>
#include <koGenStyles.h>
#include <koFilterManager.h>
#include <koStore.h>
#include <koStoreDevice.h>
#include <koQueryTrader.h>
#include <koxmlwriter.h>
#include <koOasisSettings.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <config.h>

#include <korichtext.h>
#include <kotextobject.h>
#include <kozoomhandler.h>
#include <kostyle.h>
#include <kcommand.h>
#include "KPresenterDocIface.h"

#ifdef HAVE_LIBKSPELL2
#include <kspell2/settings.h>
#endif

#include <kovariable.h>
#include <koAutoFormat.h>
#include <koDocumentInfo.h>
#include "kprvariable.h"
#include "kpbackground.h"
#include "notebar.h"
#include "kprbgspellcheck.h"
#include <kglobalsettings.h>
#include <kocommandhistory.h>
#include "koApplication.h"
#include <koOasisStyles.h>
#include <kooasiscontext.h>

#include "kprloadinginfo.h"

using namespace std;

static const int CURRENT_SYNTAX_VERSION = 2;
// Make sure an appropriate DTD is available in www/koffice/DTD if changing this value
static const char * CURRENT_DTD_VERSION = "1.2";

KPresenterChild::KPresenterChild( KPresenterDoc *_kpr, KoDocument* _doc, const QRect& _rect )
    : KoDocumentChild( _kpr, _doc, _rect )
{
    m_parent = _kpr;
}

KPresenterChild::KPresenterChild( KPresenterDoc *_kpr ) :
    KoDocumentChild( _kpr )
{
    m_parent = _kpr;
}

KPresenterChild::~KPresenterChild()
{
}

KoDocument *KPresenterChild::hitTest( const QPoint &, const QWMatrix & )
{
    return 0L;
}

KPresenterDoc::KPresenterDoc( QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name,
                              bool singleViewMode )
    : KoDocument( parentWidget,widgetName, parent, name, singleViewMode ),
      _gradientCollection(), m_customListTest( 0L ),
      m_childCountBeforeInsert( 0 )
{
    setInstance( KPresenterFactory::global() );
    //Necessary to define page where we load object otherwise copy-duplicate page doesn't work.
    if (KGlobal::locale()->measureSystem() == KLocale::Imperial) {
        m_unit = KoUnit::U_INCH;
    } else {
        m_unit = KoUnit::U_CM;
    }
    m_pageWhereLoadObject=0L;
    m_loadingInfo=0L;
    m_tabStop = MM_TO_POINT( 15.0 );
    m_styleColl=new KoStyleCollection();
    m_insertFilePage = 0;
    m_picturePath= KGlobalSettings::documentPath();
    m_globalLanguage = KGlobal::locale()->language();
    m_bGlobalHyphenation = false;
    _duplicatePage=false;

    KoParagStyle* m_standardStyle = new KoParagStyle( "Standard" );
    m_styleColl->addStyleTemplate( m_standardStyle );

    KConfig *config = KPresenterFactory::global()->config();
    config->setGroup("Document defaults" );
    QString defaultFontname=config->readEntry("DefaultFont");
    if ( !defaultFontname.isEmpty() )
        m_defaultFont.fromString( defaultFontname );
    // If not found, we automatically fallback to the application font (the one from KControl's font module)

    // Try to force a scalable font.
    m_defaultFont.setStyleStrategy( QFont::ForceOutline );
    //kdDebug(33001) << "Default font: requested family: " << m_defaultFont.family() << endl;
    //kdDebug(33001) << "Default font: real family: " << QFontInfo(m_defaultFont).family() << endl;

    int ptSize = m_defaultFont.pointSize();
    if ( ptSize == -1 ) // specified with a pixel size ?
        ptSize = QFontInfo(m_defaultFont).pointSize();
    //kdDebug(33001) << "KPresenterDoc::KPresenterDoc[2] ptSize=" << ptSize << endl;
    // Ok, this is KPresenter. A default font of 10 makes no sense. Let's go for 20.
    ptSize = QMAX( 20, ptSize );

    m_standardStyle->format().setFont( m_defaultFont );

    /// KPresenter isn't color-scheme aware, it defaults to black on white.
    m_standardStyle->format().setColor( Qt::black );

    if( config->hasGroup("Interface") ) {
        config->setGroup( "Interface" );
        m_globalLanguage=config->readEntry("language", KGlobal::locale()->language());
        m_bGlobalHyphenation=config->readBoolEntry("hyphenation", false);
    }

    m_standardStyle->format().setLanguage( m_globalLanguage);

    m_zoomHandler = new KoZoomHandler;

    m_varFormatCollection = new KoVariableFormatCollection;
    m_varColl = new KPrVariableCollection( new KoVariableSettings(), m_varFormatCollection );
#ifdef HAVE_LIBKSPELL2
    m_bgSpellCheck = new KPrBgSpellCheck(this);
#endif
    dcop = 0;
    m_initialActivePage=0;
    m_bShowStatusBar = true;
    m_autoFormat = new KoAutoFormat(this,m_varColl,m_varFormatCollection);
    _clean = true;
    _spInfiniteLoop = false;
    _spManualSwitch = true;
    _showPresentationDuration = false;
    tmpSoundFileList = QPtrList<KTempFile>();
    _xRnd = 20;
    _yRnd = 20;
    _txtBackCol = lightGray;
    _otxtBackCol = lightGray;

    m_bShowRuler=true;
    m_bAllowAutoFormat = true;

    m_bViewFormattingChars = false;
    m_bShowHelplines = false;
    m_bHelplinesToFront = false;
    m_bShowGrid = true;
    m_bGridToFont = false;

    m_bSnapToGrid = true;

    m_cursorInProtectectedArea=true;

    usedSoundFile = QStringList();
    haveNotOwnDiskSoundFile = QStringList();

    m_zoomHandler->setZoomAndResolution( 100, KoGlobal::dpiX(), KoGlobal::dpiY() );
    newZoomAndResolution(false,false);

    //   _pageLayout.format = PG_SCREEN;
    //   _pageLayout.orientation = PG_PORTRAIT;
    //   _pageLayout.width = PG_SCREEN_WIDTH;
    //   _pageLayout.height = PG_SCREEN_HEIGHT;
    //   _pageLayout.left = 0;
    //   _pageLayout.right = 0;
    //   _pageLayout.top = 0;
    //   _pageLayout.bottom = 0;
    //   _pageLayout.ptWidth = cMM_TO_POINT( PG_SCREEN_WIDTH );
    //   _pageLayout.ptHeight = cMM_TO_POINT( PG_SCREEN_HEIGHT );
    //   _pageLayout.ptLeft = 0;
    //   _pageLayout.ptRight = 0;
    //   _pageLayout.ptTop = 0;
    //   _pageLayout.ptBottom = 0;

    //_pageLayout.unit = KoUnit::U_MM;
    m_indent = MM_TO_POINT( 10.0 );
    m_gridX = MM_TO_POINT( 5.0 );
    m_gridY = MM_TO_POINT( 5.0 );

    oldGridX = m_gridX;
    oldGridY = m_gridY;

    m_masterPage=new KPrPage(this);
    KPrPage *newpage = new KPrPage( this, m_masterPage );
    m_pageList.insert( 0,newpage);
    m_bInsertDirectCursor = false;

    objStartY = 0;
    setPageLayout( m_pageLayout );
    _presPen = QPen( red, 3, SolidLine );
    ignoreSticky = TRUE;

    m_gridColor=Qt::black;

    _header = new KPTextObject( this );
    _header->setDrawEditRect( false );
    _header->setDrawEmpty( false );

    _footer = new KPTextObject( this );
    _footer->setDrawEditRect( false );
    _footer->setDrawEmpty( false );

    saveOnlyPage = -1;
    m_maxRecentFiles = 10;

    connect( QApplication::clipboard(), SIGNAL( dataChanged() ),
             this, SLOT( clipboardDataChanged() ) );

    m_commandHistory = new KoCommandHistory( actionCollection(),  true ) ;
    initConfig();

    connect( m_commandHistory, SIGNAL( documentRestored() ), this, SLOT( slotDocumentRestored() ) );
    connect( m_commandHistory, SIGNAL( commandExecuted() ), this, SLOT( slotCommandExecuted() ) );

    connect(m_varColl,SIGNAL(repaintVariable()),this,SLOT(slotRepaintVariable()));
    dcopObject();
}

void KPresenterDoc::refreshMenuCustomVariable()
{
    emit sig_refreshMenuCustomVariable();
}

void KPresenterDoc::slotDocumentRestored()
{
    setModified( false );
}

void KPresenterDoc::slotCommandExecuted()
{
    setModified( true );
}

void KPresenterDoc::setUnit( KoUnit::Unit _unit )
{
    m_unit = _unit;

    QPtrListIterator<KoView> it( views() );
    for( ; it.current(); ++it ) {
        ((KPresenterView*)it.current())->getHRuler()->setUnit( m_unit );
        ((KPresenterView*)it.current())->getVRuler()->setUnit( m_unit );
    }
}

void KPresenterDoc::saveConfig()
{
    if ( isEmbedded() || !isReadWrite())
        return;
    KConfig *config = KPresenterFactory::global()->config();
    config->setGroup( "Interface" );
    config->writeEntry( "Zoom", m_zoomHandler->zoom() );
    config->writeEntry( "AllowAutoFormat" , m_bAllowAutoFormat );
    config->writeEntry( "ViewFormattingChars", m_bViewFormattingChars );
    config->writeEntry( "ShowGrid" , m_bShowGrid );
    config->writeEntry( "GridToFront" , m_bGridToFont );
    config->writeEntry( "SnapToGrid" , m_bSnapToGrid );
    config->writeEntry( "ResolutionX", m_gridX );
    config->writeEntry( "ResolutionY", m_gridY );
    config->writeEntry( "HelpLineToFront" , m_bHelplinesToFront );
}

void KPresenterDoc::initConfig()
{
    int zoom;
    KConfig* config = KPresenterFactory::global()->config();
    if( config->hasGroup("Interface") ) {
        config->setGroup( "Interface" );
        setAutoSave( config->readNumEntry( "AutoSave", defaultAutoSave()/60 ) * 60 );
        setBackupFile( config->readBoolEntry("BackupFile", true));
        setCursorInProtectedArea( config->readBoolEntry( "cursorInProtectArea", true ));

        // Config-file value in mm, default 10 pt
        double indent =  config->readDoubleNumEntry("Indent", MM_TO_POINT(10.0) ) ;
        setIndentValue(indent);
        m_maxRecentFiles = config->readNumEntry( "NbRecentFile", 10 );
        setShowRuler(config->readBoolEntry("Rulers",true));
        zoom = config->readNumEntry( "Zoom", 100 );
        setShowStatusBar( config->readBoolEntry( "ShowStatusBar" , true ));
        setAllowAutoFormat( config->readBoolEntry( "AllowAutoFormat" , true ));
        setViewFormattingChars( config->readBoolEntry( "ViewFormattingChars", false ) );
        setShowGrid( config->readBoolEntry( "ShowGrid" , true ));
        setGridToFront(  config->readBoolEntry( "GridToFront" , false ));
        setSnapToGrid( config->readBoolEntry( "SnapToGrid", true ));
        setGridX( config->readDoubleNumEntry( "ResolutionX", MM_TO_POINT( 5.0 ) ));
        setGridY( config->readDoubleNumEntry( "ResolutionY", MM_TO_POINT( 5.0 ) ));

        setHelpLineToFront( config->readBoolEntry( "HelpLineToFront" , false ));
        m_bInsertDirectCursor= config->readBoolEntry( "InsertDirectCursor", false );
        m_globalLanguage=config->readEntry("language", KGlobal::locale()->language());

    }
    else
        zoom=100;

    QColor oldBgColor = Qt::white;
    QColor oldGridColor = Qt::black;
    if ( config->hasGroup( "KPresenter Color" ) ) {
        config->setGroup( "KPresenter Color" );
        setTxtBackCol(config->readColorEntry( "BackgroundColor", &oldBgColor ));
        setGridColor(config->readColorEntry( "GridColor", &oldGridColor ));
    }


    if( config->hasGroup("KSpell kpresenter" ) )
    {
        config->setGroup( "KSpell kpresenter" );

       // Default is false for spellcheck, but the spell-check config dialog
       // should write out "true" when the user configures spell checking.
#ifdef HAVE_LIBKSPELL2
        if ( isReadWrite() )
          m_bgSpellCheck->setEnabled(config->readBoolEntry( "SpellCheck", false ));
       else
          m_bgSpellCheck->setEnabled( false );
#endif
    }
    int undo=30;
    if(config->hasGroup("Misc" ) )
    {
        config->setGroup( "Misc" );
        undo=config->readNumEntry("UndoRedo",-1);
    }
    if(undo!=-1)
        setUndoRedoLimit(undo);

    if(config->hasGroup("Kpresenter Path" ) )
    {
        config->setGroup( "Kpresenter Path" );
        m_picturePath=config->readPathEntry( "picture path",KGlobalSettings::documentPath());
        setBackupPath(config->readPathEntry( "backup path" ));
    }

    // Apply configuration, without creating an undo/redo command
    replaceObjs( false );
    zoomHandler()->setZoom( zoom );
    newZoomAndResolution(false,false);
}

DCOPObject* KPresenterDoc::dcopObject()
{
    if ( !dcop )
        dcop = new KPresenterDocIface( this );

    return dcop;
}

KPresenterDoc::~KPresenterDoc()
{
    if(isReadWrite())
        saveConfig();
    clearTestCustomSlideShow();
    //Be carefull !!!!!! don't delete this pointer delete in stickypage
#if 0
    delete _header;
    delete _footer;
#endif

    delete m_commandHistory;
    delete m_zoomHandler;
    delete m_autoFormat;
    delete m_varColl;
    delete m_varFormatCollection;
    delete dcop;
    delete m_masterPage;
#ifdef HAVE_LIBKSPELL2
    delete m_bgSpellCheck;
#endif
    delete m_styleColl;

    m_pageList.setAutoDelete( true );
    m_pageList.clear();
    m_deletedPageList.setAutoDelete( true );
    m_deletedPageList.clear();
    tmpSoundFileList.setAutoDelete( true );
    tmpSoundFileList.clear();
}

void KPresenterDoc::addCommand( KCommand * cmd )
{
    kdDebug(33001) << "KPresenterDoc::addCommand " << cmd->name() << endl;
    m_commandHistory->addCommand( cmd, false );
    setModified( true );
}

bool KPresenterDoc::saveChildren( KoStore* _store )
{
    int i = 0;

    QPtrListIterator<KoDocumentChild> it( children() );
    for( ; it.current(); ++it ) {
        // Don't save children that are only in the undo/redo history
        // but not anymore in the presentation
        QPtrListIterator<KPrPage> pageIt( m_pageList );
        for ( int pagePos = 0; pageIt.current(); ++pageIt, ++pagePos )
        {
            if ( saveOnlyPage == -1 || pagePos == saveOnlyPage )
            {
                QPtrListIterator<KPObject> oIt(pageIt.current()->objectList());
                for (; oIt.current(); ++oIt )
                {
                    if ( oIt.current()->getType() == OT_PART &&
                         dynamic_cast<KPPartObject*>( oIt.current() )->getChild() == it.current() )
                    {
                        if (((KoDocumentChild*)(it.current()))->document()!=0)
                            if ( !((KoDocumentChild*)(it.current()))->document()->saveToStore( _store, QString::number( i++ ) ) )
                                return false;
                    }
                }
            }
        }
        if ( saveOnlyPage == -1 )
        {
            QPtrListIterator<KPObject> oIt(m_masterPage->objectList());
            for (; oIt.current(); ++oIt )
            {
                if ( oIt.current()->getType() == OT_PART &&
                        dynamic_cast<KPPartObject*>( oIt.current() )->getChild() == it.current() )
                {
                    if (((KoDocumentChild*)(it.current()))->document()!=0)
                        if ( !((KoDocumentChild*)(it.current()))->document()->saveToStore( _store, QString::number( i++ ) ) )
                            return false;
                }
            }
        }
    }
    return true;
}

QDomDocument KPresenterDoc::saveXML()
{
    if ( saveOnlyPage == -1 ) {
        emit sigProgress( 0 );
    }

    m_varColl->variableSetting()->setModificationDate(QDateTime::currentDateTime());
    recalcVariables( VT_DATE );
    recalcVariables( VT_TIME );

    QDomDocument doc = createDomDocument( "DOC", CURRENT_DTD_VERSION );
    QDomElement presenter=doc.documentElement();
    presenter.setAttribute("editor", "KPresenter");
    presenter.setAttribute("mime", "application/x-kpresenter");
    presenter.setAttribute("syntaxVersion", CURRENT_SYNTAX_VERSION);
    QDomElement paper=doc.createElement("PAPER");
    paper.setAttribute("format", static_cast<int>( m_pageLayout.format ));
    paper.setAttribute("ptWidth", m_pageLayout.ptWidth);
    paper.setAttribute("ptHeight", m_pageLayout.ptHeight);

    paper.setAttribute("orientation", static_cast<int>( m_pageLayout.orientation ));
    paper.setAttribute("unit", m_unit );
    paper.setAttribute("tabStopValue", m_tabStop );

    QDomElement paperBorders=doc.createElement("PAPERBORDERS");

    paperBorders.setAttribute("ptLeft", m_pageLayout.ptLeft);
    paperBorders.setAttribute("ptTop", m_pageLayout.ptTop);
    paperBorders.setAttribute("ptRight", m_pageLayout.ptRight);
    paperBorders.setAttribute("ptBottom", m_pageLayout.ptBottom);
    paper.appendChild(paperBorders);
    presenter.appendChild(paper);

    m_varColl->variableSetting()->save(presenter );

    presenter.appendChild(saveAttribute( doc ));

    if ( saveOnlyPage == -1 )
        emit sigProgress( 5 );

    QDomElement element=doc.createElement("BACKGROUND");
    element.appendChild(saveBackground( doc ));
    presenter.appendChild(element);

    if ( saveOnlyPage == -1 )
        emit sigProgress( 10 );

    //TODO save correct page info for header/footer
    element=doc.createElement("HEADER");
    element.setAttribute("show", static_cast<int>( m_pageList.at(0)->hasHeader() ));
    element.appendChild(_header->save( doc,0 ));
    presenter.appendChild(element);

    element=doc.createElement("FOOTER");
    element.setAttribute("show", static_cast<int>( m_pageList.at(0)->hasFooter() ));
    element.appendChild(_footer->save( doc,0 ));
    presenter.appendChild(element);

    element = doc.createElement( "HEADERFOOTERBYPAGE" );
    element.setAttribute( "value", "true" );
    presenter.appendChild( element );

    element=doc.createElement("HELPLINES");
    element.setAttribute("show", static_cast<int>( showHelplines() ));
    saveHelpLines( doc, element );
    presenter.appendChild(element);

    if ( saveOnlyPage == -1 )
    {
        if( !m_spellCheckIgnoreList.isEmpty() )
        {
            QDomElement spellCheckIgnore = doc.createElement( "SPELLCHECKIGNORELIST" );
            presenter.appendChild( spellCheckIgnore );
            for ( QStringList::Iterator it = m_spellCheckIgnoreList.begin(); it != m_spellCheckIgnoreList.end(); ++it )
            {
                QDomElement spellElem = doc.createElement( "SPELLCHECKIGNOREWORD" );
                spellCheckIgnore.appendChild( spellElem );
                spellElem.setAttribute( "word", *it );
            }
        }
    }

    if ( saveOnlyPage == -1 )
        emit sigProgress( 20 );

    presenter.appendChild(saveTitle( doc ));

    presenter.appendChild(saveNote( doc ));

    if ( saveOnlyPage == -1 )
        emit sigProgress( 30 );

    presenter.appendChild(saveObjects(doc));

    // ### If we will create a new version of the file format, fix that spelling error
    element=doc.createElement("INFINITLOOP");
    element.setAttribute("value", _spInfiniteLoop);
    presenter.appendChild(element);
    element=doc.createElement("MANUALSWITCH");
    element.setAttribute("value", _spManualSwitch);
    presenter.appendChild(element);
    element=doc.createElement("PRESSPEED");
//TODO FIXME !!!!!!!!!!
//element.setAttribute("value", static_cast<int>( presSpeed ));
    presenter.appendChild(element);
    element=doc.createElement("SHOWPRESENTATIONDURATION");
    element.setAttribute("value", _showPresentationDuration);
    presenter.appendChild(element);

    if ( saveOnlyPage == -1 )
    {
        if ( !m_customListSlideShow.isEmpty() )
        {
            element = doc.createElement( "CUSTOMSLIDESHOWCONFIG" );
            ListCustomSlideShow::Iterator it;
            for ( it = m_customListSlideShow.begin(); it != m_customListSlideShow.end(); ++it )
            {
                QDomElement slide=doc.createElement("CUSTOMSLIDESHOW");
                slide.setAttribute("name", it.key() );
                QString tmp;
                QValueListIterator<KPrPage*> itPage ;
                for( itPage = ( *it ).begin(); itPage != ( *it ).end(); ++itPage )
                {
                    int posPage = m_pageList.find(*itPage );
                    if ( posPage != -1 )
                        tmp+=( *itPage )->oasisNamePage(posPage+1)+",";
                }
                slide.setAttribute( "pages", tmp );
                element.appendChild(slide);
            }
            presenter.appendChild(element);

        }

        if ( !m_presentationName.isEmpty() )
        {
            element = doc.createElement( "DEFAULTCUSTOMSLIDESHOWNAME" );
            element.setAttribute( "name", m_presentationName );
            presenter.appendChild(element);
        }
    }

    if ( saveOnlyPage == -1 )
        emit sigProgress( 40 );

    if ( saveOnlyPage == -1 )
    {
        element=doc.createElement("SELSLIDES");
        for ( uint i = 0; i < m_pageList.count(); i++ ) {
            QDomElement slide=doc.createElement("SLIDE");
            slide.setAttribute("nr", i);
            slide.setAttribute("show", m_pageList.at(i)->isSlideSelected());
            element.appendChild(slide);
        }
        presenter.appendChild(element);

        emit sigProgress( 50 );
    }

    if ( saveOnlyPage == -1 )
    {
        QDomElement styles = doc.createElement( "STYLES" );
        presenter.appendChild( styles );
        QPtrList<KoParagStyle> styleList(m_styleColl->styleList());
        for ( KoParagStyle * p = styleList.first(); p != 0L; p = styleList.next() )
            saveStyle( p, styles );

        emit sigProgress( 60 );
    }

    // Write "OBJECT" tag for every child
    QPtrListIterator<KoDocumentChild> chl( children() );
    for( ; chl.current(); ++chl ) {
        // Don't save children that are only in the undo/redo history
        // but not anymore in the presentation
        for ( int i = 0; i < static_cast<int>( m_pageList.count() ); i++ ) {
            if ( saveOnlyPage != -1 && i != saveOnlyPage )
                continue;
            double offset=0;
            if ( saveOnlyPage == -1 )
            {
                offset = i * m_pageList.at(i)->getPageRect().height();
            }
            saveEmbeddedObject(m_pageList.at(i), chl.current(),doc,presenter,offset );
        }
        if ( saveOnlyPage == -1 )
        {
            saveEmbeddedObject(m_masterPage, chl.current(),doc,presenter,0.0 );
        }
    }

    if ( saveOnlyPage == -1 )
        emit sigProgress( 70 );
    makeUsedPixmapList();

    if (specialOutputFlag()==SaveAsKOffice1dot1)
        m_pictureCollection.saveXMLAsKOffice1Dot1( doc, presenter, usedPictures );
    else
    {
        QDomElement pictures = m_pictureCollection.saveXML( KoPictureCollection::CollectionPicture, doc, usedPictures );
        presenter.appendChild( pictures );
    }

    if ( saveOnlyPage == -1 )
        emit sigProgress( 90 );

    // Save sound file list.
    makeUsedSoundFileList();
    QDomElement soundFiles = saveUsedSoundFileToXML( doc, usedSoundFile );
    presenter.appendChild( soundFiles );

    if ( saveOnlyPage == -1 )
        setModified( false );
    return doc;
}

void KPresenterDoc::saveEmbeddedObject(KPrPage *page, const QPtrList<KoDocumentChild>& childList,
                                       QDomDocument &doc,QDomElement &presenter )
{
    QPtrListIterator<KoDocumentChild> chl( childList );
    double offset = 0.0;
    // we need no offset for objects on the master page and when we copy a page
    if ( m_pageList.findRef( page ) )
    {
        offset=m_pageList.findRef(page)*page->getPageRect().height();
    }
    for( ; chl.current(); ++chl )
        saveEmbeddedObject(page, chl.current(),doc,presenter, offset );
}

void KPresenterDoc::saveEmbeddedObject(KPrPage *page, KoDocumentChild *chl, QDomDocument &doc,
                                       QDomElement &presenter, double offset )
{
    QPtrListIterator<KPObject> oIt(page->objectList());
    for ( int pos = 0; oIt.current(); ++oIt, ++pos )
    {
        if ( oIt.current()->getType() == OT_PART &&
             static_cast<KPPartObject*>( oIt.current() )->getChild() == chl )
        {
            QDomElement embedded=doc.createElement("EMBEDDED");
            KPresenterChild* curr = (KPresenterChild*)chl;

            // geometry is no zoom value !
            QRect _rect = curr->geometry();
            int tmpX = (int)zoomHandler()->unzoomItX( _rect.x() );
            int tmpY = (int)zoomHandler()->unzoomItY( _rect.y() );
            int tmpWidth = (int)zoomHandler()->unzoomItX( _rect.width() );
            int tmpHeight = (int)zoomHandler()->unzoomItY( _rect.height() );
            curr->setGeometry( QRect( tmpX, tmpY, tmpWidth, tmpHeight ) );

            embedded.appendChild(curr->save(doc, true));

            curr->setGeometry( _rect ); // replace zoom value

            QDomElement settings=doc.createElement("SETTINGS");
            settings.setAttribute( "z-index", pos );
            if ( page == m_masterPage )
                settings.setAttribute("sticky", 1 );
            QPtrListIterator<KPObject> setOIt(page->objectList());
            for (; setOIt.current(); ++setOIt )
            {
                if ( setOIt.current()->getType() == OT_PART &&
                     dynamic_cast<KPPartObject*>( setOIt.current() )->getChild() == curr )
                    settings.appendChild(setOIt.current()->save( doc,offset ));
            }
            embedded.appendChild(settings);
            presenter.appendChild(embedded);
        }
    }

}

//TODO with changes with new file format header/footer can't be change
void KPresenterDoc::compatibilityFromOldFileFormat()
{
    //function to keep compatibility with old file format
    //for example for presSpeed
    if ( m_loadingInfo && m_loadingInfo->oldFormat() )
    {
        EffectSpeed newValue = ES_MEDIUM;
        bool presSpeedChanged = ( m_loadingInfo->presSpeed != -1 );
        if ( presSpeedChanged )
        {
            if ( m_loadingInfo->presSpeed < 3 )
                newValue = ES_SLOW;
            else if ( m_loadingInfo->presSpeed > 7 )
                newValue = ES_FAST;
        }
        if ( !m_loadingInfo->m_headerFooterByPage )
        {
            for ( int i = 0; i < static_cast<int>( m_pageList.count() ); i++ )
            {
                if ( presSpeedChanged )
                    m_pageList.at(i)->setPageEffectSpeed( newValue );
                m_pageList.at( i )->setHeader( m_loadingInfo->m_header );
                m_pageList.at( i )->setFooter( m_loadingInfo->m_footer );
            }
        }

    }
    delete m_loadingInfo;
    m_loadingInfo = 0L;

}

void KPresenterDoc::enableEmbeddedParts( bool f )
{
    QPtrListIterator<KPrPage> it( m_pageList );
    for ( ; it.current(); ++it )
        it.current()->enableEmbeddedParts(f);
}

QDomDocumentFragment KPresenterDoc::saveBackground( QDomDocument &doc )
{
    KPBackGround *kpbackground = 0;
    QDomDocumentFragment fragment=doc.createDocumentFragment();
    for ( int i = 0; i < static_cast<int>( m_pageList.count() ); i++ ) {
        if ( saveOnlyPage != -1 && i != saveOnlyPage )
            continue;
        fragment.appendChild( m_pageList.at(i)->save( doc, (specialOutputFlag()==SaveAsKOffice1dot1) ));
    }
    // save backgound of masterpage
    fragment.appendChild( m_masterPage->save( doc, (specialOutputFlag()==SaveAsKOffice1dot1) ) );
    return fragment;
}

QDomElement KPresenterDoc::saveObjects( QDomDocument &doc )
{
    QDomElement objects=doc.createElement("OBJECTS");
    double yoffset=0.0;
    for ( int i = 0; i < static_cast<int>( m_pageList.count() ); i++ ) {
        if ( saveOnlyPage != -1 && saveOnlyPage!=i)
            continue;
        yoffset=i*m_pageList.at(i)->getPageRect().height(); // yoffset is not zoom value !!
        objects=m_pageList.at(i)->saveObjects( doc, objects, yoffset, m_zoomHandler, saveOnlyPage );

    }
    if ( !_duplicatePage ) //don't copy sticky objects when we duplicate page
    {
        //offset = 0.0 when it's a sticky page.
        objects=m_masterPage->saveObjects( doc, objects, /*yoffset*/0.0, m_zoomHandler, saveOnlyPage );
    }

    return objects;
}

QDomElement KPresenterDoc::saveTitle( QDomDocument &doc )
{
    QDomElement titles=doc.createElement("PAGETITLES");

    if ( saveOnlyPage == -1 )
    { // All page titles.
        for ( int i = 0; i < static_cast<int>( m_pageList.count() ); i++ )
        {
            QDomElement title=doc.createElement("Title");
            title.setAttribute("title", m_pageList.at(i)->manualTitle());
            titles.appendChild(title);
        }
    }
    else
    { // Only current page title.
        QDomElement title=doc.createElement("Title");
        title.setAttribute("title", m_pageList.at(saveOnlyPage)->manualTitle());
        titles.appendChild(title);
    }
    return titles;
}

QDomElement KPresenterDoc::saveNote( QDomDocument &doc )
{
    QDomElement notes=doc.createElement("PAGENOTES");

    if ( saveOnlyPage == -1 ) { // All page notes.
        for ( int i = 0; i < static_cast<int>( m_pageList.count() ); i++ )
        {
            QDomElement note=doc.createElement("Note");
            note.setAttribute("note", m_pageList.at(i)->noteText( ));
            notes.appendChild(note);
        }
    }
    else { // Only current page note.
        QDomElement note=doc.createElement("Note");
        note.setAttribute("note", m_pageList.at(saveOnlyPage)->noteText( ));
        notes.appendChild(note);
    }

    return notes;
}

QDomElement KPresenterDoc::saveAttribute( QDomDocument &doc )
{
    QDomElement attributes=doc.createElement("ATTRIBUTES");
    //store first view parameter.
    int activePage=0;

    if ( m_initialActivePage )
        activePage=m_pageList.findRef(m_initialActivePage);
    activePage = QMAX( activePage, 0);
    attributes.setAttribute("activePage",activePage );
    attributes.setAttribute("gridx", m_gridX );
    attributes.setAttribute("gridy", m_gridY );
    attributes.setAttribute("snaptogrid", (int)m_bSnapToGrid );
    return attributes;
}

QDomElement KPresenterDoc::saveUsedSoundFileToXML( QDomDocument &_doc, QStringList _list )
{
    QDomElement soundFiles = _doc.createElement( "SOUNDS" );

    unsigned int i = 0;
    QStringList::Iterator it = _list.begin();
    for ( ; it != _list.end(); ++it ) {
        QString soundFileName = *it;
        int position = soundFileName.findRev( '.' );
        QString format = soundFileName.right( soundFileName.length() - position - 1 );
        QString _name = QString( "sounds/sound%1.%2" ).arg( ++i ).arg( format.lower() );

        QDomElement fileElem = _doc.createElement( "FILE" );
        soundFiles.appendChild( fileElem );
        fileElem.setAttribute( "filename", soundFileName );
        fileElem.setAttribute( "name", _name );
    }

    return soundFiles;
}

bool KPresenterDoc::completeSaving( KoStore* _store )
{
    if ( !_store ) {
        if ( saveOnlyPage == -1 ) {
            emit sigProgress( 100 );
            emit sigProgress( -1 );
        }
        return true;
    }

    if (specialOutputFlag()==SaveAsKOffice1dot1)
        m_pictureCollection.saveToStoreAsKOffice1Dot1( KoPictureCollection::CollectionImage, _store, usedPictures );
    else
        m_pictureCollection.saveToStore( KoPictureCollection::CollectionPicture, _store, usedPictures );

    saveUsedSoundFileToStore( _store, usedSoundFile );

    if ( saveOnlyPage == -1 ) {
        emit sigProgress( 100 );
        emit sigProgress( -1 );
    }

    return true;
}

int KPresenterDoc::supportedSpecialFormats() const
{
    return SaveAsKOffice1dot1 | KoDocument::supportedSpecialFormats();
}

void KPresenterDoc::saveUsedSoundFileToStore( KoStore *_store, QStringList _list )
{
    unsigned int i = 0;
    QStringList::Iterator it = _list.begin();
    for ( ; it != _list.end(); ++it ) {
        QString soundFileName = *it;
        int position = soundFileName.findRev( '.' );
        QString format = soundFileName.right( soundFileName.length() - position - 1 );
        QString _storeURL = QString( "sounds/sound%1.%2" ).arg( ++i ).arg( format.lower() );

        if ( _store->open( _storeURL ) ) {
            KoStoreDevice dev( _store );
            QFile _file( soundFileName );
            if ( _file.open( IO_ReadOnly ) ) {
                dev.writeBlock( ( _file.readAll() ).data(), _file.size() );
                _file.close();
            }
            _store->close();
        }
    }
}

bool KPresenterDoc::loadChildren( KoStore* _store )
{
    if ( objStartY == 0 && _clean) // Don't do this when inserting a template or a page...
    {
        QPtrListIterator<KoDocumentChild> it( children() );
        for( ; it.current(); ++it ) {
            if ( !((KoDocumentChild*)it.current())->loadDocument( _store ) )
                return false;
        }
    }
    else // instead load form the correct child on, m_childCountBeforeInsert has the be set
    {
        QPtrListIterator<KoDocumentChild> it( children() );
        for( int i = 0; it.current(); ++it, ++i ) {
            if ( i < m_childCountBeforeInsert )
                continue;
            if ( !((KoDocumentChild*)it.current())->loadDocument( _store ) )
                return false;
        }
    }
    return true;
}

bool KPresenterDoc::saveOasis( KoStore* store, KoXmlWriter* manifestWriter )
{

    //todo necessary for new format ?
    if ( saveOnlyPage == -1 ) {
        emit sigProgress( 0 );
    }
    if ( !store->open( "content.xml" ) )
        return false;
    m_pictureCollection.assignUniqueIds();
    KoStoreDevice contentDev( store );
    KoXmlWriter* contentWriter = createOasisXmlWriter( &contentDev, "office:document-content" );


    m_varColl->variableSetting()->setModificationDate(QDateTime::currentDateTime());
    recalcVariables( VT_DATE );
    recalcVariables( VT_TIME );

    KoGenStyles mainStyles;
    KoSavingContext savingContext( mainStyles, KoSavingContext::Store );

    // Save user styles as KoGenStyle objects
    KoSavingContext::StyleNameMap map = m_styleColl->saveOasis( mainStyles, KoGenStyle::STYLE_USER );
    savingContext.setStyleNameMap( map );

    KTempFile contentTmpFile;
    contentTmpFile.setAutoDelete( true );
    QFile* tmpFile = contentTmpFile.file();
    KoXmlWriter contentTmpWriter( tmpFile, 1 );


    //For sticky objects
    KTempFile stickyTmpFile;
    stickyTmpFile.setAutoDelete( true );
    QFile* tmpStickyFile = stickyTmpFile.file();
    KoXmlWriter stickyTmpWriter( tmpStickyFile, 1 );


    contentTmpWriter.startElement( "office:body" );
    contentTmpWriter.startElement( "office:presentation" );

    int indexObj = 1;
    int partIndexObj = 0;
//save page

    if ( saveOnlyPage != -1 )
    {
        m_pageList.at( saveOnlyPage )->saveOasisPage( store, contentTmpWriter, ( saveOnlyPage+1 ), savingContext, indexObj, partIndexObj , manifestWriter);
    }
    else
    {
        for ( int i = 0; i < static_cast<int>( m_pageList.count() ); i++ )
        {
            m_pageList.at( i )->saveOasisPage( store, contentTmpWriter, ( i+1 ), savingContext, indexObj, partIndexObj , manifestWriter);
        }
    }
    QString styleMasterNamePage;
    if ( !_duplicatePage )
    {
        m_masterPage->saveOasisStickyPage( store, stickyTmpWriter , savingContext, indexObj,partIndexObj, manifestWriter );
        styleMasterNamePage = m_masterPage->saveOasisPageStyle( store, savingContext.mainStyles() );

    }
    if ( saveOnlyPage == -1 ) //don't save setting when we save on page
    {
        saveOasisHeaderFooter( stickyTmpWriter , savingContext );
        saveOasisPresentationSettings( contentTmpWriter );
    }

    contentTmpWriter.endElement(); //office:presentation
    contentTmpWriter.endElement(); //office:body

    writeAutomaticStyles( *contentWriter, mainStyles );

    // And now we can copy over the contents from the tempfile to the real one
    tmpFile->close();
    contentWriter->addCompleteElement( tmpFile );
    contentTmpFile.close();

    contentWriter->endElement(); // root element
    contentWriter->endDocument();
    delete contentWriter;

    if ( !store->close() ) // done with content.xml
        return false;


    KoGenStyle pageLayout = m_pageLayout.saveOasis();
    pageLayout.addAttribute( "style:page-usage", "all" ); // needed?
    mainStyles.lookup( pageLayout, "pm" );


    //add manifest line for content.xml
    manifestWriter->addManifestEntry( "content.xml", "text/xml" );

    if ( !store->open( "styles.xml" ) )
        return false;

    manifestWriter->addManifestEntry( "styles.xml", "text/xml" );

    //todo fixme????
    tmpStickyFile->close();
    saveOasisDocumentStyles( store, mainStyles, tmpStickyFile, styleMasterNamePage);
    stickyTmpFile.close();

    if ( !store->close() ) // done with styles.xml
        return false;


    if ( saveOnlyPage == -1 )
        emit sigProgress( 90 );

    // Save sound file list.
//todo ????


    makeUsedPixmapList();

    m_pictureCollection.saveOasisToStore( store, usedPictures, manifestWriter);

    if(!store->open("settings.xml"))
        return false;

    KoXmlWriter& settingsWriter = *createOasisXmlWriter(&contentDev, "office:document-settings");
    settingsWriter.startElement("office:settings");

    settingsWriter.startElement("config:config-item-set");
    settingsWriter.addAttribute("config:name", "view-settings");

    KoUnit::saveOasis(&settingsWriter, m_unit);
    saveOasisSettings( settingsWriter );

    settingsWriter.endElement(); // config:config-item-set

    settingsWriter.startElement("config:config-item-set");
    settingsWriter.addAttribute("config:name", "configuration-settings");
    settingsWriter.addConfigItem("SpellCheckerIgnoreList", m_spellCheckIgnoreList.join( "," ) );
    settingsWriter.endElement(); // config:config-item-set

    m_varColl->variableSetting()->saveOasis( settingsWriter );

    settingsWriter.endElement(); // office:settings
    settingsWriter.endElement(); // Root:Element
    settingsWriter.endDocument();
    delete &settingsWriter;

    if(!store->close())
        return false;

    manifestWriter->addManifestEntry("settings.xml", "text/xml");

    //reset progressbar
    emit sigProgress( 100 );
    emit sigProgress( -1 );

    setModified( false );

    return true;
}

void KPresenterDoc::loadOasisIgnoreList( const KoOasisSettings& settings )
{
    KoOasisSettings::Items configurationSettings = settings.itemSet( "configuration-settings" );
    if ( !configurationSettings.isNull() )
    {
        const QString ignorelist = configurationSettings.parseConfigItemString( "SpellCheckerIgnoreList" );
        m_spellCheckIgnoreList = QStringList::split( ',', ignorelist );
    }
}

void KPresenterDoc::writeAutomaticStyles( KoXmlWriter& contentWriter, KoGenStyles& mainStyles )
{
    contentWriter.startElement( "office:automatic-styles" );
    QValueList<KoGenStyles::NamedStyle> styles = mainStyles.styles( KoGenStyle::STYLE_AUTO );
    QValueList<KoGenStyles::NamedStyle>::const_iterator it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( &contentWriter, mainStyles, "style:style", (*it).name, "style:paragraph-properties" );
    }

    styles = mainStyles.styles( KoGenStyle::STYLE_AUTO_LIST );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        ( *it ).style->writeStyle( &contentWriter, mainStyles, "text:list-style", (*it).name, 0 );
    }

    styles = mainStyles.styles( STYLE_BACKGROUNDPAGEAUTO );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( &contentWriter, mainStyles, "style:style", (*it).name, "style:drawing-page-properties" );
    }

    styles = mainStyles.styles( KoGenStyle::STYLE_GRAPHICAUTO );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( &contentWriter, mainStyles, "style:style", (*it).name , "style:graphic-properties"  );
    }

    styles = mainStyles.styles( KoGenStyle::STYLE_NUMERIC_DATE );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( &contentWriter, mainStyles, "number:date-style", (*it).name, 0 /*TODO ????*/  );
    }
    styles = mainStyles.styles( KoGenStyle::STYLE_NUMERIC_TIME );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( &contentWriter, mainStyles, "number:time-style", (*it).name, 0 /*TODO ????*/  );
    }

    contentWriter.endElement(); // office:automatic-styles
}

void KPresenterDoc::saveOasisHeaderFooter( KoXmlWriter & stickyTmpWriter , KoSavingContext& context )
{
    stickyTmpWriter.startElement( "style:header" );
    header()->textObject()->saveOasisContent( stickyTmpWriter, context );
    stickyTmpWriter.endElement();

    stickyTmpWriter.startElement( "style:footer" );
    footer()->textObject()->saveOasisContent( stickyTmpWriter, context );
    stickyTmpWriter.endElement();
}

void KPresenterDoc::loadOasisHeaderFooter(QDomNode & drawPage, KoOasisContext & context)
{
    QDomNode tmp = KoDom::namedItemNS( drawPage, KoXmlNS::style, "header" );
    if ( !tmp.isNull() )
    {
        //kdDebug()<<" there is a header \n";
        _header->textObject()->loadOasisContent( tmp.toElement(), context, styleCollection() );
    }
    tmp = KoDom::namedItemNS( drawPage, KoXmlNS::style, "footer" );
    if ( !tmp.isNull() )
    {
        //kdDebug()<<" there is a footer \n";
        _footer->textObject()->loadOasisContent( tmp.toElement(), context, styleCollection() );
    }
}

void KPresenterDoc::saveOasisSettings( KoXmlWriter &settingsWriter )
{
    settingsWriter.startElement("config:config-item-map-indexed");
    settingsWriter.addAttribute("config:name", "Views");
    settingsWriter.startElement( "config:config-item-map-entry" );

    //ooimpress save it as this line.
    //<config:config-item config:name="SnapLinesDrawing" config:type="string">H2260V14397H7693H12415H15345H1424</config:config-item>
    QString helpLineOasis;
    //save in mm as in oo
    for(QValueList<double>::Iterator it = m_vertHelplines.begin(); it != m_vertHelplines.end(); ++it)
    {
        int tmpX = ( int ) ( KoUnit::toMM( *it  )*100 );
        helpLineOasis+="V"+QString::number( tmpX );
    }

    for(QValueList<double>::Iterator it = m_horizHelplines.begin(); it != m_horizHelplines.end(); ++it)
    {
        int tmpY = ( int ) ( KoUnit::toMM( *it  )*100 );
        helpLineOasis+="H"+QString::number( tmpY );
    }
    for(QValueList<KoPoint>::Iterator it = m_helpPoints.begin(); it != m_helpPoints.end(); ++it)
    {
        QString str( "P%1,%2" );
        int tmpX = ( int ) ( KoUnit::toMM( ( *it ).x()  )*100 );
        int tmpY = ( int ) ( KoUnit::toMM( ( *it ).y()  )*100 );
        helpLineOasis+=str.arg( QString::number( tmpX ) ).arg( QString::number( tmpY ) );
    }
    if ( !helpLineOasis.isEmpty() )
    {
        settingsWriter.addConfigItem("SnapLinesDrawing", helpLineOasis );
    }
    //<config:config-item config:name="IsSnapToGrid" config:type="boolean">false</config:config-item>
    settingsWriter.addConfigItem( "IsSnapToGrid", m_bSnapToGrid );

    //<config:config-item config:name="GridFineWidth" config:type="int">500</config:config-item>
    settingsWriter.addConfigItem( "GridFineWidth", ( ( int ) ( KoUnit::toMM( ( m_gridX )  )*100 ) ) );


    //<config:config-item config:name="GridFineHeight" config:type="int">500</config:config-item>
    settingsWriter.addConfigItem( "GridFineHeight", ( ( int ) ( KoUnit::toMM( ( m_gridY )  )*100 ) ) );

    //<config:config-item config:name="SelectedPage" config:type="short">3</config:config-item>
    //store first view parameter.
    int activePage=0;
    if ( m_initialActivePage )
        activePage=m_pageList.findRef(m_initialActivePage);
    activePage = QMAX( activePage, 0);
    settingsWriter.addConfigItem( "SelectedPage", activePage );

    //not define into oo spec
    settingsWriter.addConfigItem( "SnapLineIsVisible", showHelplines() );
    settingsWriter.endElement();
    settingsWriter.endElement();
}

void KPresenterDoc::loadOasisSettings(const QDomDocument&settingsDoc)
{
    kdDebug()<<"void KPresenterDoc::loadOasisSettings(const QDomDocument&settingsDoc)**********\n";
    KoOasisSettings settings( settingsDoc );
    KoOasisSettings::Items viewSettings = settings.itemSet( "view-settings" );
    setUnit(KoUnit::unit(viewSettings.parseConfigItemString("unit")));
    KoOasisSettings::IndexedMap viewMap = viewSettings.indexedMap( "Views" );
    KoOasisSettings::Items firstView = viewMap.entry( 0 );
    if ( !firstView.isNull() )
    {
        parseOasisHelpLine( firstView.parseConfigItemString( "SnapLinesDrawing" ) );
        setShowHelplines( firstView.parseConfigItemBool( "SnapLineIsVisible" ) );
        int valx = firstView.parseConfigItemInt( "GridFineWidth" );
        m_gridX = MM_TO_POINT( valx / 100.0 );
        int valy = firstView.parseConfigItemInt( "GridFineHeight" );
        m_gridY = MM_TO_POINT( valy / 100.0 );

        m_bSnapToGrid = firstView.parseConfigItemBool( "IsSnapToGrid" );

        int activePage = firstView.parseConfigItemInt( "SelectedPage" );
        kdDebug()<<" activePage :"<<activePage<<endl;
        if(activePage!=-1)
            m_initialActivePage=m_pageList.at(activePage);

    }
    loadOasisIgnoreList( settings );
    m_varColl->variableSetting()->loadOasis( settings );
}

void KPresenterDoc::parseOasisHelpLine( const QString &text )
{
    QString str;
    int newPos = text.length()-1; //start to element = 1
    for ( int pos = text.length()-1; pos >=0;--pos )
    {
        if ( text[pos]=='P' )
        {
            //point
            str = text.mid( pos+1, ( newPos-pos ) );
            //kdDebug()<<" point element  :"<< str <<endl;
            QStringList listVal = QStringList::split( ",", str );
            int posX = ( listVal[0].toInt()/100 );
            int posY = ( listVal[1].toInt()/100 );
            m_helpPoints.append( KoPoint( MM_TO_POINT( posX ), MM_TO_POINT( posY )));
            newPos = pos-1;
        }
        else if ( text[pos]=='V' )
        {
            //vertical element
            str = text.mid( pos+1, ( newPos-pos ) );
            //kdDebug()<<" vertical  :"<< str <<endl;
            int posX = ( str.toInt()/100 );
            m_vertHelplines.append( MM_TO_POINT( posX ) );
            newPos = pos-1;
        }
        else if ( text[pos]=='H' )
        {
            //horizontal element
            str = text.mid( pos+1, ( newPos-pos ) );
            //kdDebug()<<" horizontal  :"<< str <<endl;
            int posY = ( str.toInt()/100 );
            m_horizHelplines.append( MM_TO_POINT( posY ) );
            newPos = pos-1;
        }
    }
}

void KPresenterDoc::loadOasisPresentationSettings( QDomNode &settingsDoc )
{
    //kdDebug()<<"presentation:settings ********************************************* \n";
    QDomElement settings( settingsDoc.toElement() );
    //kdDebug()<<"settings.attribute(presentation:endless) :"<<settings.attributeNS( KoXmlNS::presentation, "endless", QString::null)<<endl;
    if (settings.attributeNS( KoXmlNS::presentation, "endless", QString::null)=="true")
        _spInfiniteLoop = true;

    if (settings.attributeNS( KoXmlNS::presentation, "force-manual", QString::null)=="true")
        _spManualSwitch = true;
    if ( settings.hasAttributeNS( KoXmlNS::presentation, "start-page" ) )
    {
        //TODO allow to start presentation to specific page
        //???? = settings.attributeNS( KoXmlNS::presentation, "start-page", QString::null );
    }
    if ( settings.hasAttributeNS( KoXmlNS::presentation, "show" ) )
    {
        m_presentationName = settings.attributeNS( KoXmlNS::presentation, "show", QString::null );
        kdDebug()<<" default presentation name :"<<m_presentationName<<endl;
    }
    loadOasisPresentationCustomSlideShow( settingsDoc );
}

void KPresenterDoc::loadOasisPresentationCustomSlideShow( QDomNode &settingsDoc )
{
    //kdDebug()<<"void KPresenterDoc::loadOasisPresentationCustomSlideShow( QDomNode &settingsDoc )**********\n";
    for ( QDomNode element = settingsDoc.firstChild(); !element.isNull(); element = element.nextSibling() )
    {
        QDomElement e = element.toElement();
	QCString tagName = e.tagName().latin1();
        //kdDebug()<<" tagName :"<<tagName<<endl;
        if ( tagName == "show" && e.namespaceURI() == KoXmlNS::presentation)
        {
            //kdDebug()<<" e.attribute(presentation:name) :"<<e.attributeNS( KoXmlNS::presentation, "name", QString::null)<< " e.attribute(presentation:pages) :"<<e.attributeNS( KoXmlNS::presentation, "pages", QString::null)<<endl;
            QStringList tmp = QStringList::split( ",", e.attributeNS( KoXmlNS::presentation, "pages", QString::null) );
            m_loadingInfo->m_tmpCustomListMap.insert( e.attributeNS( KoXmlNS::presentation, "name", QString::null), tmp );
        }
    }
}

void KPresenterDoc::saveOasisPresentationSettings( KoXmlWriter &contentTmpWriter )
{
    //todo don't save when is not value by default (check with oo)
    //FIXME
    contentTmpWriter.startElement( "presentation:settings" );
    contentTmpWriter.addAttribute( "presentation:endless",  ( _spInfiniteLoop ? "true" : "false" ) );
    contentTmpWriter.addAttribute( "presentation:force-manual",  ( _spManualSwitch ? "true" : "false" ) );
    //add for default presentation
    if ( !m_presentationName.isEmpty() )
        contentTmpWriter.addAttribute( "presentation:show",  m_presentationName );

    saveOasisPresentationCustomSlideShow( contentTmpWriter );
    contentTmpWriter.endElement();
}

void KPresenterDoc::saveOasisPresentationCustomSlideShow( KoXmlWriter &contentTmpWriter )
{
    if ( m_customListSlideShow.isEmpty() )
        return;

    ListCustomSlideShow::Iterator it;
    for ( it = m_customListSlideShow.begin(); it != m_customListSlideShow.end(); ++it )
    {
        contentTmpWriter.startElement( "presentation:show" );
        contentTmpWriter.addAttribute( "presentation:name", it.key() );
        QString tmp;
        QValueListIterator<KPrPage*> itPage ;
        for( itPage = ( *it ).begin(); itPage != ( *it ).end(); ++itPage )
        {
            int posPage = m_pageList.find(*itPage );
            if ( posPage != -1 )
                tmp+=( *itPage )->oasisNamePage(posPage+1)+",";
        }
        contentTmpWriter.addAttribute( "presentation:pages", tmp );
        contentTmpWriter.endElement();
    }
    //<presentation:show presentation:name="New Custom Slide Show" presentation:pages="page1,page1,page1,page1,page1"/>
}

void KPresenterDoc::saveOasisDocumentStyles( KoStore* store, KoGenStyles& mainStyles, QFile* tmpStyckyFile, const QString &_styleMasterPageName ) const
{
    QString pageLayoutName;
    KoStoreDevice stylesDev( store );
    KoXmlWriter* stylesWriter = createOasisXmlWriter( &stylesDev, "office:document-styles" );

    stylesWriter->startElement( "office:styles" );
    QValueList<KoGenStyles::NamedStyle> styles = mainStyles.styles( KoGenStyle::STYLE_USER );
    QValueList<KoGenStyles::NamedStyle>::const_iterator it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( stylesWriter, mainStyles, "style:style", (*it).name, "style:paragraph-properties" );
    }
    styles = mainStyles.styles( KoGenStyle::STYLE_LIST );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( stylesWriter, mainStyles, "text:list-style", (*it).name, 0 );
    }
    styles = mainStyles.styles( KoGenStyle::STYLE_HATCH );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( stylesWriter, mainStyles, "draw:hatch", (*it).name, "style:graphic-properties" ,  true,  true /*add draw:name*/);
    }
    styles = mainStyles.styles( STYLE_GRADIENT );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( stylesWriter, mainStyles, "draw:gradient", (*it).name, "style:graphic-properties" ,  true,  true /*add draw:name*/);
    }

    styles = mainStyles.styles( STYLE_STROKE );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( stylesWriter, mainStyles, "draw:stroke-dash", (*it).name, "style:graphic-properties" ,  true,  true /*add draw:name*/);
    }

    styles = mainStyles.styles( STYLE_MARKER );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( stylesWriter, mainStyles, "draw:marker", (*it).name, "style:graphic-properties" ,  true,  true /*add draw:name*/);
    }
    styles = mainStyles.styles( STYLE_PICTURE );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( stylesWriter, mainStyles, "draw:fill-image", (*it).name, "style:image-properties" ,  true,  true /*add draw:name*/);
    }

    stylesWriter->endElement(); // office:styles

    stylesWriter->startElement( "office:automatic-styles" );
    styles = mainStyles.styles( STYLE_BACKGROUNDPAGE );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( stylesWriter, mainStyles, "style:style", (*it).name , "style:drawing-page-properties"  );
    }

    styles = mainStyles.styles( KoGenStyle::STYLE_PAGELAYOUT );
    Q_ASSERT( styles.count() == 1 );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( stylesWriter, mainStyles, "style:page-layout", (*it).name, "style:page-layout-properties", false /*don't close*/ );
        stylesWriter->endElement();
        Q_ASSERT( pageLayoutName.isEmpty() ); // if there's more than one pagemaster we need to rethink all this
        pageLayoutName = (*it).name;
    }

    styles = mainStyles.styles( STYLE_PRESENTATIONSTICKYOBJECT );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        //TODO fix me graphic-properties ???
        (*it).style->writeStyle( stylesWriter, mainStyles, "style:style", (*it).name , "style:graphic-properties"  );
    }

    stylesWriter->endElement(); // office:automatic-styles

    //code from kword
    stylesWriter->startElement( "office:master-styles" );
    stylesWriter->startElement( "style:master-page" );
    stylesWriter->addAttribute( "style:name", "Standard" );
    stylesWriter->addAttribute( "style:page-layout-name", pageLayoutName );
    kdDebug()<<" styleMasterNamePage :"<<_styleMasterPageName<<endl;
    if ( !_styleMasterPageName.isEmpty() )
        stylesWriter->addAttribute( "draw:style-name", _styleMasterPageName );


    //save sticky object
    stylesWriter->addCompleteElement( tmpStyckyFile );
    stylesWriter->endElement();
    stylesWriter->endElement(); // office:master-style


    stylesWriter->endElement(); // root element (office:document-styles)
    stylesWriter->endDocument();
    delete stylesWriter;
}

bool KPresenterDoc::loadOasis( const QDomDocument& doc, KoOasisStyles&oasisStyles, const QDomDocument&settingsDoc, KoStore*store )
{
    QTime dt;
    dt.start();
    m_loadingInfo = new KPRLoadingInfo;
    ignoreSticky = FALSE;
    emit sigProgress( 0 );
    int activePage=0;
    lastObj = -1;
    bool allSlides = false;
    // clean
    if ( _clean ) {
        __pgLayout = KoPageLayout::standardLayout();
        _spInfiniteLoop = false;
        _spManualSwitch = true;
        _showPresentationDuration = false;
        _xRnd = 20;
        _yRnd = 20;
        urlIntern = url().path();
    }
    else
        m_spellCheckIgnoreList.clear();
    emit sigProgress( 5 );

    QDomElement content = doc.documentElement();
    QDomElement body (KoDom::namedItemNS( content, KoXmlNS::office, "body" ) );
    if ( body.isNull() )
    {
        kdError(33001) << "No office:body found!" << endl;
        setErrorMessage( i18n( "Invalid document. No mimetype specified." ) );
        return false;
    }
    body = KoDom::namedItemNS( body, KoXmlNS::office, "presentation" );
    if ( body.isNull() )
    {
        kdError(33001) << "No office:presentation found!" << endl;
        setErrorMessage( i18n( "Invalid document. No mimetype specified." ) );
       return false;
    }
	//load settings
    QDomNode settings  = KoDom::namedItemNS( body, KoXmlNS::presentation, "settings" );
    kdDebug()<<"settings :"<<settings.isNull()<<endl;
    if (!settings.isNull() && _clean /*don't load settings when we copy/paste a page*/)
        loadOasisPresentationSettings( settings );

// it seems that ooimpress has different paper-settings for every slide.
    // we take the settings of the first slide for the whole document.
    QDomNode drawPage = KoDom::namedItemNS( body, KoXmlNS::draw, "page" );
    if ( drawPage.isNull() ) // no slides? give up.
        return false;
    QDomElement dp = drawPage.toElement();

    //code from kword
    // TODO variable settings
    // By default display real variable value
    if ( !isReadWrite())
        getVariableCollection()->variableSetting()->setDisplayFieldCode(false);

    KoOasisContext context( this, *m_varColl, oasisStyles, store );
    Q_ASSERT( !oasisStyles.officeStyle().isNull() );

    // Load all styles before the corresponding paragraphs try to use them!
    m_styleColl->loadOasisStyleTemplates( context );




    QString masterPageName = drawPage.toElement().attributeNS( KoXmlNS::draw, "master-page-name", QString::null );
    QDomElement *master = oasisStyles.masterPages()[ masterPageName];

    kdDebug()<<" master :"<<master<<endl;
    kdDebug()<<" masterPageName:"<<masterPageName<<endl;
    if ( ! master )
    {
        masterPageName = "Standard"; // use default layout as fallback (default in kpresenter)
        master = oasisStyles.masterPages()[ masterPageName];
        if ( !master ) //last test...
            master = oasisStyles.masterPages()["Default"];
    }
    kdDebug()<<" load sticky oasis object \n";
    kdDebug()<<" master.isNull() :"<<master->isNull()<<endl;
    QDomNode node = *master;
    kdDebug()<<" node.isNull() :"<<node.isNull()<<endl;
    loadOasisObject( m_masterPage, node , context);
    loadOasisHeaderFooter( node,context );

    kdDebug()<<" end load sticky oasis object \n";

    Q_ASSERT( master );
    QDomElement *style =master ? oasisStyles.styles()[master->attributeNS( KoXmlNS::style, "page-layout-name", QString::null )] : 0;
    QDomElement *backgroundStyle = oasisStyles.styles()[ "Standard-background"];
    kdDebug()<<"Standard background "<<backgroundStyle<<endl;
    // parse all pages
    Q_ASSERT( style );
    if ( style )
    {
        __pgLayout.loadOasis( *style );
        kdDebug()<<"Page size __pgLayout.ptWidth :"<<__pgLayout.ptWidth<<" __pgLayout.ptHeight :"<<__pgLayout.ptHeight<<endl;
        kdDebug()<<"Page orientation :"<<(( __pgLayout.orientation== PG_LANDSCAPE )? " landscape " : " portrait ")<<endl;

        kdDebug()<<" margin right:"<< __pgLayout.ptRight <<" __pgLayout.ptBottom :"<<__pgLayout.ptBottom<<" __pgLayout.ptLeft :"<<__pgLayout.ptLeft<<" __pgLayout.ptTop :"<<__pgLayout.ptTop<<endl;
    }
    if ( _clean )
    {
        /// ### this has already been done, no?
        setPageLayout( __pgLayout );
    }


    int pos = 0;
    for ( drawPage = body.firstChild(); !drawPage.isNull(); drawPage = drawPage.nextSibling() )
    {
        dp = drawPage.toElement();
        kdDebug()<<"dp.tagName() :"<<dp.tagName()<<endl;
        if ( dp.tagName()== "page" && dp.namespaceURI() == KoXmlNS::draw ) // don't try to parse "</draw:page>" as page
        {
            context.styleStack().clear(); // remove all styles
            fillStyleStack( dp, context );
            context.styleStack().save();
            kdDebug ()<<"insert new page "<<pos<<endl;
            KPrPage *newpage = 0L;
            if ( m_pageWhereLoadObject )
            {
                newpage = m_pageWhereLoadObject;
            }
            else
            {
                if ( pos != 0 )
                {
                    newpage = new KPrPage( this, m_masterPage );
                    m_pageList.insert( pos,newpage);
                }
                else //we create a first page into KPresenterDoc()
                {
                    newpage = m_pageList.at(pos);
                }
            }
            //m_pageList.at(pos)->insertManualTitle(dp.attributeNS( KoXmlNS::draw, "name", QString::null ));

            //necessary to create a unique name for page
            QString str = dp.attributeNS( KoXmlNS::draw, "name", QString::null );
            QString idPage = dp.attributeNS( KoXmlNS::draw, "id", QString::null );
            if ( str != QString( "page%1" ).arg( idPage ) )
                newpage->insertManualTitle(str);
            context.styleStack().setTypeProperties( "drawing-page" );
            if ( context.styleStack().hasAttributeNS( KoXmlNS::draw, "fill" )
                 || context.styleStack().hasAttributeNS( KoXmlNS::presentation, "transition-style" ) )
            {
                kdDebug()<<" fill or presentation-style found \n";
                newpage->loadOasis( context );
            }
            else if ( !context.styleStack().hasAttributeNS( KoXmlNS::draw, "fill" ) && backgroundStyle)
            {
                context.styleStack().save();
                context.addStyles( backgroundStyle );
                newpage->loadOasis(context);
                context.styleStack().restore();
                kdDebug()<<" load standard background \n";
            }

            //All animation object for current page is store into this element
            createPresentationAnimation(KoDom::namedItemNS( drawPage, KoXmlNS::presentation, "animations"));
            // parse all objects
            loadOasisObject( newpage, drawPage, context );

            context.styleStack().restore();
            m_loadingInfo->clearAnimationShowDict(); // clear all show animations style
            m_loadingInfo->clearAnimationHideDict(); // clear all hide animations style
            ++pos;
        }
    }


    ignoreSticky = TRUE;
    kdDebug()<<" _clean :"<<_clean<<endl;
    if(_clean)
    {
        setModified(false);
#if 0   //FIXME
        //it crashed, I don't know why for the moment.
        startBackgroundSpellCheck();
#endif
    }
    updateCustomListSlideShow( m_loadingInfo->m_tmpCustomListMap, true );
    kdDebug(33001) << "Loading took " << (float)(dt.elapsed()) / 1000.0 << " seconds" << endl;

    if ( !settingsDoc.isNull() )
    {
        loadOasisSettings( settingsDoc );
    }
    emit sigProgress( 100 );
    recalcVariables( VT_FIELD );
    emit sigProgress( -1 );

    setModified( false );
    return true;
}


void KPresenterDoc::loadOasisObject( KPrPage * newpage, QDomNode & drawPage, KoOasisContext & context, KPGroupObject *groupObject )
{
    for ( QDomNode object = drawPage.firstChild(); !object.isNull(); object = object.nextSibling() )
    {
        QDomElement o = object.toElement();
        QString name = o.tagName();
        kdDebug()<<" name :"<<name<<endl;
        context.styleStack().save();
        const bool isDrawNS = o.namespaceURI() == KoXmlNS::draw;
        //"draw:text-box"
        if ( name == "frame" && isDrawNS) // textbox
        {
            fillStyleStack( o, context );

            QDomNode imageBox = KoDom::namedItemNS( o, KoXmlNS::draw, "image" );
            kdDebug()<<" imageBox:"<<imageBox.isNull()<<endl;
            if ( !imageBox.isNull() )
            {
                KPPixmapObject *kppixmapobject = new KPPixmapObject( pictureCollection() );
                kppixmapobject->loadOasis( o, context, m_loadingInfo);
                if ( groupObject )
                    groupObject->addObjects( kppixmapobject );
                else
                    newpage->appendObject(kppixmapobject);
            }

            QDomNode object = KoDom::namedItemNS( o, KoXmlNS::draw, "object" );
            kdDebug()<<" object:"<<object.isNull()<<endl;
            if ( !object.isNull() )
            {
                fillStyleStack( o, context );
                KPresenterChild *ch = new KPresenterChild( this );
                QRect r;
                KPPartObject *kppartobject = new KPPartObject( ch );
                kppartobject->loadOasis( o, context, m_loadingInfo );
                r = ch->geometry();
                if ( groupObject )
                    groupObject->addObjects( kppartobject );
                else
                    newpage->appendObject(kppartobject);
                insertChild( ch );
                kppartobject->setOrig( r.x(), r.y() );
                kppartobject->setSize( r.width(), r.height() );
            }
            else
            {
                KPTextObject *kptextobject = new KPTextObject( this );
                kptextobject->loadOasis(o, context, m_loadingInfo);
                if ( groupObject )
                    groupObject->addObjects( kptextobject );
                else
                    newpage->appendObject(kptextobject);
            }
        }
        else if ( name == "rect" && isDrawNS) // rectangle
        {
            fillStyleStack( o, context );
            KPRectObject *kprectobject = new KPRectObject();
            kprectobject->loadOasis(o, context , m_loadingInfo);
            if ( groupObject )
                groupObject->addObjects( kprectobject );
            else
                newpage->appendObject(kprectobject);
        }
        else if ( ( name == "circle" || name == "ellipse" )&& isDrawNS)
        {
            fillStyleStack( o, context );
            if ( o.hasAttributeNS( KoXmlNS::draw, "kind" ) ) // pie, chord or arc
            {
                KPPieObject *kppieobject = new KPPieObject();
                kppieobject->loadOasis(o, context, m_loadingInfo);
                if ( groupObject )
                    groupObject->addObjects( kppieobject );
                else
                    newpage->appendObject(kppieobject);
            }
            else  // circle or ellipse
            {
                KPEllipseObject *kpellipseobject = new KPEllipseObject();
                kpellipseobject->loadOasis(o,context, m_loadingInfo);
                if ( groupObject )
                    groupObject->addObjects( kpellipseobject );
                else
                    newpage->appendObject(kpellipseobject);
            }
        }
        else if ( name == "line" && isDrawNS) // line
        {
            fillStyleStack( o, context );
            KPLineObject *kplineobject = new KPLineObject();
            kplineobject->loadOasis(o, context, m_loadingInfo);
            newpage->appendObject(kplineobject);
        }
        else if (name=="polyline" && isDrawNS) { // polyline
            fillStyleStack( o, context );
            KPPolylineObject *kppolylineobject = new KPPolylineObject();
            kppolylineobject->loadOasis(o, context, m_loadingInfo);
            if ( groupObject )
                groupObject->addObjects( kppolylineobject );
            else
                newpage->appendObject(kppolylineobject);
        }
        else if (name=="polygon" && isDrawNS) { // plcloseobject
            fillStyleStack( o, context );
            KPClosedLineObject *kpClosedObject = new KPClosedLineObject();
            kpClosedObject->loadOasis( o, context, m_loadingInfo);
            if ( groupObject )
                groupObject->addObjects( kpClosedObject );
            else
                newpage->appendObject(kpClosedObject);
        }
        //FIXME wait that it will ok'ed by oo spec
        else if (name=="regular-polygon"&& isDrawNS) { // kppolygone object
            fillStyleStack( o, context );
            KPPolygonObject *kpPolygoneObject = new KPPolygonObject();
            kpPolygoneObject->loadOasis( o, context, m_loadingInfo);
            if ( groupObject )
                groupObject->addObjects( kpPolygoneObject );
            else
                newpage->appendObject(kpPolygoneObject);
        }
#if 0 //old code
        else if ( name == "image" && isDrawNS) // image
        {
            fillStyleStack( o, context );
            KPPixmapObject *kppixmapobject = new KPPixmapObject( pictureCollection() );
            kppixmapobject->loadOasis( o, context, m_loadingInfo);
            if ( groupObject )
                groupObject->addObjects( kppixmapobject );
            else
                newpage->appendObject(kppixmapobject);
        }
#endif
        else if ( name == "path" && isDrawNS)
        {
            //we have 4 elements to use here.
            //Cubicbeziercurve/Quadricbeziercurve/closeline/KPFreehandObject
            //we must parse svd:d argument
            // "z" close element
            // "c" cubic element
            // "q" quadic element
            // parse line we use relative position
            // see http://www.w3.org/TR/SVG/paths.html#PathData
            // see svgpathparser.cc (ksvg)
            QString pathDefinition = o.attributeNS( KoXmlNS::svg, "d", QString::null);
            kdDebug()<<"pathDefinition :"<<pathDefinition<<endl;
            fillStyleStack( o, context );

            if ( pathDefinition.contains( "c" ) )
            {
                kdDebug()<<"Cubicbeziercurve \n";
                KPCubicBezierCurveObject *kpCurveObject = new KPCubicBezierCurveObject();
                kpCurveObject->loadOasis( o, context, m_loadingInfo);
                if ( groupObject )
                    groupObject->addObjects( kpCurveObject );
                else
                    newpage->appendObject( kpCurveObject );

            }
            else if ( pathDefinition.contains( "q" ) )
            {
                kdDebug()<<"Quadricbeziercurve \n";
                KPQuadricBezierCurveObject *kpQuadricObject = new KPQuadricBezierCurveObject();
                kpQuadricObject->loadOasis( o, context, m_loadingInfo);
                if ( groupObject )
                    groupObject->addObjects( kpQuadricObject );
                else
                    newpage->appendObject( kpQuadricObject );
            }
            else
            {
                kdDebug()<<"KPFreehandObject \n";
                KPFreehandObject *kpFreeHandObject = new KPFreehandObject();
                kpFreeHandObject->loadOasis( o, context, m_loadingInfo);
                if ( groupObject )
                    groupObject->addObjects( kpFreeHandObject );
                else
                    newpage->appendObject( kpFreeHandObject );
            }
        }
        else if ( name == "g" && isDrawNS)
        {
            fillStyleStack( o, context );
            KPGroupObject *kpgroupobject = new KPGroupObject();
            QDomNode nodegroup = object.firstChild();

            kpgroupobject->loadOasisGroupObject( this, newpage, object, context, m_loadingInfo);
            if ( groupObject )
                groupObject->addObjects( kpgroupobject );
            else
                newpage->appendObject(kpgroupobject);
        }
        else if ( name == "notes" && o.namespaceURI() == KoXmlNS::presentation ) // notes
        {
            //we must extend note attribute
            //kdDebug()<<"presentation:notes----------------------------------\n";
            QDomNode frameBox = KoDom::namedItemNS( o, KoXmlNS::draw, "frame" );
            //todo load layout for note.
            QDomNode textBox = KoDom::namedItemNS( frameBox, KoXmlNS::draw, "text-box" );

            if ( !textBox.isNull() )
            {
                QString note;
                for ( QDomNode text = textBox.firstChild(); !text.isNull(); text = text.nextSibling() )
                {
                    // We don't care about styles as they are not supported in kpresenter.
                    // Only add a linebreak for every child.
                    QDomElement t = text.toElement();
                    note += t.text() + "\n";
                    kdDebug()<<" note :"<<note<<endl;
                }
                newpage->setNoteText( note );
            }
        }
        else if ( ( name == "header" || name == "footer" ) && o.namespaceURI() == KoXmlNS::style )
        {
            //nothing
        }
        else
        {
            kdDebug() << "Unsupported object '" << name << "'" << endl;
            context.styleStack().restore();
            continue;
        }
        context.styleStack().restore();
    }

}

int KPresenterDoc::createPresentationAnimation(const QDomElement& element, int order, bool increaseOrder)
{
  kdDebug()<<"void KPresenterDoc::createPresentationAnimation(const QDomElement& element)\n";
  int orderAnimation = increaseOrder ? 0 : order;
  for ( QDomNode n = element.firstChild(); !n.isNull(); n = n.nextSibling() )
    {
        QDomElement e = n.toElement();
	QCString tagName = e.tagName().latin1();
        const bool isPresentationNS = e.namespaceURI() == KoXmlNS::presentation;
	kdDebug()<<"(createPresentationAnimation) tagName found :"<<tagName<<endl;
        if ( tagName == "show-shape" && isPresentationNS )
        {
            Q_ASSERT( e.hasAttributeNS( KoXmlNS::draw, "shape-id" ) );
            QString name = e.attributeNS( KoXmlNS::draw, "shape-id", QString::null );
	    kdDebug()<<" insert animation show style : name :"<<name<<endl;
            QDomElement* ep = new QDomElement( e );
            lstAnimation *tmp = new lstAnimation;
            tmp->element = ep;
            tmp->order = orderAnimation;
	    m_loadingInfo->storePresentationShowAnimation( tmp, name );
            if ( increaseOrder )
                ++orderAnimation;

        }
        else if ( tagName == "hide-shape" && isPresentationNS)
        {
            Q_ASSERT( e.hasAttributeNS( KoXmlNS::draw, "shape-id" ) );
            QString name = e.attributeNS( KoXmlNS::draw, "shape-id", QString::null );
	    kdDebug()<<" insert animation hide style : name :"<<name<<endl;
            QDomElement* ep = new QDomElement( e );
            lstAnimation *tmp = new lstAnimation;
            tmp->element = ep;
            tmp->order = orderAnimation;
	    m_loadingInfo->storePresentationHideAnimation( tmp, name );
            if ( increaseOrder )
                ++orderAnimation;

        }
        else if ( tagName == "animation-group" && isPresentationNS)
        {
            kdDebug()<<" presentation:animation-group exist \n";
            orderAnimation = createPresentationAnimation( e, orderAnimation, false );
            kdDebug()<<" end presentation:animation-group exist\n";
        }
    }
  //increase when we finish it necessary for group object
  ++orderAnimation;
  return orderAnimation;
}

void KPresenterDoc::fillStyleStack( const QDomElement& object, KoOasisContext & context )
{
    // find all styles associated with an object and push them on the stack
    if ( object.hasAttributeNS( KoXmlNS::presentation, "style-name" ))
    {
        //kdDebug()<<"Add 'presentation:style-name' \n";
        addStyles( context.oasisStyles().styles()[object.attributeNS( KoXmlNS::presentation, "style-name", QString::null )], context );
    }
    if ( object.hasAttributeNS( KoXmlNS::draw, "style-name" ) )
    {
        kdDebug()<<"draw:style-name :"<<object.attributeNS( KoXmlNS::draw, "style-name", QString::null )<<endl;
        addStyles( context.oasisStyles().styles()[object.attributeNS( KoXmlNS::draw, "style-name", QString::null )], context);
    }
    if ( object.hasAttributeNS( KoXmlNS::draw, "text-style-name" ) )
    {
        //kdDebug()<<"Add 'draw:text-style-name' \n";
        addStyles( context.oasisStyles().styles()[object.attributeNS( KoXmlNS::draw, "text-style-name", QString::null )], context );
    }
    if ( object.hasAttributeNS( KoXmlNS::text, "style-name" ) )
    {
        //kdDebug()<<"Add 'text:style-name' : "<<object.attributeNS( KoXmlNS::text, "style-name", QString::null )<<endl;
        addStyles( context.oasisStyles().styles()[object.attributeNS( KoXmlNS::text, "style-name", QString::null )], context );
    }
}

void KPresenterDoc::addStyles( const QDomElement* style, KoOasisContext & context )
{
    // this function is necessary as parent styles can have parents themself
    if ( style->hasAttributeNS( KoXmlNS::style, "parent-style-name" ) )
        addStyles( context.oasisStyles().styles()[style->attributeNS( KoXmlNS::style, "parent-style-name", QString::null )], context );
    context.addStyles( style );
}


bool KPresenterDoc::loadXML( QIODevice * dev, const QDomDocument& doc )
{
    QTime dt;
    dt.start();
    m_loadingInfo = new KPRLoadingInfo( true );

    ignoreSticky = FALSE;
    bool b=false;
    QDomElement docelem = doc.documentElement();
    const int syntaxVersion = docelem.attribute( "syntaxVersion" ).toInt();
    if ( syntaxVersion < 2 )
    {
        // This is an old style document, before the current TextObject
        // We have kprconverter.pl for it
        kdWarning(33001) << "KPresenter document version 1. Launching perl script to convert it." << endl;

        // Read the full XML and write it to a temp file
        KTempFile tmpFileIn;
        tmpFileIn.setAutoDelete( true );
        dev->reset();
        tmpFileIn.file()->writeBlock( dev->readAll() ); // copy stresm to temp file
        tmpFileIn.close();

        // Launch the perl script on it
        KTempFile tmpFileOut;
        tmpFileOut.setAutoDelete( true );
        QString cmd = KGlobal::dirs()->findExe("perl");
        if (cmd.isEmpty())
        {
            setErrorMessage( i18n("You don't appear to have PERL installed.\nIt is needed to convert this document.\nPlease install PERL and try again."));
            return false;
        }
        cmd += " ";
        cmd += locate( "exe", "kprconverter.pl" );
        cmd += " ";
        cmd += KProcess::quote( tmpFileIn.name() );
        cmd += " ";
        cmd += KProcess::quote( tmpFileOut.name() );
        system( QFile::encodeName(cmd) );

        // Build a new QDomDocument from the result
        QString errorMsg;
        int errorLine;
        int errorColumn;
        QDomDocument newdoc;
        if ( ! newdoc.setContent( tmpFileOut.file(), &errorMsg, &errorLine, &errorColumn ) )
        {
            kdError (33001) << "Parsing Error! Aborting! (in KPresenterDoc::loadXML)" << endl
                            << "  Line: " << errorLine << " Column: " << errorColumn << endl
                            << "  Message: " << errorMsg << endl;
            setErrorMessage( i18n( "parsing error in the main document (converted from an old KPresenter format) at line %1, column %2\nError message: %3" )
                             .arg( errorLine ).arg( errorColumn ).arg( i18n ( errorMsg.utf8() ) ) );
            return false;
        }
        b = loadXML( newdoc );
    }
    else
        b = loadXML( doc );

    ignoreSticky = TRUE;

    if(_clean)
    {
        startBackgroundSpellCheck();
        updateCustomListSlideShow( m_loadingInfo->m_tmpCustomListMap, true );
    }
    if ( m_pageWhereLoadObject == 0 && m_insertFilePage == 0 )
        setModified( false );
    kdDebug(33001) << "Loading took " << (float)(dt.elapsed()) / 1000.0 << " seconds" << endl;
    return b;
}

void KPresenterDoc::createHeaderFooter()
{
    //add header/footer to sticky page
    KoRect pageRect=m_masterPage->getPageRect();
    _header->setOrig(pageRect.topLeft());
    _header->setSize(pageRect.width(),20);

    _footer->setOrig(pageRect.left(),pageRect.bottom()-20);
    _footer->setSize(pageRect.width(),20);

    m_masterPage->appendObject(_header);
    m_masterPage->appendObject(_footer);
}

void KPresenterDoc::insertEmbedded( KoStore *store, QDomElement topElem, KMacroCommand * macroCmd, KPrPage *page, int pos )
{
    QDomElement elem = topElem.firstChild().toElement();
    for ( ; !elem.isNull() ; elem = elem.nextSibling().toElement() )
    {
        kdDebug(33001) << "Element name: " << elem.tagName() << endl;
        if(elem.tagName()=="EMBEDDED") {
            KPresenterChild *ch = new KPresenterChild( this );
            KPPartObject *kppartobject = 0L;
            QRect r;

            QDomElement object=elem.namedItem("OBJECT").toElement();
            if(!object.isNull()) {
                ch->load(object, true);  // true == uppercase
                r = ch->geometry();
                ch->loadDocument( store );
                insertChild( ch );
                kppartobject = new KPPartObject( ch );
            }
            QDomElement settings=elem.namedItem("SETTINGS").toElement();
            int zIndex = 0;
            if ( settings.hasAttribute( "z-index" ) )
            {
                zIndex = settings.attribute( "z-index" ).toInt();
            }
            double offset = 0.0;
            if(!settings.isNull() && kppartobject!=0)
                offset=kppartobject->load(settings);
            else if ( settings.isNull() ) // all embedded obj must have SETTING tags
            {
                delete kppartobject;
                kppartobject = 0L;
                return;
            }
            int index = m_pageList.findRef(page);
            int pageIndex = (int)(offset/__pgLayout.ptHeight)+index;
            int newPos=(int)((offset+index*__pgLayout.ptHeight)-pageIndex*__pgLayout.ptHeight);
            kppartobject->setOrig(kppartobject->getOrig().x(),newPos);

            InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Part Object" ), kppartobject, this,page );
            insertCmd->execute();
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Insert Part Object"));
            macroCmd->addCommand( insertCmd );
            if ( pos != 0 )
            {
                QPtrList<KPObject> oldList( page->objectList() );
                // tz TODO this is not 100% correct
                if ( oldList.count() > pos + zIndex )
                {
                    page->takeObject( kppartobject );
                    page->insertObject( kppartobject, pos + zIndex );
                    LowerRaiseCmd *lrCmd = new LowerRaiseCmd( i18n("Insert Part Object"),
                                                              oldList, page->objectList(),
                                                              this, page );
                    macroCmd->addCommand( lrCmd );
                }
            }
        }
    }
}

bool KPresenterDoc::loadXML( const QDomDocument &doc )
{
    emit sigProgress( 0 );
    int activePage=0;
    lastObj = -1;
    bool allSlides = false;
    // clean
    if ( _clean ) {
        __pgLayout = KoPageLayout::standardLayout();
        _spInfiniteLoop = false;
        _spManualSwitch = true;
        _showPresentationDuration = false;
        _xRnd = 20;
        _yRnd = 20;
        //_txtBackCol = white;
        urlIntern = url().path();
    }
    else
        m_spellCheckIgnoreList.clear();
    emit sigProgress( 5 );

    QDomElement document=doc.documentElement();
    // DOC
    if(document.tagName()!="DOC") {
        kdWarning(33001) << "Missing DOC" << endl;
        setErrorMessage( i18n("Invalid document, DOC tag missing.") );
        return false;
    }

    if(!document.hasAttribute("mime") ||  (
           document.attribute("mime")!="application/x-kpresenter" &&
           document.attribute("mime")!="application/vnd.kde.kpresenter" ) ) {
        kdError(33001) << "Unknown mime type " << document.attribute("mime") << endl;
        setErrorMessage( i18n("Invalid document, expected mimetype application/x-kpresenter or application/vnd.kde.kpresenter, got %1").arg(document.attribute("mime")) );
        return false;
    }
    if(document.hasAttribute("url"))
        urlIntern=KURL(document.attribute("url")).path();

    emit sigProgress( 10 );

    QDomElement elem=document.firstChild().toElement();

    uint childTotalCount=document.childNodes().count();
    uint childCount = 0;

    loadTextStyle( document );

    while(!elem.isNull()) {
        kdDebug(33001) << "Element name: " << elem.tagName() << endl;
        if(elem.tagName()=="EMBEDDED") {
            KPresenterChild *ch = new KPresenterChild( this );
            KPPartObject *kppartobject = 0L;
            QRect r;

            QDomElement object=elem.namedItem("OBJECT").toElement();
            if(!object.isNull()) {
                ch->load(object, true);  // true == uppercase
                r = ch->geometry();
                insertChild( ch );
                kppartobject = new KPPartObject( ch );
                //emit sig_insertObject( ch, kppartobject );
            }
            QDomElement settings=elem.namedItem("SETTINGS").toElement();
            int tmp=0;
            int pos = -1;
            if ( settings.hasAttribute( "z-index" ) )
            {
                pos = settings.attribute( "z-index" ).toInt();
            }
            if(settings.hasAttribute("sticky"))
                tmp=settings.attribute("sticky").toInt();
            bool sticky=static_cast<bool>(tmp);
            double offset = 0.0;
            if(!settings.isNull() && kppartobject!=0)
            {
                offset=kppartobject->load(settings);
            }
            else if ( settings.isNull() ) // all embedded obj must have SETTING tags
            {
                delete kppartobject;
                kppartobject = 0L;
            }
            //hack for some old file, they don't have ORIG tag !
            if ( offset == -1.0 )
                offset = r.y();
            if ( sticky && !ignoreSticky && kppartobject )
            {
                if ( pos == -1 )
                {
                    m_masterPage->appendObject( kppartobject );
                }
                else
                {
                    m_masterPage->insertObject( kppartobject, pos );
                }
                kppartobject->setOrig(r.x(), offset);
                kppartobject->setSize( r.width(), r.height() );
                kppartobject->setSticky(sticky);
            }
            else if ( kppartobject ) {
                kppartobject->setOrig( r.x(), 0 );
                kppartobject->setSize( r.width(), r.height() );
                if ( m_pageWhereLoadObject )
                {
                    kppartobject->setOrig( r.x(), offset );
                    m_pageWhereLoadObject->insertObject( kppartobject, pos );
                }
                else
                {
                    insertObjectInPage( offset, kppartobject, pos );
                }
            }
        } else if(elem.tagName()=="PAPER" && _clean)  {
            if(elem.hasAttribute("format"))
                __pgLayout.format=static_cast<KoFormat>(elem.attribute("format").toInt());
            if(elem.hasAttribute("orientation"))
                __pgLayout.orientation=static_cast<KoOrientation>(elem.attribute("orientation").toInt());
            if(elem.hasAttribute("ptWidth"))
                __pgLayout.ptWidth = elem.attribute("ptWidth").toDouble();
            else if(elem.hasAttribute("inchWidth"))  //compatibility
                __pgLayout.ptWidth = INCH_TO_POINT( elem.attribute("inchWidth").toDouble() );
            else if(elem.hasAttribute("mmWidth"))    //compatibility
                __pgLayout.ptWidth = MM_TO_POINT( elem.attribute("mmWidth").toDouble() );
            if(elem.hasAttribute("ptHeight"))
                __pgLayout.ptHeight = elem.attribute("ptHeight").toDouble();
            else if(elem.hasAttribute("inchHeight")) //compatibility
                __pgLayout.ptHeight = INCH_TO_POINT( elem.attribute("inchHeight").toDouble() );
            else if(elem.hasAttribute("mmHeight"))   //compatibility
                __pgLayout.ptHeight = MM_TO_POINT( elem.attribute("mmHeight").toDouble() );
            if(elem.hasAttribute("unit"))
                m_unit = static_cast<KoUnit::Unit>(elem.attribute("unit").toInt());
            if ( elem.hasAttribute("tabStopValue"))
                m_tabStop = elem.attribute("tabStopValue").toDouble();

            if(elem.hasAttribute("width"))
                __pgLayout.ptWidth = MM_TO_POINT( elem.attribute("width").toDouble() );
            if(elem.hasAttribute("height"))
                __pgLayout.ptHeight = MM_TO_POINT( elem.attribute("height").toDouble() );

            QDomElement borders=elem.namedItem("PAPERBORDERS").toElement();
            if(!borders.isNull()) {
                if(borders.hasAttribute("left"))
                    __pgLayout.ptLeft = MM_TO_POINT( borders.attribute("left").toDouble() );
                if(borders.hasAttribute("top"))
                    __pgLayout.ptTop = MM_TO_POINT( borders.attribute("top").toDouble() );
                if(borders.hasAttribute("right"))
                    __pgLayout.ptRight = MM_TO_POINT( borders.attribute("right").toDouble() );
                if(borders.hasAttribute("bottom"))
                    __pgLayout.ptBottom = MM_TO_POINT( borders.attribute("bottom").toDouble() );
                if(borders.hasAttribute("ptLeft"))
                    __pgLayout.ptLeft = borders.attribute("ptLeft").toDouble();
                else if(borders.hasAttribute("inchLeft"))    //compatibility
                    __pgLayout.ptLeft = INCH_TO_POINT( borders.attribute("inchLeft").toDouble() );
                else if(borders.hasAttribute("mmLeft"))      //compatibility
                    __pgLayout.ptLeft = MM_TO_POINT( borders.attribute("mmLeft").toDouble() );
                if(borders.hasAttribute("ptRight"))
                    __pgLayout.ptRight = borders.attribute("ptRight").toDouble();
                else if(borders.hasAttribute("inchRight"))   //compatibility
                    __pgLayout.ptRight = INCH_TO_POINT( borders.attribute("inchRight").toDouble() );
                else if(borders.hasAttribute("mmRight"))     //compatibility
                    __pgLayout.ptRight = MM_TO_POINT( borders.attribute("mmRight").toDouble() );
                if(borders.hasAttribute("ptTop"))
                    __pgLayout.ptTop = borders.attribute("ptTop").toDouble();
                else if(borders.hasAttribute("inchTop"))     //compatibility
                    __pgLayout.ptTop = INCH_TO_POINT( borders.attribute("inchTop").toDouble() );
                else if(borders.hasAttribute("mmTop"))       //compatibility
                    __pgLayout.ptTop = MM_TO_POINT( borders.attribute("mmTop").toDouble() );
                if(borders.hasAttribute("ptBottom"))
                    __pgLayout.ptBottom = borders.attribute("ptBottom").toDouble();
                else if(borders.hasAttribute("inchBottom"))  //compatibility
                    __pgLayout.ptBottom = INCH_TO_POINT( borders.attribute("inchBottom").toDouble() );
                else if(borders.hasAttribute("mmBottom"))    //compatibility
                    __pgLayout.ptBottom = MM_TO_POINT( borders.attribute("inchBottom").toDouble() );
            }
            // PAPER found and parsed -> apply page layout
            // e.g. the text objects need it
            if ( _clean )
                setPageLayout( __pgLayout );

        } else if(elem.tagName()=="VARIABLESETTINGS" && _clean){
            getVariableCollection()->variableSetting()->load(document);
            //by default display real variable value
            if ( !isReadWrite())
                getVariableCollection()->variableSetting()->setDisplayFieldCode(false);

        }
        else if(elem.tagName()=="BACKGROUND") {
            int red=0, green=0, blue=0;
            if(elem.hasAttribute("xRnd"))
                _xRnd = elem.attribute("xRnd").toInt();
            if(elem.hasAttribute("yRnd"))
                _yRnd = elem.attribute("yRnd").toInt();
            if(elem.hasAttribute("bred"))
                red = elem.attribute("bred").toInt();
            if(elem.hasAttribute("bgreen"))
                green = elem.attribute("bgreen").toInt();
            if(elem.hasAttribute("bblue"))
                blue = elem.attribute("bblue").toInt();
            loadBackground(elem);
        } else if(elem.tagName()=="HEADER") {
            if ( _clean /*don't reload header footer, header/footer was created at the beginning || !hasHeader()*/ ) {
                if(elem.hasAttribute("show")) {
                    setHeader(static_cast<bool>(elem.attribute("show").toInt()));
                }
                _header->load(elem);
            }
        } else if ( elem.tagName()=="HEADERFOOTERBYPAGE" ) {
            if ( elem.hasAttribute( "value" ) ) {
                m_loadingInfo->m_headerFooterByPage = true;
            }
        } else if(elem.tagName()=="FOOTER") {
            if ( _clean /*|| !hasFooter()*/ ) {
                if(elem.hasAttribute("show")) {
                    setFooter( static_cast<bool>(elem.attribute("show").toInt() ) );
                }
                _footer->load(elem);
            }
        }else if( elem.tagName()=="HELPLINES"){
            if ( _clean  ) {
                if(elem.hasAttribute("show")) {
                    setShowHelplines( static_cast<bool>(elem.attribute("show").toInt() ) );
                }
                loadHelpLines( elem );
            }
        }else if( elem.tagName()=="SPELLCHECKIGNORELIST"){
            QDomElement spellWord=elem.toElement();
            spellWord=spellWord.firstChild().toElement();
            while ( !spellWord.isNull() )
            {
                if ( spellWord.tagName()=="SPELLCHECKIGNOREWORD" )
                {
                    m_spellCheckIgnoreList.append(spellWord.attribute("word"));
                }
                spellWord=spellWord.nextSibling().toElement();
            }
        }else if(elem.tagName()=="ATTRIBUTES" && _clean) {
            if(elem.hasAttribute("activePage"))
                activePage=elem.attribute("activePage").toInt();
            if(elem.hasAttribute("gridx"))
                m_gridX = elem.attribute("gridx").toDouble();
            if(elem.hasAttribute("gridy"))
                m_gridY = elem.attribute("gridy").toDouble();
            if(elem.hasAttribute("snaptogrid"))
                m_bSnapToGrid = (bool)elem.attribute("snaptogrid").toInt();
        } else if(elem.tagName()=="PAGETITLES") {
            loadTitle(elem);
        } else if(elem.tagName()=="PAGENOTES") {
            loadNote(elem);
        } else if(elem.tagName()=="OBJECTS") {
            //FIXME**********************
#if 0
            lastObj = _objectList->count() - 1;
#endif
            //don't add command we don't paste object
            KCommand * cmd =loadObjects(elem);
            if ( cmd )
                delete cmd;
        } else if(elem.tagName()=="INFINITLOOP") {
            if(_clean) {
                if(elem.hasAttribute("value"))
                    _spInfiniteLoop = static_cast<bool>(elem.attribute("value").toInt());
            }
        } else if(elem.tagName()=="PRESSPEED") {
            if(_clean) {
                if(elem.hasAttribute("value"))
                    m_loadingInfo->presSpeed = elem.attribute("value").toInt();
            }
        } else if(elem.tagName()=="MANUALSWITCH") {
            if(_clean) {
                if(elem.hasAttribute("value"))
                    _spManualSwitch = static_cast<bool>(elem.attribute("value").toInt());
            }
        } else if(elem.tagName()=="SHOWPRESENTATIONDURATION") {
            if(_clean) {
                if(elem.hasAttribute("value"))
                    _showPresentationDuration = static_cast<bool>(elem.attribute("value").toInt());
            }
        } else if(elem.tagName()=="PRESSLIDES") {
            if(elem.hasAttribute("value") && elem.attribute("value").toInt()==0)
                allSlides = TRUE;
        } else if ( elem.tagName()=="DEFAULTCUSTOMSLIDESHOWNAME" ) {
            if(elem.hasAttribute("name") )
                m_presentationName=elem.attribute( "name" );
        } else if ( elem.tagName()=="CUSTOMSLIDESHOWCONFIG" ) {
            if ( _clean ) {
                QDomElement slide=elem.firstChild().toElement();
                while(!slide.isNull()) {
                    if(slide.tagName()=="CUSTOMSLIDESHOW") {
                        QStringList tmp = QStringList::split( ",", slide.attribute( "pages" ) );
                        m_loadingInfo->m_tmpCustomListMap.insert( slide.attribute( "name" ), tmp );

                    }
                    slide=slide.nextSibling().toElement();
                }
            }
        } else if(elem.tagName()=="SELSLIDES") {
            if( _clean ) { // Skip this when loading a single page
                QDomElement slide=elem.firstChild().toElement();
                while(!slide.isNull()) {
                    if(slide.tagName()=="SLIDE") {
                        int nr = -1;
                        bool show = false;
                        if(slide.hasAttribute("nr"))
                            nr=slide.attribute("nr").toInt();
                        if(slide.hasAttribute("show"))
                            show=static_cast<bool>(slide.attribute("show").toInt());
                        if ( nr >= 0 )
                        {
                            //kdDebug(33001) << "KPresenterDoc::loadXML m_selectedSlides nr=" << nr << " show=" << show << endl;
                            if ( nr > ( (int)m_pageList.count() - 1 ) )
                            {
                                for (int i=(m_pageList.count()-1); i<nr;i++)
                                    m_pageList.append( new KPrPage( this, m_masterPage ) );
                            }
                            m_pageList.at(nr)->slideSelected(show);
                        } else kdWarning(33001) << "Parse error. No nr in <SLIDE> !" << endl;
                    }
                    slide=slide.nextSibling().toElement();
                }
            }
        } else if ( elem.tagName() == "SOUNDS" ) {
            loadUsedSoundFileFromXML( elem );
        }
        elem=elem.nextSibling().toElement();

        emit sigProgress( childCount * ( 70/childTotalCount ) + 15 );
        childCount += 1;
    }

    loadPictureMap( document );

    if(activePage!=-1)
        m_initialActivePage=m_pageList.at(activePage);
    if ( m_pageWhereLoadObject == 0 && m_insertFilePage == 0 )
        setModified(false);

    return true;
}

void KPresenterDoc::loadTextStyle( const QDomElement& domElement )
{
    QDomElement style = domElement.namedItem( "STYLES" ).toElement();
    if ( _clean && ! style.isNull() )
        loadStyleTemplates( style );
}

void KPresenterDoc::loadPictureMap ( const QDomElement& domElement )
{
    m_pictureMap.clear();

    // <PICTURES>
    QDomElement picturesElem = domElement.namedItem( "PICTURES" ).toElement();
    if ( !picturesElem.isNull() )
        m_pictureCollection.readXML( picturesElem, m_pictureMap );

    // <PIXMAPS>
    QDomElement pixmapsElem = domElement.namedItem( "PIXMAPS" ).toElement();
    if ( !pixmapsElem.isNull() )
        m_pictureCollection.readXML( pixmapsElem, m_pictureMap );

    // <CLIPARTS>
    QDomElement clipartsElem = domElement.namedItem( "CLIPARTS" ).toElement();
    if ( !clipartsElem.isNull() )
        m_pictureCollection.readXML( pixmapsElem, m_pictureMap );
}

void KPresenterDoc::loadBackground( const QDomElement &element )
{
    kdDebug(33001) << "KPresenterDoc::loadBackground" << endl;
    QDomElement page=element.firstChild().toElement();
    int i=m_insertFilePage;
    while(!page.isNull()) {
        if(m_pageWhereLoadObject)
            m_pageWhereLoadObject->load(page);
        else
        {
            if ( page.tagName() == "MASTERPAGE" )
            {
                m_masterPage->load( page );
            }
            else
            {
                //test if there is a page at this index
                //=> don't add new page if there is again a page
                if ( i > ( (int)m_pageList.count() - 1 ) )
                    m_pageList.append( new KPrPage( this, m_masterPage ) );
                m_pageList.at(i)->load(page);
                i++;
            }
        }
        page=page.nextSibling().toElement();
    }
}

KCommand *KPresenterDoc::loadObjects( const QDomElement &element, bool paste )
{
    ObjType t = OT_LINE;
    QDomElement obj=element.firstChild().toElement();
    bool createMacro = false;
    KMacroCommand *macro = new KMacroCommand( i18n("Paste Objects"));
    while(!obj.isNull()) {
        if(obj.tagName()=="OBJECT" ) {
            bool sticky=false;
            int tmp=0;
            if(obj.hasAttribute("type"))
                tmp=obj.attribute("type").toInt();
            t=static_cast<ObjType>(tmp);
            tmp=0;
            if(obj.hasAttribute("sticky"))
                tmp=obj.attribute("sticky").toInt();
            sticky=static_cast<bool>(tmp);
            double offset=0;
            switch ( t ) {
            case OT_LINE: {
                KPLineObject *kplineobject = new KPLineObject();
                offset=kplineobject->load(obj);
                if ( sticky && !ignoreSticky)
                {
                    m_masterPage->appendObject(kplineobject);
                    kplineobject->setOrig(kplineobject->getOrig().x(),offset);
                    kplineobject->setSticky(sticky);
                }
                else if (m_pageWhereLoadObject && paste) {
                    kplineobject->setOrig(kplineobject->getOrig().x(),offset);
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Line" ), kplineobject, this, m_pageWhereLoadObject );
                    macro->addCommand( insertCmd );
                    createMacro=true;
                    //insertCmd->execute();
                }
                else if( m_pageWhereLoadObject &&!paste)
                {
                    m_pageWhereLoadObject->appendObject(kplineobject);
                    kplineobject->setOrig(kplineobject->getOrig().x(),offset);
                }
                else
                    insertObjectInPage(offset, kplineobject);
            } break;
            case OT_RECT: {
                KPRectObject *kprectobject = new KPRectObject();
                offset=kprectobject->load(obj);
                if ( sticky && !ignoreSticky)
                {
                    m_masterPage->appendObject(kprectobject);
                    kprectobject->setOrig(kprectobject->getOrig().x(),offset);
                    kprectobject->setSticky(sticky);
                }
                else if (m_pageWhereLoadObject && paste) {
                    kprectobject->setOrig(kprectobject->getOrig().x(),offset);
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Rectangle" ), kprectobject, this, m_pageWhereLoadObject );
                    macro->addCommand( insertCmd );
                    createMacro=true;
                }
                else if( m_pageWhereLoadObject &&!paste)
                {
                    m_pageWhereLoadObject->appendObject(kprectobject);
                    kprectobject->setOrig(kprectobject->getOrig().x(),offset);
                }
                else
                    insertObjectInPage(offset, kprectobject);
            } break;
            case OT_ELLIPSE: {
                KPEllipseObject *kpellipseobject = new KPEllipseObject();
                offset=kpellipseobject->load(obj);
                if ( sticky && !ignoreSticky)
                {
                    m_masterPage->appendObject(kpellipseobject);
                    kpellipseobject->setOrig(kpellipseobject->getOrig().x(),offset);
                    kpellipseobject->setSticky(sticky);
                }
                else if ( m_pageWhereLoadObject && paste)
                {
                    kpellipseobject->setOrig(kpellipseobject->getOrig().x(),offset);
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Ellipse" ), kpellipseobject, this, m_pageWhereLoadObject );
                    macro->addCommand( insertCmd );
                    createMacro=true;
                }
                else if( m_pageWhereLoadObject &&!paste)
                {
                    m_pageWhereLoadObject->appendObject(kpellipseobject);
                    kpellipseobject->setOrig(kpellipseobject->getOrig().x(),offset);
                }
                else
                    insertObjectInPage(offset, kpellipseobject);
            } break;
            case OT_PIE: {
                KPPieObject *kppieobject = new KPPieObject();
                offset=kppieobject->load(obj);
                if ( sticky && !ignoreSticky)
                {
                    m_masterPage->appendObject(kppieobject);
                    kppieobject->setOrig(kppieobject->getOrig().x(),offset);
                    kppieobject->setSticky(sticky);
                }
                else if ( m_pageWhereLoadObject && paste) {
                    kppieobject->setOrig(kppieobject->getOrig().x(),offset);
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Pie/Arc/Chord" ), kppieobject, this, m_pageWhereLoadObject );
                    macro->addCommand( insertCmd );
                    createMacro=true;
                }
                else if( m_pageWhereLoadObject &&!paste)
                {
                    m_pageWhereLoadObject->appendObject(kppieobject);
                    kppieobject->setOrig(kppieobject->getOrig().x(),offset);
                }
                else
                    insertObjectInPage(offset, kppieobject);
            } break;
            case OT_AUTOFORM: {
                KPAutoformObject *kpautoformobject = new KPAutoformObject();
                offset=kpautoformobject->load(obj);
                if ( sticky && !ignoreSticky)
                {
                    m_masterPage->appendObject(kpautoformobject);
                    kpautoformobject->setOrig(kpautoformobject->getOrig().x(),offset);
                    kpautoformobject->setSticky(sticky);
                }
                else if ( m_pageWhereLoadObject&& paste) {
                    kpautoformobject->setOrig(kpautoformobject->getOrig().x(),offset);
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Autoform" ), kpautoformobject, this, m_pageWhereLoadObject );
                    macro->addCommand( insertCmd );
                    createMacro=true;
                }
                else if( m_pageWhereLoadObject &&!paste)
                {
                    m_pageWhereLoadObject->appendObject(kpautoformobject);
                    kpautoformobject->setOrig(kpautoformobject->getOrig().x(),offset);
                }
                else
                    insertObjectInPage(offset, kpautoformobject);
            } break;
            case OT_TEXT: {
                KPTextObject *kptextobject = new KPTextObject( this );
                offset=kptextobject->load(obj);
                if ( sticky && !ignoreSticky)
                {
                    m_masterPage->appendObject(kptextobject);
                    kptextobject->setOrig(kptextobject->getOrig().x(),offset);
                    kptextobject->setSticky(sticky);
                }
                else if ( m_pageWhereLoadObject && paste) {
                    kptextobject->setOrig(kptextobject->getOrig().x(),offset);
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Textbox" ), kptextobject, this, m_pageWhereLoadObject );
                    macro->addCommand( insertCmd );
                    createMacro=true;
                }
                else if( m_pageWhereLoadObject &&!paste)
                {
                    m_pageWhereLoadObject->appendObject(kptextobject);
                    kptextobject->setOrig(kptextobject->getOrig().x(),offset);
                }
                else
                    insertObjectInPage(offset, kptextobject);
            } break;
            case OT_CLIPART:
            case OT_PICTURE: {
                KPPixmapObject *kppixmapobject = new KPPixmapObject( pictureCollection() );
                offset=kppixmapobject->load(obj);
                if ( sticky && !ignoreSticky)
                {
                    m_masterPage->appendObject(kppixmapobject);
                    kppixmapobject->setOrig(kppixmapobject->getOrig().x(),offset);
                    kppixmapobject->setSticky(sticky);
                }
                else if ( m_pageWhereLoadObject && paste) {
                    kppixmapobject->setOrig(kppixmapobject->getOrig().x(),offset);
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Picture" ), kppixmapobject, this, m_pageWhereLoadObject );
                    macro->addCommand( insertCmd );
                    kppixmapobject->reload();
                    createMacro=true;
                }
                else if( m_pageWhereLoadObject &&!paste)
                {
                    m_pageWhereLoadObject->appendObject(kppixmapobject);
                    kppixmapobject->setOrig(kppixmapobject->getOrig().x(),offset);
                }
                else
                    insertObjectInPage(offset, kppixmapobject);
            } break;
            case OT_FREEHAND: {
                KPFreehandObject *kpfreehandobject = new KPFreehandObject();
                offset=kpfreehandobject->load(obj);

                if ( sticky && !ignoreSticky)
                {
                    m_masterPage->appendObject(kpfreehandobject);
                    kpfreehandobject->setOrig(kpfreehandobject->getOrig().x(),offset);
                    kpfreehandobject->setSticky(sticky);
                }
                else if ( m_pageWhereLoadObject && paste) {
                    kpfreehandobject->setOrig(kpfreehandobject->getOrig().x(),offset);
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Freehand" ), kpfreehandobject, this, m_pageWhereLoadObject );
                    macro->addCommand( insertCmd );
                    createMacro=true;
                }
                else if( m_pageWhereLoadObject &&!paste)
                {
                    m_pageWhereLoadObject->appendObject(kpfreehandobject);
                    kpfreehandobject->setOrig(kpfreehandobject->getOrig().x(),offset);
                }
                else
                    insertObjectInPage(offset,kpfreehandobject);
            } break;
            case OT_POLYLINE: {
                KPPolylineObject *kppolylineobject = new KPPolylineObject();
                offset=kppolylineobject->load(obj);
                if ( sticky && !ignoreSticky)
                {
                    m_masterPage->appendObject(kppolylineobject);
                    kppolylineobject->setOrig(kppolylineobject->getOrig().x(),offset);
                    kppolylineobject->setSticky(sticky);
                }
                else if (m_pageWhereLoadObject && paste) {
                    kppolylineobject->setOrig(kppolylineobject->getOrig().x(),offset);
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Polyline" ), kppolylineobject, this, m_pageWhereLoadObject );
                    macro->addCommand( insertCmd );
                    createMacro=true;

                }
                else if( m_pageWhereLoadObject &&!paste)
                {
                    m_pageWhereLoadObject->appendObject(kppolylineobject);
                    kppolylineobject->setOrig(kppolylineobject->getOrig().x(),offset);
                }
                else
                    insertObjectInPage(offset, kppolylineobject);
            } break;
            case OT_QUADRICBEZIERCURVE: {
                KPQuadricBezierCurveObject *kpQuadricBezierCurveObject = new KPQuadricBezierCurveObject();
                offset=kpQuadricBezierCurveObject->load(obj);
                if ( sticky && !ignoreSticky)
                {
                    m_masterPage->appendObject(kpQuadricBezierCurveObject);
                    kpQuadricBezierCurveObject->setOrig(kpQuadricBezierCurveObject->getOrig().x(),offset);
                    kpQuadricBezierCurveObject->setSticky(sticky);
                }
                else if ( m_pageWhereLoadObject && paste) {
                    kpQuadricBezierCurveObject->setOrig(kpQuadricBezierCurveObject->getOrig().x(),offset);
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Quadric Bezier Curve" ), kpQuadricBezierCurveObject,
                                                          this, m_pageWhereLoadObject );
                    macro->addCommand( insertCmd );
                    createMacro=true;
                }
                else if( m_pageWhereLoadObject &&!paste)
                {
                    m_pageWhereLoadObject->appendObject(kpQuadricBezierCurveObject);
                    kpQuadricBezierCurveObject->setOrig(kpQuadricBezierCurveObject->getOrig().x(),offset);
                }
                else
                    insertObjectInPage(offset, kpQuadricBezierCurveObject);
            } break;
            case OT_CUBICBEZIERCURVE: {
                KPCubicBezierCurveObject *kpCubicBezierCurveObject = new KPCubicBezierCurveObject();
                offset=kpCubicBezierCurveObject->load(obj);
                if ( sticky && !ignoreSticky)
                {
                    m_masterPage->appendObject(kpCubicBezierCurveObject);
                    kpCubicBezierCurveObject->setOrig(kpCubicBezierCurveObject->getOrig().x(),offset);
                    kpCubicBezierCurveObject->setSticky(sticky);
                }
                else if ( m_pageWhereLoadObject && paste) {
                    kpCubicBezierCurveObject->setOrig(kpCubicBezierCurveObject->getOrig().x(),offset);
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Cubic Bezier Curve" ), kpCubicBezierCurveObject,
                                                          this, m_pageWhereLoadObject );
                    macro->addCommand( insertCmd );
                    createMacro=true;

                }
                else if( m_pageWhereLoadObject &&!paste)
                {
                    m_pageWhereLoadObject->appendObject(kpCubicBezierCurveObject);
                    kpCubicBezierCurveObject->setOrig(kpCubicBezierCurveObject->getOrig().x(),offset);
                }
                else
                    insertObjectInPage(offset, kpCubicBezierCurveObject);
            } break;
            case OT_POLYGON: {
                KPPolygonObject *kpPolygonObject = new KPPolygonObject();
                offset=kpPolygonObject->load( obj );
                if ( sticky && !ignoreSticky)
                {
                    m_masterPage->appendObject(kpPolygonObject);
                    kpPolygonObject->setOrig(kpPolygonObject->getOrig().x(),offset);
                    kpPolygonObject->setSticky(sticky);
                }
                else if ( m_pageWhereLoadObject && paste) {
                    kpPolygonObject->setOrig(kpPolygonObject->getOrig().x(),offset);
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Polygon" ), kpPolygonObject, this, m_pageWhereLoadObject);
                    macro->addCommand( insertCmd );
                    createMacro=true;

                }
                else if( m_pageWhereLoadObject &&!paste)
                {
                    m_pageWhereLoadObject->appendObject(kpPolygonObject);
                    kpPolygonObject->setOrig(kpPolygonObject->getOrig().x(),offset);
                }
                else
                    insertObjectInPage(offset, kpPolygonObject);
            } break;
            case OT_CLOSED_LINE: {
                KPClosedLineObject *kpClosedLinneObject = new KPClosedLineObject();
                offset = kpClosedLinneObject->load( obj );
                if ( sticky && !ignoreSticky) {
                    m_masterPage->appendObject( kpClosedLinneObject );
                    kpClosedLinneObject->setOrig( kpClosedLinneObject->getOrig().x(), offset );
                    kpClosedLinneObject->setSticky( sticky );
                }
                else if ( m_pageWhereLoadObject && paste ) {
                    kpClosedLinneObject->setOrig( kpClosedLinneObject->getOrig().x(), offset );
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert %1" ).arg(kpClosedLinneObject->getTypeString()),
                                                          kpClosedLinneObject, this , m_pageWhereLoadObject );
                    macro->addCommand( insertCmd );
                    createMacro = true;

                }
                else if( m_pageWhereLoadObject &&!paste)
                {
                    m_pageWhereLoadObject->appendObject( kpClosedLinneObject );
                    kpClosedLinneObject->setOrig( kpClosedLinneObject->getOrig().x(), offset );
                }
                else
                    insertObjectInPage( offset, kpClosedLinneObject );
            } break;
            case OT_GROUP: {
                KPGroupObject *kpgroupobject = new KPGroupObject();
                offset=kpgroupobject->load(obj, this);
                if ( sticky && !ignoreSticky)
                {
                    m_masterPage->appendObject(kpgroupobject);
                    kpgroupobject->setOrig(kpgroupobject->getOrig().x(),offset);
                    kpgroupobject->setSticky(sticky);
                }
                else if ( m_pageWhereLoadObject && paste) {
                    kpgroupobject->setOrig(kpgroupobject->getOrig().x(),offset);
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Group Object" ), kpgroupobject, this, m_pageWhereLoadObject );
                    macro->addCommand( insertCmd );
                    createMacro=true;

                }
                else if( m_pageWhereLoadObject &&!paste)
                {
                    m_pageWhereLoadObject->appendObject(kpgroupobject);
                    kpgroupobject->setOrig(kpgroupobject->getOrig().x(),offset);
                }
                else
                    insertObjectInPage(offset, kpgroupobject);
            } break;
            default: break;
            }
        }
        obj=obj.nextSibling().toElement();
    }

    if ( createMacro )
    {
        macro->execute();
        return macro;
    }
    else
    {
        delete macro;
        return 0L;
    }
}

void KPresenterDoc::loadTitle( const QDomElement &element )
{
    QDomElement title=element.firstChild().toElement();
    int i=m_insertFilePage;
    while ( !title.isNull() ) {
        if ( title.tagName()=="Title" )
        {
            //test if there is a page at this index
            //=> don't add new page if there is again a page
            if(!m_pageWhereLoadObject)
            {
                if ( i > ( (int)m_pageList.count() - 1 ) )
                    m_pageList.append( new KPrPage( this, m_masterPage ) );
                m_pageList.at(i)->insertManualTitle(title.attribute("title"));
                i++;
            }
            else
                m_pageWhereLoadObject->insertManualTitle(title.attribute("title"));
        }
        title=title.nextSibling().toElement();
    }
}

void KPresenterDoc::loadNote( const QDomElement &element )
{
    QDomElement note=element.firstChild().toElement();
    int i=m_insertFilePage;
    while ( !note.isNull() ) {
        if ( note.tagName()=="Note" )
        {
            //test if there is a page at this index
            //=> don't add new page if there is again a page
            if(!m_pageWhereLoadObject)
            {
                if ( i > ( (int)m_pageList.count() - 1 ) )
                    m_pageList.append( new KPrPage( this, m_masterPage ) );
                m_pageList.at(i)->setNoteText(note.attribute("note"));
                i++;
            }
            else
                m_pageWhereLoadObject->setNoteText(note.attribute("note"));
        }
        note=note.nextSibling().toElement();
    }
}

void KPresenterDoc::loadUsedSoundFileFromXML( const QDomElement &element )
{
    usedSoundFile = QStringList();
    haveNotOwnDiskSoundFile = QStringList();
    QDomElement fileElement = element.firstChild().toElement();
    while ( !fileElement.isNull() ) {
        if ( fileElement.tagName() == "FILE" ) {
            QString fileName;
            if ( fileElement.hasAttribute( "name" ) )
                fileName = fileElement.attribute( "name" );

            if ( fileElement.hasAttribute( "filename" ) ) {
                QString name = fileElement.attribute( "filename" );
                QFile _file( name );
                if ( _file.open( IO_ReadOnly ) ) {
                    fileName = name;
                    _file.close();
                }
                else
                    haveNotOwnDiskSoundFile.append( name );
            }

            usedSoundFile.append( fileName );

            fileElement = fileElement.nextSibling().toElement();
        }
    }
}

void KPresenterDoc::loadImagesFromStore( KoStore *_store )
{
    if ( _store ) {
        m_pictureCollection.readFromStore( _store, m_pictureMap );
        m_pictureMap.clear(); // Release memory
    }
}

//TODO test if it's call when we load oasis format
bool KPresenterDoc::completeLoading( KoStore* _store )
{
    //kdDebug()<<"bool KPresenterDoc::completeLoading( KoStore* _store )*************************\n";
    emit sigProgress( 80 );

    if ( _store ) {
        loadImagesFromStore( _store );
        emit sigProgress( 90 );

        if ( !usedSoundFile.isEmpty() )
            loadUsedSoundFileFromStore( _store, usedSoundFile );

        if ( _clean )
            createHeaderFooter();
        //else {
        //m_pageList.last()->updateBackgroundSize();
        //}


        if ( saveOnlyPage == -1 ) {
            // ### following call independant of saveOnlyPage's value?
            m_masterPage->completeLoading( _clean, lastObj );
            QPtrListIterator<KPrPage> it( m_pageList );
            for ( ; it.current(); ++it )
                it.current()->completeLoading( _clean, lastObj );
        }
    } else {
        if ( _clean )
        {
            /// ### this has already been done, no?
            setPageLayout( __pgLayout );
        }
        else
            setPageLayout( m_pageLayout );
    }

    compatibilityFromOldFileFormat();

    emit sigProgress( 100 );
    recalcVariables( VT_FIELD );
    emit sigProgress( -1 );

    connect( documentInfo(), SIGNAL( sigDocumentInfoModifed()),this,SLOT(slotDocumentInfoModifed() ) );
    //desactivate bgspellchecking
    //attributes isReadWrite is not placed at the beginning !
    if ( !isReadWrite())
        enableBackgroundSpellCheck( false );
    return true;
}

void KPresenterDoc::loadUsedSoundFileFromStore( KoStore *_store, QStringList _list )
{
    int i = m_insertFilePage;
    QStringList::Iterator it = _list.begin();
    for ( ; it != _list.end(); ++it ) {
        QString soundFile = *it;

        if ( _store->open( soundFile ) ) {
            kdDebug( 33001 ) << "Not found file on disk. Use this( " << soundFile << " ) file." << endl;
            KoStoreDevice dev( _store );
            int size = _store->size();
            char *data = new char[size];
            dev.readBlock( data, size );

            int position = soundFile.findRev( '.' );
            QString format = soundFile.right( soundFile.length() - position );
            KTempFile *tmpFile = new KTempFile( QString::null, format );
            tmpFile->setAutoDelete( true );
            tmpFile->file()->writeBlock( data, size );
            tmpFile->close();

            QString tmpFileName = tmpFile->name();
            tmpSoundFileList.append( tmpFile );

            QString _fileName = *haveNotOwnDiskSoundFile.at( i );
            ++i;

            QPtrListIterator<KPrPage> it( m_pageList );
            for ( ; it.current(); ++it ) {
                QString _file = it.current()->getPageSoundFileName();
                if ( !_file.isEmpty() && _file == _fileName )
                    it.current()->setPageSoundFileName( tmpFileName );

                QPtrListIterator<KPObject> oIt( it.current()->objectList() );
                for ( ; oIt.current(); ++oIt ) {
                    _file = oIt.current()->getAppearSoundEffectFileName();
                    if ( !_file.isEmpty() && _file == _fileName )
                        oIt.current()->setAppearSoundEffectFileName( tmpFileName );

                    _file = oIt.current()->getDisappearSoundEffectFileName();
                    if ( !_file.isEmpty() && _file == _fileName )
                        oIt.current()->setDisappearSoundEffectFileName( tmpFileName );
                }
            }

            _store->close();
            delete data;
        }
        else {
            kdDebug( 33001 ) << "Found this( " << soundFile << " ) file on disk" << endl;
        }
    }
}

void KPresenterDoc::setPageLayout( const KoPageLayout &pgLayout )
{
    //     if ( _pageLayout == pgLayout )
    //  return;

    m_pageLayout = pgLayout;

    //for ( int i = 0; i < static_cast<int>( m_pageList.count() ); i++ )
    //    m_pageList.at( i )->updateBackgroundSize();

    repaint( false );
    layout();
    // don't setModified(true) here, since this is called on startup
}

//when we change pagelayout we must re-position header/footer
void KPresenterDoc::updateHeaderFooterPosition( )
{
    KoRect pageRect=m_masterPage->getPageRect();
    QRect oldBoundingRect=zoomHandler()->zoomRect(_header->getBoundingRect());
    _header->setOrig(pageRect.topLeft ());
    _header->setSize(pageRect.width(),_header->getSize().height());
    repaint( oldBoundingRect );
    repaint(_header);

    oldBoundingRect=zoomHandler()->zoomRect(_footer->getBoundingRect());
    _footer->setOrig(pageRect.left(),pageRect.bottom()-_footer->getSize().height());
    _footer->setSize(pageRect.width(),_footer->getSize().height());
    repaint(oldBoundingRect);
    repaint(_footer);
}

bool KPresenterDoc::initDoc(InitDocFlags flags, QWidget* parentWidget)
{

    if (flags==KoDocument::InitDocEmpty)
    {
        QString fileName( locate("kpresenter_template", "Screenpresentations/.source/Plain.kpt",
                                 KPresenterFactory::global() ) );
        objStartY = 0;
        _clean = true;
        bool ok = loadNativeFormat( fileName );
        if ( !ok )
            showLoadingErrorDialog();
        resetURL();
        setEmpty();
        return ok;
    }

    QString file;
    KoTemplateChooseDia::ReturnType ret;
    KoTemplateChooseDia::DialogType dlgtype;
    if (flags != InitDocFileNew)
            dlgtype = KoTemplateChooseDia::Everything;
    else
            dlgtype = KoTemplateChooseDia::OnlyTemplates;

    ret = KoTemplateChooseDia::choose( KPresenterFactory::global(), file,
                                       dlgtype, "kpresenter_template", parentWidget );

    if ( ret == KoTemplateChooseDia::Template ) {
        _clean = true; //was a parameter called "clean", but unused
        bool ok = loadNativeFormat( file );
        if ( !ok )
            showLoadingErrorDialog();
        objStartY = 0;
        _clean = true;
        resetURL();
        setEmpty();
        return ok;
    } else if ( ret == KoTemplateChooseDia::File ) {
        objStartY = 0;
        _clean = true;
        KURL url( file );
        bool ok = openURL( url );
        return ok;
    } else if ( ret == KoTemplateChooseDia::Empty ) {
        QString fileName( locate("kpresenter_template", "Screenpresentations/.source/Plain.kpt",
                                 KPresenterFactory::global() ) );
        objStartY = 0;
        _clean = true;
        bool ok = loadNativeFormat( fileName );
        if ( !ok )
            showLoadingErrorDialog();
        resetURL();
        setEmpty();
        return ok;
    } else
        return false;
}

void KPresenterDoc::initEmpty()
{
    QString fileName( locate("kpresenter_template", "Screenpresentations/.source/Plain.kpt",
                             KPresenterFactory::global() ) );
    objStartY = 0;
    _clean = true;
    setModified(true);
    bool ok = loadNativeFormat( fileName );
    if ( !ok )
        showLoadingErrorDialog();
    resetURL();
}

void KPresenterDoc::setEmpty()
{
    KoDocument::setEmpty();
    // Whether loaded from template or from empty doc: this is a new one -> set creation date
    m_varColl->variableSetting()->setCreationDate(QDateTime::currentDateTime());
}

void KPresenterDoc::setGridValue( double _x, double _y, bool _replace )
{
    oldGridX = m_gridX;
    oldGridY = m_gridY;
    m_gridX=_x;
    m_gridY=_y;
    if ( _replace )
        replaceObjs();
}

void KPresenterDoc::repaint( bool erase )
{
    QPtrListIterator<KoView> it( views() );
    for( ; it.current(); ++it ) {
        KPrCanvas* canvas = ((KPresenterView*)it.current())->getCanvas();
        canvas->repaint( erase );
    }
}

void KPresenterDoc::repaint( const QRect& rect )
{
    QRect r;
    QPtrListIterator<KoView> it( views() );
    for( ; it.current(); ++it ) {
        r = rect;
        KPrCanvas* canvas = ((KPresenterView*)it.current())->getCanvas();
        r.moveTopLeft( QPoint( r.x() - canvas->diffx(),
                               r.y() - canvas->diffy() ) );
        canvas->update( r );
    }
}

void KPresenterDoc::layout(KPObject *kpobject)
{
    KPTextObject * obj = dynamic_cast<KPTextObject *>( kpobject );
    if (obj)
        obj->layout();
}

void KPresenterDoc::layout()
{
    QPtrListIterator<KoView> it( views() );
    for( ; it.current(); ++it ) {
        KPrCanvas* canvas = ((KPresenterView*)it.current())->getCanvas();
        canvas->layout();
    }
}

void KPresenterDoc::repaint( KPObject *kpobject )
{
    repaint( m_zoomHandler->zoomRect(kpobject->getBoundingRect()) );
}

QValueList<int> KPresenterDoc::getPageEffectSteps( unsigned int num )
{
    return m_pageList.at(num)->getEffectSteps();
}

QRect KPresenterDoc::getPageRect( bool decBorders ) const
{
    int pw, ph, bl = static_cast<int>(m_pageLayout.ptLeft);
    int br = static_cast<int>(m_pageLayout.ptRight);
    int bt = static_cast<int>(m_pageLayout.ptTop);
    int bb = static_cast<int>(m_pageLayout.ptBottom);
    int wid = static_cast<int>(m_pageLayout.ptWidth);
    int hei = static_cast<int>(m_pageLayout.ptHeight);

    if ( !decBorders ) {
        br = 0;
        bt = 0;
        bl = 0;
        bb = 0;
    }

    pw = wid  - ( bl + br );
    ph = hei - ( bt + bb );

    return QRect( bl, bt, pw, ph );
}

int KPresenterDoc::getLeftBorder() const
{
    return static_cast<int>(m_pageLayout.ptLeft);
}

int KPresenterDoc::getTopBorder() const
{
    return static_cast<int>(m_pageLayout.ptTop);
}

int KPresenterDoc::getBottomBorder() const
{
    return static_cast<int>(m_pageLayout.ptBottom);
}

int KPresenterDoc::getRightBorder() const
{
    return static_cast<int>(m_pageLayout.ptRight);
}

void KPresenterDoc::deletePage( int _page )
{
    kdDebug(33001) << "KPresenterDoc::deletePage " << _page << endl;
    //m_pageList.at(_page)->deletePage();
    if ( m_pageList.count()==1 )
        return;
    KPrDeletePageCmd *cmd = new KPrDeletePageCmd( i18n("Delete Slide"), _page, this );
    cmd->execute();
    addCommand(cmd);
}

void KPresenterDoc::insertPage( KPrPage *page, int currentPageNum, int insertPageNum )
{
    // check if page was allready deleted
    int pos = m_deletedPageList.findRef( page );
    if ( pos != -1 )
        m_deletedPageList.take( pos );

    m_pageList.insert( insertPageNum, page );

    pageOrderChanged();
    //activate this page in all views which on slide currentPageNum
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
    {
        KPresenterView *view = static_cast<KPresenterView*>( it.current() );
        view->addSideBarItem( insertPageNum );

        // change to the new page if the view was on the current page.
        if ( (int)view->getCurrPgNum() - 1 == currentPageNum )
        {
            view->skipToPage( insertPageNum );
        }
        else // recalc the page number as it might have been changed
        {
            view->recalcCurrentPageNum();
        }
    }
}

void KPresenterDoc::takePage( KPrPage *page, int pageNum )
{
    int pos = m_pageList.findRef( page );
    m_pageList.take( pos );
    m_deletedPageList.append( page );

    pageOrderChanged();

    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
    {
        KPresenterView *view = static_cast<KPresenterView*>( it.current() );
        view->removeSideBarItem( pos );

        // change to the new page if the view was on the current page.
        if ( (int)view->getCurrPgNum() - 1 == pos )
        {
            view->skipToPage( pageNum );
        }
        else // recalc the page number as it might have been changed
        {
            view->recalcCurrentPageNum();
        }
    }

    repaint( false );
}

void KPresenterDoc::pageOrderChanged()
{
    recalcVariables( VT_PGNUM );

    //update statusbar
    emit pageNumChanged();
    emit sig_updateMenuBar();
}

void KPresenterDoc::movePageTo( int oldPos, int newPos )
{
    kdDebug(33001) << "movePage oldPos = " << oldPos << ", neuPos = " << newPos << endl;

    KPrPage * page = m_pageList.take( oldPos );
    m_pageList.insert( newPos, page );

    pageOrderChanged();

    // Update the sidebars
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
    {
        KPresenterView *view = static_cast<KPresenterView*>( it.current() );
        view->moveSideBarItem( oldPos, newPos );

        // change to the new page if the view was on the old pos.
        if ( (int)view->getCurrPgNum() - 1 == oldPos )
        {
            view->skipToPage( newPos );
        }
        else // recalc the page number as it might have been changed
        {
            view->recalcCurrentPageNum();
        }
    }
}

QString KPresenterDoc::templateFileName( bool chooseTemplate, const QString &theFile )
{
    QString fileName;
    if ( !chooseTemplate ) {
        if ( theFile.isEmpty() )
            fileName = locateLocal( "appdata", "default.kpr" );
        else
            fileName = theFile;
    } else {
        // TODO: pass parentWidget as parameter to this method
        QWidget* parentWidget = 0;
        QString _template;
        if ( KoTemplateChooseDia::choose( KPresenterFactory::global(), _template,
                                          KoTemplateChooseDia::OnlyTemplates,
                                          "kpresenter_template", parentWidget ) == KoTemplateChooseDia::Cancel )
            return QString::null;
        QFileInfo fileInfo( _template );
        fileName = fileInfo.dirPath( true ) + "/" + fileInfo.baseName() + ".kpt";

        KURL src, dest;
        src.setPath( fileName );
        dest.setPath( locateLocal( "appdata", "default.kpr" ) );
        kdDebug(33001) << "Copying template  (in KPresenterDoc::templateFileName)" << endl
                       << "  from: " << src.prettyURL() << endl
                       << "  to: " << dest.prettyURL() << endl;
        KIO::NetAccess::file_copy( src,
				   dest,
				   -1, /* default permissions */
				   true /* overwrite */ );
    }
    return fileName;
}

int KPresenterDoc::insertNewPage( const QString &cmdName, int _page, InsertPos _insPos,
                                  bool chooseTemplate, const QString &theFile )
{
    kdDebug(33001) << "KPresenterDoc::insertNewPage " << _page << endl;

    QString fileName=templateFileName(chooseTemplate, theFile);
    if(fileName.isEmpty())
        return -1;

    _clean = false;

    objStartY=-1;

    //insert page.
    KPrPage *newpage = new KPrPage( this, m_masterPage );

    m_pageWhereLoadObject=newpage;
    m_childCountBeforeInsert = children().count();

    bool ok = loadNativeFormat( fileName );
    if ( !ok )
        showLoadingErrorDialog();

    objStartY = 0;

    KPrInsertPageCmd *cmd = new KPrInsertPageCmd( cmdName, _page, _insPos, newpage, this );
    cmd->execute();
    addCommand(cmd);

    _clean = true;
    m_pageWhereLoadObject=0L;
    m_childCountBeforeInsert = 0;
    return _page;
}

void KPresenterDoc::saveOasisPage( const QString &file, int pgnum, bool ignore )
{
    saveOnlyPage = pgnum;
    _duplicatePage=ignore;
    //save in oasis format
    saveNativeFormat( file );
    _duplicatePage=false;
    saveOnlyPage = -1;
}


void KPresenterDoc::savePage( const QString &file, int pgnum, bool ignore )
{
    saveOnlyPage = pgnum;
    _duplicatePage=ignore;
    saveNativeFormat( file );
    _duplicatePage=false;
    saveOnlyPage = -1;
}

void KPresenterDoc::replaceObjs( bool createUndoRedo )
{
    KMacroCommand * macroCmd = 0L;
    QPtrListIterator<KPrPage> oIt(m_pageList);
    for (; oIt.current(); ++oIt )
    {
        KCommand *cmd=oIt.current()->replaceObjs( createUndoRedo, oldGridX,oldGridY,_txtBackCol, _otxtBackCol);
        if(cmd && createUndoRedo)
        {
            if ( !macroCmd)
                macroCmd = new KMacroCommand( i18n("Set New Options") );
            macroCmd->addCommand(cmd);
        }
        else
            delete cmd;
    }

    if(macroCmd)
    {
        macroCmd->execute();
        addCommand(macroCmd);
    }
}

void KPresenterDoc::restoreBackground( KPrPage *page )
{
    page->background()->reload();
}

KCommand * KPresenterDoc::loadPastedObjs( const QString &in, KPrPage* _page )
{
    QDomDocument doc;
    doc.setContent( in );

    QDomElement document=doc.documentElement();

    // DOC
    if (document.tagName()!="DOC") {
        kdError(33001) << "Missing DOC" << endl;
        return 0L;
    }

    bool ok = false;

    if(document.hasAttribute("mime") && document.attribute("mime")=="application/x-kpresenter")
        ok=true;

    if ( !ok )
        return 0L;
    m_pageWhereLoadObject=_page;
    KCommand *cmd = loadObjects(document,true);
    m_pageWhereLoadObject=0L;

    repaint( false );
    setModified( true );
    return cmd;
}

void KPresenterDoc::deSelectAllObj()
{
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
        ((KPresenterView*)it.current())->getCanvas()->deSelectAllObj();
}

void KPresenterDoc::deSelectObj(KPObject *obj)
{
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
        ((KPresenterView*)it.current())->getCanvas()->deSelectObj( obj );
}

void KPresenterDoc::setHeader( bool b )
{
    _header->setDrawEditRect( b );
    _header->setDrawEmpty( b );
    if(!b)
    {
        terminateEditing(_header);
        deSelectObj(_header);
    }
    updateHeaderFooterButton();
    repaint(b);
}

void KPresenterDoc::setFooter( bool b )
{
    _footer->setDrawEditRect( b );
    _footer->setDrawEmpty( b );
    if(!b)
    {
        terminateEditing(_footer);
        deSelectObj(_footer);
    }
    updateHeaderFooterButton();
    repaint(b);
}

void KPresenterDoc::updateHeaderFooterButton()
{
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
        ((KPresenterView*)it.current())->updateHeaderFooterButton();
}

void KPresenterDoc::makeUsedPixmapList()
{
    usedPictures.clear();

    for ( uint i = 0; i < m_pageList.count(); i++ ) {
        if ( saveOnlyPage != -1 &&
             static_cast<int>(i) != saveOnlyPage )
            continue;
        m_pageList.at(i)->makeUsedPixmapList();
    }
    // ### following call independant of saveOnlyPage's value?
    if ( saveOnlyPage == -1 )
        m_masterPage->makeUsedPixmapList();
}

void KPresenterDoc::makeUsedSoundFileList()
{
    if ( saveOnlyPage != -1 )
        return;

    usedSoundFile.clear();

    QPtrListIterator<KPrPage> it( m_pageList );
    for ( ; it.current(); ++it ) {
        QString _file = it.current()->getPageSoundFileName();
        if ( !_file.isEmpty() && usedSoundFile.findIndex( _file ) == -1 )
            usedSoundFile.append( _file );

        QPtrListIterator<KPObject> oIt( it.current()->objectList() );
        for ( ; oIt.current(); ++oIt ) {
            _file = oIt.current()->getAppearSoundEffectFileName();
            if ( !_file.isEmpty() && usedSoundFile.findIndex( _file ) == -1 )
                usedSoundFile.append( _file );

            _file = oIt.current()->getDisappearSoundEffectFileName();
            if ( !_file.isEmpty() && usedSoundFile.findIndex( _file ) == -1 )
                usedSoundFile.append( _file );
        }
    }
}

KoView* KPresenterDoc::createViewInstance( QWidget* parent, const char* name )
{
    //the page numbers have to be recalced for the sticky objects
    //as it could not be done during the constructor of KPresenterView
    recalcPageNum();
    return new KPresenterView( this, parent, name );
}

void KPresenterDoc::paintContent( QPainter& painter, const QRect& rect,
                                  bool /*transparent*/, double zoomX, double zoomY )
{
    m_zoomHandler->setZoom( 100 );
    if ( zoomHandler()->zoomedResolutionX() != zoomX || zoomHandler()->zoomedResolutionY() != zoomY )
    {
        zoomHandler()->setResolution( zoomX, zoomY );
        bool forPrint = painter.device() && painter.device()->devType() == QInternal::Printer;
        newZoomAndResolution( false, forPrint );
    }
    KPrPage *page=m_pageList.first();
    if( m_initialActivePage )
        page=m_initialActivePage;

    int pageNum = m_pageList.findRef( page );

    page->background()->drawBackground( &painter, zoomHandler(), rect, false );
    //for the moment draw first page.
    QPtrListIterator<KPObject> it( page->objectList() );
    for ( ; it.current() ; ++it )
        it.current()->draw( &painter, zoomHandler(), pageNum, SM_NONE );
    it= m_masterPage->objectList();
    //draw sticky obj
    for ( ; it.current() ; ++it )
    {
        if( (it.current()==_header && !page->hasHeader())||(it.current()==_footer && !page->hasFooter()))
            continue;
        it.current()->draw( &painter, zoomHandler(), pageNum, SM_NONE );
    }
}

QPixmap KPresenterDoc::generatePreview( const QSize& size )
{
    int oldZoom = zoomHandler()->zoom();
    double oldResolutionX = zoomHandler()->resolutionX();
    double oldResolutionY = zoomHandler()->resolutionY();

    QPixmap pix = KoDocument::generatePreview(size);

    zoomHandler()->setResolution( oldResolutionX, oldResolutionY );
    zoomHandler()->setZoom(oldZoom);
    newZoomAndResolution( false, false );

    return pix;
}

void KPresenterDoc::movePage( int from, int to )
{
    kdDebug(33001) << "KPresenterDoc::movePage from=" << from << " to=" << to << endl;
    KPrMovePageCmd *cmd = new KPrMovePageCmd( i18n("Move Slide"), from, to, this );
    cmd->execute();
    addCommand(cmd);
}

void KPresenterDoc::copyOasisPage( int from )
{
    _clean = false;
    _duplicatePage=true; // ### now also set via saveOasisPage() parameter below

    kdDebug(33001) << "KPresenterDoc::copyOasisPage from=" << from << " to=" << from + 1 << endl;
    bool wasSelected = isSlideSelected( from );
    KTempFile tempFile( QString::null, ".oop" );
    tempFile.setAutoDelete( true );
    saveOasisPage( tempFile.name(), from, true );

    //insert page.
    KPrPage *newpage = new KPrPage( this, m_masterPage );

    m_pageWhereLoadObject = newpage;

    bool ok = loadNativeFormat( tempFile.name() );
    if ( !ok )
        showLoadingErrorDialog();

    KPrInsertPageCmd *cmd = new KPrInsertPageCmd( i18n("Duplicate Slide"), from, IP_AFTER, newpage, this );
    cmd->execute();
    addCommand(cmd);

    _duplicatePage=false;

    _clean = true;
    m_pageWhereLoadObject=0L;

    selectPage( from + 1, wasSelected );
}

void KPresenterDoc::copyPage( int from )
{
    _clean = false;
    m_childCountBeforeInsert = children().count();

    _duplicatePage=true; // ### now also set via savePage() parameter below

    kdDebug(33001) << "KPresenterDoc::copyPage from=" << from << " to=" << from + 1 << endl;
    bool wasSelected = isSlideSelected( from );
    KTempFile tempFile( QString::null, ".kpr" );
    tempFile.setAutoDelete( true );
    savePage( tempFile.name(), from, true );

    //insert page.
    KPrPage *newpage = new KPrPage( this, m_masterPage );

    m_pageWhereLoadObject = newpage;

    bool ok = loadNativeFormat( tempFile.name() );
    if ( !ok )
        showLoadingErrorDialog();

    KPrInsertPageCmd *cmd = new KPrInsertPageCmd( i18n("Duplicate Slide"), from, IP_AFTER, newpage, this );
    cmd->execute();
    addCommand(cmd);

    _duplicatePage=false;

    _clean = true;
    m_pageWhereLoadObject=0L;
    m_childCountBeforeInsert = 0;

    selectPage( from + 1, wasSelected );
}

void KPresenterDoc::copyOasisPageToClipboard( int pgnum )
{
    // We save the page to a temp file and set the URL of the file in the clipboard
    // Yes it's a hack but at least we don't hit the clipboard size limit :)
    // (and we don't have to implement copy-tar-structure-to-clipboard)
    // In fact it even allows copying a [1-page] kpr in konq and pasting it in kpresenter :))
    kdDebug(33001) << "KPresenterDoc::copyPageToClipboard pgnum=" << pgnum << endl;
    KTempFile tempFile( QString::null, ".oop" );
    saveOasisPage( tempFile.name(), pgnum );
    KURL url; url.setPath( tempFile.name() );
    KURL::List lst;
    lst.append( url );
    QApplication::clipboard()->setData( new KURLDrag( lst ) );
    m_tempFileInClipboard = tempFile.name(); // do this last, the above calls clipboardDataChanged
}

void KPresenterDoc::copyPageToClipboard( int pgnum )
{
    // We save the page to a temp file and set the URL of the file in the clipboard
    // Yes it's a hack but at least we don't hit the clipboard size limit :)
    // (and we don't have to implement copy-tar-structure-to-clipboard)
    // In fact it even allows copying a [1-page] kpr in konq and pasting it in kpresenter :))
    kdDebug(33001) << "KPresenterDoc::copyPageToClipboard pgnum=" << pgnum << endl;
    KTempFile tempFile( QString::null, ".kpr" );
    savePage( tempFile.name(), pgnum, true );
    KURL url; url.setPath( tempFile.name() );
    KURL::List lst;
    lst.append( url );
    QApplication::clipboard()->setData( new KURLDrag( lst ) );
    m_tempFileInClipboard = tempFile.name(); // do this last, the above calls clipboardDataChanged
}

void KPresenterDoc::pastePage( const QMimeSource * data, int pgnum )
{
    KURL::List lst;
    if ( KURLDrag::decode( data, lst ) && !lst.isEmpty() )
    {
        insertNewPage(i18n("Paste Slide"),  pgnum, IP_BEFORE, FALSE, lst.first().path() );
        //selectPage( pgnum, true /* should be part of the file ? */ );
    }
}

void KPresenterDoc::clipboardDataChanged()
{
    if ( !m_tempFileInClipboard.isEmpty() )
    {
        kdDebug(33001) << "KPresenterDoc::clipboardDataChanged, deleting temp file " << m_tempFileInClipboard << endl;
        unlink( QFile::encodeName( m_tempFileInClipboard ) );
        m_tempFileInClipboard = QString::null;
    }
    // TODO enable paste as well, when a txtobject is activated
    // and there is plain text in the clipboard. Then enable this code.
    //QMimeSource *data = QApplication::clipboard()->data();
    //bool canPaste = data->provides( "text/uri-list" ) || data->provides( "application/x-kpresenter-selection" );
    // emit enablePaste( canPaste );
}

void KPresenterDoc::selectPage( int pgNum /* 0-based */, bool select )
{
    Q_ASSERT( pgNum >= 0 );
    KPrPage *page = m_pageList.at( pgNum );
    page->slideSelected(select);
    kdDebug(33001) << "KPresenterDoc::selectPage pgNum=" << pgNum << " select=" << select << endl;
    setModified(true);

    updateSideBarItem( page );
    updatePresentationButton();
    //update statusbar
    emit pageNumChanged();
}

KPrPage * KPresenterDoc::findPage(KPObject *object)
{
    QPtrList<KPObject> masterObjects( m_masterPage->objectList() );
    if ( masterObjects.findRef( object ) != -1 )
    {
        //kdDebug(33001) << "Object is on the master page" << endl;
        return m_masterPage;
    }
    QPtrListIterator<KPrPage> it( m_pageList );
    for ( ; it.current(); ++it ) {
        QPtrList<KPObject> list( it.current()->objectList() );
        if ( list.findRef( object ) != -1 ) {
            //kdDebug(33001) << "Object is on page " << m_pageList.findRef(it.current()) + 1 << endl;
            return it.current();
        }
    }
    kdDebug(33001) << "Object not found on a page" << endl;
    return 0L;
}

KPrPage * KPresenterDoc::findPage(QPtrList<KPObject> &objects)
{
    KPObject *object;
    for ( object = objects.first(); object; object=objects.next() ) {
        QPtrList<KPObject> list( m_masterPage->objectList() );
        if ( list.findRef( object ) != -1 )
        {
            //kdDebug(33001) << "Object is on the master page" << endl;
            return m_masterPage;
        }
    }
    object = objects.first();
    for ( KPrPage *page=m_pageList.first(); page; page=m_pageList.next() ) {
        QPtrList<KPObject> list( page->objectList() );
        if ( list.findRef( object ) != -1 ) {
            //kdDebug(33001) << "The Objects are on page " << m_pageList.findRef(page) + 1 << endl;
            return page;
        }
    }
    kdDebug(33001) << "Objects not found on a page" << endl;
    return 0L;
}

void KPresenterDoc::updateSideBarItem( KPrPage * page )
{
    // Update the views
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
        static_cast<KPresenterView*>( it.current() )->updateSideBarItem( page );
}

bool KPresenterDoc::isSlideSelected( int pgNum /* 0-based */ )
{
    Q_ASSERT( pgNum >= 0 );
    return m_pageList.at(pgNum)->isSlideSelected();
}

QValueList<int> KPresenterDoc::listOfDisplaySelectedSlides( const QValueList<KPrPage*> & lst) /* returned list is 0-based */
{
    QValueList<int> result;
    QValueListConstIterator<KPrPage*> itPage;
    QValueListConstIterator<KPrPage*> itPageEnd = lst.end();
    for( itPage =  lst.begin() ; itPage != itPageEnd; ++itPage )
    {
        int pageNum = m_pageList.find(*itPage );
        if ( pageNum != -1 )
        {
            kdDebug()<<" KPresenterDoc::displaySelectedSlide : add slide number :"<<pageNum<<endl;
            result << pageNum;
        }
    }
    return result;
}


QValueList<int> KPresenterDoc::displaySelectedSlides()  /* returned list is 0-based */
{
    QValueList<int> result;
    if ( m_customListTest )
        return *m_customListTest;
    if ( m_presentationName.isEmpty() )
        return selectedSlides();
    else
    {
        kdDebug()<<" KPresenterDoc::displaySelectedSlide m_presentationName : "<<m_presentationName<<endl;
        result = listOfDisplaySelectedSlides( m_customListSlideShow[m_presentationName]);
    }
    return result;
}

QValueList<int> KPresenterDoc::selectedSlides() /* returned list is 0-based */
{
    QValueList<int> result;
    for ( int i = 0; i < static_cast<int>( m_pageList.count() ); i++ ) {
        if(m_pageList.at(i)->isSlideSelected())
            result <<i;
    }
    return result;
}

QString KPresenterDoc::selectedForPrinting() {
    QString ret;
    int start=-1, end=-1;
    bool continuous=false;
    for ( int i = 0; i < static_cast<int>( m_pageList.count() ); i++ ) {
        if(m_pageList.at(i)->isSlideSelected()) {
            if(continuous)
                ++end;
            else {
                start=i;
                end=i;
                continuous=true;
            }
        }
        else {
            if(continuous) {
                if(start==end)
                    ret+=QString::number(start+1)+",";
                else
                    ret+=QString::number(start+1)+"-"+QString::number(end+1)+",";
                continuous=false;
            }
        }
    }
    if(continuous) {
        if(start==end)
            ret+=QString::number(start+1);
        else
            ret+=QString::number(start+1)+"-"+QString::number(end+1);
    }
    if(','==ret[ret.length()-1])
        ret.truncate(ret.length()-1);
    return ret;
}

void KPresenterDoc::slotRepaintChanged( KPTextObject *kptextobj )
{
    //todo
    //use this function for the moment
    repaint( kptextobj );
}


void KPresenterDoc::recalcVariables( int type )
{
    recalcPageNum();
    m_varColl->recalcVariables(type);
    slotRepaintVariable();
}

void KPresenterDoc::slotRepaintVariable()
{
    QPtrListIterator<KPrPage> it( m_pageList );
    for ( ; it.current(); ++it )
        it.current()->slotRepaintVariable();
    m_masterPage->slotRepaintVariable();
}

void KPresenterDoc::slotDocumentInfoModifed()
{
    if (!getVariableCollection()->variableSetting()->displayFieldCode())
        recalcVariables( VT_FIELD );
}

void KPresenterDoc::reorganizeGUI()
{
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
        ((KPresenterView*)it.current())->reorganize();
}

int KPresenterDoc::undoRedoLimit() const
{
    return m_commandHistory->undoLimit();
}

void KPresenterDoc::setUndoRedoLimit(int val)
{
    m_commandHistory->setUndoLimit(val);
    m_commandHistory->setRedoLimit(val);
}

void KPresenterDoc::updateRuler()
{
    emit sig_updateRuler();
}

void KPresenterDoc::recalcPageNum()
{
    QPtrListIterator<KPrPage> it( m_pageList );
    for ( ; it.current(); ++it )
        it.current()->recalcPageNum();
    m_masterPage->recalcPageNum();
}

KPrPage * KPresenterDoc::activePage()const
{
    return m_initialActivePage;
}

void KPresenterDoc::insertObjectInPage(double offset, KPObject *_obj, int pos)
{
    /// Why does this use __pgLayout instead of m_pageLayout ?
    int page = (int)(offset/__pgLayout.ptHeight)+m_insertFilePage;
    double newPos = offset - ( page - m_insertFilePage ) * __pgLayout.ptHeight;
    // due to a very small caluculating error which gives us the wrong page
    // for objects placed on top of the page we have to move them to the right page.
    if ( __pgLayout.ptHeight - newPos < 1e-6 )
    {
        page++;
        newPos = 0.0;
    }
    if ( page > ( (int)m_pageList.count()-1 ) )
    {
        for (int i=(m_pageList.count()-1); i<page;i++)
            m_pageList.append( new KPrPage( this, m_masterPage ) );
    }
    _obj->setOrig(_obj->getOrig().x(),newPos);

    if ( pos == -1 )
    {
        m_pageList.at(page)->appendObject(_obj);
    }
    else
    {
        m_pageList.at( page )->insertObject( _obj, pos );
    }
}

void KPresenterDoc::insertPixmapKey( KoPictureKey key )
{
    if ( !usedPictures.contains( key ) )
        usedPictures.append( key );
}

KPrPage * KPresenterDoc::initialActivePage() const
{
    return m_initialActivePage;
}

void KPresenterDoc::displayActivePage(KPrPage * _page)
{
    m_initialActivePage = _page;
}

void KPresenterDoc::updateZoomRuler()
{
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
    {
        ((KPresenterView*)it.current())->getHRuler()->setZoom( m_zoomHandler->zoomedResolutionX() );
        ((KPresenterView*)it.current())->getVRuler()->setZoom( m_zoomHandler->zoomedResolutionY() );
        ((KPresenterView*)it.current())->slotUpdateRuler();
    }
}

void KPresenterDoc::newZoomAndResolution( bool updateViews, bool /*forPrint*/ )
{
    QPtrListIterator<KPrPage> it( m_pageList );
    for ( ; it.current(); ++it ) {
        QPtrListIterator<KPObject> oit(it.current()->objectList());
        for ( ; oit.current(); ++oit ) {
            if ( oit.current()->getType() == OT_TEXT )
                static_cast<KPTextObject *>( oit.current() )->textDocument()->formatCollection()->zoomChanged();
        }
    }
    if ( updateViews )
    {
        QPtrListIterator<KoView> it( views() );
        for (; it.current(); ++it )
        {
            static_cast<KPresenterView *>( it.current() )->getCanvas()->update();
            static_cast<KPresenterView *>( it.current() )->getCanvas()->layout();
        }
    }
}

bool KPresenterDoc::isHeader(const KPObject *obj) const
{
    return (obj==_header);
}

bool KPresenterDoc::isFooter(const KPObject *obj) const
{
    return (obj==_footer);
}

bool KPresenterDoc::isHeaderFooter(const KPObject *obj) const
{
    return (obj==_header)||(obj==_footer);
}

void KPresenterDoc::updateRulerPageLayout()
{
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
    {
        ((KPresenterView*)it.current())->getHRuler()->setPageLayout(m_pageLayout );
        ((KPresenterView*)it.current())->getVRuler()->setPageLayout(m_pageLayout );

    }
}

void KPresenterDoc::refreshAllNoteBarMasterPage(const QString &text, KPresenterView *exceptView)
{
    m_masterPage->setNoteText(text );
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
    {
        KPresenterView* view=(KPresenterView*)it.current();
        if ( view->getNoteBar() && view != exceptView && view->editMaster() )
            view->getNoteBar()->setCurrentNoteText(text );
    }
}

void KPresenterDoc::refreshAllNoteBar(int page, const QString &text, KPresenterView *exceptView)
{
    m_pageList.at(page)->setNoteText(text );
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
    {
        KPresenterView* view=(KPresenterView*)it.current();
        if ( view->getNoteBar() && view != exceptView && ((int)(view->getCurrPgNum())-1 == page))
            view->getNoteBar()->setCurrentNoteText(text );
    }
}

void KPresenterDoc::loadStyleTemplates( const QDomElement &stylesElem )
{
    QValueList<QString> followingStyles;

    QDomNodeList listStyles = stylesElem.elementsByTagName( "STYLE" );
    if( listStyles.count() > 0) { // we are going to import at least one style.
        KoParagStyle *s = m_styleColl->findStyle("Standard");
        kdDebug(32001) << "KPresenterDoc::loadStyleTemplates looking for Standard, to delete it. Found " << s << endl;
        if(s) // delete the standard style.
            m_styleColl->removeStyleTemplate(s);
    }
    for (unsigned int item = 0; item < listStyles.count(); item++) {
        QDomElement styleElem = listStyles.item( item ).toElement();

        KoParagStyle *sty = new KoParagStyle( QString::null );
        // Load the style from the <STYLE> element
        sty->loadStyle( styleElem );

        QDomElement formatElem = styleElem.namedItem( "FORMAT" ).toElement();
        if ( !formatElem.isNull() )
            sty->format() = KPTextObject::loadFormat( formatElem, 0L, defaultFont(), globalLanguage(), globalHyphenation() );
        else
            kdWarning(33001) << "No FORMAT tag in <STYLE>" << endl; // This leads to problems in applyStyle().

        // Style created, now let's try to add it
        sty = m_styleColl->addStyleTemplate( sty );
        kdDebug() << k_funcinfo << m_styleColl->styleList().count() << " styles, " << followingStyles.count() << " following styles" << endl;
        if(m_styleColl->styleList().count() > followingStyles.count() )
        {
            QString following = styleElem.namedItem("FOLLOWING").toElement().attribute("name");
            followingStyles.append( following );
        }
        else
            kdWarning (33001) << "Found duplicate style declaration, overwriting former " << sty->name() << endl;
    }

    Q_ASSERT( followingStyles.count() == m_styleColl->styleList().count() );
    unsigned int i=0;
    for( QValueList<QString>::Iterator it = followingStyles.begin(); it != followingStyles.end(); ++it ) {
        KoParagStyle * style = m_styleColl->findStyle(*it);
        m_styleColl->styleAt( i++)->setFollowingStyle( style );
    }
}


void KPresenterDoc::updateAllStyleLists()
{
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
        ((KPresenterView*)it.current())->updateStyleList();
}

void KPresenterDoc::applyStyleChange( KoStyleChangeDefMap changed )
{
    QPtrListIterator<KPrPage> it( m_pageList );
    for ( ; it.current(); ++it )
        it.current()->applyStyleChange( changed );
    m_masterPage->applyStyleChange( changed );
}

void KPresenterDoc::saveStyle( KoParagStyle *sty, QDomElement parentElem )
{
    QDomDocument doc = parentElem.ownerDocument();
    QDomElement styleElem = doc.createElement( "STYLE" );
    parentElem.appendChild( styleElem );

    sty->saveStyle( styleElem );
    QDomElement formatElem = doc.createElement("FORMAT");
    KPTextObject::saveFormat( formatElem, &sty->format() );
    styleElem.appendChild( formatElem );
}

void KPresenterDoc::startBackgroundSpellCheck()
{
    //don't start spell checking when document is embedded in konqueror
    if(backgroundSpellCheckEnabled() && isReadWrite())
    {
        if(m_initialActivePage->allTextObjects().count()>0)
        {
#ifdef HAVE_LIBKSPELL2
            m_bgSpellCheck->start();
#endif
        }
    }
}

void KPresenterDoc::enableBackgroundSpellCheck( bool b )
{
    //m_bgSpellCheck->enableBackgroundSpellCheck(b);
#ifdef HAVE_LIBKSPELL2
    m_bgSpellCheck->setEnabled(b);
#endif
    QPtrListIterator<KoView> it( views() );
    for( ; it.current(); ++it )
        ((KPresenterView*)it.current())->updateBgSpellCheckingState();
}

bool KPresenterDoc::backgroundSpellCheckEnabled() const
{
#ifdef HAVE_LIBKSPELL2
    return m_bgSpellCheck->enabled();
#else
    return false;
#endif
}

void KPresenterDoc::reactivateBgSpellChecking(bool refreshTextObj)
{
    QPtrListIterator<KPrPage> it( m_pageList );
#if 0
    if(m_kpresenterView && m_kpresenterView->getCanvas())
        activePage=m_kpresenterView->getCanvas()->activePage();
#endif
    KPrPage *activePage=m_initialActivePage;
    for ( ; it.current(); ++it )
    {
        if( it.current()!=activePage)
            it.current()->reactivateBgSpellChecking(false );
        else
            it.current()->reactivateBgSpellChecking( true);
    }
    m_masterPage->reactivateBgSpellChecking(refreshTextObj);
    startBackgroundSpellCheck();
}

QPtrList<KoTextObject> KPresenterDoc::allTextObjects() const
{
    QPtrList<KoTextObject> lst;
    QPtrListIterator<KPrPage> it( m_pageList );
    for ( ; it.current(); ++it )
        it.current()->addTextObjects( lst );
    m_masterPage->addTextObjects( lst );
    return lst;
}

QValueList<KoTextObject *> KPresenterDoc::visibleTextObjects( ) const
{
    QValueList<KoTextObject *> lst;
    QPtrList<KoTextObject> textFramesets = allTextObjects(  );

    KoTextObject *frm;
    for ( frm=textFramesets.first(); frm != 0; frm=textFramesets.next() ) {
        if ( frm && !frm->protectContent() )
        {
            lst.append( frm );
        }
    }
    return lst;
}

void KPresenterDoc::setShowHelplines(bool b)
{
    m_bShowHelplines = b;
    setModified( true );
}

void KPresenterDoc::horizHelplines(const QValueList<double> &lines)
{
    m_horizHelplines = lines;
}

void KPresenterDoc::vertHelplines(const QValueList<double> &lines)
{
    m_vertHelplines = lines;
}

int KPresenterDoc::indexOfHorizHelpline(double pos)
{
    int ret = 0;
    for(QValueList<double>::Iterator i = m_horizHelplines.begin(); i != m_horizHelplines.end(); ++i, ++ret)
        if(pos - 4.0 < *i && pos + 4.0 > *i)
            return ret;
    return -1;
}

int KPresenterDoc::indexOfVertHelpline(double pos)
{
    int ret = 0;
    for(QValueList<double>::Iterator i = m_vertHelplines.begin(); i != m_vertHelplines.end(); ++i, ++ret)
        if(pos - 4.0 < *i && pos + 4.0 > *i)
            return ret;
    return -1;
}

void KPresenterDoc::updateHorizHelpline(int idx, double pos)
{
    m_horizHelplines[idx] = pos;
}

void KPresenterDoc::updateVertHelpline(int idx, double pos)
{
    m_vertHelplines[idx] = pos;
}

void KPresenterDoc::addHorizHelpline(double pos)
{
    m_horizHelplines.append(pos);
}

void KPresenterDoc::addVertHelpline(double pos)
{
    m_vertHelplines.append(pos);
}

void KPresenterDoc::removeHorizHelpline(int index)
{
    if ( index >= (int)m_horizHelplines.count())
        kdDebug(33001)<<" index of remove horiz helpline doesn't exist !\n";
    else
        m_horizHelplines.remove(m_horizHelplines[index]);
}

void KPresenterDoc::removeVertHelpline( int index )
{
    if ( index >= (int)m_vertHelplines.count())
        kdDebug(33001)<<" index of remove vertical helpline doesn't exist !\n";
    else
        m_vertHelplines.remove(m_vertHelplines[index]);
}


void KPresenterDoc::updateHelpLineButton()
{
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
        ((KPresenterView*)it.current())->updateHelpLineButton();
}

void KPresenterDoc::loadHelpLines( const QDomElement &element )
{
    // In early versions of KPresenter 1.2 (up to Beta 2), there is child also naed <HELPLINES>
    QDomElement helplines=element.namedItem("HELPLINES").toElement();
    if (helplines.isNull())
        helplines=element;

    helplines=helplines.firstChild().toElement();
    while ( !helplines.isNull() )
    {
        if ( helplines.tagName()=="Vertical" )
            m_vertHelplines.append(helplines.attribute("value").toDouble());
        else if ( helplines.tagName()=="Horizontal" )
            m_horizHelplines.append(helplines.attribute("value").toDouble());
        else if ( helplines.tagName()=="HelpPoint" )
            m_helpPoints.append( KoPoint( helplines.attribute("posX").toDouble(), helplines.attribute("posY").toDouble()));
        helplines=helplines.nextSibling().toElement();
    }
}

void KPresenterDoc::saveHelpLines( QDomDocument &doc, QDomElement& element )
{
    for(QValueList<double>::Iterator it = m_vertHelplines.begin(); it != m_vertHelplines.end(); ++it)
    {
        QDomElement lines=doc.createElement("Vertical");
        lines.setAttribute("value", (double)*it);
        element.appendChild( lines );
    }

    for(QValueList<double>::Iterator it = m_horizHelplines.begin(); it != m_horizHelplines.end(); ++it)
    {
        QDomElement lines=doc.createElement("Horizontal");
        lines.setAttribute("value", *it);
        element.appendChild( lines );
    }

    for(QValueList<KoPoint>::Iterator it = m_helpPoints.begin(); it != m_helpPoints.end(); ++it)
    {
        QDomElement point=doc.createElement("HelpPoint");
        point.setAttribute("posX", (*it).x());
        point.setAttribute("posY", (*it).y());
        element.appendChild( point );
    }
}

void KPresenterDoc::updateGridButton()
{
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
        ((KPresenterView*)it.current())->updateGridButton();

}

void KPresenterDoc::removeHelpPoint( int index )
{
    if ( index >= (int)m_helpPoints.count())
        kdDebug(33001)<<" removeHelpPoint( int index ) : index is bad !\n";
    else
        m_helpPoints.remove(m_helpPoints[index]);
}

void KPresenterDoc::addHelpPoint( const KoPoint &pos )
{
    m_helpPoints.append( pos );
}

void KPresenterDoc::updateHelpPoint( int idx, const KoPoint &pos )
{
    if ( idx >= (int)m_helpPoints.count())
        kdDebug(33001)<<" updateHelpPoint : index is bad !\n";
    else
        m_helpPoints[idx] = pos;
}

int KPresenterDoc::indexOfHelpPoint( const KoPoint &pos )
{
    int ret = 0;
    for(QValueList<KoPoint>::Iterator i = m_helpPoints.begin(); i != m_helpPoints.end(); ++i, ++ret)
        if( ( pos.x() - 4.0 < (*i).x() && pos.x() + 4.0 > (*i).x())
            ||( pos.y() - 4.0 < (*i).y() && pos.y() + 4.0 > (*i).y()))
            return ret;
    return -1;
}

void KPresenterDoc::setSpellCheckIgnoreList( const QStringList& lst )
{
    m_spellCheckIgnoreList = lst;
#ifdef HAVE_LIBKSPELL2
    m_bgSpellCheck->settings()->setCurrentIgnoreList( m_spellCheckIgnoreList );
#endif
    setModified( true );
}

void KPresenterDoc::addSpellCheckIgnoreWord( const QString & word )
{
    // ### missing: undo/redo support
    if( m_spellCheckIgnoreList.findIndex( word ) == -1 )
        m_spellCheckIgnoreList.append( word );
    setSpellCheckIgnoreList( m_spellCheckIgnoreList );
}

void KPresenterDoc::updateObjectStatusBarItem()
{
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
        ((KPresenterView*)it.current())->updateObjectStatusBarItem();
}

void KPresenterDoc::updateObjectSelected()
{
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
        ((KPresenterView*)it.current())->objectSelectedChanged();
}

void KPresenterDoc::setTabStopValue ( double _tabStop )
{
    m_tabStop = _tabStop;
    QPtrListIterator<KPrPage> it( m_pageList );
    for ( ; it.current(); ++it )
        it.current()->changeTabStopValue( m_tabStop );
    //styckypage
    m_masterPage->changeTabStopValue( m_tabStop );
}

void KPresenterDoc::changeBgSpellCheckingState( bool b )
{
    enableBackgroundSpellCheck( b );
    reactivateBgSpellChecking();
    KConfig *config = KPresenterFactory::global()->config();
    config->setGroup("KSpell kpresenter" );
    config->writeEntry( "SpellCheck", (int)b );
}


bool KPresenterDoc::cursorInProtectedArea()const
{
    return m_cursorInProtectectedArea;
}

void KPresenterDoc::setCursorInProtectedArea( bool b )
{
    m_cursorInProtectectedArea=b;
    testAndCloseAllTextObjectProtectedContent();
}

void KPresenterDoc::testAndCloseAllTextObjectProtectedContent()
{
    if ( !m_cursorInProtectectedArea )
    {
        QPtrListIterator<KoView> it( views() );
        for (; it.current(); ++it )
            static_cast<KPresenterView*>(it.current())->testAndCloseAllTextObjectProtectedContent();
    }
}

void KPresenterDoc::insertFile(const QString & file )
{
    m_insertFilePage = m_pageList.count();

    m_childCountBeforeInsert = children().count();
    objStartY = 0;
    bool clean = _clean;
    _clean = false;
    bool ok = loadNativeFormat(file );
    if ( !ok )
    {
        showLoadingErrorDialog();
        return;
    }
    KMacroCommand *macro = 0L;
    for ( int i = m_insertFilePage; i<(int)m_pageList.count();i++)
    {
        if ( !macro )
            macro = new KMacroCommand( i18n("Insert File"));
        KPrInsertPageCmd * cmd = new KPrInsertPageCmd( i18n("Insert File"), i - 1, IP_AFTER, m_pageList.at(i), this ) ;
        macro->addCommand(cmd );
    }
    if ( macro )
        addCommand( macro );

    m_insertFilePage = 0;
    m_childCountBeforeInsert = 0;
    // Update the views
    int newPos = m_pageList.count()-1;
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
        static_cast<KPresenterView*>(it.current())->updateSideBar();
    _clean = clean;
    updatePresentationButton();

    //activate this page in all views (...)
    QPtrListIterator<KoView>it2( views() );
    for (; it2.current(); ++it2 )
        static_cast<KPresenterView*>(it2.current())->skipToPage(newPos);
}

void KPresenterDoc::spellCheckParagraphDeleted( KoTextParag *_parag,  KPTextObject *frm)
{
    //m_bgSpellCheck->spellCheckParagraphDeleted( _parag, frm->textObject());
}

void KPresenterDoc::updateRulerInProtectContentMode()
{
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
        static_cast<KPresenterView*>(it.current())->updateRulerInProtectContentMode();
}

void KPresenterDoc::updatePresentationButton()
{
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
        static_cast<KPresenterView*>(it.current())->updatePresentationButton((selectedSlides().count()>0));
}

void KPresenterDoc::refreshGroupButton()
{
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
        static_cast<KPresenterView*>(it.current())->refreshGroupButton();
}

void KPresenterDoc::addView( KoView *_view )
{
    KoDocument::addView( _view );
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
        static_cast<KPresenterView*>(it.current())->closeTextObject();
}

void KPresenterDoc::removeView( KoView *_view )
{
    KoDocument::removeView( _view );
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
        static_cast<KPresenterView*>(it.current())->deSelectAllObjects();
}

void KPresenterDoc::updateStyleListOrder( const QStringList &list )
{
    styleCollection()->updateStyleListOrder( list );
}

void KPresenterDoc::updateDirectCursorButton()
{
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
        static_cast<KPresenterView*>(it.current())->updateDirectCursorButton();
}

void KPresenterDoc::setInsertDirectCursor(bool _b)
{
    m_bInsertDirectCursor=_b;
    KConfig *config = KPresenterFactory::global()->config();
    config->setGroup( "Interface" );
    config->writeEntry( "InsertDirectCursor", _b );
    updateDirectCursorButton();
}

KPresenterView *KPresenterDoc::firstView() const
{
    if ( views().count()>0)
        return static_cast<KPresenterView*>(views().getFirst());
    else
        return 0L;
}

void KPresenterDoc::addWordToDictionary( const QString & word)
{
#ifdef HAVE_LIBKSPELL2
    if ( m_bgSpellCheck )
    {
        //m_bgSpellCheck->addPersonalDictonary( word );
    }
#endif
}

CustomListMap KPresenterDoc::customListSlideShow()
{
    CustomListMap listMap;
    if ( !m_customListSlideShow.isEmpty() )
    {
        //kdDebug()<<" m_customListSlideShow is not empty\n";
        ListCustomSlideShow::Iterator it;
        for ( it = m_customListSlideShow.begin(); it != m_customListSlideShow.end(); ++it )
        {
            QStringList tmp;
            QValueListIterator<KPrPage*> itPage;
            QValueListIterator<KPrPage*> itPageEnd = ( *it ).end();
            for( itPage = ( *it ).begin() ; itPage != itPageEnd ; ++itPage )
                if ( m_pageList.find(*itPage ) != -1 )
                {
                    //kdDebug()<<" add page :"<<itPage.current()->pageTitle()<<endl;
                    tmp.append( ( *itPage )->pageTitle() );
                }
            listMap.insert( it.key(), tmp);
        }
    }
    return listMap;
}

QValueList <KPrPage *> KPresenterDoc::customListPage( const QStringList & lst, bool loadOasis )
{
    QStringList tmp( lst );
    QValueList <KPrPage *> tmpValueList;
    for ( QStringList::Iterator itList = tmp.begin(); itList != tmp.end(); ++itList )
    {
        for ( int i = 0; i < static_cast<int>( m_pageList.count() ); i++ )
        {
            //kdDebug()<<" insert page name :"<<*itList<<endl;
            if ( loadOasis )
            {
                if ( m_pageList.at( i )->oasisNamePage(i+1)== ( *itList ) )
                {
                    tmpValueList.append(  m_pageList.at( i ) );
                    //kdDebug()<<" really insert\n";
                    break;
                }
            }
            else
            {
                if ( m_pageList.at( i )->pageTitle()== ( *itList ) )
                {
                    tmpValueList.append( m_pageList.at( i ) );
                    //kdDebug()<<" really insert\n";
                    break;
                }
            }

        }
    }
    return tmpValueList;

}

void KPresenterDoc::updateCustomListSlideShow( CustomListMap & map, bool loadOasis )
{
    m_customListSlideShow.clear();
    CustomListMap::Iterator it;
    for ( it = map.begin(); it != map.end(); ++it ) {
        m_customListSlideShow.insert( it.key(), customListPage( it.data(), loadOasis )  );
    }
    setModified( true );
}

QStringList KPresenterDoc::presentationList()
{
    QStringList lst;
    if ( !m_customListSlideShow.isEmpty() )
    {
        ListCustomSlideShow::Iterator it;
        for ( it = m_customListSlideShow.begin(); it != m_customListSlideShow.end(); ++it )
            lst << it.key();
    }
    return lst;
}

void KPresenterDoc::addTestCustomSlideShow( const QStringList &lst, KPresenterView *_view )
{
    delete m_customListTest;
    m_customListTest = new QValueList<int>( listOfDisplaySelectedSlides( customListPage( lst) ) );
    if ( _view )
        _view->screenStartFromFirst();

}

void KPresenterDoc::clearTestCustomSlideShow()
{
    delete m_customListTest;
    m_customListTest = 0L;
}


#include "kpresenter_doc.moc"
