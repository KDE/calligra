#ifndef __torben_h__
#define __torben_h__

#include <config.h>
#ifdef HAVE_MINI_STL
#include <ministl/list.h>
#include <ministl/string>
#else
#include <list.h>
#include <string>
#endif
#include <iostream.h>



typedef list<string> strlist;

class tstring : public string
{
public:
  tstring() : string() { }
  tstring (const string& str) : string( str ) { }
#ifdef HAVE_MINI_STL
  tstring (const string& str, size_type pos, size_type n = NPOS )
    : string( str, pos, n ) { }
#else
  tstring (const string& str, size_type pos, size_type n = npos )
    : string( str, pos, n ) { }
#endif
  tstring (const char* s, size_type n)
    : string( s, n ) { }
  tstring (const char* s)
    : string( s ) { }
  tstring( size_type n, char c)
    : string( n, c ) { }

  string& stripWhiteSpace( string& _res ) const;
  string& stripWhiteSpace();
  string& right( string &_res, int n ) const;
};

/**************************************************************
 *
 * StrOStream
 *
 **************************************************************/

class tostrstreambuffer : public streambuf
{
protected:
  static const int m_bufferSize = 4096;   // Groesse des Datenm_buffers
  char m_buffer[m_bufferSize + 1];            // Datenm_buffer
  string &m_string;
  
public:
  /* Konstruktor
   *  - Datenm_buffer initialisieren
   *  - ein Zeichen kleiner,
   *    damit das m_bufferSize-te Zeichen overflow() ausloest
   */
  tostrstreambuffer( string &_str ) : m_string( _str )
  {
    setp (m_buffer, m_buffer+(m_bufferSize-1));
  }

  /* Destruktor
   *  - Datenm_buffer leeren
   */
  virtual ~tostrstreambuffer()
  {
    sync();
  }

protected:
  /* Zeichen im buffer abspeichern
   */
  int emptybuffer()
  {
    int anz = pptr()-pbase();
    m_buffer[ anz ] = 0;
    m_string += m_buffer;

    pbump (-anz);    // Schreibzeiger entspr. zuruecksetzen
    return anz;
  }

  /* M_buffer voll
   *  - c und alle vorherigen Zeichen ausgeben
   */
  virtual int overflow (int c)
  {
    if (c != EOF)
    {
      // Zeichen noch in den M_buffer einfuegen
      *pptr() = c;
      pbump(1);
    }
    if ( emptybuffer() == EOF )
    {
      return EOF;
    }
    return c;
  }

  /* Daten mit Datentraeger abgleichen
   *  - buffer explizit leeren
   */
  virtual int sync ()
  {
    if (emptybuffer() == EOF)
    {
      // FEHLER
      return -1;
    }
    return 0;
  }
};

class tostrstream : public ostream
{
public:
  tostrstream( string& _str ) : ostream( &m_buf ), m_buf( _str ) { }

protected:
  tostrstreambuffer m_buf;
};

#endif
