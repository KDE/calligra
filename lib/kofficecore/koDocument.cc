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

#include "koDocument.h"
#include "koApplication.h"
#include "koStream.h"
#include "koQueryTypes.h"
#include "koFilterManager.h"

#include <koStore.h>
#include <koBinaryStore.h>
#include <koTarStore.h>
#include <koStoreStream.h>

#include <komlWriter.h>
#include <komlMime.h>
#include <komlStreamFeed.h>

#include <klocale.h>
#include <kapp.h>
#include <kdebug.h>
#include <kmessagebox.h>

#include <qpainter.h>
#include <qcolor.h>
#include <qpicture.h>
#include <qdom.h>

// Define the protocol used here for embedded documents' URL
// This used to "store:" but KURL didn't like it,
// so let's simply make it "tar:" !
#define STORE_PROTOCOL "tar:"
#define STORE_PROTOCOL_LENGTH 4
// Warning, keep it sync in koTarStore.cc


/**********************************************************
 *
 * KoDocumentChild
 *
 **********************************************************/

KoDocumentChild::KoDocumentChild( KoDocument* parent, KoDocument* doc, const QRect& geometry )
    : PartChild( parent, doc, geometry )
{
}

KoDocumentChild::KoDocumentChild( KoDocument* parent )
    : PartChild( parent )
{
}

bool KoDocumentChild::load( KOMLParser& parser, vector<KOMLAttrib>& _attribs )
{
  vector<KOMLAttrib>::const_iterator it = _attribs.begin();
  for( ; it != _attribs.end(); it++ )
  {
    if ( (*it).m_strName == "url" )
    {
      m_tmpURL = (*it).m_strValue.c_str();
    }
    else if ( (*it).m_strName == "mime" )
    {
      m_tmpMimeType = (*it).m_strValue.c_str();
    }
    else
      kDebugInfo( 30003, "Unknown attrib 'OBJECT:%s'", (*it).m_strName.c_str() );
  }

  if ( m_tmpURL.isEmpty() )
  {	
    kDebugInfo( 30003, "Empty 'url' attribute in OBJECT" );
    return false;
  }
  else if ( m_tmpMimeType.isEmpty() )
  {
    kDebugInfo( 30003, "Empty 'mime' attribute in OBJECT" );
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
      m_tmpGeometry = tagToRect( lst );
      setGeometry( m_tmpGeometry );
    }
    else
      kDebugInfo( 30003, "Unknown tag '%s' in OBJECT", tag.c_str() );

    if ( !parser.close( tag ) )
    {
      kDebugInfo( 30003, "ERR: Closing Child in OBJECT" );
      return false;
    }
  }

  if ( !brect )
  {
    kDebugInfo( 30003, "Missing RECT in OBJECT" );
    return false;
  }

  return true;
}

bool KoDocumentChild::load( const QDomElement& element )
{
    if ( element.hasAttribute( "url" ) )
	m_tmpURL = element.attribute("url");
    if ( element.hasAttribute("mime") )
	m_tmpMimeType = element.attribute("mime");

    if ( m_tmpURL.isEmpty() )
    {	
	kDebugInfo( 30003, "Empty 'url' attribute in OBJECT" );
	return false;
    }
    if ( m_tmpMimeType.isEmpty() )
    {
	kDebugInfo( 30003, "Empty 'mime' attribute in OBJECT" );
	return false;
    }

    bool brect = FALSE;
    QDomElement e = element.firstChild().toElement();
    for( ; !e.isNull(); e = e.nextSibling().toElement() )
    {
	if ( e.tagName() == "rect" )
        {
	    brect = true;
	    m_tmpGeometry = e.toRect();
	}
    }

    if ( !brect )
    {
	kDebugInfo( 30003, "Missing RECT in OBJECT" );
	return false;
    }

    return true;
}

bool KoDocumentChild::loadTag( KOMLParser&, const string&, vector<KOMLAttrib>& )
{
    return FALSE;
}

bool KoDocumentChild::loadDocument( KoStore* _store )
{
  assert( !m_tmpURL.isEmpty() );

  kDebugInfo( 30003, QString("Trying to load %1").arg(m_tmpURL) );

  KoDocumentEntry e = KoDocumentEntry::queryByMimeType( m_tmpMimeType );
  if ( e.isEmpty() )
  {
    kDebugInfo( 30003, "ERROR: Could not create child document" );
    return false;
  }

  // ######## Torben: Do some error handling on createDoc here
  setPart( e.createDoc( (KoDocument*)parent() ), m_tmpGeometry );

  bool res;
  if ( m_tmpURL.left( STORE_PROTOCOL_LENGTH ) == STORE_PROTOCOL )
      res = document()->loadFromStore( _store, m_tmpURL );
  else
  {
      // Reference to an external document. Hmmm...
      res = document()->loadFromURL( m_tmpURL );
      // Still waiting...
      QApplication::setOverrideCursor( waitCursor );
  }

  m_tmpURL = QString::null;

  return res;
}

