/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2005-2006 Thorsten Zachmann <zachmann@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KPrDocument.h"
#include "KPrView.h"
#include "KPrCanvas.h"
#include "KPrPage.h"
#include "KPrObject.h"
#include "KPrLineObject.h"
#include "KPrRectObject.h"
#include "KPrEllipseObject.h"
#include "KPrAutoformObject.h"
#include "KPrTextObject.h"
#include "KPrTextDocument.h"
#include "KPrPixmapObject.h"
#include "KPrPieObject.h"
#include "KPrPartObject.h"
#include "KPrGroupObject.h"
#include "KPrCommand.h"
#include "insertpagedia.h"
#include "KPrFreehandObject.h"
#include "KPrPolylineObject.h"
#include "KPrBezierCurveObject.h"
#include "KPrPolygonObject.h"
#include "KPrClosedLineObject.h"
#include "KPrSVGPathParser.h"

#include <q3popupmenu.h>
#include <qclipboard.h>
#include <QRegExp>
#include <qfileinfo.h>
#include <qdom.h>
#include <q3dict.h>
//Added by qt3to4:
#include <Q3CString>
#include <Q3PtrList>
#include <Q3ValueList>
#include <QPixmap>

#include <KoDom.h>
#include <KoXmlNS.h>

#include <kurl.h>
#include <kdebug.h>
#include <KoGlobal.h>
#include <kapplication.h>
#include <k3urldrag.h>
#include <ktempfile.h>
#include <klocale.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <kprocess.h>
#include <kio/netaccess.h>
#include <kcommand.h>

#include <KoTemplateChooseDia.h>
#include <KoRuler.h>
#include <KoGenStyles.h>
#include <KoFilterManager.h>
#include <KoStore.h>
#include <KoStoreDevice.h>
#include <KoQueryTrader.h>
#include <KoXmlWriter.h>
#include <KoOasisSettings.h>
#include <KoMainWindow.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <config.h>

#include <KoRichText.h>
#include <KoTextObject.h>
#include <KoTextZoomHandler.h>
#include <KoStyleCollection.h>
#include <kcommand.h>
#include "KPrDocumentIface.h"

#include <kspell2/settings.h>

#include <KoVariable.h>
#include <KoAutoFormat.h>
#include <KoDocumentInfo.h>
#include "KPrVariableCollection.h"
#include "KPrBackground.h"
#include "KPrNoteBar.h"
#include "KPrBgSpellCheck.h"
#include <kglobalsettings.h>
#include <kcommand.h>
#include "KoApplication.h"
#include <KoOasisStyles.h>
#include <KoOasisContext.h>

#include "KPrLoadingInfo.h"

using namespace std;

static const int CURRENT_SYNTAX_VERSION = 2;
// Make sure an appropriate DTD is available in www/koffice/DTD if changing this value
static const char * CURRENT_DTD_VERSION = "1.2";

KPrChild::KPrChild( KPrDocument *_kpr, KoDocument* _doc, const QRect& _rect )
    : KoDocumentChild( _kpr, _doc, _rect )
{
    m_parent = _kpr;
}

KPrChild::KPrChild( KPrDocument *_kpr ) :
    KoDocumentChild( _kpr )
{
    m_parent = _kpr;
}

KPrChild::~KPrChild()
{
}

KoDocument *KPrChild::hitTest( const QPoint &, const QMatrix & )
{
    return 0L;
}

KPrDocument::KPrDocument( QWidget *parentWidget, QObject* parent,
                              bool singleViewMode )
    : KoDocument( parentWidget, parent, singleViewMode ),
      _gradientCollection(), m_customListTest( 0L ),
      m_childCountBeforeInsert( 0 )
{
    setInstance( KPrFactory::global() );
    setTemplateType( "kpresenter_template" );
    //Necessary to define page where we load object otherwise copy-duplicate page doesn't work.
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
    m_styleColl->addStyle( m_standardStyle );

    KConfig *config = KPrFactory::global()->config();
    config->setGroup("Document defaults" );
    QString defaultFontname=config->readEntry("DefaultFont");
    if ( !defaultFontname.isEmpty() )
        m_defaultFont.fromString( defaultFontname );
    // If not found, we automatically fallback to the application font (the one from KControl's font module)

    // Try to force a scalable font.
    m_defaultFont.setStyleStrategy( QFont::ForceOutline );
    //kDebug(33001) << "Default font: requested family: " << m_defaultFont.family() << endl;
    //kDebug(33001) << "Default font: real family: " << QFontInfo(m_defaultFont).family() << endl;

    int ptSize = m_defaultFont.pointSize();
    if ( ptSize == -1 ) // specified with a pixel size ?
        ptSize = QFontInfo(m_defaultFont).pointSize();
    //kDebug(33001) << "KPrDocument::KPrDocument[2] ptSize=" << ptSize << endl;
    // Ok, this is KPresenter. A default font of 10 makes no sense. Let's go for 20.
    ptSize = qMax( 20, ptSize );

    m_standardStyle->format().setFont( m_defaultFont );

    /// KPresenter isn't color-scheme aware, it defaults to black on white.
    m_standardStyle->format().setColor( Qt::black );

    if( config->hasGroup("Interface") ) {
        config->setGroup( "Interface" );
        m_globalLanguage=config->readEntry("language", KGlobal::locale()->language());
        m_bGlobalHyphenation=config->readBoolEntry("hyphenation", false);
    }

    m_standardStyle->format().setLanguage( m_globalLanguage);

    m_zoomHandler = new KoTextZoomHandler;

    m_varFormatCollection = new KoVariableFormatCollection;
    m_varColl = new KPrVariableCollection( new KoVariableSettings(), m_varFormatCollection );
    m_bgSpellCheck = new KPrBgSpellCheck(this);
    dcop = 0;
    m_initialActivePage=0;
    m_bShowStatusBar = true;
    m_autoFormat = new KoAutoFormat(this,m_varColl,m_varFormatCollection);
    _clean = true;
    _spInfiniteLoop = false;
    _spManualSwitch = true;
    _showPresentationDuration = false;
    tmpSoundFileList = Q3PtrList<KTempFile>();
    _xRnd = 20;
    _yRnd = 20;
    _txtBackCol = Qt::lightGray;
    _otxtBackCol = Qt::lightGray;

    m_bShowRuler=true;
    m_bAllowAutoFormat = true;

    m_bViewFormattingChars = false;
    m_bShowGuideLines = true;

    m_bShowGrid = true;

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
    _presPen = QPen( Qt::red, 3, Qt::SolidLine );
    ignoreSticky = true;

    m_gridColor=Qt::black;

    _header = new KPrTextObject( this );
    _header->setDrawEditRect( false );
    _header->setDrawEmpty( false );

    _footer = new KPrTextObject( this );
    _footer->setDrawEditRect( false );
    _footer->setDrawEmpty( false );

    saveOnlyPage = -1;
    m_maxRecentFiles = 10;

    connect( QApplication::clipboard(), SIGNAL( dataChanged() ),
             this, SLOT( clipboardDataChanged() ) );

    m_commandHistory = new KCommandHistory( actionCollection(),  true ) ;
    initConfig();

    connect( m_commandHistory, SIGNAL( documentRestored() ), this, SLOT( slotDocumentRestored() ) );
    connect( m_commandHistory, SIGNAL( commandExecuted() ), this, SLOT( slotCommandExecuted() ) );

    dcopObject();
}

void KPrDocument::refreshMenuCustomVariable()
{
    emit sig_refreshMenuCustomVariable();
}

void KPrDocument::slotDocumentRestored()
{
    setModified( false );
}

void KPrDocument::slotCommandExecuted()
{
    setModified( true );
}

void KPrDocument::saveConfig()
{
    if ( !isReadWrite())
        return;
    KConfigGroup group( KoGlobal::kofficeConfig(), "Spelling" );
    group.writeEntry( "PersonalDict", m_spellCheckPersonalDict );
    if ( !isEmbedded() )
    {
        KConfig *config = KPrFactory::global()->config();
        config->setGroup( "Interface" );
        config->writeEntry( "Zoom", m_zoomHandler->zoom() );
        config->writeEntry( "AllowAutoFormat" , m_bAllowAutoFormat );
        config->writeEntry( "ViewFormattingChars", m_bViewFormattingChars );
        config->writeEntry( "ShowGrid" , m_bShowGrid );
        config->writeEntry( "SnapToGrid" , m_bSnapToGrid );
        config->writeEntry( "ResolutionX", m_gridX );
        config->writeEntry( "ResolutionY", m_gridY );
    }
}

void KPrDocument::initConfig()
{
    int zoom;
    KConfig* config = KPrFactory::global()->config();
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
        setSnapToGrid( config->readBoolEntry( "SnapToGrid", true ));
        setGridX( config->readDoubleNumEntry( "ResolutionX", MM_TO_POINT( 5.0 ) ));
        setGridY( config->readDoubleNumEntry( "ResolutionY", MM_TO_POINT( 5.0 ) ));

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
        if ( isReadWrite() )
          m_bgSpellCheck->setEnabled(config->readBoolEntry( "SpellCheck", false ));
       else
          m_bgSpellCheck->setEnabled( false );
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

    // Load personal dict
    KConfigGroup group( KoGlobal::kofficeConfig(), "Spelling" );
    m_spellCheckPersonalDict = group.readListEntry( "PersonalDict" );

    // Apply configuration, without creating an undo/redo command
    replaceObjs( false );
    zoomHandler()->setZoom( zoom );
    newZoomAndResolution(false,false);
}

DCOPObject* KPrDocument::dcopObject()
{
    if ( !dcop )
        dcop = new KPrDocumentIface( this );

    return dcop;
}

KPrDocument::~KPrDocument()
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
    delete m_bgSpellCheck;
    delete m_styleColl;

    m_pageList.setAutoDelete( true );
    m_pageList.clear();
    m_deletedPageList.setAutoDelete( true );
    m_deletedPageList.clear();
    tmpSoundFileList.setAutoDelete( true );
    tmpSoundFileList.clear();
}

void KPrDocument::addCommand( KCommand * cmd )
{
    kDebug(33001) << "KPrDocument::addCommand " << cmd->name() << endl;
    m_commandHistory->addCommand( cmd, false );
    setModified( true );
}

