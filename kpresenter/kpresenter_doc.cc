/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include "kpresenter_doc.h"
#include "kpresenter_doc.moc"
#include "kpresenter_view.h"
#include "kpresenter_shell.h"
#include "page.h"
#include "ktextedit.h"
#include "footer_header.h"
#include "kplineobject.h"
#include "kprectobject.h"
#include "kpellipseobject.h"
#include "kpautoformobject.h"
#include "kpclipartobject.h"
#include "kptextobject.h"
#include "kppixmapobject.h"
#include "kppieobject.h"
#include "kppartobject.h"
#include "kpgroupobject.h"
#include "movecmd.h"
#include "insertcmd.h"
#include "deletecmd.h"
#include "setoptionscmd.h"
#include "chgclipcmd.h"
#include "chgpixcmd.h"
#include "lowraicmd.h"
#include "pievaluecmd.h"
#include "rectvaluecmd.h"
#include "penbrushcmd.h"
#include "grpobjcmd.h"
#include "commandhistory.h"
#include "styledia.h"
#include "inspagedia.h"

#include <qrect.h>
#include <qpainter.h>
#include <qcolor.h>
#include <qevent.h>
#include <qpixmap.h>
#include <qwmatrix.h>
#include <qbrush.h>
#include <qpen.h>
#include <qpopupmenu.h>
#include <qcursor.h>
#include <qclipboard.h>
#include <qregexp.h>
#include <qfileinfo.h>
#include <qmap.h>
#include <qdatetime.h>
#include <qimage.h>
#include <qpicture.h>
#include <qbuffer.h>

#include <kurl.h>

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

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "KPresenterDocIface.h"
#include <iostream>
#include <cassert>
using namespace std;

#ifndef QT_NO_ASCII_CAST
#define QT_NO_ASCII_CAST
#endif
#ifndef QT_NO_CAST_ASCII
#define QT_NO_CAST_ASCII
#endif

static const int CURRENT_SYNTAX_VERSION = 2; // Reggie: make this 2 when committing the new text object

/******************************************************************/
/* class KPresenterChild                                          */
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

bool KPresenterChild::load( KOMLParser& parser, QValueList<KOMLAttrib>& _attribs )
{
  QValueList<KOMLAttrib>::ConstIterator it = _attribs.begin();
  for( ; it != _attribs.end(); ++it )
  {
    if ( (*it).m_strName == "url" )
    {
      m_tmpURL = (*it).m_strValue;
    }
    else if ( (*it).m_strName == "mime" )
    {
      m_tmpMimeType = (*it).m_strValue;
    }
    else
      kdDebug(30003) << "Unknown attrib 'OBJECT:" << (*it).m_strName << "'" << endl;
  }

  if ( m_tmpURL.isEmpty() )
  {
    kdDebug(30003) << "Empty 'url' attribute in OBJECT" << endl;
    return false;
  }
  else if ( m_tmpMimeType.isEmpty() )
  {
    kdDebug(30003) << "Empty 'mime' attribute in OBJECT" << endl;
    return false;
  }

  QString tag;
  QValueList<KOMLAttrib> lst;
  QString name;

  bool brect = false;

  // RECT
  while( parser.open( QString::null, tag ) )
  {
    parser.parseTag( tag, name, lst );

    if ( name == "RECT" )
    {
      brect = true;
      m_tmpGeometry = tagToRect( lst );
      setGeometry( m_tmpGeometry );
    }
    else
      kdDebug(30003) << "Unknown tag '" << tag << "' in OBJECT" << endl;

    if ( !parser.close( tag ) )
    {
      kdDebug(30003) << "ERR: Closing Child in OBJECT" << endl;
      return false;
    }
  }

  if ( !brect )
  {
    kdDebug(30003) << "Missing RECT in OBJECT" << endl;
    return false;
  }

  return true;
}


bool KPresenterChild::save( QTextStream& out )
{
  assert( document() );

  out << indent << "<OBJECT url=\"" << document()->url().url() << "\" mime=\""
      << document()->mimeType() << "\">"
      << geometry() << "</OBJECT>" << endl;

  return true;
}

/******************************************************************/
/* class KPresenterDoc                                            */
/******************************************************************/

