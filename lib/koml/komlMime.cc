#include "komlMime.h"

#include <string.h>
#include <assert.h>
#include <ctype.h>

KOMLHeaderParser::KOMLHeaderParser( istream &_str ) : m_stream( _str )
{
}
  
bool KOMLHeaderParser::parse()
{
  string line;
  
  do
  {
    if ( m_stream.eof() )
    {
      cerr << "Unexpected end of file" << endl;
      return false;
    }
    
    getline( m_stream, line );
    
    if ( line.empty() )
      return true;

    int i = line.find( ':' );
    if ( i < 0 )
      cerr << "Suspicious line: " << line << endl;
    else
    {
      string name, arg;
      name = line.substr( 0, i );
      i++;
      while( isspace( line[ i ] ) && i < static_cast<int>(line.length()) )
	i++;
      arg = line.substr( i, line.length() - i + 1 );

      m_mapHeader[ name ] = arg;

      if ( name == "Content-Type" )
      {
	int j = arg.find( ';' );
	if ( j >= 0 )
	{
	  m_strMimeType = arg.substr( 0, j );
	  j++;
	  while( isspace( arg[ j ] ) && i < static_cast<int>(arg.length()) )
	    j++;
	  if ( strncasecmp( "boundary=\"", arg.c_str() + j, 10 ) == 0 )
	  {
	    j += 10;
	    unsigned int cnt = 0;
	    while( arg[ j + cnt ] != '"' && j + cnt < arg.length() )
	      cnt++;
	    if ( j + cnt == arg.length() )
	      cerr << "Suspicious boundary" << endl;
	    else
	      m_strBoundary = arg.substr( j, cnt );
	  }
	  else if ( strncasecmp( "name=\"", arg.c_str() + j, 6 ) == 0 )
	  {
	    j += 6;
	    unsigned int cnt = 0;
	    while( arg[ j + cnt ] != '"' && j + cnt < arg.length() )
	      cnt++;
	    if ( j + cnt == arg.length() )
	      cerr << "Suspicious name" << endl;
	    else
	      m_strName = arg.substr( j, cnt );
	  }
	}
	else
	{
	  m_strMimeType = arg;
	}
      }
      
      // cout << name << endl << arg << endl;
    }
  } while( 1 );
}

KOMLBodyParser::KOMLBodyParser( istream& _str, const char *_boundary ) : m_stream( _str )
{
  m_strBoundary = _boundary;
}
  
KOMLBodyParser::EndMarker KOMLBodyParser::read( string &_str )
{
  if ( m_stream.eof() )
    return KOMLBodyParser::END;
  
  getline( m_stream, _str );
  if ( _str.length() >= 2 && _str[0] == '-' && _str[1] == '-' )
    if ( strncmp( m_strBoundary.c_str(), _str.c_str() + 2, m_strBoundary.length() ) == 0 )
    {
      if ( _str.length() >= m_strBoundary.length() + 4 && _str[ m_strBoundary.length() + 2 ] == '-' &&
	   _str[ m_strBoundary.length() + 3 ] == '-' )
	return KOMLBodyParser::END;
      else
	return KOMLBodyParser::STOP;
    }
  
  return KOMLBodyParser::OK;
}


void Base64::encode( char *_dest, unsigned char c1, unsigned char c2, unsigned char c3, int _len )
{
  /* Conversion table. */
  static char tbl[64] = {
    'A','B','C','D','E','F','G','H',
    'I','J','K','L','M','N','O','P',
    'Q','R','S','T','U','V','W','X',
    'Y','Z','a','b','c','d','e','f',
    'g','h','i','j','k','l','m','n',
    'o','p','q','r','s','t','u','v',
    'w','x','y','z','0','1','2','3',
    '4','5','6','7','8','9','+','/'
  };
  
  /* Transform the 3x8 bits to 4x6 bits, as required by
     base64.  */
  *_dest++ = tbl[ c1 >> 2 ];
  *_dest++ = tbl[ ( ( c1 & 3) << 4 ) | ( c2 >> 4 ) ];
  *_dest++ = tbl[ ( ( c2 & 0xf) << 2 ) | ( c3 >> 6 ) ];
  *_dest++ = tbl[ c3 & 0x3f ];
  *_dest = 0;
  
  if ( _len == 2 )
    *(_dest - 1) = '=';
  else if ( _len == 1 )
    *(_dest - 1) = *(_dest - 2) = '=';
}

