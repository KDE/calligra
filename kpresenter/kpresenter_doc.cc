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
#include <footer_header.h>
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
#include <koAutoFormat.h>

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
#include <koDocumentInfo.h>
#include "kprvariable.h"

using namespace std;

static const int CURRENT_SYNTAX_VERSION = 2;

/******************************************************************/
/* class KPresenterChild					  */
/******************************************************************/

/*====================== constructor =============================*/
KPresenterChild::KPresenterChild( KPresenterDoc *_kpr, KoDocument* _doc, const QRect& _rect )
    : KoDocumentChild( _kpr, _doc, QRect( _rect.left(), _rect.top(), _rect.width(), _rect.height() ) )
{
}

/*====================== constructor =============================*/
KPresenterChild::KPresenterChild( KPresenterDoc *_kpr ) :
    KoDocumentChild( _kpr )
{
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
    : KoDocument( parentWidget, widgetName, parent, name, singleViewMode ),
      _gradientCollection(), _clipartCollection(), _hasHeader( false ),
      _hasFooter( false ), m_unit( KoUnit::U_MM )
{
    setInstance( KPresenterFactory::global() );

    m_standardStyle = new KoStyle( "Standard" );

    m_defaultFont = KoGlobal::defaultFont();
    // Zoom its size (we have to use QFontInfo, in case the font was specified with a pixel size)
    m_defaultFont.setPointSize( KoTextZoomHandler::ptToLayoutUnitPt( QFontInfo(m_defaultFont).pointSize() ) );
    m_standardStyle->format().setFont( m_defaultFont );
    m_zoomHandler = new KoZoomHandler;

    m_varFormatCollection = new KoVariableFormatCollection;
    m_varColl=new KPrVariableCollection;

    dcop = 0;
    m_kpresenterView = 0;
    m_initialActivePage=0;
    m_autoFormat = new KoAutoFormat(this);
    _clean = true;
    _spInfinitLoop = false;
    _spManualSwitch = true;
    _rastX = 10;
    _rastY = 10;
    _xRnd = 20;
    _yRnd = 20;
    _orastX = 10;
    _orastY = 10;
    _txtBackCol = lightGray;
    _otxtBackCol = lightGray;
    m_pKSpellConfig=0;
    m_bDontCheckUpperWord=false;
    m_bDontCheckTitleCase=false;
    m_bShowRuler=true;
    //todo add zoom
    m_zoomHandler->setZoomAndResolution( 100, QPaintDevice::x11AppDpiX(), QPaintDevice::x11AppDpiY(), false, false );

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

    _pageLayout.unit = KoUnit::U_MM;
    m_indent = MM_TO_POINT( 10.0 );
    KPrPage *newpage=new KPrPage(this);
    m_pageList.insert( 0,newpage);
    emit sig_changeActivePage(newpage );
    objStartY = 0;
    setPageLayout( _pageLayout );
    _presPen = QPen( red, 3, SolidLine );
    presSpeed = 2;
    pasting = false;
    pasteXOffset = pasteYOffset = 0;
    ignoreSticky = TRUE;
    m_pixmapMap = 0L;
    raiseAndLowerObject = false;
    _header = new KPTextObject( this );
    _header->setDrawEditRect( false );
    _footer = new KPTextObject( this );
    _footer->setDrawEditRect( false );
    _footer->setDrawEmpty( false );
    _header->setDrawEmpty( false );

    headerFooterEdit = new KPFooterHeaderEditor( this );
    headerFooterEdit->setCaption( i18n( "KPresenter - Header/Footer Editor" ) );
    headerFooterEdit->hide();

    saveOnlyPage = -1;
    m_maxRecentFiles = 10;

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
    _pageLayout.unit=m_unit;

    QPtrListIterator<KoView> it( views() );
    for( ; it.current(); ++it ) {
        ((KPresenterView*)it.current())->getHRuler()->setUnit( KoUnit::unitName( m_unit ) );
        ((KPresenterView*)it.current())->getVRuler()->setUnit( KoUnit::unitName( m_unit ) );
    }
}

void KPresenterDoc::initConfig()
{
    KConfig* config = KPresenterFactory::global()->config();
    if( config->hasGroup("Interface") ) {
        config->setGroup( "Interface" );
        setAutoSave( config->readNumEntry( "AutoSave", defaultAutoSave()/60 ) * 60 );
        _rastX = config->readNumEntry( "RastX", 10 );
        _rastY = config->readNumEntry( "RastY", 10 );
        // Config-file value in mm, default 10 pt
        double indent = MM_TO_POINT( config->readDoubleNumEntry("Indent", POINT_TO_MM(10.0) ) );
        setIndentValue(indent);
        m_maxRecentFiles = config->readNumEntry( "NbRecentFile", 10 );
        setShowRuler(config->readBoolEntry("Rulers",true));
    }

    QColor oldBgColor = Qt::white;
    if(  config->hasGroup( "KPresenter Color" ) ) {
        config->setGroup( "KPresenter Color" );
        setTxtBackCol(config->readColorEntry( "BackgroundColor", &oldBgColor ));
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
}

KoStyle* KPresenterDoc::standardStyle()
{
    return m_standardStyle;
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
    //_commands.clear(); // done before deleting the objectlist (e.g. for lowraicmd)
    headerFooterEdit->allowClose();
    delete headerFooterEdit;

    delete _header;
    delete _footer;

    delete m_standardStyle;

    delete m_commandHistory;
    delete m_zoomHandler;
    delete m_autoFormat;
    delete m_varColl;
    delete m_varFormatCollection;
    delete dcop;
}


void KPresenterDoc::addCommand( KCommand * cmd )
{
    kdDebug() << "KWDocument::addCommand " << cmd->name() << endl;
    m_commandHistory->addCommand( cmd, false );
    setModified( true );
}

/*======================= make child list intern ================*/
bool KPresenterDoc::saveChildren( KoStore* _store, const QString &_path )
{
    int i = 0;
    KPObject *kpobject = 0L;

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
                      QString internURL = QString( "%1/%2" ).arg( _path ).arg( i++ );
                      if (((KoDocumentChild*)(it.current()))->document()!=0)
                          if ( !((KoDocumentChild*)(it.current()))->document()->saveToStore( _store, internURL ) )
                              return false;
                  }
              }
          }
      }
    }
    return true;
}

