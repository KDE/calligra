#ifndef __kstrstream_h__
#define __kstrstream_h__

#include <iostream>
#include <string>

class KOStrStreamBuffer : public streambuf
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
  KOStrStreamBuffer( string &_str ) : m_string( _str )
  {
    setp (m_buffer, m_buffer+(m_bufferSize-1));
  }

  /* Destruktor
   *  - Datenm_buffer leeren
   */
  virtual ~KOStrStreamBuffer()
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

class KOStrStream : public ostream
{
public:
  KOStrStream( string& _str ) : ostream( &m_buf ), m_buf( _str ) { }

protected:
  KOStrStreamBuffer m_buf;
};

#endif