int Base64::decode( char *_dest, unsigned char c1, unsigned char c2, unsigned char c3, unsigned char c4 )
{
  static unsigned char tbl[128] = {
    '\377','\377','\377','\377','\377','\377','\377','\377',
    '\377','\377','\377','\377','\377','\377','\377','\377',
    '\377','\377','\377','\377','\377','\377','\377','\377',
    '\377','\377','\377','\377','\377','\377','\377','\377',
    '\377','\377','\377','\377','\377','\377','\377','\377',
    '\377','\377','\377',    62,'\377','\377','\377',    63,
        52,    53,    54,    55,    56,    57,    58,    59,
        60,    61,'\377','\377','\377','\377','\377','\377',
    '\377',     0,     1,     2,     3,     4,     5,     6,
         7,     8,     9,    10,    11,    12,    13,    14,
        15,    16,    17,    18,    19,    20,    21,    22,
        23,    24,    25,'\377','\377','\377','\377','\377',
    '\377',    26,    27,    28,    29,    30,    31,    32,
        33,    34,    35,    36,    37,    38,    39,    40,
        41,    42,    43,    44,    45,    46,    47,    48,
        49,    50,    51,'\377','\377','\377','\377','\377'
  };

  int conv = 3;
  if ( c3 == '=' )
    conv = 1;
  else if ( c4 == '=' )
    conv = 2;
  
  c1 = tbl[ c1 ];
  c2 = tbl[ c2 ];
  c3 = tbl[ c3 ];
  c4 = tbl[ c4 ];

  if ( conv >= 1 )
    *_dest++ = ( c1 << 2 ) | ( ( c2 >> 4 ) & 3 );
  if ( conv >= 2 )
    *_dest++ = ( c2 << 4 ) | ( c3 >> 2 );
  if ( conv == 3 )
    *_dest++ = ( ( c3 & 3 ) << 6 ) | c4;

  return conv;
}

int Base64DecodeBuffer::underflow()
{
    // Leseposition vor Puffer-Ende ?
    if (gptr() < egptr() )
    {
      return *gptr();
    }

    if ( m_in.eof() )
    {
      cerr << "Unexpected end of input" << endl;
      m_bEnd = true;
    }
    
    if ( m_bEnd )
      return EOF;

    /* Anzahl Putback-Bereich ermitteln
     *  - Anzahl bereits gelesener Zeichen
     *  - maximal 4
     */
    int anzPutback;
    anzPutback = gptr() - eback();
    if ( anzPutback > 4 )
      anzPutback = 4;

    /* die bis zu vier vorher gelesenen Zeichen nach vorne
     * in den Putback-Puffer (erste 4 Zeichen)
     */
    memcpy( m_buffer + ( 4 - anzPutback ), gptr()-anzPutback, anzPutback );

    int want = ( m_iBufferSize - 4 ) * 4 / 3;
    char buf[ want ];
    
    int got = 0;
    while( !m_in.eof() && !m_bEnd && got < want )
    {
      char c = m_in.get();
      buf[ got ] = c;
      if ( c == '=' )
      {
	cout << "END OF BASE64" << endl;
	
	if ( got % 4 == 2 )
	{
	  if ( m_in.eof() )
	  {
	    cerr << "Unexpected EOF" << endl;
	    return EOF;
	  }
	  got++;
	  c = m_in.get();
	  if ( c != '=' )
	  {
	    cerr << "Not correct base64" << endl;
	    return EOF;
	  }
	  buf[ got++ ] = c;
	  m_bEnd = true;
	}
	else if ( got % 4 == 3 )
	{
	  got++;
	  m_bEnd = true;
	}
	else 
	{
	  cerr << "Unexpected =" << endl;
	  return EOF;
	}
      }
      else if ( !isspace( c ) )
	got++;
    }
    
    if( got % 4 != 0 )
    {
      cerr << "Unexpected EOF 2" << endl;
      return EOF;
    }
    
    int anz = 0;
    int cnt = got / 4;
    for( int j = 0; j < cnt; j++ )
    {
      int conv = decode( m_buffer + j * 3 + 4, buf[ j * 4 ], buf[ j * 4 + 1 ], buf[ j * 4 + 2 ], buf[ j * 4 + 3 ] );
      assert( conv == 3 || j == cnt - 1 );
      anz += conv;
    }
        
    /* Puffer-Zeiger neu setzen
     */
    setg ( m_buffer + ( 4 - anzPutback ),   // Putback-Anfang
	   m_buffer + 4,                // Leseposition
	   m_buffer + 4 + anz );           // Puffer-Ende

    // naechstes Zeichen zurueckliefern
    return *gptr();
}

