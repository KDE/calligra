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
#include <page.h>
#include <footer_header.h>
#include <kplineobject.h>
#include <kprectobject.h>
#include <kpellipseobject.h>
#include <kpautoformobject.h>
#include <kpclipartobject.h>
#include <kptextobject.h>
#include <kppixmapobject.h>
#include <kppieobject.h>
#include <kppartobject.h>
#include <kpgroupobject.h>
#include <movecmd.h>
#include <insertcmd.h>
#include <deletecmd.h>
#include <setoptionscmd.h>
#include <chgclipcmd.h>
#include <chgpixcmd.h>
#include <lowraicmd.h>
#include <pievaluecmd.h>
#include <rectvaluecmd.h>
#include <penbrushcmd.h>
#include <grpobjcmd.h>
#include <styledia.h>
#include <insertpagedia.h>

#include <qpopupmenu.h>
#include <qclipboard.h>
#include <qregexp.h>
#include <qfileinfo.h>
#include <qdom.h>

#include <kurl.h>
#include <kurldrag.h>
#include <ktempfile.h>
#include <klocale.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kconfig.h>
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

#include <KPresenterDocIface.h>
#include <cassert>
using namespace std;

static const int CURRENT_SYNTAX_VERSION = 2;

/******************************************************************/
/* class KPresenterChild					  */
/******************************************************************/

/*====================== constructor =============================*/
KPresenterChild::KPresenterChild( KPresenterDoc *_kpr, KoDocument* _doc, const QRect& _rect, int _diffx, int _diffy )
    : KoDocumentChild( _kpr, _doc, QRect( _rect.left() + _diffx, _rect.top() + _diffy, _rect.width(), _rect.height() ) )
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

bool KPresenterChild::loadXML( const QDomElement &element )
{
    if ( element.hasAttribute( "url" ) )
        m_tmpURL = element.attribute("url");
    if ( element.hasAttribute("mime") )
        m_tmpMimeType = element.attribute("mime");

    if ( m_tmpURL.isEmpty() )
    {
        kdDebug() << "Empty 'url' attribute in OBJECT" << endl;
        return false;
    }
    if ( m_tmpMimeType.isEmpty() )
    {
        kdDebug() << "Empty 'mime' attribute in OBJECT" << endl;
        return false;
    }

   bool brect = false;
    QDomElement e = element.firstChild().toElement();
    for( ; !e.isNull(); e = e.nextSibling().toElement() )
    {
        if ( e.tagName() == "RECT" )
        {
            brect = true;
            int x, y, w, h;
            x=y=w=h=0;
            if ( e.hasAttribute( "x" ) )
                x = e.attribute( "x" ).toInt(&brect);
            if ( e.hasAttribute( "y" ) )
                y = e.attribute( "y" ).toInt(&brect);
            if ( e.hasAttribute( "w" ) )
                w = e.attribute( "w" ).toInt(&brect);
            if ( e.hasAttribute( "h" ) )
                h = e.attribute( "h" ).toInt(&brect);
            m_tmpGeometry = QRect(x, y, w, h);
            setGeometry( m_tmpGeometry );
        }
    }

    if ( !brect )
    {
        kdDebug() << "Missing RECT in OBJECT" << endl;
        return false;
    }

    return true;
}


QDomElement KPresenterChild::saveXML( QDomDocument& doc )
{
    if (document()==0)
        return QDomElement();

    QDomElement object=doc.createElement("OBJECT");
    object.setAttribute("url", document()->url().url());
    object.setAttribute("mime", document()->nativeFormatMimeType());
    QDomElement rect=doc.createElement("RECT");
    rect.setAttribute( "x", geometry().left() );
    rect.setAttribute( "y", geometry().top() );
    rect.setAttribute( "w", geometry().width() );
    rect.setAttribute( "h", geometry().height() );
    object.appendChild(rect);
    return object;
}

/******************************************************************/
/* class KPresenterDoc						  */
/******************************************************************/

/*====================== constructor =============================*/
KPresenterDoc::KPresenterDoc( QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name, bool singleViewMode )
    : KoDocument( parentWidget, widgetName, parent, name, singleViewMode ),
      _gradientCollection(), _clipartCollection(), _commands(), _hasHeader( false ),
      _hasFooter( false ), urlIntern()
{
    fCollection = new KTextEditFormatCollection;
    setInstance( KPresenterFactory::global() );

    dcop = 0;
    _clean = true;
    _objectList = new QList<KPObject>;
    _objectList->setAutoDelete( false );
    _backgroundList.setAutoDelete( true );
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
    _pageLayout.unit = PG_MM;
    objStartY = 0;
    setPageLayout( _pageLayout, 0, 0 );
    _presPen = QPen( red, 3, SolidLine );
    presSpeed = 2;
    pasting = false;
    pasteXOffset = pasteYOffset = 0;
    ignoreSticky = TRUE;

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

    connect( &_commands, SIGNAL( undoRedoChanged( QString, QString ) ),
		      this, SLOT( slotUndoRedoChanged( QString, QString ) ) );

    connect( QApplication::clipboard(), SIGNAL( dataChanged() ),
             this, SLOT( clipboardDataChanged() ) );

    if ( name )
	dcopObject();
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
    headerFooterEdit->allowClose();
    delete headerFooterEdit;

    delete _header;
    delete _footer;

    _objectList->setAutoDelete( true );
    _objectList->clear();
    delete _objectList;
    _backgroundList.clear();

    delete fCollection;
}