QDomElement KoDocumentChild::save( QDomDocument& doc )
{
    QDomElement e = doc.createElement( "object" );
    e.setAttribute( "url", document()->url().url() );
    e.setAttribute( "mime", document()->mimeType() );
    QDomElement rect = doc.createElement( "rect" );
    rect.setAttribute( "x", geometry().left() );
    rect.setAttribute( "y", geometry().top() );
    rect.setAttribute( "w", geometry().width() );
    rect.setAttribute( "h", geometry().height() );
    e.appendChild(rect);
    return e;
}

bool KoDocumentChild::save( ostream& out )
{
  QString u = document()->url().url();
  QString mime = document()->mimeType();

  out << indent << "<OBJECT url=\"" << u.ascii() << "\" mime=\"" << mime.ascii() << "\">"
      << geometry() << "</OBJECT>" << endl;

  return true;
}

bool KoDocumentChild::isStoredExtern()
{
  const KURL & url = document()->url();
  if ( !url.hasPath() )
    return false;
  if ( url.protocol() == STORE_PROTOCOL )
    return false;

  return true;
}

KURL KoDocumentChild::url()
{
    return ( document() ? document()->url() : KURL() );
}

KoDocumentChild::~KoDocumentChild()
{
}

/**********************************************************
 *
 * KoDocument
 *
 **********************************************************/

KoDocument::KoDocument( QObject* parent, const char* name )
    : ContainerPart( parent, name )
{
    m_bModified = FALSE;
    m_bEmpty = TRUE;
}

bool KoDocument::saveChildren( KoStore* /*_store*/, const char */*_path*/ )
{
  // Lets assume that we do not have children
  kDebugWarning( 30003, "KoDocument::saveChildren( KoStore*, const char * )");
  kDebugWarning( 30003, "Not implemented ( not really an error )" );
  return true;
}

bool KoDocument::saveToURL( const KURL &url, const QCString &_format )
{
  if ( url.isMalformed() )
  {
    kDebugInfo( 30003, "malformed URL" );
    return false;
  }

  if ( !url.isLocalFile() )
  {
    KMessageBox::error( 0L, i18n( "Can not save to remote URL (not implemented yet)" ) );
    return false;
  }

  if ( hasToWriteMultipart() )
  {
    kDebugInfo( 30003, "Saving to store" );

    //Use this to save to a binary store (deprecated)
    //KoStore * store = new KoBinaryStore ( url.path(), KOStore::Write );

    KoStore* store = new KoTarStore( url.path(), KoStore::Write );

    // Save childen first since they might get a new url
    if ( store->bad() || !saveChildren( store, STORE_PROTOCOL ) )
    {
      delete store;
      return false;
    }

    kDebugInfo( 30003, "Saving root" );
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
    }
    else
      return false;

    bool ret = completeSaving( store );
    kDebugInfo( 30003, "Saving done" );
    delete store;
    return ret;
  }
  else
  {
    ofstream out( url.path() );
    if ( !out )
    {
      KMessageBox::error( 0L, i18n("Could not write to\n%1" ).arg(url.path()) );
      return false;
    }

    return save( out, _format );
  }
}

bool KoDocument::saveToStore( KoStore* _store, const QCString & _format, const QString & _path )
{
  kDebugInfo( 30003, "Saving document to store" );

  // Use the path as the internal url
  setURL( _path );

  // Save childen first since they might get a new url
  if ( !saveChildren( _store, _path ) )
    return false;

  QString u = url().url();
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

  kDebugInfo( 30003, "Saved document to store" );

  return true;
}

bool KoDocument::loadFromURL( const KURL & url )
{
  kDebugInfo( 30003, QString("KoDocument::loadFromURL( %1 )").arg(url.url()) );
  // TODO : assert local url
  QString file = url.path();

  QApplication::setOverrideCursor( waitCursor );
  // Launch a filter if we need one for this url ?
  QString importedFile = KoFilterManager::self()->import( file, KOAPP->nativeFormatMimeType() );

  // The filter, if any, has been applied. It's all native format now.
  bool loadOk = (!importedFile.isEmpty()) &&        // Empty = an error occured in the filter
    loadNativeFormat( importedFile );

  if (!loadOk)
    KMessageBox::error( 0L, i18n( "Could not open\n%1" ).arg(importedFile) );

  if ( importedFile != file )
  {
    // We opened a temporary file (result of an import filter)
    // Set document URL to empty - we don't want to save in /tmp !
    setURL(KURL());
    // and remove temp file
    unlink( importedFile.ascii() );
  }
  QApplication::restoreOverrideCursor();
  return loadOk;
}

