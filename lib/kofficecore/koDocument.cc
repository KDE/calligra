/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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

#include "koDocument.h"
#include "koQueryTypes.h"

#include <koStore.h>

#include <kurl.h>
#include <klocale.h>
#include <kapp.h>

#include <qmsgbox.h>
#include <qpainter.h>
#include <qcolor.h>
#include <qpicture.h>
#include <qbuffer.h>
#include <qfile.h>

/**********************************************************
 *
 * KoDocumentChildPicture
 *
 **********************************************************/

KoDocumentChildPicture::KoDocumentChildPicture( KoDocumentChild *_child )
{
  m_pChild = _child;
}

KoDocumentChildPicture::~KoDocumentChildPicture()
{
}

/**********************************************************
 *
 * KoDocumentChild
 *
 **********************************************************/

KoDocumentChild::KoDocumentChild( const QRect& _rect, KOffice::Document_ptr _doc )
{
  m_rDoc = KOffice::Document::_duplicate( _doc );
  CORBA::String_var m = m_rDoc->mimeType();
  m_strMimeType = m.in();
  m_geometry = _rect;
  m_pPicture = 0L;
  // We assume for now that we dont have a printing extension
  m_bHasPrintingExtension = false;
}

KoDocumentChild::KoDocumentChild()
{
  m_rDoc = 0L;
  m_pPicture = 0L;
  // We assume for now that we dont have a printing extension
  m_bHasPrintingExtension = false;
}

void KoDocumentChild::setGeometry( const QRect& _rect )
{
  m_geometry = _rect;
  if ( m_pPicture )
  {
    delete m_pPicture;
    m_pPicture = 0L;
  }
}

KOffice::View_ptr KoDocumentChild::createView( KOffice::MainWindow_ptr _main )
{
  KOffice::View_var v;
  if ( CORBA::is_nil( m_rDoc ) )
  {
      kdebug( KDEBUG_FATAL, 30003, "KoDocumentChild::createView failed since m_rDoc is nil !" );
      return 0L;
  }

  try
  {
    // We make an upcast here. This will ALWAYS succeed
    OpenParts::View_var d = m_rDoc->createView();
    if ( CORBA::is_nil( d ) )
      return 0L;
    v = KOffice::View::_narrow( d );
    if( CORBA::is_nil( v ) )
    {
      kdebug( KDEBUG_FATAL, 30003, "Shit! We did not get a view!!!" );
      exit(1);
    }
  }
  catch ( OpenParts::Document::MultipleViewsNotSupported &_ex )
  {
    // HACK
    kdebug( KDEBUG_ERROR, 30003, "void KSpreadView::slotInsertObject( const QRect& _rect, OPParts::Document_ptr _doc )" );
    kdebug( KDEBUG_ERROR, 30003, "Could not create view" );
    return 0L;
  }

  if ( CORBA::is_nil( v ) )
  {
    kdebug( KDEBUG_ERROR, 30003, "void KSpreadView::slotInsertObject( const QRect& _rect, OPParts::Document_ptr _doc )" );
    kdebug( KDEBUG_ERROR, 30003, "return value is 0L" );
    return 0L;
  }

  v->setMode( KOffice::View::ChildMode );
  v->setMainWindow( _main );

  return KOffice::View::_duplicate( v );
}

bool KoDocumentChild::load( const QDomElement& element )
{
  m_strURL = element.attribute( "url" );
  m_strMimeType = element.attribute( "mime" );

  if ( m_strURL.isEmpty() )
  {	
    kdebug( KDEBUG_INFO, 30003, "Empty 'id' attribute in OBJECT" );
    return false;
  }
  else if ( m_strMimeType.isEmpty() )
  {
    kdebug( KDEBUG_INFO, 30003, "Empty mime attribute in OBJECT" );
    return false;
  }

  QDomElement e = element.namedItem( "geometry" ).toElement();
  if ( e.isNull() )
  {
    kdebug( KDEBUG_INFO, 30003, "Missing RECT in OBJECT" );
    return false;
  }
  m_geometry = e.toRect();

  return true;
}