/*====================== constructor =============================*/
KPresenterDoc::KPresenterDoc( QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name, bool singleViewMode )
    : KoDocument( parentWidget, widgetName, parent, name, singleViewMode ),
      _pixmapCollection(), _gradientCollection(), _clipartCollection(), _commands(), _hasHeader( false ),
      _hasFooter( false ), urlIntern()
{
    fCollection = new KTextEditFormatCollection;
    setInstance( KPresenterFactory::global() );

    dcop = 0;
    docAlreadyOpen = false;
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
    presSpeed = PS_NORMAL;
    pasting = false;
    pasteXOffset = pasteYOffset = 0;

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

    QWidget::connect( &_commands, SIGNAL( undoRedoChanged( QString, QString ) ),
                      this, SLOT( slotUndoRedoChanged( QString, QString ) ) );


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
//    sdeb( "KPresenterDoc::~KPresenterDoc()\n" );

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

/*======================== draw contents as QPicture =============*/
void KPresenterDoc::draw( QPaintDevice* /*_dev*/, long int /*_width*/, long int /*_height*/,
                          float /*_scale*/ )
{
    kdWarning() << "***********************************************"  << endl;
    kdWarning() << "KPresenter doesn't support KoDocument::draw( ... ) now!" << endl;
    kdWarning() <<  "***********************************************" << endl;
}

/*======================= make child list intern ================*/
bool KPresenterDoc::saveChildren( KoStore* _store, const QString &_path )
{
    int i = 0;

    QListIterator<KoDocumentChild> it( children() );
    for( ; it.current(); ++it ) {
        QString internURL = QString( "%1/%2" ).arg( _path ).arg( i++ );
        if ( !((KoDocumentChild*)(it.current()))->document()->saveToStore( _store, internURL ) )
          return false;
    }
    return true;
}

/*========================== save ===============================*/
bool KPresenterDoc::saveToStream(QIODevice * dev)
{
    QTextStream out( dev );
    out.setEncoding(QTextStream::UnicodeUTF8);
    KPObject *kpobject = 0L;

    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
    out << otag << "<DOC author=\"" << "Reginald Stadlbauer" << "\" email=\"" << "reggie@kde.org" << "\" editor=\""
        << "KPresenter"
        << "\" mime=\"" << "application/x-kpresenter"
        << "\" syntaxVersion=\"" << CURRENT_SYNTAX_VERSION << "\">"
        << endl;

    out << otag << "<PAPER format=\"" << static_cast<int>( _pageLayout.format ) << "\" ptWidth=\""
        << _pageLayout.ptWidth
        << "\" ptHeight=\"" << _pageLayout.ptHeight
        << "\" mmWidth =\"" << _pageLayout.mmWidth << "\" mmHeight=\"" << _pageLayout.mmHeight
        << "\" inchWidth =\"" << _pageLayout.inchWidth << "\" inchHeight=\"" << _pageLayout.inchHeight
        << "\" orientation=\"" << static_cast<int>( _pageLayout.orientation ) << "\" unit=\""
        << static_cast<int>( _pageLayout.unit ) << "\">" << endl;
    out << indent << "<PAPERBORDERS mmLeft=\"" << _pageLayout.mmLeft << "\" mmTop=\"" << _pageLayout.mmTop
        << "\" mmRight=\""
        << _pageLayout.mmRight << "\" mmBottom=\"" << _pageLayout.mmBottom
        << "\" ptLeft=\"" << _pageLayout.ptLeft << "\" ptTop=\"" << _pageLayout.ptTop << "\" ptRight=\""
        << _pageLayout.ptRight << "\" ptBottom=\"" << _pageLayout.ptBottom
        << "\" inchLeft=\"" << _pageLayout.inchLeft << "\" inchTop=\"" << _pageLayout.inchTop << "\" inchRight=\""
        << _pageLayout.inchRight << "\" inchBottom=\"" << _pageLayout.inchBottom << "\"/>" << endl;
    out << etag << "</PAPER>" << endl;

    out << otag << "<BACKGROUND" << " rastX=\"" << _rastX << "\" rastY=\"" << _rastY
        << "\" bred=\"" << _txtBackCol.red() << "\" bgreen=\"" << _txtBackCol.green() << "\" bblue=\""
        << _txtBackCol.blue() << "\">" << endl;
    saveBackground( out );
    out << etag << "</BACKGROUND>" << endl;

    out << otag << "<HEADER show=\"" << static_cast<int>( hasHeader() ) << "\">" << endl;
    _header->save( out );
    out << etag << "</HEADER>" << endl;

    out << otag << "<FOOTER show=\"" << static_cast<int>( hasFooter() ) << "\">" << endl;
    _footer->save( out );
    out << etag << "</FOOTER>" << endl;

    out << otag << "<OBJECTS>" << endl;
    saveObjects( out );
    out << etag << "</OBJECTS>" << endl;

    out << indent << "<INFINITLOOP value=\"" << _spInfinitLoop << "\"/>" << endl;
    out << indent << "<MANUALSWITCH value=\"" << _spManualSwitch << "\"/>" << endl;
    out << indent << "<PRESSPEED value=\"" << static_cast<int>( presSpeed ) << "\"/>" << endl;
    out << indent << "<PRESSLIDES value=\"" << static_cast<int>( presentSlides ) << "\"/>" << endl;

    out << otag << "<SELSLIDES>" << endl;
    QMap<int,bool>::Iterator sit = selectedSlides.begin();
    for ( ; sit != selectedSlides.end(); ++sit )
        out << indent << "<SLIDE nr=\"" << sit.key() << "\" show=\"" << ( *sit ) << "\"/>" << endl;
    out << etag << "</SELSLIDES>" << endl;

    // Write "OBJECT" tag for every child
    QListIterator<KoDocumentChild> chl( children() );
    for( ; chl.current(); ++chl ) {
        out << otag << "<EMBEDDED>" << endl;

        KPresenterChild* curr = (KPresenterChild*)chl.current();

        curr->save( out );

        out << otag << "<SETTINGS>" << endl;
        for ( unsigned int i = 0; i < _objectList->count(); i++ ) {
            kpobject = _objectList->at( i );
            if ( kpobject->getType() == OT_PART &&
                 dynamic_cast<KPPartObject*>( kpobject )->getChild() == curr )
                kpobject->save( out );
        }
        out << etag << "</SETTINGS> "<< endl;

        out << etag << "</EMBEDDED>" << endl;
    }

    makeUsedPixmapList();

    out << otag << "<PIXMAPS>" << endl;

    int i = 0;
    QMap< KPPixmapDataCollection::Key, QImage >::Iterator it = _pixmapCollection.getPixmapDataCollection().begin();

    for ( ; it != _pixmapCollection.getPixmapDataCollection().end(); ++it ) {
        if ( usedPixmaps.contains( it.key() ) ) {
            KPPixmapDataCollection::Key key = it.key();
            QString format = QFileInfo( it.key().filename ).extension().upper();
            if ( format == "JPG" )
                format = "JPEG";
            if ( QImage::outputFormats().find( format.latin1() ) == -1 )
                format = "BMP";
            QString pictureName = QString( "pictures/picture%1.%2" ).arg( ++i ).arg( format.lower() );
            if ( !isStoredExtern() )
              pictureName.prepend( url().url() + "/" );
            out << indent << "<KEY " << key << " name=\""
                << pictureName << "\" />" << endl;
        }
    }

    out << etag << "</PIXMAPS>" << endl;

    out << otag << "<CLIPARTS>" << endl;

    i = 0;
    QMap< KPClipartCollection::Key, QPicture >::Iterator it2 = _clipartCollection.begin();

    for( ; it2 != _clipartCollection.end(); ++it2 ) {
        KPClipartCollection::Key key = it2.key();
        QString clipartName = QString( "cliparts/clipart%1.wmf" ).arg( ++i );
        if ( !isStoredExtern() )
          clipartName.prepend( url().url() + "/" );
        out << indent << "<KEY " << key << " name=\""
            << clipartName << "\" />" << endl;
    }

    out << etag << "</CLIPARTS>" << endl;

    out << etag << "</DOC>" << endl;

    setModified( false );

    return true;
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
void KPresenterDoc::saveBackground( QTextStream& out )
{
    KPBackGround *kpbackground = 0;

    for ( int i = 0; i < static_cast<int>( _backgroundList.count() ); i++ ) {
        if ( saveOnlyPage != -1 &&
             i != saveOnlyPage )
            continue;
        kpbackground = _backgroundList.at( i );
        out << otag << "<PAGE>" << endl;
        kpbackground->save( out );
        out << etag << "</PAGE>" << endl;
    }
}

/*========================== save objects =======================*/
void KPresenterDoc::saveObjects( QTextStream& out )
{
    KPObject *kpobject = 0;

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
        if ( saveOnlyPage != -1 ) {
            int pg = getPageOfObj( i, 0, 0 ) - 1;
            if ( saveOnlyPage != pg )
                continue;
        }
        kpobject = objectList()->at( i );
        if ( kpobject->getType() == OT_PART ) continue;
        out << otag << "<OBJECT type=\"" << static_cast<int>( kpobject->getType() ) << "\">" << endl;
        QPoint orig = kpobject->getOrig();
        if ( saveOnlyPage != -1 )
            kpobject->moveBy( 0, -saveOnlyPage * getPageSize( 0, 0, 0 ).height() );
        kpobject->save( out );
        if ( saveOnlyPage != -1 )
            kpobject->setOrig( orig );
        out << etag << "</OBJECT>" << endl;
    }
}

/*==============================================================*/
bool KPresenterDoc::completeSaving( KoStore* _store )
{
    if ( !_store )
        return true;

    QMap< KPPixmapDataCollection::Key, QImage >::Iterator it = _pixmapCollection.getPixmapDataCollection().begin();

    int i = 0;
    for( ; it != _pixmapCollection.getPixmapDataCollection().end(); ++it ) {
        if ( _pixmapCollection.getPixmapDataCollection().references( it.key() ) > 0 &&
             !it.key().filename.isEmpty() && usedPixmaps.contains( it.key() ) ) {

            QString format = QFileInfo( it.key().filename ).extension().upper();
            if ( format == "JPG" )
                format = "JPEG";
            if ( QImage::outputFormats().find( format.latin1() ) == -1 )
                format = "BMP";

            QString u2 = QString( "pictures/picture%1.%2" ).arg( ++i ).arg( format.lower() );

            if ( !isStoredExtern() )
              u2.prepend( url().url() + "/" );

            if ( _store->open( u2 ) ) {
              KoStoreDevice dev( _store );
              QImageIO io;
              io.setIODevice( &dev );
              io.setImage( it.data() );
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
    QListIterator<KoDocumentChild> it( children() );
    for( ; it.current(); ++it ) {
        if ( !((KoDocumentChild*)it.current())->loadDocument( _store ) )
            return false;
    }

    return true;
}

bool KPresenterDoc::loadXML( QIODevice * dev, const QDomDocument& doc )
{
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
            KMessageBox::error(0L,"You don't appear to have perl installed.\nIt is needed to convert this document.\nInstall perl and try again.");
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
        KOMLParser parser( newdoc );
        return loadXML( parser );
    }
    else
    {
        KOMLParser parser( doc );
        return loadXML( parser );
    }
}

/*========================== load ===============================*/
bool KPresenterDoc::loadXML( KOMLParser & parser )
{
    QString tag;
    QValueList<KOMLAttrib> lst;
    QString name;

    pixmapCollectionKeys.clear();
    pixmapCollectionNames.clear();
    clipartCollectionKeys.clear();
    clipartCollectionNames.clear();
    lastObj = -1;

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
        presentSlides = PS_ALL;
    }

    if ( !docAlreadyOpen ) {
        // DOC
        if ( !parser.open( "DOC", tag ) ) {
            kdWarning() << "Missing DOC" << endl;
            return false;
        }

        parser.parseTag( tag, name, lst );
        QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
        for( ; it != lst.end(); ++it ) {
            if ( ( *it ).m_strName == "mime" ) {
                if ( ( *it ).m_strValue != "application/x-kpresenter" ) {
                    kdError() << "Unknown mime type " << ( *it ).m_strValue << endl;
                    return false;
                }
            } else if ( ( *it ).m_strName == "url" )
                urlIntern = KURL( ( *it ).m_strValue ).path();
        }
    }

    docAlreadyOpen = false;

    // PAPER
    while ( parser.open( QString::null, tag ) ) {
        parser.parseTag( tag, name, lst );

        if ( name == "EMBEDDED" ) {
            parser.parseTag( tag, name, lst );
            // huh? testing iterators? >;p (Werner)
            //QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            //for( ; it != lst.end(); ++it ) {
            //}
            KPresenterChild *ch = new KPresenterChild( this );
            KPPartObject *kppartobject = 0L;
            QRect r;

            while ( parser.open( QString::null, tag ) ) {
                parser.parseTag( tag, name, lst );
                if ( name == "OBJECT" ) {
                    ch->load( parser, lst );
                    r = ch->geometry();
                    insertChild( ch );
                    kppartobject = new KPPartObject( ch );
                    kppartobject->setOrig( r.x(), r.y() );
                    kppartobject->setSize( r.width(), r.height() );
                    _objectList->append( kppartobject );
                    //emit sig_insertObject( ch, kppartobject );
                } else if ( name == "SETTINGS" ) {
                    parser.parseTag( tag, name, lst );
                    // yeah - let's iterate a little bit - it's fun after all ;) (Werner)
                    //vector<KOMLAttrib>::const_iterator it = lst.begin();
                    //for( ; it != lst.end(); it++ ) {
                    //}
                    kppartobject->load( parser, lst );
                } else
                    kdError() << "Unknown tag '" << tag << "' in EMBEDDED" << endl;

                if ( !parser.close( tag ) ) {
                    kdError() << "ERR: Closing Child" << endl;
                    return false;
                }
            }
            if ( kppartobject ) {
                kppartobject->setOrig( r.x(), r.y() );
                kppartobject->setSize( r.width(), r.height() );
            }
        } else if ( name == "PAPER" ) {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "format" )
                    __pgLayout.format = ( KoFormat )( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "orientation" )
                    __pgLayout.orientation = ( KoOrientation )( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "width" ) {
                    __pgLayout.mmWidth = ( *it ).m_strValue.toDouble();
                    __pgLayout.ptWidth = MM_TO_POINT( __pgLayout.mmWidth );
                    __pgLayout.inchWidth = MM_TO_INCH( __pgLayout.mmWidth );
                } else if ( ( *it ).m_strName == "height" ) {
                    __pgLayout.mmHeight = ( *it ).m_strValue.toDouble();
                    __pgLayout.ptHeight = MM_TO_POINT( __pgLayout.mmHeight );
                    __pgLayout.inchHeight = MM_TO_INCH( __pgLayout.mmHeight );
                }
                else if ( ( *it ).m_strName == "ptWidth" )
                    __pgLayout.ptWidth = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "inchWidth" )
                    __pgLayout.inchWidth = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "mmWidth" )
                    __pgLayout.mmWidth = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "ptHeight" )
                    __pgLayout.ptHeight = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "inchHeight" )
                    __pgLayout.inchHeight = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "mmHeight" )
                    __pgLayout.mmHeight = ( *it ).m_strValue.toDouble();
                else if ( ( *it ).m_strName == "unit" )
                    __pgLayout.unit = static_cast<KoUnit>( ( *it ).m_strValue.toInt() );
                else
                    kdError() << "Unknown attrib PAPER:'" << ( *it ).m_strName << "'" << endl;
            }

            // PAPERBORDERS, HEAD, FOOT
            while ( parser.open( QString::null, tag ) ) {
                parser.parseTag( tag, name, lst );
                if ( name == "PAPERBORDERS" ) {
                    parser.parseTag( tag, name, lst );
                    QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
                    for( ; it != lst.end(); ++it ) {
                        if ( ( *it ).m_strName == "left" ) {
                            __pgLayout.mmLeft = ( *it ).m_strValue.toDouble();
                            __pgLayout.ptLeft = MM_TO_POINT( __pgLayout.mmLeft );
                            __pgLayout.inchLeft = MM_TO_INCH( __pgLayout.mmLeft );
                        } else if ( ( *it ).m_strName == "top" ) {
                            __pgLayout.mmTop = ( *it ).m_strValue.toDouble();
                            __pgLayout.ptTop = MM_TO_POINT( __pgLayout.mmTop );
                            __pgLayout.inchTop = MM_TO_INCH( __pgLayout.mmTop );
                        } else if ( ( *it ).m_strName == "right" ) {
                            __pgLayout.mmRight = ( *it ).m_strValue.toDouble();
                            __pgLayout.ptRight = MM_TO_POINT( __pgLayout.mmRight );
                            __pgLayout.inchRight = MM_TO_INCH( __pgLayout.mmRight );
                        } else if ( ( *it ).m_strName == "bottom" ) {
                            __pgLayout.mmBottom = ( *it ).m_strValue.toDouble();
                            __pgLayout.ptBottom = MM_TO_POINT( __pgLayout.mmBottom );
                            __pgLayout.inchBottom = MM_TO_INCH( __pgLayout.mmBottom );
                        }
                        else if ( ( *it ).m_strName == "ptLeft" )
                            __pgLayout.ptLeft = ( *it ).m_strValue.toDouble();
                        else if ( ( *it ).m_strName == "inchLeft" )
                            __pgLayout.inchLeft = ( *it ).m_strValue.toDouble();
                        else if ( ( *it ).m_strName == "mmLeft" )
                            __pgLayout.mmLeft = ( *it ).m_strValue.toDouble();
                        else if ( ( *it ).m_strName == "ptRight" )
                            __pgLayout.ptRight = ( *it ).m_strValue.toDouble();
                        else if ( ( *it ).m_strName == "inchRight" )
                            __pgLayout.inchRight = ( *it ).m_strValue.toDouble();
                        else if ( ( *it ).m_strName == "mmRight" )
                            __pgLayout.mmRight = ( *it ).m_strValue.toDouble();
                        else if ( ( *it ).m_strName == "ptTop" )
                            __pgLayout.ptTop = ( *it ).m_strValue.toDouble();
                        else if ( ( *it ).m_strName == "inchTop" )
                            __pgLayout.inchTop = ( *it ).m_strValue.toDouble();
                        else if ( ( *it ).m_strName == "mmTop" )
                            __pgLayout.mmTop = ( *it ).m_strValue.toDouble();
                        else if ( ( *it ).m_strName == "ptBottom" )
                            __pgLayout.ptBottom = ( *it ).m_strValue.toDouble();
                        else if ( ( *it ).m_strName == "inchBottom" )
                            __pgLayout.inchBottom = ( *it ).m_strValue.toDouble();
                        else if ( ( *it ).m_strName == "mmBottom" )
                            __pgLayout.mmBottom = ( *it ).m_strValue.toDouble();
                        else
                            kdError() << "Unknown attrib 'PAPERBORDERS:" << ( *it ).m_strName << "'" << endl;
                    }
                } else
                    kdError() << "Unknown tag '" << tag << "' in PAPER" << endl;

                if ( !parser.close( tag ) ) {
                    kdError() << "ERR: Closing Child" << endl;
                    return false;
                }
            }

        } else if ( name == "BACKGROUND" ) {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "rastX" )
                    _rastX = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "rastY" )
                    _rastY = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "xRnd" )
                    _xRnd = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "yRnd" )
                    _yRnd = ( *it ).m_strValue.toInt();
                else if ( ( *it ).m_strName == "bred" )
                    _txtBackCol.setRgb( ( *it ).m_strValue.toInt(),
                                        _txtBackCol.green(), _txtBackCol.blue() );
                else if ( ( *it ).m_strName == "bgreen" )
                    _txtBackCol.setRgb( _txtBackCol.red(), ( *it ).m_strValue.toInt(),
                                        _txtBackCol.blue() );
                else if ( ( *it ).m_strName == "bblue" )
                    _txtBackCol.setRgb( _txtBackCol.red(), _txtBackCol.green(),
                                        ( *it ).m_strValue.toInt() );
                else
                    kdError() << "Unknown attrib BACKGROUND:'" << ( *it ).m_strName << "'" << endl;
            }
            loadBackground( parser, lst );
        } else if ( name == "HEADER" ) {
            if ( _clean || !hasHeader() ) {
                parser.parseTag( tag, name, lst );
                QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
                for( ; it != lst.end(); ++it ) {
                    if ( ( *it ).m_strName == "show" ) {
                        setHeader( static_cast<bool>( ( *it ).m_strValue.toInt() ) );
                        headerFooterEdit->setShowHeader( hasHeader() );
                    }
                }
                _header->load( parser, lst );
            }
        } else if ( name == "FOOTER" ) {
            if ( _clean || !hasFooter() ) {
                parser.parseTag( tag, name, lst );
                QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
                for( ; it != lst.end(); ++it ) {
                    if ( ( *it ).m_strName == "show" ) {
                        setFooter( static_cast<bool>( ( *it ).m_strValue.toInt() ) );
                        headerFooterEdit->setShowFooter( hasFooter() );
                    }
                }
                _footer->load( parser, lst );
            }
        } else if ( name == "OBJECTS" ) {
            parser.parseTag( tag, name, lst );
            lastObj = _objectList->count() - 1;
            loadObjects( parser, lst );
        } else if ( name == "INFINITLOOP" ) {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "value" )
                    _spInfinitLoop = static_cast<bool>( ( *it ).m_strValue.toInt() );
            }
        } else if ( name == "PRESSPEED" ) {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "value" )
                    presSpeed = static_cast<PresSpeed>( ( *it ).m_strValue.toInt() );
            }
        } else if ( name == "MANUALSWITCH" ) {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "value" )
                    _spManualSwitch = static_cast<bool>( ( *it ).m_strValue.toInt() );
            }
        } else if ( name == "PRESSLIDES" ) {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "value" )
                    presentSlides = static_cast<PresentSlides>( ( *it ).m_strValue.toInt() );
            }
        } else if ( name == "SELSLIDES" ) {
            parser.parseTag( tag, name, lst );
            // I'm quite sure the iterator works, so I commented that out ;) (Werner)
            //QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            //for( ; it != lst.end(); ++it ) {
            //}

            while ( parser.open( QString::null, tag ) ) {
                int nr;
                bool show;

                parser.parseTag( tag, name, lst );
                if ( name == "SLIDE" ) {
                    parser.parseTag( tag, name, lst );
                    QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
                    for( ; it != lst.end(); ++it ) {
                        if ( ( *it ).m_strName == "nr" )
                            nr = ( *it ).m_strValue.toInt();
                        else if ( ( *it ).m_strName == "show" )
                            show = static_cast<bool>( ( *it ).m_strValue.toInt() );
                    }
                    selectedSlides.insert( nr, show );
                } else
                    kdError() << "Unknown tag '" << tag << "' in SELSLIDES" << endl;

                if ( !parser.close( tag ) ) {
                    kdError() << "ERR: Closing Child" << endl;
                    return false;
                }
            }
        } else if ( name == "PIXMAPS" ) {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
            }

            while ( parser.open( QString::null, tag ) ) {
                KPPixmapDataCollection::Key key;
                int year, month, day, hour, minute, second, msec;
                QString n;

                parser.parseTag( tag, name, lst );
                if ( name == "KEY" ) {
                    parser.parseTag( tag, name, lst );
                    QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
                    n = QString::null;
                    for( ; it != lst.end(); ++it ) {
                        if ( ( *it ).m_strName == "filename" )
                            key.filename = ( *it ).m_strValue;
                        else if ( ( *it ).m_strName == "year" )
                            year = ( *it ).m_strValue.toInt();
                        else if ( ( *it ).m_strName == "month" )
                            month = ( *it ).m_strValue.toInt();
                        else if ( ( *it ).m_strName == "day" )
                            day = ( *it ).m_strValue.toInt();
                        else if ( ( *it ).m_strName == "hour" )
                            hour = ( *it ).m_strValue.toInt();
                        else if ( ( *it ).m_strName == "minute" )
                            minute = ( *it ).m_strValue.toInt();
                        else if ( ( *it ).m_strName == "second" )
                            second = ( *it ).m_strValue.toInt();
                        else if ( ( *it ).m_strName == "msec" )
                            msec = ( *it ).m_strValue.toInt();
                        else if ( ( *it ).m_strName == "name" )
                            n = ( *it ).m_strValue;
                        else
                            kdError() << "Unknown attrib 'KEY: " << ( *it ).m_strName << "'" << endl;
                    }
                    key.lastModified.setDate( QDate( year, month, day ) );
                    key.lastModified.setTime( QTime( hour, minute, second, msec ) );

                    pixmapCollectionKeys.append( key );
                    pixmapCollectionNames.append( n );
                } else
                    kdError() << "Unknown tag '" << tag << "' in PIXMAPS" << endl;

                if ( !parser.close( tag ) ) {
                    kdError() << "ERR: Closing Child" << endl;
                    return false;
                }
            }
        } else if ( name == "CLIPARTS" ) {
            parser.parseTag( tag, name, lst );
            //QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            //for( ; it != lst.end(); ++it ) {
            //}

            while ( parser.open( QString::null, tag ) ) {
                KPClipartCollection::Key key;
                int year, month, day, hour, minute, second, msec;
                QString n;

                parser.parseTag( tag, name, lst );
                if ( name == "KEY" ) {
                    n = QString::null;
                    parser.parseTag( tag, name, lst );
                    QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
                    for( ; it != lst.end(); ++it )
                    {
                        if ( ( *it ).m_strName == "filename" )
                            key.filename = ( *it ).m_strValue;
                        else if ( ( *it ).m_strName == "year" )
                            year = ( *it ).m_strValue.toInt();
                        else if ( ( *it ).m_strName == "month" )
                            month = ( *it ).m_strValue.toInt();
                        else if ( ( *it ).m_strName == "day" )
                            day = ( *it ).m_strValue.toInt();
                        else if ( ( *it ).m_strName == "hour" )
                            hour = ( *it ).m_strValue.toInt();
                        else if ( ( *it ).m_strName == "minute" )
                            minute = ( *it ).m_strValue.toInt();
                        else if ( ( *it ).m_strName == "second" )
                            second = ( *it ).m_strValue.toInt();
                        else if ( ( *it ).m_strName == "msec" )
                            msec = ( *it ).m_strValue.toInt();
                        else if ( ( *it ).m_strName == "name" )
                            n = ( *it ).m_strValue;
                        else
                            kdError() << "Unknown attrib 'KEY: " << ( *it ).m_strName << "'" << endl;
                    }
                    key.lastModified.setDate( QDate( year, month, day ) );
                    key.lastModified.setTime( QTime( hour, minute, second, msec ) );

                    clipartCollectionKeys.append( key );
                    clipartCollectionNames.append( n );
                } else
                    kdError() << "Unknown tag '" << tag << "' in CLIPARTS" << endl;

                if ( !parser.close( tag ) ) {
                    kdError() << "ERR: Closing Child" << endl;
                    return false;
                }
            }
        } else
            kdError() << "Unknown tag '" << tag << "' in PRESENTATION" << endl;

        if ( !parser.close( tag ) ) {
            kdError() << "ERR: Closing Child" << endl;
            return false;
        }
    }

    if ( _rastX == 0 ) _rastX = 10;
    if ( _rastY == 0 ) _rastY = 10;

    addToRecentlyOpenedList( url().url() );

    return true;
}