/*========================== save ===============================*/
QDomDocument KPresenterDoc::saveXML()
{
    KPObject *kpobject = 0L;
    QDomDocument doc("DOC");
    doc.appendChild( doc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );
    QDomElement presenter=doc.createElement("DOC");
    presenter.setAttribute("author", "Reginald Stadlbauer");
    presenter.setAttribute("email", "reggie@kde.org");
    presenter.setAttribute("editor", "KPresenter");
    presenter.setAttribute("mime", "application/x-kpresenter");
    presenter.setAttribute("syntaxVersion", CURRENT_SYNTAX_VERSION);
    doc.appendChild(presenter);
    QDomElement paper=doc.createElement("PAPER");
    paper.setAttribute("format", static_cast<int>( _pageLayout.format ));
    paper.setAttribute("ptWidth", _pageLayout.ptWidth);
    paper.setAttribute("ptHeight", _pageLayout.ptHeight);

    paper.setAttribute("orientation", static_cast<int>( _pageLayout.orientation ));
    paper.setAttribute("unit", static_cast<int>( _pageLayout.unit ));
    QDomElement paperBorders=doc.createElement("PAPERBORDERS");

    paperBorders.setAttribute("ptLeft", _pageLayout.ptLeft);
    paperBorders.setAttribute("ptTop", _pageLayout.ptTop);
    paperBorders.setAttribute("ptRight", _pageLayout.ptRight);
    paperBorders.setAttribute("ptBottom", _pageLayout.ptBottom);
    paper.appendChild(paperBorders);
    presenter.appendChild(paper);

    getVariableCollection()->variableSetting()->save(presenter );

    presenter.appendChild(saveAttribute( doc ));

    QDomElement element=doc.createElement("BACKGROUND");
    element.setAttribute("rastX", _rastX);
    element.setAttribute("rastY", _rastY);

    element.setAttribute("color", _txtBackCol.name());

    element.appendChild(saveBackground( doc ));
    presenter.appendChild(element);

    element=doc.createElement("HEADER");
    element.setAttribute("show", static_cast<int>( hasHeader() ));
    element.appendChild(_header->save( doc,0 ));
    presenter.appendChild(element);

    element=doc.createElement("FOOTER");
    element.setAttribute("show", static_cast<int>( hasFooter() ));
    element.appendChild(_footer->save( doc,0 ));
    presenter.appendChild(element);

    presenter.appendChild(saveTitle( doc ));

    presenter.appendChild(saveNote( doc ));

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

    if ( saveOnlyPage == -1 )
    {
        element=doc.createElement("SELSLIDES");
        QValueList<bool>::ConstIterator sit = m_selectedSlides.begin();
        for ( int i = 0; sit != m_selectedSlides.end(); ++sit, ++i ) {
            QDomElement slide=doc.createElement("SLIDE");
            slide.setAttribute("nr", i);
            slide.setAttribute("show", ( *sit ));
            element.appendChild(slide);
        }
        presenter.appendChild(element);
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
            QPtrListIterator<KPObject> oIt(m_pageList.at(i)->objectList());
            for (; oIt.current(); ++oIt )
            {
                int offset=i*m_pageList.at(i)->getZoomPageRect().height();
                if ( oIt.current()->getType() == OT_PART &&
                     dynamic_cast<KPPartObject*>( oIt.current() )->getChild() == chl.current() )
                {
                    QDomElement embedded=doc.createElement("EMBEDDED");
                    KPresenterChild* curr = (KPresenterChild*)chl.current();
                    embedded.appendChild(curr->save(doc, true));
                    QDomElement settings=doc.createElement("SETTINGS");
                    QPtrListIterator<KPObject> setOIt(m_pageList.at(i)->objectList());
                    for (; setOIt.current(); ++setOIt )
                    {
                        if ( setOIt.current()->getType() == OT_PART &&
                             dynamic_cast<KPPartObject*>( setOIt.current() )->getChild() == curr )
                            settings.appendChild(kpobject->save( doc,offset ));
                    }
                    embedded.appendChild(settings);
                    presenter.appendChild(embedded);
                }
            }
        }
    }
    makeUsedPixmapList();

    // Save the PIXMAPS and CLIPARTS list
    QString prefix = isStoredExtern() ? QString::null : url().url() + "/";

    QDomElement pixmaps = _imageCollection.saveXML( doc, usedPixmaps, prefix );
    presenter.appendChild( pixmaps );

    QDomElement cliparts = _clipartCollection.saveXML( doc, usedCliparts, prefix );
    presenter.appendChild( cliparts );

    setModified( false );
    return doc;
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
    KPObject *kpobject = 0;
    QDomElement objects=doc.createElement("OBJECTS");

    for ( int i = 0; i < static_cast<int>( m_pageList.count() ); i++ ) {
        if ( saveOnlyPage != -1 && saveOnlyPage!=i)
                continue;
        int yoffset=i*m_pageList.at(i)->getZoomPageRect().height();
        QPtrListIterator<KPObject> oIt(m_pageList.at(i)->objectList());
        for (; oIt.current(); ++oIt )
        {
            if ( oIt.current()->getType() == OT_PART )
                continue;
            QDomElement object=doc.createElement("OBJECT");
            object.setAttribute("type", static_cast<int>( oIt.current()->getType() ));

            bool _sticky = oIt.current()->isSticky();
            if (_sticky)
                object.setAttribute("sticky", static_cast<int>(_sticky));

            QPoint orig =oIt.current()->getOrig();
            if ( saveOnlyPage != -1 )
                yoffset=0;
            //add yoffset to compatibility with koffice 1.1
            object.appendChild(oIt.current()->save( doc, yoffset ));
            if ( saveOnlyPage != -1 )
                oIt.current()->setOrig( orig );
            objects.appendChild(object);
        }
    }
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
            title.setAttribute("title", m_pageList.at(i)->getManualTitle());
            titles.appendChild(title);
        }
    }
    else
    { // Only current page title.
        QDomElement title=doc.createElement("Title");
        title.setAttribute("title", m_pageList.at(saveOnlyPage)->getManualTitle());
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
            note.setAttribute("note", m_pageList.at(i)->getNoteText( ));
            notes.appendChild(note);
        }
    }
    else { // Only current page note.
        QDomElement note=doc.createElement("Note");
        note.setAttribute("note", m_pageList.at(saveOnlyPage)->getNoteText( ));
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
    return attributes;
}