bool KoDocumentChild::loadDocument( KOStore::Store_ptr _store, const char *_format )
{
  assert( !m_strURL.isEmpty() );

  kdebug( KDEBUG_INFO, 30003, "Trying to load %c", m_strURL.ascii() );
  KURL u( m_strURL );

  KoDocumentEntry e = KoDocumentEntry::queryByMimeType( m_strMimeType );
  if ( e.isEmpty() )
  {
    kdebug( KDEBUG_INFO, 30003, "ERROR: Could not create child document" );
    return false;
  }

  m_rDoc = e.createDoc();
  if ( CORBA::is_nil( m_rDoc ) )
  {
    kdebug( KDEBUG_INFO, 30003, "ERROR: Could not create child document" );
    return false;
  }

  if ( strcmp( u.protocol(), "store" ) == 0 )
    return m_rDoc->loadFromStore( _store, m_strURL );

  return m_rDoc->loadFromURL( m_strURL, _format );
}

QDomElement KoDocumentChild::save( QDomDocument& doc )
{
  CORBA::String_var u = m_rDoc->url();
  CORBA::String_var mime = m_rDoc->mimeType();

  QDomElement e = doc.createElement( "object" );
  e.setAttribute( "url", u.in() );
  e.setAttribute( "mime", mime.in() );
  e.appendChild( doc.createElement( "geometry", m_geometry ) );

  return e;
}

bool KoDocumentChild::isStoredExtern()
{
  CORBA::String_var url = m_rDoc->url();
  QString s = url.in();
  if ( s.isEmpty() )
    return false;
  KURL u( s );
  return ( u.protocol() != "store" );
}

QPicture* KoDocumentChild::draw( float _scale, bool _force_update )
{
  kdebug( KDEBUG_INFO, 30003, "QPicture* KoDocumentChild::draw( bool _force )" );

  // No support for printing extension? => return white plane
  if ( m_pPicture != 0L && !m_bHasPrintingExtension )
    return m_pPicture;

  // Do we have it in the cache ?
  if ( m_pPicture != 0L && m_pictureScale == _scale && !_force_update )
    return m_pPicture;

  kdebug( KDEBUG_INFO, 30003, "Trying to fetch the QPicture stuff" );

  // Try to get the printing extension
  KOffice::Print_var print;
  CORBA::Object_var obj = m_rDoc->getInterface( "IDL:KOffice/Print:1.0" );
  if ( !CORBA::is_nil( obj ) )
    print = KOffice::Print::_narrow( m_rDoc );

  // Draw a white area if there is no printing extension
  if ( CORBA::is_nil( print ) || CORBA::is_nil( obj ) )
  {
    if ( m_pPicture == 0L )
      m_pPicture = new QPicture;
    m_pictureScale = _scale;

    kdebug( KDEBUG_INFO, 30003, "KOffice::Print not supported" );
    QPainter painter;
    painter.begin( m_pPicture );

    painter.fillRect( 0, 0, m_geometry.width(), m_geometry.height(), Qt::white );
    painter.end();
    return m_pPicture;
  }

  kdebug( KDEBUG_INFO, 30003, "Fetching data" );

  // retrieve the QPicture
  KOffice::Print::Picture_var pic = print->picture( m_geometry.width(), m_geometry.height(), _scale );

  CORBA::ULong len = pic->length();
  kdebug( KDEBUG_INFO, 30003, "GOT %i bytes", len );
  QByteArray buffer( len );
  char *p = buffer.data();
  for( CORBA::ULong i = 0; i < len; ++i )
    *p++ = pic[ i ];

  // Free memory
  pic->length( 0 );

  m_bHasPrintingExtension = true;

  if ( m_pPicture == 0L )
    m_pPicture = new QPicture;
  m_pictureScale = _scale;

  m_pPicture->setData( p, len );

  return m_pPicture;
}

KoDocumentChild::~KoDocumentChild()
{
  m_rDoc = 0L;
}

/**********************************************************
 *
 * KoDocument
 *
 **********************************************************/

KoDocument::KoDocument()
{
  ADD_INTERFACE( "IDL:KOffice/Document:1.0" );
}