bool KPrDocument::saveChildren( KoStore* _store )
{
    int i = 0;

    Q3PtrListIterator<KoDocumentChild> it( children() );
    for( ; it.current(); ++it ) {
        // Don't save children that are only in the undo/redo history
        // but not anymore in the presentation
        Q3PtrListIterator<KPrPage> pageIt( m_pageList );
        for ( int pagePos = 0; pageIt.current(); ++pageIt, ++pagePos )
        {
            if ( saveOnlyPage == -1 || pagePos == saveOnlyPage )
            {
                Q3PtrListIterator<KPrObject> oIt(pageIt.current()->objectList());
                for (; oIt.current(); ++oIt )
                {
                    if ( oIt.current()->getType() == OT_PART &&
                         dynamic_cast<KPrPartObject*>( oIt.current() )->getChild() == it.current() )
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
            Q3PtrListIterator<KPrObject> oIt(m_masterPage->objectList());
            for (; oIt.current(); ++oIt )
            {
                if ( oIt.current()->getType() == OT_PART &&
                        dynamic_cast<KPrPartObject*>( oIt.current() )->getChild() == it.current() )
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

QDomDocument KPrDocument::saveXML()
{
    if ( saveOnlyPage == -1 ) {
        emit sigProgress( 0 );
    }

    m_varColl->variableSetting()->setModificationDate(QDateTime::currentDateTime());
    recalcVariables( VT_DATE );
    recalcVariables( VT_TIME );
    recalcVariables( VT_STATISTIC );
    QDomDocument doc = createDomDocument( "DOC", CURRENT_DTD_VERSION );
    QDomElement presenter=doc.documentElement();
    presenter.setAttribute("editor", "KPresenter");
    presenter.setAttribute("mime", "application/x-kpresenter");
    presenter.setAttribute("syntaxVersion", CURRENT_SYNTAX_VERSION);
    QDomElement paper=doc.createElement("PAPER");
    paper.setAttribute("format", static_cast<int>( m_pageLayout.format ));
    paper.setAttribute("ptWidth", QString::number( m_pageLayout.ptWidth, 'g', 10 ));
    paper.setAttribute("ptHeight", QString::number( m_pageLayout.ptHeight, 'g', 10 ));

    paper.setAttribute("orientation", static_cast<int>( m_pageLayout.orientation ));
    paper.setAttribute("unit", unit() );
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
    element.setAttribute("show", static_cast<int>( showGuideLines() ));
    saveGuideLines( doc, element );
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
            QMap<KPrPage *, QString> page2name;
            int pos = 1;
            for ( Q3PtrListIterator<KPrPage> it( m_pageList ); it.current(); ++it )
            {
                page2name.insert( it.current(), "page" + QString::number( pos++ ) ) ;
            }

            element = doc.createElement( "CUSTOMSLIDESHOWCONFIG" );
            CustomSlideShowMap::Iterator it;
            for ( it = m_customListSlideShow.begin(); it != m_customListSlideShow.end(); ++it )
            {
                QDomElement slide=doc.createElement("CUSTOMSLIDESHOW");
                slide.setAttribute("name", it.key() );
                QString tmp;
                Q3ValueListIterator<KPrPage*> itPage ;
                for( itPage = ( *it ).begin(); itPage != ( *it ).end(); ++itPage )
                {
                    int posPage = m_pageList.find( *itPage );
                    if ( posPage != -1 )
                    {
                        if ( itPage != ( *it ).begin() )
                            tmp += ",";
                        tmp += page2name[*itPage];
                    }
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
        Q3ValueList<KoUserStyle *> styleList(m_styleColl->styleList());
        for ( Q3ValueList<KoUserStyle *>::const_iterator it = styleList.begin(), end = styleList.end();
              it != end ; ++it )
            saveStyle( static_cast<KoParagStyle *>( *it ), styles );

        emit sigProgress( 60 );
    }

    // Write "OBJECT" tag for every child
    Q3PtrListIterator<KoDocumentChild> chl( children() );
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

    QDomElement pictures = m_pictureCollection.saveXML( KoPictureCollection::CollectionPicture, doc, usedPictures );
    presenter.appendChild( pictures );

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

void KPrDocument::saveEmbeddedObject(KPrPage *page, const Q3PtrList<KoDocumentChild>& childList,
                                       QDomDocument &doc,QDomElement &presenter )
{
    Q3PtrListIterator<KoDocumentChild> chl( childList );
    double offset = 0.0;
    // we need no offset for objects on the master page and when we copy a page
    if ( m_pageList.findRef( page ) )
    {
        offset=m_pageList.findRef(page)*page->getPageRect().height();
    }
    for( ; chl.current(); ++chl )
        saveEmbeddedObject(page, chl.current(),doc,presenter, offset );
}

void KPrDocument::saveEmbeddedObject(KPrPage *page, KoDocumentChild *chl, QDomDocument &doc,
                                       QDomElement &presenter, double offset )
{
    Q3PtrListIterator<KPrObject> oIt(page->objectList());
    for ( int pos = 0; oIt.current(); ++oIt, ++pos )
    {
        if ( oIt.current()->getType() == OT_PART &&
             static_cast<KPrPartObject*>( oIt.current() )->getChild() == chl )
        {
            QDomElement embedded=doc.createElement("EMBEDDED");
            KPrChild* curr = (KPrChild*)chl;

            // geometry is no zoom value !
            QRect _rect = curr->geometry();
            int tmpX = (int)zoomHandler()->unzoomItXOld( _rect.x() );
            int tmpY = (int)zoomHandler()->unzoomItYOld( _rect.y() );
            int tmpWidth = (int)zoomHandler()->unzoomItXOld( _rect.width() );
            int tmpHeight = (int)zoomHandler()->unzoomItYOld( _rect.height() );
            curr->setGeometry( QRect( tmpX, tmpY, tmpWidth, tmpHeight ) );

            embedded.appendChild(curr->save(doc, true));

            curr->setGeometry( _rect ); // replace zoom value

            QDomElement settings=doc.createElement("SETTINGS");
            settings.setAttribute( "z-index", pos );
            if ( page == m_masterPage )
                settings.setAttribute("sticky", 1 );
            Q3PtrListIterator<KPrObject> setOIt(page->objectList());
            for (; setOIt.current(); ++setOIt )
            {
                if ( setOIt.current()->getType() == OT_PART &&
                     dynamic_cast<KPrPartObject*>( setOIt.current() )->getChild() == curr )
                    settings.appendChild(setOIt.current()->save( doc,offset ));
            }
            embedded.appendChild(settings);
            presenter.appendChild(embedded);
        }
    }

}

//TODO with changes with new file format header/footer can't be change
void KPrDocument::compatibilityFromOldFileFormat()
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

void KPrDocument::enableEmbeddedParts( bool f )
{
    Q3PtrListIterator<KPrPage> it( m_pageList );
    for ( ; it.current(); ++it )
        it.current()->enableEmbeddedParts(f);
}

QDomDocumentFragment KPrDocument::saveBackground( QDomDocument &doc )
{
    QDomDocumentFragment fragment=doc.createDocumentFragment();
    for ( int i = 0; i < static_cast<int>( m_pageList.count() ); i++ ) {
        if ( saveOnlyPage != -1 && i != saveOnlyPage )
            continue;
        fragment.appendChild( m_pageList.at(i)->save( doc ) );
    }
    // save backgound of masterpage only when the complete doc is saved
    if ( saveOnlyPage == -1 )
    {
        fragment.appendChild( m_masterPage->save( doc ) );
    }
    return fragment;
}

QDomElement KPrDocument::saveObjects( QDomDocument &doc )
{
    QDomElement objects=doc.createElement("OBJECTS");
    double yoffset=0.0;
    for ( int i = 0; i < static_cast<int>( m_pageList.count() ); i++ ) {
        if ( saveOnlyPage != -1 && saveOnlyPage!=i)
            continue;
        yoffset=i*m_pageList.at(i)->getPageRect().height(); // yoffset is not zoom value !!
        objects=m_pageList.at(i)->saveObjects( doc, objects, yoffset, saveOnlyPage );

    }
    if ( !_duplicatePage ) //don't copy objects on master slide when we duplicate page
    {
        objects = m_masterPage->saveObjects( doc, objects, 0.0, saveOnlyPage );
    }

    return objects;
}

QDomElement KPrDocument::saveTitle( QDomDocument &doc )
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

QDomElement KPrDocument::saveNote( QDomDocument &doc )
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

QDomElement KPrDocument::saveAttribute( QDomDocument &doc )
{
    QDomElement attributes=doc.createElement("ATTRIBUTES");
    //store first view parameter.
    int activePage=0;

    if ( m_initialActivePage )
        activePage=m_pageList.findRef(m_initialActivePage);
    activePage = qMax( activePage, 0);
    attributes.setAttribute("activePage",activePage );
    attributes.setAttribute("gridx", m_gridX );
    attributes.setAttribute("gridy", m_gridY );
    attributes.setAttribute("snaptogrid", (int)m_bSnapToGrid );
    return attributes;
}

QDomElement KPrDocument::saveUsedSoundFileToXML( QDomDocument &_doc, QStringList _list )
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

bool KPrDocument::completeSaving( KoStore* _store )
{
    if ( !_store ) {
        if ( saveOnlyPage == -1 ) {
            emit sigProgress( 100 );
            emit sigProgress( -1 );
        }
        return true;
    }

    m_pictureCollection.saveToStore( KoPictureCollection::CollectionPicture, _store, usedPictures );

    saveUsedSoundFileToStore( _store, usedSoundFile );

    if ( saveOnlyPage == -1 ) {
        emit sigProgress( 100 );
        emit sigProgress( -1 );
    }

    return true;
}

int KPrDocument::supportedSpecialFormats() const
{
    return KoDocument::supportedSpecialFormats();
}

void KPrDocument::saveUsedSoundFileToStore( KoStore *_store, QStringList _list )
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
            if ( _file.open( QIODevice::ReadOnly ) ) {
                dev.write( ( _file.readAll() ).data(), _file.size() );
                _file.close();
            }
            _store->close();
        }
    }
}

bool KPrDocument::loadChildren( KoStore* _store )
{
    if ( objStartY == 0 && _clean) // Don't do this when inserting a template or a page...
    {
        Q3PtrListIterator<KoDocumentChild> it( children() );
        for( ; it.current(); ++it ) {
            if ( !((KoDocumentChild*)it.current())->loadDocument( _store ) )
                return false;
        }
    }
    else // instead load form the correct child on, m_childCountBeforeInsert has the be set
    {
        Q3PtrListIterator<KoDocumentChild> it( children() );
        for( int i = 0; it.current(); ++it, ++i ) {
            if ( i < m_childCountBeforeInsert )
                continue;
            if ( !((KoDocumentChild*)it.current())->loadDocument( _store ) )
                return false;
        }
    }
    return true;
}

bool KPrDocument::saveOasis( KoStore* store, KoXmlWriter* manifestWriter )
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
    recalcVariables( VT_STATISTIC );
    KoGenStyles mainStyles;
    KoSavingContext savingContext( mainStyles, 0, false, KoSavingContext::Store );

    // Save user styles as KoGenStyle objects
    m_styleColl->saveOasis( mainStyles, KoGenStyle::STYLE_USER, savingContext );

    KTempFile contentTmpFile;
    contentTmpFile.setAutoDelete( true );
    QFile* tmpFile = contentTmpFile.file();
    KoXmlWriter contentTmpWriter( tmpFile, 1 );


    //For sticky objects
    KTempFile stickyTmpFile;
    stickyTmpFile.setAutoDelete( true );
    QFile* masterStyles = stickyTmpFile.file();
    KoXmlWriter stickyTmpWriter( masterStyles, 1 );


    contentTmpWriter.startElement( "office:body" );
    contentTmpWriter.startElement( "office:presentation" );

    saveOasisCustomFied( contentTmpWriter );

    int indexObj = 1;
    int partIndexObj = 0;
//save page

    QMap<QString, int> pageNames;

    if ( !_duplicatePage )
    {
        m_masterPage->saveOasisPage( store, stickyTmpWriter, 0, savingContext, indexObj, partIndexObj, manifestWriter, pageNames );

        // Now mark all autostyles as "for styles.xml" since headers/footers need them
        Q3ValueList<KoGenStyles::NamedStyle> autoStyles = mainStyles.styles(  KoGenStyle::STYLE_AUTO );
        for ( Q3ValueList<KoGenStyles::NamedStyle>::const_iterator it = autoStyles.begin();
                it != autoStyles.end(); ++it ) {
            kDebug() << "marking for styles.xml:" << (  *it ).name << endl;
            mainStyles.markStyleForStylesXml(  ( *it ).name );
        }

    }

    if ( saveOnlyPage != -1 )
    {
        m_pageList.at( saveOnlyPage )->saveOasisPage( store, contentTmpWriter, ( saveOnlyPage+1 ), savingContext, indexObj, partIndexObj , manifestWriter, pageNames );
    }
    else
    {
        for ( int i = 0; i < static_cast<int>( m_pageList.count() ); i++ )
        {
            m_pageList.at( i )->saveOasisPage( store, contentTmpWriter, ( i+1 ), savingContext, indexObj, partIndexObj , manifestWriter, pageNames );
        }
    }
    if ( saveOnlyPage == -1 ) //don't save setting when we save on page
    {
        QMap<int, QString> page2name;
        QMap<QString, int>::ConstIterator it( pageNames.begin() );
        for ( ; it != pageNames.end(); ++it )
        {
            page2name.insert( it.data(), it.key() );
        }
        saveOasisPresentationSettings( contentTmpWriter, page2name );
    }

    contentTmpWriter.endElement(); //office:presentation
    contentTmpWriter.endElement(); //office:body

    writeAutomaticStyles( *contentWriter, mainStyles, savingContext, false );

    // And now we can copy over the contents from the tempfile to the real one
    tmpFile->close();
    contentWriter->addCompleteElement( tmpFile );
    contentTmpFile.close();

    contentWriter->endElement(); // root element
    contentWriter->endDocument();
    delete contentWriter;

    if ( !store->close() ) // done with content.xml
        return false;

    //add manifest line for content.xml
    manifestWriter->addManifestEntry( "content.xml", "text/xml" );

    if ( !store->open( "styles.xml" ) )
        return false;

    manifestWriter->addManifestEntry( "styles.xml", "text/xml" );

    //todo fixme????
    masterStyles->close();
    saveOasisDocumentStyles( store, mainStyles, masterStyles, savingContext );
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

    KoUnit::saveOasis(&settingsWriter, unit());
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

void KPrDocument::saveOasisCustomFied( KoXmlWriter &writer )const
{
    bool customVariableFound = false;
    Q3PtrListIterator<KoVariable> it( m_varColl->getVariables() );
    for ( ; it.current() ; ++it )
    {
        if ( it.current()->type() == VT_CUSTOM )
        {
            if ( !customVariableFound )
            {
                writer.startElement( "text:user-field-decls" );
                customVariableFound = true;
            }
            //<text:user-field-decl office:value-type="string" office:string-value="dfddd" text:name="cvbcbcbx"/>
            writer.startElement( "text:user-field-decl" );
            writer.addAttribute( "office:value-type", "string" );
            writer.addAttribute( "office:string-value", static_cast<KoCustomVariable *>( it.current() )->value() );
            writer.addAttribute( "text:name", static_cast<KoCustomVariable*>( it.current() )->name() );
            writer.endElement();
        }
    }
    if ( customVariableFound )
        writer.endElement();
}


void KPrDocument::loadOasisIgnoreList( const KoOasisSettings& settings )
{
    KoOasisSettings::Items configurationSettings = settings.itemSet( "configuration-settings" );
    if ( !configurationSettings.isNull() )
    {
        const QString ignorelist = configurationSettings.parseConfigItemString( "SpellCheckerIgnoreList" );
        m_spellCheckIgnoreList = QStringList::split( ',', ignorelist );
    }
}

void KPrDocument::writeAutomaticStyles( KoXmlWriter& contentWriter, KoGenStyles& mainStyles, KoSavingContext& context, bool stylesDotXml )
{
    context.writeFontFaces( contentWriter );
    if ( !stylesDotXml )
    {
        contentWriter.startElement( "office:automatic-styles" );
    }
    Q3ValueList<KoGenStyles::NamedStyle> styles = mainStyles.styles( KoGenStyle::STYLE_AUTO, stylesDotXml );
    Q3ValueList<KoGenStyles::NamedStyle>::const_iterator it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( &contentWriter, mainStyles, "style:style", (*it).name, "style:paragraph-properties" );
    }

    styles = mainStyles.styles( KoGenStyle::STYLE_AUTO_LIST, stylesDotXml );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        ( *it ).style->writeStyle( &contentWriter, mainStyles, "text:list-style", (*it).name, 0 );
    }

    styles = mainStyles.styles( STYLE_BACKGROUNDPAGEAUTO, stylesDotXml );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( &contentWriter, mainStyles, "style:style", (*it).name, "style:drawing-page-properties" );
    }

    styles = mainStyles.styles( KoGenStyle::STYLE_GRAPHICAUTO, stylesDotXml );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( &contentWriter, mainStyles, "style:style", (*it).name , "style:graphic-properties"  );
    }

    styles = mainStyles.styles( KoGenStyle::STYLE_NUMERIC_DATE, stylesDotXml );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( &contentWriter, mainStyles, "number:date-style", (*it).name, 0 /*TODO ????*/  );
    }
    styles = mainStyles.styles( KoGenStyle::STYLE_NUMERIC_TIME, stylesDotXml );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( &contentWriter, mainStyles, "number:time-style", (*it).name, 0 /*TODO ????*/  );
    }

    if ( !stylesDotXml )
    {
        contentWriter.endElement(); // office:automatic-styles
    }
}

void KPrDocument::loadOasisHeaderFooter(QDomNode & drawPage, KoOasisContext & context)
{
    QDomNode tmp = KoDom::namedItemNS( drawPage, KoXmlNS::style, "header" );
    if ( !tmp.isNull() )
    {
        //kDebug()<<" there is a header \n";
        _header->textObject()->loadOasisContent( tmp.toElement(), context, styleCollection() );
    }
    tmp = KoDom::namedItemNS( drawPage, KoXmlNS::style, "footer" );
    if ( !tmp.isNull() )
    {
        //kDebug()<<" there is a footer \n";
        _footer->textObject()->loadOasisContent( tmp.toElement(), context, styleCollection() );
    }
}