/*======================= make child list intern ================*/
bool KPresenterDoc::saveChildren( KoStore* _store, const QString &_path )
{
    int i = 0;
    KPObject *kpobject = 0L;

    if ( saveOnlyPage == -1 ) // Don't save all children into template for one page
           // ###### TODO: save objects that are on that page
    {
      QListIterator<KoDocumentChild> it( children() );
      for( ; it.current(); ++it ) {
          // Don't save children that are only in the undo/redo history
          // but not anymore in the presentation
          for ( unsigned int j = 0; j < _objectList->count(); j++ )
          {
              kpobject = _objectList->at( j );
              if ( kpobject->getType() == OT_PART &&
                   dynamic_cast<KPPartObject*>( kpobject )->getChild() == it.current() )
              {
                  QString internURL = QString( "%1/%2" ).arg( _path ).arg( i++ );
                  if (((KoDocumentChild*)(it.current()))->document()!=0)
                      if ( !((KoDocumentChild*)(it.current()))->document()->saveToStore( _store, internURL ) )
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
    paper.setAttribute("mmWidth", _pageLayout.mmWidth);
    paper.setAttribute("mmHeight", _pageLayout.mmHeight);
    paper.setAttribute("inchWidth", _pageLayout.inchWidth);
    paper.setAttribute("inchHeight", _pageLayout.inchHeight);
    paper.setAttribute("orientation", static_cast<int>( _pageLayout.orientation ));
    paper.setAttribute("unit", static_cast<int>( _pageLayout.unit ));
    QDomElement paperBorders=doc.createElement("PAPERBORDERS");
    paperBorders.setAttribute("mmLeft", _pageLayout.mmLeft);
    paperBorders.setAttribute("mmTop", _pageLayout.mmTop);
    paperBorders.setAttribute("mmRight", _pageLayout.mmRight);
    paperBorders.setAttribute("mmBottom", _pageLayout.mmBottom);
    paperBorders.setAttribute("ptLeft", _pageLayout.ptLeft);
    paperBorders.setAttribute("ptTop", _pageLayout.ptTop);
    paperBorders.setAttribute("ptRight", _pageLayout.ptRight);
    paperBorders.setAttribute("ptBottom", _pageLayout.ptBottom);
    paperBorders.setAttribute("inchLeft", _pageLayout.inchLeft);
    paperBorders.setAttribute("inchTop", _pageLayout.inchTop);
    paperBorders.setAttribute("inchRight", _pageLayout.inchRight);
    paperBorders.setAttribute("inchBottom", _pageLayout.inchBottom);
    paper.appendChild(paperBorders);
    presenter.appendChild(paper);

    QDomElement element=doc.createElement("BACKGROUND");
    element.setAttribute("rastX", _rastX);
    element.setAttribute("rastY", _rastY);
    element.setAttribute("bred", _txtBackCol.red());
    element.setAttribute("bgreen", _txtBackCol.green());
    element.setAttribute("bblue", _txtBackCol.blue());
    element.appendChild(saveBackground( doc ));
    presenter.appendChild(element);

    element=doc.createElement("HEADER");
    element.setAttribute("show", static_cast<int>( hasHeader() ));
    element.appendChild(_header->save( doc ));
    presenter.appendChild(element);

    element=doc.createElement("FOOTER");
    element.setAttribute("show", static_cast<int>( hasFooter() ));
    element.appendChild(_footer->save( doc ));
    presenter.appendChild(element);

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
        QValueList<bool>::Iterator sit = m_selectedSlides.begin();
        for ( int i = 0; sit != m_selectedSlides.end(); ++sit, ++i ) {
            QDomElement slide=doc.createElement("SLIDE");
            slide.setAttribute("nr", i);
            slide.setAttribute("show", ( *sit ));
            element.appendChild(slide);
        }
        presenter.appendChild(element);
    }

    // Write "OBJECT" tag for every child
    QListIterator<KoDocumentChild> chl( children() );
    for( ; chl.current(); ++chl ) {
        // Don't save children that are only in the undo/redo history
        // but not anymore in the presentation
        for ( unsigned int j = 0; j < _objectList->count(); j++ )
        {
            kpobject = _objectList->at( j );
            if ( saveOnlyPage != -1 ) {
                int pg = getPageOfObj( j, 0, 0 ) - 1;
                if ( saveOnlyPage != pg )
                    continue;
            }
            if ( kpobject->getType() == OT_PART &&
                 dynamic_cast<KPPartObject*>( kpobject )->getChild() == chl.current() )
            {
                QDomElement embedded=doc.createElement("EMBEDDED");
                KPresenterChild* curr = (KPresenterChild*)chl.current();
                embedded.appendChild(curr->saveXML( doc ));
                QDomElement settings=doc.createElement("SETTINGS");
                for ( unsigned int i = 0; i < _objectList->count(); i++ ) {
                    kpobject = _objectList->at( i );
                    if ( kpobject->getType() == OT_PART &&
                         dynamic_cast<KPPartObject*>( kpobject )->getChild() == curr )
                        settings.appendChild(kpobject->save( doc ));
                }
                embedded.appendChild(settings);
                presenter.appendChild(embedded);
            }
        }
    }

    makeUsedPixmapList();

    QDomElement pixmaps=doc.createElement("PIXMAPS");
    int i = 0;
    KPImageCollection::ConstIterator it = _imageCollection.begin();
    KPImageCollection::ConstIterator end = _imageCollection.end();

    for ( ; it != end; ++it ) {
        if ( usedPixmaps.contains( it.key() ) ) {
            KPImageKey key = it.key();
            QString format = QFileInfo( key.filename ).extension().upper();
            if ( format == "JPG" )
                format = "JPEG";
            if ( QImage::outputFormats().find( format.latin1() ) == -1 )
                format = "PNG";
            QString pictureName = QString( "pictures/picture%1.%2" ).arg( ++i ).arg( format.lower() );
            if ( !isStoredExtern() )
                pictureName.prepend( url().url() + "/" );

            element=doc.createElement("KEY");
            key.setAttributes(element);
            element.setAttribute("name", pictureName);
            pixmaps.appendChild(element);
        }
    }
    presenter.appendChild(pixmaps);

    QDomElement cliparts=doc.createElement("CLIPARTS");
    i = 0;
    QMap< KPClipartCollection::Key, QPicture >::Iterator it2 = _clipartCollection.begin();

    for( ; it2 != _clipartCollection.end(); ++it2 ) {
        KPClipartCollection::Key key = it2.key();
        QString clipartName = QString( "cliparts/clipart%1.wmf" ).arg( ++i );
        if ( !isStoredExtern() )
            clipartName.prepend( url().url() + "/" );
        element=doc.createElement("KEY");
        key.setAttributes(element);
        element.setAttribute("name", clipartName);
        cliparts.appendChild(element);
    }
    presenter.appendChild(cliparts);

    setModified( false );
    return doc;
}

/*===============================================================*/
void KPresenterDoc::enableEmbeddedParts( bool f )
{
    KPObject *kpobject = 0L;

    for ( unsigned int k = 0; k < _objectList->count(); k++ ) {
	kpobject = _objectList->at( k );
	if ( kpobject->getType() == OT_PART )
	    dynamic_cast<KPPartObject*>( kpobject )->enableDrawing( f );
    }
}

/*========================== save background ====================*/
QDomDocumentFragment KPresenterDoc::saveBackground( QDomDocument &doc )
{
    KPBackGround *kpbackground = 0;
    QDomDocumentFragment fragment=doc.createDocumentFragment();

    for ( int i = 0; i < static_cast<int>( _backgroundList.count() ); i++ ) {
	if ( saveOnlyPage != -1 &&
	     i != saveOnlyPage )
	    continue;
	kpbackground = _backgroundList.at( i );
	fragment.appendChild(kpbackground->save( doc ));
    }
    return fragment;
}

/*========================== save objects =======================*/
QDomElement KPresenterDoc::saveObjects( QDomDocument &doc )
{
    KPObject *kpobject = 0;
    QDomElement objects=doc.createElement("OBJECTS");

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
        if ( saveOnlyPage != -1 ) {
            int pg = getPageOfObj( i, 0, 0 ) - 1;
            if ( saveOnlyPage != pg )
                continue;
        }
        kpobject = objectList()->at( i );
        if ( kpobject->getType() == OT_PART ) continue;
        QDomElement object=doc.createElement("OBJECT");
        object.setAttribute("type", static_cast<int>( kpobject->getType() ));
        object.setAttribute("sticky", static_cast<int>(kpobject->isSticky()));
        QPoint orig = kpobject->getOrig();
        if ( saveOnlyPage != -1 )
            kpobject->moveBy( 0, -saveOnlyPage * getPageRect( 0, 0, 0 ).height() );
        object.appendChild(kpobject->save( doc ));
        if ( saveOnlyPage != -1 )
            kpobject->setOrig( orig );
        objects.appendChild(object);
    }
    return objects;
}

/*==============================================================*/
bool KPresenterDoc::completeSaving( KoStore* _store )
{
    if ( !_store )
	return true;

    KPImageCollection::ConstIterator it = _imageCollection.begin();
    KPImageCollection::ConstIterator end = _imageCollection.end();

    int i = 0;
    for( ; it != end; ++it ) {
	if ( !it.key().filename.isEmpty() && usedPixmaps.contains( it.key() ) ) {

	    QString format = QFileInfo( it.key().filename ).extension().upper();
	    if ( format == "JPG" )
		format = "JPEG";
	    if ( QImage::outputFormats().find( format.latin1() ) == -1 )
		format = "PNG";

	    QString u2 = QString( "pictures/picture%1.%2" ).arg( ++i ).arg( format.lower() );

	    if ( !isStoredExtern() )
		u2.prepend( url().url() + "/" );

	    if ( _store->open( u2 ) ) {
		KoStoreDevice dev( _store );
		QImageIO io;
		io.setIODevice( &dev );
		io.setImage( it.data().image() );
		io.setFormat( format.latin1() );
		io.write();
		_store->close();
	    }
	}
    }

    QMap< KPClipartCollection::Key, QPicture >::Iterator it2 = _clipartCollection.begin();
    i = 0;
    for( ; it2 != _clipartCollection.end(); ++it2 ) {
	if ( _clipartCollection.references( it2.key() ) > 0 && !it2.key().filename.isEmpty() ) {

	    QString u2 = QString( "cliparts/clipart%1.wmf" ).arg( ++i );
	    if ( !isStoredExtern() )
		u2.prepend( url().url() + "/" );

	    if ( _store->open( u2 ) ) {
		KoStoreDevice dev( _store );
		dev.writeBlock( it2.data().data(), it2.data().size() );
		_store->close();
	    }
	}
    }

    return true;
}

/*========================== load ===============================*/
bool KPresenterDoc::loadChildren( KoStore* _store )
{
    if ( objStartY == 0 ) // Don't do this when inserting a template or a page...
    {
      QListIterator<KoDocumentChild> it( children() );
      for( ; it.current(); ++it ) {
        if ( !((KoDocumentChild*)it.current())->loadDocument( _store ) )
          return false;
      }
    }
    return true;
}

bool KPresenterDoc::loadXML( QIODevice * dev, const QDomDocument& doc )
{
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
	QCString cmd = KGlobal::dirs()->findExe("perl").local8Bit();
	if (cmd.isEmpty())
	{
	    KMessageBox::error(0L, i18n("You don't appear to have PERL installed.\nIt is needed to convert this document.\nPlease install PERL and try again."));
	    return false;
	}
	cmd += " ";
	cmd += QFile::encodeName(locate( "exe", "kprconverter.pl" )) + " ";
	cmd += QFile::encodeName( tmpFileIn.name() ) + " ";
	cmd += QFile::encodeName( tmpFileOut.name() );
	system( cmd );

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
    setModified(false);
    return b;
}

/*========================== load ===============================*/
bool KPresenterDoc::loadXML( const QDomDocument &doc )
{
    pixmapCollectionKeys.clear();
    pixmapCollectionNames.clear();
    clipartCollectionKeys.clear();
    clipartCollectionNames.clear();
    lastObj = -1;
    bool allSlides = false;

    // clean
    if ( _clean ) {
        //KoPageLayout __pgLayout;
        __pgLayout = KoPageLayoutDia::standardLayout();
        __pgLayout.unit = PG_MM;

        if ( !_backgroundList.isEmpty() )
            _backgroundList.clear();
        delete _objectList;
        _objectList = new QList<KPObject>;
        _objectList->setAutoDelete( false );
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

    QDomElement document=doc.documentElement();
    // DOC
    if(document.tagName()!="DOC") {
        kdWarning() << "Missing DOC" << endl;
        return false;
    }

    if(!document.hasAttribute("mime") || document.attribute("mime")!="application/x-kpresenter") {
        kdError() << "Unknown mime type " << document.attribute("mime") << endl;
        return false;
    }
    if(document.hasAttribute("url"))
        urlIntern=KURL(document.attribute("url")).path();

    QDomElement elem=document.firstChild().toElement();
    while(!elem.isNull()) {
        if(elem.tagName()=="EMBEDDED") {
            KPresenterChild *ch = new KPresenterChild( this );
            KPPartObject *kppartobject = 0L;
            QRect r;

            QDomElement object=elem.namedItem("OBJECT").toElement();
            if(!object.isNull()) {
                ch->loadXML(object);
                r = ch->geometry();
                insertChild( ch );
                kppartobject = new KPPartObject( ch );
                kppartobject->setOrig( r.x(), r.y() );
                kppartobject->setSize( r.width(), r.height() );
                _objectList->append( kppartobject );
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
            if(elem.hasAttribute("inchWidth"))
                __pgLayout.inchWidth = elem.attribute("inchWidth").toDouble();
            if(elem.hasAttribute("mmWidth"))
                __pgLayout.mmWidth = elem.attribute("mmWidth").toDouble();
            if(elem.hasAttribute("ptHeight"))
                __pgLayout.ptHeight = elem.attribute("ptHeight").toDouble();
            if(elem.hasAttribute("inchHeight"))
                __pgLayout.inchHeight = elem.attribute("inchHeight").toDouble();
            if(elem.hasAttribute("mmHeight"))
                __pgLayout.mmHeight = elem.attribute("mmHeight").toDouble();
            if(elem.hasAttribute("unit"))
                __pgLayout.unit = static_cast<KoUnit>(elem.attribute("unit").toInt());
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
                if(borders.hasAttribute("inchLeft"))
                    __pgLayout.inchLeft = borders.attribute("inchLeft").toDouble();
                if(borders.hasAttribute("mmLeft"))
                    __pgLayout.mmLeft = borders.attribute("mmLeft").toDouble();
                if(borders.hasAttribute("ptRight"))
                    __pgLayout.ptRight = borders.attribute("ptRight").toDouble();
                if(borders.hasAttribute("inchRight"))
                    __pgLayout.inchRight = borders.attribute("inchRight").toDouble();
                if(borders.hasAttribute("mmRight"))
                    __pgLayout.mmRight = borders.attribute("mmRight").toDouble();
                if(borders.hasAttribute("ptTop"))
                    __pgLayout.ptTop = borders.attribute("ptTop").toDouble();
                if(borders.hasAttribute("inchTop"))
                    __pgLayout.inchTop = borders.attribute("inchTop").toDouble();
                if(borders.hasAttribute("mmTop"))
                    __pgLayout.mmTop = borders.attribute("mmTop").toDouble();
                if(borders.hasAttribute("ptBottom"))
                    __pgLayout.ptBottom = borders.attribute("ptBottom").toDouble();
                if(borders.hasAttribute("inchBottom"))
                    __pgLayout.inchBottom = borders.attribute("inchBottom").toDouble();
                if(borders.hasAttribute("mmBottom"))
                    __pgLayout.mmBottom = borders.attribute("inchBottom").toDouble();
            }
        } else if(elem.tagName()=="BACKGROUND") {
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
        } else if(elem.tagName()=="OBJECTS") {
            lastObj = _objectList->count() - 1;
            loadObjects(elem);
        } else if(elem.tagName()=="INFINITLOOP") {
            if(elem.hasAttribute("value"))
                _spInfinitLoop = static_cast<bool>(elem.attribute("value").toInt());
        } else if(elem.tagName()=="PRESSPEED") {
            if(elem.hasAttribute("value"))
                presSpeed = static_cast<PresSpeed>(elem.attribute("value").toInt());
        } else if(elem.tagName()=="MANUALSWITCH") {
            if(elem.hasAttribute("value"))
                _spManualSwitch = static_cast<bool>(elem.attribute("value").toInt());
        } else if(elem.tagName()=="PRESSLIDES") {
            if(elem.hasAttribute("value") && elem.attribute("value").toInt()==0)
                allSlides = TRUE;
        } else if(elem.tagName()=="SELSLIDES") {
            QDomElement slide=elem.firstChild().toElement();
            while(!slide.isNull()) {
                if( _clean ) // Skip this when loading a single page
                    if(slide.tagName()=="SLIDE") {
                        int nr = -1;
                        bool show = false;
                        if(slide.hasAttribute("nr"))
                            nr=slide.attribute("nr").toInt();
                        if(slide.hasAttribute("show"))
                            show=static_cast<bool>(slide.attribute("show").toInt());
                        // We assume that the PAGE tags were found before the
                        // SELSLIDES tag (this is always the case)
                        if ( nr >= 0 )
                        {
                            //kdDebug() << "KPresenterDoc::loadXML m_selectedSlides nr=" << nr << " show=" << show << endl;
                            ASSERT( nr < (int)m_selectedSlides.count() );
                            m_selectedSlides[nr] = show;
                        } else kdWarning() << "Parse error. No nr in <SLIDE> !" << endl;
                    }
                slide=slide.nextSibling().toElement();
            }
        } else if(elem.tagName()=="PIXMAPS") {
            QDomElement keyElement=elem.firstChild().toElement();
            while(!keyElement.isNull()) {
                if(keyElement.tagName()=="KEY") {
                    KPImageKey key;
                    QString n;
                    key.loadAttributes(keyElement, QDate(), QTime());
                    if(keyElement.hasAttribute("name"))
                        n=keyElement.attribute("name");
                    pixmapCollectionKeys.append( key );
                    pixmapCollectionNames.append( n );
                }
                keyElement=keyElement.nextSibling().toElement();
            }
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
    }

    if ( _rastX == 0 ) _rastX = 10;
    if ( _rastY == 0 ) _rastY = 10;

    if ( allSlides && _clean ) {
        //kdDebug() << "KPresenterDoc::loadXML allSlides" << endl;
        QValueList<bool>::Iterator sit = m_selectedSlides.begin();
        for ( ; sit != m_selectedSlides.end(); ++sit )
            (*sit) = true;
    }

    setModified(false);
    return true;
}

/*====================== load background =========================*/
void KPresenterDoc::loadBackground( const QDomElement &element )
{
    QDomElement page=element.firstChild().toElement();
    while(!page.isNull()) {
        insertNewPage( 0, 0, false ); // appends an entry to _backgroundList
        KPBackGround *kpbackground = _backgroundList.last();
        kpbackground->load( page );
        m_selectedSlides.append( true ); // create an entry for this page
        page=page.nextSibling().toElement();
    }
}

/*========================= load objects =========================*/
void KPresenterDoc::loadObjects( const QDomElement &element, bool _paste )
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

            switch ( t ) {
            case OT_LINE: {
                KPLineObject *kplineobject = new KPLineObject();
                kplineobject->load(obj);
                if ( _paste ) {
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Line" ), kplineobject, this );
                    insertCmd->execute();
                    _commands.addCommand( insertCmd );
                } else
                    _objectList->append( kplineobject );
            } break;
            case OT_RECT: {
                KPRectObject *kprectobject = new KPRectObject();
                kprectobject->setRnds( _xRnd, _yRnd );
                kprectobject->load(obj);
                if ( _paste ) {
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Rectangle" ), kprectobject, this );
                    insertCmd->execute();
                    _commands.addCommand( insertCmd );
                } else
                    _objectList->append( kprectobject );
            } break;
            case OT_ELLIPSE: {
                KPEllipseObject *kpellipseobject = new KPEllipseObject();
                kpellipseobject->load(obj);
                if ( _paste ) {
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Ellipse" ), kpellipseobject, this );
                    insertCmd->execute();
                    _commands.addCommand( insertCmd );
                } else
                    _objectList->append( kpellipseobject );
            } break;
            case OT_PIE: {
                KPPieObject *kppieobject = new KPPieObject();
                kppieobject->load(obj);
                if ( _paste ) {
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Pie/Arc/Chors" ), kppieobject, this );
                    insertCmd->execute();
                    _commands.addCommand( insertCmd );
                } else
                    _objectList->append( kppieobject );
            } break;
            case OT_AUTOFORM: {
                KPAutoformObject *kpautoformobject = new KPAutoformObject();
                kpautoformobject->load(obj);
                if ( _paste ) {
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Autoform" ), kpautoformobject, this );
                    insertCmd->execute();
                    _commands.addCommand( insertCmd );
                } else
                    _objectList->append( kpautoformobject );
            } break;
            case OT_CLIPART: {
                KPClipartObject *kpclipartobject = new KPClipartObject( &_clipartCollection );
                kpclipartobject->load(obj);
                if ( _paste ) {
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Clipart" ), kpclipartobject, this );
                    insertCmd->execute();
                    _commands.addCommand( insertCmd );
                    kpclipartobject->reload();
                } else
                    _objectList->append( kpclipartobject );
            } break;
            case OT_TEXT: {
                KPTextObject *kptextobject = new KPTextObject( this );
                kptextobject->load(obj);
                if ( _paste ) {
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Textbox" ), kptextobject, this );
                    insertCmd->execute();
                    _commands.addCommand( insertCmd );
                } else
                    _objectList->append( kptextobject );
            } break;
            case OT_PICTURE: {
                KPPixmapObject *kppixmapobject = new KPPixmapObject( &_imageCollection );
                kppixmapobject->load(obj);
                if ( _paste ) {
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Picture" ), kppixmapobject, this );
                    insertCmd->execute();
                    _commands.addCommand( insertCmd );
                    kppixmapobject->reload();
                } else
                    _objectList->append( kppixmapobject );
            } break;
            case OT_GROUP: {
                KPGroupObject *kpgroupobject = new KPGroupObject();
                kpgroupobject->load(obj, this);
                if ( _paste ) {
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Group Object" ), kpgroupobject, this );
                    insertCmd->execute();
                    _commands.addCommand( insertCmd );
                } else
                    _objectList->append( kpgroupobject );
            } break;
            default: break;
            }

            if ( objStartY > 0 )
                _objectList->last()->moveBy( 0, objStartY );
            if ( pasting ) {
                _objectList->last()->moveBy( pasteXOffset, pasteYOffset );
                _objectList->last()->setSelected( true );
            }
            if ( !ignoreSticky )
                _objectList->last()->setSticky( sticky );
        }
        obj=obj.nextSibling().toElement();
    }
}

/*===================================================================*/
bool KPresenterDoc::completeLoading( KoStore* _store )
{
    if ( _store ) {
	QString str = urlIntern.isEmpty() ? url().path() : urlIntern;

	QValueListIterator<KPImageKey> it = pixmapCollectionKeys.begin();
	QStringList::Iterator nit = pixmapCollectionNames.begin();

	for ( ; it != pixmapCollectionKeys.end(); ++it, ++nit ) {
	    QString u = QString::null;

	    if ( !( *nit ).isEmpty() )
		u = *nit;
	    else {
		u = str;
		u += "/";
		u += it.node->data.toString();
	    }

	    QImage img;

	    if ( _store->open( u ) ) {
		KoStoreDevice dev(_store );
		QImageIO io( &dev, 0 );
		if(!io.read())
		    // okay - has to be a funky - old xpm in a very old kpr file...
		    // Don't ask me why this is needed...
		    img=QImage(_store->read(_store->size()));
		else
		    img = io.image();

		_store->close();
	    } else {
		u.prepend( "file:" );
		if ( _store->open( u ) ) {
		    KoStoreDevice dev(_store );
		    QImageIO io( &dev, 0 );
		    io.read( );
		    img = io.image();

		    _store->close();
		}
	    }

            _imageCollection.insertImage( *it, img );
//	    _pixmapCollection.getPixmapDataCollection().insertPixmapData( it.node->data, img );
	}

	QValueListIterator<KPClipartCollection::Key> it2 = clipartCollectionKeys.begin();
	QStringList::Iterator nit2 = clipartCollectionNames.begin();

	for ( ; it2 != clipartCollectionKeys.end(); ++it2, ++nit2 ) {
	    QString u = QString::null;

	    if ( !( *nit2 ).isEmpty() )
		u = *nit2;
	    else {
		u = str;
		u += "/";
		u += it2.node->data.toString();
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
	    setPageLayout( __pgLayout, 0, 0 );
	else {
	    QRect r = getPageRect( 0, 0, 0 );
	    _backgroundList.last()->setBgSize( r.size() );
	}

	KPObject *kpobject = 0;
	for ( kpobject = _objectList->first(); kpobject; kpobject = _objectList->next() ) {
	    if ( kpobject->getType() == OT_PICTURE ) {
		if ( _clean || _objectList->findRef( kpobject ) > lastObj )
		    dynamic_cast<KPPixmapObject*>( kpobject )->reload();
	    } else if ( kpobject->getType() == OT_CLIPART )
		dynamic_cast<KPClipartObject*>( kpobject )->reload();
	    else if ( kpobject->getType() == OT_TEXT )
		dynamic_cast<KPTextObject*>( kpobject )->recalcPageNum( this );
	}

    } else {
	if ( _clean )
	    setPageLayout( __pgLayout, 0, 0 );
	else
	    setPageLayout( _pageLayout, 0, 0 );
    }

//    _pixmapCollection.setAllowChangeRef( true );
//    _pixmapCollection.getPixmapDataCollection().setAllowChangeRef( true );

    return true;
}

/*========================= insert an object =====================*/
void KPresenterDoc::insertObject( const QRect& _rect, KoDocumentEntry& _e, int _diffx, int _diffy )
{

    KoDocument* doc = _e.createDoc( this );
    if ( !doc || !doc->initDoc() ) {
	return;
    }

    KPresenterChild* ch = new KPresenterChild( this, doc, _rect, _diffx, _diffy );

    insertChild( ch );
    setModified( true );

    KPPartObject *kppartobject = new KPPartObject( ch );
    kppartobject->setOrig( _rect.x() + _diffx, _rect.y() + _diffy );
    kppartobject->setSize( _rect.width(), _rect.height() );
    kppartobject->setSelected( true );
    QWidget::connect(ch, SIGNAL(changed(KoChild *)), kppartobject, SLOT(slot_changed(KoChild *)) );

    InsertCmd *insertCmd = new InsertCmd( i18n( "Embed Object" ), kppartobject, this );
    insertCmd->execute();
    _commands.addCommand( insertCmd );

    //emit sig_insertObject( ch, kppartobject );

    repaint( false );
}

/*===================== set page layout ==========================*/
void KPresenterDoc::setPageLayout( KoPageLayout pgLayout, int diffx, int diffy )
{
    //     if ( _pageLayout == pgLayout )
    //	return;

    _pageLayout = pgLayout;
    QRect r = getPageRect( 0, diffx, diffy );

    for ( int i = 0; i < static_cast<int>( _backgroundList.count() ); i++ )
        _backgroundList.at( i )->setBgSize( r.size() );

    QString unit;
    switch ( _pageLayout.unit ) {
    case PG_MM: unit = "mm";
	break;
    case PG_PT: unit = "pt";
	break;
    case PG_INCH: unit = "inch";
	break;
    }
    setUnit( _pageLayout.unit, unit );

    repaint( false );
    // don't setModified(true) here, since this is called on startup
}

/*==================== insert a new page =========================*/
unsigned int KPresenterDoc::insertNewPage( int diffx, int diffy, bool _restore )
{

    KPBackGround *kpbackground = new KPBackGround( &_imageCollection, &_gradientCollection,
						   &_clipartCollection, this );
    _backgroundList.append( kpbackground );

    if ( _restore ) {
	QRect r = getPageRect( 0, diffx, diffy );
	_backgroundList.last()->setBgSize( r.size() );
	repaint( false );
    }

    setModified(true);
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
	objStartY = getPageRect( _backgroundList.count() - 1, 0, 0 ).y() + getPageRect( _backgroundList.count() - 1,
											0, 0 ).height();
	bool ok = loadNativeFormat( fileName );
	objStartY = 0;
	_clean = true;
	setModified(true);
	resetURL();
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
	setModified(true);
	bool ok = loadNativeFormat( fileName );
	resetURL();
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

/*==================== set background color ======================*/
void KPresenterDoc::setBackColor( unsigned int pageNum, QColor backColor1, QColor backColor2, BCType bcType,
				  bool unbalanced, int xfactor, int yfactor )
{
    KPBackGround *kpbackground = 0;

    if ( pageNum < _backgroundList.count() ) {
	kpbackground = backgroundList()->at( pageNum );
	kpbackground->setBackColor1( backColor1 );
	kpbackground->setBackColor2( backColor2 );
	kpbackground->setBackColorType( bcType );
	kpbackground->setBackUnbalanced( unbalanced );
	kpbackground->setBackXFactor( xfactor );
	kpbackground->setBackYFactor( yfactor );
    }
    setModified(true);
}

/*==================== set background picture ====================*/
void KPresenterDoc::setBackPixFilename( unsigned int pageNum, QString backPix )
{
//    QMap< KPPixmapDataCollection::Key, QImage >::Iterator it = _pixmapCollection.getPixmapDataCollection().begin();
    KPImageCollection::Iterator it = _imageCollection.begin();
    QDateTime dt;

    if ( !QFileInfo( backPix ).exists() ) {
	for ( ; it != _imageCollection.end(); ++it ) {
	    if ( it.key().filename == backPix ) {
		dt = it.key().lastModified;
		break;
	    }
	}
    }

    if ( pageNum < _backgroundList.count() )
	backgroundList()->at( pageNum )->setBackPixmap( backPix, dt );
    setModified(true);
}

/*==================== set background clipart ====================*/
void KPresenterDoc::setBackClipFilename( unsigned int pageNum, QString backClip )
{
    QMap< KPClipartCollection::Key, QPicture >::Iterator it = _clipartCollection.begin();
    QDateTime dt;

    if ( !QFileInfo( backClip ).exists() ) {
	for ( ; it != _clipartCollection.end(); ++it ) {
	    if ( it.key().filename == backClip ) {
		dt = it.key().lastModified;
		break;
	    }
	}
    }

    if ( pageNum < _backgroundList.count() )
	backgroundList()->at( pageNum )->setBackClipFilename( backClip, dt );
    setModified(true);
}

/*================= set background pic view ======================*/
void KPresenterDoc::setBackView( unsigned int pageNum, BackView backView )
{
    if ( pageNum < _backgroundList.count() )
	backgroundList()->at( pageNum )->setBackView( backView );
    setModified(true);
}

/*==================== set background type =======================*/
void KPresenterDoc::setBackType( unsigned int pageNum, BackType backType )
{
    if ( pageNum < _backgroundList.count() )
	backgroundList()->at( pageNum )->setBackType( backType );
    setModified(true);
}

/*========================== set page effect =====================*/
void KPresenterDoc::setPageEffect( unsigned int pageNum, PageEffect pageEffect )
{
    if ( pageNum < _backgroundList.count() )
	backgroundList()->at( pageNum )->setPageEffect( pageEffect );
    setModified(true);
}

/*===================== set pen and brush ========================*/
bool KPresenterDoc::setPenBrush( QPen pen, QBrush brush, LineEnd lb, LineEnd le, FillType ft, QColor g1, QColor g2,
				 BCType gt, bool unbalanced, int xfactor, int yfactor, bool sticky )
{
    KPObject *kpobject = 0;
    bool ret = false;

    QList<KPObject> _objects;
    QList<PenBrushCmd::Pen> _oldPen;
    QList<PenBrushCmd::Brush> _oldBrush;
    PenBrushCmd::Pen _newPen, *ptmp;
    PenBrushCmd::Brush _newBrush, *btmp;

    _newPen.pen = QPen( pen );
    _newPen.lineBegin = lb;
    _newPen.lineEnd = le;

    _newBrush.brush = QBrush( brush );
    _newBrush.fillType = ft;
    _newBrush.gColor1 = g1;
    _newBrush.gColor2 = g2;
    _newBrush.gType = gt;
    _newBrush.unbalanced = unbalanced;
    _newBrush.xfactor = xfactor;
    _newBrush.yfactor = yfactor;

    _objects.setAutoDelete( false );
    _oldPen.setAutoDelete( false );
    _oldBrush.setAutoDelete( false );

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
	kpobject = objectList()->at( i );
	if ( kpobject->isSelected() ) {
	    kpobject->setSticky( sticky );
	    ptmp = new PenBrushCmd::Pen;
	    btmp = new PenBrushCmd::Brush;
	    switch ( kpobject->getType() ) {
	    case OT_LINE:
		ptmp->pen = QPen( dynamic_cast<KPLineObject*>( kpobject )->getPen() );
		ptmp->lineBegin = dynamic_cast<KPLineObject*>( kpobject )->getLineBegin();
		ptmp->lineEnd = dynamic_cast<KPLineObject*>( kpobject )->getLineEnd();
		ret = true;
		break;
	    case OT_RECT:
		ptmp->pen = QPen( dynamic_cast<KPRectObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPRectObject*>( kpobject )->getBrush() );
		btmp->fillType = dynamic_cast<KPRectObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPRectObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPRectObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPRectObject*>( kpobject )->getGType();
		btmp->unbalanced = dynamic_cast<KPRectObject*>( kpobject )->getGUnbalanced();
		btmp->xfactor = dynamic_cast<KPRectObject*>( kpobject )->getGXFactor();
		btmp->yfactor = dynamic_cast<KPRectObject*>( kpobject )->getGYFactor();
		ret = true;
		break;
	    case OT_ELLIPSE:
		ptmp->pen = QPen( dynamic_cast<KPEllipseObject*>( kpobject )->getPen() );
		btmp->brush = dynamic_cast<KPEllipseObject*>( kpobject )->getBrush();
		btmp->fillType = dynamic_cast<KPEllipseObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPEllipseObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPEllipseObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPEllipseObject*>( kpobject )->getGType();
		btmp->unbalanced = dynamic_cast<KPEllipseObject*>( kpobject )->getGUnbalanced();
		btmp->xfactor = dynamic_cast<KPEllipseObject*>( kpobject )->getGXFactor();
		btmp->yfactor = dynamic_cast<KPEllipseObject*>( kpobject )->getGYFactor();
		ret = true;
		break;
	    case OT_AUTOFORM:
		ptmp->pen = QPen( dynamic_cast<KPAutoformObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPAutoformObject*>( kpobject )->getBrush() );
		ptmp->lineBegin = dynamic_cast<KPAutoformObject*>( kpobject )->getLineBegin();
		ptmp->lineEnd = dynamic_cast<KPAutoformObject*>( kpobject )->getLineEnd();
		btmp->fillType = dynamic_cast<KPAutoformObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPAutoformObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPAutoformObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPAutoformObject*>( kpobject )->getGType();
		btmp->unbalanced = dynamic_cast<KPAutoformObject*>( kpobject )->getGUnbalanced();
		btmp->xfactor = dynamic_cast<KPAutoformObject*>( kpobject )->getGXFactor();
		btmp->yfactor = dynamic_cast<KPAutoformObject*>( kpobject )->getGYFactor();
		ret = true;
		break;
	    case OT_PIE:
		ptmp->pen = QPen( dynamic_cast<KPPieObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPPieObject*>( kpobject )->getBrush() );
		ptmp->lineBegin = dynamic_cast<KPPieObject*>( kpobject )->getLineBegin();
		ptmp->lineEnd = dynamic_cast<KPPieObject*>( kpobject )->getLineEnd();
		btmp->fillType = dynamic_cast<KPPieObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPPieObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPPieObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPPieObject*>( kpobject )->getGType();
		btmp->unbalanced = dynamic_cast<KPPieObject*>( kpobject )->getGUnbalanced();
		btmp->xfactor = dynamic_cast<KPPieObject*>( kpobject )->getGXFactor();
		btmp->yfactor = dynamic_cast<KPPieObject*>( kpobject )->getGYFactor();
		ret = true;
		break;
	    case OT_PART:
		ptmp->pen = QPen( dynamic_cast<KPPartObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPPartObject*>( kpobject )->getBrush() );
		btmp->fillType = dynamic_cast<KPPartObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPPartObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPPartObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPPartObject*>( kpobject )->getGType();
		btmp->unbalanced = dynamic_cast<KPPartObject*>( kpobject )->getGUnbalanced();
		btmp->xfactor = dynamic_cast<KPPartObject*>( kpobject )->getGXFactor();
		btmp->yfactor = dynamic_cast<KPPartObject*>( kpobject )->getGYFactor();
		ret = true;
		break;
	    case OT_TEXT:
		ptmp->pen = QPen( dynamic_cast<KPTextObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPTextObject*>( kpobject )->getBrush() );
		btmp->fillType = dynamic_cast<KPTextObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPTextObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPTextObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPTextObject*>( kpobject )->getGType();
		btmp->unbalanced = dynamic_cast<KPTextObject*>( kpobject )->getGUnbalanced();
		btmp->xfactor = dynamic_cast<KPTextObject*>( kpobject )->getGXFactor();
		btmp->yfactor = dynamic_cast<KPTextObject*>( kpobject )->getGYFactor();
		ret = true;
		break;
	    case OT_PICTURE:
		ptmp->pen = QPen( dynamic_cast<KPPixmapObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPPixmapObject*>( kpobject )->getBrush() );
		btmp->fillType = dynamic_cast<KPPixmapObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPPixmapObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPPixmapObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPPixmapObject*>( kpobject )->getGType();
		btmp->unbalanced = dynamic_cast<KPPixmapObject*>( kpobject )->getGUnbalanced();
		btmp->xfactor = dynamic_cast<KPPixmapObject*>( kpobject )->getGXFactor();
		btmp->yfactor = dynamic_cast<KPPixmapObject*>( kpobject )->getGYFactor();
		ret = true;
		break;
	    case OT_CLIPART:
		ptmp->pen = QPen( dynamic_cast<KPClipartObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPClipartObject*>( kpobject )->getBrush() );
		btmp->fillType = dynamic_cast<KPClipartObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPClipartObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPClipartObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPClipartObject*>( kpobject )->getGType();
		btmp->unbalanced = dynamic_cast<KPClipartObject*>( kpobject )->getGUnbalanced();
		btmp->xfactor = dynamic_cast<KPClipartObject*>( kpobject )->getGXFactor();
		btmp->yfactor = dynamic_cast<KPClipartObject*>( kpobject )->getGYFactor();
		ret = true;
		break;
	    default: break;
	    }
	    _oldPen.append( ptmp );
	    _oldBrush.append( btmp );
	    _objects.append( kpobject );
	}
    }

    if ( !_objects.isEmpty() ) {
	PenBrushCmd *penBrushCmd = new PenBrushCmd( i18n( "Apply Styles" ), _oldPen, _oldBrush,
						    _newPen, _newBrush, _objects, this );
	commands()->addCommand( penBrushCmd );
	penBrushCmd->execute();
    } else {
	_oldPen.setAutoDelete( true );
	_oldPen.clear();
	_oldBrush.setAutoDelete( true );
	_oldBrush.clear();
    }

    setModified(true);
    return ret;
}

/*================================================================*/
bool KPresenterDoc::setLineBegin( LineEnd lb )
{
    KPObject *kpobject = 0;
    bool ret = false;

    QList<KPObject> _objects;
    QList<PenBrushCmd::Pen> _oldPen;
    QList<PenBrushCmd::Brush> _oldBrush;
    PenBrushCmd::Pen _newPen, *ptmp;
    PenBrushCmd::Brush _newBrush, *btmp;

    _newPen.lineBegin = lb;

    _objects.setAutoDelete( false );
    _oldPen.setAutoDelete( false );
    _oldBrush.setAutoDelete( false );

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
	kpobject = objectList()->at( i );
	if ( kpobject->isSelected() ) {
	    btmp = new PenBrushCmd::Brush;
	    ptmp = new PenBrushCmd::Pen;
	    switch ( kpobject->getType() ) {
	    case OT_LINE: {
		ptmp->pen = QPen( dynamic_cast<KPLineObject*>( kpobject )->getPen() );
		ptmp->lineBegin = dynamic_cast<KPLineObject*>( kpobject )->getLineBegin();
		ptmp->lineEnd = dynamic_cast<KPLineObject*>( kpobject )->getLineEnd();
		ret = true;
	    } break;
	    case OT_AUTOFORM: {
		ptmp->pen = QPen( dynamic_cast<KPAutoformObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPAutoformObject*>( kpobject )->getBrush() );
		ptmp->lineBegin = dynamic_cast<KPAutoformObject*>( kpobject )->getLineBegin();
		ptmp->lineEnd = dynamic_cast<KPAutoformObject*>( kpobject )->getLineEnd();
		btmp->fillType = dynamic_cast<KPAutoformObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPAutoformObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPAutoformObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPAutoformObject*>( kpobject )->getGType();
		ret = true;
	    } break;
	    default: continue; break;
	    }
	    _oldPen.append( ptmp );
	    _oldBrush.append( btmp );
	    _objects.append( kpobject );
	}
    }

    if ( !_objects.isEmpty() ) {
	PenBrushCmd *penBrushCmd = new PenBrushCmd( i18n( "Change Line Begin" ), _oldPen, _oldBrush,
						    _newPen, _newBrush, _objects, this, PenBrushCmd::LB_ONLY );
	commands()->addCommand( penBrushCmd );
	penBrushCmd->execute();
    } else {
	_oldPen.setAutoDelete( true );
	_oldPen.clear();
	_oldBrush.setAutoDelete( true );
	_oldBrush.clear();
    }

    setModified(true);
    return ret;
}

/*================================================================*/
bool KPresenterDoc::setLineEnd( LineEnd le )
{
    KPObject *kpobject = 0;
    bool ret = false;

    QList<KPObject> _objects;
    QList<PenBrushCmd::Pen> _oldPen;
    QList<PenBrushCmd::Brush> _oldBrush;
    PenBrushCmd::Pen _newPen, *ptmp;
    PenBrushCmd::Brush _newBrush, *btmp;

    _newPen.lineEnd = le;

    _objects.setAutoDelete( false );
    _oldPen.setAutoDelete( false );
    _oldBrush.setAutoDelete( false );

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
	kpobject = objectList()->at( i );
	if ( kpobject->isSelected() ) {
	    btmp = new PenBrushCmd::Brush;
	    ptmp = new PenBrushCmd::Pen;
	    switch ( kpobject->getType() )
	    {
	    case OT_LINE: {
		ptmp->pen = QPen( dynamic_cast<KPLineObject*>( kpobject )->getPen() );
		ptmp->lineBegin = dynamic_cast<KPLineObject*>( kpobject )->getLineBegin();
		ptmp->lineEnd = dynamic_cast<KPLineObject*>( kpobject )->getLineEnd();
		ret = true;
	    } break;
	    case OT_AUTOFORM: {
		ptmp->pen = QPen( dynamic_cast<KPAutoformObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPAutoformObject*>( kpobject )->getBrush() );
		ptmp->lineBegin = dynamic_cast<KPAutoformObject*>( kpobject )->getLineBegin();
		ptmp->lineEnd = dynamic_cast<KPAutoformObject*>( kpobject )->getLineEnd();
		btmp->fillType = dynamic_cast<KPAutoformObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPAutoformObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPAutoformObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPAutoformObject*>( kpobject )->getGType();
		ret = true;
	    } break;
	    default: continue; break;
	    }
	    _oldPen.append( ptmp );
	    _oldBrush.append( btmp );
	    _objects.append( kpobject );
	}
    }

    if ( !_objects.isEmpty() ) {
	PenBrushCmd *penBrushCmd = new PenBrushCmd( i18n( "Change Line End" ), _oldPen, _oldBrush,
						    _newPen, _newBrush, _objects, this, PenBrushCmd::LE_ONLY );
	commands()->addCommand( penBrushCmd );
	penBrushCmd->execute();
    } else {
	_oldPen.setAutoDelete( true );
	_oldPen.clear();
	_oldBrush.setAutoDelete( true );
	_oldBrush.clear();
    }

    setModified(true);
    return ret;
}

/*================================================================*/
bool KPresenterDoc::setPieSettings( PieType pieType, int angle, int len )
{
    bool ret = false;

    KPObject *kpobject = 0;
    QList<KPObject> _objects;
    QList<PieValueCmd::PieValues> _oldValues;
    PieValueCmd::PieValues _newValues, *tmp;

    _objects.setAutoDelete( false );
    _oldValues.setAutoDelete( false );

    _newValues.pieType = pieType;
    _newValues.pieAngle = angle;
    _newValues.pieLength = len;

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
	kpobject = objectList()->at( i );
	if ( kpobject->getType() == OT_PIE ) {
	    if ( kpobject->isSelected() ) {
		tmp = new PieValueCmd::PieValues;
		tmp->pieType = dynamic_cast<KPPieObject*>( kpobject )->getPieType();
		tmp->pieAngle = dynamic_cast<KPPieObject*>( kpobject )->getPieAngle();
		tmp->pieLength = dynamic_cast<KPPieObject*>( kpobject )->getPieLength();
		_oldValues.append( tmp );
		_objects.append( kpobject );
		ret = true;
	    }
	}
    }

    if ( !_objects.isEmpty() ) {
	PieValueCmd *pieValueCmd = new PieValueCmd( i18n( "Change Pie/Arc/Chord Values" ),
						    _oldValues, _newValues, _objects, this );
	commands()->addCommand( pieValueCmd );
	pieValueCmd->execute();
    } else {
	_oldValues.setAutoDelete( true );
	_oldValues.clear();
    }

    setModified(true);
    return ret;
}