/*==============================================================*/
bool KPresenterDoc::completeSaving( KoStore* _store )
{
    if ( !_store )
	return true;
    QString prefix = isStoredExtern() ? QString::null : url().url() + "/";
    _imageCollection.saveToStore( _store, usedPixmaps, prefix );
    _clipartCollection.saveToStore( _store, usedCliparts, prefix );
    return true;
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
    initConfig();
    setModified(false);

    kdDebug() << "Loading took " << (float)(dt.elapsed()) / 1000 << " seconds" << endl;

    return b;
}

/*========================== load ===============================*/
bool KPresenterDoc::loadXML( const QDomDocument &doc )
{
    emit sigProgress( 0 );
    int activePage=0;
    delete m_pixmapMap;
    m_pixmapMap = 0L;
    clipartCollectionKeys.clear();
    clipartCollectionNames.clear();
    lastObj = -1;
    bool allSlides = false;

    // clean
    if ( _clean ) {
        //KoPageLayout __pgLayout;
        __pgLayout = KoPageLayoutDia::standardLayout();
        __pgLayout.unit = KoUnit::U_MM;
        _spInfinitLoop = false;
        _spManualSwitch = true;
        _rastX = 20;
        _rastY = 20;
        _xRnd = 20;
        _yRnd = 20;
        _txtBackCol = white;
        urlIntern = url().path();
        m_selectedSlides.clear();
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

    uint childCount=document.childNodes().count();

    while(!elem.isNull()) {
        uint base = childCount;

        if(elem.tagName()=="EMBEDDED") {
            KPresenterChild *ch = new KPresenterChild( this );
            KPPartObject *kppartobject = 0L;
            QRect r;

            QDomElement object=elem.namedItem("OBJECT").toElement();
            if(!object.isNull()) {
                ch->load(object, true);  // true == uppercase
                r = ch->geometry();
                insertChild( ch );
                //FIXME**************************
#if 0
                kppartobject = new KPPartObject( ch );
                kppartobject->setOrig( r.x(), r.y() );
                kppartobject->setSize( r.width(), r.height() );
                _objectList->append( kppartobject );
#endif
                //emit sig_insertObject( ch, kppartobject );
            }
            QDomElement settings=elem.namedItem("SETTINGS").toElement();
            if(!settings.isNull() && kppartobject!=0)
                kppartobject->load(settings);
            if ( kppartobject ) {
                kppartobject->setOrig( r.x(), r.y() );
                kppartobject->setSize( r.width(), r.height() );
            }
        } else if(elem.tagName()=="PAPER")  {
            if(elem.hasAttribute("format"))
                __pgLayout.format=static_cast<KoFormat>(elem.attribute("format").toInt());
            if(elem.hasAttribute("orientation"))
                __pgLayout.orientation=static_cast<KoOrientation>(elem.attribute("orientation").toInt());
            if(elem.hasAttribute("ptWidth"))
                __pgLayout.ptWidth = elem.attribute("ptWidth").toDouble();
            if(elem.hasAttribute("inchWidth"))  //compatibility
                __pgLayout.inchWidth = elem.attribute("inchWidth").toDouble();
            if(elem.hasAttribute("mmWidth"))    //compatibility
                __pgLayout.mmWidth = elem.attribute("mmWidth").toDouble();
            if(elem.hasAttribute("ptHeight"))
                __pgLayout.ptHeight = elem.attribute("ptHeight").toDouble();
            if(elem.hasAttribute("inchHeight")) //compatibility
                __pgLayout.inchHeight = elem.attribute("inchHeight").toDouble();
            if(elem.hasAttribute("mmHeight"))   //compatibility
                __pgLayout.mmHeight = elem.attribute("mmHeight").toDouble();
            if(elem.hasAttribute("unit"))
                __pgLayout.unit = static_cast<KoUnit::Unit>(elem.attribute("unit").toInt());
            if(elem.hasAttribute("width")) {
                __pgLayout.mmWidth = elem.attribute("width").toDouble();
                __pgLayout.ptWidth = MM_TO_POINT( __pgLayout.mmWidth );
                __pgLayout.inchWidth = MM_TO_INCH( __pgLayout.mmWidth );
            }
            if(elem.hasAttribute("height")) {
                __pgLayout.mmHeight = elem.attribute("height").toDouble();
                __pgLayout.ptHeight = MM_TO_POINT( __pgLayout.mmHeight );
                __pgLayout.inchHeight = MM_TO_INCH( __pgLayout.mmHeight );
            }

            QDomElement borders=elem.namedItem("PAPERBORDERS").toElement();
            if(!borders.isNull()) {
                if(borders.hasAttribute("left")) {
                    __pgLayout.mmLeft = borders.attribute("left").toDouble();
                    __pgLayout.ptLeft = MM_TO_POINT( __pgLayout.mmLeft );
                    __pgLayout.inchLeft = MM_TO_INCH( __pgLayout.mmLeft );
                }
                if(borders.hasAttribute("top")) {
                    __pgLayout.mmTop = borders.attribute("top").toDouble();
                    __pgLayout.ptTop = MM_TO_POINT( __pgLayout.mmTop );
                    __pgLayout.inchTop = MM_TO_INCH( __pgLayout.mmTop );
                }
                if(borders.hasAttribute("right")) {
                    __pgLayout.mmRight = borders.attribute("right").toDouble();
                    __pgLayout.ptRight = MM_TO_POINT( __pgLayout.mmRight );
                    __pgLayout.inchRight = MM_TO_INCH( __pgLayout.mmRight );
                }
                if(borders.hasAttribute("bottom")) {
                    __pgLayout.mmBottom = borders.attribute("bottom").toDouble();
                    __pgLayout.ptBottom = MM_TO_POINT( __pgLayout.mmBottom );
                    __pgLayout.inchBottom = MM_TO_INCH( __pgLayout.mmBottom );
                }
                if(borders.hasAttribute("ptLeft"))
                    __pgLayout.ptLeft = borders.attribute("ptLeft").toDouble();
                if(borders.hasAttribute("inchLeft"))    //compatibility
                    __pgLayout.inchLeft = borders.attribute("inchLeft").toDouble();
                if(borders.hasAttribute("mmLeft"))      //compatibility
                    __pgLayout.mmLeft = borders.attribute("mmLeft").toDouble();
                if(borders.hasAttribute("ptRight"))
                    __pgLayout.ptRight = borders.attribute("ptRight").toDouble();
                if(borders.hasAttribute("inchRight"))   //compatibility
                    __pgLayout.inchRight = borders.attribute("inchRight").toDouble();
                if(borders.hasAttribute("mmRight"))     //compatibility
                    __pgLayout.mmRight = borders.attribute("mmRight").toDouble();
                if(borders.hasAttribute("ptTop"))
                    __pgLayout.ptTop = borders.attribute("ptTop").toDouble();
                if(borders.hasAttribute("inchTop"))     //compatibility
                    __pgLayout.inchTop = borders.attribute("inchTop").toDouble();
                if(borders.hasAttribute("mmTop"))       //compatibility
                    __pgLayout.mmTop = borders.attribute("mmTop").toDouble();
                if(borders.hasAttribute("ptBottom"))
                    __pgLayout.ptBottom = borders.attribute("ptBottom").toDouble();
                if(borders.hasAttribute("inchBottom"))  //compatibility
                    __pgLayout.inchBottom = borders.attribute("inchBottom").toDouble();
                if(borders.hasAttribute("mmBottom"))    //compatibility
                    __pgLayout.mmBottom = borders.attribute("inchBottom").toDouble();
            }
        } else if(elem.tagName()=="VARIABLESETTINGS"){
            getVariableCollection()->variableSetting()->load(document);
        }
        else if(elem.tagName()=="BACKGROUND") {
            int red=0, green=0, blue=0;
            if(elem.hasAttribute("rastX"))
                _rastX = elem.attribute("rastX").toInt();
            if(elem.hasAttribute("rastY"))
                _rastY = elem.attribute("rastY").toInt();
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
            if(elem.hasAttribute("color"))
                _txtBackCol.setNamedColor(elem.attribute("color"));
            else
                _txtBackCol.setRgb(red, green, blue);
            loadBackground(elem);
        } else if(elem.tagName()=="HEADER") {
            if ( _clean || !hasHeader() ) {
                if(elem.hasAttribute("show")) {
                    setHeader(static_cast<bool>(elem.attribute("show").toInt()));
                    headerFooterEdit->setShowHeader( hasHeader() );
                }
                _header->load(elem);
            }
        } else if(elem.tagName()=="FOOTER") {
            if ( _clean || !hasFooter() ) {
                if(elem.hasAttribute("show")) {
                    setFooter( static_cast<bool>(elem.attribute("show").toInt() ) );
                    headerFooterEdit->setShowFooter( hasFooter() );
                }
                _footer->load(elem);
            }
        } else if(elem.tagName()=="ATTRIBUTES") {
            if(elem.hasAttribute("activePage"))
                activePage=elem.attribute("activePage").toInt();
        } else if(elem.tagName()=="PAGETITLES") {
            loadTitle(elem);
        } else if(elem.tagName()=="PAGENOTES") {
            loadNote(elem);
        } else if(elem.tagName()=="OBJECTS") {
            //FIXME**********************
#if 0
            lastObj = _objectList->count() - 1;
#endif
            loadObjects(elem);
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
                            Q_ASSERT( nr == (int)m_selectedSlides.count() );
                            m_selectedSlides.append( show );
                        } else kdWarning() << "Parse error. No nr in <SLIDE> !" << endl;
                    }
                    slide=slide.nextSibling().toElement();
                }
            }
        } else if(elem.tagName()=="PIXMAPS") {
            QDateTime defaultDateTime( _imageCollection.tmpDate(), _imageCollection.tmpTime() );
            m_pixmapMap = new QMap<KoImageKey, QString>( _imageCollection.readXML( elem, defaultDateTime ) );
        } else if(elem.tagName()=="CLIPARTS") {
            QDomElement keyElement=elem.firstChild().toElement();
            while(!keyElement.isNull()) {
                if(keyElement.tagName()=="KEY") {
                    KPClipartCollection::Key key;
                    QString n;
                    key.loadAttributes(keyElement, QDate(), QTime());
                    if(keyElement.hasAttribute("name"))
                        n=keyElement.attribute("name");
                    clipartCollectionKeys.append( key );
                    clipartCollectionNames.append( n );
                }
                keyElement=keyElement.nextSibling().toElement();
            }
        }
        elem=elem.nextSibling().toElement();

        base-=1;

        emit sigProgress(::abs(100-base/childCount*100)+10);
    }

    if ( _rastX == 0 ) _rastX = 10;
    if ( _rastY == 0 ) _rastY = 10;

    if ( _clean ) {
        // Fix the selectedslides list (for all docs)
        while ( m_selectedSlides.count() < getPageNums() )
            m_selectedSlides.append(true);

        // Not sure this is necessary anymore.
        if ( allSlides ) {
            //kdDebug(33001) << "KPresenterDoc::loadXML allSlides" << endl;
            QValueList<bool>::Iterator sit = m_selectedSlides.begin();
            for ( ; sit != m_selectedSlides.end(); ++sit )
                (*sit) = true;
        }
    }
    if(activePage!=-1)
        m_initialActivePage=m_pageList.at(activePage);
    setModified(false);
    emit sigProgress(-1);
    return true;
}