void KoDocument::cleanUp()
{
  if ( m_bIsClean )
    return;

  m_lstAllChildren.clear();

  OPDocumentIf::cleanUp();
}

void KoDocument::makeChildList( KOffice::Document_ptr _root, const char *_url )
{
  bool is_embedded = true;
  // Is this document stored extern ?
  if ( !m_strURL.isEmpty() )
  {
    KURL u( m_strURL );
    // Do we already use the "store" protocol ?
    if ( !u.isMalformed() && strcmp( u.protocol(), "store" ) != 0 )
      // No "store" protocol, so we are saved externally
      is_embedded = false;
  }

  // If not stored extern, we want to embed it and take the suggested name.
  if ( is_embedded )
    setURL( _url );

  // Tell our parent about us and our decision regarding the URL
  _root->addToChildList( this, _url );
  // Proceed with our children
  makeChildListIntern( _root, _url );
}

void KoDocument::makeChildListIntern( KOffice::Document_ptr /* root */, const char * /* _id */)
{
  // Lets assume that we do not have children
  kdebug( KDEBUG_FATAL, 30003, "void Document_impl::makeChildListIntern( OPParts::Document_ptr _root, const char *_id )" );
  kdebug( KDEBUG_FATAL, 30003, "Not implemented ( not really an error )" );
}

void KoDocument::makeChildListIntern()
{
  m_lstAllChildren.clear();

  makeChildListIntern( this, "store:" );
}

void KoDocument::addToChildList( KOffice::Document_ptr _child, const char *_url )
{
  m_lstAllChildren.append( KoDocument::SimpleDocumentChild( _child, _url ) );
}

bool KoDocument::saveChildren( KOStore::Store_ptr _store )
{
  QValueList<KoDocument::SimpleDocumentChild>::Iterator it;
  for( it = m_lstAllChildren.begin(); it != m_lstAllChildren.end(); ++it )
  {
    kdebug( KDEBUG_INFO, 30003, "Saving child %c", (*it).url().ascii() );

    KURL u( (*it).url() );
    // Do we have to save this child embedded ?
    if ( u.protocol() != "store" )
      continue;

    // Save it as child in the document store
    KOffice::Document_var doc = (*it).document();
    if ( !doc->saveToStore( _store, 0 ) )
      return false;

    kdebug( KDEBUG_INFO, 30003, "Saved child %c", (*it).url().ascii() );
  }

  m_lstAllChildren.clear();

  return true;
}

CORBA::Boolean KoDocument::saveToURL( const char *_url, const char* _format )
{
  KURL u( _url );
  if ( u.isMalformed() )
  {
    kdebug( KDEBUG_INFO, 30003, "malformed URL" );
    return false;
  }

  if ( !u.isLocalFile() )
  {
    QMessageBox::critical( (QWidget*)0L, i18n("KOffice Error"), i18n( "Can not save to remote URL\n" ), i18n( "OK" ) );
    return false;
  }

  if ( hasToWriteMultipart() )
  {
    kdebug( KDEBUG_INFO, 30003, "Saving to store" );

    KoStore store( u.path(), KOStore::Write );
    // TODO: Check for error

    makeChildListIntern();

    CORBA::String_var mime = mimeType();
    store.open( "root", mime.in() );
    
    QBuffer buffer;
    buffer.open( IO_WriteOnly );
    if ( !save( &buffer, &store, _format ) )
    {
      store.close();
      return false;
    }
    store.close();

    kdebug( KDEBUG_INFO, 30003, "Saving children" );

    // Lets write all direct and indirect children
    if ( !saveChildren( &store ) )
      return false;

    if ( !completeSaving( &store ) )
      return false;

    kdebug( KDEBUG_INFO, 30003, "Saving done" );
  }
  else
  {
    QFile file( _url );
    if ( !file.open( IO_WriteOnly ) )
      return false;

    if ( !save( &file, 0, _format ) )
      return false;

    // Lets write all direct and indirect children
    // We have no store so they have to be written
    // to external files.
    if ( !saveChildren( 0L ) )
      return false;

    if ( !completeSaving( 0L ) )
      return false;
  }

  return true;
}

