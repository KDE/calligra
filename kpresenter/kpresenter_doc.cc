/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1998                   */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* KPresenter is under GNU GPL                                    */
/******************************************************************/
/* Module: KPresenter Document                                    */
/******************************************************************/

#include "kpresenter_doc.h"
#include "kpresenter_doc.moc"
#include "kpresenter_shell.h"
#include "kpresenter_view.h"
#include "page.h"
#include "kpresenter_view.h"
#include "ktextobject.h"
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
#include "commandhistory.h"
#include "styledia.h"

#include <qrect.h>
#include <qpainter.h>
#include <qcolor.h>
#include <qevent.h>
#include <qpixmap.h>
#include <qwmatrix.h>
#include <qbrush.h>
#include <qpen.h>
#include <qpopmenu.h>
#include <qcursor.h>
#include <qmsgbox.h>
#include <qclipbrd.h>
#include <qregexp.h>
#include <qfileinf.h>
#include <qmap.h>
#include <qdatetime.h>
#include <qimage.h>
#include <qpicture.h>
#include <qbuffer.h>

#include <kurl.h>

#include <klocale.h>
#include <kfiledialog.h>

#include <koView.h>
#include <koTemplateChooseDia.h>
#include <koRuler.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/******************************************************************/
/* class KPresenterChild                                          */
/******************************************************************/

/*====================== constructor =============================*/
KPresenterChild::KPresenterChild( KPresenterDoc *_kpr, const QRect& _rect, KOffice::Document_ptr _doc,
                                  int _diffx, int _diffy )
    : KoDocumentChild( _rect, _doc )
{
    m_pKPresenterDoc = _kpr;
    m_rDoc = KOffice::Document::_duplicate( _doc );
    setGeometry( QRect( _rect.left() + _diffx, _rect.top() + _diffy, _rect.width(), _rect.height() ) );
}

/*====================== constructor =============================*/
KPresenterChild::KPresenterChild( KPresenterDoc *_kpr ) :
    KoDocumentChild()
{
    m_pKPresenterDoc = _kpr;
}

/*====================== destructor ==============================*/
KPresenterChild::~KPresenterChild()
{
    m_rDoc = 0L;
}

/******************************************************************/
/* class KPresenterDoc                                            */
/******************************************************************/

/*====================== constructor =============================*/
KPresenterDoc::KPresenterDoc()
    : _pixmapCollection(), _gradientCollection(), _clipartCollection(), _commands(), _hasHeader( false ),
      _hasFooter( false ), urlIntern()
{
    ADD_INTERFACE( "IDL:KOffice/Print:1.0" )
        // Use CORBA mechanism for deleting views
        m_lstViews.setAutoDelete( false );
    m_lstChildren.setAutoDelete( true );

    m_bModified = false;

    // init
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

    _header = new KPTextObject;
    _header->setDrawEditRect( false );
    _footer = new KPTextObject;
    _footer->setDrawEditRect( false );
    _footer->setDrawEmpty( false );
    _header->setDrawEmpty( false );

    headerFooterEdit = new KPFooterHeaderEditor( this );
    headerFooterEdit->setCaption( i18n( "KPresenter - Header/Footer Editor" ) );
    headerFooterEdit->hide();

    QObject::connect( &_commands, SIGNAL( undoRedoChanged( QString, QString ) ), this, SLOT( slotUndoRedoChanged( QString, QString ) ) );
}

/*====================== destructor ==============================*/
KPresenterDoc::~KPresenterDoc()
{
    sdeb( "KPresenterDoc::~KPresenterDoc()\n" );

    headerFooterEdit->allowClose();
    delete headerFooterEdit;

    delete _header;
    delete _footer;

    _objectList->clear();
    delete _objectList;
    _backgroundList.clear();
    cleanUp();
    edeb( "...KPresenterDoc::~KPresenterDoc() %i\n", _refcnt() );
}

/*======================== draw contents as QPicture =============*/
void KPresenterDoc::draw( QPaintDevice* _dev, CORBA::Long _width, CORBA::Long _height,
                          CORBA::Float _scale )
{
    warning( "***********************************************" );
    warning( i18n( "KPresenter doesn't support KoDocument::draw( ... ) now!" ) );
    warning( "***********************************************" );

    return;

    if ( m_lstViews.count() > 0 )
    {
        QPainter painter;
        painter.begin( _dev );

        if ( _scale != 1.0 )
            painter.scale( _scale, _scale );

        m_lstViews.at( 0 )->getPage()->draw( QRect( 0, 0, _width, _height ), &painter );

        painter.end();
    }
}

/*======================= clean up ===============================*/
void KPresenterDoc::cleanUp()
{
    if ( m_bIsClean ) return;

    assert( m_lstViews.count() == 0 );

    m_lstChildren.clear();

    KoDocument::cleanUp();
}

/*========================== save ===============================*/
bool KPresenterDoc::hasToWriteMultipart()
{
//     QListIterator<KPresenterChild> it( m_lstChildren );
//     for( ; it.current(); ++it )
//     {
//         if ( !it.current()->isStoredExtern() )
//             return true;
//     }
//     return false;
    return true;
}

/*======================= make child list intern ================*/
void KPresenterDoc::makeChildListIntern( KOffice::Document_ptr _doc, const char *_path )
{
    int i = 0;

    QListIterator<KPresenterChild> it( m_lstChildren );
    for( ; it.current(); ++it )
    {
        QString tmp;
        tmp.sprintf( "/%i", i++ );
        QString path( _path );
        path += tmp.data();

        KOffice::Document_var doc = it.current()->document();
        doc->makeChildList( _doc, path );
    }
}

/*========================== save ===============================*/
bool KPresenterDoc::save(ostream& out,const char * /* format */)
{
    QApplication::setOverrideCursor( waitCursor );
    KPObject *kpobject = 0L;

    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
    out << otag << "<DOC author=\"" << "Reginald Stadlbauer" << "\" email=\"" << "reggie@kde.org" << "\" editor=\"" << "KPresenter"
        << "\" mime=\"" << "application/x-kpresenter" << "\" url=\"" << url() << "\">" << endl;

    out << otag << "<PAPER format=\"" << static_cast<int>( _pageLayout.format ) << "\" ptWidth=\"" << _pageLayout.ptWidth
        << "\" ptHeight=\"" << _pageLayout.ptHeight
        << "\" mmWidth =\"" << _pageLayout.mmWidth << "\" mmHeight=\"" << _pageLayout.mmHeight
        << "\" inchWidth =\"" << _pageLayout.inchWidth << "\" inchHeight=\"" << _pageLayout.inchHeight
        << "\" orientation=\"" << static_cast<int>( _pageLayout.orientation ) << "\" unit=\"" << static_cast<int>( _pageLayout.unit ) << "\">" << endl;
    out << indent << "<PAPERBORDERS mmLeft=\"" << _pageLayout.mmLeft << "\" mmTop=\"" << _pageLayout.mmTop << "\" mmRight=\""
        << _pageLayout.mmRight << "\" mmBottom=\"" << _pageLayout.mmBottom
        << "\" ptLeft=\"" << _pageLayout.ptLeft << "\" ptTop=\"" << _pageLayout.ptTop << "\" ptRight=\""
        << _pageLayout.ptRight << "\" ptBottom=\"" << _pageLayout.ptBottom
        << "\" inchLeft=\"" << _pageLayout.inchLeft << "\" inchTop=\"" << _pageLayout.inchTop << "\" inchRight=\""
        << _pageLayout.inchRight << "\" inchBottom=\"" << _pageLayout.inchBottom << "\"/>" << endl;
    out << etag << "</PAPER>" << endl;

    out << otag << "<BACKGROUND" << " rastX=\"" << _rastX << "\" rastY=\"" << _rastY
        << "\" bred=\"" << _txtBackCol.red() << "\" bgreen=\"" << _txtBackCol.green() << "\" bblue=\"" << _txtBackCol.blue() << "\">" << endl;
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
    QListIterator<KPresenterChild> chl( m_lstChildren );
    for( ; chl.current(); ++chl )
    {
        out << otag << "<EMBEDDED>" << endl;

        chl.current()->save( out );

        out << otag << "<SETTINGS>" << endl;
        for ( unsigned int i = 0; i < _objectList->count(); i++ )
        {
            kpobject = _objectList->at( i );
            if ( kpobject->getType() == OT_PART && dynamic_cast<KPPartObject*>( kpobject )->getChild() == chl.current() )
                kpobject->save( out );
        }
        out << etag << "</SETTINGS> "<< endl;

        out << etag << "</EMBEDDED>" << endl;
    }

    makeUsedPixmapList();

    out << otag << "<PIXMAPS>" << endl;

    QMap< KPPixmapDataCollection::Key, QImage >::Iterator it = _pixmapCollection.getPixmapDataCollection().begin();

    for ( ; it != _pixmapCollection.getPixmapDataCollection().end(); ++it )
    {
        if ( usedPixmaps.contains( it.key() ) )
        {
            KPPixmapDataCollection::Key key = it.key();
            out << indent << "<KEY " << key << " />" << endl;
        }
    }

    out << etag << "</PIXMAPS>" << endl;

    out << otag << "<CLIPARTS>" << endl;

    QMap< KPClipartCollection::Key, QPicture >::Iterator it2 = _clipartCollection.begin();

    for( ; it2 != _clipartCollection.end(); ++it2 )
    {
        KPClipartCollection::Key key = it2.key();
        out << indent << "<KEY " << key << " />" << endl;
    }

    out << etag << "</CLIPARTS>" << endl;

    out << etag << "</DOC>" << endl;

    setModified( false );
    QApplication::restoreOverrideCursor();

    return true;
}

/*===============================================================*/
void KPresenterDoc::enableEmbeddedParts( bool f )
{
    KPObject *kpobject = 0L;

    for ( unsigned int k = 0; k < _objectList->count(); k++ )
    {
        kpobject = _objectList->at( k );
        if ( kpobject->getType() == OT_PART )
            dynamic_cast<KPPartObject*>( kpobject )->enableDrawing( f );
    }
}

/*========================== save background ====================*/
void KPresenterDoc::saveBackground( ostream& out )
{
    KPBackGround *kpbackground = 0;

    for ( int i = 0; i < static_cast<int>( _backgroundList.count() ); i++ )
    {
        kpbackground = _backgroundList.at( i );
        out << otag << "<PAGE>" << endl;
        kpbackground->save( out );
        out << etag << "</PAGE>" << endl;
    }
}

/*========================== save objects =======================*/
void KPresenterDoc::saveObjects( ostream& out )
{
    KPObject *kpobject = 0;

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
    {
        kpobject = objectList()->at( i );
        if ( kpobject->getType() == OT_PART ) continue;
        out << otag << "<OBJECT type=\"" << static_cast<int>( kpobject->getType() ) << "\">" << endl;
        kpobject->save( out );
        out << etag << "</OBJECT>" << endl;
    }
}