/*====================== load background =========================*/
void KPresenterDoc::loadBackground( const QDomElement &element )
{
    kdDebug(33001) << "KPresenterDoc::loadBackground" << endl;
    QDomElement page=element.firstChild().toElement();
    int i=0;
    while(!page.isNull()) {
        //test if there is a page at this index
        //=> don't add new page if there is again a page
        if(i>(m_pageList.count()-1))
            m_pageList.append(new KPrPage(this));
        m_pageList.at(i)->background()->load(page);
        page=page.nextSibling().toElement();
        i++;
    }
}

/*========================= load objects =========================*/
void KPresenterDoc::loadObjects( const QDomElement &element, bool _paste, KPrPage *_page )
{
    ObjType t = OT_LINE;
    QDomElement obj=element.firstChild().toElement();
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
            int offset=0;
            switch ( t ) {
            case OT_LINE: {
                KPLineObject *kplineobject = new KPLineObject();
                offset=kplineobject->load(obj);
                if ( _paste && _page) {
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Line" ), kplineobject, this,_page );
                    insertCmd->execute();
                    addCommand( insertCmd );
                } else
                    insertObjectInPage(offset, kplineobject);
            } break;
            case OT_RECT: {
                KPRectObject *kprectobject = new KPRectObject();
                offset=kprectobject->load(obj);
                if ( _paste && _page) {
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Rectangle" ), kprectobject, this, _page );
                    insertCmd->execute();
                    addCommand( insertCmd );
                } else
                    insertObjectInPage(offset, kprectobject);
            } break;
            case OT_ELLIPSE: {
                KPEllipseObject *kpellipseobject = new KPEllipseObject();
                offset=kpellipseobject->load(obj);
                if ( _paste && _page) {
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Ellipse" ), kpellipseobject, this, _page );
                    insertCmd->execute();
                    addCommand( insertCmd );
                } else
                    insertObjectInPage(offset, kpellipseobject);
            } break;
            case OT_PIE: {
                KPPieObject *kppieobject = new KPPieObject();
                offset=kppieobject->load(obj);
                if ( _paste && _page) {
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Pie/Arc/Chord" ), kppieobject, this, _page );
                    insertCmd->execute();
                    addCommand( insertCmd );
                } else
                    insertObjectInPage(offset, kppieobject);
            } break;
            case OT_AUTOFORM: {
                KPAutoformObject *kpautoformobject = new KPAutoformObject();
                offset=kpautoformobject->load(obj);
                if ( _paste && _page) {
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Autoform" ), kpautoformobject, this, _page );
                    insertCmd->execute();
                    addCommand( insertCmd );
                } else
                    insertObjectInPage(offset, kpautoformobject);
            } break;
            case OT_CLIPART: {
                KPClipartObject *kpclipartobject = new KPClipartObject( &_clipartCollection );
                offset=kpclipartobject->load(obj);
                if ( _paste && _page) {
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Clipart" ), kpclipartobject, this , _page);
                    insertCmd->execute();
                    addCommand( insertCmd );
                    kpclipartobject->reload();
                } else
                    insertObjectInPage(offset, kpclipartobject);
            } break;
            case OT_TEXT: {
                KPTextObject *kptextobject = new KPTextObject( this );
                offset=kptextobject->load(obj);
                if ( _paste && _page) {
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Textbox" ), kptextobject, this, _page );
                    insertCmd->execute();
                    addCommand( insertCmd );
                } else
                    insertObjectInPage(offset, kptextobject);
            } break;
            case OT_PICTURE: {
                KPPixmapObject *kppixmapobject = new KPPixmapObject( &_imageCollection );
                offset=kppixmapobject->load(obj);
                if ( _paste && _page) {
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Picture" ), kppixmapobject, this, _page );
                    insertCmd->execute();
                    addCommand( insertCmd );
                    kppixmapobject->reload();
                } else
                    insertObjectInPage(offset, kppixmapobject);
            } break;
            case OT_FREEHAND: {
                KPFreehandObject *kpfreehandobject = new KPFreehandObject();
                offset=kpfreehandobject->load(obj);
                if ( _paste && _page) {
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Freehand" ), kpfreehandobject, this, _page );
                    insertCmd->execute();
                    addCommand( insertCmd );
                } else
                    insertObjectInPage(offset,kpfreehandobject);
            } break;
            case OT_POLYLINE: {
                KPPolylineObject *kppolylineobject = new KPPolylineObject();
                offset=kppolylineobject->load(obj);
                if ( _paste && _page) {
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Polyline" ), kppolylineobject, this, _page );
                    insertCmd->execute();
                    addCommand( insertCmd );
                } else
                    insertObjectInPage(offset, kppolylineobject);
            } break;
            case OT_QUADRICBEZIERCURVE: {
                KPQuadricBezierCurveObject *kpQuadricBezierCurveObject = new KPQuadricBezierCurveObject();
                offset=kpQuadricBezierCurveObject->load(obj);
                if ( _paste && _page) {
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Quadric Bezier Curve" ), kpQuadricBezierCurveObject, this, _page );
                    insertCmd->execute();
                    addCommand( insertCmd );
                } else
                    insertObjectInPage(offset, kpQuadricBezierCurveObject);
            } break;
            case OT_CUBICBEZIERCURVE: {
                KPCubicBezierCurveObject *kpCubicBezierCurveObject = new KPCubicBezierCurveObject();
                offset=kpCubicBezierCurveObject->load(obj);
                if ( _paste && _page) {
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Cubic Bezier Curve" ), kpCubicBezierCurveObject, this, _page );
                    insertCmd->execute();
                    addCommand( insertCmd );
                } else
                    insertObjectInPage(offset, kpCubicBezierCurveObject);
            } break;
            case OT_POLYGON: {
                KPPolygonObject *kpPolygonObject = new KPPolygonObject();
                offset=kpPolygonObject->load( obj );
                if ( _paste && _page) {
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Polygon" ), kpPolygonObject, this , _page);
                    insertCmd->execute();
                    addCommand( insertCmd );
                } else
                    insertObjectInPage(offset, kpPolygonObject);
            } break;
            case OT_GROUP: {
                KPGroupObject *kpgroupobject = new KPGroupObject();
                offset=kpgroupobject->load(obj, this);
                if ( _paste && _page) {
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Group Object" ), kpgroupobject, this, _page );
                    insertCmd->execute();
                    addCommand( insertCmd );
                } else
                    insertObjectInPage(offset, kpgroupobject);
            } break;
            default: break;
            }
#if 0
            if ( objStartY > 0 )
                _objectList->last()->moveBy( 0, objStartY );
            if ( pasting ) {
                _objectList->last()->moveBy( pasteXOffset, pasteYOffset );
                _objectList->last()->setSelected( true );
            }
            if ( !ignoreSticky )
                _objectList->last()->setSticky( sticky );
#endif
        }
        obj=obj.nextSibling().toElement();
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
            if(i>(m_pageList.count()-1))
                m_pageList.append(new KPrPage(this));
            m_pageList.at(i)->insertManualTitle(title.attribute("title"));
            i++;
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
            if(i>(m_pageList.count()-1))
                m_pageList.append(new KPrPage(this));
            m_pageList.at(i)->setNoteText(note.attribute("note"));
            i++;
        }
        note=note.nextSibling().toElement();
    }
}

