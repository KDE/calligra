#include <fstream>
#include <strstream>

#include "koDocument.h"
#include "koIMR.h"
#include "koStream.h"

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

KoDocumentChild::KoDocumentChild( const QRect& _rect, OPParts::Document_ptr _doc )
{
  m_rDoc = OPParts::Document::_duplicate( _doc );
  CORBA::String_var m = m_rDoc->mimeType();
  m_strMimeType = m;
  m_geometry = _rect;
  m_pPicture = 0L;
  m_bHasPrintingExtension = 0L;
}

KoDocumentChild::KoDocumentChild()
{
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

OPParts::View_ptr KoDocumentChild::createView( OPParts::PartShell_ptr _shell )
{
  OPParts::View_var v;

  try
  { 
    v = m_rDoc->createView();
    if( CORBA::is_nil( v ) )
    {
      cerr << "Shit! We did not get a view!!!" << endl;
      exit(1);
    }
  }
  catch ( OPParts::Document::MultipleViewsNotSupported &_ex )
  {
    // HACK
    cerr << "void KSpreadView::slotInsertObject( const QRect& _rect, OPParts::Document_ptr _doc )" << endl;
    cerr << "Could not create view" << endl;
    return 0L;
  }

  if ( CORBA::is_nil( v ) )
  {
    cerr << "void KSpreadView::slotInsertObject( const QRect& _rect, OPParts::Document_ptr _doc )" << endl;
    cerr << "return value is 0L" << endl;
    return 0L;
  }

  v->setMode( OPParts::Part::ChildMode );
  v->setPartShell( _shell );

  return OPParts::View::_duplicate( v );
} 

bool KoDocumentChild::load( KOMLParser& parser, vector<KOMLAttrib>& _attribs )
{
  vector<KOMLAttrib>::const_iterator it = _attribs.begin();
  for( ; it != _attribs.end(); it++ )
  {
    if ( (*it).m_strName == "src" )
    {
      m_strSource = (*it).m_strValue;
    }
    else if ( (*it).m_strName == "mime" )
    {
      m_strMimeType = (*it).m_strValue;
    }
    else
      cerr << "Unknown attrib 'OBJECT:" << (*it).m_strName << "'" << endl;
  }

  if ( m_strSource.empty() )
  {
    cerr << "Empty src attribute in OBJECT" << endl;
    return false;
  }
  else if ( m_strMimeType.empty() )
  {
    cerr << "Empty mime attribute in OBJECT" << endl;
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
      cerr << "Unknown tag '" << tag << "' in OBJECT" << endl;

    if ( !parser.close( tag ) )
    {
      cerr << "ERR: Closing Child in OBJECT" << endl;
      return false;
    }
  }

  if ( !brect )
  {
    cerr << "Missing RECT in OBJECT" << endl;
    return false;
  }
  
  return true;
}

bool KoDocumentChild::loadDocument( OPParts::MimeMultipartDict_ptr _dict )
{
  assert( !m_strSource.empty() );

  cout << "Trying to load " << m_strSource << endl;
  KURL u( m_strSource.c_str() );
  if ( strcmp( u.protocol(), "mime" ) != 0 )
  {
    if ( !loadDocument() )
    {
      cerr << "Could not load " << m_strSource << endl;
      return false;
    }
  }
  else
  {
    OPParts::MimeMultipartEntity_var e = _dict->find( m_strSource.c_str() );
    if ( CORBA::is_nil( e ) )
    {
      cerr << "Could not find id '" << m_strSource << "'" << endl;
      return false;
    }
    cerr << "Trying embedded" << endl;
    if ( !loadDocumentAsMimePart( _dict, e ) )
    {
      cerr << "Could not load embedded " << m_strSource << endl;
      return false;
    }
  }

  return true;
}
 
bool KoDocumentChild::loadDocument()
{
  m_rDoc = imr_createDocByMimeType( m_strMimeType.c_str() );
  if ( CORBA::is_nil( m_rDoc ) )
  {
    cerr << "ERROR: Could not create child document" << endl;
    return false;
  }
  m_rDoc->open( m_strSource.c_str() );
  
  return true;
}

bool KoDocumentChild::loadDocumentAsMimePart( OPParts::MimeMultipartDict_ptr _dict, OPParts::MimeMultipartEntity_ptr _e )
{
  cerr << "bool KoDocumentChild::loadDocumentAsMimePart( OPParts::MimeMultipartDict_ptr _dict, OPParts::MimeMultipartEntity_ptr _e )" << endl;
  
  cerr << "mime =\"" << m_strMimeType << endl;
  
  m_rDoc = imr_createDocByMimeType( m_strMimeType.c_str() );
  cerr << "Created Document" << endl;
  if ( CORBA::is_nil( m_rDoc ) )
  {
    cerr << "ERROR: Could not create child document with mime type " << m_strMimeType << endl;
    return false;
  }
  cerr << "1" << endl;
  CORBA::String_var id = _e->id();
  cerr << "2" << endl;
  m_rDoc->openMimePart( _dict, id );
  cerr << "3" << endl;
  return true;
}

bool KoDocumentChild::save( ostream& out )
{
  CORBA::String_var n = m_rDoc->id();
  CORBA::String_var mime = m_rDoc->mimeType();
  
  out << indent << "<OBJECT src=\"" << n << "\" mime=\"" << mime << "\">" << m_geometry << "</OBJECT>" << endl;

  return true;
}

bool KoDocumentChild::isStoredExtern()
{
  CORBA::String_var n = m_rDoc->id();
  string s = static_cast<const char*>(n);
  if ( s.empty() )
    return false;
  KURL u( s.c_str() );
  if ( strcmp( u.protocol(), "mime" ) == 0 )
    return false;

  return true;
}

QPicture* KoDocumentChild::draw( bool _force )
{
  cout << "QPicture* KoDocumentChild::draw( bool _force )" << endl;
  
  if ( m_pPicture != 0L && !m_bHasPrintingExtension && !_force )
    return 0L;

  if ( m_pPicture != 0L )
    return m_pPicture;
  
  cout << "Trying to fetch the QPicture stuff" << endl;
  
  CORBA::Object_var obj = m_rDoc->getInterface( "IDL:OPParts/Print:1.0" );
  if ( CORBA::is_nil( obj ) )
  {
    if ( !_force )
      return 0L;
  
    if ( m_pPicture == 0L )
      m_pPicture = new QPicture;
  
    // Draw a white area instead
    cout << "OPParts::Print not supported" << endl;
    QPainter painter;
    painter.begin( m_pPicture );
    
    painter.fillRect( 0, 0, m_geometry.width(), m_geometry.height(), white );
    painter.end();
    return m_pPicture;
  }

  OPParts::Print_var print = OPParts::Print::_narrow( m_rDoc );
  if ( CORBA::is_nil( print ) )
  {
    if ( !_force )
      return 0L;
  
    if ( m_pPicture == 0L )
      m_pPicture = new QPicture;

    // Draw a white area instead
    cerr << "ERROR: Could not narrow to OPParts::Print" << endl;
    QPainter painter;
    painter.begin( m_pPicture );
    
    painter.fillRect( 0, 0, m_geometry.width(), m_geometry.height(), white );
    painter.end();
    return m_pPicture;
  }
  
  cout << "Fetching data" << endl;
  CORBA::String_var str( print->encodedMetaFile( m_geometry.width(), m_geometry.height() ) );
  cout << "Fetched data" << endl;
  
  int inlen = strlen( str );

  if ( inlen % 4 != 0 )
  {
    cerr << "ERROR: len of BASE64 not devideable by 4" << endl;

    if ( !_force )
      return 0L;

    if ( m_pPicture == 0L )
      m_pPicture = new QPicture;

    QPainter painter;
    painter.begin( m_pPicture );
    
    painter.fillRect( 0, 0, m_geometry.width(), m_geometry.height(), white );
    painter.end();

    return m_pPicture;
  }

  cout << "Base64 bytes are " << inlen << endl;
  
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
  
  cout << "GOT " << got << " bytes" << endl;

  m_bHasPrintingExtension = true;

  if ( m_pPicture == 0L )
    m_pPicture = new QPicture;
  
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

CORBA::Boolean KoDocument::saveAs( const char *_url, const char *)
{
  KURL u( _url );
  if ( u.isMalformed() )
  {
    cerr << "malformed URL" << endl;
    return false;
  }
  
  if ( !u.isLocalFile() )
  {
    QMessageBox::critical( (QWidget*)0L, i18n("KOffice Error"), i18n( "Can not save to remote URL\n" ), i18n( "OK" ) );
    return false;
  }

  ofstream out( u.path() );
  if ( !out )
  {
    string tmp = i18n("Could not write to\n" );
    tmp += u.path();
    cerr << tmp << endl;
    QMessageBox::critical( (QWidget*)0L, i18n("KOffice Error"), tmp.c_str(), i18n( "OK" ) );
    return false;
  }

  string boundary = createBoundary();

  if ( hasToWriteMultipart() )
  {
    CORBA::String_var mime = mimeType();
    writeMagic( out, mime );
    writeMimeHeader( out, boundary.c_str() );
    
    out << "Das sollte jetzt das dict sein!" << endl;
    out << "--" << boundary << endl;
    
    writeBodyHeader( out, mime, 0L, "mime:/", 0L, copyright() );
    makeChildListIntern();
  }
  
  if ( !save( out ) )
    return false;

  if ( hasToWriteMultipart() )
  {
    out << "--" << boundary << endl;
    
    out.close();
    
    // Lets write all direct and indirect children
    if ( !saveChildren( u.path(), boundary.c_str() ) )
      return false;
    
    // Write the comment. This is a trick, since we need
    // "--[boundary]--" at the end
    ofstream o2( u.path(), ios::app | ios::out );
    writeBodyHeader( o2, "text/plain", 0L, "mime:/Comment", 0L, 0L );
    o2 << comment() << endl;
    o2 << "--" << boundary << "--" << endl;
  }

  return true;
}

CORBA::Boolean KoDocument::saveAsMimePart( const char *_url, const char *, const char *_boundary )
{
  KURL u( _url );
  if ( u.isMalformed() )
  {
    cerr << "malformed URL" << endl;
    return false;
  }
  
  if ( !u.isLocalFile() )
  {
    QMessageBox::critical( (QWidget*)0L, i18n("KOffice Error"), i18n( "Can not save to remote URL\n" ), i18n( "OK" ) );
    return false;
  }

  ofstream out( u.path(), ios::out | ios::app );
  if ( !out )
  {
    QString tmp;
    tmp.sprintf( i18n("Could not write to\n%s" ), u.path() );
    cerr << tmp << endl;
    QMessageBox::critical( (QWidget*)0L, i18n("KOffice Error"), tmp, i18n( "OK" ) );
    return false;
  }

  CORBA::String_var mime = mimeType();
  CORBA::String_var i = id();
  writeBodyHeader( out, mime, 0L, i, 0L, copyright() );

  if ( !save( out ) )
    return false;

  out << "--" << _boundary << endl;

  out.close();
  
  return true;
}

CORBA::Boolean KoDocument::open( const char *_url )
{
  KURL u( _url );
  if ( u.isMalformed() )
  {
    cerr << "Malformed URL " << _url << endl;
    return false;
  }
  
  if ( !u.isLocalFile() )
  {
    cerr << "Can not save to remote URL" << endl;
    return false;
  }

  ifstream in( u.path() );
  if ( !in )
  {
    cerr << "Could not open" << u.path() << endl;
    return false;
  }

  // Find out about the expected mime type
  CORBA::String_var mime = mimeType();  

  // Try to find out wether it is a mime multi part file
  char buf[5];
  in.get( buf[0] ); in.get( buf[1] ); in.get( buf[2] ); in.get( buf[3] ); buf[4] = 0;
  in.unget(); in.unget(); in.unget(); in.unget();

  cout << "PATTERN=" << buf << endl;
  
  // Standalone XML or some binary data ?  
  if ( strncasecmp( buf, "MIME", 4 ) != 0 &&
       strncasecmp( buf, "Magi", 4 ) != 0 )
  {
#if not_more
  /*
   * I've removed this code in order to use my own XML parser in 
   * my document class. But it shouldn't break the other implementations
   * because koDocument::load (istream&, bool) does (nearly) the same.
   * (kus)
   */
    // Load XML ?
    if ( strncasecmp( buf, "<?xm", 4 ) == 0 )
    {
      KOMLStreamFeed feed( in );
      KOMLParser parser( &feed );
  
      if ( !load( parser ) )
	return false;
    }
    // Load binary data
    else
#endif
      if ( !load( in, true ) )
	return false;
    
    cout << "!!!!!!!!!!!!!!!!!!!! CHILDREN !!!!!!!!!!!!!!!!!!!!" << endl;
    
    OPParts::MimeMultipartDict_var dict = new KOMLDict;
    
    if ( !loadChildren( dict ) )
    {	  
      cerr << "ERROR: Could not load children" << endl;
      return false;
    }
    
    return true;
  }
  // Mime Multipart ?
  else 
  {
    string magic;
    getline( in, magic );
    string pattern = "Magic-Line: ";
    pattern += mime;

    if ( strncasecmp( magic.c_str(), "MIME-Version: 1.0", 17 ) != 0 &&
	 strncasecmp( magic.c_str(), pattern.c_str(), pattern.length() ) != 0 )
    {
      cerr << "Unknown document format" << endl;
      return false;
    }
  }
  
  // Read Mime-Header
  KOMLHeaderParser hp( in );
  if ( !hp.parse() )
  {
    cerr << "Parse Error in MimeType Header" << endl;
    return false;
  }
  cout << "Type=" << hp.mimeType() << endl;
  cout << "Boundary=" << hp.boundary() << endl;
  cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
  
  // HACK
  // Skip the dict right now
  KOMLBodyIStream d( in, hp.boundary() );
  pump p( d, cout );
  p.run();
  
  cout << "!!!!!!!!!!!!!!!!!!!!! DICT !!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
  
  // Read in the dict of all bodies
  int pos = in.tellg();
  OPParts::MimeMultipartDict_var dict = new KOMLDict( in, u.path(), hp.boundary() );
  in.seekg( pos );
  
  cout << "!!!!!!!!!!!!!!!!!!!!! PARSE !!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
  
  // HACK
  // Assume the root doc follows as next part
  KOMLHeaderParser h( in );
  if ( !h.parse() )
  {
    cerr << "Parse Error in Body Part" << endl;
    return false;
  }
  cout << "Type=" << h.mimeType() << endl;
  cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
  if ( strcasecmp( h.mimeType(), mime.operator const char*() ) != 0L )
  {
    cerr << "Unknown MimeType " << h.mimeType() << endl;
    cerr << "Not supported" << endl;
    return false;
  }
  
  KOMLBodyIStream bs( in, hp.boundary() );

  bs.get( buf[0] ); bs.get( buf[1] ); bs.get( buf[2] ); bs.get( buf[3] ); buf[4] = 0;
  bs.unget(); bs.unget(); bs.unget(); bs.unget();
  
  // Load XML ?
  if ( strncasecmp( buf, "<?xm", 4 ) == 0 )
  {
    KOMLStreamFeed feed( bs );
    KOMLParser parser( &feed );
  
    if ( !load( parser ) )
      return false;
  }
  // Load binary data
  else
    if ( !load( bs, false ) )
      return false;
  
  cout << "!!!!!!!!!!!!!!!!!!!! CHILDREN !!!!!!!!!!!!!!!!!!!!" << endl;
  
  if ( !loadChildren( dict ) )
  {
    cerr << "ERROR: Could not load children" << endl;
    return false;
  }

  return true;
}

CORBA::Boolean KoDocument::openMimePart( OPParts::MimeMultipartDict_ptr _dict, const char *_id )
{
  CORBA::String_var mime = mimeType();

  OPParts::MimeMultipartEntity_var e = _dict->find( _id );
  if ( CORBA::is_nil( e ) )
  {
    cerr << "Unknown ID " << _id << endl;
    return false;
  }
  
  CORBA::String_var filename = _dict->filename();

  ifstream in( filename );
  if ( !in )
  {
    cerr << "Could not open" << filename << endl;
    return false;
  }

  CORBA::String_var bound = _dict->boundary();
  CORBA::Long start = e->start();

  in.seekg( start );

  // HACK
  // Assume the root doc follows as next part
  KOMLHeaderParser h( in );
  if ( !h.parse() )
  {
    cerr << "Parse Error in Body Part" << endl;
    return false;
  }
  cout << "Type=" << h.mimeType() << endl;
  cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
  if ( strcasecmp( h.mimeType(), mime.operator const char*() ) != 0L )
  {
    cerr << "Unknown MimeType " << h.mimeType() << endl;
    cerr << "Not supported" << endl;
    return false;
  }

  KOMLBodyIStream bs( in, bound );

  char buf[5];
  bs.get( buf[0] ); bs.get( buf[1] ); bs.get( buf[2] ); bs.get( buf[3] ); buf[4] = 0;
  bs.unget(); bs.unget(); bs.unget(); bs.unget();
  
#if not_more
  /*
   * I've removed this code in order to use my own XML parser in 
   * my document class. But it shouldn't break the other implementations
   * because koDocument::load (istream&, bool) does (nearly) the same.
   * (kus)
   */
  // Load XML ?
  if ( strncasecmp( buf, "<?xm", 4 ) == 0 )
  {
    KOMLStreamFeed feed( bs );
    KOMLParser parser( &feed );
  
    if ( !load( parser ) )
      return false;
  }
  // Load binary data
  else
#endif
    if ( !load( bs, false ) )
      return false;
  
  if ( !loadChildren( _dict ) )
  {
    cerr << "ERROR: Could not load children" << endl;
    return false;
  }
  
  return true;
}

bool KoDocument::load( istream &in, bool )
{
  KOMLStreamFeed feed( in );
  KOMLParser parser( &feed );
  
  if ( !load( parser ) )
    return false;

  return true;
}