/*==============================================================*/
bool KPresenterDoc::completeSaving( KOStore::Store_ptr _store )
{
    if ( !_store )
        return true;

    CORBA::String_var u = url();
    QMap< KPPixmapDataCollection::Key, QImage >::Iterator it = _pixmapCollection.getPixmapDataCollection().begin();

    for( ; it != _pixmapCollection.getPixmapDataCollection().end(); ++it )
    {
        if ( _pixmapCollection.getPixmapDataCollection().references( it.key() ) > 0 &&
             !it.key().filename.isEmpty() && usedPixmaps.contains( it.key() ) )
        {
            QString u2 = u.in();
            u2 += "/";
            u2 += it.key().toString();

            QString format = QFileInfo( it.key().filename ).extension().upper();
            if ( format == "JPG" )
                format = "JPEG";
            if ( QImage::outputFormats().find( format ) == -1 )
                format = "BMP";

            QString mime = "image/" + format.lower();
            _store->open( u2, mime.lower() );
            ostorestream out( _store );
            writeImageToStream( out, it.data(), format );
            out.flush();
            _store->close();
        }
    }

    QMap< KPClipartCollection::Key, QPicture >::Iterator it2 = _clipartCollection.begin();

    for( ; it2 != _clipartCollection.end(); ++it2 )
    {
        if ( _clipartCollection.references( it2.key() ) > 0 &&
             !it2.key().filename.isEmpty() )
        {
            QString u2 = u.in();
            u2 += "/";
            u2 += it2.key().toString();

            QString mime = "clipart/wmf";
            _store->open( u2, mime.lower() );
            ostorestream out( _store );
            out << it2.data();
            out.flush();
            _store->close();
        }
    }

    return true;
}

/*========================== load ===============================*/
bool KPresenterDoc::loadChildren( KOStore::Store_ptr _store )
{
    QListIterator<KPresenterChild> it( m_lstChildren );
    for( ; it.current(); ++it )
    {
        if ( !it.current()->loadDocument( _store, it.current()->mimeType() ) )
            return false;
    }

    return true;
}

/*========================= load a template =====================*/
bool KPresenterDoc::load_template( const QString &_url )
{
//     KURL u( _url );
//     if ( u.isMalformed() )
//         return false;

//     if ( !u.isLocalFile() )
//     {
//         cerr << "Can not open remote URL" << endl;
//         return false;
//     }

//     ifstream in( u.path() );
//     if ( !in )
//     {
//         cerr << "Could not open" << u.path().latin1() << endl;
//         return false;
//     }

//     KOMLStreamFeed feed( in );
//     KOMLParser parser( &feed );

//     if ( !loadXML( parser, 0L ) )
//         return false;

//     m_bModified = true;
//     in.close();
//     return true;
    return loadFromURL( _url, "application/x-kpresenter" );
}

/*========================== load ===============================*/
bool KPresenterDoc::loadXML( KOMLParser& parser, KOStore::Store_ptr _store )
{
    QApplication::setOverrideCursor( waitCursor );
    string tag;
    vector<KOMLAttrib> lst;
    string name;

    pixmapCollectionKeys.clear();
    clipartCollectionKeys.clear();

    //KoPageLayout __pgLayout;
    __pgLayout.unit = PG_MM;

    // clean
    if ( _clean )
    {
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
        urlIntern = url();
        presentSlides = PS_ALL;
    }

    // DOC
    if ( !parser.open( "DOC", tag ) )
    {
        cerr << "Missing DOC" << endl;
        QApplication::restoreOverrideCursor();
        return false;
    }

    KOMLParser::parseTag( tag.c_str(), name, lst );
    vector<KOMLAttrib>::const_iterator it = lst.begin();
    for( ; it != lst.end(); it++ )
    {
        if ( ( *it ).m_strName == "mime" )
        {
            if ( ( *it ).m_strValue != "application/x-kpresenter" )
            {
                cerr << "Unknown mime type " << ( *it ).m_strValue << endl;
                QApplication::restoreOverrideCursor();
                return false;
            }
        }
        else if ( ( *it ).m_strName == "url" )
            urlIntern = ( *it ).m_strValue.c_str();
    }

    // PAPER
    while ( parser.open( 0L, tag ) )
    {
        KOMLParser::parseTag( tag.c_str(), name, lst );

        if ( name == "EMBEDDED" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
            }
            KPresenterChild *ch = new KPresenterChild( this );
            KPPartObject *kppartobject = 0L;

            while ( parser.open( 0L, tag ) )
            {
                KOMLParser::parseTag( tag.c_str(), name, lst );
                if ( name == "OBJECT" )
                {
                    ch->load( parser, lst );
                    QRect r = ch->geometry();
                    insertChild( ch );
                    kppartobject = new KPPartObject( ch );
                    kppartobject->setOrig( r.x(), r.y() );
                    kppartobject->setSize( r.width(), r.height() );
                    _objectList->append( kppartobject );
                    emit sig_insertObject( ch, kppartobject );
                }
                else if ( name == "SETTINGS" )
                {
                    KOMLParser::parseTag( tag.c_str(), name, lst );
                    vector<KOMLAttrib>::const_iterator it = lst.begin();
                    for( ; it != lst.end(); it++ )
                    {
                    }
                    kppartobject->load( parser, lst );
                }
                else
                    cerr << "Unknown tag '" << tag << "' in EMBEDDED" << endl;

                if ( !parser.close( tag ) )
                {
                    cerr << "ERR: Closing Child" << endl;
                    QApplication::restoreOverrideCursor();
                    return false;
                }
            }
        }
        else if ( name == "PAPER" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "format" )
                    __pgLayout.format = ( KoFormat )atoi( ( *it ).m_strValue.c_str() );
                else if ( ( *it ).m_strName == "orientation" )
                    __pgLayout.orientation = ( KoOrientation )atoi( ( *it ).m_strValue.c_str() );
                else if ( ( *it ).m_strName == "width" )
                {
                    __pgLayout.width = __pgLayout.mmWidth = static_cast<double>( atof( ( *it ).m_strValue.c_str() ) );
                    __pgLayout.ptWidth = MM_TO_POINT( static_cast<double>( atof( ( *it ).m_strValue.c_str() ) ) );
                    __pgLayout.inchWidth = MM_TO_INCH( static_cast<double>( atof( ( *it ).m_strValue.c_str() ) ) );
                }
                else if ( ( *it ).m_strName == "height" )
                {
                    __pgLayout.height = __pgLayout.mmHeight = static_cast<double>( atof( ( *it ).m_strValue.c_str() ) );
                    __pgLayout.ptHeight = MM_TO_POINT( static_cast<double>( atof( ( *it ).m_strValue.c_str() ) ) );
                    __pgLayout.inchHeight = MM_TO_INCH( static_cast<double>( atof( ( *it ).m_strValue.c_str() ) ) );
                }
                else if ( ( *it ).m_strName == "ptWidth" )
                    __pgLayout.ptWidth = atoi( ( *it ).m_strValue.c_str() );
                else if ( ( *it ).m_strName == "inchWidth" )
                    __pgLayout.inchWidth = atof( ( *it ).m_strValue.c_str() );
                else if ( ( *it ).m_strName == "mmWidth" )
                    __pgLayout.mmWidth = __pgLayout.width = atof( ( *it ).m_strValue.c_str() );
                else if ( ( *it ).m_strName == "ptHeight" )
                    __pgLayout.ptHeight = atoi( ( *it ).m_strValue.c_str() );
                else if ( ( *it ).m_strName == "inchHeight" )
                    __pgLayout.inchHeight = atof( ( *it ).m_strValue.c_str() );
                else if ( ( *it ).m_strName == "mmHeight" )
                    __pgLayout.mmHeight = __pgLayout.height = atof( ( *it ).m_strValue.c_str() );
                else if ( ( *it ).m_strName == "unit" )
                    __pgLayout.unit = static_cast<KoUnit>( atoi( ( *it ).m_strValue.c_str() ) );
                else
                    cerr << "Unknown attrib PAPER:'" << ( *it ).m_strName << "'" << endl;
            }

            // PAPERBORDERS, HEAD, FOOT
            while ( parser.open( 0L, tag ) )
            {
                KOMLParser::parseTag( tag.c_str(), name, lst );
                if ( name == "PAPERBORDERS" )
                {
                    KOMLParser::parseTag( tag.c_str(), name, lst );
                    vector<KOMLAttrib>::const_iterator it = lst.begin();
                    for( ; it != lst.end(); it++ )
                    {
                        if ( ( *it ).m_strName == "left" )
                        {
                            __pgLayout.left = __pgLayout.mmLeft = ( double )atof( ( *it ).m_strValue.c_str() );
                            __pgLayout.ptLeft = MM_TO_POINT( ( double )atof( ( *it ).m_strValue.c_str() ) );
                            __pgLayout.inchLeft = MM_TO_INCH( ( double )atof( ( *it ).m_strValue.c_str() ) );
                        }
                        else if ( ( *it ).m_strName == "top" )
                        {
                            __pgLayout.top = __pgLayout.mmTop = ( double )atof( ( *it ).m_strValue.c_str() );
                            __pgLayout.ptTop = MM_TO_POINT( ( double )atof( ( *it ).m_strValue.c_str() ) );
                            __pgLayout.inchTop = MM_TO_INCH( ( double )atof( ( *it ).m_strValue.c_str() ) );
                        }
                        else if ( ( *it ).m_strName == "right" )
                        {
                            __pgLayout.right = __pgLayout.mmRight = ( double )atof( ( *it ).m_strValue.c_str() );
                            __pgLayout.ptRight = MM_TO_POINT( ( double )atof( ( *it ).m_strValue.c_str() ) );
                            __pgLayout.inchRight = MM_TO_INCH( ( double )atof( ( *it ).m_strValue.c_str() ) );
                        }
                        else if ( ( *it ).m_strName == "bottom" )
                        {
                            __pgLayout.bottom = __pgLayout.mmBottom = ( double )atof( ( *it ).m_strValue.c_str() );
                            __pgLayout.ptBottom = MM_TO_POINT( ( double )atof( ( *it ).m_strValue.c_str() ) );
                            __pgLayout.inchBottom = MM_TO_INCH( ( double )atof( ( *it ).m_strValue.c_str() ) );
                        }
                        else if ( ( *it ).m_strName == "ptLeft" )
                            __pgLayout.ptLeft = atoi( ( *it ).m_strValue.c_str() );
                        else if ( ( *it ).m_strName == "inchLeft" )
                            __pgLayout.inchLeft = atof( ( *it ).m_strValue.c_str() );
                        else if ( ( *it ).m_strName == "mmLeft" )
                            __pgLayout.mmLeft = __pgLayout.left = atof( ( *it ).m_strValue.c_str() );
                        else if ( ( *it ).m_strName == "ptRight" )
                            __pgLayout.ptRight = atoi( ( *it ).m_strValue.c_str() );
                        else if ( ( *it ).m_strName == "inchRight" )
                            __pgLayout.inchRight = atof( ( *it ).m_strValue.c_str() );
                        else if ( ( *it ).m_strName == "mmRight" )
                            __pgLayout.mmRight = __pgLayout.right = atof( ( *it ).m_strValue.c_str() );
                        else if ( ( *it ).m_strName == "ptTop" )
                            __pgLayout.ptTop = atoi( ( *it ).m_strValue.c_str() );
                        else if ( ( *it ).m_strName == "inchTop" )
                            __pgLayout.inchTop = atof( ( *it ).m_strValue.c_str() );
                        else if ( ( *it ).m_strName == "mmTop" )
                            __pgLayout.mmTop = __pgLayout.top = atof( ( *it ).m_strValue.c_str() );
                        else if ( ( *it ).m_strName == "ptBottom" )
                            __pgLayout.ptBottom = atoi( ( *it ).m_strValue.c_str() );
                        else if ( ( *it ).m_strName == "inchBottom" )
                            __pgLayout.inchBottom = atof( ( *it ).m_strValue.c_str() );
                        else if ( ( *it ).m_strName == "mmBottom" )
                            __pgLayout.mmBottom = __pgLayout.bottom = atof( ( *it ).m_strValue.c_str() );
                        else
                            cerr << "Unknown attrib 'PAPERBORDERS:" << ( *it ).m_strName << "'" << endl;
                    }
                }
                else
                    cerr << "Unknown tag '" << tag << "' in PAPER" << endl;

                if ( !parser.close( tag ) )
                {
                    cerr << "ERR: Closing Child" << endl;
                    QApplication::restoreOverrideCursor();
                    return false;
                }
            }

        }

        else if ( name == "BACKGROUND" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "rastX" )
                    _rastX = atoi( ( *it ).m_strValue.c_str() );
                else if ( ( *it ).m_strName == "rastY" )
                    _rastY = atoi( ( *it ).m_strValue.c_str() );
                else if ( ( *it ).m_strName == "xRnd" )
                    _xRnd = atoi( ( *it ).m_strValue.c_str() );
                else if ( ( *it ).m_strName == "yRnd" )
                    _yRnd = atoi( ( *it ).m_strValue.c_str() );
                else if ( ( *it ).m_strName == "bred" )
                    _txtBackCol.setRgb( atoi( ( *it ).m_strValue.c_str() ),
                                        _txtBackCol.green(), _txtBackCol.blue() );
                else if ( ( *it ).m_strName == "bgreen" )
                    _txtBackCol.setRgb( _txtBackCol.red(), atoi( ( *it ).m_strValue.c_str() ),
                                        _txtBackCol.blue() );
                else if ( ( *it ).m_strName == "bblue" )
                    _txtBackCol.setRgb( _txtBackCol.red(), _txtBackCol.green(),
                                        atoi( ( *it ).m_strValue.c_str() ) );
                else
                    cerr << "Unknown attrib BACKGROUND:'" << ( *it ).m_strName << "'" << endl;
            }
            loadBackground( parser, lst );
        }

        else if ( name == "HEADER" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "show" )
                    setHeader( static_cast<bool>( atoi( ( *it ).m_strValue.c_str() ) ) );
            }
            _header->load( parser, lst );
        }

        else if ( name == "FOOTER" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "show" )
                    setFooter( static_cast<bool>( atoi( ( *it ).m_strValue.c_str() ) ) );
            }
            _footer->load( parser, lst );
        }

        else if ( name == "OBJECTS" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
            }
            loadObjects( parser, lst );
        }

        else if ( name == "INFINITLOOP" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "value" )
                    _spInfinitLoop = static_cast<bool>( atoi( ( *it ).m_strValue.c_str() ) );
            }
        }

        else if ( name == "PRESSPEED" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "value" )
                    presSpeed = static_cast<PresSpeed>( atoi( ( *it ).m_strValue.c_str() ) );
            }
        }

        else if ( name == "MANUALSWITCH" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "value" )
                    _spManualSwitch = static_cast<bool>( atoi( ( *it ).m_strValue.c_str() ) );
            }
        }

        else if ( name == "PRESSLIDES" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "value" )
                    presentSlides = static_cast<PresentSlides>( atoi( ( *it ).m_strValue.c_str() ) );
            }
        }

        else if ( name == "SELSLIDES" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
            }

            while ( parser.open( 0L, tag ) )
            {
                int nr;
                bool show;

                KOMLParser::parseTag( tag.c_str(), name, lst );
                if ( name == "SLIDE" )
                {
                    KOMLParser::parseTag( tag.c_str(), name, lst );
                    vector<KOMLAttrib>::const_iterator it = lst.begin();
                    for( ; it != lst.end(); it++ )
                    {
                        if ( ( *it ).m_strName == "nr" )
                            nr = atoi( ( *it ).m_strValue.c_str() );
                        else if ( ( *it ).m_strName == "show" )
                            show = static_cast<bool>( atoi( ( *it ).m_strValue.c_str() ) );
                    }
                    selectedSlides.insert( nr, show );
                }
                else
                    cerr << "Unknown tag '" << tag << "' in SELSLIDES" << endl;

                if ( !parser.close( tag ) )
                {
                    cerr << "ERR: Closing Child" << endl;
                    QApplication::restoreOverrideCursor();
                    return false;
                }
            }
        }

        else if ( name == "PIXMAPS" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
            }

            while ( parser.open( 0L, tag ) )
            {
                KPPixmapDataCollection::Key key;
                int year, month, day, hour, minute, second, msec;

                KOMLParser::parseTag( tag.c_str(), name, lst );
                if ( name == "KEY" )
                {
                    KOMLParser::parseTag( tag.c_str(), name, lst );
                    vector<KOMLAttrib>::const_iterator it = lst.begin();
                    for( ; it != lst.end(); it++ )
                    {
                        if ( ( *it ).m_strName == "filename" )
                            key.filename = ( *it ).m_strValue.c_str();
                        else if ( ( *it ).m_strName == "year" )
                            year = atoi( ( *it ).m_strValue.c_str() );
                        else if ( ( *it ).m_strName == "month" )
                            month = atoi( ( *it ).m_strValue.c_str() );
                        else if ( ( *it ).m_strName == "day" )
                            day = atoi( ( *it ).m_strValue.c_str() );
                        else if ( ( *it ).m_strName == "hour" )
                            hour = atoi( ( *it ).m_strValue.c_str() );
                        else if ( ( *it ).m_strName == "minute" )
                            minute = atoi( ( *it ).m_strValue.c_str() );
                        else if ( ( *it ).m_strName == "second" )
                            second = atoi( ( *it ).m_strValue.c_str() );
                        else if ( ( *it ).m_strName == "msec" )
                            msec = atoi( ( *it ).m_strValue.c_str() );
                        else
                            cerr << "Unknown attrib 'KEY: " << ( *it ).m_strName << "'" << endl;
                    }
                    key.lastModified.setDate( QDate( year, month, day ) );
                    key.lastModified.setTime( QTime( hour, minute, second, msec ) );

                    pixmapCollectionKeys.append( key );
                }
                else
                    cerr << "Unknown tag '" << tag << "' in PIXMAPS" << endl;

                if ( !parser.close( tag ) )
                {
                    cerr << "ERR: Closing Child" << endl;
                    QApplication::restoreOverrideCursor();
                    return false;
                }
            }
        }

        else if ( name == "CLIPARTS" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
            }

            while ( parser.open( 0L, tag ) )
            {
                KPClipartCollection::Key key;
                int year, month, day, hour, minute, second, msec;

                KOMLParser::parseTag( tag.c_str(), name, lst );
                if ( name == "KEY" )
                {
                    KOMLParser::parseTag( tag.c_str(), name, lst );
                    vector<KOMLAttrib>::const_iterator it = lst.begin();
                    for( ; it != lst.end(); it++ )
                    {
                        if ( ( *it ).m_strName == "filename" )
                            key.filename = ( *it ).m_strValue.c_str();
                        else if ( ( *it ).m_strName == "year" )
                            year = atoi( ( *it ).m_strValue.c_str() );
                        else if ( ( *it ).m_strName == "month" )
                            month = atoi( ( *it ).m_strValue.c_str() );
                        else if ( ( *it ).m_strName == "day" )
                            day = atoi( ( *it ).m_strValue.c_str() );
                        else if ( ( *it ).m_strName == "hour" )
                            hour = atoi( ( *it ).m_strValue.c_str() );
                        else if ( ( *it ).m_strName == "minute" )
                            minute = atoi( ( *it ).m_strValue.c_str() );
                        else if ( ( *it ).m_strName == "second" )
                            second = atoi( ( *it ).m_strValue.c_str() );
                        else if ( ( *it ).m_strName == "msec" )
                            msec = atoi( ( *it ).m_strValue.c_str() );
                        else
                            cerr << "Unknown attrib 'KEY: " << ( *it ).m_strName << "'" << endl;
                    }
                    key.lastModified.setDate( QDate( year, month, day ) );
                    key.lastModified.setTime( QTime( hour, minute, second, msec ) );

                    clipartCollectionKeys.append( key );
                }
                else
                    cerr << "Unknown tag '" << tag << "' in CLIPARTS" << endl;

                if ( !parser.close( tag ) )
                {
                    cerr << "ERR: Closing Child" << endl;
                    QApplication::restoreOverrideCursor();
                    return false;
                }
            }
        }

        else
            cerr << "Unknown tag '" << tag << "' in PRESENTATION" << endl;

        if ( !parser.close( tag ) )
        {
            cerr << "ERR: Closing Child" << endl;
            QApplication::restoreOverrideCursor();
            return false;
        }
    }

    if ( _rastX == 0 ) _rastX = 10;
    if ( _rastY == 0 ) _rastY = 10;

    QApplication::restoreOverrideCursor();

    if ( !_store )
    {
        setPageLayout( __pgLayout, 0, 0 );

        KPObject *kpobject = 0L;
        for ( kpobject = _objectList->first(); kpobject; kpobject = _objectList->next() )
        {
            if ( kpobject->getType() == OT_PICTURE )
                dynamic_cast<KPPixmapObject*>( kpobject )->reload();
        }

        _pixmapCollection.setAllowChangeRef( true );
        _pixmapCollection.getPixmapDataCollection().setAllowChangeRef( true );
    }

    return true;
}

