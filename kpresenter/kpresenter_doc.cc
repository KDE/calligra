/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include <kpresenter_doc.h>
#include <kpresenter_view.h>
#include "kprcanvas.h"
#include <kplineobject.h>
#include <kprectobject.h>
#include <kpellipseobject.h>
#include <kpautoformobject.h>
#include <kpclipartobject.h>
#include <kptextobject.h>
#include <kprtextdocument.h>
#include <kppixmapobject.h>
#include <kppieobject.h>
#include <kppartobject.h>
#include <kpgroupobject.h>
#include <kprcommand.h>
#include <styledia.h>
#include <insertpagedia.h>
#include <kpfreehandobject.h>
#include <kppolylineobject.h>
#include <kpquadricbeziercurveobject.h>
#include <kpcubicbeziercurveobject.h>
#include <kppolygonobject.h>
#include <kpclosedlineobject.h>

#include <qpopupmenu.h>
#include <qclipboard.h>
#include <qregexp.h>
#include <qfileinfo.h>
#include <qdom.h>

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

#include <koTemplateChooseDia.h>
#include <koRuler.h>
#include <koFilterManager.h>
#include <koStore.h>
#include <koStoreDevice.h>
#include <koQueryTrader.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <config.h>

#include <qrichtext_p.h>
#include <kotextobject.h>
#include <kozoomhandler.h>
#include <kostyle.h>
#include <kcommand.h>
#include <KPresenterDocIface.h>
#include <kspell.h>

#include <koVariable.h>
#include <koAutoFormat.h>
#include <koDocumentInfo.h>
#include "kprvariable.h"
#include "kpbackground.h"
#include "notebar.h"
#include "kprbgspellcheck.h"

using namespace std;

static const int CURRENT_SYNTAX_VERSION = 2;
// Make sure an appropriate DTD is available in www/koffice/DTD if changing this value
static const char * CURRENT_DTD_VERSION = "1.1";

/******************************************************************/
/* class KPresenterChild					  */
/******************************************************************/

/*====================== constructor =============================*/
KPresenterChild::KPresenterChild( KPresenterDoc *_kpr, KoDocument* _doc, const QRect& _rect )
    : KoDocumentChild( _kpr, _doc, _rect )
{
    m_parent = _kpr;
}

/*====================== constructor =============================*/
KPresenterChild::KPresenterChild( KPresenterDoc *_kpr ) :
    KoDocumentChild( _kpr )
{
    m_parent = _kpr;
}

/*====================== destructor ==============================*/
KPresenterChild::~KPresenterChild()
{
}

KoDocument *KPresenterChild::hitTest( const QPoint &, const QWMatrix & )
{
    return 0L;
}

/******************************************************************/
/* class KPresenterDoc						  */
/******************************************************************/

/*====================== constructor =============================*/
KPresenterDoc::KPresenterDoc( QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name, bool singleViewMode )
    : KoDocument( parentWidget,widgetName, parent, name, singleViewMode ),
      _gradientCollection(),
      _hasHeader( false ),
      _hasFooter( false ),
      m_pixmapMap( NULL ),
      m_clipartMap( NULL ),
      m_unit( KoUnit::U_MM )
{
    setInstance( KPresenterFactory::global() );
    //Necessary to define page where we load object otherwise copy-duplicate page doesn't work.
    m_pageWhereLoadObject=0L;
    bgObjSpellChecked = 0L;
    m_tabStop = MM_TO_POINT( 15.0 );
    m_styleColl=new KoStyleCollection();

    KoStyle* m_standardStyle = new KoStyle( "Standard" );
    m_styleColl->addStyleTemplate( m_standardStyle );

    KConfig *config = KPresenterFactory::global()->config();
    config->setGroup("Document defaults" );
    QString defaultFontname=config->readEntry("DefaultFont");
    if ( !defaultFontname.isEmpty() )
        m_defaultFont.fromString( defaultFontname );
    // If not found, we automatically fallback to the application font (the one from KControl's font module)

    // Try to force a scalable font.
    m_defaultFont.setStyleStrategy( QFont::ForceOutline );
    //kdDebug() << "Default font: requested family: " << m_defaultFont.family() << endl;
    //kdDebug() << "Default font: real family: " << QFontInfo(m_defaultFont).family() << endl;

    int ptSize = m_defaultFont.pointSize();
    if ( ptSize == -1 ) // specified with a pixel size ?
        ptSize = QFontInfo(m_defaultFont).pointSize();
    //kdDebug() << "KPresenterDoc::KPresenterDoc[2] ptSize=" << ptSize << endl;
    // Ok, this is KPresenter. A default font of 10 makes no sense. Let's go for 20.
    ptSize = QMAX( 20, ptSize );

    // Zoom its size to layout units
    m_defaultFont.setPointSize( m_zoomHandler->ptToLayoutUnitPt( ptSize ) );

    m_standardStyle->format().setFont( m_defaultFont );
    m_zoomHandler = new KoZoomHandler;

    m_varFormatCollection = new KoVariableFormatCollection;
    m_varColl=new KPrVariableCollection( new KoVariableSettings());
    m_bgSpellCheck = new KPrBgSpellCheck(this);

    dcop = 0;
    m_kpresenterView = 0;
    m_initialActivePage=0;
    m_bShowStatusBar = true;
    m_autoFormat = new KoAutoFormat(this,m_varColl,m_varFormatCollection);
    _clean = true;
    _spInfinitLoop = false;
    _spManualSwitch = true;
    _showPresentationDuration = false;
    tmpSoundFileList = QPtrList<KTempFile>();
    _xRnd = 20;
    _yRnd = 20;
    _txtBackCol = lightGray;
    _otxtBackCol = lightGray;
    m_pKSpellConfig=0;
    m_bDontCheckUpperWord=false;
    m_bDontCheckTitleCase=false;
    m_bShowRuler=true;
    m_bAllowAutoFormat = true;

    m_bShowHelplines = false;
    m_bHelplinesToFront = false;
    m_bShowGrid = false;
    m_bGridToFont = false;

    m_bSnapToGrid= false;

    usedSoundFile = QStringList();
    haveNotOwnDiskSoundFile = QStringList();

    m_zoomHandler->setZoomAndResolution( 100, QPaintDevice::x11AppDpiX(), QPaintDevice::x11AppDpiY() );
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
    m_gridX = MM_TO_POINT( 10.0 );
    m_gridY = MM_TO_POINT( 10.0 );

    KPrPage *newpage=new KPrPage(this);
    m_pageList.insert( 0,newpage);
    emit sig_changeActivePage(newpage );
    m_stickyPage=new KPrPage(this);

    objStartY = 0;
    setPageLayout( m_pageLayout );
    _presPen = QPen( red, 3, SolidLine );
    presSpeed = 2;
    ignoreSticky = TRUE;
    m_pixmapMap = 0L;
    raiseAndLowerObject = false;

    m_gridColor=Qt::black;

    _header = new KPTextObject( this );
    _header->setDrawEditRect( false );
    _header->setDrawEmpty( false );

    _footer = new KPTextObject( this );
    _footer->setDrawEditRect( false );
    _footer->setDrawEmpty( false );


    saveOnlyPage = -1;
    m_maxRecentFiles = 10;

    initConfig();

    connect( QApplication::clipboard(), SIGNAL( dataChanged() ),
             this, SLOT( clipboardDataChanged() ) );

    m_commandHistory = new KCommandHistory( actionCollection(),  false ) ;
    connect( m_commandHistory, SIGNAL( documentRestored() ), this, SLOT( slotDocumentRestored() ) );
    connect( m_commandHistory, SIGNAL( commandExecuted() ), this, SLOT( slotCommandExecuted() ) );

    connect(m_varColl,SIGNAL(repaintVariable()),this,SLOT(slotRepaintVariable()));
    connect( documentInfo(), SIGNAL( sigDocumentInfoModifed()),this,SLOT(slotDocumentInfoModifed() ) );

    if ( name )
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
    KConfig *config = KPresenterFactory::global()->config();
    config->setGroup( "Interface" );
    config->writeEntry( "Zoom", m_zoomHandler->zoom() );
    config->writeEntry( "AllowAutoFormat" , m_bAllowAutoFormat );
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
        // Config-file value in mm, default 10 pt
        double indent =  config->readDoubleNumEntry("Indent", MM_TO_POINT(10.0) ) ;
        setIndentValue(indent);
        m_maxRecentFiles = config->readNumEntry( "NbRecentFile", 10 );
        setShowRuler(config->readBoolEntry("Rulers",true));
        zoom = config->readNumEntry( "Zoom", 100 );
        setShowStatusBar( config->readBoolEntry( "ShowStatusBar" , true ));
        setAllowAutoFormat( config->readBoolEntry( "AllowAutoFormat" , true ));
        setShowGrid( config->readBoolEntry( "ShowGrid" , false ));
        setGridToFront(  config->readBoolEntry( "GridToFront" , false ));
        setSnapToGrid( config->readBoolEntry( "SnapToGrid", true ));
        setGridX( config->readDoubleNumEntry( "ResolutionX", MM_TO_POINT( 10.0 ) ));
        setGridY( config->readDoubleNumEntry( "ResolutionY", MM_TO_POINT( 10.0 ) ));

        setHelpLineToFront( config->readBoolEntry( "HelpLineToFront" , false ));
    }
    else
        zoom=100;

    QColor oldBgColor = Qt::white;
    QColor oldGridColor = Qt::black;
    if(  config->hasGroup( "KPresenter Color" ) ) {
        config->setGroup( "KPresenter Color" );
        setTxtBackCol(config->readColorEntry( "BackgroundColor", &oldBgColor ));
        setGridColor(config->readColorEntry( "GridColor", &oldGridColor ));
    }

    KSpellConfig ksconfig;

    if( config->hasGroup("KSpell kpresenter" ) )
    {
        config->setGroup( "KSpell kpresenter" );
        ksconfig.setNoRootAffix(config->readNumEntry ("KSpell_NoRootAffix", 0));
        ksconfig.setRunTogether(config->readNumEntry ("KSpell_RunTogether", 0));
        ksconfig.setDictionary(config->readEntry ("KSpell_Dictionary", ""));
        ksconfig.setDictFromList(config->readNumEntry ("KSpell_DictFromList", FALSE));
        ksconfig.setEncoding(config->readNumEntry ("KSpell_Encoding", KS_E_ASCII));
        ksconfig.setClient(config->readNumEntry ("KSpell_Client", KS_CLIENT_ISPELL));
        setKSpellConfig(ksconfig);
        setDontCheckUpperWord(config->readBoolEntry("KSpell_dont_check_upper_word",false));
        setDontCheckTitleCase(config->readBoolEntry("KSpell_dont_check_title_case",false));
        m_bgSpellCheck->enableBackgroundSpellCheck(config->readBoolEntry( "SpellCheck", false ));

    }

    if(config->hasGroup("Misc" ) )
    {
        config->setGroup( "Misc" );
        int undo=config->readNumEntry("UndoRedo",-1);
        if(undo!=-1)
            setUndoRedoLimit(undo);
    }

    // Apply configuration, without creating an undo/redo command
    replaceObjs( false );
    zoomHandler()->setZoomAndResolution( zoom, QPaintDevice::x11AppDpiX(), QPaintDevice::x11AppDpiY() );
    newZoomAndResolution(false,false);


}