void KPrDocument::saveOasisSettings( KoXmlWriter &settingsWriter )
{
    settingsWriter.startElement("config:config-item-map-indexed");
    settingsWriter.addAttribute("config:name", "Views");
    settingsWriter.startElement( "config:config-item-map-entry" );

    //ooimpress save it as this line.
    //<config:config-item config:name="SnapLinesDrawing" config:type="string">H2260V14397H7693H12415H15345H1424</config:config-item>
    QString guideLinesOasis;
    //save in mm as in oo
    for( Q3ValueList<double>::Iterator it = m_vGuideLines.begin(); it != m_vGuideLines.end(); ++it )
    {
        int tmpX = ( int ) ( KoUnit::toMM( *it  )*100 );
        guideLinesOasis += "V" + QString::number( tmpX );
    }

    for( Q3ValueList<double>::Iterator it = m_hGuideLines.begin(); it != m_hGuideLines.end(); ++it )
    {
        int tmpY = ( int ) ( KoUnit::toMM( *it  )*100 );
        guideLinesOasis += "H" + QString::number( tmpY );
    }
    if ( !guideLinesOasis.isEmpty() )
    {
        settingsWriter.addConfigItem( "SnapLinesDrawing", guideLinesOasis );
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
    activePage = qMax( activePage, 0);
    settingsWriter.addConfigItem( "SelectedPage", activePage );

    //not define into oo spec
    settingsWriter.addConfigItem( "SnapLineIsVisible", showGuideLines() );
    settingsWriter.endElement();
    settingsWriter.endElement();
}

void KPrDocument::loadOasisSettings(const QDomDocument&settingsDoc)
{
    kDebug(33001)<<"void KPrDocument::loadOasisSettings(const QDomDocument&settingsDoc)**********\n";
    KoOasisSettings settings( settingsDoc );
    KoOasisSettings::Items viewSettings = settings.itemSet( "view-settings" );
    setUnit(KoUnit::unit(viewSettings.parseConfigItemString("unit")));
    KoOasisSettings::IndexedMap viewMap = viewSettings.indexedMap( "Views" );
    KoOasisSettings::Items firstView = viewMap.entry( 0 );
    if ( !firstView.isNull() )
    {
        parseOasisGuideLines( firstView.parseConfigItemString( "SnapLinesDrawing" ) );
        setShowGuideLines( firstView.parseConfigItemBool( "SnapLineIsVisible" ) );
        int valx = firstView.parseConfigItemInt( "GridFineWidth" );
        m_gridX = MM_TO_POINT( valx / 100.0 );
        int valy = firstView.parseConfigItemInt( "GridFineHeight" );
        m_gridY = MM_TO_POINT( valy / 100.0 );

        m_bSnapToGrid = firstView.parseConfigItemBool( "IsSnapToGrid" );

        int activePage = firstView.parseConfigItemInt( "SelectedPage" );
        kDebug(33001)<<" activePage :"<<activePage<<endl;
        if(activePage!=-1)
            m_initialActivePage=m_pageList.at(activePage);
    }
    loadOasisIgnoreList( settings );
    m_varColl->variableSetting()->loadOasis( settings );
}

void KPrDocument::parseOasisGuideLines( const QString &text )
{
    QString str;
    int newPos = text.length()-1; //start to element = 1
    for ( int pos = text.length()-1; pos >=0;--pos )
    {
        if ( text[pos]=='V' )
        {
            //vertical element
            str = text.mid( pos+1, ( newPos-pos ) );
            //kDebug()<<" vertical  :"<< str <<endl;
            double posX = ( str.toInt() / 100.0 );
            m_vGuideLines.append( MM_TO_POINT( posX ) );
            newPos = pos-1;
        }
        else if ( text[pos]=='H' )
        {
            //horizontal element
            str = text.mid( pos+1, ( newPos-pos ) );
            //kDebug()<<" horizontal  :"<< str <<endl;
            double posY = ( str.toInt() / 100.0 );
            m_hGuideLines.append( MM_TO_POINT( posY ) );
            newPos = pos-1;
        }
    }
}

void KPrDocument::loadOasisPresentationSettings( QDomNode &settingsDoc )
{
    //kDebug()<<"presentation:settings ********************************************* \n";
    QDomElement settings( settingsDoc.toElement() );
    //kDebug()<<"settings.attribute(presentation:endless) :"<<settings.attributeNS( KoXmlNS::presentation, "endless", QString::null)<<endl;
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
        kDebug()<<" default presentation name :"<<m_presentationName<<endl;
    }
    loadOasisPresentationCustomSlideShow( settingsDoc );
}

void KPrDocument::loadOasisPresentationCustomSlideShow( QDomNode &settingsDoc )
{
    //kDebug()<<"void KPrDocument::loadOasisPresentationCustomSlideShow( QDomNode &settingsDoc )**********\n";
    for ( QDomNode element = settingsDoc.firstChild(); !element.isNull(); element = element.nextSibling() )
    {
        QDomElement e = element.toElement();
        Q3CString tagName = e.tagName().toLatin1();
        //kDebug()<<" tagName :"<<tagName<<endl;
        if ( tagName == "show" && e.namespaceURI() == KoXmlNS::presentation )
        {
            //kDebug()<<" e.attribute(presentation:name) :"<<e.attributeNS( KoXmlNS::presentation, "name", QString::null)<< " e.attribute(presentation:pages) :"<<e.attributeNS( KoXmlNS::presentation, "pages", QString::null)<<endl;
            QString name = e.attributeNS( KoXmlNS::presentation, "name", QString::null );
            QStringList tmp = QStringList::split( ",", e.attributeNS( KoXmlNS::presentation, "pages", QString::null) );
            Q3ValueList<KPrPage *> pageList;
            for ( QStringList::Iterator it = tmp.begin(); it != tmp.end(); ++it )
            {
                if ( m_loadingInfo->m_name2page.contains( *it ) )
                {
                    kDebug(33001) << "slide show " << name << " page = " << *it << endl;
                    pageList.push_back( m_loadingInfo->m_name2page[*it] );
                }
            }
            if ( ! pageList.empty() )
            {
                m_customListSlideShow.insert( name, pageList );
            }
        }
    }
}

void KPrDocument::saveOasisPresentationSettings( KoXmlWriter &contentTmpWriter, QMap<int, QString> &page2name )
{
    //todo don't save when is not value by default (check with oo)
    //FIXME
    contentTmpWriter.startElement( "presentation:settings" );
    contentTmpWriter.addAttribute( "presentation:endless",  ( _spInfiniteLoop ? "true" : "false" ) );
    contentTmpWriter.addAttribute( "presentation:force-manual",  ( _spManualSwitch ? "true" : "false" ) );
    //add for default presentation
    if ( !m_presentationName.isEmpty() )
        contentTmpWriter.addAttribute( "presentation:show",  m_presentationName );

    saveOasisPresentationCustomSlideShow( contentTmpWriter, page2name );
    contentTmpWriter.endElement();
}

void KPrDocument::saveOasisPresentationCustomSlideShow( KoXmlWriter &contentTmpWriter, QMap<int, QString> &page2name )
{
    if ( m_customListSlideShow.isEmpty() )
        return;

    CustomSlideShowMap::Iterator it;
    for ( it = m_customListSlideShow.begin(); it != m_customListSlideShow.end(); ++it )
    {
        contentTmpWriter.startElement( "presentation:show" );
        contentTmpWriter.addAttribute( "presentation:name", it.key() );
        QString tmp;
        Q3ValueListIterator<KPrPage*> itPage ;
        for( itPage = ( *it ).begin(); itPage != ( *it ).end(); ++itPage )
        {
            int posPage = m_pageList.find(*itPage );
            if ( posPage != -1 )
            {
                if ( itPage != ( *it ).begin() )
                    tmp += ",";
                //tmp+=( *itPage )->oasisNamePage(posPage+1)+",";
                tmp += page2name[posPage + 1];
            }
        }
        contentTmpWriter.addAttribute( "presentation:pages", tmp );
        contentTmpWriter.endElement();
    }
    //<presentation:show presentation:name="New Custom Slide Show" presentation:pages="page1,page1,page1,page1,page1"/>
}

void KPrDocument::saveOasisDocumentStyles( KoStore* store, KoGenStyles& mainStyles, QFile* masterStyles,
                                           KoSavingContext & savingContext, SaveFlag saveFlag ) const
{
    KoStoreDevice stylesDev( store );
    KoXmlWriter* stylesWriter = createOasisXmlWriter( &stylesDev, "office:document-styles" );

    stylesWriter->startElement( "office:styles" );
    Q3ValueList<KoGenStyles::NamedStyle> styles = mainStyles.styles( KoGenStyle::STYLE_USER );
    Q3ValueList<KoGenStyles::NamedStyle>::const_iterator it = styles.begin();
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

    if ( saveFlag == SaveAll )
    {
        stylesWriter->startElement( "office:automatic-styles" );
        // this has to be the first
        if ( masterStyles )
        {
            writeAutomaticStyles( *stylesWriter, mainStyles, savingContext, true );
        }

        styles = mainStyles.styles( STYLE_BACKGROUNDPAGE );
        it = styles.begin();
        for ( ; it != styles.end() ; ++it ) {
            (*it).style->writeStyle( stylesWriter, mainStyles, "style:style", (*it).name , "style:drawing-page-properties"  );
        }

        // if there's more than one pagemaster we need to rethink all this
        styles = mainStyles.styles( KoGenStyle::STYLE_PAGELAYOUT );
        Q_ASSERT( styles.count() == 1 );
        it = styles.begin();
        for ( ; it != styles.end() ; ++it ) {
            (*it).style->writeStyle( stylesWriter, mainStyles, "style:page-layout", (*it).name, "style:page-layout-properties", false /*don't close*/ );
            stylesWriter->endElement();
        }

        styles = mainStyles.styles( STYLE_PRESENTATIONSTICKYOBJECT );
        it = styles.begin();
        for ( ; it != styles.end() ; ++it ) {
            //TODO fix me graphic-properties ???
            (*it).style->writeStyle( stylesWriter, mainStyles, "style:style", (*it).name , "style:graphic-properties"  );
        }

        stylesWriter->endElement(); // office:automatic-styles


        if ( masterStyles )
        {
            stylesWriter->startElement( "office:master-styles" );
            stylesWriter->addCompleteElement( masterStyles );
            stylesWriter->endElement();
        }
    }

    stylesWriter->endElement(); // root element (office:document-styles)
    stylesWriter->endDocument();
    delete stylesWriter;
}