/*====================== load background =========================*/
void KPresenterDoc::loadBackground( KOMLParser& parser, vector<KOMLAttrib>& lst )
{
    string tag;
    string name;

    while ( parser.open( 0L, tag ) )
    {
        KOMLParser::parseTag( tag.c_str(), name, lst );

        // page
        if ( name == "PAGE" )
        {
            insertNewPage( 0, 0, false );
            KPBackGround *kpbackground = _backgroundList.last();
            kpbackground->load( parser, lst );
            if ( !selectedSlides.contains( _backgroundList.count() - 1 ) )
                selectedSlides.insert( _backgroundList.count() - 1, true );
        }
        else
            cerr << "Unknown tag '" << tag << "' in BACKGROUND" << endl;

        if ( !parser.close( tag ) )
        {
            cerr << "ERR: Closing Child" << endl;
            return;
        }
    }
}

/*========================= load objects =========================*/
void KPresenterDoc::loadObjects( KOMLParser& parser, vector<KOMLAttrib>& lst, bool _paste )
{
    string tag;
    string name;
    ObjType t = OT_LINE;

    while ( parser.open( 0L, tag ) )
    {
        KOMLParser::parseTag( tag.c_str(), name, lst );

        // object
        if ( name == "OBJECT" )
        {
            KOMLParser::parseTag( tag.c_str(), name, lst );
            vector<KOMLAttrib>::const_iterator it = lst.begin();
            for( ; it != lst.end(); it++ )
            {
                if ( ( *it ).m_strName == "type" )
                    t = ( ObjType )atoi( ( *it ).m_strValue.c_str() );
            }

            switch ( t )
            {
            case OT_LINE:
            {
                KPLineObject *kplineobject = new KPLineObject();
                kplineobject->load( parser, lst );

                if ( _paste )
                {
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Line" ), kplineobject, this );
                    insertCmd->execute();
                    _commands.addCommand( insertCmd );
                }
                else
                    _objectList->append( kplineobject );
            } break;
            case OT_RECT:
            {
                KPRectObject *kprectobject = new KPRectObject();
                kprectobject->setRnds( _xRnd, _yRnd );
                kprectobject->load( parser, lst );

                if ( _paste )
                {
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Rectangle" ), kprectobject, this );
                    insertCmd->execute();
                    _commands.addCommand( insertCmd );
                }
                else
                    _objectList->append( kprectobject );
            } break;
            case OT_ELLIPSE:
            {
                KPEllipseObject *kpellipseobject = new KPEllipseObject();
                kpellipseobject->load( parser, lst );

                if ( _paste )
                {
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Ellipse" ), kpellipseobject, this );
                    insertCmd->execute();
                    _commands.addCommand( insertCmd );
                }
                else
                    _objectList->append( kpellipseobject );
            } break;
            case OT_PIE:
            {
                KPPieObject *kppieobject = new KPPieObject();
                kppieobject->load( parser, lst );

                if ( _paste )
                {
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Pie/Arc/Chors" ), kppieobject, this );
                    insertCmd->execute();
                    _commands.addCommand( insertCmd );
                }
                else
                    _objectList->append( kppieobject );
            } break;
            case OT_AUTOFORM:
            {
                KPAutoformObject *kpautoformobject = new KPAutoformObject();
                kpautoformobject->load( parser, lst );

                if ( _paste )
                {
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Autoform" ), kpautoformobject, this );
                    insertCmd->execute();
                    _commands.addCommand( insertCmd );
                }
                else
                    _objectList->append( kpautoformobject );
            } break;
            case OT_CLIPART:
            {
                KPClipartObject *kpclipartobject = new KPClipartObject( &_clipartCollection );
                kpclipartobject->load( parser, lst );

                if ( _paste )
                {
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Clipart" ), kpclipartobject, this );
                    insertCmd->execute();
                    _commands.addCommand( insertCmd );
                }
                else
                    _objectList->append( kpclipartobject );
            } break;
            case OT_TEXT:
            {
                KPTextObject *kptextobject = new KPTextObject();
                kptextobject->load( parser, lst );

                if ( _paste )
                {
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Text" ), kptextobject, this );
                    insertCmd->execute();
                    _commands.addCommand( insertCmd );
                }
                else
                    _objectList->append( kptextobject );
            } break;
            case OT_PICTURE:
            {
                KPPixmapObject *kppixmapobject = new KPPixmapObject( &_pixmapCollection );
                kppixmapobject->load( parser, lst );

                if ( _paste )
                {
                    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Picture" ), kppixmapobject, this );
                    insertCmd->execute();
                    _commands.addCommand( insertCmd );
                }
                else
                    _objectList->append( kppixmapobject );
            } break;
            default: break;
            }

            if ( objStartY > 0 ) _objectList->last()->moveBy( 0, objStartY );
            if ( pasting )
            {
                _objectList->last()->moveBy( pasteXOffset, pasteYOffset );
                _objectList->last()->setSelected( true );
            }
        }
        else
            cerr << "Unknown tag '" << tag << "' in OBJECTS" << endl;

        if ( !parser.close( tag ) )
        {
            cerr << "ERR: Closing Child" << endl;
            return;
        }
    }
}