/*==============================================================*/
DCOPObject* KPresenterDoc::dcopObject()
{
    if ( !dcop )
	dcop = new KPresenterDocIface( this );

    return dcop;
}

/*==============================================================*/
KPresenterDoc::~KPresenterDoc()
{
    if(isReadWrite())
        saveConfig();
    //_commands.clear(); // done before deleting the objectlist (e.g. for lowraicmd)
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
    delete m_stickyPage;
    delete m_bgSpellCheck;
    delete m_styleColl;
    delete m_pKSpellConfig;
    m_pageList.setAutoDelete( true );
    m_pageList.clear();
    kdDebug()<<" delete m_deletedPageList \n";
    m_deletedPageList.setAutoDelete( true );
    m_deletedPageList.clear();
    tmpSoundFileList.setAutoDelete( true );
    tmpSoundFileList.clear();
}


void KPresenterDoc::addCommand( KCommand * cmd )
{
    kdDebug() << "KPresenterDoc::addCommand " << cmd->name() << endl;
    m_commandHistory->addCommand( cmd, false );
    setModified( true );
}

/*======================= make child list intern ================*/
bool KPresenterDoc::saveChildren( KoStore* _store )
{
    int i = 0;

    if ( saveOnlyPage == -1 ) // Don't save all children into template for one page
        // ###### TODO: save objects that are on that page
    {
        QPtrListIterator<KoDocumentChild> it( children() );
        for( ; it.current(); ++it ) {
            // Don't save children that are only in the undo/redo history
            // but not anymore in the presentation
            KPrPage *page;
            for ( page = m_pageList.first(); page ; page = m_pageList.next() )
            {
                QPtrListIterator<KPObject> oIt(page->objectList());
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
            QPtrListIterator<KPObject> oIt(m_stickyPage->objectList());
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

/*========================== save ===============================*/
QDomDocument KPresenterDoc::saveXML()
{
    if ( saveOnlyPage == -1 ) {
        emit sigStartProgressForSaveFile();
        emit sigProgress( 0 );
    }

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

    getVariableCollection()->variableSetting()->save(presenter );

    presenter.appendChild(saveAttribute( doc ));

    if ( saveOnlyPage == -1 )
        emit sigProgress( 5 );

    QDomElement element=doc.createElement("BACKGROUND");
#if 0
    element.setAttribute("color", _txtBackCol.name());
#endif
    element.appendChild(saveBackground( doc ));
    presenter.appendChild(element);

    if ( saveOnlyPage == -1 )
        emit sigProgress( 10 );

    element=doc.createElement("HEADER");
    element.setAttribute("show", static_cast<int>( hasHeader() ));
    element.appendChild(_header->save( doc,0 ));
    presenter.appendChild(element);

    element=doc.createElement("FOOTER");
    element.setAttribute("show", static_cast<int>( hasFooter() ));
    element.appendChild(_footer->save( doc,0 ));
    presenter.appendChild(element);

    element=doc.createElement("HELPLINES");
    element.setAttribute("show", static_cast<int>( showHelplines() ));
    element.appendChild( saveHelpLines( doc ));
    presenter.appendChild(element);

    if ( saveOnlyPage == -1 )
    {
        if( !m_spellListIgnoreAll.isEmpty() )
        {
            QDomElement spellCheckIgnore = doc.createElement( "SPELLCHECKIGNORELIST" );
            presenter.appendChild( spellCheckIgnore );
            for ( QStringList::Iterator it = m_spellListIgnoreAll.begin(); it != m_spellListIgnoreAll.end(); ++it )
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
    element.setAttribute("value", _spInfinitLoop);
    presenter.appendChild(element);
    element=doc.createElement("MANUALSWITCH");
    element.setAttribute("value", _spManualSwitch);
    presenter.appendChild(element);
    element=doc.createElement("PRESSPEED");
    element.setAttribute("value", static_cast<int>( presSpeed ));
    presenter.appendChild(element);
    element=doc.createElement("SHOWPRESENTATIONDURATION");
    element.setAttribute("value", _showPresentationDuration);
    presenter.appendChild(element);

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
        QPtrList<KoStyle> m_styleList(m_styleColl->styleList());
        for ( KoStyle * p = m_styleList.first(); p != 0L; p = m_styleList.next() )
            saveStyle( p, styles );

        emit sigProgress( 60 );
    }

    // Write "OBJECT" tag for every child
    QPtrListIterator<KoDocumentChild> chl( children() );
    for( ; chl.current(); ++chl ) {
        // Don't save children that are only in the undo/redo history
        // but not anymore in the presentation
        for ( int i = 0; i < static_cast<int>( m_pageList.count() ); i++ ) {
            if ( saveOnlyPage != -1 &&
                 i != saveOnlyPage )
                continue;
            double offset=i*m_pageList.at(i)->getPageRect().height();
            saveEmbeddedObject(m_pageList.at(i), chl.current(),doc,presenter,offset );
        }
        saveEmbeddedObject(m_stickyPage, chl.current(),doc,presenter,0.0 );
    }

    if ( saveOnlyPage == -1 )
        emit sigProgress( 70 );

    makeUsedPixmapList();

    QDomElement pixmaps = _imageCollection.saveXML( KoPictureCollection::CollectionImage, doc, usedPixmaps );
    presenter.appendChild( pixmaps );

    if ( saveOnlyPage == -1 )
        emit sigProgress( 80 );

    QDomElement cliparts = _clipartCollection.saveXML( KoPictureCollection::CollectionClipart, doc, usedCliparts );
    presenter.appendChild( cliparts );

    if ( saveOnlyPage == -1 )
        emit sigProgress( 90 );

    // Save sound file list.
    makeUsedSoundFileList();
    QDomElement soundFiles = saveUsedSoundFileToXML( doc, usedSoundFile );
    presenter.appendChild( soundFiles );

    setModified( false );
    return doc;
}


void KPresenterDoc::saveEmbeddedObject(KPrPage *page, KoDocumentChild *chl,QDomDocument &doc,QDomElement &presenter, double offset )
{
    QPtrListIterator<KPObject> oIt(page->objectList());
    for (; oIt.current(); ++oIt )
    {
        if ( oIt.current()->getType() == OT_PART &&
             dynamic_cast<KPPartObject*>( oIt.current() )->getChild() == chl )
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
            if (  oIt.current()->isSticky() )
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

/*===============================================================*/
void KPresenterDoc::enableEmbeddedParts( bool f )
{
    KPrPage *page=0L;
    for(page=m_pageList.first(); page; page=m_pageList.next())
        page->enableEmbeddedParts(f);
}

/*========================== save background ====================*/
QDomDocumentFragment KPresenterDoc::saveBackground( QDomDocument &doc )
{
    KPBackGround *kpbackground = 0;
    QDomDocumentFragment fragment=doc.createDocumentFragment();
    for ( int i = 0; i < static_cast<int>( m_pageList.count() ); i++ ) {
	if ( saveOnlyPage != -1 &&
	     i != saveOnlyPage )
	    continue;
	kpbackground = m_pageList.at(i)->background();
	fragment.appendChild(kpbackground->save( doc ));
    }
    return fragment;
}

/*========================== save objects =======================*/
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
    //offset = 0.0 when it's a sticky page.
    objects=m_stickyPage->saveObjects( doc, objects, /*yoffset*/0.0, m_zoomHandler, saveOnlyPage );

    return objects;
}

/*========================== save page title ====================*/
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

/*===================== save page note ========================*/
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
    int activePage=m_pageList.findRef(m_kpresenterView->getCanvas()->activePage());
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

/*==============================================================*/
bool KPresenterDoc::completeSaving( KoStore* _store )
{
    if ( !_store ) {
        if ( saveOnlyPage == -1 ) {
            emit sigProgress( 100 );
            emit sigProgress( -1 );
            emit sigStopProgressForSaveFile();
        }
	return true;
    }
    _imageCollection.saveToStore( KoPictureCollection::CollectionImage, _store, usedPixmaps );
    _clipartCollection.saveToStore( KoPictureCollection::CollectionClipart, _store, usedCliparts );
    saveUsedSoundFileToStore( _store, usedSoundFile );

    if ( saveOnlyPage == -1 ) {
        emit sigProgress( 100 );
        emit sigProgress( -1 );
        emit sigStopProgressForSaveFile();
    }

    return true;
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


/*========================== load ===============================*/
bool KPresenterDoc::loadChildren( KoStore* _store )
{
    if ( objStartY == 0 ) // Don't do this when inserting a template or a page...
    {
      QPtrListIterator<KoDocumentChild> it( children() );
      for( ; it.current(); ++it ) {
        if ( !((KoDocumentChild*)it.current())->loadDocument( _store ) )
          return false;
      }
    }
    return true;
}

bool KPresenterDoc::loadXML( QIODevice * dev, const QDomDocument& doc )
{
    QTime dt;
    dt.start();

    ignoreSticky = FALSE;
    bool b=false;
    QDomElement docelem = doc.documentElement();
    int syntaxVersion = docelem.attribute( "syntaxVersion" ).toInt();
    if ( (syntaxVersion == 0 || syntaxVersion == 1) && CURRENT_SYNTAX_VERSION > 1 )
    {
	// This is an old style document, before the current TextObject
	// We have kprconverter.pl for it
	kdWarning() << "KPresenter document version 1. Launching perl script to convert it." << endl;

	// Read the full XML and write it to a temp file
	KTempFile tmpFileIn;
	tmpFileIn.setAutoDelete( true );
	{
	    dev->reset();
	    QByteArray array = dev->readAll();
	    *tmpFileIn.textStream() << (const char*)array.data();
	}
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
	cmd += locate( "exe", "kprconverter.pl" ) + " ";
	cmd += tmpFileIn.name() + " ";
	cmd += tmpFileOut.name();
	system( QFile::encodeName(cmd) );

	// Build a new QDomDocument from the result
	QDomDocument newdoc;
	newdoc.setContent( tmpFileOut.file() );
	b = loadXML( newdoc );
	ignoreSticky = TRUE;
    }
    else
    {
	b = loadXML( doc );
	ignoreSticky = TRUE;
    }
    if(_clean)
    {
        setModified(false);
        startBackgroundSpellCheck();
    }

    kdDebug() << "Loading took " << (float)(dt.elapsed()) / 1000 << " seconds" << endl;
    return b;
}

void KPresenterDoc::createHeaderFooter()
{
    //add header/footer to sticky page
    KoRect pageRect=m_stickyPage->getPageRect();
    _header->setOrig(pageRect.topLeft ());
    _header->setSize(pageRect.width(),20); //80 why not ?

    _footer->setOrig(pageRect.left(),pageRect.bottom()-80);
    _footer->setSize(pageRect.width(),20); //80 why not ?

    m_stickyPage->appendObject(_header);
    m_stickyPage->appendObject(_footer);
}

/*========================== load ===============================*/
bool KPresenterDoc::loadXML( const QDomDocument &doc )
{
    emit sigProgress( 0 );
    int activePage=0;
    delete m_pixmapMap;
    m_pixmapMap = NULL;
    delete m_clipartMap;
    m_clipartMap = NULL;
    lastObj = -1;
    bool allSlides = false;
    m_spellListIgnoreAll.clear();
    // clean
    if ( _clean ) {
        //KoPageLayout __pgLayout;
        __pgLayout = KoPageLayoutDia::standardLayout();
        //__pgLayout.unit = KoUnit::U_MM;
        _spInfinitLoop = false;
        _spManualSwitch = true;
        _showPresentationDuration = false;
        _xRnd = 20;
        _yRnd = 20;
        //_txtBackCol = white;
        urlIntern = url().path();
    }

    emit sigProgress( 5 );

    QDomElement document=doc.documentElement();
    // DOC
    if(document.tagName()!="DOC") {
        kdWarning() << "Missing DOC" << endl;
        setErrorMessage( i18n("Invalid document, DOC tag missing") );
        return false;
    }

    if(!document.hasAttribute("mime") ||  (
                document.attribute("mime")!="application/x-kpresenter" && document.attribute("mime")!="application/vnd.kde.kpresenter" ) ) {
        kdError() << "Unknown mime type " << document.attribute("mime") << endl;
        setErrorMessage( i18n("Invalid document, expected mimetype application/x-kpresenter or application/vnd.kde.kpresenter, got %1").arg(document.attribute("mime")) );
        return false;
    }
    if(document.hasAttribute("url"))
        urlIntern=KURL(document.attribute("url")).path();

    emit sigProgress( 10 );

    QDomElement elem=document.firstChild().toElement();

    uint childTotalCount=document.childNodes().count();
    uint childCount = 0;

    while(!elem.isNull()) {
        kdDebug() << "Element name: " << elem.tagName() << endl;
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
            if(settings.hasAttribute("sticky"))
                tmp=settings.attribute("sticky").toInt();
            bool sticky=static_cast<bool>(tmp);
            double offset = 0.0;
            if(!settings.isNull() && kppartobject!=0)
                offset=kppartobject->load(settings);
            else if ( settings.isNull() ) // all embedded obj must have SETTING tags
            {
                delete kppartobject;
                kppartobject = 0L;
            }
            if ( sticky && !ignoreSticky && kppartobject )
            {
                m_stickyPage->appendObject(kppartobject );
                kppartobject->setOrig(r.x(), offset);
                kppartobject->setSize( r.width(), r.height() );
                kppartobject->setSticky(sticky);
            }
            else if ( kppartobject ) {
                kppartobject->setOrig( r.x(), 0 );
                kppartobject->setSize( r.width(), r.height() );
                insertObjectInPage(offset, kppartobject);
            }
        } else if(elem.tagName()=="PAPER")  {
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
        } else if(elem.tagName()=="VARIABLESETTINGS"){
            getVariableCollection()->variableSetting()->load(document);
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
#if 0
            if(elem.hasAttribute("color"))
                _txtBackCol.setNamedColor(elem.attribute("color"));
            else
                _txtBackCol.setRgb(red, green, blue);
#endif
            loadBackground(elem);
        } else if(elem.tagName()=="HEADER") {
            if ( _clean /*don't reload header footer, header/footer was created at the beginning || !hasHeader()*/ ) {
                if(elem.hasAttribute("show")) {
                    setHeader(static_cast<bool>(elem.attribute("show").toInt()));
                }
                _header->load(elem);
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
                    m_spellListIgnoreAll.append(spellWord.attribute("word"));
                }
                spellWord=spellWord.nextSibling().toElement();
            }
            m_bgSpellCheck->addIgnoreWordAllList( m_spellListIgnoreAll );
        }else if(elem.tagName()=="ATTRIBUTES") {
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
        } else if( _clean && elem.tagName()=="STYLES"){
            // Load all styles before the corresponding paragraphs try to use them!
            loadStyleTemplates( elem );
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
                    _spInfinitLoop = static_cast<bool>(elem.attribute("value").toInt());
            }
        } else if(elem.tagName()=="PRESSPEED") {
            if(_clean) {
                if(elem.hasAttribute("value"))
                    presSpeed = static_cast<PresSpeed>(elem.attribute("value").toInt());
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
                                    m_pageList.append(new KPrPage(this));
                            }
                            m_pageList.at(nr)->slideSelected(show);
                        } else kdWarning() << "Parse error. No nr in <SLIDE> !" << endl;
                    }
                    slide=slide.nextSibling().toElement();
                }
            }
        } else if(elem.tagName()=="PIXMAPS") {
            m_pixmapMap = new QMap<KoPictureKey, QString>( _imageCollection.readXML( elem ) );
        } else if(elem.tagName()=="CLIPARTS") {
            m_clipartMap = new QMap<KoPictureKey, QString>( _clipartCollection.readXML( elem ) );
        } else if ( elem.tagName() == "SOUNDS" ) {
            loadUsedSoundFileFromXML( elem );
        }
        elem=elem.nextSibling().toElement();

        emit sigProgress( childCount * ( 70/childTotalCount ) + 15 );
        childCount += 1;
    }

    if(activePage!=-1)
        m_initialActivePage=m_pageList.at(activePage);
    setModified(false);

    return true;
}

