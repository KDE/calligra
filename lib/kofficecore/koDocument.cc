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

#include <fstream>

#ifdef KDE_USE_FINAL
#include <CUTE.h> // include first, to get POA stuff (hack)
#endif

#include "koDocument.h"
#include "koStream.h"
#include "koQueryTypes.h"

#include <koBinaryStore.h>
#include <koTarStore.h>
#include <koStoreStream.h>

#include <komlWriter.h>
#include <komlMime.h>
#include <komlStreamFeed.h>

#include <kurl.h>
#include <klocale.h>
#include <kapp.h>

#include <qmsgbox.h>
#include <qpainter.h>
#include <qcolor.h>
#include <qpicture.h>

// Define the protocol used here for embedded documents' URL
// This used to "store:" but KURL didn't like it, 
// so let's simply make it "tar:" !
#define STORE_PROTOCOL "tar:"
#define STORE_PROTOCOL_LENGTH 4
// Warning, keep it sync in koTarStore.cc

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
  m_strMimeType = m_rDoc->mimeType();
  m_geometry = _rect;
  m_pPicture = 0L;
  m_bHasPrintingExtension = false;
}

KoDocumentChild::KoDocumentChild()
{
  m_rDoc = 0L;
  m_pPicture = 0L;
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
      kdebug( KDEBUG_FATAL, 30003, "Ouch! We did not get a view!!!" );
      exit(1);
    }
  }
  catch ( OpenParts::Document::MultipleViewsNotSupported &_ex )
  {
    // HACK
    kdebug( KDEBUG_ERROR, 30003, "void KSpreadView::slotInsertObject( const QRect& _rect, OPParts::Document_ptr _doc )" );
    kdebug( KDEBUG_ERROR, 30003, "Could not create view - KoDocumentChild::createView" );
    return 0L;
  }

  if ( CORBA::is_nil( v ) )
  {
    kdebug( KDEBUG_ERROR, 30003, "void KSpreadView::slotInsertObject( const QRect& _rect, OPParts::Document_ptr _doc )" );
    kdebug( KDEBUG_ERROR, 30003, "return value is 0L - KoDocumentChild::createView" );
    return 0L;
  }

  v->setMode( KOffice::View::ChildMode );
  v->setMainWindow( _main );

  return KOffice::View::_duplicate( v );
}

bool KoDocumentChild::load( KOMLParser& parser, vector<KOMLAttrib>& _attribs )
{
  vector<KOMLAttrib>::const_iterator it = _attribs.begin();
  for( ; it != _attribs.end(); it++ )
  {
    if ( (*it).m_strName == "url" )
    {
      m_strURL = (*it).m_strValue.c_str();
    }
    else if ( (*it).m_strName == "mime" )
    {
      m_strMimeType = (*it).m_strValue.c_str();
    }
    else
      kdebug( KDEBUG_INFO, 30003, "Unknown attrib 'OBJECT:%s'", (*it).m_strName.c_str() );
  }

  if ( m_strURL.isEmpty() )
  {	
    kdebug( KDEBUG_INFO, 30003, "Empty 'url' attribute in OBJECT" );
    return false;
  }
  else if ( m_strMimeType.isEmpty() )
  {
    kdebug( KDEBUG_INFO, 30003, "Empty 'mime' attribute in OBJECT" );
    return false;
  }

  string tag;
  vector<KOMLAttrib> lst;
  string name;

  bool brect = false;

  // RECT
  while( parser.open( 0L, tag ) )
  {
    KOMLParser::parseTag( tag.c_str(), name, lst );

    if ( name == "RECT" )
    {
      brect = true;
      m_geometry = tagToRect( lst );
    }
    else
      kdebug( KDEBUG_INFO, 30003, "Unknown tag '%s' in OBJECT", tag.c_str() );

    if ( !parser.close( tag ) )
    {
      kdebug( KDEBUG_INFO, 30003, "ERR: Closing Child in OBJECT" );
      return false;
    }
  }

  if ( !brect )
  {
    kdebug( KDEBUG_INFO, 30003, "Missing RECT in OBJECT" );
    return false;
  }

  return true;
}

bool KoDocumentChild::loadDocument( KOStore::Store_ptr _store )
{
  assert( !m_strURL.isEmpty() );

  kdebug( KDEBUG_INFO, 30003, "Trying to load %s", m_strURL.ascii() );

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

  if ( m_strURL.left( STORE_PROTOCOL_LENGTH ) == STORE_PROTOCOL )
    return m_rDoc->loadFromStore( _store, m_strURL );

  return m_rDoc->loadFromURL( m_strURL );
}

bool KoDocumentChild::save( ostream& out )
{
  QString u = m_rDoc->url();
  QString mime = m_rDoc->mimeType();

  out << indent << "<OBJECT url=\"" << u.ascii() << "\" mime=\"" << mime.ascii() << "\">"
      << m_geometry << "</OBJECT>" << endl;

  return true;
}