CORBA::Boolean KoDocument::saveToStore( KOStore::Store_ptr _store, const char *_format )
{
  kdebug( KDEBUG_INFO, 30003, "Saving document to store" );

  CORBA::String_var mime = mimeType();
  CORBA::String_var u = url();

  _store->open( u, mime );

  QBuffer buffer;
  buffer.open( IO_WriteOnly );
  if ( !save( &buffer, _store, _format ) )
  {
    _store->close();
    return false;
  }

  _store->close();

  // Lets write all direct and indirect children
  if ( !saveChildren( _store ) )
    return false;

  if ( !completeSaving( _store ) )
    return false;

  kdebug( KDEBUG_INFO, 30003, "Saved document to store" );

  return true;
}

CORBA::Boolean KoDocument::loadFromURL( const char *_url, const char * )
{
  KURL u( _url );
  if ( u.isMalformed() )
  {
    kdebug( KDEBUG_INFO, 30003, "Malformed URL %c", _url );
    return false;
  }

  if ( !u.isLocalFile() )
  {
    kdebug( KDEBUG_INFO, 30003, "Can not load from remote URL currently" );
    return false;
  }

  setURL( _url );

  // Try to find out wether it is a pure XML file or a KoStore
  QFile file( u.path() );
  if ( !file.open( IO_ReadOnly ) )
  {
    kdebug( KDEBUG_INFO, 30003, "Could not open %c", u.path().ascii() );
    return false;
  }
  char buf[5];
  file.readBlock( buf, 4 );
  file.close();

  kdebug( KDEBUG_INFO, 30003, "PATTERN=%c", buf );

  // Is it a koffice store ?
  if ( strncasecmp( buf, "KS01", 4 ) == 0 )
  {
    KoStore store( u.path(), KOStore::Read );
    // TODO: Check for errors
    store.open( "root", 0L );
    int size = store.size();
    QByteArray buffer( size );
    store.read( buffer.data(), size );

    if ( !load( buffer, &store ) )
      return false;

    store.close();

    // Load the children from the store
    if ( !loadChildren( &store ) )
    {	
      kdebug( KDEBUG_INFO, 30003, "ERROR: Could not load children" );
      return false;
    }

    return completeLoading( &store );
  }
  // Standalone XML or some binary data
  else
  {
    QFile file( u.path() );
    if ( !file.open( IO_ReadOnly ) )
      return false;
    uint size = file.size();
    QByteArray buffer( size );
    file.readBlock( buffer.data(), size );
    file.close();

    if ( !load( buffer, 0 ) )
      return false;

    return completeLoading( 0 );
  }
}

CORBA::Boolean KoDocument::loadFromStore( KOStore::Store_ptr _store, const char *_url )
{
  setURL( _url );

  _store->open( _url, 0L );
  CORBA::Long size = _store->size();
  QByteArray buffer( size );

  // A special scope to get rid of "data"
  {
    KOStore::Data_var data = _store->read( size );
    CORBA::ULong len = data->length();
    
    char* p = buffer.data();
    for( CORBA::ULong i = 0; i < len; ++i )
      *p++ = data[ i ];
  }
  _store->close();

  if ( !load( buffer, _store ) )
    return false;

  if ( !loadChildren( _store ) )
  {	
    kdebug( KDEBUG_INFO, 30003, "ERROR: Could not load children" );
    return false;
  }

  return completeLoading( _store );
}

bool KoDocument::load( QByteArray& buffer, KOStore::Store_ptr _store )
{
  // Load XML ?
  if (buffer.size() > 5 && strncasecmp( buffer.data(), "<?xml", 5 ) == 0 )
  {
    QDomDocument doc;
    if ( !doc.setContent( QString::fromUtf8( buffer.data(), buffer.size() ) ) )
      return false;

    if ( !loadXML( doc, _store ) )
      return false;
  }
  // Load binary data
  else
  {
    if ( !loadBinary( buffer, _store ) )
      return false;
  }

  return true;
}

void KoDocument::setURL( const char *_url )
{
  m_strURL = _url;
}

char* KoDocument::url()
{
  if ( m_strURL.isEmpty() )
    return CORBA::string_dup( "" );

  return CORBA::string_dup( m_strURL );
}