/*================================================================*/
bool KPresenterDoc::setRectSettings( int _rx, int _ry )
{
    bool ret = false;

    KPObject *kpobject = 0;
    QList<KPObject> _objects;
    QList<RectValueCmd::RectValues> _oldValues;
    RectValueCmd::RectValues _newValues, *tmp;

    _objects.setAutoDelete( false );
    _oldValues.setAutoDelete( false );

    _newValues.xRnd = _rx;
    _newValues.yRnd = _ry;

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
	kpobject = objectList()->at( i );
	if ( kpobject->getType() == OT_RECT ) {
	    if ( kpobject->isSelected() ) {
		tmp = new RectValueCmd::RectValues;
		dynamic_cast<KPRectObject*>( kpobject )->getRnds( tmp->xRnd, tmp->yRnd );
		_oldValues.append( tmp );
		_objects.append( kpobject );
		ret = true;
	    }
	}
    }

    if ( !_objects.isEmpty() ) {
	RectValueCmd *rectValueCmd = new RectValueCmd( i18n( "Change Rectangle values" ), _oldValues,
						       _newValues, _objects, this );
	commands()->addCommand( rectValueCmd );
	rectValueCmd->execute();
    } else {
	_oldValues.setAutoDelete( true );
	_oldValues.clear();
    }

    setModified(true);
    return ret;
}