bool KoDocumentChild::isStoredExtern()
{
  QString url = m_rDoc->url();
  if ( url.isEmpty() )
    return false;
  if ( url.left( STORE_PROTOCOL_LENGTH ) == STORE_PROTOCOL )
    return false;

  return true;
}

QPicture* KoDocumentChild::draw( float _scale, bool _force_update )
{
  kdebug( KDEBUG_INFO, 30003, "QPicture* KoDocumentChild::draw( bool _force )" );

  // No support for printing extension? => return white plane
  if ( m_pPicture != 0L && !m_bHasPrintingExtension )
    return m_pPicture;

  if ( m_pPicture != 0L && m_pictureScale == _scale && !_force_update )
    return m_pPicture;

  kdebug( KDEBUG_INFO, 30003, "Trying to fetch the QPicture stuff" );

  CORBA::Object_var obj = m_rDoc->getInterface( "IDL:KOffice/Print:1.0" );
  if ( CORBA::is_nil( obj ) )
  {
    if ( m_pPicture == 0L )
      m_pPicture = new QPicture;
    m_pictureScale = _scale;

    // Draw a white area instead
    kdebug( KDEBUG_INFO, 30003, "KOffice::Print not supported" );
    QPainter painter;
    painter.begin( m_pPicture );

    painter.fillRect( 0, 0, m_geometry.width(), m_geometry.height(), Qt::white );
    painter.end();
    return m_pPicture;
  }

  KOffice::Print_var print = KOffice::Print::_narrow( m_rDoc );
  if ( CORBA::is_nil( print ) )
  {
    if ( m_pPicture == 0L )
      m_pPicture = new QPicture;
    m_pictureScale = _scale;

    // Draw a white area instead
    kdebug( KDEBUG_INFO, 30003, "ERROR: Could not narrow to OPParts::Print" );
    QPainter painter;
    painter.begin( m_pPicture );

    painter.fillRect( 0, 0, m_geometry.width(), m_geometry.height(), Qt::white );
    painter.end();
    return m_pPicture;
  }

  kdebug( KDEBUG_INFO, 30003, "Fetching data" );
  QString str( print->encodedMetaFile( m_geometry.width(), m_geometry.height(),
						 _scale ) );
  kdebug( KDEBUG_INFO, 30003, "Fetched data" );

  int inlen = strlen( str );

  if ( inlen % 4 != 0 )
  {
    kdebug( KDEBUG_INFO, 30003, "ERROR: len of BASE64 not devideable by 4" );

    if ( m_pPicture == 0L )
      m_pPicture = new QPicture;
    m_pictureScale = _scale;

    QPainter painter;
    painter.begin( m_pPicture );

    painter.fillRect( 0, 0, m_geometry.width(), m_geometry.height(), Qt::white );
    painter.end();

    return m_pPicture;
  }

  kdebug( KDEBUG_INFO, 30003, "Base64 bytes are %i", inlen );

  Base64 b;
  char *p = new char[ inlen * 3 / 4 + 10 ];
  const char *src = static_cast<const char*>(str);
  int anz = inlen / 4;
  int got = 0;
  for( int i = 0; i < anz; i++ )
  {
    got += b.decode( p + got, src[ 0 ], src[ 1 ], src[ 2 ], src[ 3 ] );
    src += 4;
  }

  kdebug( KDEBUG_INFO, 30003, "GOT %i bytes", got );

  m_bHasPrintingExtension = true;

  if ( m_pPicture == 0L )
    m_pPicture = new QPicture;
  m_pictureScale = _scale;

  m_pPicture->setData( p, got );
  delete p;

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

  OPDocumentIf::cleanUp();
}

bool KoDocument::saveChildren( KOStore::Store_ptr /*_store*/, const char */*_path*/ ) 
{
  // Lets assume that we do not have children
  kdebug( KDEBUG_WARN, 30003, "KoDocument::saveChildren( KOStore::Store_ptr, const char * )");
  kdebug( KDEBUG_WARN, 30003, "Not implemented ( not really an error )" );
  return true;
}

bool KoDocument::saveToURL( const QString &_url, const QCString &_format )
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

    //Use this to save to a binary store (deprecated)
    //KoStore * store = new KoBinaryStore ( u.path(), KOStore::Write );

    KoStore * store = new KoTarStore( u.path(), KOStore::Write );

    // Save childen first since they might get a new url
    if ( store->bad() || !saveChildren( store, STORE_PROTOCOL ) )
    {
      delete store;
      return false;
    }

    kdebug( KDEBUG_INFO, 30003, "Saving root" );
    if ( store->open( "root", _format ) )
    {
      ostorestream out( store );
      if ( !save( out, _format ) )
      {
	store->close();
	return false;
      }
      out.flush();
      store->close();
    } else
      return false;

    bool ret = completeSaving( store );
    kdebug( KDEBUG_INFO, 30003, "Saving done" );
    delete store;
    return ret;
  }
  else
  {
    ofstream out( u.path() );
    if ( !out )
    {
      QString tmp = i18n("Could not write to\n" );
      tmp += u.path();
      kdebug( KDEBUG_INFO, 30003, tmp );
      QMessageBox::critical( (QWidget*)0L, i18n("KOffice Error"), tmp, i18n( "OK" ) );
      return false;
    }

    return save( out, _format );
  }
}