/*====================== load background =========================*/
void KPresenterDoc::loadBackground( const QDomElement &element )
{
    kdDebug(33001) << "KPresenterDoc::loadBackground" << endl;
    QDomElement page=element.firstChild().toElement();
    int i=0;
    while(!page.isNull()) {
        if(m_pageWhereLoadObject)
            m_pageWhereLoadObject->background()->load(page);
        else
        {
            //test if there is a page at this index
            //=> don't add new page if there is again a page
            if ( i > ( (int)m_pageList.count() - 1 ) )
                m_pageList.append(new KPrPage(this));
            m_pageList.at(i)->background()->load(page);
            i++;
        }
         page=page.nextSibling().toElement();
    }
}

/*========================= load objects =========================*/
KCommand *KPresenterDoc::loadObjects( const QDomElement &element,bool paste )
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
                    m_stickyPage->appendObject(kplineobject);
                    kplineobject->setOrig(kplineobject->getOrig().x(),offset);
                    kplineobject->setSticky(sticky);
                }
                else if (m_pageWhereLoadObject && paste) {
                    kplineobject->setOrig(kplineobject->getOrig().x(),offset);
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Line" ), kplineobject, this,m_pageWhereLoadObject );
                    macro->addCommand( insertCmd );
                    createMacro=true;
                    //insertCmd->execute();
                    //addCommand( insertCmd );
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
                    m_stickyPage->appendObject(kprectobject);
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
                    m_stickyPage->appendObject(kpellipseobject);
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
                    m_stickyPage->appendObject(kppieobject);
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
                    m_stickyPage->appendObject(kpautoformobject);
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
            case OT_CLIPART: {
                KPClipartObject *kpclipartobject = new KPClipartObject( &_clipartCollection );
                offset=kpclipartobject->load(obj);
                if ( sticky && !ignoreSticky)
                {
                    m_stickyPage->appendObject(kpclipartobject);
                    kpclipartobject->setOrig(kpclipartobject->getOrig().x(),offset);
                    kpclipartobject->setSticky(sticky);
                }
                else if ( m_pageWhereLoadObject && paste) {
                    kpclipartobject->setOrig(kpclipartobject->getOrig().x(),offset);
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Clipart" ), kpclipartobject, this , m_pageWhereLoadObject);
                    macro->addCommand( insertCmd );
                    kpclipartobject->reload();
                    createMacro=true;
#if 0
                    insertCmd->execute();
                    addCommand( insertCmd );
                    kpclipartobject->reload();
#endif
                }
                else if( m_pageWhereLoadObject &&!paste)
                {
                    m_pageWhereLoadObject->appendObject(kpclipartobject);
                    kpclipartobject->setOrig(kpclipartobject->getOrig().x(),offset);
                }
                else
                    insertObjectInPage(offset, kpclipartobject);
            } break;
            case OT_TEXT: {
                KPTextObject *kptextobject = new KPTextObject( this );
                offset=kptextobject->load(obj);
                if ( sticky && !ignoreSticky)
                {
                    m_stickyPage->appendObject(kptextobject);
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
            case OT_PICTURE: {
                KPPixmapObject *kppixmapobject = new KPPixmapObject( &_imageCollection );
                offset=kppixmapobject->load(obj);
                if ( sticky && !ignoreSticky)
                {
                    m_stickyPage->appendObject(kppixmapobject);
                    kppixmapobject->setOrig(kppixmapobject->getOrig().x(),offset);
                    kppixmapobject->setSticky(sticky);
                }
                else if ( m_pageWhereLoadObject && paste) {
                    kppixmapobject->setOrig(kppixmapobject->getOrig().x(),offset);
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Picture" ), kppixmapobject, this, m_pageWhereLoadObject );
                    macro->addCommand( insertCmd );
                    kppixmapobject->reload();
                    createMacro=true;

#if 0
                    insertCmd->execute();
                    addCommand( insertCmd );
                    kppixmapobject->reload();
#endif
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
                    m_stickyPage->appendObject(kpfreehandobject);
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
                    m_stickyPage->appendObject(kppolylineobject);
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
                    m_stickyPage->appendObject(kpQuadricBezierCurveObject);
                    kpQuadricBezierCurveObject->setOrig(kpQuadricBezierCurveObject->getOrig().x(),offset);
                    kpQuadricBezierCurveObject->setSticky(sticky);
                }
                else if ( m_pageWhereLoadObject && paste) {
                    kpQuadricBezierCurveObject->setOrig(kpQuadricBezierCurveObject->getOrig().x(),offset);
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Quadric Bezier Curve" ), kpQuadricBezierCurveObject, this, m_pageWhereLoadObject );
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
                    m_stickyPage->appendObject(kpCubicBezierCurveObject);
                    kpCubicBezierCurveObject->setOrig(kpCubicBezierCurveObject->getOrig().x(),offset);
                    kpCubicBezierCurveObject->setSticky(sticky);
                }
                else if ( m_pageWhereLoadObject && paste) {
                    kpCubicBezierCurveObject->setOrig(kpCubicBezierCurveObject->getOrig().x(),offset);
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Cubic Bezier Curve" ), kpCubicBezierCurveObject, this, m_pageWhereLoadObject );
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
                    m_stickyPage->appendObject(kpPolygonObject);
                    kpPolygonObject->setOrig(kpPolygonObject->getOrig().x(),offset);
                    kpPolygonObject->setSticky(sticky);
                }
                else if ( m_pageWhereLoadObject && paste) {
                    kpPolygonObject->setOrig(kpPolygonObject->getOrig().x(),offset);
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Polygon" ), kpPolygonObject, this , m_pageWhereLoadObject);
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
                    m_stickyPage->appendObject( kpClosedLinneObject );
                    kpClosedLinneObject->setOrig( kpClosedLinneObject->getOrig().x(), offset );
                    kpClosedLinneObject->setSticky( sticky );
                }
                else if ( m_pageWhereLoadObject && paste ) {
                    kpClosedLinneObject->setOrig( kpClosedLinneObject->getOrig().x(), offset );
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert " ) + kpClosedLinneObject->getTypeString(), kpClosedLinneObject, this , m_pageWhereLoadObject );
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
                    m_stickyPage->appendObject(kpgroupobject);
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

/*========================= load page title =========================*/
void KPresenterDoc::loadTitle( const QDomElement &element )
{
    QDomElement title=element.firstChild().toElement();
    int i=0;
    while ( !title.isNull() ) {
        if ( title.tagName()=="Title" )
        {
            //test if there is a page at this index
            //=> don't add new page if there is again a page
            if(!m_pageWhereLoadObject)
            {
                if ( i > ( (int)m_pageList.count() - 1 ) )
                    m_pageList.append(new KPrPage(this));
                m_pageList.at(i)->insertManualTitle(title.attribute("title"));
                i++;
            }
            else
                m_pageWhereLoadObject->insertManualTitle(title.attribute("title"));
        }
        title=title.nextSibling().toElement();
    }
}

/*========================= load page note =========================*/
void KPresenterDoc::loadNote( const QDomElement &element )
{
    QDomElement note=element.firstChild().toElement();
    int i=0;
    while ( !note.isNull() ) {
        if ( note.tagName()=="Note" )
        {
            //test if there is a page at this index
            //=> don't add new page if there is again a page
            if(!m_pageWhereLoadObject)
            {
                if ( i > ( (int)m_pageList.count() - 1 ) )
                    m_pageList.append(new KPrPage(this));
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

/*===================================================================*/
bool KPresenterDoc::completeLoading( KoStore* _store )
{
    if ( _store ) {
        if ( m_pixmapMap ) {
            _imageCollection.readFromStore( _store, *m_pixmapMap );
            delete m_pixmapMap;
            m_pixmapMap = NULL;
        }
        emit sigProgress( 80 );

        if ( m_clipartMap ) {
            _clipartCollection.readFromStore( _store, *m_clipartMap );
            delete m_clipartMap;
            m_clipartMap = NULL;
        }
        emit sigProgress( 90 );

        if ( !usedSoundFile.isEmpty() )
            loadUsedSoundFileFromStore( _store, usedSoundFile );

	if ( _clean )
        {
	    setPageLayout( __pgLayout );
            createHeaderFooter();
        }
	//else {
            //m_pageList.last()->updateBackgroundSize();
	//}


        if ( saveOnlyPage == -1 ) {
            KPrPage *page;
            for ( page = m_pageList.first(); page ; page = m_pageList.next() )
                page->completeLoading( _clean, lastObj );
        }
    } else {
	if ( _clean )
        {
	    setPageLayout( __pgLayout );
        }
	else
	    setPageLayout( m_pageLayout );
    }

    emit sigProgress( 100 );
    recalcVariables( VT_FIELD );
    emit sigProgress( -1 );

    return true;
}

void KPresenterDoc::loadUsedSoundFileFromStore( KoStore *_store, QStringList _list )
{
    int i = 0;
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

/*===================== set page layout ==========================*/
void KPresenterDoc::setPageLayout( KoPageLayout pgLayout )
{
    //     if ( _pageLayout == pgLayout )
    //	return;

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
    KoRect pageRect=m_stickyPage->getPageRect();
    QRect oldBoundingRect=zoomHandler()->zoomRect(_header->getBoundingRect(zoomHandler()));
    _header->setOrig(pageRect.topLeft ());
    _header->setSize(pageRect.width(),_header->getSize().height());
    repaint( oldBoundingRect );
    repaint(_header);

    oldBoundingRect=zoomHandler()->zoomRect(_footer->getBoundingRect(zoomHandler()));
    _footer->setOrig(pageRect.left(),pageRect.bottom()-_footer->getSize().height());
    _footer->setSize(pageRect.width(),_footer->getSize().height());
    repaint(oldBoundingRect);
    repaint(_footer);
}

bool KPresenterDoc::insertNewTemplate( bool clean )
{
    QString _template;
    KoTemplateChooseDia::ReturnType ret;

    ret = KoTemplateChooseDia::choose(	KPresenterFactory::global(), _template,
					"application/x-kpresenter", "*.kpr",
					i18n("KPresenter"), KoTemplateChooseDia::Everything,
					"kpresenter_template" );

    if ( ret == KoTemplateChooseDia::Template ) {
	QFileInfo fileInfo( _template );
	QString fileName( fileInfo.dirPath( true ) + "/" + fileInfo.baseName() + ".kpt" );
	_clean = clean;
	bool ok = loadNativeFormat( fileName );
	objStartY = 0;
	_clean = true;
	resetURL();
        setEmpty();
	return ok;
    } else if ( ret == KoTemplateChooseDia::File ) {
	objStartY = 0;
	_clean = true;
	KURL url;
	url.setPath( _template );
	bool ok = openURL( url );
	return ok;
    } else if ( ret == KoTemplateChooseDia::Empty ) {
	QString fileName( locate("kpresenter_template", "Screenpresentations/.source/Plain.kpt",
				 KPresenterFactory::global() ) );
	objStartY = 0;
	_clean = true;
	bool ok = loadNativeFormat( fileName );
	resetURL();
        setEmpty();
	return ok;
    } else
	return false;
}

/*================================================================*/
void KPresenterDoc::initEmpty()
{
    QString fileName( locate("kpresenter_template", "Screenpresentations/.source/Plain.kpt",
			     KPresenterFactory::global() ) );
    objStartY = 0;
    _clean = true;
    setModified(true);
    loadNativeFormat( fileName );
    resetURL();
}

/*======================= set rasters ===========================*/
void KPresenterDoc::setGridValue( double _x, double _y, bool _replace )
{
    oldGridX = m_gridX;
    oldGridY = m_gridY;
    m_gridX=_x;
    m_gridY=_y;
    if ( _replace )
      replaceObjs();
}

/*=================== repaint all views =========================*/
void KPresenterDoc::repaint( bool erase )
{
    QPtrListIterator<KoView> it( views() );
    for( ; it.current(); ++it ) {
	KPrCanvas* canvas = ((KPresenterView*)it.current())->getCanvas();
	canvas->repaint( erase );
    }
}

/*===================== repaint =================================*/
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

/*===================== repaint =================================*/
void KPresenterDoc::repaint( KPObject *kpobject )
{
    repaint( m_zoomHandler->zoomRect(kpobject->getBoundingRect(m_zoomHandler)) );
}

QValueList<int> KPresenterDoc::reorderPage( unsigned int num )
{
    return m_pageList.at(num)->reorderPage();
}

/*================== get size of page ===========================*/
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

/*================================================================*/
int KPresenterDoc::getLeftBorder() const
{
    return static_cast<int>(m_pageLayout.ptLeft);
}

/*================================================================*/
int KPresenterDoc::getTopBorder() const
{
    return static_cast<int>(m_pageLayout.ptTop);
}

/*================================================================*/
int KPresenterDoc::getBottomBorder() const
{
    return static_cast<int>(m_pageLayout.ptBottom);
}

int KPresenterDoc::getRightBorder() const
{
    return static_cast<int>(m_pageLayout.ptRight);
}



/*================================================================*/
void KPresenterDoc::deletePage( int _page )
{
    kdDebug(33001) << "KPresenterDoc::deletePage " << _page << endl;
    //m_pageList.at(_page)->deletePage();

    KPrDeletePageCmd *cmd=new KPrDeletePageCmd(i18n("Delete Page"),_page,m_pageList.at(_page),this);
    cmd->execute();
    addCommand(cmd);
}

void KPresenterDoc::insertPage( KPrPage *_page, int position)
{
    int pos=m_deletedPageList.findRef(_page);
    if ( pos != -1 )
        m_deletedPageList.take( pos);

    if ( m_deletedPageList.findRef( _page ) )
        m_deletedPageList.remove( _page );
    m_pageList.insert( position,_page);
    //active this page
    emit sig_changeActivePage(_page );
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
        static_cast<KPresenterView*>(it.current())->skipToPage(position);

}

void KPresenterDoc::takePage(KPrPage *_page)
{
    int pos=m_pageList.findRef(_page);
    m_pageList.take( pos);
    m_deletedPageList.append( _page );


    //active previous page
    emit sig_changeActivePage(_page );

    //repaint( false );

    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
        static_cast<KPresenterView*>(it.current())->skipToPage(pos-1);

    repaint( false );

    emit sig_updateMenuBar();
}

void KPresenterDoc::addRemovePage( int pos, bool addPage )
{
    kdDebug() << "addRemovePage pos = " << pos << endl;
    recalcPageNum();

    recalcVariables( VT_PGNUM );

    // Update the sidebars
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it ) {
        if ( addPage ) {
            static_cast<KPresenterView*>(it.current())->addSideBarItem( pos );
        }
        else {
            static_cast<KPresenterView*>(it.current())->removeSideBarItem( pos );
        }
    }

    //update statusbar
    emit pageNumChanged();
    emit sig_updateMenuBar();
}

void KPresenterDoc::movePageTo( int oldPos, int newPos )
{
    kdDebug() << "movePage oldPos = " << oldPos << ", neuPos = " << newPos << endl;
    recalcPageNum();

    recalcVariables( VT_PGNUM );

    // Update the sidebars
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
        static_cast<KPresenterView*>(it.current())->moveSideBarItem( oldPos, newPos );

    //update statusbar
    emit pageNumChanged();
    emit sig_updateMenuBar();
}

QString KPresenterDoc::templateFileName(bool chooseTemplate, const QString &theFile )
{
    QString fileName;
    if ( !chooseTemplate ) {
        if ( theFile.isEmpty() )
            fileName = locateLocal( "appdata", "default.kpr" );
        else
	    fileName = theFile;
    } else {
	QString _template;
	if ( KoTemplateChooseDia::choose(  KPresenterFactory::global(), _template,
					   "", QString::null, QString::null, KoTemplateChooseDia::OnlyTemplates,
					   "kpresenter_template") == KoTemplateChooseDia::Cancel )
	    return QString("");
	QFileInfo fileInfo( _template );
	fileName = fileInfo.dirPath( true ) + "/" + fileInfo.baseName() + ".kpt";
	QString cmd = "cp " + fileName + " " + locateLocal( "appdata", "default.kpr" );
	system( QFile::encodeName(cmd) );
    }
    return fileName;
}

int KPresenterDoc::insertNewPage( const QString &cmdName, int _page, InsertPos _insPos, bool chooseTemplate, const QString &theFile )
{
    kdDebug(33001) << "KPresenterDoc::insertNewPage " << _page << endl;

    QString fileName=templateFileName(chooseTemplate, theFile );
    if(fileName.isEmpty())
        return -1;

    _clean = false;

    if ( _insPos == IP_AFTER )
	_page++;

    objStartY=-1;

    //insert page.
    KPrPage *newpage=new KPrPage(this);

    m_pageWhereLoadObject=newpage;

    loadNativeFormat( fileName );

    objStartY = 0;

    KPrInsertPageCmd *cmd=new KPrInsertPageCmd(cmdName ,_page, newpage, this );
    cmd->execute();
    addCommand(cmd);

    _clean = true;
    m_pageWhereLoadObject=0L;
    return _page;
}

/*=============================================================*/
void KPresenterDoc::savePage( const QString &file, int pgnum )
{
    saveOnlyPage = pgnum;
    saveNativeFormat( file );
    saveOnlyPage = -1;
}


/*====================== replace objects =========================*/
void KPresenterDoc::replaceObjs( bool createUndoRedo )
{
    KMacroCommand * macroCmd = new KMacroCommand( i18n("Set new Options") );
    bool addMacroCommand=false;
    QPtrListIterator<KPrPage> oIt(m_pageList);
    for (; oIt.current(); ++oIt )
      {
        KCommand *cmd=oIt.current()->replaceObjs( createUndoRedo, oldGridX,oldGridY,_txtBackCol, _otxtBackCol);
        if(cmd && createUndoRedo)
        {
            macroCmd->addCommand(cmd);
            addMacroCommand=true;
        }
        else
            delete cmd;
    }
    if(addMacroCommand)
    {
        macroCmd->execute();
        addCommand(macroCmd);
    }
    else
        delete macroCmd;
}

/*========================= restore background ==================*/
void KPresenterDoc::restoreBackground( KPrPage *page )
{
    page->background()->reload();
}

/*==================== load pasted objects ==============================*/
KCommand * KPresenterDoc::loadPastedObjs( const QString &in,KPrPage* _page )
{
    QDomDocument doc;
    doc.setContent( in );

    QDomElement document=doc.documentElement();

    // DOC
    if (document.tagName()!="DOC") {
        kdError() << "Missing DOC" << endl;
        return 0L;
    }

    bool ok = false;

    if(document.hasAttribute("mime") && document.attribute("mime")=="application/x-kpresenter-selection")
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

/*================= deselect all objs ===========================*/
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
	((KPresenterView*)it.current())->getCanvas()->deSelectObj(obj );
}

void KPresenterDoc::setHeader( bool b )
{
    _hasHeader = b;
    _header->setDrawEditRect( b );
    _header->setDrawEmpty( b );
    if(!b)
    {
        terminateEditing(_header);
        deSelectObj(_header);
    }
    updateHeaderFooterButton();
    repaint(_hasHeader);
}

void KPresenterDoc::setFooter( bool b )
{
    _hasFooter = b;
    _footer->setDrawEditRect( b );
    _footer->setDrawEmpty( b );
    if(!b)
    {
        terminateEditing(_footer);
        deSelectObj(_footer);
    }
    updateHeaderFooterButton();
    repaint(_footer);
}

void KPresenterDoc::updateHeaderFooterButton()
{
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
	((KPresenterView*)it.current())->updateHeaderFooterButton();
}

void KPresenterDoc::makeUsedPixmapList()
{
    usedPixmaps.clear();
    usedCliparts.clear();

    for ( uint i = 0; i < m_pageList.count(); i++ ) {
	if ( saveOnlyPage != -1 &&
	     static_cast<int>(i) != saveOnlyPage )
	    continue;
        m_pageList.at(i)->makeUsedPixmapList();
    }
}

/*================================================================*/
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

/*================================================================*/
KoView* KPresenterDoc::createViewInstance( QWidget* parent, const char* name )
{
    m_kpresenterView = new KPresenterView( this, parent, name );
    return (KoView *)m_kpresenterView;
}

/*================================================================*/
void KPresenterDoc::paintContent( QPainter& painter, const QRect& rect, bool /*transparent*/, double zoomX, double zoomY )
{
    m_zoomHandler->setZoomAndResolution( 100, QPaintDevice::x11AppDpiX(), QPaintDevice::x11AppDpiY() );
    if ( zoomHandler()->zoomedResolutionX() != zoomX || zoomHandler()->zoomedResolutionY() != zoomY )
    {
        zoomHandler()->setResolution( zoomX, zoomY );
        bool forPrint = painter.device() && painter.device()->devType() == QInternal::Printer;
        newZoomAndResolution( false, forPrint );
    }
    KPrPage *page=m_pageList.first();
    if(m_kpresenterView && m_kpresenterView->getCanvas() && m_kpresenterView->getCanvas()->activePage())
        page=m_kpresenterView->getCanvas()->activePage();
    else if( m_initialActivePage )
        page=m_initialActivePage;
    //draw background
    page->background()->draw( &painter, zoomHandler(), rect, false );
    //for the moment draw first page.
    QPtrListIterator<KPObject> it( page->objectList() );
    for ( ; it.current() ; ++it )
    {
        it.current()->draw( &painter, zoomHandler(), SM_NONE );
    }
    it= m_stickyPage->objectList();
    //draw sticky obj
    for ( ; it.current() ; ++it )
    {
        if( (it.current()==_header && !hasHeader())||(it.current()==_footer && !hasFooter()))
            continue;
        it.current()->draw( &painter, zoomHandler(), SM_NONE );
    }

}

QPixmap KPresenterDoc::generatePreview( const QSize& size )
{
    int oldZoom = zoomHandler()->zoom();
    double oldResX = zoomHandler()->resolutionX();
    double oldResY = zoomHandler()->resolutionY();

    QPixmap pix = KoDocument::generatePreview(size);

    zoomHandler()->setZoomAndResolution(oldZoom, oldResX * 72, oldResY * 72);
    newZoomAndResolution( false, false );

    return pix;
}

void KPresenterDoc::movePage( int from, int to )
{
    kdDebug(33001) << "KPresenterDoc::movePage from=" << from << " to=" << to << endl;
    KPrMovePageCmd *cmd=new KPrMovePageCmd( i18n("Move Page"),from,to, m_pageList.at(from) ,this );
    cmd->execute();
    addCommand(cmd);
}

void KPresenterDoc::copyPage( int from, int to )
{
    kdDebug(33001) << "KPresenterDoc::copyPage from=" << from << " to=" << to << endl;
    bool wasSelected = isSlideSelected( from );
    KTempFile tempFile( QString::null, ".kpr" );
    tempFile.setAutoDelete( true );
    savePage( tempFile.name(), from );

    _clean = false;

    //insert page.
    KPrPage *newpage=new KPrPage(this);

    m_pageWhereLoadObject=newpage;

    loadNativeFormat( tempFile.name() );

    KPrInsertPageCmd *cmd=new KPrInsertPageCmd(i18n("Duplicate Page") ,to, newpage, this );
    cmd->execute();
    addCommand(cmd);

    _clean = true;
    m_pageWhereLoadObject=0L;

    selectPage( to, wasSelected );
}

void KPresenterDoc::copyPageToClipboard( int pgnum )
{
    // We save the page to a temp file and set the URL of the file in the clipboard
    // Yes it's a hack but at least we don't hit the clipboard size limit :)
    // (and we don't have to implement copy-tar-structure-to-clipboard)
    // In fact it even allows copying a [1-page] kpr in konq and pasting it in kpresenter :))
    kdDebug(33001) << "KPresenterDoc::copyPageToClipboard pgnum=" << pgnum << endl;
    KTempFile tempFile( QString::null, ".kpr" );
    savePage( tempFile.name(), pgnum );
    KURL url; url.setPath( tempFile.name() );
    KURL::List lst;
    lst.append( url );
    QApplication::clipboard()->setData( KURLDrag::newDrag( lst ) );
    m_tempFileInClipboard = tempFile.name(); // do this last, the above calls clipboardDataChanged
}

void KPresenterDoc::pastePage( const QMimeSource * data, int pgnum )
{
    KURL::List lst;
    if ( KURLDrag::decode( data, lst ) && !lst.isEmpty() )
    {
        insertNewPage(i18n("Paste Page"),  pgnum, IP_BEFORE, FALSE, lst.first().path() );
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
    m_pageList.at(pgNum)->slideSelected(select);
    kdDebug(33001) << "KPresenterDoc::selectPage pgNum=" << pgNum << " select=" << select << endl;
    setModified(true);

    updateSideBarItem(pgNum);

    //update statusbar
    emit pageNumChanged();
}

void KPresenterDoc::updateSideBarItem(int pgNum)
{
    // Update the views
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
        static_cast<KPresenterView*>(it.current())->updateSideBarItem( pgNum );
}

bool KPresenterDoc::isSlideSelected( int pgNum /* 0-based */ )
{
    Q_ASSERT( pgNum >= 0 );
    return m_pageList.at(pgNum)->isSlideSelected();
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

void KPresenterDoc::setKSpellConfig(KSpellConfig _kspell)
{
  if(m_pKSpellConfig==0)
    m_pKSpellConfig=new KSpellConfig();

  m_pKSpellConfig->setNoRootAffix(_kspell.noRootAffix ());
  m_pKSpellConfig->setRunTogether(_kspell.runTogether ());
  m_pKSpellConfig->setDictionary(_kspell.dictionary ());
  m_pKSpellConfig->setDictFromList(_kspell.dictFromList());
  m_pKSpellConfig->setEncoding(_kspell.encoding());
  m_pKSpellConfig->setClient(_kspell.client());

  m_bgSpellCheck->setKSpellConfig(_kspell);;

}

void KPresenterDoc::recalcVariables( int type )
{
    recalcPageNum();
    m_varColl->recalcVariables(type);
    slotRepaintVariable();
}

void KPresenterDoc::slotRepaintVariable()
{
    KPrPage *page=0L;
    for(page=pageList().first(); page; page=pageList().next())
        page->slotRepaintVariable();
}

void KPresenterDoc::slotDocumentInfoModifed()
{
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
    KPrPage *page=0L;
    for(page=pageList().first(); page; page=pageList().next())
        page->recalcPageNum();
    //specific sticky page recalc
    QPtrListIterator<KPObject> it( m_stickyPage->objectList() );
    //test for load file, activepage doesn't exist during loading
    if(m_kpresenterView && m_kpresenterView->getCanvas() && m_kpresenterView->getCanvas()->activePage())
    {
        for ( ; it.current() ; ++it )
        {
            if ( it.current()->getType() == OT_TEXT )
                ( (KPTextObject*)it.current() )->recalcPageNum( this, m_kpresenterView->getCanvas()->activePage() );
        }
    }
}

void KPresenterDoc::insertObjectInPage(double offset, KPObject *_obj)
{
    int page = (int)(offset/__pgLayout.ptHeight);
    int newPos=(int)(offset-page*__pgLayout.ptHeight);
    if ( page > ( (int)m_pageList.count()-1 ) )
    {
        for (int i=(m_pageList.count()-1); i<page;i++)
            m_pageList.append(new KPrPage(this));
    }
    _obj->setOrig(_obj->getOrig().x(),newPos);

    if ( _obj->getType() == OT_GROUP ) {
        KPGroupObject *_groupObj = static_cast<KPGroupObject*>( _obj );
        QPtrListIterator<KPObject> it( _groupObj->getObjects() );
        for ( ; it.current(); ++it ) {
            double xPos = it.current()->getOrig().x();
            double yPos = it.current()->getOrig().y() - page * __pgLayout.ptHeight;
            it.current()->setOrig( xPos, yPos );
        }

        // Hmmm... strange. I don't understand why this is necessary.
        // If there is not this, group object is not displayed correctly when after 1 page.
        _obj->setSize( _obj->getSize() );
    }
    m_pageList.at(page)->appendObject(_obj);
}

void KPresenterDoc::insertPixmapKey( KoPictureKey key )
{
    if ( !usedPixmaps.contains( key ) )
        usedPixmaps.append( key );
}

void KPresenterDoc::insertClipartKey( KoPictureKey key )
{
    if ( !usedCliparts.contains( key ) )
        usedCliparts.append( key );
}

KPrPage * KPresenterDoc::initialActivePage() const
{
    return m_initialActivePage;
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


KPrPage * KPresenterDoc::stickyPage() const
{
    return m_stickyPage;
}

bool KPresenterDoc::isHeaderFooter(const KPObject *obj) const
{
    return (obj==_header || obj==_footer);
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


void KPresenterDoc::loadStyleTemplates( QDomElement stylesElem )
{
    QValueList<QString> followingStyles;
    QPtrList<KoStyle>m_styleList(m_styleColl->styleList());

    QDomNodeList listStyles = stylesElem.elementsByTagName( "STYLE" );
    for (unsigned int item = 0; item < listStyles.count(); item++) {
        QDomElement styleElem = listStyles.item( item ).toElement();

        KoStyle *sty = new KoStyle( QString::null );
        // Load the paraglayout from the <STYLE> element
        KoParagLayout lay = KoStyle::loadStyle( styleElem );
        // This way, KWTextParag::setParagLayout also sets the style pointer, to this style
        lay.style = sty;
        sty->paragLayout() = lay;

        QDomElement nameElem = styleElem.namedItem("NAME").toElement();
        if ( !nameElem.isNull() )
        {
            sty->setName( nameElem.attribute("value") );
            //kdDebug() << "KoStyle created " << this << " name=" << m_name << endl;
        } else
            kdWarning() << "No NAME tag in LAYOUT -> no name for this style!" << endl;

        sty->setFollowingStyle( sty );

#if 0 //FIXME
        QDomElement formatElem = styleElem.namedItem( "FORMAT" ).toElement();
        if ( !formatElem.isNull() )
            sty->format() = KWTextParag::loadFormat( formatElem, 0L, defaultFont() );
        else
            kdWarning(33001) << "No FORMAT tag in <STYLE>" << endl; // This leads to problems in applyStyle().
#endif
        // Style created, now let's try to add it

        sty = m_styleColl->addStyleTemplate( sty );
        if(m_styleList.count() > followingStyles.count() )
        {
            QString following = styleElem.namedItem("FOLLOWING").toElement().attribute("name");
            followingStyles.append( following );
        }
        else
            kdWarning () << "Found duplicate style declaration, overwriting former " << sty->name() << endl;
    }

    Q_ASSERT( followingStyles.count() == m_styleList.count() );

    unsigned int i=0;
    for( QValueList<QString>::Iterator it = followingStyles.begin(); it != followingStyles.end(); ++it ) {
        KoStyle * style = m_styleColl->findStyle(*it);
        m_styleColl->styleAt( i++)->setFollowingStyle( style );
    }
}


void KPresenterDoc::updateAllStyleLists()
{
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
    {
        ((KPresenterView*)it.current())->updateStyleList();
    }
}

void KPresenterDoc::applyStyleChange( KoStyle * changedStyle, int paragLayoutChanged, int formatChanged )
{
    QPtrListIterator<KPrPage> it( m_pageList );
    for ( ; it.current(); ++it )
    {
        it.current()->applyStyleChange( changedStyle, paragLayoutChanged, formatChanged );
    }
    //styckypage
    m_stickyPage->applyStyleChange( changedStyle, paragLayoutChanged, formatChanged );
}

void KPresenterDoc::saveStyle( KoStyle *sty, QDomElement parentElem )
{
    QDomDocument doc = parentElem.ownerDocument();
    QDomElement styleElem = doc.createElement( "STYLE" );
    parentElem.appendChild( styleElem );

    sty->saveStyle( styleElem );
#if 0 //fixme
    QDomElement formatElem = KWTextParag::saveFormat( doc, &sty->format(), 0L, 0, 0 );
    styleElem.appendChild( formatElem );
#endif
}

void KPresenterDoc::startBackgroundSpellCheck()
{
    //don't start spell checking when document is embedded in konqueror
    if(backgroundSpellCheckEnabled() && isReadWrite())
    {
        if(m_initialActivePage->objectText().count()>0)
        {
            m_bgSpellCheck->objectForSpell(m_initialActivePage->textFrameSet (0));
            m_bgSpellCheck->startBackgroundSpellCheck();
        }
    }

}

void KPresenterDoc::enableBackgroundSpellCheck( bool b )
{
    m_bgSpellCheck->enableBackgroundSpellCheck(b);
}

bool KPresenterDoc::backgroundSpellCheckEnabled() const
{
    return m_bgSpellCheck->backgroundSpellCheckEnabled();
}


void KPresenterDoc::reactivateBgSpellChecking(bool refreshTextObj)
{
    QPtrListIterator<KPrPage> it( m_pageList );
    KPrPage *activePage=0L;
    if(m_kpresenterView && m_kpresenterView->getCanvas())
        activePage=m_kpresenterView->getCanvas()->activePage();
    for ( ; it.current(); ++it )
    {
        if( it.current()!=activePage)
            it.current()->reactivateBgSpellChecking(false );
        else
            it.current()->reactivateBgSpellChecking( true);
    }
    m_stickyPage->reactivateBgSpellChecking(refreshTextObj);
    startBackgroundSpellCheck();
}

KPTextObject* KPresenterDoc::nextTextFrameSet(KPTextObject *obj)
{
    if(m_kpresenterView && m_kpresenterView->getCanvas())
    {
        bool findObject = m_kpresenterView->getCanvas()->activePage()->findTextObject( bgObjSpellChecked );
        if ( !findObject )
        {
            findObject = stickyPage()->findTextObject( bgObjSpellChecked );
            if ( findObject )
            {
                bgObjSpellChecked = stickyPage()->nextTextObject( obj );
                if ( bgObjSpellChecked )
                    return bgObjSpellChecked->nextTextObject();
                else
                    return 0L;
            }
        }
        bgObjSpellChecked = m_kpresenterView->getCanvas()->activePage()->nextTextObject( obj );
        if ( bgObjSpellChecked )
            return bgObjSpellChecked->nextTextObject();
        else
        {
            bgObjSpellChecked = stickyPage()->nextTextObject( obj );
            if ( bgObjSpellChecked )
                return bgObjSpellChecked->nextTextObject();
            else
                return 0L;
        }
        return 0L;
    }
    return 0L;
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
        kdDebug()<<" index of remove horiz helpline doesn't exit !\n";
    else
        m_horizHelplines.remove(m_horizHelplines[index]);
}

void KPresenterDoc::removeVertHelpline( int index )
{
    if ( index >= (int)m_vertHelplines.count())
        kdDebug()<<" index of remove vertical helpline doesn't exit !\n";
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
    QDomElement helplines=element.namedItem("HELPLINES").toElement();
    helplines=helplines.firstChild().toElement();
    while ( !helplines.isNull() ) {
        if ( helplines.tagName()=="Vertical" )
        {
            m_vertHelplines.append(helplines.attribute("value").toDouble());
        }
        else if ( helplines.tagName()=="Horizontal" )
        {
            m_horizHelplines.append(helplines.attribute("value").toDouble());
        }
        else if ( helplines.tagName()=="HelpPoint" )
        {
            m_helpPoints.append( KoPoint( helplines.attribute("posX").toDouble(), helplines.attribute("posY").toDouble()));
        }
        helplines=helplines.nextSibling().toElement();
    }

}

QDomElement KPresenterDoc::saveHelpLines( QDomDocument &doc )
{
    QDomElement helplines=doc.createElement("HELPLINES");

    for(QValueList<double>::Iterator it = m_vertHelplines.begin(); it != m_vertHelplines.end(); ++it)
    {
        QDomElement lines=doc.createElement("Vertical");
        lines.setAttribute("value", (double)*it);
        helplines.appendChild( lines );
    }

    for(QValueList<double>::Iterator it = m_horizHelplines.begin(); it != m_horizHelplines.end(); ++it)
    {
        QDomElement lines=doc.createElement("Horizontal");
        lines.setAttribute("value", *it);
        helplines.appendChild( lines );
    }
    for(QValueList<KoPoint>::Iterator it = m_helpPoints.begin(); it != m_helpPoints.end(); ++it)
    {
        QDomElement point=doc.createElement("HelpPoint");
        point.setAttribute("posX", (*it).x());
        point.setAttribute("posY", (*it).y());
        helplines.appendChild( point );
    }

    return helplines;
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
        kdDebug()<<" removeHelpPoint( int index ) : index is bad !\n";
    else
        m_helpPoints.remove(m_helpPoints[index]);
}

void KPresenterDoc::addHelpPoint( KoPoint pos )
{
    m_helpPoints.append( pos );
}

void KPresenterDoc::updateHelpPoint( int idx, KoPoint pos )
{
    if ( idx >= (int)m_helpPoints.count())
        kdDebug()<<" updateHelpPoint : index is bad !\n";
    else
    {
        m_helpPoints[idx] = pos;
    }

}

int KPresenterDoc::indexOfHelpPoint( KoPoint pos )
{
    int ret = 0;
    for(QValueList<KoPoint>::Iterator i = m_helpPoints.begin(); i != m_helpPoints.end(); ++i, ++ret)
        if( ( pos.x() - 4.0 < (*i).x() && pos.x() + 4.0 > (*i).x())
                ||( pos.y() - 4.0 < (*i).y() && pos.y() + 4.0 > (*i).y()))
            return ret;
    return -1;
}

void KPresenterDoc::addIgnoreWordAll( const QString & word)
{
    if( m_spellListIgnoreAll.findIndex( word )==-1)
        m_spellListIgnoreAll.append( word );
    m_bgSpellCheck->addIgnoreWordAll( word );
}

void KPresenterDoc::clearIgnoreWordAll( )
{
    m_spellListIgnoreAll.clear();
    m_bgSpellCheck->clearIgnoreWordAll( );
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

void KPresenterDoc::setDontCheckUpperWord(bool _b)
{
    m_bDontCheckUpperWord=_b;
    m_bgSpellCheck->setIgnoreUpperWords( _b);
}

void KPresenterDoc::setDontCheckTitleCase(bool _b)
{
    m_bDontCheckTitleCase=_b;
    m_bgSpellCheck->setIgnoreTitleCase( _b );
}

void KPresenterDoc::setTabStopValue ( double _tabStop )
{
    m_tabStop = _tabStop;
    QPtrListIterator<KPrPage> it( m_pageList );
    for ( ; it.current(); ++it )
    {
        it.current()->changeTabStopValue( m_tabStop );
    }
    //styckypage
    m_stickyPage->changeTabStopValue( m_tabStop );
}

void KPresenterDoc::changeBgSpellCheckingState( bool b )
{
    enableBackgroundSpellCheck( b );
    reactivateBgSpellChecking();
    KConfig *config = KPresenterFactory::global()->config();
    config->setGroup("KSpell kpresenter" );
    config->writeEntry( "SpellCheck", (int)b );
}


#include <kpresenter_doc.moc>