/*================================================================*/
bool KPresenterDoc::setPenColor( QColor c, bool fill )
{
    KPObject *kpobject = 0;
    bool ret = false;

    QList<KPObject> _objects;
    QList<PenBrushCmd::Pen> _oldPen;
    QList<PenBrushCmd::Brush> _oldBrush;
    PenBrushCmd::Pen _newPen, *ptmp;
    PenBrushCmd::Brush _newBrush, *btmp;

    if ( !fill )
	_newPen.pen = NoPen;
    else
	_newPen.pen = QPen( c );

    _objects.setAutoDelete( false );
    _oldPen.setAutoDelete( false );
    _oldBrush.setAutoDelete( false );

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
	kpobject = objectList()->at( i );
	if ( kpobject->isSelected() ) {
	    ptmp = new PenBrushCmd::Pen;
	    btmp = new PenBrushCmd::Brush;
	    switch ( kpobject->getType() ) {
	    case OT_LINE: {
		ptmp->pen = QPen( dynamic_cast<KPLineObject*>( kpobject )->getPen() );
		ptmp->lineBegin = dynamic_cast<KPLineObject*>( kpobject )->getLineBegin();
		ptmp->lineEnd = dynamic_cast<KPLineObject*>( kpobject )->getLineEnd();
		ret = true;
	    } break;
	    case OT_RECT: {
		ptmp->pen = QPen( dynamic_cast<KPRectObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPRectObject*>( kpobject )->getBrush() );
		btmp->fillType = dynamic_cast<KPRectObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPRectObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPRectObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPRectObject*>( kpobject )->getGType();
		ret = true;
	    } break;
	    case OT_ELLIPSE: {
		ptmp->pen = QPen( dynamic_cast<KPEllipseObject*>( kpobject )->getPen() );
		btmp->brush = dynamic_cast<KPEllipseObject*>( kpobject )->getBrush();
		btmp->fillType = dynamic_cast<KPEllipseObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPEllipseObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPEllipseObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPEllipseObject*>( kpobject )->getGType();
		ret = true;
	    } break;
	    case OT_AUTOFORM: {
		ptmp->pen = QPen( dynamic_cast<KPAutoformObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPAutoformObject*>( kpobject )->getBrush() );
		ptmp->lineBegin = dynamic_cast<KPAutoformObject*>( kpobject )->getLineBegin();
		ptmp->lineEnd = dynamic_cast<KPAutoformObject*>( kpobject )->getLineEnd();
		btmp->fillType = dynamic_cast<KPAutoformObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPAutoformObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPAutoformObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPAutoformObject*>( kpobject )->getGType();
		ret = true;
	    } break;
	    case OT_PIE: {
		ptmp->pen = QPen( dynamic_cast<KPPieObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPPieObject*>( kpobject )->getBrush() );
		ptmp->lineBegin = dynamic_cast<KPPieObject*>( kpobject )->getLineBegin();
		ptmp->lineEnd = dynamic_cast<KPPieObject*>( kpobject )->getLineEnd();
		btmp->fillType = dynamic_cast<KPPieObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPPieObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPPieObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPPieObject*>( kpobject )->getGType();
		ret = true;
	    } break;
	    case OT_PART: {
		ptmp->pen = QPen( dynamic_cast<KPPartObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPPartObject*>( kpobject )->getBrush() );
		btmp->fillType = dynamic_cast<KPPartObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPPartObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPPartObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPPartObject*>( kpobject )->getGType();
		ret = true;
	    } break;
	    case OT_TEXT: {
		ptmp->pen = QPen( dynamic_cast<KPTextObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPTextObject*>( kpobject )->getBrush() );
		btmp->fillType = dynamic_cast<KPTextObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPTextObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPTextObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPTextObject*>( kpobject )->getGType();
		ret = true;
	    } break;
	    case OT_PICTURE: {
		ptmp->pen = QPen( dynamic_cast<KPPixmapObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPPixmapObject*>( kpobject )->getBrush() );
		btmp->fillType = dynamic_cast<KPPixmapObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPPixmapObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPPixmapObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPPixmapObject*>( kpobject )->getGType();
		ret = true;
	    } break;
	    case OT_CLIPART: {
		ptmp->pen = QPen( dynamic_cast<KPClipartObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPClipartObject*>( kpobject )->getBrush() );
		btmp->fillType = dynamic_cast<KPClipartObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPClipartObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPClipartObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPClipartObject*>( kpobject )->getGType();
		ret = true;
	    } break;
	    default: break;
	    }
	    _oldPen.append( ptmp );
	    _oldBrush.append( btmp );
	    _objects.append( kpobject );
	}
    }

    if ( !_objects.isEmpty() ) {
	PenBrushCmd *penBrushCmd = new PenBrushCmd( i18n( "Change Pen" ), _oldPen, _oldBrush, _newPen,
						    _newBrush, _objects, this, PenBrushCmd::PEN_ONLY );
	commands()->addCommand( penBrushCmd );
	penBrushCmd->execute();
    } else {
	_oldPen.setAutoDelete( true );
	_oldPen.clear();
	_oldBrush.setAutoDelete( true );
	_oldBrush.clear();
    }

    setModified(true);
    return ret;
}