int KOMLBodyIBuffer::underflow()
{
    // Leseposition vor Puffer-Ende ?
    if (gptr() < egptr() )
    {
      return *gptr();
    }

    if ( m_in.eof() )
      m_bEnd = true;
    
    if ( m_bEnd )
      return EOF;

    /* Anzahl Putback-Bereich ermitteln
     *  - Anzahl bereits gelesener Zeichen
     *  - maximal 4
     */
    int anzPutback;
    anzPutback = gptr() - eback();
    if ( anzPutback > 4 )
      anzPutback = 4;

    /* die bis zu vier vorher gelesenen Zeichen nach vorne
     * in den Putback-Puffer (erste 4 Zeichen)
     */
    memcpy( m_buffer + ( 4 - anzPutback ), gptr()-anzPutback, anzPutback );

    m_in.get( m_buffer + 4, m_iBufferSize - 4 );
    
    if ( m_bNewLine )
    {
      m_bNewLine = false;

      unsigned int len = strlen( m_buffer + 4 );
      if ( len >= 2 && m_buffer[ 4 ] == '-' && m_buffer[5] == '-' )
	if ( strncmp( m_strBoundary.c_str(), m_buffer + 4 + 2, m_strBoundary.length() ) == 0 )
	{
	  if ( len >= m_strBoundary.length() + 4 && m_buffer[ m_strBoundary.length() + 2 + 4 ] == '-' &&
	       m_buffer[ m_strBoundary.length() + 3 + 4 ] == '-' )
	    m_stream.setFileEnd();
	  else
	    m_stream.setBodyEnd();
	  m_bEnd = true;

	  // Read '\n'
	  if ( !m_in.eof() )
	    m_in.get();
	  
	  return EOF;
	}
    }

    int anz = m_in.gcount();
    if ( !m_in.eof() && !m_in.fail() )
    {
      char c = m_in.get();
      m_buffer[ 4 + anz++ ] = c;
      if ( c == '\n' )
	m_bNewLine = true;
    }
    else if ( !m_in.eof() && m_in.fail() )
      cin.clear( cin.rdstate() & ~ios::failbit );

    /* Puffer-Zeiger neu setzen
     */
    setg ( m_buffer + ( 4 - anzPutback ),   // Putback-Anfang
	   m_buffer + 4,                // Leseposition
	   m_buffer + 4 + anz );           // Puffer-Ende

    // naechstes Zeichen zurueckliefern
    return *gptr();
}

void writeMagic( ostream& outs, const char *_mime )
{
  outs << "Magic-Line: " << _mime << endl;
}

void writeMimeHeader( ostream& outs, const char *_boundary )
{
  outs << "MIME-Version: 1.0" << endl;
  outs << "Content-Type: MULTIPART/MIXED; BOUNDARY=\"" << _boundary << '"' << endl << endl;
}

/*
ostream& mimeHeader( ostream& outs )
{
  outs << "MIME-Version: 1.0" << endl;
  outs << "Content-Type: MULTIPART/MIXED; BOUNDARY=\"8323328-1174688662-894405323=:23363\"" << endl << endl;
  
  return outs;
} */

/*
ostream& mimeBodyEnd( ostream& outs )
{
  outs << "--8323328-1174688662-894405323=:23363" << endl;

  return outs;
} */
/*
ostream& mimeFileEnd( ostream& outs )
{
  outs << "--8323328-1174688662-894405323=:23363--" << endl;

  return outs;
} */

void writeBodyHeader( ostream& outs, const char *_mime, const char *_name, const char *_id, const char *_encoding, const char *_comment )
{
  if ( _mime )
    outs << "Content-Type: " << _mime;
  if ( _name )
    outs << "; name=\"" << _name << '"' << endl;
  else
    outs << endl;
  if ( _id )
    outs << "Content-ID: " << _id << endl;
  if ( _encoding )
    outs << "Content-Transfer-Encoding: " << _encoding << endl;
  if ( _comment )
    outs << "Content-Description: " << _comment << endl;
  outs << endl;
}

string createBoundary()
{
  // HACK: do it by random here!
  string b = "8323328-1174688662-894405323=:23363";
  return b;
}

/*************************************************************
 *
 * KOMLDict
 *
 *************************************************************/

KOMLDict::KOMLDict( istream& _str, const char *_filename, const char *_boundary ) : m_in( _str )
{
  m_strFilename = _filename;
  m_strBoundary = _boundary;
  m_bEmpty = false;
  
  m_bOk = scan();
}

KOMLDict::~KOMLDict()
{
  map<string,KOMLEntity*>::iterator pos = m_mapEntities.begin();
  for ( ; pos != m_mapEntities.end(); ++pos )
  {
    CORBA::release( pos->second );
  }
}
  
bool KOMLDict::scan()
{ 
  if ( m_bEmpty )
    return true;
  
  KOMLBodyParser::EndMarker res;

  do
  {  
    int start = m_in.tellg();
  
    KOMLHeaderParser p( m_in );
    if ( !p.parse() )
    {
      cerr << "Parse Error" << endl;
      return false;
    }
    
    map<string,string>::iterator pos = p.header().find( "Content-ID" );
    if ( pos == p.header().end() )
    {
      cerr << "No ID defined" << endl;
      return false;
    }
    
    KOMLBodyParser b( m_in, m_strBoundary.c_str() );
    string text;
    while( ( res = b.read( text ) ) == KOMLBodyParser::OK ) ;

    int end = m_in.tellg();
    
    m_mapEntities[ pos->second ] = new KOMLEntity( pos->second.c_str(), p.header(), start, end - start );
  }
  while( res == KOMLBodyParser::STOP );
  
  return true;
}

OPParts::MimeMultipartEntity_ptr KOMLDict::find( const char *_id )
{
  map<string,KOMLEntity*>::iterator pos = m_mapEntities.find( _id );
  if ( pos == m_mapEntities.end() )
    return 0L;
  
  return OPParts::MimeMultipartEntity::_duplicate( pos->second );
}

char* KOMLEntity::mimeType() 
{
  return find( "Content-Type" );
}

char* KOMLEntity::find( const char *_id ) 
{
  map<string,string>::iterator pos = m_mapHeader.find( _id );
  if ( pos == m_mapHeader.end() )
    return 0L;

  return CORBA::string_dup( pos->second.c_str() );
}