bool KoDocument::loadNativeFormat( const KURL & url )
{
  QApplication::setOverrideCursor( waitCursor );

  kDebugInfo( 30003, QString("KoDocument::loadNativeFormat( %1 )").arg(url.url()) );

  if ( url.isMalformed() )
  {
    kDebugWarning( 30003, QString("Malformed URL %1").arg(url.url()) );
    QApplication::restoreOverrideCursor();
    return false;
  }

  if ( !url.isLocalFile() )
  {
    kDebugInfo( 30003, "Can not load remote URL (not implemented yet)" );
    QApplication::restoreOverrideCursor();
    return false;
  }

  ifstream in( url.path() );
  if ( !in )
  {
    kDebugWarning( 30003, QString("Could not open %1").arg(url.path()) );
    QApplication::restoreOverrideCursor();
    return false;
  }

  // Try to find out whether it is a mime multi part file
  char buf[5];
  in.get( buf[0] ); in.get( buf[1] ); in.get( buf[2] ); in.get( buf[3] ); buf[4] = 0;
  in.unget(); in.unget(); in.unget(); in.unget();

  //kDebugInfo( 30003, "PATTERN=%s", buf );

  // Store the URL as the Document URL
  setURL( url );

  // Is it plain XML ?
  if ( strncasecmp( buf, "<?xm", 4 ) == 0 )
  {
    bool res = load( in, 0L );
    in.close();
    if ( res )
      res = completeLoading( 0L );

    QApplication::restoreOverrideCursor();
    return res;
  } else
  { // It's a koffice store (binary or tar.gz)
    in.close();
    KoStore * store;
    if ( strncasecmp( buf, "KS01", 4 ) == 0 )
    {
      store = new KoBinaryStore( url.path(), KoStore::Read );
    }
    else // new (tar.gz)
    {
      store = new KoTarStore( url.path(), KoStore::Read );
    }

    if ( store->bad() )
    {
      delete store;
      QApplication::restoreOverrideCursor();
      return false;
    }

    if ( store->open( "root", "" ) )
    {
      istorestream in( store );
      if ( !load( in, store ) )
      {
        delete store;
        QApplication::restoreOverrideCursor();
        return false;
      }
      store->close();
    }

    if ( !loadChildren( store ) )
    {	
      kDebugInfo( 30003, "ERROR: Could not load children" );
      delete store;
      QApplication::restoreOverrideCursor();
      return false;
    }

    bool res = completeLoading( store );
    delete store;
    QApplication::restoreOverrideCursor();
    return res;
  }
}

bool KoDocument::loadFromStore( KoStore* _store, const KURL & url )
{
  if ( _store->open( url.url(), "" ) )
  {
    istorestream in( _store );
    if ( !load( in, _store ) )
      return false;
    _store->close();
  }
  // Store as document URL
  setURL( url );

  if ( !loadChildren( _store ) )
  {	
    kDebugInfo( 30003, "ERROR: Could not load children" );
    return false;
  }

  return completeLoading( _store );
}

bool KoDocument::load( istream& in, KoStore* _store )
{
  kDebugInfo( 30003, "KoDocument::load( istream& in, KoStore* _store )");
  // Try to find out whether it is a mime multi part file
  char buf[5];
  in.get( buf[0] ); in.get( buf[1] ); in.get( buf[2] ); in.get( buf[3] ); buf[4] = 0;
  in.unget(); in.unget(); in.unget(); in.unget();

  kDebugInfo( 30003, "PATTERN2=%s", buf );

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
  return ( m_strURL.protocol() != STORE_PROTOCOL );
}

bool KoDocument::isModified() const
{
    return m_bModified;
}

void KoDocument::setModified( bool _mod )
{
    m_bModified = _mod;

    if ( m_bModified )
	m_bEmpty = FALSE;
}

bool KoDocument::isEmpty() const
{
    return m_bEmpty;
}

void KoDocument::setURL( const KURL& url )
{
    m_strURL = url;
}

const KURL & KoDocument::url() const
{
    return m_strURL;
}

bool KoDocument::loadBinary( istream& , bool, KoStore* )
{
    kdebug( KDEBUG_ERROR, 30003, "KoDocument::loadBinary not implemented" );
    return false;
}

bool KoDocument::loadXML( KOMLParser&, KoStore*  )
{
    kdebug( KDEBUG_ERROR, 30003, "KoDocument::loadXML not implemented" );
    return false;
}

bool KoDocument::loadChildren( KoStore* )
{
    return true;
}

bool KoDocument::completeLoading( KoStore* )
{
    return true;
}

bool KoDocument::completeSaving( KoStore* )
{
    return true;
}

bool KoDocument::save( ostream&, const char* )
{
    kdebug( KDEBUG_ERROR, 30003, "KoDocument::save not implemented" );
    return false;
}

QString KoDocument::copyright() const
{
    return "";
}

QString KoDocument::comment() const
{
    return "";
}

void KoDocument::insertChild( PartChild* child )
{
    m_bModified = TRUE;

    ContainerPart::insertChild( child );
}

bool KoDocument::hasToWriteMultipart()
{
    return FALSE;
}

#include "koDocument.moc"