/*================================================================*/
bool KPresenterDoc::setBrushColor( QColor c, bool fill )
{
    KPObject *kpobject = 0;
    bool ret = false;

    QList<KPObject> _objects;
    QList<PenBrushCmd::Pen> _oldPen;
    QList<PenBrushCmd::Brush> _oldBrush;
    PenBrushCmd::Pen _newPen, *ptmp;
    PenBrushCmd::Brush _newBrush, *btmp;

    _newBrush.fillType = FT_BRUSH;
    if ( !fill )
	_newBrush.brush = NoBrush;
    else
	_newBrush.brush = QBrush( c );

    _objects.setAutoDelete( false );
    _oldPen.setAutoDelete( false );
    _oldBrush.setAutoDelete( false );


    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
	kpobject = objectList()->at( i );
	if ( kpobject->isSelected() && kpobject->getType() != OT_LINE ) {
	    ptmp = new PenBrushCmd::Pen;
	    btmp = new PenBrushCmd::Brush;
	    switch ( kpobject->getType() )
	    {
	    case OT_RECT: {
		ptmp->pen = QPen( dynamic_cast<KPRectObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPRectObject*>( kpobject )->getBrush() );
		btmp->fillType = dynamic_cast<KPRectObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPRectObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPRectObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPRectObject*>( kpobject )->getGType();
		ret = true;
	    } break;
	    case OT_ELLIPSE: {
		ptmp->pen = QPen( dynamic_cast<KPEllipseObject*>( kpobject )->getPen() );
		btmp->brush = dynamic_cast<KPEllipseObject*>( kpobject )->getBrush();
		btmp->fillType = dynamic_cast<KPEllipseObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPEllipseObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPEllipseObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPEllipseObject*>( kpobject )->getGType();
		ret = true;
	    } break;
	    case OT_AUTOFORM: {
		ptmp->pen = QPen( dynamic_cast<KPAutoformObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPAutoformObject*>( kpobject )->getBrush() );
		ptmp->lineBegin = dynamic_cast<KPAutoformObject*>( kpobject )->getLineBegin();
		ptmp->lineEnd = dynamic_cast<KPAutoformObject*>( kpobject )->getLineEnd();
		btmp->fillType = dynamic_cast<KPAutoformObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPAutoformObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPAutoformObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPAutoformObject*>( kpobject )->getGType();
		ret = true;
	    } break;
	    case OT_PIE: {
		ptmp->pen = QPen( dynamic_cast<KPPieObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPPieObject*>( kpobject )->getBrush() );
		ptmp->lineBegin = dynamic_cast<KPPieObject*>( kpobject )->getLineBegin();
		ptmp->lineEnd = dynamic_cast<KPPieObject*>( kpobject )->getLineEnd();
		btmp->fillType = dynamic_cast<KPPieObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPPieObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPPieObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPPieObject*>( kpobject )->getGType();
		ret = true;
	    } break;
	    case OT_PART: {
		ptmp->pen = QPen( dynamic_cast<KPPartObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPPartObject*>( kpobject )->getBrush() );
		btmp->fillType = dynamic_cast<KPPartObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPPartObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPPartObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPPartObject*>( kpobject )->getGType();
		ret = true;
	    } break;
	    case OT_TEXT: {
		ptmp->pen = QPen( dynamic_cast<KPTextObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPTextObject*>( kpobject )->getBrush() );
		btmp->fillType = dynamic_cast<KPTextObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPTextObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPTextObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPTextObject*>( kpobject )->getGType();
		ret = true;
	    } break;
	    case OT_PICTURE: {
		ptmp->pen = QPen( dynamic_cast<KPPixmapObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPPixmapObject*>( kpobject )->getBrush() );
		btmp->fillType = dynamic_cast<KPPixmapObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPPixmapObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPPixmapObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPPixmapObject*>( kpobject )->getGType();
		ret = true;
	    } break;
	    case OT_CLIPART: {
		ptmp->pen = QPen( dynamic_cast<KPClipartObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPClipartObject*>( kpobject )->getBrush() );
		btmp->fillType = dynamic_cast<KPClipartObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPClipartObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPClipartObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPClipartObject*>( kpobject )->getGType();
		ret = true;
	    } break;
	    default: continue; break;
	    }
	    _oldPen.append( ptmp );
	    _oldBrush.append( btmp );
	    _objects.append( kpobject );
	}
    }

    if ( !_objects.isEmpty() ) {
	PenBrushCmd *penBrushCmd = new PenBrushCmd( i18n( "Change Brush" ), _oldPen, _oldBrush, _newPen,
						    _newBrush, _objects, this, PenBrushCmd::BRUSH_ONLY );
	commands()->addCommand( penBrushCmd );
	penBrushCmd->execute();
    } else {
	_oldPen.setAutoDelete( true );
	_oldPen.clear();
	_oldBrush.setAutoDelete( true );
	_oldBrush.clear();
    }

    setModified(true);
    return ret;
}

/*=============================================================*/
BackType KPresenterDoc::getBackType( unsigned int pageNum )
{
    if ( pageNum < _backgroundList.count() )
	return backgroundList()->at( pageNum )->getBackType();

    return BT_COLOR;
}

/*=============================================================*/
BackView KPresenterDoc::getBackView( unsigned int pageNum )
{
    if ( pageNum < _backgroundList.count() )
	return backgroundList()->at( pageNum )->getBackView();

    return BV_TILED;
}

/*=============================================================*/
QString KPresenterDoc::getBackPixFilename( unsigned int pageNum )
{
    if ( pageNum < _backgroundList.count() )
	return backgroundList()->at( pageNum )->getBackPixFilename();

    return QString::null;
}

/*=============================================================*/
QString KPresenterDoc::getBackClipFilename( unsigned int pageNum )
{
    if ( pageNum < _backgroundList.count() )
	return backgroundList()->at( pageNum )->getBackClipFilename();

    return QString::null;
}

/*=============================================================*/
QDateTime KPresenterDoc::getBackPixLastModified( unsigned int pageNum )
{
    if ( pageNum < _backgroundList.count() )
	return backgroundList()->at( pageNum )->getKey().lastModified;

    return QDateTime();
}

/*=============================================================*/
QDateTime KPresenterDoc::getBackClipLastModified( unsigned int pageNum )
{
    if ( pageNum < _backgroundList.count() )
	return backgroundList()->at( pageNum )->getKey().lastModified;

    return QDateTime();
}

/*=============================================================*/
QColor KPresenterDoc::getBackColor1( unsigned int pageNum )
{
    if ( pageNum < _backgroundList.count() )
	return backgroundList()->at( pageNum )->getBackColor1();

    return white;
}

/*=============================================================*/
QColor KPresenterDoc::getBackColor2( unsigned int pageNum )
{
    if ( pageNum < _backgroundList.count() )
	return backgroundList()->at( pageNum )->getBackColor2();

    return white;
}

/*=============================================================*/
bool KPresenterDoc::getBackUnbalanced( unsigned int pageNum )
{
    if ( pageNum < _backgroundList.count() )
	return backgroundList()->at( pageNum )->getBackUnbalanced();

    return false;
}

/*=============================================================*/
int KPresenterDoc::getBackXFactor( unsigned int pageNum )
{
    if ( pageNum < _backgroundList.count() )
	return backgroundList()->at( pageNum )->getBackXFactor();

    return 100;
}

/*=============================================================*/
int KPresenterDoc::getBackYFactor( unsigned int pageNum )
{
    if ( pageNum < _backgroundList.count() )
	return backgroundList()->at( pageNum )->getBackYFactor();

    return 100;
}

/*=============================================================*/
BCType KPresenterDoc::getBackColorType( unsigned int pageNum )
{
    if ( pageNum < _backgroundList.count() )
	return backgroundList()->at( pageNum )->getBackColorType();

    return BCT_PLAIN;
}

/*=============================================================*/
PageEffect KPresenterDoc::getPageEffect( unsigned int pageNum )
{
    if ( pageNum < _backgroundList.count() )
	return backgroundList()->at( pageNum )->getPageEffect();

    return PEF_NONE;
}

/*=============================================================*/
QPen KPresenterDoc::getPen( QPen pen )
{
    KPObject *kpobject = 0;

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
	kpobject = objectList()->at( i );
	if ( kpobject->isSelected() ) {
	    switch ( kpobject->getType() ) {
	    case OT_LINE:
		return dynamic_cast<KPLineObject*>( kpobject )->getPen();
		break;
	    case OT_RECT:
		return dynamic_cast<KPRectObject*>( kpobject )->getPen();
		break;
	    case OT_ELLIPSE:
		return dynamic_cast<KPEllipseObject*>( kpobject )->getPen();
		break;
	    case OT_PIE:
		return dynamic_cast<KPPieObject*>( kpobject )->getPen();
		break;
	    case OT_AUTOFORM:
		return dynamic_cast<KPAutoformObject*>( kpobject )->getPen();
		break;
	    case OT_PART:
		return dynamic_cast<KPPartObject*>( kpobject )->getPen();
		break;
	    case OT_PICTURE:
		return dynamic_cast<KPPixmapObject*>( kpobject )->getPen();
		break;
	    case OT_CLIPART:
		return dynamic_cast<KPClipartObject*>( kpobject )->getPen();
		break;
	    case OT_TEXT:
		return dynamic_cast<KPTextObject*>( kpobject )->getPen();
		break;
	    default: break;
	    }
	}
    }

    return pen;
}

/*========================= get line begin ========================*/
LineEnd KPresenterDoc::getLineBegin( LineEnd lb )
{
    KPObject *kpobject = 0;

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
	kpobject = objectList()->at( i );
	if ( kpobject->isSelected() ) {
	    switch ( kpobject->getType() )
	    {
	    case OT_LINE:
		return dynamic_cast<KPLineObject*>( kpobject )->getLineBegin();
		break;
	    case OT_AUTOFORM:
		return dynamic_cast<KPAutoformObject*>( kpobject )->getLineBegin();
		break;
	    case OT_PIE:
		return dynamic_cast<KPPieObject*>( kpobject )->getLineBegin();
		break;
	    default: break;
	    }
	}
    }

    return lb;
}

/*========================= get line end =========================*/
LineEnd KPresenterDoc::getLineEnd( LineEnd le )
{
    KPObject *kpobject = 0;

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
	kpobject = objectList()->at( i );
	if ( kpobject->isSelected() ) {
	    switch ( kpobject->getType() ) {
	    case OT_LINE:
		return dynamic_cast<KPLineObject*>( kpobject )->getLineEnd();
		break;
	    case OT_AUTOFORM:
		return dynamic_cast<KPAutoformObject*>( kpobject )->getLineEnd();
		break;
	    case OT_PIE:
		return dynamic_cast<KPPieObject*>( kpobject )->getLineEnd();
		break;
	    default: break;
	    }
	}
    }

    return le;
}

/*========================= get brush =============================*/
QBrush KPresenterDoc::getBrush( QBrush brush )
{
    KPObject *kpobject = 0;

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
	kpobject = objectList()->at( i );
	if ( kpobject->isSelected() ) {
	    switch ( kpobject->getType() ) {
	    case OT_RECT:
		return dynamic_cast<KPRectObject*>( kpobject )->getBrush();
		break;
	    case OT_ELLIPSE:
		return dynamic_cast<KPEllipseObject*>( kpobject )->getBrush();
		break;
	    case OT_AUTOFORM:
		return dynamic_cast<KPAutoformObject*>( kpobject )->getBrush();
		break;
	    case OT_PIE:
		return dynamic_cast<KPPieObject*>( kpobject )->getBrush();
		break;
	    case OT_PART:
		return dynamic_cast<KPPartObject*>( kpobject )->getBrush();
		break;
	    case OT_PICTURE:
		return dynamic_cast<KPPixmapObject*>( kpobject )->getBrush();
		break;
	    case OT_CLIPART:
		return dynamic_cast<KPClipartObject*>( kpobject )->getBrush();
		break;
	    case OT_TEXT:
		return dynamic_cast<KPTextObject*>( kpobject )->getBrush();
		break;
	    default: break;
	    }
	}
    }

    return brush;
}

/*================================================================*/
FillType KPresenterDoc::getFillType( FillType ft )
{
    KPObject *kpobject = 0;

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
	kpobject = objectList()->at( i );
	if ( kpobject->isSelected() ) {
	    switch ( kpobject->getType() ) {
	    case OT_RECT:
		return dynamic_cast<KPRectObject*>( kpobject )->getFillType();
		break;
	    case OT_ELLIPSE:
		return dynamic_cast<KPEllipseObject*>( kpobject )->getFillType();
		break;
	    case OT_AUTOFORM:
		return dynamic_cast<KPAutoformObject*>( kpobject )->getFillType();
		break;
	    case OT_PIE:
		return dynamic_cast<KPPieObject*>( kpobject )->getFillType();
		break;
	    case OT_PART:
		return dynamic_cast<KPPartObject*>( kpobject )->getFillType();
		break;
	    case OT_PICTURE:
		return dynamic_cast<KPPixmapObject*>( kpobject )->getFillType();
		break;
	    case OT_CLIPART:
		return dynamic_cast<KPClipartObject*>( kpobject )->getFillType();
		break;
	    case OT_TEXT:
		return dynamic_cast<KPTextObject*>( kpobject )->getFillType();
		break;
	    default: break;
	    }
	}
    }

    return ft;
}

/*================================================================*/
QColor KPresenterDoc::getGColor1( QColor g1 )
{
    KPObject *kpobject = 0;

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
	kpobject = objectList()->at( i );
	if ( kpobject->isSelected() ) {
	    switch ( kpobject->getType() ) {
	    case OT_RECT:
		return dynamic_cast<KPRectObject*>( kpobject )->getGColor1();
		break;
	    case OT_ELLIPSE:
		return dynamic_cast<KPEllipseObject*>( kpobject )->getGColor1();
		break;
	    case OT_AUTOFORM:
		return dynamic_cast<KPAutoformObject*>( kpobject )->getGColor1();
		break;
	    case OT_PIE:
		return dynamic_cast<KPPieObject*>( kpobject )->getGColor1();
		break;
	    case OT_PART:
		return dynamic_cast<KPPartObject*>( kpobject )->getGColor1();
		break;
	    case OT_PICTURE:
		return dynamic_cast<KPPixmapObject*>( kpobject )->getGColor1();
		break;
	    case OT_CLIPART:
		return dynamic_cast<KPClipartObject*>( kpobject )->getGColor1();
		break;
	    case OT_TEXT:
		return dynamic_cast<KPTextObject*>( kpobject )->getGColor1();
		break;
	    default: break;
	    }
	}
    }

    return g1;
}

/*================================================================*/
QColor KPresenterDoc::getGColor2( QColor g2 )
{
    KPObject *kpobject = 0;

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
	kpobject = objectList()->at( i );
	if ( kpobject->isSelected() ) {
	    switch ( kpobject->getType() ) {
	    case OT_RECT:
		return dynamic_cast<KPRectObject*>( kpobject )->getGColor2();
		break;
	    case OT_ELLIPSE:
		return dynamic_cast<KPEllipseObject*>( kpobject )->getGColor2();
		break;
	    case OT_AUTOFORM:
		return dynamic_cast<KPAutoformObject*>( kpobject )->getGColor2();
		break;
	    case OT_PIE:
		return dynamic_cast<KPPieObject*>( kpobject )->getGColor2();
		break;
	    case OT_PART:
		return dynamic_cast<KPPartObject*>( kpobject )->getGColor2();
		break;
	    case OT_PICTURE:
		return dynamic_cast<KPPixmapObject*>( kpobject )->getGColor2();
		break;
	    case OT_CLIPART:
		return dynamic_cast<KPClipartObject*>( kpobject )->getGColor2();
		break;
	    case OT_TEXT:
		return dynamic_cast<KPTextObject*>( kpobject )->getGColor2();
		break;
	    default: break;
	    }
	}
    }

    return g2;
}