bool KPrDocument::loadOasis( const QDomDocument& doc, KoOasisStyles&oasisStyles, const QDomDocument&settingsDoc, KoStore*store )
{
    QTime dt;
    dt.start();
    m_loadingInfo = new KPrLoadingInfo;
    ignoreSticky = false;
    emit sigProgress( 0 );

    lastObj = -1;
    // clean
    if ( _clean ) {
        m_styleColl->clear();
        // Some simple import filters don't define any style,
        // so let's have a Standard style at least
        KoParagStyle * standardStyle = new KoParagStyle( "Standard" ); // This gets translated later on
        //kDebug() << "KWDocument::KWDocument creating standardStyle " << standardStyle << endl;
        standardStyle->format().setFont( m_defaultFont );
        m_styleColl->addStyle( standardStyle );

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
    QDomElement realBody (KoDom::namedItemNS( content, KoXmlNS::office, "body" ) );
    if ( realBody.isNull() )
    {
        kError(33001) << "No office:body found!" << endl;
        setErrorMessage( i18n( "Invalid OASIS OpenDocument file. No office:body tag found." ) );
        return false;
    }
    QDomElement body = KoDom::namedItemNS( realBody, KoXmlNS::office, "presentation" );
    if ( body.isNull() )
    {
        kError(33001) << "No office:presentation found!" << endl;
        QDomElement childElem;
        QString localName;
        forEachElement( childElem, realBody ) {
            localName = childElem.localName();
        }
        if ( localName.isEmpty() )
            setErrorMessage( i18n( "Invalid OASIS OpenDocument file. No tag found inside office:body." ) );
        else
            setErrorMessage( i18n( "This document is not a presentation, but a %1. Please try opening it with the appropriate application.", KoDocument::tagNameToDocumentType( localName ) ) );
        return false;
    }

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
    m_styleColl->loadOasisStyles( context );

    // if we only copy a page we have no master
    // also don't copy master when you insert file as long as we don't have multiple masters
    if ( !m_pageWhereLoadObject && _clean )
    {
        QString masterPageName = drawPage.toElement().attributeNS( KoXmlNS::draw, "master-page-name", QString::null );
        QDomElement *master = oasisStyles.masterPages()[ masterPageName];

        kDebug()<<" master :"<<master<<endl;
        kDebug()<<" masterPageName:"<<masterPageName<<endl;
        if ( ! master )
        {
            masterPageName = "Standard"; // use default layout as fallback (default in kpresenter)
            master = oasisStyles.masterPages()[ masterPageName];
            if ( !master ) //last test...
                master = oasisStyles.masterPages()["Default"];
        }

        if ( master == 0 )
        {
            setErrorMessage( i18n( "Invalid OASIS OpenDocument file. No master-style found inside office:master-styles." ) );
            return false;
        }

        kDebug()<<" load oasis master styles\n";
        QDomNode node = *master;
        QDomElement masterElement = node.toElement();
        kDebug()<<" node.isNull() :"<<node.isNull()<< ", " << masterElement.attributeNS( KoXmlNS::draw, "style-name", QString::null ) << endl;
        // add the correct styles
        const QDomElement* masterPageStyle = context.oasisStyles().findStyleAutoStyle( masterElement.attributeNS( KoXmlNS::draw, "style-name", QString::null ), "drawing-page" );
        context.styleStack().push( *masterPageStyle );

        context.setUseStylesAutoStyles( true );
        m_masterPage->loadOasis( context );

        createPresentationAnimation( KoDom::namedItemNS( node, KoXmlNS::presentation, "animations"));

        loadOasisObject( m_masterPage, node , context);

        m_loadingInfo->clearAnimationShowDict(); // clear all show animations style
        m_loadingInfo->clearAnimationHideDict(); // clear all hide animations style

        loadOasisHeaderFooter( node,context );
        context.setUseStylesAutoStyles( false );

        kDebug()<<" end load oasis master style \n";

        Q_ASSERT( master );
        const QDomElement *style = master ? oasisStyles.findStyle(master->attributeNS( KoXmlNS::style, "page-layout-name", QString::null )) : 0;
        const QDomElement *backgroundStyle = oasisStyles.findStyle( "Standard-background", "presentation" );
        kDebug()<<"Standard background "<<backgroundStyle<<endl;
        // parse all pages
        Q_ASSERT( style );
        if ( style )
        {
            __pgLayout.loadOasis( *style );
            kDebug()<<"Page size __pgLayout.ptWidth :"<<__pgLayout.ptWidth<<" __pgLayout.ptHeight :"<<__pgLayout.ptHeight<<endl;
            kDebug()<<"Page orientation :"<<(( __pgLayout.orientation== PG_LANDSCAPE )? " landscape " : " portrait ")<<endl;

            kDebug()<<" margin right:"<< __pgLayout.ptRight <<" __pgLayout.ptBottom :"<<__pgLayout.ptBottom<<" __pgLayout.ptLeft :"<<__pgLayout.ptLeft<<" __pgLayout.ptTop :"<<__pgLayout.ptTop<<endl;
        }
        if ( _clean )
        {
            /// ### this has already been done, no?
            setPageLayout( __pgLayout );
        }
    }

    int pos = m_insertFilePage;
    for ( drawPage = body.firstChild(); !drawPage.isNull(); drawPage = drawPage.nextSibling() )
    {
        dp = drawPage.toElement();
        kDebug()<<"dp.tagName() :"<<dp.tagName()<<endl;
        if ( dp.tagName()== "page" && dp.namespaceURI() == KoXmlNS::draw ) // don't try to parse "</draw:page>" as page
        {
            context.styleStack().clear(); // remove all styles
            fillStyleStack( dp, context, "drawing-page" );
            context.styleStack().save();
            kDebug ()<<"insert new page "<<pos<<endl;
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
                else //we create a first page into KPrDocument()
                {
                    newpage = m_pageList.at(pos);
                }
            }
            //only set the manual title if it is different to the draw:id. Only in this case it had one.
            QString str = dp.attributeNS( KoXmlNS::draw, "name", QString::null );
            m_loadingInfo->m_name2page.insert( str, newpage );
            QString idPage = dp.attributeNS( KoXmlNS::draw, "id", QString::null );

            if ( dp.hasAttributeNS( KoXmlNS::koffice, "name" ) )
            {
                str = dp.attributeNS( KoXmlNS::koffice, "name", QString::null );
                newpage->insertManualTitle(str);
            }
            else
            {
                // OO uses /page[0-9]+$/ as default for no name set
                QRegExp rx( "^page[0-9]+$" );
                if ( rx.search( str ) == -1 )
                    newpage->insertManualTitle(str);
            }
            context.styleStack().setTypeProperties( "drawing-page" );

            newpage->loadOasis( context );

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

    //load settings at the end as we need to know what the draw:name of a page is
    QDomNode settings  = KoDom::namedItemNS( body, KoXmlNS::presentation, "settings" );
    kDebug()<<"settings :"<<settings.isNull()<<endl;
    if (!settings.isNull() && _clean /*don't load settings when we copy/paste a page*/)
        loadOasisPresentationSettings( settings );

    ignoreSticky = true;
    kDebug()<<" _clean :"<<_clean<<endl;
    if(_clean)
    {
        setModified(false);
#if 0   //FIXME
        //it crashed, I don't know why for the moment.
        startBackgroundSpellCheck();
#endif
    }
    kDebug(33001) << "Loading took " << (float)(dt.elapsed()) / 1000.0 << " seconds" << endl;

    if ( !settingsDoc.isNull() )
    {
        loadOasisSettings( settingsDoc );
    }

    // set the initial active page
    if ( m_initialActivePage == 0 )
    {
        m_initialActivePage = m_pageList.at( 0 );
    }

    emit sigProgress( 100 );
    recalcVariables( VT_FIELD );
    recalcVariables( VT_STATISTIC );
    emit sigProgress( -1 );

    setModified( false );
    return true;
}


void KPrDocument::loadOasisObject( KPrPage * newpage, QDomNode & drawPage, KoOasisContext & context, KPrGroupObject *groupObject )
{
    for ( QDomNode object = drawPage.firstChild(); !object.isNull(); object = object.nextSibling() )
    {
        QDomElement o = object.toElement();
        QString name = o.tagName();
        if ( !name.isEmpty() )
        {
            kDebug()<<" name :"<<name<<endl;
            if ( o.hasAttributeNS( KoXmlNS::presentation, "placeholder" ) &&
                 o.attributeNS( KoXmlNS::presentation, "placeholder", QString::null ) == "true" )
            {
                kDebug(33001) << "Placeholder" << endl;
                continue;
            }
            context.styleStack().save();
            const bool isDrawNS = o.namespaceURI() == KoXmlNS::draw;
            // draw:frame
            if ( name == "frame" && isDrawNS )
            {
                fillStyleStack( o, context, "graphic" );
                QDomElement elem;
                forEachElement( elem, o )
                {
                    if ( elem.namespaceURI() != KoXmlNS::draw )
                        continue;
                    const QString localName = elem.localName();
                    if ( localName == "text-box" )
                    {
                        KPrTextObject *kptextobject = new KPrTextObject( this );
                        kptextobject->loadOasis(o, context, m_loadingInfo);
                        if ( groupObject )
                            groupObject->addObjects( kptextobject );
                        else
                            newpage->appendObject(kptextobject);
                        break;
                    }
                    else if ( localName == "image" )
                    {
                        KPrPixmapObject *kppixmapobject = new KPrPixmapObject( pictureCollection() );
                        kppixmapobject->loadOasis( o, context, m_loadingInfo);
                        if ( groupObject )
                            groupObject->addObjects( kppixmapobject );
                        else
                            newpage->appendObject(kppixmapobject);
                        break;
                    }
                    else if ( localName == "object" )
                    {
                        KPrChild *ch = new KPrChild( this );
                        QRect r;
                        KPrPartObject *kppartobject = new KPrPartObject( ch );
                        kppartobject->loadOasis( o, context, m_loadingInfo );
                        r = ch->geometry();
                        if ( groupObject )
                            groupObject->addObjects( kppartobject );
                        else
                            newpage->appendObject(kppartobject);
                        insertChild( ch );
                        kppartobject->setOrig( r.x(), r.y() );
                        kppartobject->setSize( r.width(), r.height() );
                        break;
                    }
                }
            }
            else if ( name == "rect" && isDrawNS) // rectangle
            {
                fillStyleStack( o, context, "graphic" );
                KPrRectObject *kprectobject = new KPrRectObject();
                kprectobject->loadOasis(o, context , m_loadingInfo);
                if ( groupObject )
                    groupObject->addObjects( kprectobject );
                else
                    newpage->appendObject(kprectobject);
            }
            else if ( ( name == "circle" || name == "ellipse" )&& isDrawNS)
            {
                fillStyleStack( o, context, "graphic" );
                if ( o.hasAttributeNS( KoXmlNS::draw, "kind" ) ) // pie, chord or arc
                {
                    KPrPieObject *kppieobject = new KPrPieObject();
                    kppieobject->loadOasis(o, context, m_loadingInfo);
                    if ( groupObject )
                        groupObject->addObjects( kppieobject );
                    else
                        newpage->appendObject(kppieobject);
                }
                else  // circle or ellipse
                {
                    KPrEllipseObject *kpellipseobject = new KPrEllipseObject();
                    kpellipseobject->loadOasis(o,context, m_loadingInfo);
                    if ( groupObject )
                        groupObject->addObjects( kpellipseobject );
                    else
                        newpage->appendObject(kpellipseobject);
                }
            }
            else if ( name == "line" && isDrawNS) // line
            {
                fillStyleStack( o, context, "graphic" );
                KPrLineObject *kplineobject = new KPrLineObject();
                kplineobject->loadOasis(o, context, m_loadingInfo);
                if ( groupObject )
                    groupObject->addObjects( kplineobject );
                else
                    newpage->appendObject( kplineobject );
            }
            else if (name=="polyline" && isDrawNS) { // polyline
                fillStyleStack( o, context, "graphic" );
                KPrPolylineObject *kppolylineobject = new KPrPolylineObject();
                kppolylineobject->loadOasis(o, context, m_loadingInfo);
                if ( groupObject )
                    groupObject->addObjects( kppolylineobject );
                else
                    newpage->appendObject(kppolylineobject);
            }
            else if (name=="polygon" && isDrawNS) { // plcloseobject
                fillStyleStack( o, context, "graphic" );
                KPrClosedLineObject *kpClosedObject = new KPrClosedLineObject();
                kpClosedObject->loadOasis( o, context, m_loadingInfo);
                if ( groupObject )
                    groupObject->addObjects( kpClosedObject );
                else
                    newpage->appendObject(kpClosedObject);
            }
            else if (name=="regular-polygon"&& isDrawNS) { // kppolygone object
                fillStyleStack( o, context, "graphic" );
                KPrPolygonObject *kpPolygoneObject = new KPrPolygonObject();
                kpPolygoneObject->loadOasis( o, context, m_loadingInfo);
                if ( groupObject )
                    groupObject->addObjects( kpPolygoneObject );
                else
                    newpage->appendObject(kpPolygoneObject);
            }
            else if ( name == "path" && isDrawNS)
            {
                fillStyleStack( o, context, "graphic" );
                QString d = o.attributeNS( KoXmlNS::svg, "d", QString::null);

                KPrSVGPathParser parser;
                ObjType objType = parser.getType( d );

                switch ( objType )
                {
                    case OT_CUBICBEZIERCURVE:
                        {
                            kDebug(33001) << "Cubicbeziercurve" << endl;
                            KPrCubicBezierCurveObject *kpCurveObject = new KPrCubicBezierCurveObject();
                            kpCurveObject->loadOasis( o, context, m_loadingInfo );
                            if ( groupObject )
                                groupObject->addObjects( kpCurveObject );
                            else
                                newpage->appendObject( kpCurveObject );
                        } break;
                    case OT_QUADRICBEZIERCURVE:
                        {
                            kDebug(33001) << "Quadricbeziercurve" << endl;
                            KPrQuadricBezierCurveObject *kpQuadricObject = new KPrQuadricBezierCurveObject();
                            kpQuadricObject->loadOasis( o, context, m_loadingInfo );
                            if ( groupObject )
                                groupObject->addObjects( kpQuadricObject );
                            else
                                newpage->appendObject( kpQuadricObject );
                        } break;
                    case OT_FREEHAND:
                        {
                            kDebug(33001) << "Freehand" << endl;
                            KPrFreehandObject *kpFreeHandObject = new KPrFreehandObject();
                            kpFreeHandObject->loadOasis( o, context, m_loadingInfo );
                            if ( groupObject )
                                groupObject->addObjects( kpFreeHandObject );
                            else
                                newpage->appendObject( kpFreeHandObject );
                        } break;
                    case OT_CLOSED_LINE:
                        {
                            kDebug(33001) << "Closed Line" << endl;
                            KPrClosedLineObject *kpClosedObject = new KPrClosedLineObject();
                            kpClosedObject->loadOasis( o, context, m_loadingInfo );
                            if ( groupObject )
                                groupObject->addObjects( kpClosedObject );
                            else
                                newpage->appendObject( kpClosedObject );
                        } break;
                    default:
                        kDebug(33001) << "draw:path found unsupported object type " << objType << " in svg:d " << d << endl;
                        break;
                }
            }
            else if ( name == "custom-shape" && isDrawNS )
            {
                fillStyleStack( o, context, "graphic" );

                QDomElement enhancedGeometry = KoDom::namedItemNS( o, KoXmlNS::draw, "enhanced-geometry" );

                if ( !enhancedGeometry.isNull() )
                {
                    QString d = enhancedGeometry.attributeNS( KoXmlNS::draw, "enhanced-path", QString::null );
                    QRegExp rx( "^([0-9 ML]+Z) N$" );
                    if ( rx.search( d ) != -1 )
                    {
                        d = rx.cap( 1 );
                        KPrSVGPathParser parser;
                        ObjType objType = parser.getType( d );

                        switch ( objType )
                        {
#if 0 // not yet supported
                            case OT_CUBICBEZIERCURVE:
                                {
                                    kDebug(33001) << "Cubicbeziercurve" << endl;
                                    KPrCubicBezierCurveObject *kpCurveObject = new KPrCubicBezierCurveObject();
                                    kpCurveObject->loadOasis( o, context, m_loadingInfo );
                                    if ( groupObject )
                                        groupObject->addObjects( kpCurveObject );
                                    else
                                        newpage->appendObject( kpCurveObject );
                                } break;
                            case OT_QUADRICBEZIERCURVE:
                                {
                                    kDebug(33001) << "Quadricbeziercurve" << endl;
                                    KPrQuadricBezierCurveObject *kpQuadricObject = new KPrQuadricBezierCurveObject();
                                    kpQuadricObject->loadOasis( o, context, m_loadingInfo );
                                    if ( groupObject )
                                        groupObject->addObjects( kpQuadricObject );
                                    else
                                        newpage->appendObject( kpQuadricObject );
                                } break;
                            case OT_FREEHAND:
                                {
                                    kDebug(33001) << "Freehand" << endl;
                                    KPrFreehandObject *kpFreeHandObject = new KPrFreehandObject();
                                    kpFreeHandObject->loadOasis( o, context, m_loadingInfo );
                                    if ( groupObject )
                                        groupObject->addObjects( kpFreeHandObject );
                                    else
                                        newpage->appendObject( kpFreeHandObject );
                                } break;
#endif
                            case OT_CLOSED_LINE:
                                {
                                    kDebug(33001) << "Closed Line" << endl;
                                    KPrClosedLineObject *kpClosedObject = new KPrClosedLineObject();
                                    kpClosedObject->loadOasis( o, context, m_loadingInfo );
                                    if ( groupObject )
                                        groupObject->addObjects( kpClosedObject );
                                    else
                                        newpage->appendObject( kpClosedObject );
                                } break;
                            default:
                                kDebug(33001) << "draw:custom-shape found unsupported object type " << objType << " in draw:enhanced-path " << d << endl;
                                break;
                        }
                    }
                    else
                    {
                        kDebug(33001) << "draw:custom-shape not supported" << endl;
                    }
                }
            }
            else if ( name == "g" && isDrawNS)
            {
                fillStyleStack( o, context, "graphic" );
                KPrGroupObject *kpgroupobject = new KPrGroupObject();
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
                //kDebug()<<"presentation:notes----------------------------------\n";
                QDomNode frameBox = KoDom::namedItemNS( o, KoXmlNS::draw, "frame" );
                QString note;

                while ( !frameBox.isNull() )
                {
                    //add an empty line between the different frames
                    if ( !note.isEmpty() )
                        note += "\n";

                    //todo load layout for note.
                    QDomNode textBox = KoDom::namedItemNS( frameBox, KoXmlNS::draw, "text-box" );

                    if ( !textBox.isNull() )
                    {
                        for ( QDomNode text = textBox.firstChild(); !text.isNull(); text = text.nextSibling() )
                        {
                            // We don't care about styles as they are not supported in kpresenter.
                            // Only add a linebreak for every child.
                            QDomElement t = text.toElement();
                            if ( t.tagName() == "p" )
                            {
                                note += t.text() + "\n";
                            }
                        }
                    }
                    frameBox = frameBox.nextSibling();
                }
                newpage->setNoteText( note );
            }
            else if ( ( name == "header" || name == "footer" ) && o.namespaceURI() == KoXmlNS::style ||
                      ( name == "animations" && o.namespaceURI() == KoXmlNS::presentation) )
            {
                //nothing
            }
            else
            {
                kDebug() << "Unsupported object '" << name << "'" << endl;
            }
            context.styleStack().restore();
        }
    }

}

int KPrDocument::createPresentationAnimation(const QDomElement& element, int order, bool increaseOrder)
{
    kDebug()<<"void KPrDocument::createPresentationAnimation(const QDomElement& element)\n";
    int orderAnimation = increaseOrder ? 0 : order;
    for ( QDomNode n = element.firstChild(); !n.isNull(); n = n.nextSibling() )
    {
        QDomElement e = n.toElement();
        Q3CString tagName = e.tagName().toLatin1();
        if ( ! tagName.isEmpty() ) // only tags that open
        {
            const bool isPresentationNS = e.namespaceURI() == KoXmlNS::presentation;
            if ( isPresentationNS &&
                 ( tagName == "show-shape" || tagName == "hide-shape" ) )
            {
                Q_ASSERT( e.hasAttributeNS( KoXmlNS::draw, "shape-id" ) );
                QString name = e.attributeNS( KoXmlNS::draw, "shape-id", QString::null );
                kDebug()<<" insert animation " << tagName << " name :" << name << endl;

                if ( e.hasAttributeNS( KoXmlNS::koffice, "order-id" ) )
                {
                    orderAnimation = e.attributeNS( KoXmlNS::koffice, "order-id", QString::null ).toInt();
                }

                lstAnimation *tmp = new lstAnimation;
                tmp->element = new QDomElement( e );
                tmp->order = orderAnimation;
                if ( tagName == "show-shape" )
                {
                    m_loadingInfo->storePresentationShowAnimation( tmp, name );
                }
                else
                {
                    m_loadingInfo->storePresentationHideAnimation( tmp, name );
                }
                if ( increaseOrder )
                    ++orderAnimation;
            }
            else if ( tagName == "animation-group" && isPresentationNS )
            {
                orderAnimation = createPresentationAnimation( e, orderAnimation, false );
            }
        }
    }
    //increase when we finish it necessary for group object
    ++orderAnimation;
    return orderAnimation;
}

void KPrDocument::fillStyleStack( const QDomElement& object, KoOasisContext & context, const char* family )
{
    // See OpenDoc 9.2.15 Common Drawing Shape Attributes
    // presentation:style-name is allways family presentation
    if ( object.hasAttributeNS( KoXmlNS::presentation, "style-name" ))
    {
        context.fillStyleStack( object, KoXmlNS::presentation, "style-name", "presentation" );
    }
    if ( object.hasAttributeNS( KoXmlNS::draw, "style-name" ) )
    {
        context.fillStyleStack( object, KoXmlNS::draw, "style-name", family );
    }
    // draw:tex-style-name is allways family paragraph
    if ( object.hasAttributeNS( KoXmlNS::draw, "text-style-name" ) )
    {
        context.fillStyleStack( object, KoXmlNS::draw, "text-style-name", "paragraph" );
    }
    if ( object.hasAttributeNS( KoXmlNS::text, "style-name" ) )
    {
        context.fillStyleStack( object, KoXmlNS::text, "style-name", family );
    }
}

bool KPrDocument::loadXML( QIODevice * dev, const QDomDocument& doc )
{
    QTime dt;
    dt.start();
    m_loadingInfo = new KPrLoadingInfo( true );

    ignoreSticky = false;
    bool b=false;
    QDomElement docelem = doc.documentElement();
    const int syntaxVersion = docelem.attribute( "syntaxVersion" ).toInt();
    if ( syntaxVersion < 2 )
    {
        // This is an old style document, before the current TextObject
        // We have kprconverter.pl for it
        kWarning(33001) << "KPresenter document version 1. Launching perl script to convert it." << endl;

        // Read the full XML and write it to a temp file
        KTempFile tmpFileIn;
        tmpFileIn.setAutoDelete( true );
        dev->reset();
        tmpFileIn.file()->write( dev->readAll() ); // copy stresm to temp file
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
            kError (33001) << "Parsing Error! Aborting! (in KPrDocument::loadXML)" << endl
                            << "  Line: " << errorLine << " Column: " << errorColumn << endl
                            << "  Message: " << errorMsg << endl;
            setErrorMessage( i18n( "parsing error in the main document (converted from an old KPresenter format) at line %1, column %2\nError message: %3" 
                             ,errorLine, errorColumn, i18n ( errorMsg.toUtf8()  )));
            return false;
        }
        b = loadXML( newdoc );
    }
    else
        b = loadXML( doc );

    ignoreSticky = true;

    if(_clean)
    {
        startBackgroundSpellCheck();
    }
    if ( m_pageWhereLoadObject == 0 && m_insertFilePage == 0 )
        setModified( false );
    kDebug(33001) << "Loading took " << (float)(dt.elapsed()) / 1000.0 << " seconds" << endl;
    return b;
}

void KPrDocument::createHeaderFooter()
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

void KPrDocument::insertEmbedded( KoStore *store, QDomElement topElem, KMacroCommand * macroCmd, KPrPage *page, int pos )
{
    QDomElement elem = topElem.firstChild().toElement();
    for ( ; !elem.isNull() ; elem = elem.nextSibling().toElement() )
    {
        kDebug(33001) << "Element name: " << elem.tagName() << endl;
        if(elem.tagName()=="EMBEDDED") {
            KPrChild *ch = new KPrChild( this );
            KPrPartObject *kppartobject = 0L;
            QRect r;

            QDomElement object=elem.namedItem("OBJECT").toElement();
            if(!object.isNull()) {
                ch->load(object, true);  // true == uppercase
                r = ch->geometry();
                ch->loadDocument( store );
                insertChild( ch );
                kppartobject = new KPrPartObject( ch );
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

            KPrInsertCmd *insertCmd = new KPrInsertCmd( i18n( "Insert Part Object" ), kppartobject, this,page );
            insertCmd->execute();
            if ( !macroCmd )
                macroCmd = new KMacroCommand( i18n("Insert Part Object"));
            macroCmd->addCommand( insertCmd );
            if ( pos != 0 )
            {
                const Q3PtrList<KPrObject>& oldList( page->objectList() );
                // tz TODO this is not 100% correct
                if ( static_cast<int>( oldList.count() ) > pos + zIndex )
                {
                    page->takeObject( kppartobject );
                    page->insertObject( kppartobject, pos + zIndex );
                    KPrLowerRaiseCmd *lrCmd = new KPrLowerRaiseCmd( i18n("Insert Part Object"),
                                                              oldList, page->objectList(),
                                                              this, page );
                    macroCmd->addCommand( lrCmd );
                }
            }
        }
    }
}

bool KPrDocument::loadXML( const QDomDocument &doc )
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
        kWarning(33001) << "Missing DOC" << endl;
        setErrorMessage( i18n("Invalid document, DOC tag missing.") );
        return false;
    }

    if(!document.hasAttribute("mime") ||  (
           document.attribute("mime")!="application/x-kpresenter" &&
           document.attribute("mime")!="application/vnd.kde.kpresenter" ) ) {
        kError(33001) << "Unknown mime type " << document.attribute("mime") << endl;
        setErrorMessage( i18n("Invalid document, expected mimetype application/x-kpresenter or application/vnd.kde.kpresenter, got %1").arg(document.attribute("mime")) );
        return false;
    }
    if(document.hasAttribute("url"))
        urlIntern=KUrl(document.attribute("url")).path();

    emit sigProgress( 10 );

    QDomElement elem=document.firstChild().toElement();

    uint childTotalCount=document.childNodes().count();
    uint childCount = 0;

    loadTextStyle( document );

    while(!elem.isNull()) {
        kDebug(33001) << "Element name: " << elem.tagName() << endl;
        if(elem.tagName()=="EMBEDDED") {
            KPrChild *ch = new KPrChild( this );
            KPrPartObject *kppartobject = 0L;
            QRect r;

            QDomElement object=elem.namedItem("OBJECT").toElement();
            if(!object.isNull()) {
                ch->load(object, true);  // true == uppercase
                r = ch->geometry();
                insertChild( ch );
                kppartobject = new KPrPartObject( ch );
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
                kppartobject->setOrig(kppartobject->getOrig().x(), offset);
            }
            else if ( kppartobject ) {
                if ( m_pageWhereLoadObject )
                {
                    kppartobject->setOrig(kppartobject->getOrig().x(), offset);
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
                setUnit( static_cast<KoUnit::Unit>(elem.attribute("unit").toInt()) );
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
                    setShowGuideLines( static_cast<bool>(elem.attribute("show").toInt() ) );
                }
                loadGuideLines( elem );
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
                allSlides = true;
        } else if ( elem.tagName()=="DEFAULTCUSTOMSLIDESHOWNAME" ) {
            if(elem.hasAttribute("name") )
                m_presentationName=elem.attribute( "name" );
        } else if ( elem.tagName()=="CUSTOMSLIDESHOWCONFIG" ) {
            if ( _clean ) {
                QMap<QString, KPrPage *> name2page;
                int pos = 1;
                for ( Q3PtrListIterator<KPrPage> it( m_pageList ); it.current(); ++it )
                {
                    name2page.insert( "page" + QString::number( pos++ ), it.current() ) ;
                }

                QDomElement slide=elem.firstChild().toElement();
                while(!slide.isNull()) {
                    if(slide.tagName()=="CUSTOMSLIDESHOW") {
                        QStringList tmp = QStringList::split( ",", slide.attribute( "pages" ) );
                        Q3ValueList<KPrPage *> pageList;
                        for ( QStringList::Iterator it = tmp.begin(); it != tmp.end(); ++it )
                        {
                            if ( name2page.contains( *it ) )
                            {
                                kDebug(33001) << "slide show " << slide.attribute( "name" ) << " page = " << *it << endl;
                                pageList.push_back( name2page[*it] );
                            }
                        }
                        if ( ! pageList.empty() )
                        {
                            m_customListSlideShow.insert( slide.attribute( "name" ), pageList );
                        }
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
                            //kDebug(33001) << "KPrDocument::loadXML m_selectedSlides nr=" << nr << " show=" << show << endl;
                            if ( nr > ( (int)m_pageList.count() - 1 ) )
                            {
                                for (int i=(m_pageList.count()-1); i<nr;i++)
                                    m_pageList.append( new KPrPage( this, m_masterPage ) );
                            }
                            m_pageList.at(nr)->slideSelected(show);
                        } else kWarning(33001) << "Parse error. No nr in <SLIDE> !" << endl;
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

void KPrDocument::loadTextStyle( const QDomElement& domElement )
{
    QDomElement style = domElement.namedItem( "STYLES" ).toElement();
    if ( _clean && ! style.isNull() )
        loadStyleTemplates( style );
}

void KPrDocument::loadPictureMap ( const QDomElement& domElement )
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

void KPrDocument::loadBackground( const QDomElement &element )
{
    kDebug(33001) << "KPrDocument::loadBackground" << endl;
    QDomElement page=element.firstChild().toElement();
    int i=m_insertFilePage;
    while(!page.isNull()) {
        if(m_pageWhereLoadObject)
        {
            kDebug(33001) << "m_pageWhereLoadObject->load(...)" << m_pageWhereLoadObject <<  endl;
            m_pageWhereLoadObject->load(page);
            break;
        }
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

KCommand *KPrDocument::loadObjects( const QDomElement &element, bool paste )
{
    ObjType t = OT_LINE;
    QDomElement obj=element.firstChild().toElement();
    Q3ValueList<KPrObject *> pasteObjects;
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
                KPrLineObject *kplineobject = new KPrLineObject();
                offset=kplineobject->load(obj);
                if ( sticky && !ignoreSticky)
                {
                    m_masterPage->appendObject(kplineobject);
                    kplineobject->setOrig(kplineobject->getOrig().x(),offset);
                }
                else if (m_pageWhereLoadObject && paste) {
                    kplineobject->setOrig(kplineobject->getOrig().x(),offset);
                    pasteObjects.append( kplineobject );
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
                KPrRectObject *kprectobject = new KPrRectObject();
                offset=kprectobject->load(obj);
                if ( sticky && !ignoreSticky)
                {
                    m_masterPage->appendObject(kprectobject);
                    kprectobject->setOrig(kprectobject->getOrig().x(),offset);
                }
                else if (m_pageWhereLoadObject && paste) {
                    kprectobject->setOrig(kprectobject->getOrig().x(),offset);
                    pasteObjects.append( kprectobject );
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
                KPrEllipseObject *kpellipseobject = new KPrEllipseObject();
                offset=kpellipseobject->load(obj);
                if ( sticky && !ignoreSticky)
                {
                    m_masterPage->appendObject(kpellipseobject);
                    kpellipseobject->setOrig(kpellipseobject->getOrig().x(),offset);
                }
                else if ( m_pageWhereLoadObject && paste)
                {
                    kpellipseobject->setOrig(kpellipseobject->getOrig().x(),offset);
                    pasteObjects.append( kpellipseobject );
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
                KPrPieObject *kppieobject = new KPrPieObject();
                offset=kppieobject->load(obj);
                if ( sticky && !ignoreSticky)
                {
                    m_masterPage->appendObject(kppieobject);
                    kppieobject->setOrig(kppieobject->getOrig().x(),offset);
                }
                else if ( m_pageWhereLoadObject && paste) {
                    kppieobject->setOrig(kppieobject->getOrig().x(),offset);
                    pasteObjects.append( kppieobject );
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
                KPrAutoformObject *kpautoformobject = new KPrAutoformObject();
                offset=kpautoformobject->load(obj);
                if ( sticky && !ignoreSticky)
                {
                    m_masterPage->appendObject(kpautoformobject);
                    kpautoformobject->setOrig(kpautoformobject->getOrig().x(),offset);
                }
                else if ( m_pageWhereLoadObject&& paste) {
                    kpautoformobject->setOrig(kpautoformobject->getOrig().x(),offset);
                    pasteObjects.append( kpautoformobject );
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
                KPrTextObject *kptextobject = new KPrTextObject( this );
                offset=kptextobject->load(obj);
                if ( sticky && !ignoreSticky)
                {
                    m_masterPage->appendObject(kptextobject);
                    kptextobject->setOrig(kptextobject->getOrig().x(),offset);
                }
                else if ( m_pageWhereLoadObject && paste) {
                    kptextobject->setOrig(kptextobject->getOrig().x(),offset);
                    pasteObjects.append( kptextobject );
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
                KPrPixmapObject *kppixmapobject = new KPrPixmapObject( pictureCollection() );
                offset=kppixmapobject->load(obj);
                if ( sticky && !ignoreSticky)
                {
                    m_masterPage->appendObject(kppixmapobject);
                    kppixmapobject->setOrig(kppixmapobject->getOrig().x(),offset);
                }
                else if ( m_pageWhereLoadObject && paste) {
                    kppixmapobject->setOrig(kppixmapobject->getOrig().x(),offset);
                    pasteObjects.append( kppixmapobject );
                    kppixmapobject->reload();
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
                KPrFreehandObject *kpfreehandobject = new KPrFreehandObject();
                offset=kpfreehandobject->load(obj);

                if ( sticky && !ignoreSticky)
                {
                    m_masterPage->appendObject(kpfreehandobject);
                    kpfreehandobject->setOrig(kpfreehandobject->getOrig().x(),offset);
                }
                else if ( m_pageWhereLoadObject && paste) {
                    kpfreehandobject->setOrig(kpfreehandobject->getOrig().x(),offset);
                    pasteObjects.append( kpfreehandobject );
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
                KPrPolylineObject *kppolylineobject = new KPrPolylineObject();
                offset=kppolylineobject->load(obj);
                if ( sticky && !ignoreSticky)
                {
                    m_masterPage->appendObject(kppolylineobject);
                    kppolylineobject->setOrig(kppolylineobject->getOrig().x(),offset);
                }
                else if (m_pageWhereLoadObject && paste) {
                    kppolylineobject->setOrig(kppolylineobject->getOrig().x(),offset);
                    pasteObjects.append( kppolylineobject );
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
                KPrQuadricBezierCurveObject *kpQuadricBezierCurveObject = new KPrQuadricBezierCurveObject();
                offset=kpQuadricBezierCurveObject->load(obj);
                if ( sticky && !ignoreSticky)
                {
                    m_masterPage->appendObject(kpQuadricBezierCurveObject);
                    kpQuadricBezierCurveObject->setOrig(kpQuadricBezierCurveObject->getOrig().x(),offset);
                }
                else if ( m_pageWhereLoadObject && paste) {
                    kpQuadricBezierCurveObject->setOrig(kpQuadricBezierCurveObject->getOrig().x(),offset);
                    pasteObjects.append( kpQuadricBezierCurveObject );
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
                KPrCubicBezierCurveObject *kpCubicBezierCurveObject = new KPrCubicBezierCurveObject();
                offset=kpCubicBezierCurveObject->load(obj);
                if ( sticky && !ignoreSticky)
                {
                    m_masterPage->appendObject(kpCubicBezierCurveObject);
                    kpCubicBezierCurveObject->setOrig(kpCubicBezierCurveObject->getOrig().x(),offset);
                }
                else if ( m_pageWhereLoadObject && paste) {
                    kpCubicBezierCurveObject->setOrig(kpCubicBezierCurveObject->getOrig().x(),offset);
                    pasteObjects.append( kpCubicBezierCurveObject );
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
                KPrPolygonObject *kpPolygonObject = new KPrPolygonObject();
                offset=kpPolygonObject->load( obj );
                if ( sticky && !ignoreSticky)
                {
                    m_masterPage->appendObject(kpPolygonObject);
                    kpPolygonObject->setOrig(kpPolygonObject->getOrig().x(),offset);
                }
                else if ( m_pageWhereLoadObject && paste) {
                    kpPolygonObject->setOrig(kpPolygonObject->getOrig().x(),offset);
                    pasteObjects.append( kpPolygonObject );
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
                KPrClosedLineObject *kpClosedLinneObject = new KPrClosedLineObject();
                offset = kpClosedLinneObject->load( obj );
                if ( sticky && !ignoreSticky) {
                    m_masterPage->appendObject( kpClosedLinneObject );
                    kpClosedLinneObject->setOrig( kpClosedLinneObject->getOrig().x(), offset );
                }
                else if ( m_pageWhereLoadObject && paste ) {
                    kpClosedLinneObject->setOrig( kpClosedLinneObject->getOrig().x(), offset );
                    pasteObjects.append( kpClosedLinneObject );
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
                KPrGroupObject *kpgroupobject = new KPrGroupObject();
                offset=kpgroupobject->load(obj, this);
                if ( sticky && !ignoreSticky)
                {
                    m_masterPage->appendObject(kpgroupobject);
                    kpgroupobject->setOrig(kpgroupobject->getOrig().x(),offset);
                }
                else if ( m_pageWhereLoadObject && paste) {
                    kpgroupobject->setOrig(kpgroupobject->getOrig().x(),offset);
                    pasteObjects.append( kpgroupobject );
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

    KPrInsertCmd *insertCmd = 0;
    if ( ! pasteObjects.empty() )
    {
        insertCmd = new KPrInsertCmd( i18n( "Paste Objects" ), pasteObjects,
                                      this , m_pageWhereLoadObject );
        insertCmd->execute();
    }
    return insertCmd;
}

void KPrDocument::loadTitle( const QDomElement &element )
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

void KPrDocument::loadNote( const QDomElement &element )
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

void KPrDocument::loadUsedSoundFileFromXML( const QDomElement &element )
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
                if ( _file.open( QIODevice::ReadOnly ) ) {
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

void KPrDocument::loadImagesFromStore( KoStore *_store )
{
    if ( _store ) {
        m_pictureCollection.readFromStore( _store, m_pictureMap );
        m_pictureMap.clear(); // Release memory
    }
}

bool KPrDocument::completeLoading( KoStore* _store )
{
    kDebug()<<"bool KPrDocument::completeLoading( KoStore* _store )*************************\n";
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
            Q3PtrListIterator<KPrPage> it( m_pageList );
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

void KPrDocument::loadUsedSoundFileFromStore( KoStore *_store, QStringList _list )
{
    int i = m_insertFilePage;
    QStringList::Iterator it = _list.begin();
    for ( ; it != _list.end(); ++it ) {
        QString soundFile = *it;

        if ( _store->open( soundFile ) ) {
            kDebug( 33001 ) << "Not found file on disk. Use this( " << soundFile << " ) file." << endl;
            KoStoreDevice dev( _store );
            int size = _store->size();
            // TODO: why not use QByteArray here?
            char *data = new char[size];
            dev.read( data, size );

            int position = soundFile.findRev( '.' );
            QString format = soundFile.right( soundFile.length() - position );
            KTempFile *tmpFile = new KTempFile( QString::null, format );
            tmpFile->setAutoDelete( true );
            tmpFile->file()->write( data, size );
            tmpFile->close();

            QString tmpFileName = tmpFile->name();
            tmpSoundFileList.append( tmpFile );

            QString _fileName = haveNotOwnDiskSoundFile.at( i );
            ++i;

            Q3PtrListIterator<KPrPage> it( m_pageList );
            for ( ; it.current(); ++it ) {
                QString _file = it.current()->getPageSoundFileName();
                if ( !_file.isEmpty() && _file == _fileName )
                    it.current()->setPageSoundFileName( tmpFileName );

                Q3PtrListIterator<KPrObject> oIt( it.current()->objectList() );
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
            delete[] data;
        }
        else {
            kDebug( 33001 ) << "Found this( " << soundFile << " ) file on disk" << endl;
        }
    }
}

void KPrDocument::setPageLayout( const KoPageLayout &pgLayout )
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
void KPrDocument::updateHeaderFooterPosition( )
{
    KoRect pageRect=m_masterPage->getPageRect();
    QRect oldBoundingRect = zoomHandler()->zoomRectOld( _header->getRepaintRect() );
    _header->setOrig(pageRect.topLeft ());
    _header->setSize(pageRect.width(),_header->getSize().height());
    repaint( oldBoundingRect );
    repaint(_header);

    oldBoundingRect = zoomHandler()->zoomRectOld( _footer->getRepaintRect() );
    _footer->setOrig(pageRect.left(),pageRect.bottom()-_footer->getSize().height());
    _footer->setSize(pageRect.width(),_footer->getSize().height());
    repaint(oldBoundingRect);
    repaint(_footer);
}

bool KPrDocument::initDoc(InitDocFlags flags, QWidget* parentWidget)
{

    if (flags==KoDocument::InitDocEmpty)
    {
        QString fileName( locate("kpresenter_template", "Screenpresentations/.source/Plain.kpt",
                                 KPrFactory::global() ) );
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

    ret = KoTemplateChooseDia::choose( KPrFactory::global(), file,
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
        KUrl url( file );
        bool ok = openURL( url );
        return ok;
    } else if ( ret == KoTemplateChooseDia::Empty ) {
        QString fileName( locate("kpresenter_template", "Screenpresentations/.source/Plain.kpt",
                                 KPrFactory::global() ) );
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

void KPrDocument::openExistingFile( const QString& file )
{
    objStartY = 0;
    _clean = true;
    KoDocument::openExistingFile( file );
}

void KPrDocument::openTemplate( const QString& file )
{
  KoDocument::openTemplate( file );
  objStartY = 0;
  _clean = true;
}

void KPrDocument::initEmpty()
{
    QString fileName( locate("kpresenter_template", "Screenpresentations/.source/Plain.kpt",
                             KPrFactory::global() ) );
    objStartY = 0;
    _clean = true;
    setModified(true);
    bool ok = loadNativeFormat( fileName );
    if ( !ok )
        showLoadingErrorDialog();
    resetURL();
}

void KPrDocument::setEmpty()
{
    KoDocument::setEmpty();
    // Whether loaded from template or from empty doc: this is a new one -> set creation date
    m_varColl->variableSetting()->setCreationDate(QDateTime::currentDateTime());
    recalcVariables( VT_DATE ); // , VST_CREATION_DATE ...
    // If we then load a document, it will override that date.
}

void KPrDocument::setGridValue( double _x, double _y, bool _replace )
{
    oldGridX = m_gridX;
    oldGridY = m_gridY;
    m_gridX=_x;
    m_gridY=_y;
    if ( _replace )
        replaceObjs();
}

void KPrDocument::repaint( bool erase )
{
    foreach ( KoView* view, views() ) {
        KPrCanvas* canvas = static_cast<KPrView*>(view)->getCanvas();
        canvas->repaint( erase );
    }
}

void KPrDocument::repaint( const QRect& rect )
{
    QRect r;
    foreach ( KoView* view, views() ) {
        r = rect;
        KPrCanvas* canvas = static_cast<KPrView*>(view)->getCanvas();
        r.moveTopLeft( QPoint( r.x() - canvas->diffx(),
                               r.y() - canvas->diffy() ) );
        canvas->update( r );
    }
}

void KPrDocument::layout(KPrObject *kpobject)
{
    KPrTextObject * obj = dynamic_cast<KPrTextObject *>( kpobject );
    if (obj)
        obj->layout();
}

void KPrDocument::layout()
{
    foreach ( KoView* view, views() ) {
        KPrCanvas* canvas = static_cast<KPrView*>(view)->getCanvas();
        canvas->layout();
    }
}

void KPrDocument::repaint( KPrObject *kpobject )
{
    repaint( m_zoomHandler->zoomRectOld( kpobject->getRepaintRect() ) );
}

Q3ValueList<int> KPrDocument::getPageEffectSteps( unsigned int num )
{
    return m_pageList.at(num)->getEffectSteps();
}

QRect KPrDocument::getPageRect( bool decBorders ) const
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

int KPrDocument::getLeftBorder() const
{
    return static_cast<int>(m_pageLayout.ptLeft);
}

int KPrDocument::getTopBorder() const
{
    return static_cast<int>(m_pageLayout.ptTop);
}

int KPrDocument::getBottomBorder() const
{
    return static_cast<int>(m_pageLayout.ptBottom);
}

int KPrDocument::getRightBorder() const
{
    return static_cast<int>(m_pageLayout.ptRight);
}

void KPrDocument::deletePage( int _page )
{
    kDebug(33001) << "KPrDocument::deletePage " << _page << endl;
    //m_pageList.at(_page)->deletePage();
    if ( m_pageList.count()==1 )
        return;
    KPrDeletePageCmd *cmd = new KPrDeletePageCmd( i18n("Delete Slide"), _page, this );
    cmd->execute();
    addCommand(cmd);
}

void KPrDocument::insertPage( KPrPage *page, int currentPageNum, int insertPageNum )
{
    // check if page was allready deleted
    int pos = m_deletedPageList.findRef( page );
    if ( pos != -1 )
        m_deletedPageList.take( pos );

    m_pageList.insert( insertPageNum, page );

    pageOrderChanged();
    //activate this page in all views which on slide currentPageNum
    foreach ( KoView* v, views() )
    {
        KPrView *view = static_cast<KPrView*>( v );
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

void KPrDocument::takePage( KPrPage *page, int pageNum )
{
    int pos = m_pageList.findRef( page );
    m_pageList.take( pos );
    m_deletedPageList.append( page );

    pageOrderChanged();

    foreach ( KoView* v, views() )
    {
        KPrView *view = static_cast<KPrView*>( v );
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

void KPrDocument::pageOrderChanged()
{
    recalcVariables( VT_PGNUM );

    //update statusbar
    emit pageNumChanged();
    emit sig_updateMenuBar();
}

void KPrDocument::movePageTo( int oldPos, int newPos )
{
    kDebug(33001) << "movePage oldPos = " << oldPos << ", neuPos = " << newPos << endl;

    KPrPage * page = m_pageList.take( oldPos );
    m_pageList.insert( newPos, page );

    pageOrderChanged();

    // Update the sidebars
    foreach ( KoView* v, views() )
    {
        KPrView *view = static_cast<KPrView*>( v );
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

QString KPrDocument::templateFileName( bool chooseTemplate, const QString &theFile )
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
        if ( KoTemplateChooseDia::choose( KPrFactory::global(), _template,
                                          KoTemplateChooseDia::OnlyTemplates,
                                          "kpresenter_template", parentWidget ) == KoTemplateChooseDia::Cancel )
            return QString::null;
        QFileInfo fileInfo( _template );
        fileName = fileInfo.dirPath( true ) + "/" + fileInfo.baseName() + ".kpt";

        KUrl src, dest;
        src.setPath( fileName );
        dest.setPath( locateLocal( "appdata", "default.kpr" ) );
        kDebug(33001) << "Copying template  (in KPrDocument::templateFileName)" << endl
                       << "  from: " << src.prettyUrl() << endl
                       << "  to: " << dest.prettyUrl() << endl;
        KIO::NetAccess::file_copy( src,
				   dest,
				   -1, /* default permissions */
				   true /* overwrite */ );
    }
    return fileName;
}

int KPrDocument::insertNewPage( const QString &cmdName, int _page, InsertPos _insPos,
                                  bool chooseTemplate, const QString &theFile )
{
    kDebug(33001) << "KPrDocument::insertNewPage " << _page << endl;

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

void KPrDocument::savePage( const QString &file, int pgnum, bool ignore )
{
    saveOnlyPage = pgnum;
    _duplicatePage=ignore;
    saveNativeFormat( file );
    _duplicatePage=false;
    saveOnlyPage = -1;
}

void KPrDocument::replaceObjs( bool createUndoRedo )
{
    KMacroCommand * macroCmd = 0L;
    Q3PtrListIterator<KPrPage> oIt(m_pageList);
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

void KPrDocument::restoreBackground( KPrPage *page )
{
    page->background()->reload();
}

KCommand * KPrDocument::loadPastedObjs( const QString &in, KPrPage* _page )
{
    QDomDocument doc;
    doc.setContent( in );

    QDomElement document=doc.documentElement();

    // DOC
    if (document.tagName()!="DOC") {
        kError(33001) << "Missing DOC" << endl;
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

void KPrDocument::deSelectAllObj()
{
    foreach ( KoView* view, views() )
        static_cast<KPrView*>(view)->getCanvas()->deSelectAllObj();
}

void KPrDocument::deSelectObj(KPrObject *obj)
{
    foreach ( KoView* view, views() )
        static_cast<KPrView*>(view)->getCanvas()->deSelectObj( obj );
}

void KPrDocument::setDisplayObjectMasterPage( bool b )
{
    foreach ( KoView* view, views() )
        static_cast<KPrView*>(view)->updateDisplayObjectMasterPageButton();
    repaint(b);
}

void KPrDocument::setDisplayBackground( bool b )
{
    foreach ( KoView* view, views() )
        static_cast<KPrView*>(view)->updateDisplayBackgroundButton();
    repaint(b);
}

void KPrDocument::setHeader( bool b )
{
    _header->setDrawEditRect( b );
    _header->setDrawEmpty( b );
    if(!b)
    {
        terminateEditing(_header);
        deSelectObj(_header);
    }
    m_masterPage->setHeader( b, false );
    updateHeaderFooterButton();
    repaint(b);
}

void KPrDocument::setFooter( bool b )
{
    _footer->setDrawEditRect( b );
    _footer->setDrawEmpty( b );
    if(!b)
    {
        terminateEditing(_footer);
        deSelectObj(_footer);
    }
    m_masterPage->setFooter( b, false );
    updateHeaderFooterButton();
    repaint(b);
}

void KPrDocument::updateHeaderFooterButton()
{
    foreach ( KoView* view, views() )
        static_cast<KPrView*>(view)->updateHeaderFooterButton();
}

void KPrDocument::makeUsedPixmapList()
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

void KPrDocument::makeUsedSoundFileList()
{
    if ( saveOnlyPage != -1 )
        return;

    usedSoundFile.clear();

    Q3PtrListIterator<KPrPage> it( m_pageList );
    for ( ; it.current(); ++it ) {
        QString _file = it.current()->getPageSoundFileName();
        if ( !_file.isEmpty() && usedSoundFile.findIndex( _file ) == -1 )
            usedSoundFile.append( _file );

        Q3PtrListIterator<KPrObject> oIt( it.current()->objectList() );
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

KoView* KPrDocument::createViewInstance( QWidget* parent, const char* name )
{
    //the page numbers have to be recalced for the sticky objects
    //as it could not be done during the constructor of KPrView
    recalcPageNum();
    return new KPrView( this, parent, name );
}

void KPrDocument::paintContent( QPainter& painter, const QRect& rect,
                                  bool /*transparent*/, double zoomX, double zoomY )
{
    m_zoomHandler->setZoom( 100 );
    if ( zoomHandler()->zoomedResolutionX() != zoomX || zoomHandler()->zoomedResolutionY() != zoomY )
    {
        int zoomLevel = qRound( 100 * zoomY / zoomHandler()->zoomedResolutionY() ); // ## ignores the case where the x and y scaling differs
        zoomHandler()->setZoom( zoomLevel );
        bool forPrint = painter.device() && painter.device()->devType() == QInternal::Printer;
        newZoomAndResolution( false, forPrint );
    }
    //for the moment draw first page.
    KPrPage *page=m_pageList.first();
    if( m_initialActivePage )
        page = m_initialActivePage;

    int pageNum = m_pageList.findRef( page );

    if ( page->displayBackground() )
        page->background()->drawBackground( &painter, zoomHandler(), rect, false );
    if ( page->displayObjectFromMasterPage() )
    {
        KPrPage *masterPage = page->masterPage();
        if ( masterPage )
        {
            Q3PtrListIterator<KPrObject> it( masterPage->objectList() );
            //draw objects on master slide
            for ( ; it.current() ; ++it )
            {
                if( (it.current()==_header && !page->hasHeader())||(it.current()==_footer && !page->hasFooter()))
                    continue;
                it.current()->draw( &painter, zoomHandler(), pageNum, SM_NONE );
            }
        }
    }
    Q3PtrListIterator<KPrObject> it( page->objectList() );
    for ( ; it.current() ; ++it )
        it.current()->draw( &painter, zoomHandler(), pageNum, SM_NONE );
}

QPixmap KPrDocument::generatePreview( const QSize& size )
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

void KPrDocument::addShell( KoMainWindow *shell )
{
    connect( shell, SIGNAL( documentSaved() ), m_commandHistory, SLOT( documentSaved() ) );
    KoDocument::addShell( shell );
}

void KPrDocument::movePage( int from, int to )
{
    kDebug(33001) << "KPrDocument::movePage from=" << from << " to=" << to << endl;
    KPrMovePageCmd *cmd = new KPrMovePageCmd( i18n("Move Slide"), from, to, this );
    cmd->execute();
    addCommand(cmd);
}

void KPrDocument::copyPage( int from )
{
    _clean = false;
    m_childCountBeforeInsert = children().count();

    _duplicatePage=true; // ### now also set via savePage() parameter below

    kDebug(33001) << "KPrDocument::copyPage from=" << from << " to=" << from + 1 << endl;
    kDebug(33001) << "mimeType = " << mimeType() << ", outputMimeType = " << outputMimeType() << endl;
    bool wasSelected = isSlideSelected( from );
    KTempFile tempFile( QString::null, mimeType() == nativeOasisMimeType() ? ".oop": ".kpr" );
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

void KPrDocument::copyPageToClipboard( int pgnum )
{
    // We save the page to a temp file and set the URL of the file in the clipboard
    // Yes it's a hack but at least we don't hit the clipboard size limit :)
    // (and we don't have to implement copy-tar-structure-to-clipboard)
    // In fact it even allows copying a [1-page] kpr in konq and pasting it in kpresenter :))
    kDebug(33001) << "KPrDocument::copyPageToClipboard pgnum=" << pgnum << endl;
    kDebug(33001) << "mimeType = " << mimeType() << ", outputMimeType = " << outputMimeType() << endl;
    KTempFile tempFile( QString::null, mimeType() == nativeOasisMimeType() ? ".oop": ".kpr" );
    savePage( tempFile.name(), pgnum, true );
    KUrl url; url.setPath( tempFile.name() );
    KUrl::List lst;
    lst.append( url );
    QApplication::clipboard()->setData( new K3URLDrag( lst ) );
    m_tempFileInClipboard = tempFile.name(); // do this last, the above calls clipboardDataChanged
}

void KPrDocument::pastePage( const QMimeSource * data, int pgnum )
{
    KUrl::List lst;
    if ( K3URLDrag::decode( data, lst ) && !lst.isEmpty() )
    {
        insertNewPage(i18n("Paste Slide"),  pgnum, IP_BEFORE, false, lst.first().path() );
        //selectPage( pgnum, true /* should be part of the file ? */ );
    }
}

void KPrDocument::clipboardDataChanged()
{
    if ( !m_tempFileInClipboard.isEmpty() )
    {
        kDebug(33001) << "KPrDocument::clipboardDataChanged, deleting temp file " << m_tempFileInClipboard << endl;
        unlink( QFile::encodeName( m_tempFileInClipboard ) );
        m_tempFileInClipboard = QString::null;
    }
    // TODO enable paste as well, when a txtobject is activated
    // and there is plain text in the clipboard. Then enable this code.
    //QMimeSource *data = QApplication::clipboard()->data();
    //bool canPaste = data->provides( "text/uri-list" ) || data->provides( "application/x-kpresenter-selection" );
    // emit enablePaste( canPaste );
}

void KPrDocument::selectPage( int pgNum /* 0-based */, bool select )
{
    Q_ASSERT( pgNum >= 0 );
    KPrPage *page = m_pageList.at( pgNum );
    page->slideSelected(select);
    kDebug(33001) << "KPrDocument::selectPage pgNum=" << pgNum << " select=" << select << endl;
    setModified(true);

    updateSideBarItem( page );
    updatePresentationButton();
    //update statusbar
    emit pageNumChanged();
}

KPrPage * KPrDocument::findPage(KPrObject *object)
{
    Q3PtrList<KPrObject> masterObjects( m_masterPage->objectList() );
    if ( masterObjects.findRef( object ) != -1 )
    {
        //kDebug(33001) << "Object is on the master page" << endl;
        return m_masterPage;
    }
    Q3PtrListIterator<KPrPage> it( m_pageList );
    for ( ; it.current(); ++it ) {
        Q3PtrList<KPrObject> list( it.current()->objectList() );
        if ( list.findRef( object ) != -1 ) {
            //kDebug(33001) << "Object is on page " << m_pageList.findRef(it.current()) + 1 << endl;
            return it.current();
        }
    }
    kDebug(33001) << "Object not found on a page" << endl;
    return 0L;
}

KPrPage * KPrDocument::findPage(Q3PtrList<KPrObject> &objects)
{
    KPrObject *object;
    for ( object = objects.first(); object; object=objects.next() ) {
        Q3PtrList<KPrObject> list( m_masterPage->objectList() );
        if ( list.findRef( object ) != -1 )
        {
            //kDebug(33001) << "Object is on the master page" << endl;
            return m_masterPage;
        }
    }
    object = objects.first();
    for ( KPrPage *page=m_pageList.first(); page; page=m_pageList.next() ) {
        Q3PtrList<KPrObject> list( page->objectList() );
        if ( list.findRef( object ) != -1 ) {
            //kDebug(33001) << "The Objects are on page " << m_pageList.findRef(page) + 1 << endl;
            return page;
        }
    }
    kDebug(33001) << "Objects not found on a page" << endl;
    return 0L;
}

void KPrDocument::updateSideBarItem( KPrPage * page )
{
    // Update the views
    foreach ( KoView* view, views() )
        static_cast<KPrView*>(view)->updateSideBarItem( page );
}

bool KPrDocument::isSlideSelected( int pgNum /* 0-based */ )
{
    Q_ASSERT( pgNum >= 0 );
    return m_pageList.at(pgNum)->isSlideSelected();
}

Q3ValueList<int> KPrDocument::listOfDisplaySelectedSlides( const Q3ValueList<KPrPage*> & lst) /* returned list is 0-based */
{
    Q3ValueList<int> result;
    Q3ValueListConstIterator<KPrPage*> itPage;
    Q3ValueListConstIterator<KPrPage*> itPageEnd = lst.end();
    for( itPage =  lst.begin() ; itPage != itPageEnd; ++itPage )
    {
        int pageNum = m_pageList.find(*itPage );
        if ( pageNum != -1 )
        {
            kDebug()<<" KPrDocument::displaySelectedSlide : add slide number :"<<pageNum<<endl;
            result << pageNum;
        }
    }
    return result;
}


Q3ValueList<int> KPrDocument::displaySelectedSlides()  /* returned list is 0-based */
{
    Q3ValueList<int> result;
    if ( m_customListTest )
        return *m_customListTest;
    if ( m_presentationName.isEmpty() )
        return selectedSlides();
    else
    {
        kDebug()<<" KPrDocument::displaySelectedSlide m_presentationName : "<<m_presentationName<<endl;
        result = listOfDisplaySelectedSlides( m_customListSlideShow[m_presentationName]);
    }
    return result;
}

Q3ValueList<int> KPrDocument::selectedSlides() /* returned list is 0-based */
{
    Q3ValueList<int> result;
    for ( int i = 0; i < static_cast<int>( m_pageList.count() ); i++ ) {
        if(m_pageList.at(i)->isSlideSelected())
            result <<i;
    }
    return result;
}

QString KPrDocument::selectedForPrinting() {
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

void KPrDocument::slotRepaintChanged( KPrTextObject *kptextobj )
{
    //todo
    //use this function for the moment
    repaint( kptextobj );
}


void KPrDocument::recalcVariables( int type )
{
    recalcPageNum();
    Q3ValueList<KoVariable* > modifiedVariables = m_varColl->recalcVariables(type);
    if ( modifiedVariables.isEmpty() )
        return;

    // TODO use the return value from recalcVariables to only repaint what has changed.
    Q3PtrListIterator<KPrPage> it( m_pageList );
    for ( ; it.current(); ++it )
        it.current()->slotRepaintVariable();
    m_masterPage->slotRepaintVariable();
}

void KPrDocument::slotGuideLinesChanged( KoView *view )
{
    ( (KPrView*)view )->getCanvas()->guideLines().getGuideLines( m_hGuideLines, m_vGuideLines );
    foreach ( KoView* v, views() )
    {
        if ( v != view )
        {
            static_cast<KPrView*>(v)->getCanvas()->guideLines().setGuideLines( m_hGuideLines, m_vGuideLines );
        }
    }
}

void KPrDocument::slotDocumentInfoModifed()
{
    if (!getVariableCollection()->variableSetting()->displayFieldCode())
        recalcVariables( VT_FIELD );
}

void KPrDocument::reorganizeGUI()
{
    foreach ( KoView* view, views() )
        static_cast<KPrView*>(view)->reorganize();
}

int KPrDocument::undoRedoLimit() const
{
    return m_commandHistory->undoLimit();
}

void KPrDocument::setUndoRedoLimit(int val)
{
    m_commandHistory->setUndoLimit(val);
    m_commandHistory->setRedoLimit(val);
}

void KPrDocument::updateRuler()
{
    emit sig_updateRuler();
}

void KPrDocument::recalcPageNum()
{
    Q3PtrListIterator<KPrPage> it( m_pageList );
    for ( ; it.current(); ++it )
        it.current()->recalcPageNum();
    m_masterPage->recalcPageNum();
}

KPrPage * KPrDocument::activePage()const
{
    return m_initialActivePage;
}

void KPrDocument::insertObjectInPage(double offset, KPrObject *_obj, int pos)
{
    /// Why does this use __pgLayout instead of m_pageLayout ?
    int page = (int)(offset/__pgLayout.ptHeight)+m_insertFilePage;
    if ( page < 0 )
    {
        kDebug(33001) << "insertObjectInPage object cound not be inserted page = " << page << endl;
        return;
    }
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

void KPrDocument::insertPixmapKey( KoPictureKey key )
{
    if ( !usedPictures.contains( key ) )
        usedPictures.append( key );
}

KPrPage * KPrDocument::initialActivePage() const
{
    return m_initialActivePage;
}

void KPrDocument::displayActivePage(KPrPage * _page)
{
    m_initialActivePage = _page;
}

void KPrDocument::updateZoomRuler()
{
    foreach ( KoView* view, views() )
    {
        static_cast<KPrView*>(view)->getHRuler()->setZoom( m_zoomHandler->zoomedResolutionX() );
        static_cast<KPrView*>(view)->getVRuler()->setZoom( m_zoomHandler->zoomedResolutionY() );
        static_cast<KPrView*>(view)->slotUpdateRuler();
    }
}

void KPrDocument::newZoomAndResolution( bool updateViews, bool /*forPrint*/ )
{
    Q3PtrListIterator<KPrPage> it( m_pageList );
    for ( ; it.current(); ++it ) {
        Q3PtrListIterator<KPrObject> oit(it.current()->objectList());
        for ( ; oit.current(); ++oit ) {
            if ( oit.current()->getType() == OT_TEXT )
                static_cast<KPrTextObject *>( oit.current() )->textDocument()->formatCollection()->zoomChanged();
        }
    }
    if ( updateViews )
    {
        foreach ( KoView* view, views() )
        {
            static_cast<KPrView *>( view )->getCanvas()->update();
            static_cast<KPrView *>( view )->getCanvas()->layout();
        }
    }
}

bool KPrDocument::isHeader(const KPrObject *obj) const
{
    return (obj==_header);
}

bool KPrDocument::isFooter(const KPrObject *obj) const
{
    return (obj==_footer);
}

bool KPrDocument::isHeaderFooter(const KPrObject *obj) const
{
    return (obj==_header)||(obj==_footer);
}

void KPrDocument::updateRulerPageLayout()
{
    foreach ( KoView* view, views() )
    {
        static_cast<KPrView*>(view)->getHRuler()->setPageLayout(m_pageLayout );
        static_cast<KPrView*>(view)->getVRuler()->setPageLayout(m_pageLayout );
    }
}

void KPrDocument::refreshAllNoteBarMasterPage(const QString &text, KPrView *exceptView)
{
    m_masterPage->setNoteText(text );
    foreach ( KoView* v, views() )
    {
        KPrView* view = static_cast<KPrView*>(v);
        if ( view->getNoteBar() && view != exceptView && view->editMaster() )
            view->getNoteBar()->setCurrentNoteText(text );
    }
}

void KPrDocument::refreshAllNoteBar(int page, const QString &text, KPrView *exceptView)
{
    m_pageList.at(page)->setNoteText(text );
    foreach ( KoView* v, views() )
    {
        KPrView* view=(KPrView*)v;
        if ( view->getNoteBar() && view != exceptView && ((int)(view->getCurrPgNum())-1 == page))
            view->getNoteBar()->setCurrentNoteText(text );
    }
}

void KPrDocument::loadStyleTemplates( const QDomElement &stylesElem )
{
    Q3ValueList<QString> followingStyles;

    QDomNodeList listStyles = stylesElem.elementsByTagName( "STYLE" );
    if( listStyles.count() > 0) { // we are going to import at least one style.
        KoParagStyle *s = m_styleColl->findStyle("Standard");
        kDebug(32001) << "KPrDocument::loadStyleTemplates looking for Standard, to delete it. Found " << s << endl;
        if(s) // delete the standard style.
            m_styleColl->removeStyle(s);
    }
    for (unsigned int item = 0; item < listStyles.count(); item++) {
        QDomElement styleElem = listStyles.item( item ).toElement();

        KoParagStyle *sty = new KoParagStyle( QString::null );
        // Load the style from the <STYLE> element
        sty->loadStyle( styleElem );

        QDomElement formatElem = styleElem.namedItem( "FORMAT" ).toElement();
        if ( !formatElem.isNull() )
            sty->format() = KPrTextObject::loadFormat( formatElem, 0L, defaultFont(), globalLanguage(), globalHyphenation() );
        else
            kWarning(33001) << "No FORMAT tag in <STYLE>" << endl; // This leads to problems in applyStyle().

        // Style created, now let's try to add it
        sty = m_styleColl->addStyle( sty );
        kDebug() << k_funcinfo << m_styleColl->styleList().count() << " styles, " << followingStyles.count() << " following styles" << endl;
        if(m_styleColl->styleList().count() > followingStyles.count() )
        {
            QString following = styleElem.namedItem("FOLLOWING").toElement().attribute("name");
            followingStyles.append( following );
        }
        else
            kWarning (33001) << "Found duplicate style declaration, overwriting former " << sty->name() << endl;
    }

    Q_ASSERT( followingStyles.count() == m_styleColl->styleList().count() );
    unsigned int i=0;
    for( Q3ValueList<QString>::Iterator it = followingStyles.begin(); it != followingStyles.end(); ++it ) {
        KoParagStyle * style = m_styleColl->findStyle(*it);
        m_styleColl->styleAt( i++)->setFollowingStyle( style );
    }
}


void KPrDocument::updateAllStyleLists()
{
    foreach ( KoView* view, views() )
        static_cast<KPrView*>(view)->updateStyleList();
}

void KPrDocument::applyStyleChange( KoStyleChangeDefMap changed )
{
    Q3PtrListIterator<KPrPage> it( m_pageList );
    for ( ; it.current(); ++it )
        it.current()->applyStyleChange( changed );
    m_masterPage->applyStyleChange( changed );
}

void KPrDocument::saveStyle( KoParagStyle *sty, QDomElement parentElem )
{
    QDomDocument doc = parentElem.ownerDocument();
    QDomElement styleElem = doc.createElement( "STYLE" );
    parentElem.appendChild( styleElem );

    sty->saveStyle( styleElem );
    QDomElement formatElem = doc.createElement("FORMAT");
    KPrTextObject::saveFormat( formatElem, &sty->format() );
    styleElem.appendChild( formatElem );
}

void KPrDocument::startBackgroundSpellCheck()
{
    //don't start spell checking when document is embedded in konqueror
    if(backgroundSpellCheckEnabled() && isReadWrite())
    {
        if(m_initialActivePage->allTextObjects().count()>0)
        {
            m_bgSpellCheck->start();
        }
    }
}

void KPrDocument::enableBackgroundSpellCheck( bool b )
{
    //m_bgSpellCheck->enableBackgroundSpellCheck(b);
    m_bgSpellCheck->setEnabled(b);
    foreach ( KoView* view, views() )
        static_cast<KPrView*>(view)->updateBgSpellCheckingState();
}

bool KPrDocument::backgroundSpellCheckEnabled() const
{
    return m_bgSpellCheck->enabled();
}

void KPrDocument::reactivateBgSpellChecking(bool refreshTextObj)
{
    Q3PtrListIterator<KPrPage> it( m_pageList );
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

Q3PtrList<KoTextObject> KPrDocument::allTextObjects() const
{
    Q3PtrList<KoTextObject> lst;
    Q3PtrListIterator<KPrPage> it( m_pageList );
    for ( ; it.current(); ++it )
        it.current()->addTextObjects( lst );
    m_masterPage->addTextObjects( lst );
    return lst;
}

Q3ValueList<KoTextDocument *> KPrDocument::allTextDocuments() const
{
    Q3ValueList<KoTextDocument *> lst;
    const Q3PtrList<KoTextObject> textObjects = allTextObjects();
    Q3PtrListIterator<KoTextObject> it( textObjects );
    for ( ; it.current() ; ++it ) {
        lst.append( it.current()->textDocument() );
    }
    return lst;
}

Q3ValueList<KoTextObject *> KPrDocument::visibleTextObjects( ) const
{
    Q3ValueList<KoTextObject *> lst;
    Q3PtrList<KoTextObject> textFramesets = allTextObjects(  );

    KoTextObject *frm;
    for ( frm=textFramesets.first(); frm != 0; frm=textFramesets.next() ) {
        if ( frm && !frm->protectContent() )
        {
            lst.append( frm );
        }
    }
    return lst;
}

void KPrDocument::setShowGuideLines( bool b )
{
    m_bShowGuideLines = b;
    setModified( true );
}

void KPrDocument::horizontalGuideLines( const Q3ValueList<double> &lines )
{
    m_hGuideLines = lines;
}

void KPrDocument::verticalGuideLines( const Q3ValueList<double> &lines )
{
    m_vGuideLines = lines;
}


void KPrDocument::addGuideLine( Qt::Orientation o, double pos )
{
    if ( o == Qt::Horizontal )
    {
        m_hGuideLines.append( pos );
    }
    else
    {
        m_vGuideLines.append( pos );
    }

    foreach ( KoView* view, views() )
    {
        ( (KPrView*)view )->getCanvas()->guideLines().setGuideLines( m_hGuideLines, m_vGuideLines );
    }
}


void KPrDocument::updateGuideLineButton()
{
    foreach ( KoView* view, views() )
        static_cast<KPrView*>(view)->updateGuideLineButton();
}

void KPrDocument::loadGuideLines( const QDomElement &element )
{
    // In early versions of KPresenter 1.2 (up to Beta 2), there is child also naed <HELPLINES>
    // Before KPresenter 1.5 the guide lines where named helplines that is why they are still
    // named like this in the fileformat
    QDomElement guidelines = element.namedItem( "HELPLINES" ).toElement();
    if ( guidelines.isNull() )
        guidelines = element;

    guidelines = guidelines.firstChild().toElement();
    while ( !guidelines.isNull() )
    {
        if ( guidelines.tagName() == "Vertical" )
            m_vGuideLines.append( guidelines.attribute( "value" ).toDouble() );
        else if ( guidelines.tagName() == "Horizontal" )
            m_hGuideLines.append( guidelines.attribute( "value" ).toDouble() );
        guidelines = guidelines.nextSibling().toElement();
    }
}

void KPrDocument::saveGuideLines( QDomDocument &doc, QDomElement& element )
{
    for(Q3ValueList<double>::Iterator it = m_vGuideLines.begin(); it != m_vGuideLines.end(); ++it)
    {
        QDomElement lines=doc.createElement("Vertical");
        lines.setAttribute("value", (double)*it);
        element.appendChild( lines );
    }

    for(Q3ValueList<double>::Iterator it = m_hGuideLines.begin(); it != m_hGuideLines.end(); ++it)
    {
        QDomElement lines=doc.createElement("Horizontal");
        lines.setAttribute("value", *it);
        element.appendChild( lines );
    }
}

void KPrDocument::updateGridButton()
{
    foreach ( KoView* view, views() )
        static_cast<KPrView*>(view)->updateGridButton();

}

void KPrDocument::setSpellCheckIgnoreList( const QStringList& lst )
{
    m_spellCheckIgnoreList = lst;
    m_bgSpellCheck->settings()->setCurrentIgnoreList( m_spellCheckIgnoreList + m_spellCheckPersonalDict );
   setModified( true );
}

void KPrDocument::addSpellCheckIgnoreWord( const QString & word )
{
    // ### missing: undo/redo support
    if( m_spellCheckIgnoreList.findIndex( word ) == -1 )
        m_spellCheckIgnoreList.append( word );
    setSpellCheckIgnoreList( m_spellCheckIgnoreList );
}

void KPrDocument::updateObjectStatusBarItem()
{
    foreach ( KoView* view, views() )
        static_cast<KPrView*>(view)->updateObjectStatusBarItem();
}

void KPrDocument::updateObjectSelected()
{
    foreach ( KoView* view, views() )
        static_cast<KPrView*>(view)->objectSelectedChanged();
}

void KPrDocument::setTabStopValue ( double _tabStop )
{
    m_tabStop = _tabStop;
    Q3PtrListIterator<KPrPage> it( m_pageList );
    for ( ; it.current(); ++it )
        it.current()->changeTabStopValue( m_tabStop );
    //styckypage
    m_masterPage->changeTabStopValue( m_tabStop );
}

void KPrDocument::changeBgSpellCheckingState( bool b )
{
    enableBackgroundSpellCheck( b );
    reactivateBgSpellChecking();
    KConfig *config = KPrFactory::global()->config();
    config->setGroup("KSpell kpresenter" );
    config->writeEntry( "SpellCheck", (int)b );
}


bool KPrDocument::cursorInProtectedArea()const
{
    return m_cursorInProtectectedArea;
}

void KPrDocument::setCursorInProtectedArea( bool b )
{
    m_cursorInProtectectedArea=b;
    testAndCloseAllTextObjectProtectedContent();
}

void KPrDocument::testAndCloseAllTextObjectProtectedContent()
{
    if ( !m_cursorInProtectectedArea )
    {
        foreach ( KoView* view, views() )
            static_cast<KPrView*>(view)->testAndCloseAllTextObjectProtectedContent();
    }
}

void KPrDocument::insertFile(const QString & file )
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
    foreach ( KoView* view, views() )
        static_cast<KPrView*>(view)->updateSideBar();
    _clean = clean;
    updatePresentationButton();

    //activate this page in all views (...)
    foreach ( KoView* view, views() )
        static_cast<KPrView*>(view)->skipToPage(newPos);
}

void KPrDocument::spellCheckParagraphDeleted( KoTextParag * /* _parag */,  KPrTextObject * /* frm */ )
{
    //m_bgSpellCheck->spellCheckParagraphDeleted( _parag, frm->textObject());
}

void KPrDocument::updateRulerInProtectContentMode()
{
    foreach ( KoView* view, views() )
        static_cast<KPrView*>(view)->updateRulerInProtectContentMode();
}

void KPrDocument::updatePresentationButton()
{
    foreach ( KoView* view, views() )
        static_cast<KPrView*>(view)->updatePresentationButton((selectedSlides().count()>0));
}

void KPrDocument::refreshGroupButton()
{
    foreach ( KoView* view, views() )
        static_cast<KPrView*>(view)->refreshGroupButton();
}

void KPrDocument::addView( KoView *_view )
{
    KoDocument::addView( _view );
    foreach ( KoView* view, views() )
        static_cast<KPrView*>(view)->closeTextObject();
}

void KPrDocument::removeView( KoView *_view )
{
    KoDocument::removeView( _view );
    foreach ( KoView* view, views() )
        static_cast<KPrView*>(view)->deSelectAllObjects();
}

void KPrDocument::updateStyleListOrder( const QStringList &list )
{
    styleCollection()->updateStyleListOrder( list );
}

void KPrDocument::updateDirectCursorButton()
{
    foreach ( KoView* view, views() )
        static_cast<KPrView*>(view)->updateDirectCursorButton();
}

void KPrDocument::setInsertDirectCursor(bool _b)
{
    m_bInsertDirectCursor=_b;
    KConfig *config = KPrFactory::global()->config();
    config->setGroup( "Interface" );
    config->writeEntry( "InsertDirectCursor", _b );
    updateDirectCursorButton();
}

KPrView *KPrDocument::firstView() const
{
    if ( views().count()>0)
        return static_cast<KPrView*>(views().first());
    else
        return 0L;
}

void KPrDocument::addWordToDictionary( const QString & word)
{
    if ( m_bgSpellCheck )
    {
        if( m_spellCheckPersonalDict.findIndex( word ) == -1 )
            m_spellCheckPersonalDict.append( word );
        m_bgSpellCheck->settings()->setCurrentIgnoreList( m_spellCheckIgnoreList + m_spellCheckPersonalDict );
        if ( backgroundSpellCheckEnabled() )
            // Re-check everything to make this word normal again
            reactivateBgSpellChecking();
    }
}

Q3ValueList <KPrPage *> KPrDocument::customListPage( const QStringList & lst, bool loadOasis )
{
    QStringList tmp( lst );
    Q3ValueList <KPrPage *> tmpValueList;
    for ( QStringList::Iterator itList = tmp.begin(); itList != tmp.end(); ++itList )
    {
        for ( int i = 0; i < static_cast<int>( m_pageList.count() ); i++ )
        {
            //kDebug()<<" insert page name :"<<*itList<<endl;
            if ( loadOasis )
            {
                if ( m_pageList.at( i )->oasisNamePage(i+1)== ( *itList ) )
                {
                    tmpValueList.append(  m_pageList.at( i ) );
                    //kDebug()<<" really insert\n";
                    break;
                }
            }
            else
            {
                if ( m_pageList.at( i )->pageTitle()== ( *itList ) )
                {
                    tmpValueList.append( m_pageList.at( i ) );
                    //kDebug()<<" really insert\n";
                    break;
                }
            }

        }
    }
    return tmpValueList;

}

void KPrDocument::setCustomSlideShows( const CustomSlideShowMap & customSlideShows )
{
    m_customListSlideShow = customSlideShows;
    setModified( true );
}

QStringList KPrDocument::presentationList()
{
    QStringList lst;
    if ( !m_customListSlideShow.isEmpty() )
    {
        CustomSlideShowMap::Iterator it;
        for ( it = m_customListSlideShow.begin(); it != m_customListSlideShow.end(); ++it )
            lst << it.key();
    }
    return lst;
}

void KPrDocument::testCustomSlideShow( const Q3ValueList<KPrPage *> &pages, KPrView *view )
{
    delete m_customListTest;
    m_customListTest = new Q3ValueList<int>( listOfDisplaySelectedSlides( pages ) );
    if ( view )
        view->screenStartFromFirst();

}

void KPrDocument::clearTestCustomSlideShow()
{
    delete m_customListTest;
    m_customListTest = 0L;
}


#include "KPrDocument.moc"