/*====================== load background =========================*/
void KPresenterDoc::loadBackground( KOMLParser& parser, QValueList<KOMLAttrib>& lst )
{
    QString tag;
    QString name;

    while ( parser.open( QString::null, tag ) ) {
        parser.parseTag( tag, name, lst );

        // page
        if ( name == "PAGE" ) {
            insertNewPage( 0, 0, false );
            KPBackGround *kpbackground = _backgroundList.last();
            kpbackground->load( parser, lst );
            if ( !selectedSlides.contains( _backgroundList.count() - 1 ) )
                selectedSlides.insert( _backgroundList.count() - 1, true );
        } else
            kdWarning() << "Unknown tag '" << tag << "' in BACKGROUND" << endl;

        if ( !parser.close( tag ) ) {
            kdWarning() << "ERR: Closing Child" << endl;
            return;
        }
    }
}

/*========================= load objects =========================*/
void KPresenterDoc::loadObjects( KOMLParser& parser, QValueList<KOMLAttrib>& lst, bool _paste )
{
    QString tag;
    QString name;
    ObjType t = OT_LINE;

    while ( parser.open( QString::null, tag ) ) {
        parser.parseTag( tag, name, lst );

        // object
        if ( name == "OBJECT" ) {
            parser.parseTag( tag, name, lst );
            QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
            for( ; it != lst.end(); ++it ) {
                if ( ( *it ).m_strName == "type" )
                    t = ( ObjType )( *it ).m_strValue.toInt();
            }

            switch ( t ) {
            case OT_LINE: {
                KPLineObject *kplineobject = new KPLineObject();
                kplineobject->load( parser, lst );

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
                kprectobject->load( parser, lst );

                if ( _paste ) {
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Rectangle" ), kprectobject, this );
                    insertCmd->execute();
                    _commands.addCommand( insertCmd );
                } else
                    _objectList->append( kprectobject );
            } break;
            case OT_ELLIPSE: {
                KPEllipseObject *kpellipseobject = new KPEllipseObject();
                kpellipseobject->load( parser, lst );

                if ( _paste ) {
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Ellipse" ), kpellipseobject, this );
                    insertCmd->execute();
                    _commands.addCommand( insertCmd );
                } else
                    _objectList->append( kpellipseobject );
            } break;
            case OT_PIE: {
                KPPieObject *kppieobject = new KPPieObject();
                kppieobject->load( parser, lst );

                if ( _paste ) {
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Pie/Arc/Chors" ), kppieobject, this );
                    insertCmd->execute();
                    _commands.addCommand( insertCmd );
                } else
                    _objectList->append( kppieobject );
            } break;
            case OT_AUTOFORM: {
                KPAutoformObject *kpautoformobject = new KPAutoformObject();
                kpautoformobject->load( parser, lst );

                if ( _paste ) {
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Autoform" ), kpautoformobject, this );
                    insertCmd->execute();
                    _commands.addCommand( insertCmd );
                } else
                    _objectList->append( kpautoformobject );
            } break;
            case OT_CLIPART: {
                KPClipartObject *kpclipartobject = new KPClipartObject( &_clipartCollection );
                kpclipartobject->load( parser, lst );

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
                kptextobject->load( parser, lst );

                if ( _paste ) {
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Text" ), kptextobject, this );
                    insertCmd->execute();
                    _commands.addCommand( insertCmd );
                } else
                    _objectList->append( kptextobject );
            } break;
            case OT_PICTURE: {
                KPPixmapObject *kppixmapobject = new KPPixmapObject( &_pixmapCollection );
                kppixmapobject->load( parser, lst );

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
                kpgroupobject->load( parser, lst, this );

                if ( _paste ) {
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Group Object" ), kpgroupobject, this );
                    insertCmd->execute();
                    _commands.addCommand( insertCmd );
                } else
                    _objectList->append( kpgroupobject );
            } break;
            default: break;
            }

            if ( objStartY > 0 ) _objectList->last()->moveBy( 0, objStartY );
            if ( pasting ) {
                _objectList->last()->moveBy( pasteXOffset, pasteYOffset );
                _objectList->last()->setSelected( true );
            }
        } else
            kdWarning() << "Unknown tag '" << tag << "' in OBJECTS" << endl;

        if ( !parser.close( tag ) ) {
            kdWarning() << "ERR: Closing Child" << endl;
            return;
        }
    }
}