/*================================================================*/
BCType KPresenterDoc::getGType( BCType gt )
{
    KPObject *kpobject = 0;

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
	kpobject = objectList()->at( i );
	if ( kpobject->isSelected() ) {
	    switch ( kpobject->getType() ) {
	    case OT_RECT:
		return dynamic_cast<KPRectObject*>( kpobject )->getGType();
		break;
	    case OT_ELLIPSE:
		return dynamic_cast<KPEllipseObject*>( kpobject )->getGType();
		break;
	    case OT_AUTOFORM:
		return dynamic_cast<KPAutoformObject*>( kpobject )->getGType();
		break;
	    case OT_PIE:
		return dynamic_cast<KPPieObject*>( kpobject )->getGType();
		break;
	    case OT_PART:
		return dynamic_cast<KPPartObject*>( kpobject )->getGType();
		break;
	    case OT_PICTURE:
		return dynamic_cast<KPPixmapObject*>( kpobject )->getGType();
		break;
	    case OT_CLIPART:
		return dynamic_cast<KPClipartObject*>( kpobject )->getGType();
		break;
	    case OT_TEXT:
		return dynamic_cast<KPTextObject*>( kpobject )->getGType();
		break;
	    default: break;
	    }
	}
    }

    return gt;
}

/*================================================================*/
bool KPresenterDoc::getGUnbalanced( bool  unbalanced )
{
    KPObject *kpobject = 0;

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
	kpobject = objectList()->at( i );
	if ( kpobject->isSelected() ) {
	    switch ( kpobject->getType() ) {
	    case OT_RECT:
		return dynamic_cast<KPRectObject*>( kpobject )->getGUnbalanced();
		break;
	    case OT_ELLIPSE:
		return dynamic_cast<KPEllipseObject*>( kpobject )->getGUnbalanced();
		break;
	    case OT_AUTOFORM:
		return dynamic_cast<KPAutoformObject*>( kpobject )->getGUnbalanced();
		break;
	    case OT_PIE:
		return dynamic_cast<KPPieObject*>( kpobject )->getGUnbalanced();
		break;
	    case OT_PART:
		return dynamic_cast<KPPartObject*>( kpobject )->getGUnbalanced();
		break;
	    case OT_PICTURE:
		return dynamic_cast<KPPixmapObject*>( kpobject )->getGUnbalanced();
		break;
	    case OT_CLIPART:
		return dynamic_cast<KPClipartObject*>( kpobject )->getGUnbalanced();
		break;
	    case OT_TEXT:
		return dynamic_cast<KPTextObject*>( kpobject )->getGUnbalanced();
		break;
	    default: break;
	    }
	}
    }

    return unbalanced;
}

/*================================================================*/
int KPresenterDoc::getGXFactor( int xfactor )
{
    KPObject *kpobject = 0;

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
	kpobject = objectList()->at( i );
	if ( kpobject->isSelected() ) {
	    switch ( kpobject->getType() ) {
	    case OT_RECT:
		return dynamic_cast<KPRectObject*>( kpobject )->getGXFactor();
		break;
	    case OT_ELLIPSE:
		return dynamic_cast<KPEllipseObject*>( kpobject )->getGXFactor();
		break;
	    case OT_AUTOFORM:
		return dynamic_cast<KPAutoformObject*>( kpobject )->getGXFactor();
		break;
	    case OT_PIE:
		return dynamic_cast<KPPieObject*>( kpobject )->getGXFactor();
		break;
	    case OT_PART:
		return dynamic_cast<KPPartObject*>( kpobject )->getGXFactor();
		break;
	    case OT_PICTURE:
		return dynamic_cast<KPPixmapObject*>( kpobject )->getGXFactor();
		break;
	    case OT_CLIPART:
		return dynamic_cast<KPClipartObject*>( kpobject )->getGXFactor();
		break;
	    case OT_TEXT:
		return dynamic_cast<KPTextObject*>( kpobject )->getGXFactor();
		break;
	    default: break;
	    }
	}
    }

    return xfactor;
}

/*================================================================*/
int KPresenterDoc::getGYFactor( int yfactor )
{
    KPObject *kpobject = 0;

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
	kpobject = objectList()->at( i );
	if ( kpobject->isSelected() ) {
	    switch ( kpobject->getType() ) {
	    case OT_RECT:
		return dynamic_cast<KPRectObject*>( kpobject )->getGYFactor();
		break;
	    case OT_ELLIPSE:
		return dynamic_cast<KPEllipseObject*>( kpobject )->getGYFactor();
		break;
	    case OT_AUTOFORM:
		return dynamic_cast<KPAutoformObject*>( kpobject )->getGYFactor();
		break;
	    case OT_PIE:
		return dynamic_cast<KPPieObject*>( kpobject )->getGYFactor();
		break;
	    case OT_PART:
		return dynamic_cast<KPPartObject*>( kpobject )->getGYFactor();
		break;
	    case OT_PICTURE:
		return dynamic_cast<KPPixmapObject*>( kpobject )->getGYFactor();
		break;
	    case OT_CLIPART:
		return dynamic_cast<KPClipartObject*>( kpobject )->getGYFactor();
		break;
	    case OT_TEXT:
		return dynamic_cast<KPTextObject*>( kpobject )->getGYFactor();
		break;
	    default: break;
	    }
	}
    }

    return yfactor;
}

/*================================================================*/
PieType KPresenterDoc::getPieType( PieType pieType )
{
    KPObject *kpobject = 0;

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
	kpobject = objectList()->at( i );
	if ( kpobject->isSelected() && kpobject->getType() == OT_PIE )
	    return dynamic_cast<KPPieObject*>( kpobject )->getPieType();
    }

    return pieType;
}

bool KPresenterDoc::getSticky( bool s )
{
    KPObject *kpobject = 0;

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
	kpobject = objectList()->at( i );
	if ( kpobject->isSelected() )
	    return kpobject->isSticky();
    }

    return s;
}

/*================================================================*/
int KPresenterDoc::getPieLength( int pieLength )
{
    KPObject *kpobject = 0;

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
	kpobject = objectList()->at( i );
	if ( kpobject->isSelected() && kpobject->getType() == OT_PIE )
	    return dynamic_cast<KPPieObject*>( kpobject )->getPieLength();
    }

    return pieLength;
}

/*================================================================*/
int KPresenterDoc::getPieAngle( int pieAngle )
{
    KPObject *kpobject = 0;

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
	kpobject = objectList()->at( i );
	if ( kpobject->isSelected() && kpobject->getType() == OT_PIE )
	    return dynamic_cast<KPPieObject*>( kpobject )->getPieAngle();
    }

    return pieAngle;
}

/*================================================================*/
int KPresenterDoc::getRndX( int _rx )
{
    KPObject *kpobject = 0;

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
	kpobject = objectList()->at( i );
	if ( kpobject->isSelected() && kpobject->getType() == OT_RECT ) {
	    int tmp;
	    dynamic_cast<KPRectObject*>( kpobject )->getRnds( _rx, tmp );
	    return _rx;
	}
    }

    return _rx;
}

/*================================================================*/
int KPresenterDoc::getRndY( int _ry )
{
    KPObject *kpobject = 0;

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
	kpobject = objectList()->at( i );
	if ( kpobject->isSelected() && kpobject->getType() == OT_RECT ) {
	    int tmp;
	    dynamic_cast<KPRectObject*>( kpobject )->getRnds( tmp, _ry );
	    return _ry;
	}
    }

    return _ry;
}

/*======================== lower objects =========================*/
void KPresenterDoc::lowerObjs( int /*diffx*/, int /*diffy*/ )
{
    KPObject *kpobject = 0;

    QList<KPObject> *_new = new QList<KPObject>;

    for ( unsigned int j = 0; j < _objectList->count(); j++ )
	_new->append( _objectList->at( j ) );

    _new->setAutoDelete( false );

    for ( int i = 0; i < static_cast<int>( _new->count() ); i++ ) {
	kpobject = _new->at( i );
	if ( kpobject->isSelected() ) {
	    _new->take( i );
	    _new->insert( 0, kpobject );
	}
    }

    LowerRaiseCmd *lrCmd = new LowerRaiseCmd( i18n( "Lower Object( s )" ), _objectList, _new, this );
    lrCmd->execute();
    _commands.addCommand( lrCmd );

    setModified(true);
}

/*========================= raise object =========================*/
void KPresenterDoc::raiseObjs( int /*diffx*/, int /*diffy*/ )
{
    KPObject *kpobject = 0;

    QList<KPObject> *_new = new QList<KPObject>;

    for ( unsigned int j = 0; j < _objectList->count(); j++ )
	_new->append( _objectList->at( j ) );

    _new->setAutoDelete( false );

    for ( int i = 0; i < static_cast<int>( _new->count() ); i++ ) {
	kpobject = objectList()->at( i );
	if ( kpobject->isSelected() ) {
	    _new->take( i );
	    _new->append( kpobject );
	}
    }

    LowerRaiseCmd *lrCmd = new LowerRaiseCmd( i18n( "Raise Object( s )" ), _objectList, _new, this );
    lrCmd->execute();
    _commands.addCommand( lrCmd );

    setModified(true);
}

/*=================== insert a picture ==========================*/
void KPresenterDoc::insertPicture( QString filename, int diffx, int diffy, int _x , int _y )
{
//    QMap< KPPixmapDataCollection::Key, QImage >::Iterator it = _pixmapCollection.getPixmapDataCollection().begin();
    KPImageCollection::Iterator it = _imageCollection.begin();
    QDateTime dt;

    if ( !QFileInfo( filename ).exists() ) {
	for( ; it != _imageCollection.end(); ++it ) {
	    if ( it.key().filename == filename ) {
		dt = it.key().lastModified;
		break;
	    }
	}
    }

    KPPixmapObject *kppixmapobject = new KPPixmapObject( &_imageCollection, filename, dt );
    kppixmapobject->setOrig( ( ( diffx + _x ) / _rastX ) * _rastX, ( ( diffy + _y ) / _rastY ) * _rastY );
    kppixmapobject->setSelected( true );

    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Picture" ), kppixmapobject, this );
    insertCmd->execute();
    _commands.addCommand( insertCmd );

    QRect s = getPageRect( 0, 0, 0 );
    float fakt = 1;
    if ( kppixmapobject->getSize().width() > s.width() )
	fakt = (float)s.width() / (float)kppixmapobject->getSize().width();
    if ( kppixmapobject->getSize().height() > s.height() )
	fakt = QMIN( fakt, (float)s.height() / (float)kppixmapobject->getSize().height() );

    if ( fakt < 1 ) {
	int w = (int)( fakt * (float)kppixmapobject->getSize().width() );
	int h = (int)( fakt * (float)kppixmapobject->getSize().height() );
	kppixmapobject->setSize( w, h );
	repaint( false );
    }

    setModified(true);
}

/*=================== insert a clipart ==========================*/
void KPresenterDoc::insertClipart( QString filename, int diffx, int diffy )
{
    QMap< KPClipartCollection::Key, QPicture >::Iterator it = _clipartCollection.begin();
    QDateTime dt;

    if ( !QFileInfo( filename ).exists() ) {
	for ( ; it != _clipartCollection.end(); ++it ) {
	    if ( it.key().filename == filename ) {
		dt = it.key().lastModified;
		break;
	    }
	}
    }

    KPClipartObject *kpclipartobject = new KPClipartObject( &_clipartCollection, filename, dt );
    kpclipartobject->setOrig( ( ( diffx + 10 ) / _rastX ) * _rastX, ( ( diffy + 10 ) / _rastY ) * _rastY );
    kpclipartobject->setSize( 150, 150 );
    kpclipartobject->setSelected( true );

    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Clipart" ), kpclipartobject, this );
    insertCmd->execute();
    _commands.addCommand( insertCmd );

    setModified(true);
}

/*======================= change picture ========================*/
void KPresenterDoc::changePicture( QString filename, int /*diffx*/, int /*diffy*/ )
{
    KPObject *kpobject = 0;

//    QMap< KPPixmapDataCollection::Key, QImage >::Iterator it = _pixmapCollection.getPixmapDataCollection().begin();
    KPImageCollection::Iterator it = _imageCollection.begin();
    QDateTime dt;

    if ( !QFileInfo( filename ).exists() ) {
	for( ; it != _imageCollection.end(); ++it ) {
	    if ( it.key().filename == filename ) {
		dt = it.key().lastModified;
		break;
	    }
	}
    }

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
	kpobject = objectList()->at( i );
	if ( kpobject->isSelected() && kpobject->getType() == OT_PICTURE ) {
	    KPPixmapObject *pix = new KPPixmapObject( &_imageCollection, filename, QDateTime() );

	    ChgPixCmd *chgPixCmd = new ChgPixCmd( i18n( "Change pixmap" ), dynamic_cast<KPPixmapObject*>( kpobject ),
						  pix, this );
	    chgPixCmd->execute();
	    _commands.addCommand( chgPixCmd );
	    break;
	}
    }

    setModified(true);
}

/*======================= change clipart ========================*/
void KPresenterDoc::changeClipart( QString filename, int /*diffx*/, int /*diffy*/ )
{
    QMap< KPClipartCollection::Key, QPicture >::Iterator it = _clipartCollection.begin();
    QDateTime dt;

    if ( !QFileInfo( filename ).exists() ) {
	for ( ; it != _clipartCollection.end(); ++it ) {
	    if ( it.key().filename == filename ) {
		dt = it.key().lastModified;
		break;
	    }
	}
    }

    KPObject *kpobject = 0;

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
	kpobject = objectList()->at( i );
	if ( kpobject->isSelected() && kpobject->getType() == OT_CLIPART ) {
	    ChgClipCmd *chgClipCmd = new ChgClipCmd( i18n( "Change clipart" ),
						     dynamic_cast<KPClipartObject*>( kpobject ),
						     dynamic_cast<KPClipartObject*>( kpobject )->getKey(),
						     KPClipartCollection::Key( filename, dt ), this );
	    chgClipCmd->execute();
	    _commands.addCommand( chgClipCmd );
	    break;
	}
    }

    setModified(true);
}