/*===================================================================*/
bool KPresenterDoc::completeLoading( KoStore* _store )
{
    if ( _store ) {
        QString prefix = urlIntern.isEmpty() ? url().path() : urlIntern;
        prefix += '/';
        _imageCollection.readFromStore( _store, *m_pixmapMap, prefix );
        delete m_pixmapMap;
        m_pixmapMap = 0L;

	QValueListIterator<KPClipartCollection::Key> it2 = clipartCollectionKeys.begin();
	QStringList::ConstIterator nit2 = clipartCollectionNames.begin();

	for ( ; it2 != clipartCollectionKeys.end(); ++it2, ++nit2 ) {
	    QString u = QString::null;

	    if ( !( *nit2 ).isEmpty() )
		u = *nit2;
	    else {
		u = prefix + it2.node->data.toString();
	    }

	    QPicture pic;

	    if ( _store->open( u ) ) {
		KoStoreDevice dev(_store );
		int size = _store->size();
		char * data = new char[size];
		dev.readBlock( data, size );
		pic.setData( data, size );
		delete data;
		_store->close();
	    } else {
		u.prepend( "file:" );
		if ( _store->open( u ) ) {
		    KoStoreDevice dev(_store );
		    int size = _store->size();
		    char * data = new char[size];
		    dev.readBlock( data, size );
		    pic.setData( data, size );
		    delete data;
		    _store->close();
		}
	    }

	    _clipartCollection.insertClipart( it2.node->data, pic );
	}

//	_pixmapCollection.setAllowChangeRef( true );
//	_pixmapCollection.getPixmapDataCollection().setAllowChangeRef( true );

	if ( _clean )
	    setPageLayout( __pgLayout );
	else {
	    QRect r = m_pageList.last()->getZoomPageRect();
            m_pageList.last()->background()->setBgSize(r.size());
	}


          KPrPage *page;
          for ( page = m_pageList.first(); page ; page = m_pageList.next() )
              page->completeLoading( _clean, lastObj );

    } else {
	if ( _clean )
	    setPageLayout( __pgLayout );
	else
	    setPageLayout( _pageLayout );
    }
    recalcVariables( VT_FIELD );
    return true;
}


