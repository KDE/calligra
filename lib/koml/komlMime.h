#ifndef __komlMime_h__
#define __komlMime_h__

#include <iostream>
#include <torben.h>
#include <map>
#include <string>

#include <parts.h>

class KOMLEntity : virtual public OPParts::MimeMultipartEntity_skel
{
public:
  // C++
  KOMLEntity() { };
  KOMLEntity( const char *_id, map<string,string> &_map, int _start, int _len )
  { m_mapHeader = _map; m_strId = _id; m_iLen = _len; m_iStart = _start; }
  KOMLEntity( const KOMLEntity& _e )
  { m_mapHeader = _e.m_mapHeader; m_strId = _e.m_strId; m_iLen = _e.m_iLen; m_iStart = _e.m_iStart; }
  
  map<string,string>& header() { return m_mapHeader; }

  // IDL
  char *id() { return CORBA::string_dup( m_strId.c_str() ); }
  CORBA::Long len() { return m_iLen; }
  CORBA::Long start() { return m_iStart; }
  char *mimeType();
  char* find( const char* _id );
  
protected:
  // C++
  int m_iStart;
  int m_iLen;
  string m_strId;
  map<string,string> m_mapHeader;
};

class KOMLDict : virtual public OPParts::MimeMultipartDict_skel
{
public:
  // C++
  KOMLDict() : m_in( cin ) { m_bEmpty = true; m_bOk = true; }
  KOMLDict( istream& _str, const char *_filename, const char *_boundary );
  ~KOMLDict();
  
  bool isOk() { return m_bOk; }
  
  // IDL
  OPParts::MimeMultipartEntity_ptr find( const char *_id );
  char *boundary() { return CORBA::string_dup( m_strBoundary.c_str() ); }
  // HACK
  char *host() { return CORBA::string_dup( "localhost" ); }
  char *filename() { return CORBA::string_dup( m_strFilename.c_str() ); }
  
protected:
  // C++
  bool scan();

  istream &m_in;
  map<string,KOMLEntity*> m_mapEntities;
  bool m_bEmpty;
  bool m_bOk;
  string m_strBoundary;
  string m_strFilename;
};

class KOMLHeaderParser
{
public:
  KOMLHeaderParser( istream &_str );
  
  bool parse();
  map<string,string>& header() { return m_mapHeader; }
  
  const char* mimeType() { return m_strMimeType.c_str(); }
  const char* boundary() { return m_strBoundary.c_str(); }
  const char* name() { return m_strName.c_str(); }

protected:
  istream &m_stream;
  map<string,string> m_mapHeader;
  string m_strMimeType;
  string m_strBoundary;
  string m_strName;
};

class KOMLBodyParser
{
public:
  enum EndMarker { OK, STOP, END };
  
  KOMLBodyParser( istream& _str, const char *_boundary );
  
  EndMarker read( string &_str );
  
protected:
  istream &m_stream;
  string m_strBoundary;
};

class Base64
{
public:
  Base64() { };
  virtual ~Base64() { };

  void encode( char *p, unsigned char c1, unsigned char c2, unsigned char c3, int len = 3 );
  int decode( char *_dest, unsigned char c1, unsigned char c2, unsigned char c3, unsigned char c4 );
};

#include <streambuf.h>

class Base64EncodeBuffer : public streambuf, protected Base64
{
protected:
  static const int m_bufferSize = 48;   // Groesse des Datenm_buffers
  char m_buffer[m_bufferSize];            // Datenm_buffer
  char m_buffer2[m_bufferSize * 4 / 3 + 3 + 1 ];            // Datenm_buffer 2

public:
  /* Konstruktor
   *  - Datenm_buffer initialisieren
   *  - ein Zeichen kleiner,
   *    damit das m_bufferSize-te Zeichen overflow() ausloest
   */
  Base64EncodeBuffer( ostream &_str ) : m_out( _str )
  {
    setp (m_buffer, m_buffer+(m_bufferSize-1));
  }

  /* Destruktor
   *  - Datenm_buffer leeren
   */
  virtual ~Base64EncodeBuffer()
  {
    sync();
  }

protected:
  /* Zeichen im Buffer abspeichern
   */
  int emptyBuffer()
  {
    int anz = pptr()-pbase();

    int cnt = anz / 3;
    int rest = anz % 3;
    
    int pos = 0;
    int i;
    for ( i = 0; i < cnt; i++ )
    {
      encode( m_buffer2 + i*4, m_buffer[ pos ], m_buffer[ pos + 1 ], m_buffer[ pos + 2 ], 3 );
      pos += 3;
    }
    
    if ( rest == 1 )
      encode( m_buffer2 + (i++)*4, m_buffer[ pos ], 0, 0, 1 );
    else if ( rest == 2 )
      encode( m_buffer2 + (i++)*4, m_buffer[ pos ], m_buffer[ pos + 1 ], 0, 2 );

    m_buffer2[ i*4 ] = 0;
    
    m_out.write( m_buffer2, i*4 );
    
     if ( m_out.eof() )
       return EOF;

    pbump (-anz);    // Schreibzeiger entspr. zuruecksetzen
    return anz;
  }