/*===================== insert a line ===========================*/
void KPresenterDoc::insertLine( QRect r, QPen pen, LineEnd lb, LineEnd le, LineType lt, int diffx, int diffy )
{
    KPLineObject *kplineobject = new KPLineObject( pen, lb, le, lt );
    kplineobject->setOrig( r.x() + diffx, r.y() + diffy );
    kplineobject->setSize( r.width(), r.height() );
    kplineobject->setSelected( true );

    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Line" ), kplineobject, this );
    insertCmd->execute();
    _commands.addCommand( insertCmd );

    setModified(true);
}

/*===================== insert a rectangle =======================*/
void KPresenterDoc::insertRectangle( QRect r, QPen pen, QBrush brush, FillType ft, QColor g1, QColor g2,
				     BCType gt, int rndX, int rndY, bool unbalanced, int xfactor, int yfactor, int diffx, int diffy )
{
    KPRectObject *kprectobject = new KPRectObject( pen, brush, ft, g1, g2, gt, rndX, rndY,
						   unbalanced, xfactor, yfactor );
    kprectobject->setOrig( r.x() + diffx, r.y() + diffy );
    kprectobject->setSize( r.width(), r.height() );
    kprectobject->setSelected( true );

    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Rectangle" ), kprectobject, this );
    insertCmd->execute();
    _commands.addCommand( insertCmd );

    setModified(true);
}

/*===================== insert a circle or ellipse ===============*/
void KPresenterDoc::insertCircleOrEllipse( QRect r, QPen pen, QBrush brush, FillType ft, QColor g1, QColor g2,
					   BCType gt, bool unbalanced, int xfactor, int yfactor, int diffx, int diffy )
{
    KPEllipseObject *kpellipseobject = new KPEllipseObject( pen, brush, ft, g1, g2, gt,
							    unbalanced, xfactor, yfactor );
    kpellipseobject->setOrig( r.x() + diffx, r.y() + diffy );
    kpellipseobject->setSize( r.width(), r.height() );
    kpellipseobject->setSelected( true );

    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Ellipse" ), kpellipseobject, this );
    insertCmd->execute();
    _commands.addCommand( insertCmd );

    setModified(true);
}

/*================================================================*/
void KPresenterDoc::insertPie( QRect r, QPen pen, QBrush brush, FillType ft, QColor g1, QColor g2,
			       BCType gt, PieType pt, int _angle, int _len, LineEnd lb, LineEnd le,
			       bool unbalanced, int xfactor, int yfactor, int diffx, int diffy )
{
    KPPieObject *kppieobject = new KPPieObject( pen, brush, ft, g1, g2, gt, pt, _angle,
						_len, lb, le, unbalanced, xfactor, yfactor );
    kppieobject->setOrig( r.x() + diffx, r.y() + diffy );
    kppieobject->setSize( r.width(), r.height() );
    kppieobject->setSelected( true );

    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Pie/Arc/Chord" ), kppieobject, this );
    insertCmd->execute();
    _commands.addCommand( insertCmd );

    setModified(true);
}

/*===================== insert a textobject =====================*/
void KPresenterDoc::insertText( QRect r, int diffx, int diffy, QString text, KPresenterView *_view )
{
    KPTextObject *kptextobject = new KPTextObject( this );
    kptextobject->setOrig( r.x() + diffx, r.y() + diffy );
    kptextobject->setSize( r.width(), r.height() );
    kptextobject->setSelected( true );
    if ( !text.isEmpty() && _view ) {
	kptextobject->getKTextObject()->clear();
	kptextobject->getKTextObject()->setText( text );
	kptextobject->getKTextObject()->document()->setFontToAll( _view->currFont() );
	kptextobject->getKTextObject()->document()->setColorToAll( _view->currColor() );
    }

    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Textbox" ), kptextobject, this );
    insertCmd->execute();
    _commands.addCommand( insertCmd );

    setModified(true);
}

/*======================= insert an autoform ====================*/
void KPresenterDoc::insertAutoform( QRect r, QPen pen, QBrush brush, LineEnd lb, LineEnd le, FillType ft,
				    QColor g1, QColor g2, BCType gt, QString fileName, bool unbalanced,
				    int xfactor, int yfactor, int diffx, int diffy )
{
    KPAutoformObject *kpautoformobject = new KPAutoformObject( pen, brush, fileName, lb, le, ft,
							       g1, g2, gt, unbalanced, xfactor, yfactor );
    kpautoformobject->setOrig( r.x() + diffx, r.y() + diffy );
    kpautoformobject->setSize( r.width(), r.height() );
    kpautoformobject->setSelected( true );

    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Autoform" ), kpautoformobject, this );
    insertCmd->execute();
    _commands.addCommand( insertCmd );

    setModified(true);
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
    QListIterator<KoView> it( views() );
    for( ; it.current(); ++it ) {
	// I am doing a cast to KPresenterView here, since some austrian hacker :-)
	// decided to overload the non virtual repaint method!
	((KPresenterView*)it.current())->repaint( erase );
    }
}

/*==============================================================*/
void KPresenterDoc::setUnit( KoUnit _unit, QString __unit )
{
    _pageLayout.unit = _unit;

    QListIterator<KoView> it( views() );
    for( ; it.current(); ++it ) {
	((KPresenterView*)it.current())->getHRuler()->setUnit( __unit );
	((KPresenterView*)it.current())->getVRuler()->setUnit( __unit );
    }
}

/*===================== repaint =================================*/
void KPresenterDoc::repaint( QRect rect )
{
    QRect r;

    QListIterator<KoView> it( views() );
    for( ; it.current(); ++it ) {
	r = rect;
	r.moveTopLeft( QPoint( r.x() - ((KPresenterView*)it.current())->getDiffX(),
			       r.y() - ((KPresenterView*)it.current())->getDiffY() ) );

	// I am doing a cast to KPresenterView here, since some austrian hacker :-)
	// decided to overload the non virtual repaint method!
	((KPresenterView*)it.current())->repaint( r, false );
    }
}

/*===================== repaint =================================*/
void KPresenterDoc::repaint( KPObject *kpobject )
{
    QRect r;

    QListIterator<KoView> it( views() );
    for( ; it.current(); ++it )
    {
	r = kpobject->getBoundingRect( 0, 0 );
	r.moveTopLeft( QPoint( r.x() - ((KPresenterView*)it.current())->getDiffX(),
			       r.y() - ((KPresenterView*)it.current())->getDiffY() ) );

	// I am doing a cast to KPresenterView here, since some austrian hacker :-)
	// decided to overload the non virtual repaint method!
	((KPresenterView*)it.current())->repaint( r, false );
    }
}

/*==================== reorder page =============================*/
QValueList<int> KPresenterDoc::reorderPage( unsigned int num, int diffx, int diffy, float fakt )
{
    QValueList<int> orderList;

    orderList.append( 0 );

    KPObject *kpobject = 0;

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
	kpobject = objectList()->at( i );
	if ( getPageOfObj( i, diffx, diffy, fakt ) == static_cast<int>( num ) ) {
	    if ( orderList.find( kpobject->getPresNum() ) == orderList.end() ) {
		if ( orderList.isEmpty() )
		    orderList.append( kpobject->getPresNum() );
		else {
		    QValueList<int>::Iterator it = orderList.begin();
		    for ( ; *it < kpobject->getPresNum() && it != orderList.end(); ++it );
		    orderList.insert( it, kpobject->getPresNum() );
		}
	    }
	    if ( kpobject->getDisappear() && orderList.find( kpobject->getDisappearNum() ) == orderList.end() ) {
		if ( orderList.isEmpty() )
		    orderList.append( kpobject->getDisappearNum() );
		else {
		    QValueList<int>::Iterator it = orderList.begin();
		    for ( ; *it < kpobject->getDisappearNum() && it != orderList.end(); ++it );
		    orderList.insert( it, kpobject->getDisappearNum() );
		}
	    }
	}
    }

    return orderList;
}

/*====================== get page of object ======================*/
int KPresenterDoc::getPageOfObj( int objNum, int diffx, int diffy, float fakt )
{
    QRect rect;
    int deskw = QApplication::desktop()->width();

    // Are diffx/diffy really necessary here ? It seems to me we would
    // get the same result with 0,0 instead, since this stuff surely
    // doesn't depend on the scrollbar positions, nor the current page (DF).
    KPObject * kpobject = objectList()->at( objNum );
    for ( int j = 0; j < static_cast<int>( _backgroundList.count() ); j++ ) {
        rect = getPageRect( j, diffx, diffy, fakt, false );
        rect.setWidth( deskw );
        if ( rect.intersects( kpobject->getBoundingRect( diffx, diffy ) ) ) {
            QRect r = rect.intersect( kpobject->getBoundingRect( diffx, diffy ) );
            if ( r.width() * r.height() > ( kpobject->getBoundingRect( diffx, diffy ).width() * kpobject->getBoundingRect( diffx, diffy ).height() ) / 4 )
                return j+1;
        }
    }
    return -1;
}

/*================== get size of page ===========================*/
QRect KPresenterDoc::getPageRect( unsigned int num, int diffx, int diffy, float fakt , bool decBorders )
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

    return QRect( -diffx + bl, -diffy + bt + num * ( bt + bb + ph ), pw, ph );
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
    kdDebug() << "KPresenterDoc::deletePage " << _page << endl;
    KPObject *kpobject = 0;
    int _h = getPageRect( 0, 0, 0 ).height();

    deSelectAllObj();
    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
	kpobject = objectList()->at( i );
	if ( getPageOfObj( i, 0, 0 ) - 1 == _page )
	    kpobject->setSelected( true );
    }
    deleteObjs( false );
    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
	kpobject = objectList()->at( i );
	if ( getPageOfObj( i, 0, 0 ) - 1 > _page )
	    kpobject->setOrig( kpobject->getOrig().x(), kpobject->getOrig().y() - _h );
    }

    for ( kpobject = objectList()->first(); kpobject; kpobject = objectList()->next() ) {
	if ( kpobject->getType() == OT_TEXT )
	    ( (KPTextObject*)kpobject )->recalcPageNum( this );
    }

    _backgroundList.remove( _page );
    repaint( false );

    ASSERT( _page < (int)m_selectedSlides.count() );
    m_selectedSlides.remove( m_selectedSlides.at( _page ) );
    // Update the sidebars
    QListIterator<KoView> it( views() );
    for (; it.current(); ++it )
        static_cast<KPresenterView*>(it.current())->updateSideBar();
}

/*================================================================*/
int KPresenterDoc::insertPage( int _page, InsertPos _insPos, bool chooseTemplate, const QString &theFile )
{
    kdDebug() << "KPresenterDoc::insertPage " << _page << endl;
    KPObject *kpobject = 0;
    int _h = getPageRect( 0, 0, 0 ).height();

    if ( _insPos == IP_BEFORE )
	_page--;

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
	kpobject = objectList()->at( i );
	if ( getPageOfObj( i, 0, 0 ) - 1 > _page )
	    kpobject->setOrig( kpobject->getOrig().x(), kpobject->getOrig().y() + _h );
    }

    if ( _insPos == IP_BEFORE )
	_page++;

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
    objStartY = getPageRect( _page - 1, 0, 0 ).y() + getPageRect( _page - 1, 0, 0 ).height();
    loadNativeFormat( fileName );
    objStartY = 0;
    _clean = true;
    setModified(true);
    KPBackGround *kpbackground = _backgroundList.at( _backgroundList.count() - 1 );
    _backgroundList.take( _backgroundList.count() - 1 );
    _backgroundList.insert( _page, kpbackground );
    if ( _page < (int)m_selectedSlides.count() )
        m_selectedSlides.insert( m_selectedSlides.at(_page), true );
    else
        m_selectedSlides.append( true );
    // Update the sidebars
    QListIterator<KoView> it( views() );
    for (; it.current(); ++it )
        static_cast<KPresenterView*>(it.current())->updateSideBar();
    return _page;
}

/*================ return number of selected objs ================*/
int KPresenterDoc::numSelected()
{
    int num = 0;

    KPObject *kpobject = 0;

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
	kpobject = objectList()->at( i );
	if ( kpobject->isSelected() ) num++;
    }

    return num;
}

/*==================== return selected obj ======================*/
KPObject* KPresenterDoc::getSelectedObj()
{
    KPObject *kpobject = 0;

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
	kpobject = objectList()->at( i );
	if ( kpobject->isSelected() ) return kpobject;
    }

    return 0;
}

/*======================= delete objects =========================*/
void KPresenterDoc::deleteObjs( bool _add )
{
    KPObject *kpobject = 0;
    QList<KPObject> _objects;
    _objects.setAutoDelete( false );

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
	kpobject = objectList()->at( i );
	if ( kpobject->isSelected() )
	    _objects.append( kpobject );
    }

    DeleteCmd *deleteCmd = new DeleteCmd( i18n( "Delete object( s )" ), _objects, this );
    deleteCmd->execute();

    if ( _add ) _commands.addCommand( deleteCmd );

    setModified(true);
}

/*========================== copy objects ========================*/
void KPresenterDoc::copyObjs( int diffx, int diffy )
{
    if ( !numSelected() )
        return;
    KPObject *kpobject = 0;

    QDomDocument doc("DOC");
    QDomElement presenter=doc.createElement("DOC");
    presenter.setAttribute("editor", "KPresenter");
    presenter.setAttribute("mime", "application/x-kpresenter-selection");
    doc.appendChild(presenter);
    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
        kpobject = objectList()->at( i );
        if ( kpobject->isSelected() ) {
            QDomElement object=doc.createElement("OBJECT");
            object.setAttribute("type", static_cast<int>( kpobject->getType() ));
            kpobject->moveBy( -diffx, -diffy );
            object.appendChild(kpobject->save( doc ));
            presenter.appendChild(object);
            kpobject->moveBy( diffx, diffy );
        }
    }

    QStoredDrag * drag = new QStoredDrag( "application/x-kpresenter-selection" );
    drag->setEncodedData( doc.toCString() );
    QApplication::clipboard()->setData( drag );
}

/*=============================================================*/
void KPresenterDoc::savePage( const QString &file, int pgnum )
{
    saveOnlyPage = pgnum;
    saveNativeFormat( file );
    saveOnlyPage = -1;
}

/*========================= paste objects ========================*/
void KPresenterDoc::pasteObjs( const QByteArray & data, int diffx, int diffy, int currPage )
{
    deSelectAllObj();

    pasting = true;
    pasteXOffset = diffx + 20;
    pasteYOffset = diffy + 20;
    QString clip_str = QString::fromUtf8( data );

    if ( clip_str.isEmpty() ) return;

    loadPastedObjs( clip_str, currPage );

    pasting = false;
    setModified(true);
}