bool KoDocument::saveToStore( KOStore::Store_ptr _store, const QCString & _format, const QString & _path )
{
  kdebug( KDEBUG_INFO, 30003, "Saving document to store" );

  // Use the path as the internal url
  setURL( _path );

  // Save childen first since they might get a new url
  if ( !saveChildren( _store, _path ) )
    return false;

  QString u = url();
  if ( _store->open( u, _format ) )
  {
    ostorestream out( _store );
    if ( !save( out, _format ) )
      return false;
    out.flush();
    _store->close();
  }

  if ( !completeSaving( _store ) )
    return false;

  kdebug( KDEBUG_INFO, 30003, "Saved document to store" );

  return true;
}

bool KoDocument::loadFromURL( const QString & _url )
{
  kdebug( KDEBUG_INFO, 30003, "KoDocument::loadFromURL( %s )", _url.ascii() );
  KURL u( _url );
  if ( u.isMalformed() )
  {
    kdebug( KDEBUG_INFO, 30003, "Malformed URL %s", _url.ascii() );
    return false;
  }

  if ( !u.isLocalFile() )
  {
    kdebug( KDEBUG_INFO, 30003, "Can not save to remote URL" );
    return false;
  }

  ifstream in( u.path() );
  if ( !in )
  {
    kdebug( KDEBUG_INFO, 30003, "Could not open %s", u.path().ascii() );
    return false;
  }

  // Try to find out whether it is a mime multi part file
  char buf[5];
  in.get( buf[0] ); in.get( buf[1] ); in.get( buf[2] ); in.get( buf[3] ); buf[4] = 0;
  in.unget(); in.unget(); in.unget(); in.unget();

  kdebug( KDEBUG_INFO, 30003, "PATTERN=%s", buf );

  setURL( _url );

  // Is it plain XML ?
  if ( strncasecmp( buf, "<?xm", 4 ) == 0 )
  {
    bool res = load( in, 0L );
    in.close();
    if ( !res )
      return false;

    return completeLoading( 0L );
  } else
  { // It's a koffice store (binary or tar.gz)
    in.close();
    KoStore * store;
    if ( strncasecmp( buf, "KS01", 4 ) == 0 )
    {
      store = new KoBinaryStore( u.path(), KOStore::Read );
    }
    else // new (tar.gz)
    {
      store = new KoTarStore( u.path(), KOStore::Read );
    }

    if ( store->bad() )
    {
      delete store;
      return false;
    }

    if ( store->open( "root", "" ) )
    {
      istorestream in( store );
      if ( !load( in, store ) )
      {
        delete store;
        return false;
      }
      store->close();
    }
      
    if ( !loadChildren( store ) )
    {	
      kdebug( KDEBUG_INFO, 30003, "ERROR: Could not load children" );
      return false;
    }

    return completeLoading( store );
  }
}

bool KoDocument::loadFromStore( KOStore::Store_ptr _store, const QString & _url )
{
  if ( _store->open( _url, "" ) )
  {
    istorestream in( _store );
    if ( !load( in, _store ) )
      return false;
    _store->close();
  }
  setURL( _url );

  if ( !loadChildren( _store ) )
  {	
    kdebug( KDEBUG_INFO, 30003, "ERROR: Could not load children" );
    return false;
  }

  return completeLoading( _store );
}

bool KoDocument::load( istream& in, KOStore::Store_ptr _store )
{
  kdebug( KDEBUG_INFO, 30003, "KoDocument::load( istream& in, KOStore::Store_ptr _store )");
  // Try to find out whether it is a mime multi part file
  char buf[5];
  in.get( buf[0] ); in.get( buf[1] ); in.get( buf[2] ); in.get( buf[3] ); buf[4] = 0;
  in.unget(); in.unget(); in.unget(); in.unget();

  kdebug( KDEBUG_INFO, 30003, "PATTERN2=%s", buf );

  // Load XML ?
  if ( strncasecmp( buf, "<?xm", 4 ) == 0 )
  {
    KOMLStreamFeed feed( in );
    KOMLParser parser( &feed );

    if ( !loadXML( parser, _store ) )
      return false;
  }
  // Load binary data
  else
  {
    if ( !loadBinary( in, false, _store ) )
      return false;
  }

  return true;
}

bool KoDocument::isStoredExtern()
{
  return ( m_strURL.left( STORE_PROTOCOL_LENGTH ) != STORE_PROTOCOL );
}