  /* M_Buffer voll
   *  - c und alle vorherigen Zeichen ausgeben
   */
  virtual int overflow (int c)
  {
    if (c != EOF)
    {
      // Zeichen noch in den M_Buffer einfuegen
      *pptr() = c;
      pbump(1);
    }
    if ( emptyBuffer() == EOF )
    {
      return EOF;
    }
    return c;
  }

  /* Daten mit Datentraeger abgleichen
   *  - Buffer explizit leeren
   */
  virtual int sync ()
  {
    if (emptyBuffer() == EOF)
    {
      // FEHLER
      return -1;
    }
    return 0;
  }

  ostream &m_out;
};

class Base64OStream : public ostream
{
public:
  Base64OStream( ostream& _str ) : ostream( &m_buf ), m_buf( _str ) { }

protected:
  Base64EncodeBuffer m_buf;
};


class Base64DecodeBuffer : public streambuf, protected Base64
{
protected:
  /* Datenpuffer:
   *  maximal 4 Zeichen Putback-Bereich plus
   *  maximal 6 Zeichen normaler Lesepuffer
   */
  const int m_iBufferSize = 48 + 4;     // Groesse des Datenpuffers
  char m_buffer[ m_iBufferSize ];

public:
  /* Konstruktor
   *  - Datenpuffer leer initialisieren
   *  - keine Putback-Reserve
   *  => underflow() forcieren
   */
  Base64DecodeBuffer( istream& _str ) : m_in( _str )
  {
    setg ( m_buffer + 4,     // Putback-Anfang
	   m_buffer + 4,     // Leseposition
	   m_buffer + 4 );    // Puffer-Ende

    m_bEnd = false;
  }

protected:
  /* neue Zeichen in den Puffer einlesen
   */
  virtual int underflow();
  
  istream &m_in;

  bool m_bEnd;
};

class Base64IStream : public istream
{
public:
  Base64IStream( istream& _str ) : istream( &m_buf ), m_buf( _str ) { }

protected:
  Base64DecodeBuffer m_buf;
};

/**************************************************************
 *
 * KOMLBodyIBuffer
 *
 **************************************************************/

class KOMLBodyIStream;

class KOMLBodyIBuffer : public streambuf
{
protected:
  /* Datenpuffer:
   *  maximal 4 Zeichen Putback-Bereich plus
   *  maximal 6 Zeichen normaler Lesepuffer
   */
  const int m_iBufferSize = 252 + 4;     // Groesse des Datenpuffers
  char m_buffer[ m_iBufferSize ];

public:
  /* Konstruktor
   *  - Datenpuffer leer initialisieren
   *  - keine Putback-Reserve
   *  => underflow() forcieren
   */
  KOMLBodyIBuffer( istream& _str, const char *_boundary, KOMLBodyIStream& _kin ) : m_in( _str ), m_stream( _kin )
  {
    setg ( m_buffer + 4,     // Putback-Anfang
	   m_buffer + 4,     // Leseposition
	   m_buffer + 4 );    // Puffer-Ende

    m_bEnd = false;
    m_bNewLine = true;
    m_strBoundary = _boundary;
  }

protected:
  /* neue Zeichen in den Puffer einlesen
   */
  virtual int underflow();

  istream &m_in;

  bool m_bEnd;
  bool m_bNewLine;
  string m_strBoundary;
  KOMLBodyIStream& m_stream;
};

/**************************************************************
 *
 * KOMLBodyIStream
 *
 **************************************************************/

class KOMLBodyIStream : public istream
{
  friend KOMLBodyIBuffer;
  
public:
  KOMLBodyIStream( istream& _str, const char *_boundary ) : istream( &m_buf ), m_buf( _str, _boundary, *this )
  {
    m_bFileEnd = false;
    m_bBodyEnd = false;
  }

  bool fileEnd() { return m_bFileEnd; }
  bool bodyEnd() { return m_bBodyEnd; }
  
protected:
  void setFileEnd() { m_bFileEnd = true; }
  void setBodyEnd() { m_bBodyEnd = true; }
  
  KOMLBodyIBuffer m_buf;
  bool m_bFileEnd;
  bool m_bBodyEnd;
};

class pump
{
public:
  pump( istream& _in, ostream& _out ) : m_in( _in ), m_out( _out ) { }
  
  void run()
  {
    char buffer[ 4096 ];
    while ( !m_in.eof() && m_out.good() )
    {
      m_in.read( buffer, 4096 );
      m_out.write( buffer, m_in.gcount() );
    }
  }
  
protected:
  istream &m_in;
  ostream &m_out;
};

// HACK: Do not use
// ostream& mimeHeader( ostream& outs );
void writeMagic( ostream& outs, const char *_mime );
void writeMimeHeader( ostream& outs, const char *_boundary );
void writeBodyHeader( ostream& outs, const char *_mime, const char *_name, const char *_id, const char *_encoding, const char *_comment );
// HACK: Do not use
//ostream& mimeBodyEnd( ostream& outs );
// HACK: Do not use
//ostream& mimeFileEnd( ostream& outs );
string createBoundary();

#endif