/*====================== replace objects =========================*/
void KPresenterDoc::replaceObjs()
{
    KPObject *kpobject = 0;
    int ox, oy;
    QList<KPObject> _objects;
    QList<QPoint> _diffs;
    _objects.setAutoDelete( false );
    _diffs.setAutoDelete( false );

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
	kpobject = objectList()->at( i );
	ox = kpobject->getOrig().x();
	oy = kpobject->getOrig().y();

	ox = ( ox / _rastX ) * _rastX;
	oy = ( oy / _rastY ) * _rastY;

	_diffs.append( new QPoint( ox - kpobject->getOrig().x(), oy - kpobject->getOrig().y() ) );
	_objects.append( kpobject );
    }

    SetOptionsCmd *setOptionsCmd = new SetOptionsCmd( i18n( "Set new options" ), _diffs, _objects, _rastX, _rastY,
						      _orastX, _orastY, _txtBackCol, _otxtBackCol, this );
    _commands.addCommand( setOptionsCmd );
    setOptionsCmd->execute();

    setModified(true);
}

/*========================= restore background ==================*/
void KPresenterDoc::restoreBackground( int pageNum )
{
    if ( pageNum < static_cast<int>( _backgroundList.count() ) )
	backgroundList()->at( pageNum )->restore();
}

/*==================== load pasted objects ==============================*/
void KPresenterDoc::loadPastedObjs( const QString &in, int )
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

    loadObjects(document, true);

    repaint( false );
    setModified( true );
}

/*================= deselect all objs ===========================*/
void KPresenterDoc::deSelectAllObj()
{
    QListIterator<KoView> it( views() );
    for (; it.current(); ++it )
	((KPresenterView*)it.current())->getPage()->deSelectAllObj();
}

/*======================== align objects left ===================*/
void KPresenterDoc::alignObjsLeft()
{
    KPObject *kpobject = 0;
    QList<KPObject> _objects;
    QList<QPoint> _diffs;
    _objects.setAutoDelete( false );
    _diffs.setAutoDelete( false );
    int _x = getPageRect( 1, 0, 0 ).x();

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
	kpobject = objectList()->at( i );
	if ( kpobject->isSelected() ) {
	    _objects.append( kpobject );
	    _diffs.append( new QPoint( _x - kpobject->getOrig().x(), 0 ) );
	}
    }

    MoveByCmd2 *moveByCmd2 = new MoveByCmd2( i18n( "Align object( s ) left" ), _diffs, _objects, this );
    _commands.addCommand( moveByCmd2 );
    moveByCmd2->execute();
}

/*==================== align objects center h ===================*/
void KPresenterDoc::alignObjsCenterH()
{
    KPObject *kpobject = 0;
    QList<KPObject> _objects;
    QList<QPoint> _diffs;
    _objects.setAutoDelete( false );
    _diffs.setAutoDelete( false );
    int _x = getPageRect( 1, 0, 0 ).x();
    int _w = getPageRect( 1, 0, 0 ).width();

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
	kpobject = objectList()->at( i );
	if ( kpobject->isSelected() ) {
	    _objects.append( kpobject );
	    _diffs.append( new QPoint( ( _w - kpobject->getSize().width() ) / 2 - kpobject->getOrig().x() + _x, 0 ) );
	}
    }

    MoveByCmd2 *moveByCmd2 = new MoveByCmd2( i18n( "Align object( s ) centered ( horizontal )" ),
					     _diffs, _objects, this );
    _commands.addCommand( moveByCmd2 );
    moveByCmd2->execute();
}

/*==================== align objects right ======================*/
void KPresenterDoc::alignObjsRight()
{
    KPObject *kpobject = 0;
    QList<KPObject> _objects;
    QList<QPoint> _diffs;
    _objects.setAutoDelete( false );
    _diffs.setAutoDelete( false );
    int _w = getPageRect( 1, 0, 0 ).x() + getPageRect( 1, 0, 0 ).width();

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
	kpobject = objectList()->at( i );
	if ( kpobject->isSelected() ) {
	    _objects.append( kpobject );
	    _diffs.append( new QPoint( ( _w - kpobject->getSize().width() ) - kpobject->getOrig().x(), 0 ) );
	}
    }

    MoveByCmd2 *moveByCmd2 = new MoveByCmd2( i18n( "Align object( s ) right" ), _diffs, _objects, this );
    _commands.addCommand( moveByCmd2 );
    moveByCmd2->execute();
}

/*==================== align objects top ========================*/
void KPresenterDoc::alignObjsTop()
{
    KPObject *kpobject = 0;
    QList<KPObject> _objects;
    QList<QPoint> _diffs;
    _objects.setAutoDelete( false );
    _diffs.setAutoDelete( false );
    int pgnum, _y;

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
	kpobject = objectList()->at( i );
	if ( kpobject->isSelected() ) {
	    pgnum = getPageOfObj( i, 0, 0 );
	    if ( pgnum != -1 ) {
		_y = getPageRect( pgnum - 1, 0, 0 ).y();
		_objects.append( kpobject );
		_diffs.append( new QPoint( 0, _y - kpobject->getOrig().y() ) );
	    }
	}
    }

    MoveByCmd2 *moveByCmd2 = new MoveByCmd2( i18n( "Align object( s ) top" ), _diffs, _objects, this );
    _commands.addCommand( moveByCmd2 );
    moveByCmd2->execute();
}

/*==================== align objects center v ===================*/
void KPresenterDoc::alignObjsCenterV()
{
    KPObject *kpobject = 0;
    QList<KPObject> _objects;
    QList<QPoint> _diffs;
    _objects.setAutoDelete( false );
    _diffs.setAutoDelete( false );
    int pgnum, _y, _h;

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
	kpobject = objectList()->at( i );
	if ( kpobject->isSelected() ) {
	    pgnum = getPageOfObj( i, 0, 0 );
	    if ( pgnum != -1 ) {
		_y = getPageRect( pgnum - 1, 0, 0 ).y();
		_h = getPageRect( pgnum - 1, 0, 0 ).height();
		_objects.append( kpobject );
		_diffs.append( new QPoint( 0, ( _h - kpobject->getSize().height() ) / 2 -
					   kpobject->getOrig().y() + _y ) );
	    }
	}
    }

    MoveByCmd2 *moveByCmd2 = new MoveByCmd2( i18n( "Align object( s ) center / vertical" ), _diffs, _objects, this );
    _commands.addCommand( moveByCmd2 );
    moveByCmd2->execute();
}

/*==================== align objects top ========================*/
void KPresenterDoc::alignObjsBottom()
{
    KPObject *kpobject = 0;
    QList<KPObject> _objects;
    QList<QPoint> _diffs;
    _objects.setAutoDelete( false );
    _diffs.setAutoDelete( false );
    int pgnum, _h;

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
	kpobject = objectList()->at( i );
	if ( kpobject->isSelected() ) {
	    pgnum = getPageOfObj( i, 0, 0 );
	    if ( pgnum != -1 ) {
		_h = getPageRect( pgnum - 1, 0, 0 ).y() + getPageRect( pgnum - 1, 0, 0 ).height();
		_objects.append( kpobject );
		_diffs.append( new QPoint( 0, _h - kpobject->getSize().height() - kpobject->getOrig().y() ) );
	    }
	}
    }

    MoveByCmd2 *moveByCmd2 = new MoveByCmd2( i18n( "Align object( s ) bottom" ), _diffs, _objects, this );
    _commands.addCommand( moveByCmd2 );
    moveByCmd2->execute();
}

/*================= undo redo changed ===========================*/
void KPresenterDoc::slotUndoRedoChanged( QString _undo, QString _redo )
{
    QListIterator<KoView> it( views() );
    for (; it.current(); ++it )
    {
	((KPresenterView*)it.current())->changeUndo( _undo, !_undo.isEmpty() );
	((KPresenterView*)it.current())->changeRedo( _redo, !_redo.isEmpty() );
    }
    setModified(true);
}

/*==============================================================*/
int KPresenterDoc::getPenBrushFlags()
{
    int flags = 0;
    KPObject *kpobject = 0;

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
        kpobject = objectList()->at( i );
        if ( kpobject->isSelected() ) {
            switch ( kpobject->getType() ) {
                case OT_LINE:
                    flags = flags | StyleDia::SdPen;
                    break;
                case OT_PIE:
                    flags=flags | StyleDia::SdPen | StyleDia::SdBrush;
                    break;
                case OT_RECT: case OT_PART:  case OT_AUTOFORM: case OT_ELLIPSE:
                case OT_TEXT: case OT_PICTURE: case OT_CLIPART: {
                    flags = flags | StyleDia::SdPen;
                    flags = flags | StyleDia::SdBrush | StyleDia::SdGradient;
                } break;
                default: break;
            }
        }
    }

    if ( flags == 0 )
	flags = StyleDia::SdAll;
    return flags;
}

/*================================================================*/
QString KPresenterDoc::getPageTitle( unsigned int pgNum, const QString &_title, float fakt )
{
    QList<KPTextObject> objs;
    QRect rect = getPageRect( pgNum, 0, 0, fakt );

    KPObject *kpobject = 0L;
    KPTextObject *tmp = 0L;
    for ( kpobject = _objectList->first(); kpobject; kpobject = _objectList->next() )
        if ( kpobject->getType() == OT_TEXT && rect.contains( kpobject->getBoundingRect( 0, 0 ) ) ) {
            tmp=static_cast<KPTextObject*>( kpobject );
            if(tmp->getKTextObject()->lines() > 0)
                objs.append( tmp );
        }

    if ( objs.isEmpty() )
        return QString( _title );

    tmp = objs.first();
    KPTextObject *textobject=tmp;
    for ( tmp = objs.next(); tmp; tmp = objs.next() )
        if ( tmp->getOrig().y() < textobject->getOrig().y() )
            textobject = tmp;

    // this can't happen, but you never know :- )
    if ( !textobject )
        return QString( _title );

    QString txt = textobject->getKTextObject()->text().stripWhiteSpace();
    if ( txt.isEmpty() )
        return _title;
    unsigned int i=0;
    for( ; i<txt.length(), txt[i]=='\n'; ++i);
    int j=txt.find('\n', i);
    if(i==0 && j==-1)
        return txt;
    return txt.mid(i, j);
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

    KPObject *kpobject = 0L;
    int i = 0;
    for ( kpobject = _objectList->first(); kpobject; kpobject = _objectList->next(), ++i ) {
	if ( kpobject->getType() == OT_PICTURE ) {
	    if ( saveOnlyPage != -1 ) {
		int pg = getPageOfObj( i, 0, 0 ) - 1;
		if ( saveOnlyPage != pg )
		    continue;
	    }
	    usedPixmaps.append( dynamic_cast<KPPixmapObject*>( kpobject )->getKey() );
	}
    }

    KPBackGround *kpbackground = 0;
    i = 0;
    for ( kpbackground = _backgroundList.first(); kpbackground; kpbackground = _backgroundList.next(), ++i ) {
	if ( kpbackground->getBackType() == BT_PICTURE ) {
	    if ( saveOnlyPage != -1 && i != saveOnlyPage )
		continue;
	    usedPixmaps.append( kpbackground->getKey() );
	}
    }
}

/*================================================================*/
KoView* KPresenterDoc::createViewInstance( QWidget* parent, const char* name )
{
    return new KPresenterView( this, parent, name );
}

/*================================================================*/
void KPresenterDoc::paintContent( QPainter& painter, const QRect& rect, bool /*transparent*/, double /*zoomX*/, double /*zoomY*/ )
{
    unsigned int i = 0;
    QListIterator<KPBackGround> bIt( _backgroundList );
    for (; bIt.current(); ++bIt, i++ )
    {
        QRect r = getPageRect( i, 0, 0, 1.0, false );
        if ( rect.intersects( r ) )
            bIt.current()->draw( &painter, QPoint( r.x(), r.y() ), false );
    }


    QListIterator<KPObject> oIt( *_objectList );
    for (; oIt.current(); ++oIt )
        if ( rect.intersects( oIt.current()->getBoundingRect( 0, 0 ) ) )
            oIt.current()->draw( &painter, 0, 0 );

}

/*================================================================*/
void KPresenterDoc::groupObjects()
{
    QList<KPObject> objs;
    objs.setAutoDelete( false );
    KPObject *kpobject;
    for ( kpobject = _objectList->first(); kpobject; kpobject = _objectList->next() ) {
	if ( kpobject->isSelected() )
	    objs.append( kpobject );
    }

    if ( objs.count() < 2 )
	// Shouldn't the action be disabled, to prevent this? (David)
	KMessageBox::information( 0,  // TODO: provide a parent widget
				  i18n( "You have to select at least 2 objects\n"
					"which should be grouped together!"),
				  i18n( "Group Objects" ) );
    else {
	GroupObjCmd *groupObjCmd = new GroupObjCmd( i18n( "Group Objects" ), objs, this );
	_commands.addCommand( groupObjCmd );
	groupObjCmd->execute();
    }
}

/*================================================================*/
void KPresenterDoc::ungroupObjects()
{
    KPObject *kpobject = getSelectedObj();
    if ( kpobject && kpobject->getType() == OT_GROUP ) {
	UnGroupObjCmd *unGroupObjCmd = new UnGroupObjCmd( i18n( "Ungroup Objects" ),
							  (KPGroupObject*)kpobject, this );
	_commands.addCommand( unGroupObjCmd );
	unGroupObjCmd->execute();
    }
}

void KPresenterDoc::movePage( int from, int to )
{
    kdDebug() << "KPresenterDoc::movePage from=" << from << " to=" << to << endl;
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
    kdDebug() << "KPresenterDoc::copyPage from=" << from << " to=" << to << endl;
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
    kdDebug() << "KPresenterDoc::copyPageToClipboard pgnum=" << pgnum << endl;
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
        kdDebug() << "KPresenterDoc::clipboardDataChanged, deleting temp file " << m_tempFileInClipboard << endl;
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
    ASSERT( pgNum >= 0 );
    ASSERT( pgNum < (int)m_selectedSlides.count() );
    m_selectedSlides[ pgNum ] = select;
    kdDebug() << "KPresenterDoc::selectPage pgNum=" << pgNum << " select=" << select << endl;
    setModified(true);
    // Update the views
    QListIterator<KoView> it( views() );
    for (; it.current(); ++it )
        static_cast<KPresenterView*>(it.current())->updateSideBarItem( pgNum );
}

bool KPresenterDoc::isSlideSelected( int pgNum /* 0-based */ ) const
{
    ASSERT( pgNum >= 0 );
    ASSERT( pgNum < (int)m_selectedSlides.count() );
    return m_selectedSlides[ pgNum ];
}

QValueList<int> KPresenterDoc::selectedSlides() const /* returned list is 0-based */
{
    QValueList<int> result;
    QValueList<bool>::ConstIterator sit = m_selectedSlides.begin();
    for ( int i = 0; sit != m_selectedSlides.end(); ++sit, ++i )
        if ( *sit )
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

#include <kpresenter_doc.moc>