/*===================== set page layout ==========================*/
void KPresenterDoc::setPageLayout( KoPageLayout pgLayout )
{
    //     if ( _pageLayout == pgLayout )
    //	return;

    _pageLayout = pgLayout;

    for ( int i = 0; i < static_cast<int>( m_pageList.count() ); i++ )
        m_pageList.at( i )->background()->setBgSize( m_pageList.at( i )->getZoomPageRect().size() );
    setUnit(  _pageLayout.unit );

    repaint( false );
    // don't setModified(true) here, since this is called on startup
}

/*==================== insert a new page =========================*/
unsigned int KPresenterDoc::insertNewPage( int diffx, int diffy, bool _restore )
{
    if ( _restore ) {
	QRect r = m_pageList.last()->getZoomPageRect();
        m_pageList.last()->background()->setBgSize(r.size());
	repaint( false );
    }

    return getPageNums();
}

/*================================================================*/
bool KPresenterDoc::insertNewTemplate( int /*diffx*/, int /*diffy*/, bool clean )
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
void KPresenterDoc::setRasters( unsigned int rx, unsigned int ry, bool _replace )
{
    _orastX = _rastX;
    _orastY = _rastY;
    _rastX = rx;
    _rastY = ry;
    if ( _replace ) replaceObjs();
}

/*=================== repaint all views =========================*/
void KPresenterDoc::repaint( bool erase )
{
    QPtrListIterator<KoView> it( views() );
    for( ; it.current(); ++it ) {
	// I am doing a cast to KPresenterView here, since some austrian hacker :-)
	// decided to overload the non virtual repaint method!
	((KPresenterView*)it.current())->repaint( erase );
    }
}

/*===================== repaint =================================*/
void KPresenterDoc::repaint( const QRect& rect )
{
    QRect r;

    QPtrListIterator<KoView> it( views() );
    for( ; it.current(); ++it ) {
	r = rect;
	r.moveTopLeft( QPoint( r.x() - ((KPresenterView*)it.current())->getCanvas()->diffx(),
			       r.y() - ((KPresenterView*)it.current())->getCanvas()->diffy() ) );

	// I am doing a cast to KPresenterView here, since some austrian hacker :-)
	// decided to overload the non virtual repaint method!
	//((KPresenterView*)it.current())->repaint( r, false );
	it.current()->update( r );
    }
}