/*===================================================================*/
bool KPresenterDoc::completeLoading( KoStore* _store )
{
    if ( _store ) {
        QString str = urlIntern.isEmpty() ? url().path() : urlIntern;

        QValueListIterator<KPPixmapDataCollection::Key> it = pixmapCollectionKeys.begin();
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

            _pixmapCollection.getPixmapDataCollection().insertPixmapData( it.node->data, img );
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

        _pixmapCollection.setAllowChangeRef( true );
        _pixmapCollection.getPixmapDataCollection().setAllowChangeRef( true );

        if ( _clean )
            setPageLayout( __pgLayout, 0, 0 );
        else {
            QRect r = getPageSize( 0, 0, 0 );
            _backgroundList.last()->setSize( r.width(), r.height() );
            _backgroundList.last()->restore();
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

    _pixmapCollection.setAllowChangeRef( true );
    _pixmapCollection.getPixmapDataCollection().setAllowChangeRef( true );

    return true;
}

/*========================== output formats ======================*/
QStrList KPresenterDoc::outputFormats()
{
    return new QStrList();
}

/*========================== input formats =======================*/
QStrList KPresenterDoc::inputFormats()
{
    return new QStrList();
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

/*======================= change child geometry ==================*/
#if 0
  // David: this is unused
void KPresenterDoc::changeChildGeometry( KPresenterChild *_child, const QRect& _rect, int /*_diffx*/, int /*_diffy*/ )
{
    _child->setGeometry( _rect );

    emit sig_updateChildGeometry( _child );

    //    m_bModified = true;
    setModified(true);
}
#endif

/*======================= child iterator =========================*/
// ### Reggie: I think nobody needs this function any more
/* QListIterator<KPresenterChild> KPresenterDoc::childIterator()
{
    return QListIterator<KPresenterChild> ( m_lstChildren );
} */

/*===================== set page layout ==========================*/
void KPresenterDoc::setPageLayout( KoPageLayout pgLayout, int diffx, int diffy )
{
//     if ( _pageLayout == pgLayout )
//      return;

    _pageLayout = pgLayout;
    QRect r = getPageSize( 0, diffx, diffy );

    for ( int i = 0; i < static_cast<int>( _backgroundList.count() ); i++ ) {
        if ( QSize( r.width(), r.height() ) != _backgroundList.at( i )->getSize() ) {
            _backgroundList.at( i )->setSize( r.width(), r.height() );
            _backgroundList.at( i )->restore();
        }
    }


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

    KPBackGround *kpbackground = new KPBackGround( &_pixmapCollection, &_gradientCollection,
                                                   &_clipartCollection, this );
    _backgroundList.append( kpbackground );

    if ( _restore ) {
        QRect r = getPageSize( 0, diffx, diffy );
        _backgroundList.last()->setSize( r.width(), r.height() );
        _backgroundList.last()->restore();
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

    ret = KoTemplateChooseDia::choose(  KPresenterFactory::global(), _template,
                                        "application/x-kpresenter", "*.kpr",
                                        i18n("KPresenter"), KoTemplateChooseDia::Everything,
                                        "kpresenter_template" );

    if ( ret == KoTemplateChooseDia::Template ) {
        QFileInfo fileInfo( _template );
        QString fileName( fileInfo.dirPath( true ) + "/" + fileInfo.baseName() + ".kpt" );
        _clean = clean;
        objStartY = getPageSize( _backgroundList.count() - 1, 0, 0 ).y() + getPageSize( _backgroundList.count() - 1,
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
        setModified(true);
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
    QMap< KPPixmapDataCollection::Key, QImage >::Iterator it = _pixmapCollection.getPixmapDataCollection().begin();
    QDateTime dt;

    if ( !QFileInfo( backPix ).exists() ) {
        for ( ; it != _pixmapCollection.getPixmapDataCollection().end(); ++it ) {
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
                                 BCType gt, bool unbalanced, int xfactor, int yfactor )
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

    LowerRaiseCmd *lrCmd = new LowerRaiseCmd( i18n( "Lower Object( s )" ), _objectList, _new, this );
    lrCmd->execute();
    _commands.addCommand( lrCmd );

    setModified(true);
}

/*=================== insert a picture ==========================*/
void KPresenterDoc::insertPicture( QString filename, int diffx, int diffy, int _x , int _y )
{
    QMap< KPPixmapDataCollection::Key, QImage >::Iterator it = _pixmapCollection.getPixmapDataCollection().begin();
    QDateTime dt;

    if ( !QFileInfo( filename ).exists() ) {
        for( ; it != _pixmapCollection.getPixmapDataCollection().end(); ++it ) {
            if ( it.key().filename == filename ) {
                dt = it.key().lastModified;
                break;
            }
        }
    }

    KPPixmapObject *kppixmapobject = new KPPixmapObject( &_pixmapCollection, filename, dt );
    kppixmapobject->setOrig( ( ( diffx + _x ) / _rastX ) * _rastX, ( ( diffy + _y ) / _rastY ) * _rastY );
    kppixmapobject->setSelected( true );

    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert picture" ), kppixmapobject, this );
    insertCmd->execute();
    _commands.addCommand( insertCmd );

    QRect s = getPageSize( 0, 0, 0 );
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

    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert clipart" ), kpclipartobject, this );
    insertCmd->execute();
    _commands.addCommand( insertCmd );

    setModified(true);
}

/*======================= change picture ========================*/
void KPresenterDoc::changePicture( QString filename, int /*diffx*/, int /*diffy*/ )
{
    KPObject *kpobject = 0;

    QMap< KPPixmapDataCollection::Key, QImage >::Iterator it = _pixmapCollection.getPixmapDataCollection().begin();
    QDateTime dt;

    if ( !QFileInfo( filename ).exists() ) {
        for( ; it != _pixmapCollection.getPixmapDataCollection().end(); ++it ) {
            if ( it.key().filename == filename ) {
                dt = it.key().lastModified;
                break;
            }
        }
    }

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
        kpobject = objectList()->at( i );
        if ( kpobject->isSelected() && kpobject->getType() == OT_PICTURE ) {
            KPPixmapObject *pix = new KPPixmapObject( &_pixmapCollection, filename, QDateTime() );

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

    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert line" ), kplineobject, this );
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

    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert rectangle" ), kprectobject, this );
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

    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert ellipse" ), kpellipseobject, this );
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

    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert pie/arc/chord" ), kppieobject, this );
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

    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert text" ), kptextobject, this );
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

    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert autoform" ), kpautoformobject, this );
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
    KoView* view = firstView();
    for( ; view; view = nextView() ) {
        // I am doing a cast to KPresenterView here, since some austrian hacker :-)
        // decided to overload the non virtual repaint method!
        ((KPresenterView*)view)->repaint( erase );
    }
}

/*==============================================================*/
void KPresenterDoc::setUnit( KoUnit _unit, QString __unit )
{
    _pageLayout.unit = _unit;

    KoView* view = firstView();
    for( ; view; view = nextView() ) {
        ((KPresenterView*)view)->getHRuler()->setUnit( __unit );
        ((KPresenterView*)view)->getVRuler()->setUnit( __unit );
    }
}

/*===================== repaint =================================*/
void KPresenterDoc::repaint( QRect rect )
{
    QRect r;

    KoView* view = firstView();
    for( ; view; view = nextView() ) {
        r = rect;
        r.moveTopLeft( QPoint( r.x() - ((KPresenterView*)view)->getDiffX(),
                               r.y() - ((KPresenterView*)view)->getDiffY() ) );

        // I am doing a cast to KPresenterView here, since some austrian hacker :-)
        // decided to overload the non virtual repaint method!
        ((KPresenterView*)view)->repaint( r, false );
    }
}

/*===================== repaint =================================*/
void KPresenterDoc::repaint( KPObject *kpobject )
{
    QRect r;

    KoView* view = firstView();
    for( ; view; view = nextView() )
    {
        r = kpobject->getBoundingRect( 0, 0 );
        r.moveTopLeft( QPoint( r.x() - ((KPresenterView*)view)->getDiffX(),
                               r.y() - ((KPresenterView*)view)->getDiffY() ) );

        // I am doing a cast to KPresenterView here, since some austrian hacker :-)
        // decided to overload the non virtual repaint method!
        ((KPresenterView*)view)->repaint( r, false );
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

    setModified(true);
    return orderList;
}

/*====================== get page of object ======================*/
int KPresenterDoc::getPageOfObj( int objNum, int diffx, int diffy, float fakt )
{
    QRect rect;

    KPObject *kpobject = 0;

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
        kpobject = objectList()->at( i );
        if ( i == objNum ) {
            for ( int j = 0; j < static_cast<int>( _backgroundList.count() ); j++ ) {
                rect = getPageSize( j, diffx, diffy, fakt, false );
                rect.setWidth( QApplication::desktop()->width() );
                if ( rect.intersects( kpobject->getBoundingRect( diffx, diffy ) ) ) {
                    QRect r = rect.intersect( kpobject->getBoundingRect( diffx, diffy ) );
                    if ( r.width() * r.height() > ( kpobject->getBoundingRect( diffx, diffy ).width() * kpobject->getBoundingRect( diffx, diffy ).height() ) / 4 )
                        return j+1;
                }
            }
        }
    }
    return -1;
}

/*================== get size of page ===========================*/
QRect KPresenterDoc::getPageSize( unsigned int num, int diffx, int diffy, float fakt , bool decBorders )
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

    return QRect( -diffx + bl, -diffy + bt + num * bt + num * bb + num * ph, pw, ph );
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
void KPresenterDoc::deletePage( int _page, DelPageMode _delPageMode )
{
    KPObject *kpobject = 0;
    int _h = getPageSize( 0, 0, 0 ).height();

    if ( _delPageMode == DPM_DEL_OBJS || _delPageMode == DPM_DEL_MOVE_OBJS ) {
        deSelectAllObj();
        for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
            kpobject = objectList()->at( i );
            if ( getPageOfObj( i, 0, 0 ) - 1 == _page )
                kpobject->setSelected( true );
        }
        deleteObjs( false );
    }

    if ( _delPageMode == DPM_MOVE_OBJS || _delPageMode == DPM_DEL_MOVE_OBJS ) {
        for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
            kpobject = objectList()->at( i );
            if ( getPageOfObj( i, 0, 0 ) - 1 > _page )
                kpobject->setOrig( kpobject->getOrig().x(), kpobject->getOrig().y() - _h );
        }
    }

    for ( kpobject = objectList()->first(); kpobject; kpobject = objectList()->next() ) {
        if ( kpobject->getType() == OT_TEXT )
            ( (KPTextObject*)kpobject )->recalcPageNum( this );
    }

    _backgroundList.remove( _page );
    repaint( false );
}

/*================================================================*/
void KPresenterDoc::insertPage( int _page, InsPageMode _insPageMode, InsertPos _insPos )
{
    KPObject *kpobject = 0;
    int _h = getPageSize( 0, 0, 0 ).height();

    if ( _insPos == IP_BEFORE ) _page--;

    if ( _insPageMode == IPM_MOVE_OBJS ) {
        for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
            kpobject = objectList()->at( i );
            if ( getPageOfObj( i, 0, 0 ) - 1 > _page )
                kpobject->setOrig( kpobject->getOrig().x(), kpobject->getOrig().y() + _h );
        }
    }

    if ( _insPos == IP_BEFORE ) _page++;

    QString _template;

    if ( KoTemplateChooseDia::choose(  KPresenterFactory::global(), _template,
                                       "", QString::null, QString::null, KoTemplateChooseDia::OnlyTemplates,
                                       "kpresenter_template") != KoTemplateChooseDia::Cancel ) {
        QFileInfo fileInfo( _template );
        QString fileName( fileInfo.dirPath( true ) + "/" + fileInfo.baseName() + ".kpt" );
        _clean = false;

        if ( _insPos == IP_AFTER ) _page++;
        objStartY = getPageSize( _page - 1, 0, 0 ).y() + getPageSize( _page - 1, 0, 0 ).height();
        loadNativeFormat( fileName );
        objStartY = 0;
        _clean = true;
        setModified(true);
        KPBackGround *kpbackground = _backgroundList.at( _backgroundList.count() - 1 );
        _backgroundList.take( _backgroundList.count() - 1 );
        _backgroundList.insert( _page, kpbackground );
    }
    repaint( false );
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
    QClipboard *cb = QApplication::clipboard();
    QString clip_str;
    QTextStream out( &clip_str, IO_WriteOnly );
    KPObject *kpobject = 0;

    out << otag << "<DOC author=\"" << "Reginald Stadlbauer" << "\" email=\"" << "reggie@kde.org"
        << "\" editor=\"" << "KPresenter"
        << "\" mime=\"" << "application/x-kpresenter-selection" << "\">" << endl;
    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
        kpobject = objectList()->at( i );
        if ( kpobject->isSelected() ) {
            out << otag << "<OBJECT type=\"" << static_cast<int>( kpobject->getType() ) << "\">" << endl;
            kpobject->moveBy( -diffx, -diffy );
            kpobject->save( out );
            kpobject->moveBy( diffx, diffy );
            out << etag << "</OBJECT>" << endl;
        }
    }
    out << etag << "</DOC>" << endl;

    cb->setText( clip_str );
}

/*=============================================================*/
void KPresenterDoc::savePage( const QString &file, int pgnum )
{
    saveOnlyPage = pgnum;
    saveNativeFormat( file );
    saveOnlyPage = -1;
}

/*========================= paste objects ========================*/
void KPresenterDoc::pasteObjs( int diffx, int diffy, int currPage )
{
    deSelectAllObj();

    pasting = true;
    pasteXOffset = diffx + 20;
    pasteYOffset = diffy + 20;
    QString clip_str = QApplication::clipboard()->text();

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
void KPresenterDoc::loadPastedObjs( const QString &in, int currPage )
{
    QDomDocument doc;
    doc.setContent( in );
    KOMLParser parser( doc );

    QString tag;
    QValueList<KOMLAttrib> lst;
    QString name;

    // DOC
    if ( !parser.open( "DOC", tag ) ) {
        kdError() << "Missing DOC" << endl;
        return;
    }

    bool insertPage = false;
    bool ok = false;

    parser.parseTag( tag, name, lst );
    QValueList<KOMLAttrib>::ConstIterator it = lst.begin();
    for( ; it != lst.end(); ++it ) {
        if ( ( *it ).m_strName == "mime" ) {
            if ( ( *it ).m_strValue == "application/x-kpresenter-selection" ) {
                ok = true;
            } else if ( ( *it ).m_strValue == "application/x-kpresenter-page-selection" ) {
                ok = true;
                insertPage = true;
            }
        }
    }

    if ( !ok )
        return;

    if ( !insertPage )
        loadObjects( parser, lst, true );
    else {
        InsPageDia *dia = new InsPageDia( 0, 0, this, currPage );
        if ( dia->exec() == QDialog::Accepted ) {
            KPObject *kpobject = 0;
            int _h = getPageSize( 0, 0, 0 ).height();
            InsertPos _insPos = dia->getInsertPos();
            int _page = dia->getPageNum();
            InsPageMode _insPageMode = dia->getInsPageMode();

            if ( _insPos == IP_BEFORE )
                _page--;

            if ( _insPageMode == IPM_MOVE_OBJS ) {
                for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
                    kpobject = objectList()->at( i );
                    if ( getPageOfObj( i, 0, 0 ) - 1 > _page )
                        kpobject->setOrig( kpobject->getOrig().x(), kpobject->getOrig().y() + _h );
                }
            }

            if ( _insPos == IP_BEFORE )
                _page++;
            pasting = false;
            _clean = false;

            if ( _insPos == IP_AFTER )
                _page++;
            objStartY = getPageSize( _page - 1, 0, 0 ).y() + getPageSize( _page - 1, 0, 0 ).height();
            docAlreadyOpen = true;
            loadXML( parser );
            objStartY = 0;
            _clean = true;
            KPBackGround *kpbackground = _backgroundList.at( _backgroundList.count() - 1 );
            _backgroundList.take( _backgroundList.count() - 1 );
            _backgroundList.insert( _page, kpbackground );
        }
        delete dia;
    }

    repaint( false );
    setModified(true);
}

/*================= deselect all objs ===========================*/
void KPresenterDoc::deSelectAllObj()
{
    KoView* view = firstView();
    for( ; view; view = nextView() )
        ((KPresenterView*)view)->getPage()->deSelectAllObj();
}

/*======================== align objects left ===================*/
void KPresenterDoc::alignObjsLeft()
{
    KPObject *kpobject = 0;
    QList<KPObject> _objects;
    QList<QPoint> _diffs;
    _objects.setAutoDelete( false );
    _diffs.setAutoDelete( false );
    int _x = getPageSize( 1, 0, 0 ).x();

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
    int _x = getPageSize( 1, 0, 0 ).x();
    int _w = getPageSize( 1, 0, 0 ).width();

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
    int _w = getPageSize( 1, 0, 0 ).x() + getPageSize( 1, 0, 0 ).width();

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
                _y = getPageSize( pgnum - 1, 0, 0 ).y();
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
                _y = getPageSize( pgnum - 1, 0, 0 ).y();
                _h = getPageSize( pgnum - 1, 0, 0 ).height();
                _objects.append( kpobject );
                _diffs.append( new QPoint( 0, ( _h - kpobject->getSize().height() ) / 2 -
                                           kpobject->getOrig().y() + _y ) );
            }
        }
    }

    MoveByCmd2 *moveByCmd2 = new MoveByCmd2( i18n( "Align object( s ) top" ), _diffs, _objects, this );
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
                _h = getPageSize( pgnum - 1, 0, 0 ).y() + getPageSize( pgnum - 1, 0, 0 ).height();
                _objects.append( kpobject );
                _diffs.append( new QPoint( 0, _h - kpobject->getSize().height() - kpobject->getOrig().y() ) );
            }
        }
    }

    MoveByCmd2 *moveByCmd2 = new MoveByCmd2( i18n( "Align object( s ) top" ), _diffs, _objects, this );
    _commands.addCommand( moveByCmd2 );
    moveByCmd2->execute();
}