/*===================================================================*/
bool KPresenterDoc::completeLoading( KOStore::Store_ptr _store )
{
    if ( _store )
    {
        CORBA::String_var str = urlIntern.isEmpty() ? url() : urlIntern.latin1();

        QValueListIterator<KPPixmapDataCollection::Key> it = pixmapCollectionKeys.begin();

        for ( ; it != pixmapCollectionKeys.end(); ++it )
        {
            QString u = str.in();
            u += "/";
            u += it.node->data.toString();

            QImage img;

            _store->open( u, 0L );
            {
                istorestream in( _store );
                in >> img;
            }
            _store->close();

            _pixmapCollection.getPixmapDataCollection().insertPixmapData( it.node->data, img );
        }

        QValueListIterator<KPClipartCollection::Key> it2 = clipartCollectionKeys.begin();

        for ( ; it2 != clipartCollectionKeys.end(); ++it2 )
        {
            QString u = str.in();
            u += "/";
            u += it2.node->data.toString();

            QPicture pic;
            QCString buf;

            _store->open( u, 0L );
            {
                istorestream in( _store );
                in >> pic;
            }
            _store->close();

            _clipartCollection.insertClipart( it2.node->data, pic );
        }

        KPObject *kpobject = 0L;
        for ( kpobject = _objectList->first(); kpobject; kpobject = _objectList->next() )
        {
            if ( kpobject->getType() == OT_PICTURE )
                dynamic_cast<KPPixmapObject*>( kpobject )->reload();
            else if ( kpobject->getType() == OT_CLIPART )
                dynamic_cast<KPClipartObject*>( kpobject )->reload();
        }

    }
    setPageLayout( __pgLayout, 0, 0 );

    _pixmapCollection.setAllowChangeRef( true );
    _pixmapCollection.getPixmapDataCollection().setAllowChangeRef( true );

    return true;
}

/*========================= create a new shell ========================*/
KOffice::MainWindow_ptr KPresenterDoc::createMainWindow()
{
    KPresenterShell* shell = new KPresenterShell;
    shell->show();
    shell->setDocument( this );

    return KOffice::MainWindow::_duplicate( shell->koInterface() );
}

/*========================= create a view ========================*/
KPresenterView* KPresenterDoc::createPresenterView( QWidget* _parent )
{
    KPresenterView *p = new KPresenterView( _parent, 0L, this );
    //p->QWidget::show();
    m_lstViews.append( p );

    return p;
}

/*========================= create a view ========================*/
OpenParts::View_ptr KPresenterDoc::createView()
{
    return OpenParts::View::_duplicate( createPresenterView() );
}