/*===================== repaint =================================*/
void KPresenterDoc::repaint( KPObject *kpobject )
{
    QRect r;

    QPtrListIterator<KoView> it( views() );
    for( ; it.current(); ++it )
    {
	r = kpobject->getBoundingRect(  );
	r.moveTopLeft( QPoint( r.x() - ((KPresenterView*)it.current())->getCanvas()->diffx(),
			       r.y() - ((KPresenterView*)it.current())->getCanvas()->diffy() ) );
	it.current()->update( r );
    }
}

QValueList<int> KPresenterDoc::reorderPage( unsigned int num )
{
    return m_pageList.at(num)->reorderPage();
}

/*================== get size of page ===========================*/
QRect KPresenterDoc::getPageRect( unsigned int num, int diffx, int diffy,
				  float fakt, bool decBorders ) const
{
    int pw, ph, bl = static_cast<int>(_pageLayout.ptLeft);
    int br = static_cast<int>(_pageLayout.ptRight);
    int bt = static_cast<int>(_pageLayout.ptTop);
    int bb = static_cast<int>(_pageLayout.ptBottom);
    int wid = static_cast<int>(_pageLayout.ptWidth);
    int hei = static_cast<int>(_pageLayout.ptHeight);

    if ( !decBorders ) {
	br = 0;
	bt = 0;
	bl = 0;
	bb = 0;
    }

    pw = wid  - ( bl + br );
    ph = hei - ( bt + bb );

    pw = static_cast<int>( static_cast<float>( pw ) * fakt );
    ph = static_cast<int>( static_cast<float>( ph ) * fakt );
//FIXME : num
    return QRect( -diffx + bl, -diffy + bt +/* num **/1* ( bt + bb + ph ), pw, ph );
}

/*================================================================*/
int KPresenterDoc::getLeftBorder()
{
    return static_cast<int>(_pageLayout.ptLeft);
}

/*================================================================*/
int KPresenterDoc::getTopBorder()
{
    return static_cast<int>(_pageLayout.ptTop);
}

/*================================================================*/
int KPresenterDoc::getBottomBorder()
{
    return static_cast<int>(_pageLayout.ptBottom);
}

/*================================================================*/
void KPresenterDoc::deletePage( int _page )
{
    kdDebug(33001) << "KPresenterDoc::deletePage " << _page << endl;

    deSelectAllObj();
    m_pageList.at(_page)->deletePage();


    recalcPageNum();

    recalcVariables( VT_PGNUM );


    //remove page.
    m_pageList.remove( _page);
    //set active page -1
    emit sig_changeActivePage(m_pageList.at(_page-1) );

    repaint( false );

    Q_ASSERT( _page < (int)m_selectedSlides.count() );
    m_selectedSlides.remove( m_selectedSlides.at( _page ) );

    // Update the sidebars
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
        static_cast<KPresenterView*>(it.current())->updateSideBar();

    //update statusbar
    emit pageNumChanged();
}