/*================= undo redo changed ===========================*/
void KPresenterDoc::slotUndoRedoChanged( QString _undo, QString _redo )
{
    KoView* view = firstView();
    for( ; view; view = nextView() )
    {
        ((KPresenterView*)view)->changeUndo( _undo, !_undo.isEmpty() );
        ((KPresenterView*)view)->changeRedo( _redo, !_redo.isEmpty() );
    }
}

/*==============================================================*/
void KPresenterDoc::slotDocumentLoaded()
{
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
            case OT_AUTOFORM: case OT_RECT: case OT_ELLIPSE: case OT_PIE: case OT_PART:
            case OT_TEXT: case OT_PICTURE: case OT_CLIPART: {
                flags = flags | StyleDia::SdPen;
                flags = flags | StyleDia::SdBrush;
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
    QList<KPObject> objs;
    objs.setAutoDelete( false );

    QRect rect = getPageSize( pgNum, 0, 0, fakt );

    KPObject *kpobject = 0L, *obj = 0L;
    for ( kpobject = _objectList->first(); kpobject; kpobject = _objectList->next() )
        if ( kpobject->getType() == OT_TEXT && rect.contains( kpobject->getBoundingRect( 0, 0 ) ) &&
             dynamic_cast<KPTextObject*>( kpobject )->getKTextObject()->lines() > 0 )
            objs.append( kpobject );

    if ( objs.isEmpty() )
        return QString( _title );

    obj = objs.first();

    kpobject = objs.first();
    for ( kpobject = objs.next(); kpobject; kpobject = objs.next() )
        if ( kpobject->getOrig().y() < obj->getOrig().y() )
            obj = kpobject;

    // this can't happen, but you never know :- )
    if ( !obj )
        return QString( _title );

    KTextEdit *txtObj = dynamic_cast<KPTextObject*>( obj )->getKTextObject();

    QString txt = txtObj->text();
    int i = txt.find( '\n' );
    if ( i == -1 )
        return txt;
    return txt.left( i );
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
QValueList<int> KPresenterDoc::getSlides( int currPgNum )
{
    QValueList<int> lst;
    switch ( presentSlides ) {
    case PS_ALL:
        for ( unsigned int i = 0;i < _backgroundList.count(); ++i )
            lst.append( i + 1 );
        break;
    case PS_CURRENT:
        lst.append( currPgNum );
        break;
    case PS_SELECTED:
        QMap<int,bool>::Iterator it = selectedSlides.begin();
        for ( ; it != selectedSlides.end(); ++it )
            if ( ( *it ) )
                lst.append( it.key() + 1 );
        break;
    }

    return lst;
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
KoMainWindow* KPresenterDoc::createShell()
{
    KoMainWindow* shell = new KPresenterShell;
    shell->show();

    return shell;
}

/*================================================================*/
KoView* KPresenterDoc::createViewInstance( QWidget* parent, const char* name )
{
    return new KPresenterView( this, parent, name );
}

/*================================================================*/
void KPresenterDoc::paintContent( QPainter& /*painter*/, const QRect& /*rect*/, bool /*transparent*/ )
{
    qDebug("------------------ ::paintContent still unimplemented ----------" );
}

/*================================================================*/
void KPresenterDoc::copyPage( int num )
{
    num--;
    if ( num < 0 || num >= (int)_backgroundList.count() )
        return;

    QClipboard *cb = QApplication::clipboard();
    QString clip_str;
    QTextStream out( &clip_str, IO_WriteOnly );
    KPObject *kpobject = 0;
    KPBackGround *kpbackground = _backgroundList.at( num );

    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
    out << otag << "<DOC author=\"" << "Reginald Stadlbauer" << "\" email=\"" << "reggie@kde.org"
        << "\" editor=\"" << "KPresenter"
        << "\" mime=\"" << "application/x-kpresenter-page-selection" << "\">" << endl;
    out << otag << "<BACKGROUND>" << endl;
    out << otag << "<PAGE>" << endl;
    kpbackground->save( out );
    out << etag << "</PAGE>" << endl;
    out << etag << "</BACKGROUND>" << endl;

    out << otag << "<OBJECTS>" << endl;
    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ ) {
        kpobject = objectList()->at( i );
        if ( getPageOfObj( i, 0, 0 ) == num + 1 ) {
            out << otag << "<OBJECT type=\"" << static_cast<int>( kpobject->getType() ) << "\">" << endl;
            int y = kpobject->getOrig().y();
            kpobject->setOrig( kpobject->getOrig().x(), kpobject->getOrig().y() -
                              kpbackground->getSize().height() * num );
            kpobject->save( out );
            kpobject->setOrig( kpobject->getOrig().x(), y );
            out << etag << "</OBJECT>" << endl;
        }
    }
    out << etag << "</OBJECTS>" << endl;
    out << etag << "</DOC>" << endl;

    cb->setText( clip_str );
}

/*================================================================*/
void KPresenterDoc::addToRecentlyOpenedList( const QString &file )
{
    if ( file.right( 3 ) == "kpt" )
        return;

    KConfig *config = KPresenterFactory::global()->config();
    config->setGroup( "Global" );
    QStringList lst = config->readListEntry( "recently opened" );
    if ( !lst.contains( file ) ) {
        lst << file;
        config->writeEntry( "recently opened", lst );
        config->sync();
    }
}

/*================================================================*/
QStringList KPresenterDoc::getRecentryOpenedList()
{
    KConfig *config = KPresenterFactory::global()->config();
    config->setGroup( "Global" );
    return config->readListEntry( "recently opened" );
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
                                  i18n( "KPresenter - Group Objects" ) );
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
        UnGroupObjCmd *unGroupObjCmd = new UnGroupObjCmd( i18n( "Ungroup Object" ),
                                                          (KPGroupObject*)kpobject, this );
        _commands.addCommand( unGroupObjCmd );
        unGroupObjCmd->execute();
    }
}