/*========================== view list ===========================*/
void KPresenterDoc::viewList( KOffice::Document::ViewList*& _list )
{
    ( *_list ).length( m_lstViews.count() );

    int i = 0;
    QListIterator<KPresenterView> it( m_lstViews );
    for( ; it.current(); ++it )
        ( *_list )[ i++ ] = OpenParts::View::_duplicate( it.current() );
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

/*========================= add view =============================*/
void KPresenterDoc::addView( KPresenterView *_view )
{
    m_lstViews.append( _view );
}

/*======================== remove view ===========================*/
void KPresenterDoc::removeView( KPresenterView *_view )
{
    m_lstViews.setAutoDelete( false );
    m_lstViews.removeRef( _view );
    m_lstViews.setAutoDelete( true );
}

/*========================= insert an object =====================*/
void KPresenterDoc::insertObject( const QRect& _rect, KoDocumentEntry& _e, int _diffx, int _diffy )
{
    KOffice::Document_var doc = _e.createDoc();
    if ( CORBA::is_nil( doc ) )
        return;

    if ( !doc->initDoc() )
    {
        QMessageBox::critical( ( QWidget* )0L, i18n( "KPresenter Error" ), i18n( "Could not init" ), i18n( "OK" ) );
        return;
    }

    KPresenterChild* ch = new KPresenterChild( this, _rect, doc, _diffx, _diffy );

    insertChild( ch );
    m_bModified = true;

    KPPartObject *kppartobject = new KPPartObject( ch );
    kppartobject->setOrig( _rect.x() + _diffx, _rect.y() + _diffy );
    kppartobject->setSize( _rect.width(), _rect.height() );
    kppartobject->setSelected( true );

    InsertCmd *insertCmd = new InsertCmd( i18n( "Embed Object" ), kppartobject, this );
    insertCmd->execute();
    _commands.addCommand( insertCmd );

    emit sig_insertObject( ch, kppartobject );

    repaint( false );
}

/*========================= insert a child object =====================*/
void KPresenterDoc::insertChild( KPresenterChild *_child )
{
    m_lstChildren.append( _child );
    m_bModified = true;
}

/*======================= change child geometry ==================*/
void KPresenterDoc::changeChildGeometry( KPresenterChild *_child, const QRect& _rect, int /*_diffx*/, int /*_diffy*/ )
{
    _child->setGeometry( _rect );

    emit sig_updateChildGeometry( _child );

    m_bModified = true;
}

/*======================= child iterator =========================*/
QListIterator<KPresenterChild> KPresenterDoc::childIterator()
{
    return QListIterator<KPresenterChild> ( m_lstChildren );
}

/*===================== set page layout ==========================*/
void KPresenterDoc::setPageLayout( KoPageLayout pgLayout, int diffx, int diffy )
{
    _pageLayout = pgLayout;
    QRect r = getPageSize( 0, diffx, diffy );

    for ( int i = 0; i < static_cast<int>( _backgroundList.count() ); i++ )
    {
        _backgroundList.at( i )->setSize( r.width(), r.height() );
        _backgroundList.at( i )->restore();
    }


    QString unit;
    switch ( _pageLayout.unit )
    {
    case PG_MM: unit = "mm";
        break;
    case PG_PT: unit = "pt";
        break;
    case PG_INCH: unit = "inch";
        break;
    }
    setUnit( _pageLayout.unit, unit );

    m_bModified = true;
    repaint( false );
}

/*==================== insert a new page =========================*/
unsigned int KPresenterDoc::insertNewPage( int diffx, int diffy, bool _restore  )
{

    KPBackGround *kpbackground = new KPBackGround( &_pixmapCollection, &_gradientCollection, &_clipartCollection, this );
    _backgroundList.append( kpbackground );

    if ( _restore )
    {
        QRect r = getPageSize( 0, diffx, diffy );
        _backgroundList.last()->setSize( r.width(), r.height() );
        _backgroundList.last()->restore();
        repaint( false );
    }

    m_bModified = true;
    return getPageNums();
}

/*================================================================*/
bool KPresenterDoc::insertNewTemplate( int /*diffx*/, int /*diffy*/, bool clean )
{
    QString templateDir = KApplication::kde_datadir();

    QString _template;
    QString _globalTemplatePath = kapp->kde_datadir() + "/kpresenter/templates/";
    QString _personalTemplatePath = kapp->localkdedir() + "/share/apps/kpresenter/templates/";

    KoTemplateChooseDia::ReturnType ret;
    ret = KoTemplateChooseDia::chooseTemplate( _globalTemplatePath, _personalTemplatePath, _template, true, false );

    if ( ret == KoTemplateChooseDia::Template )
    {
        QFileInfo fileInfo( _template );
        QString fileName( fileInfo.dirPath( true ) + "/" + fileInfo.baseName() + ".kpt" );
        _clean = clean;
        objStartY = getPageSize( _backgroundList.count() - 1, 0, 0 ).y() + getPageSize( _backgroundList.count() - 1, 0, 0 ).height();
        load_template( fileName.data() );
        objStartY = 0;
        _clean = true;
        m_bModified = true;
        setURL( QString::null );
        return true;
    }
    else if ( ret == KoTemplateChooseDia::File )
    {
        objStartY = 0;
        _clean = true;
        m_bModified = true;
        load_template( _template );
        setURL( _template );
        return true;
    }
    else if ( ret == KoTemplateChooseDia::Empty )
    {
        QString fileName( _globalTemplatePath + "Screenpresentations/Plain.kpt" );
        objStartY = 0;
        _clean = true;
        m_bModified = true;
        load_template( fileName );
        setURL( QString::null );
        return true;
    }
    else
        return false;
}

/*==================== set background color ======================*/
void KPresenterDoc::setBackColor( unsigned int pageNum, QColor backColor1, QColor backColor2, BCType bcType )
{
    KPBackGround *kpbackground = 0;

    if ( pageNum < _backgroundList.count() )
    {
        kpbackground = backgroundList()->at( pageNum );
        kpbackground->setBackColor1( backColor1 );
        kpbackground->setBackColor2( backColor2 );
        kpbackground->setBackColorType( bcType );
    }
    m_bModified = true;
}

/*==================== set background picture ====================*/
void KPresenterDoc::setBackPixFilename( unsigned int pageNum, QString backPix )
{
    QMap< KPPixmapDataCollection::Key, QImage >::Iterator it = _pixmapCollection.getPixmapDataCollection().begin();
    QDateTime dt;

    if ( !QFileInfo( backPix ).exists() )
    {
        for ( ; it != _pixmapCollection.getPixmapDataCollection().end(); ++it )
        {
            if ( it.key().filename == backPix )
            {
                dt = it.key().lastModified;
                break;
            }
        }
    }

    if ( pageNum < _backgroundList.count() )
        backgroundList()->at( pageNum )->setBackPixmap( backPix, dt );
    m_bModified = true;
}

/*==================== set background clipart ====================*/
void KPresenterDoc::setBackClipFilename( unsigned int pageNum, QString backClip )
{
    QMap< KPClipartCollection::Key, QPicture >::Iterator it = _clipartCollection.begin();
    QDateTime dt;

    if ( !QFileInfo( backClip ).exists() )
    {
        for ( ; it != _clipartCollection.end(); ++it )
        {
            if ( it.key().filename == backClip )
            {
                dt = it.key().lastModified;
                break;
            }
        }
    }

    if ( pageNum < _backgroundList.count() )
        backgroundList()->at( pageNum )->setBackClipFilename( backClip, dt );
    m_bModified = true;
}

/*================= set background pic view ======================*/
void KPresenterDoc::setBackView( unsigned int pageNum, BackView backView )
{
    if ( pageNum < _backgroundList.count() )
        backgroundList()->at( pageNum )->setBackView( backView );
    m_bModified = true;
}

/*==================== set background type =======================*/
void KPresenterDoc::setBackType( unsigned int pageNum, BackType backType )
{
    if ( pageNum < _backgroundList.count() )
        backgroundList()->at( pageNum )->setBackType( backType );
    m_bModified = true;
}

/*========================== set page effect =====================*/
void KPresenterDoc::setPageEffect( unsigned int pageNum, PageEffect pageEffect )
{
    if ( pageNum < _backgroundList.count() )
        backgroundList()->at( pageNum )->setPageEffect( pageEffect );
    m_bModified = true;
}

/*===================== set pen and brush ========================*/
bool KPresenterDoc::setPenBrush( QPen pen, QBrush brush, LineEnd lb, LineEnd le, FillType ft, QColor g1, QColor g2, BCType gt )
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

    _objects.setAutoDelete( false );
    _oldPen.setAutoDelete( false );
    _oldBrush.setAutoDelete( false );

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
    {
        kpobject = objectList()->at( i );
        if ( kpobject->isSelected() )
        {
            ptmp = new PenBrushCmd::Pen;
            btmp = new PenBrushCmd::Brush;
            switch ( kpobject->getType() )
            {
            case OT_LINE:
            {
                ptmp->pen = QPen( dynamic_cast<KPLineObject*>( kpobject )->getPen() );
                ptmp->lineBegin = dynamic_cast<KPLineObject*>( kpobject )->getLineBegin();
                ptmp->lineEnd = dynamic_cast<KPLineObject*>( kpobject )->getLineEnd();
                ret = true;
            } break;
            case OT_RECT:
            {
                ptmp->pen = QPen( dynamic_cast<KPRectObject*>( kpobject )->getPen() );
                btmp->brush = QBrush( dynamic_cast<KPRectObject*>( kpobject )->getBrush() );
                btmp->fillType = dynamic_cast<KPRectObject*>( kpobject )->getFillType();
                btmp->gColor1 = dynamic_cast<KPRectObject*>( kpobject )->getGColor1();
                btmp->gColor2 = dynamic_cast<KPRectObject*>( kpobject )->getGColor2();
                btmp->gType = dynamic_cast<KPRectObject*>( kpobject )->getGType();
                ret = true;
            } break;
            case OT_ELLIPSE:
            {
                ptmp->pen = QPen( dynamic_cast<KPEllipseObject*>( kpobject )->getPen() );
                btmp->brush = dynamic_cast<KPEllipseObject*>( kpobject )->getBrush();
                btmp->fillType = dynamic_cast<KPEllipseObject*>( kpobject )->getFillType();
                btmp->gColor1 = dynamic_cast<KPEllipseObject*>( kpobject )->getGColor1();
                btmp->gColor2 = dynamic_cast<KPEllipseObject*>( kpobject )->getGColor2();
                btmp->gType = dynamic_cast<KPEllipseObject*>( kpobject )->getGType();
                ret = true;
            } break;
            case OT_AUTOFORM:
            {
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
            case OT_PIE:
            {
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
            case OT_PART:
            {
                ptmp->pen = QPen( dynamic_cast<KPPartObject*>( kpobject )->getPen() );
                btmp->brush = QBrush( dynamic_cast<KPPartObject*>( kpobject )->getBrush() );
                btmp->fillType = dynamic_cast<KPPartObject*>( kpobject )->getFillType();
                btmp->gColor1 = dynamic_cast<KPPartObject*>( kpobject )->getGColor1();
                btmp->gColor2 = dynamic_cast<KPPartObject*>( kpobject )->getGColor2();
                btmp->gType = dynamic_cast<KPPartObject*>( kpobject )->getGType();
                ret = true;
            } break;
            case OT_TEXT:
            {
                ptmp->pen = QPen( dynamic_cast<KPTextObject*>( kpobject )->getPen() );
                btmp->brush = QBrush( dynamic_cast<KPTextObject*>( kpobject )->getBrush() );
                btmp->fillType = dynamic_cast<KPTextObject*>( kpobject )->getFillType();
                btmp->gColor1 = dynamic_cast<KPTextObject*>( kpobject )->getGColor1();
                btmp->gColor2 = dynamic_cast<KPTextObject*>( kpobject )->getGColor2();
                btmp->gType = dynamic_cast<KPTextObject*>( kpobject )->getGType();
                ret = true;
            } break;
            case OT_PICTURE:
            {
                ptmp->pen = QPen( dynamic_cast<KPPixmapObject*>( kpobject )->getPen() );
                btmp->brush = QBrush( dynamic_cast<KPPixmapObject*>( kpobject )->getBrush() );
                btmp->fillType = dynamic_cast<KPPixmapObject*>( kpobject )->getFillType();
                btmp->gColor1 = dynamic_cast<KPPixmapObject*>( kpobject )->getGColor1();
                btmp->gColor2 = dynamic_cast<KPPixmapObject*>( kpobject )->getGColor2();
                btmp->gType = dynamic_cast<KPPixmapObject*>( kpobject )->getGType();
                ret = true;
            } break;
            case OT_CLIPART:
            {
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

    if ( !_objects.isEmpty() )
    {
        PenBrushCmd *penBrushCmd = new PenBrushCmd( i18n( "Apply Styles" ), _oldPen, _oldBrush, _newPen, _newBrush, _objects, this );
        commands()->addCommand( penBrushCmd );
        penBrushCmd->execute();
    }
    else
    {
        _oldPen.setAutoDelete( true );
        _oldPen.clear();
        _oldBrush.setAutoDelete( true );
        _oldBrush.clear();
    }

    m_bModified = true;
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

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
    {
        kpobject = objectList()->at( i );
        if ( kpobject->isSelected() )
        {
            btmp = new PenBrushCmd::Brush;
            ptmp = new PenBrushCmd::Pen;
            switch ( kpobject->getType() )
            {
            case OT_LINE:
            {
                ptmp->pen = QPen( dynamic_cast<KPLineObject*>( kpobject )->getPen() );
                ptmp->lineBegin = dynamic_cast<KPLineObject*>( kpobject )->getLineBegin();
                ptmp->lineEnd = dynamic_cast<KPLineObject*>( kpobject )->getLineEnd();
                ret = true;
            } break;
            case OT_AUTOFORM:
            {
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

    if ( !_objects.isEmpty() )
    {
        PenBrushCmd *penBrushCmd = new PenBrushCmd( i18n( "Change Line Begin" ), _oldPen, _oldBrush, _newPen, _newBrush, _objects,
                                                    this, PenBrushCmd::LB_ONLY );
        commands()->addCommand( penBrushCmd );
        penBrushCmd->execute();
    }
    else
    {
        _oldPen.setAutoDelete( true );
        _oldPen.clear();
        _oldBrush.setAutoDelete( true );
        _oldBrush.clear();
    }

    m_bModified = true;
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

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
    {
        kpobject = objectList()->at( i );
        if ( kpobject->isSelected() )
        {
            btmp = new PenBrushCmd::Brush;
            ptmp = new PenBrushCmd::Pen;
            switch ( kpobject->getType() )
            {
            case OT_LINE:
            {
                ptmp->pen = QPen( dynamic_cast<KPLineObject*>( kpobject )->getPen() );
                ptmp->lineBegin = dynamic_cast<KPLineObject*>( kpobject )->getLineBegin();
                ptmp->lineEnd = dynamic_cast<KPLineObject*>( kpobject )->getLineEnd();
                ret = true;
            } break;
            case OT_AUTOFORM:
            {
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

    if ( !_objects.isEmpty() )
    {
        PenBrushCmd *penBrushCmd = new PenBrushCmd( i18n( "Change Line End" ), _oldPen, _oldBrush, _newPen, _newBrush, _objects,
                                                    this, PenBrushCmd::LE_ONLY );
        commands()->addCommand( penBrushCmd );
        penBrushCmd->execute();
    }
    else
    {
        _oldPen.setAutoDelete( true );
        _oldPen.clear();
        _oldBrush.setAutoDelete( true );
        _oldBrush.clear();
    }

    m_bModified = true;
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

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
    {
        kpobject = objectList()->at( i );
        if ( kpobject->getType() == OT_PIE )
        {
            if ( kpobject->isSelected() )
            {
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

    if ( !_objects.isEmpty() )
    {
        PieValueCmd *pieValueCmd = new PieValueCmd( i18n( "Change Pie/Arc/Chord Values" ), _oldValues, _newValues, _objects, this );
        commands()->addCommand( pieValueCmd );
        pieValueCmd->execute();
    }
    else
    {
        _oldValues.setAutoDelete( true );
        _oldValues.clear();
    }

    m_bModified = true;
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

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
    {
        kpobject = objectList()->at( i );
        if ( kpobject->getType() == OT_RECT )
        {
            if ( kpobject->isSelected() )
            {
                tmp = new RectValueCmd::RectValues;
                dynamic_cast<KPRectObject*>( kpobject )->getRnds( tmp->xRnd, tmp->yRnd );
                _oldValues.append( tmp );
                _objects.append( kpobject );
                ret = true;
            }
        }
    }

    if ( !_objects.isEmpty() )
    {
        RectValueCmd *rectValueCmd = new RectValueCmd( i18n( "Change Rectangle values" ), _oldValues, _newValues, _objects, this );
        commands()->addCommand( rectValueCmd );
        rectValueCmd->execute();
    }
    else
    {
        _oldValues.setAutoDelete( true );
        _oldValues.clear();
    }

    m_bModified = true;
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

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
    {
        kpobject = objectList()->at( i );
        if ( kpobject->isSelected() )
        {
            ptmp = new PenBrushCmd::Pen;
            btmp = new PenBrushCmd::Brush;
            switch ( kpobject->getType() )
            {
            case OT_LINE:
            {
                ptmp->pen = QPen( dynamic_cast<KPLineObject*>( kpobject )->getPen() );
                ptmp->lineBegin = dynamic_cast<KPLineObject*>( kpobject )->getLineBegin();
                ptmp->lineEnd = dynamic_cast<KPLineObject*>( kpobject )->getLineEnd();
                ret = true;
            } break;
            case OT_RECT:
            {
                ptmp->pen = QPen( dynamic_cast<KPRectObject*>( kpobject )->getPen() );
                btmp->brush = QBrush( dynamic_cast<KPRectObject*>( kpobject )->getBrush() );
                btmp->fillType = dynamic_cast<KPRectObject*>( kpobject )->getFillType();
                btmp->gColor1 = dynamic_cast<KPRectObject*>( kpobject )->getGColor1();
                btmp->gColor2 = dynamic_cast<KPRectObject*>( kpobject )->getGColor2();
                btmp->gType = dynamic_cast<KPRectObject*>( kpobject )->getGType();
                ret = true;
            } break;
            case OT_ELLIPSE:
            {
                ptmp->pen = QPen( dynamic_cast<KPEllipseObject*>( kpobject )->getPen() );
                btmp->brush = dynamic_cast<KPEllipseObject*>( kpobject )->getBrush();
                btmp->fillType = dynamic_cast<KPEllipseObject*>( kpobject )->getFillType();
                btmp->gColor1 = dynamic_cast<KPEllipseObject*>( kpobject )->getGColor1();
                btmp->gColor2 = dynamic_cast<KPEllipseObject*>( kpobject )->getGColor2();
                btmp->gType = dynamic_cast<KPEllipseObject*>( kpobject )->getGType();
                ret = true;
            } break;
            case OT_AUTOFORM:
            {
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
            case OT_PIE:
            {
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
            case OT_PART:
            {
                ptmp->pen = QPen( dynamic_cast<KPPartObject*>( kpobject )->getPen() );
                btmp->brush = QBrush( dynamic_cast<KPPartObject*>( kpobject )->getBrush() );
                btmp->fillType = dynamic_cast<KPPartObject*>( kpobject )->getFillType();
                btmp->gColor1 = dynamic_cast<KPPartObject*>( kpobject )->getGColor1();
                btmp->gColor2 = dynamic_cast<KPPartObject*>( kpobject )->getGColor2();
                btmp->gType = dynamic_cast<KPPartObject*>( kpobject )->getGType();
                ret = true;
            } break;
            case OT_TEXT:
            {
                ptmp->pen = QPen( dynamic_cast<KPTextObject*>( kpobject )->getPen() );
                btmp->brush = QBrush( dynamic_cast<KPTextObject*>( kpobject )->getBrush() );
                btmp->fillType = dynamic_cast<KPTextObject*>( kpobject )->getFillType();
                btmp->gColor1 = dynamic_cast<KPTextObject*>( kpobject )->getGColor1();
                btmp->gColor2 = dynamic_cast<KPTextObject*>( kpobject )->getGColor2();
                btmp->gType = dynamic_cast<KPTextObject*>( kpobject )->getGType();
                ret = true;
            } break;
            case OT_PICTURE:
            {
                ptmp->pen = QPen( dynamic_cast<KPPixmapObject*>( kpobject )->getPen() );
                btmp->brush = QBrush( dynamic_cast<KPPixmapObject*>( kpobject )->getBrush() );
                btmp->fillType = dynamic_cast<KPPixmapObject*>( kpobject )->getFillType();
                btmp->gColor1 = dynamic_cast<KPPixmapObject*>( kpobject )->getGColor1();
                btmp->gColor2 = dynamic_cast<KPPixmapObject*>( kpobject )->getGColor2();
                btmp->gType = dynamic_cast<KPPixmapObject*>( kpobject )->getGType();
                ret = true;
            } break;
            case OT_CLIPART:
            {
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

    if ( !_objects.isEmpty() )
    {
        PenBrushCmd *penBrushCmd = new PenBrushCmd( i18n( "Change Pen" ), _oldPen, _oldBrush, _newPen, _newBrush, _objects,
                                                    this, PenBrushCmd::PEN_ONLY );
        commands()->addCommand( penBrushCmd );
        penBrushCmd->execute();
    }
    else
    {
        _oldPen.setAutoDelete( true );
        _oldPen.clear();
        _oldBrush.setAutoDelete( true );
        _oldBrush.clear();
    }

    m_bModified = true;
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


    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
    {
        kpobject = objectList()->at( i );
        if ( kpobject->isSelected() && kpobject->getType() != OT_LINE )
        {
            ptmp = new PenBrushCmd::Pen;
            btmp = new PenBrushCmd::Brush;
            switch ( kpobject->getType() )
            {
            case OT_RECT:
            {
                ptmp->pen = QPen( dynamic_cast<KPRectObject*>( kpobject )->getPen() );
                btmp->brush = QBrush( dynamic_cast<KPRectObject*>( kpobject )->getBrush() );
                btmp->fillType = dynamic_cast<KPRectObject*>( kpobject )->getFillType();
                btmp->gColor1 = dynamic_cast<KPRectObject*>( kpobject )->getGColor1();
                btmp->gColor2 = dynamic_cast<KPRectObject*>( kpobject )->getGColor2();
                btmp->gType = dynamic_cast<KPRectObject*>( kpobject )->getGType();
                ret = true;
            } break;
            case OT_ELLIPSE:
            {
                ptmp->pen = QPen( dynamic_cast<KPEllipseObject*>( kpobject )->getPen() );
                btmp->brush = dynamic_cast<KPEllipseObject*>( kpobject )->getBrush();
                btmp->fillType = dynamic_cast<KPEllipseObject*>( kpobject )->getFillType();
                btmp->gColor1 = dynamic_cast<KPEllipseObject*>( kpobject )->getGColor1();
                btmp->gColor2 = dynamic_cast<KPEllipseObject*>( kpobject )->getGColor2();
                btmp->gType = dynamic_cast<KPEllipseObject*>( kpobject )->getGType();
                ret = true;
            } break;
            case OT_AUTOFORM:
            {
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
            case OT_PIE:
            {
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
            case OT_PART:
            {
                ptmp->pen = QPen( dynamic_cast<KPPartObject*>( kpobject )->getPen() );
                btmp->brush = QBrush( dynamic_cast<KPPartObject*>( kpobject )->getBrush() );
                btmp->fillType = dynamic_cast<KPPartObject*>( kpobject )->getFillType();
                btmp->gColor1 = dynamic_cast<KPPartObject*>( kpobject )->getGColor1();
                btmp->gColor2 = dynamic_cast<KPPartObject*>( kpobject )->getGColor2();
                btmp->gType = dynamic_cast<KPPartObject*>( kpobject )->getGType();
                ret = true;
            } break;
            case OT_TEXT:
            {
                ptmp->pen = QPen( dynamic_cast<KPTextObject*>( kpobject )->getPen() );
                btmp->brush = QBrush( dynamic_cast<KPTextObject*>( kpobject )->getBrush() );
                btmp->fillType = dynamic_cast<KPTextObject*>( kpobject )->getFillType();
                btmp->gColor1 = dynamic_cast<KPTextObject*>( kpobject )->getGColor1();
                btmp->gColor2 = dynamic_cast<KPTextObject*>( kpobject )->getGColor2();
                btmp->gType = dynamic_cast<KPTextObject*>( kpobject )->getGType();
                ret = true;
            } break;
            case OT_PICTURE:
            {
                ptmp->pen = QPen( dynamic_cast<KPPixmapObject*>( kpobject )->getPen() );
                btmp->brush = QBrush( dynamic_cast<KPPixmapObject*>( kpobject )->getBrush() );
                btmp->fillType = dynamic_cast<KPPixmapObject*>( kpobject )->getFillType();
                btmp->gColor1 = dynamic_cast<KPPixmapObject*>( kpobject )->getGColor1();
                btmp->gColor2 = dynamic_cast<KPPixmapObject*>( kpobject )->getGColor2();
                btmp->gType = dynamic_cast<KPPixmapObject*>( kpobject )->getGType();
                ret = true;
            } break;
            case OT_CLIPART:
            {
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

    if ( !_objects.isEmpty() )
    {
        PenBrushCmd *penBrushCmd = new PenBrushCmd( i18n( "Change Brush" ), _oldPen, _oldBrush, _newPen, _newBrush, _objects,
                                                    this, PenBrushCmd::BRUSH_ONLY );
        commands()->addCommand( penBrushCmd );
        penBrushCmd->execute();
    }
    else
    {
        _oldPen.setAutoDelete( true );
        _oldPen.clear();
        _oldBrush.setAutoDelete( true );
        _oldBrush.clear();
    }

    m_bModified = true;
    return ret;
}

/*=================== get background type ========================*/
BackType KPresenterDoc::getBackType( unsigned int pageNum )
{
    if ( pageNum < _backgroundList.count() )
        return backgroundList()->at( pageNum )->getBackType();

    return BT_COLOR;
}

/*=================== get background pic view ====================*/
BackView KPresenterDoc::getBackView( unsigned int pageNum )
{
    if ( pageNum < _backgroundList.count() )
        return backgroundList()->at( pageNum )->getBackView();

    return BV_TILED;
}

/*=================== get background picture =====================*/
QString KPresenterDoc::getBackPixFilename( unsigned int pageNum )
{
    if ( pageNum < _backgroundList.count() )
        return backgroundList()->at( pageNum )->getBackPixFilename();

    return QString::null;
}

/*=================== get background clipart =====================*/
QString KPresenterDoc::getBackClipFilename( unsigned int pageNum )
{
    if ( pageNum < _backgroundList.count() )
        return backgroundList()->at( pageNum )->getBackClipFilename();

    return QString::null;
}

/*=================== get background color 1 ======================*/
QColor KPresenterDoc::getBackColor1( unsigned int pageNum )
{
    if ( pageNum < _backgroundList.count() )
        return backgroundList()->at( pageNum )->getBackColor1();

    return white;
}

/*=================== get background color 2 ======================*/
QColor KPresenterDoc::getBackColor2( unsigned int pageNum )
{
    if ( pageNum < _backgroundList.count() )
        return backgroundList()->at( pageNum )->getBackColor2();

    return white;
}

/*=================== get background color type ==================*/
BCType KPresenterDoc::getBackColorType( unsigned int pageNum )
{
    if ( pageNum < _backgroundList.count() )
        return backgroundList()->at( pageNum )->getBackColorType();

    return BCT_PLAIN;
}

/*====================== get page effect =========================*/
PageEffect KPresenterDoc::getPageEffect( unsigned int pageNum )
{
    if ( pageNum < _backgroundList.count() )
        return backgroundList()->at( pageNum )->getPageEffect();

    return PEF_NONE;
}

/*========================= get pen ==============================*/
QPen KPresenterDoc::getPen( QPen pen )
{
    KPObject *kpobject = 0;

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
    {
        kpobject = objectList()->at( i );
        if ( kpobject->isSelected() )
        {
            switch ( kpobject->getType() )
            {
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

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
    {
        kpobject = objectList()->at( i );
        if ( kpobject->isSelected() )
        {
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

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
    {
        kpobject = objectList()->at( i );
        if ( kpobject->isSelected() )
        {
            switch ( kpobject->getType() )
            {
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

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
    {
        kpobject = objectList()->at( i );
        if ( kpobject->isSelected() )
        {
            switch ( kpobject->getType() )
            {
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

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
    {
        kpobject = objectList()->at( i );
        if ( kpobject->isSelected() )
        {
            switch ( kpobject->getType() )
            {
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

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
    {
        kpobject = objectList()->at( i );
        if ( kpobject->isSelected() )
        {
            switch ( kpobject->getType() )
            {
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

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
    {
        kpobject = objectList()->at( i );
        if ( kpobject->isSelected() )
        {
            switch ( kpobject->getType() )
            {
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

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
    {
        kpobject = objectList()->at( i );
        if ( kpobject->isSelected() )
        {
            switch ( kpobject->getType() )
            {
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
PieType KPresenterDoc::getPieType( PieType pieType )
{
    KPObject *kpobject = 0;

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
    {
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

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
    {
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

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
    {
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

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
    {
        kpobject = objectList()->at( i );
        if ( kpobject->isSelected() && kpobject->getType() == OT_RECT )
        {
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

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
    {
        kpobject = objectList()->at( i );
        if ( kpobject->isSelected() && kpobject->getType() == OT_RECT )
        {
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

    for ( int i = 0; i < static_cast<int>( _new->count() ); i++ )
    {
        kpobject = _new->at( i );
        if ( kpobject->isSelected() )
        {
            _new->take( i );
            _new->insert( 0, kpobject );
        }
    }

    LowerRaiseCmd *lrCmd = new LowerRaiseCmd( i18n( "Lower Object( s )" ), _objectList, _new, this );
    lrCmd->execute();
    _commands.addCommand( lrCmd );

    m_bModified = true;
}

/*========================= raise object =========================*/
void KPresenterDoc::raiseObjs( int /*diffx*/, int /*diffy*/ )
{
    KPObject *kpobject = 0;

    QList<KPObject> *_new = new QList<KPObject>;

    for ( unsigned int j = 0; j < _objectList->count(); j++ )
        _new->append( _objectList->at( j ) );

    _new->setAutoDelete( false );

    for ( int i = 0; i < static_cast<int>( _new->count() ); i++ )
    {
        kpobject = objectList()->at( i );
        if ( kpobject->isSelected() )
        {
            _new->take( i );
            _new->append( kpobject );
        }
    }

    LowerRaiseCmd *lrCmd = new LowerRaiseCmd( i18n( "Lower Object( s )" ), _objectList, _new, this );
    lrCmd->execute();
    _commands.addCommand( lrCmd );

    m_bModified = true;
}

/*=================== insert a picture ==========================*/
void KPresenterDoc::insertPicture( QString filename, int diffx, int diffy, int _x , int _y )
{
    QMap< KPPixmapDataCollection::Key, QImage >::Iterator it = _pixmapCollection.getPixmapDataCollection().begin();
    QDateTime dt;

    if ( !QFileInfo( filename ).exists() )
    {
        for( ; it != _pixmapCollection.getPixmapDataCollection().end(); ++it )
        {
            if ( it.key().filename == filename )
            {
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

    m_bModified = true;
}

/*=================== insert a clipart ==========================*/
void KPresenterDoc::insertClipart( QString filename, int diffx, int diffy )
{
    QMap< KPClipartCollection::Key, QPicture >::Iterator it = _clipartCollection.begin();
    QDateTime dt;

    if ( !QFileInfo( filename ).exists() )
    {
        for ( ; it != _clipartCollection.end(); ++it )
        {
            if ( it.key().filename == filename )
            {
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

    m_bModified = true;
}

/*======================= change picture ========================*/
void KPresenterDoc::changePicture( QString filename, int /*diffx*/, int /*diffy*/ )
{
    KPObject *kpobject = 0;

    QMap< KPPixmapDataCollection::Key, QImage >::Iterator it = _pixmapCollection.getPixmapDataCollection().begin();
    QDateTime dt;

    if ( !QFileInfo( filename ).exists() )
    {
        for( ; it != _pixmapCollection.getPixmapDataCollection().end(); ++it )
        {
            if ( it.key().filename == filename )
            {
                dt = it.key().lastModified;
                break;
            }
        }
    }

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
    {
        kpobject = objectList()->at( i );
        if ( kpobject->isSelected() && kpobject->getType() == OT_PICTURE )
        {
            KPPixmapObject *pix = new KPPixmapObject( &_pixmapCollection, filename, QDateTime() );

            ChgPixCmd *chgPixCmd = new ChgPixCmd( i18n( "Change pixmap" ), dynamic_cast<KPPixmapObject*>( kpobject ),
                                                  pix, this );
            chgPixCmd->execute();
            _commands.addCommand( chgPixCmd );
            break;
        }
    }

    m_bModified = true;
}

/*======================= change clipart ========================*/
void KPresenterDoc::changeClipart( QString filename, int /*diffx*/, int /*diffy*/ )
{
    QMap< KPClipartCollection::Key, QPicture >::Iterator it = _clipartCollection.begin();
    QDateTime dt;

    if ( !QFileInfo( filename ).exists() )
    {
        for ( ; it != _clipartCollection.end(); ++it )
        {
            if ( it.key().filename == filename )
            {
                dt = it.key().lastModified;
                break;
            }
        }
    }

    KPObject *kpobject = 0;

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
    {
        kpobject = objectList()->at( i );
        if ( kpobject->isSelected() && kpobject->getType() == OT_CLIPART )
        {
            ChgClipCmd *chgClipCmd = new ChgClipCmd( i18n( "Change clipart" ), dynamic_cast<KPClipartObject*>( kpobject ),
                                                     dynamic_cast<KPClipartObject*>( kpobject )->getKey(),
                                                     KPClipartCollection::Key( filename, dt ), this );
            chgClipCmd->execute();
            _commands.addCommand( chgClipCmd );
            break;
        }
    }

    m_bModified = true;
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

    m_bModified = true;
}

/*===================== insert a rectangle =======================*/
void KPresenterDoc::insertRectangle( QRect r, QPen pen, QBrush brush, FillType ft, QColor g1, QColor g2,
                                     BCType gt, int rndX, int rndY, int diffx, int diffy )
{
    KPRectObject *kprectobject = new KPRectObject( pen, brush, ft, g1, g2, gt, rndX, rndY );
    kprectobject->setOrig( r.x() + diffx, r.y() + diffy );
    kprectobject->setSize( r.width(), r.height() );
    kprectobject->setSelected( true );

    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert rectangle" ), kprectobject, this );
    insertCmd->execute();
    _commands.addCommand( insertCmd );

    m_bModified = true;
}

/*===================== insert a circle or ellipse ===============*/
void KPresenterDoc::insertCircleOrEllipse( QRect r, QPen pen, QBrush brush, FillType ft, QColor g1, QColor g2,
                                           BCType gt, int diffx, int diffy )
{
    KPEllipseObject *kpellipseobject = new KPEllipseObject( pen, brush, ft, g1, g2, gt );
    kpellipseobject->setOrig( r.x() + diffx, r.y() + diffy );
    kpellipseobject->setSize( r.width(), r.height() );
    kpellipseobject->setSelected( true );

    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert ellipse" ), kpellipseobject, this );
    insertCmd->execute();
    _commands.addCommand( insertCmd );

    m_bModified = true;
}

/*================================================================*/
void KPresenterDoc::insertPie( QRect r, QPen pen, QBrush brush, FillType ft, QColor g1, QColor g2,
                               BCType gt, PieType pt, int _angle, int _len, LineEnd lb, LineEnd le, int diffx, int diffy )
{
    KPPieObject *kppieobject = new KPPieObject( pen, brush, ft, g1, g2, gt, pt, _angle, _len, lb, le );
    kppieobject->setOrig( r.x() + diffx, r.y() + diffy );
    kppieobject->setSize( r.width(), r.height() );
    kppieobject->setSelected( true );

    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert pie/arc/chord" ), kppieobject, this );
    insertCmd->execute();
    _commands.addCommand( insertCmd );

    m_bModified = true;
}

/*===================== insert a textobject =====================*/
void KPresenterDoc::insertText( QRect r, int diffx, int diffy, QString text, KPresenterView *_view )
{
    KPTextObject *kptextobject = new KPTextObject();
    kptextobject->setOrig( r.x() + diffx, r.y() + diffy );
    kptextobject->setSize( r.width(), r.height() );
    kptextobject->setSelected( true );
    if ( !text.isEmpty() && _view )
    {
        kptextobject->getKTextObject()->clear();
        kptextobject->getKTextObject()->addText( text, _view->currFont(), _view->currColor() );
    }

    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert text" ), kptextobject, this );
    insertCmd->execute();
    _commands.addCommand( insertCmd );

    m_bModified = true;
}

/*======================= insert an autoform ====================*/
void KPresenterDoc::insertAutoform( QRect r, QPen pen, QBrush brush, LineEnd lb, LineEnd le, FillType ft, QColor g1, QColor g2,
                                    BCType gt, QString fileName, int diffx, int diffy )
{
    KPAutoformObject *kpautoformobject = new KPAutoformObject( pen, brush, fileName, lb, le, ft, g1, g2, gt );
    kpautoformobject->setOrig( r.x() + diffx, r.y() + diffy );
    kpautoformobject->setSize( r.width(), r.height() );
    kpautoformobject->setSelected( true );

    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert autoform" ), kpautoformobject, this );
    insertCmd->execute();
    _commands.addCommand( insertCmd );

    m_bModified = true;
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
    if ( !m_lstViews.isEmpty() )
    {
        for ( viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() )
            viewPtr->repaint( erase );
    }
}

/*==============================================================*/
void KPresenterDoc::setUnit( KoUnit _unit, QString __unit )
{
    _pageLayout.unit = _unit;

    if ( !m_lstViews.isEmpty() )
    {
        for ( viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() )
        {
            viewPtr->getHRuler()->setUnit( __unit );
            viewPtr->getVRuler()->setUnit( __unit );
        }
    }
}

/*===================== repaint =================================*/
void KPresenterDoc::repaint( QRect rect )
{
    if ( !m_lstViews.isEmpty() )
    {
        QRect r;

        for ( viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() )
        {
            r = rect;
            r.moveTopLeft( QPoint( r.x() - viewPtr->getDiffX(), r.y() - viewPtr->getDiffY() ) );

            viewPtr->repaint( r, false );
        }
    }
}

/*===================== repaint =================================*/
void KPresenterDoc::repaint( KPObject *kpobject )
{
    if ( !m_lstViews.isEmpty() )
    {
        QRect r;

        for ( viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() )
        {
            r = kpobject->getBoundingRect( 0, 0 );
            r.moveTopLeft( QPoint( r.x() - viewPtr->getDiffX(), r.y() - viewPtr->getDiffY() ) );

            viewPtr->repaint( r, false );
        }
    }
}

/*==================== reorder page =============================*/
QValueList<int> KPresenterDoc::reorderPage( unsigned int num, int diffx, int diffy, float fakt )
{
    QValueList<int> orderList;

    orderList.append( 0 );

    KPObject *kpobject = 0;

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
    {
        kpobject = objectList()->at( i );
        if ( getPageOfObj( i, diffx, diffy, fakt ) == static_cast<int>( num ) )
        {
            if ( orderList.find( kpobject->getPresNum() ) == orderList.end() )
            {
                if ( orderList.isEmpty() )
                    orderList.append( kpobject->getPresNum() );
                else
                {
                    QValueList<int>::Iterator it = orderList.begin();
                    for ( ; *it < kpobject->getPresNum() && it != orderList.end(); ++it );
                    orderList.insert( it, kpobject->getPresNum() );
                }
            }
            if ( kpobject->getDisappear() && orderList.find( kpobject->getDisappearNum() ) == orderList.end() )
            {
                if ( orderList.isEmpty() )
                    orderList.append( kpobject->getDisappearNum() );
                else
                {
                    QValueList<int>::Iterator it = orderList.begin();
                    for ( ; *it < kpobject->getDisappearNum() && it != orderList.end(); ++it );
                    orderList.insert( it, kpobject->getDisappearNum() );
                }
            }
        }
    }

    m_bModified = true;
    return orderList;
}

/*====================== get page of object ======================*/
int KPresenterDoc::getPageOfObj( int objNum, int diffx, int diffy, float fakt )
{
    QRect rect;

    KPObject *kpobject = 0;

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
    {
        kpobject = objectList()->at( i );
        if ( i == objNum )
        {
            for ( int j = 0; j < static_cast<int>( _backgroundList.count() ); j++ )
            {
                rect = getPageSize( j, diffx, diffy, fakt, false );
                rect.setWidth( QApplication::desktop()->width() );
                if ( rect.intersects( kpobject->getBoundingRect( diffx, diffy ) ) )
                    return j+1;
            }
        }
    }
    return -1;
}

/*================== get size of page ===========================*/
QRect KPresenterDoc::getPageSize( unsigned int num, int diffx, int diffy, float fakt , bool decBorders )
{
//   double fact = 1;
//   if ( _pageLayout.unit == PG_CM ) fact = 10;
//   if ( _pageLayout.unit == PG_INCH ) fact = 25.4;

    int pw, ph, bl = _pageLayout.ptLeft;
    int br = _pageLayout.ptRight;
    int bt = _pageLayout.ptTop;
    int bb = _pageLayout.ptBottom;
    int wid = _pageLayout.ptWidth;
    int hei = _pageLayout.ptHeight;

    if ( !decBorders )
    {
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
    return _pageLayout.ptLeft;
}

/*================================================================*/
int KPresenterDoc::getTopBorder()
{
    return _pageLayout.ptTop;
}

/*================================================================*/
int KPresenterDoc::getBottomBorder()
{
    return _pageLayout.ptBottom;
}

/*================================================================*/
void KPresenterDoc::deletePage( int _page, DelPageMode _delPageMode )
{
    KPObject *kpobject = 0;
    int _h = getPageSize( 0, 0, 0 ).height();

    if ( _delPageMode == DPM_DEL_OBJS || _delPageMode == DPM_DEL_MOVE_OBJS )
    {
        deSelectAllObj();
        for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
        {
            kpobject = objectList()->at( i );
            if ( getPageOfObj( i, 0, 0 ) - 1 == _page )
                kpobject->setSelected( true );
        }
        deleteObjs( false );
    }

    if ( _delPageMode == DPM_MOVE_OBJS || _delPageMode == DPM_DEL_MOVE_OBJS )
    {
        for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
        {
            kpobject = objectList()->at( i );
            if ( getPageOfObj( i, 0, 0 ) - 1 > _page )
                kpobject->setOrig( kpobject->getOrig().x(), kpobject->getOrig().y() - _h );
        }
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

    if ( _insPageMode == IPM_MOVE_OBJS )
    {
        for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
        {
            kpobject = objectList()->at( i );
            if ( getPageOfObj( i, 0, 0 ) - 1 > _page )
                kpobject->setOrig( kpobject->getOrig().x(), kpobject->getOrig().y() + _h );
        }
    }

    if ( _insPos == IP_BEFORE ) _page++;

    QString templateDir = KApplication::kde_datadir();

    QString _template;
    QString _globalTemplatePath = kapp->kde_datadir() + "/kpresenter/templates/";
    QString _personalTemplatePath = kapp->localkdedir() + "/share/apps/kpresenter/templates/";

    if ( KoTemplateChooseDia::chooseTemplate( _globalTemplatePath, _personalTemplatePath, _template, true, true ) != KoTemplateChooseDia::Cancel )
    {
        QFileInfo fileInfo( _template );
        QString fileName( fileInfo.dirPath( true ) + "/" + fileInfo.baseName() + ".kpt" );
        _clean = false;

        if ( _insPos == IP_AFTER ) _page++;
        objStartY = getPageSize( _page - 1, 0, 0 ).y() + getPageSize( _page - 1, 0, 0 ).height();
        load_template( fileName.data() );
        objStartY = 0;
        _clean = true;
        m_bModified = true;
        KPBackGround *kpbackground = _backgroundList.at( _backgroundList.count() - 1 );
        _backgroundList.take( _backgroundList.count() - 1 );
        _backgroundList.insert( _page, kpbackground );
        setURL( QString::null );
    }

    repaint( false );
}

/*================ return number of selected objs ================*/
int KPresenterDoc::numSelected()
{
    int num = 0;

    KPObject *kpobject = 0;

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
    {
        kpobject = objectList()->at( i );
        if ( kpobject->isSelected() ) num++;
    }

    return num;
}

/*==================== return selected obj ======================*/
KPObject* KPresenterDoc::getSelectedObj()
{
    KPObject *kpobject = 0;

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
    {
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

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
    {
        kpobject = objectList()->at( i );
        if ( kpobject->isSelected() )
            _objects.append( kpobject );
    }

    DeleteCmd *deleteCmd = new DeleteCmd( i18n( "Delete object( s )" ), _objects, this );
    deleteCmd->execute();

    if ( _add ) _commands.addCommand( deleteCmd );

    m_bModified = true;
}

/*========================== copy objects ========================*/
void KPresenterDoc::copyObjs( int diffx, int diffy )
{
    QClipboard *cb = QApplication::clipboard();
    string clip_str;
    tostrstream out( clip_str );
    KPObject *kpobject = 0;

    out << otag << "<DOC author=\"" << "Reginald Stadlbauer" << "\" email=\"" << "reggie@kde.org" << "\" editor=\"" << "KPresenter"
        << "\" mime=\"" << "application/x-kpresenter-selection" << "\">" << endl;
    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
    {
        kpobject = objectList()->at( i );
        if ( kpobject->isSelected() )
        {
            out << otag << "<OBJECT type=\"" << static_cast<int>( kpobject->getType() ) << "\">" << endl;
            kpobject->moveBy( -diffx, -diffy );
            kpobject->save( out );
            kpobject->moveBy( diffx, diffy );
            out << etag << "</OBJECT>" << endl;
        }
    }
    out << etag << "</DOC>" << endl;

    cb->setText( clip_str.c_str() );
}

/*========================= paste objects ========================*/
void KPresenterDoc::pasteObjs( int diffx, int diffy )
{
    deSelectAllObj();

    pasting = true;
    pasteXOffset = diffx + 20;
    pasteYOffset = diffy + 20;
    string clip_str = QApplication::clipboard()->text().ascii();

    if ( clip_str.empty() ) return;

    istrstream in( clip_str.c_str() );
    loadStream( in );

    pasting = false;
    m_bModified = true;
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

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
    {
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

    m_bModified = true;
}

/*========================= restore background ==================*/
void KPresenterDoc::restoreBackground( int pageNum )
{
    if ( pageNum < static_cast<int>( _backgroundList.count() ) )
        backgroundList()->at( pageNum )->restore();
}

/*==================== load stream ==============================*/
void KPresenterDoc::loadStream( istream &in )
{
    KOMLStreamFeed feed( in );
    KOMLParser parser( &feed );

    string tag;
    vector<KOMLAttrib> lst;
    string name;

    // DOC
    if ( !parser.open( "DOC", tag ) )
    {
        cerr << "Missing DOC" << endl;
        return;
    }

    KOMLParser::parseTag( tag.c_str(), name, lst );
    vector<KOMLAttrib>::const_iterator it = lst.begin();
    for( ; it != lst.end(); it++ )
    {
        if ( ( *it ).m_strName == "mime" )
        {
            if ( ( *it ).m_strValue != "application/x-kpresenter-selection" )
            {
                cerr << "Unknown mime type " << ( *it ).m_strValue << endl;
                return;
            }
        }
    }

    loadObjects( parser, lst, true );

    repaint( false );
    m_bModified = true;
}

/*================= deselect all objs ===========================*/
void KPresenterDoc::deSelectAllObj()
{
    if ( !m_lstViews.isEmpty() )
    {
        for ( viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() )
            viewPtr->getPage()->deSelectAllObj();
    }
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

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
    {
        kpobject = objectList()->at( i );
        if ( kpobject->isSelected() )
        {
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

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
    {
        kpobject = objectList()->at( i );
        if ( kpobject->isSelected() )
        {
            _objects.append( kpobject );
            _diffs.append( new QPoint( ( _w - kpobject->getSize().width() ) / 2 - kpobject->getOrig().x() + _x, 0 ) );
        }
    }

    MoveByCmd2 *moveByCmd2 = new MoveByCmd2( i18n( "Align object( s ) centered ( horizontal )" ), _diffs, _objects, this );
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

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
    {
        kpobject = objectList()->at( i );
        if ( kpobject->isSelected() )
        {
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

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
    {
        kpobject = objectList()->at( i );
        if ( kpobject->isSelected() )
        {
            pgnum = getPageOfObj( i, 0, 0 );
            if ( pgnum != -1 )
            {
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

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
    {
        kpobject = objectList()->at( i );
        if ( kpobject->isSelected() )
        {
            pgnum = getPageOfObj( i, 0, 0 );
            if ( pgnum != -1 )
            {
                _y = getPageSize( pgnum - 1, 0, 0 ).y();
                _h = getPageSize( pgnum - 1, 0, 0 ).height();
                _objects.append( kpobject );
                _diffs.append( new QPoint( 0, ( _h - kpobject->getSize().height() ) / 2 - kpobject->getOrig().y() + _y ) );
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

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
    {
        kpobject = objectList()->at( i );
        if ( kpobject->isSelected() )
        {
            pgnum = getPageOfObj( i, 0, 0 );
            if ( pgnum != -1 )
            {
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
    if ( !m_lstViews.isEmpty() )
    {
        for ( viewPtr = m_lstViews.first(); viewPtr != 0; viewPtr = m_lstViews.next() )
        {
            viewPtr->changeUndo( _undo, !_undo.isEmpty() );
            viewPtr->changeRedo( _redo, !_redo.isEmpty() );
        }
    }
}

/*=================== count of views ===========================*/
int KPresenterDoc::viewCount()
{
    return m_lstViews.count();
}

/*==============================================================*/
int KPresenterDoc::getPenBrushFlags()
{
    int flags = 0;
    KPObject *kpobject = 0;

    for ( int i = 0; i < static_cast<int>( objectList()->count() ); i++ )
    {
        kpobject = objectList()->at( i );
        if ( kpobject->isSelected() )
        {
            switch ( kpobject->getType() )
            {
            case OT_LINE:
                flags = flags | SD_PEN;
                break;
            case OT_AUTOFORM: case OT_RECT: case OT_ELLIPSE: case OT_PIE: case OT_PART: case OT_TEXT: case OT_PICTURE: case OT_CLIPART:
            {
                flags = flags | SD_PEN;
                flags = flags | SD_BRUSH;
            } break;
            default: break;
            }
        }
    }

    if ( flags == 0 ) flags = SD_PEN | SD_BRUSH;
    return flags;
}

/*================================================================*/
QString KPresenterDoc::getPageTitle( unsigned int pgNum, const QString &_title )
{
    QList<KPObject> objs;
    objs.setAutoDelete( false );

    QRect rect = getPageSize( pgNum, 0, 0 );

    KPObject *kpobject = 0L, *obj = 0L;
    for ( kpobject = _objectList->first(); kpobject; kpobject = _objectList->next() )
        if ( kpobject->getType() == OT_TEXT && rect.intersects( kpobject->getBoundingRect( 0, 0 ) ) &&
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

    KTextObject *txtObj = dynamic_cast<KPTextObject*>( obj )->getKTextObject();

    if ( txtObj->lines() == 2 )
    {
        QString l1 = txtObj->lineAt( 0 )->getText();
        l1 = l1.simplifyWhiteSpace();
        QString l2 = txtObj->lineAt( 1 )->getText();
        l2 = l2.simplifyWhiteSpace();
        return QString( "%1 %2" ).arg( l1 ).arg( l2 );
    }

    return QString( txtObj->lineAt( 0 )->getText() ).simplifyWhiteSpace();
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
    switch ( presentSlides )
    {
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
    for ( kpobject = _objectList->first(); kpobject; kpobject = _objectList->next() )
        if ( kpobject->getType() == OT_PICTURE )
            usedPixmaps.append( dynamic_cast<KPPixmapObject*>( kpobject )->getKey() );

    KPBackGround *kpbackground = 0;
    for ( kpbackground = _backgroundList.first(); kpbackground; kpbackground = _backgroundList.next() )
        if ( kpbackground->getBackType() == BT_PICTURE )
            usedPixmaps.append( kpbackground->getKey() );

}