/*================================================================*/
int KPresenterDoc::insertPage( int _page, InsertPos _insPos, bool chooseTemplate, const QString &theFile )
{
    kdDebug(33001) << "KPresenterDoc::insertPage " << _page << endl;

    QString _template, fileName;
    if ( !chooseTemplate ) {
	_template = QString::fromLocal8Bit( getenv( "HOME" ) );
	_template += "/.default.kpr";\
	fileName = _template;
	if ( !theFile.isEmpty() )
	    fileName = theFile;
    } else {
	if ( KoTemplateChooseDia::choose(  KPresenterFactory::global(), _template,
					   "", QString::null, QString::null, KoTemplateChooseDia::OnlyTemplates,
					   "kpresenter_template") == KoTemplateChooseDia::Cancel )
	    return -1;
	QFileInfo fileInfo( _template );
	fileName = fileInfo.dirPath( true ) + "/" + fileInfo.baseName() + ".kpt";
	QString cmd = "cp " + fileName + " " + QString::fromLocal8Bit( getenv( "HOME" ) ) + "/.default.kpr";
	system( QFile::encodeName(cmd) );
    }

    _clean = false;

    if ( _insPos == IP_AFTER )
	_page++;

    if ( _page == 0 )
        objStartY = -1;
    else
        objStartY = 0;

    loadNativeFormat( fileName );
    objStartY = 0;

    _clean = true;
    setModified(true);

    //insert page.
    KPrPage *newpage=new KPrPage(this);
    m_pageList.insert( _page,newpage);
    emit sig_changeActivePage(newpage );

    if ( _page < (int)m_selectedSlides.count() )
    {
        kdDebug(33001) << "KPresenterDoc::insertPage inserting in m_selectedSlides at position " << _page << endl;
        m_selectedSlides.insert( m_selectedSlides.at(_page), true );
        kdDebug(33001) << "KPresenterDoc::insertPage count is now " << m_selectedSlides.count() << endl;
    }
    else
        m_selectedSlides.append( true );

    recalcPageNum();

    recalcVariables( VT_PGNUM );

    //update statusbar
    emit pageNumChanged();

    // Update the sidebars
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
        static_cast<KPresenterView*>(it.current())->updateSideBar();
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
    KMacroCommand * macroCmd = new KMacroCommand( i18n("Set new options") );
    bool addMacroCommand=false;
    for ( int i = 0; i < static_cast<int>( m_pageList.count() ); i++ )
    {
        KCommand *cmd=m_pageList.at(i)->replaceObjs( createUndoRedo, _orastX,_orastY,_txtBackCol, _otxtBackCol);
        if(cmd && createUndoRedo)
        {
            macroCmd->addCommand(cmd);
            addMacroCommand=true;
        }
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
    page->background()->restore();
}

/*==================== load pasted objects ==============================*/
void KPresenterDoc::loadPastedObjs( const QString &in, int,KPrPage* _page )
{
    QDomDocument doc;
    doc.setContent( in );

    QDomElement document=doc.documentElement();

    // DOC
    if (document.tagName()!="DOC") {
        kdError() << "Missing DOC" << endl;
        return;
    }

    bool ok = false;

    if(document.hasAttribute("mime") && document.attribute("mime")=="application/x-kpresenter-selection")
        ok=true;

    if ( !ok )
        return;

    loadObjects(document, true,_page);

    repaint( false );
    setModified( true );
}

/*================= deselect all objs ===========================*/
void KPresenterDoc::deSelectAllObj()
{
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
	((KPresenterView*)it.current())->getCanvas()->deSelectAllObj();
}

/*================================================================*/
void KPresenterDoc::setHeader( bool b )
{
    _hasHeader = b;
}

/*================================================================*/
void KPresenterDoc::setFooter( bool b )
{
    _hasFooter = b;
}

/*================================================================*/
void KPresenterDoc::makeUsedPixmapList()
{
    usedPixmaps.clear();
    usedCliparts.clear();

    for ( int i = 0; i < static_cast<int>( m_pageList.count() ); i++ ) {
	if ( saveOnlyPage != -1 &&
	     i != saveOnlyPage )
	    continue;
        m_pageList.at(i)->makeUsedPixmapList();
    }
}

/*================================================================*/
KoView* KPresenterDoc::createViewInstance( QWidget* parent, const char* name )
{
    m_kpresenterView = new KPresenterView( this, parent, name );
    return (KoView *)m_kpresenterView;
}

/*================================================================*/
void KPresenterDoc::paintContent( QPainter& painter, const QRect& rect, bool /*transparent*/, double /*zoomX*/, double /*zoomY*/ )
{
    unsigned int i = 0;
#if 0
    QPtrListIterator<KPBackGround> bIt( _backgroundList );
    for (; bIt.current(); ++bIt, i++ )
    {
        QRect r = getPageRect( i, 0, 0, 1.0, false );
        if ( rect.intersects( r ) )
            bIt.current()->draw( &painter, QPoint( r.x(), r.y() ), false );
    }
#endif
    kdDebug()<<"paintContent==================================\n";
#if 0
    QPtrListIterator<KPObject> oIt( *_objectList );
    for (; oIt.current(); ++oIt )
        if ( rect.intersects( oIt.current()->getBoundingRect( 0, 0 ) ) )
        {
            oIt.current()->drawSelection( false );
            oIt.current()->draw( &painter, 0, 0 );
            oIt.current()->drawSelection( true );
        }
#endif
}


void KPresenterDoc::movePage( int from, int to )
{
    kdDebug(33001) << "KPresenterDoc::movePage from=" << from << " to=" << to << endl;
    bool wasSelected = isSlideSelected( from );
    KTempFile tempFile( QString::null, ".kpr" );
    tempFile.setAutoDelete( true );
    savePage( tempFile.name(), from );
    deletePage( from );
    insertPage( to, IP_BEFORE, FALSE, tempFile.name() );
    selectPage( to, wasSelected );
}

void KPresenterDoc::copyPage( int from, int to )
{
    kdDebug(33001) << "KPresenterDoc::copyPage from=" << from << " to=" << to << endl;
    bool wasSelected = isSlideSelected( from );
    KTempFile tempFile( QString::null, ".kpr" );
    tempFile.setAutoDelete( true );
    savePage( tempFile.name(), from );
    insertPage( to, IP_BEFORE, FALSE, tempFile.name() );
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
        insertPage( pgnum, IP_BEFORE, FALSE, lst.first().path() );
        selectPage( pgnum, true /* should be part of the file ? */ );
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
    Q_ASSERT( pgNum < (int)m_selectedSlides.count() );
    m_selectedSlides[ pgNum ] = select;
    kdDebug(33001) << "KPresenterDoc::selectPage pgNum=" << pgNum << " select=" << select << endl;
    setModified(true);
    // Update the views
    QPtrListIterator<KoView> it( views() );
    for (; it.current(); ++it )
        static_cast<KPresenterView*>(it.current())->updateSideBarItem( pgNum );

    //update statusbar
    emit pageNumChanged();
}

bool KPresenterDoc::isSlideSelected( int pgNum /* 0-based */ ) const
{
    Q_ASSERT( pgNum >= 0 );
    Q_ASSERT( pgNum < (int)m_selectedSlides.count() );
    return m_selectedSlides[ pgNum ];
}

QValueList<int> KPresenterDoc::selectedSlides() const /* returned list is 0-based */
{

    int pageNums = getPageNums(); // to be safe
    QValueList<int> result;
    QValueList<bool>::ConstIterator sit = m_selectedSlides.begin();
    for ( int i = 0; sit != m_selectedSlides.end(); ++sit, ++i )
        if ( *sit && i < pageNums )
            result << i;
    return result;
}

QString KPresenterDoc::selectedForPrinting() const {

    QString ret;
    QValueList<bool>::ConstIterator sit = m_selectedSlides.begin();
    int start=-1, end=-1, i=0;
    bool continuous=false;
    for ( ; sit!=m_selectedSlides.end(); ++sit, ++i) {
        if(*sit) {
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
}

void KPresenterDoc::recalcVariables( int type )
{
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

int KPresenterDoc::undoRedoLimit()
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
}

void KPresenterDoc::insertObjectInPage(int offset, KPObject *_obj)
{
    int page = offset/__pgLayout.ptHeight;
    int newPos=(offset-page*__pgLayout.ptHeight);
    if( page > (m_pageList.count()-1))
    {
        for (int i=(m_pageList.count()-1); i<page;i++)
            m_pageList.append(new KPrPage(this));
    }
    _obj->setOrig(_obj->getOrig().x(),newPos);
    m_pageList.at(page)->appendObject(_obj);
}

void KPresenterDoc::appendPixmapKey( KPImageKey key)
{
    usedPixmaps.append(key);
}

void KPresenterDoc::appendClipartKey(KPClipartKey key)
{
    usedCliparts.append(key);
}

KPrPage * KPresenterDoc::initialActivePage()
{
    return m_initialActivePage;
}

#include <kpresenter_doc.moc>
