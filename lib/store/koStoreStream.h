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

#ifndef __koStoreStream_h__
#define __koStoreStream_h__

#include <fstream>

/**
 * Define ostorestream and istorestream, used to conveniently
 * write and read from a KoStore.
 * Warning : pretty hairy code.
 * If only the comments weren't in German !
 * (David)
 */

// (Werner) I've translated the comments - there might be
// some errors, though :(
// (David) Thanks !!!

class KoStore;

class ostorestreambuffer : public streambuf
{
protected:
  static const int m_bufferSize = 4096;   // Size of the buffer
  char m_buffer[m_bufferSize + 1];
  KoStore* m_pStore;

public:
  /* CTOR
   *  - initialize m_buffer
   *  - the whole stuff has to be one char shorter to
   *    trigger overflow() if the m_bufferSized char is reached
   */
  ostorestreambuffer( KoStore* _store ) : m_pStore( _store )
  {
    setp (m_buffer, m_buffer+(m_bufferSize-1));
  }

  /* DTOR
   *  - empty our m_buffer
   */
  virtual ~ostorestreambuffer()
  {
    sync();
  }

protected:
  /* store chars in the buffer
   */
  int emptybuffer();

  /* m_buffer if full
   *  - pass on all chars of buffer and "c"
   */
  virtual int overflow (int c)
  {
    if (c != EOF)
    {
      // add "c" to m_buffer
      *pptr() = c;
      pbump(1);
    }
    if ( emptybuffer() == EOF )
    {
      return EOF;
    }
    return c;
  }

  /* sync the buffers
   *  - explicitly empty the buffer
   */
  virtual int sync ()
  {
    if (emptybuffer() == EOF)
    {
      // ERROR
      return -1;
    }
    return 0;
  }
};

class ostorestream : public ostream
{
public:
  ostorestream( KoStore* _store ) : ostream( &m_buf ), m_buf( _store ) { }

protected:
  ostorestreambuffer m_buf;
};

/*******************************************
 *
 * Store Input Buffer
 *
 *******************************************/

class istorestreambuffer : public streambuf
{
protected:
  /* data buffer:
   *  max. 4 chars additional Putback-Zone
   *  max. 8192 chars "normal" read buffer
   */
  static const int pufferSize = 8192 + 4;     // size of the buffer
  char puffer[pufferSize];       // buffer (buffer -> German: "Puffer" :)
  KoStore* m_pStore;

public:
  /* CTOR
   *  - initialize empty buffer
   *  - no Putback-Zone!
   *  => force underflow()
   */
  istorestreambuffer( KoStore* _store ) : m_pStore( _store )
  {
    //kdebug( KDEBUG_INFO, 30002, "Pointer constructor" );
    setg (puffer+4,     // beginning of Putback-Zone
	  puffer+4,     // read posiition
	  puffer+4);    // end of the buffer
  }

protected:
  /* read new chars into the buffer
   */
  virtual int underflow ();
};

/*******************************************
 *
 * Store Input Stream
 *
 *******************************************/

class istorestream : public istream
{
public:
  istorestream( KoStore* _store ) : istream( &m_buf ), m_buf( _store ) { }

protected:
  istorestreambuffer m_buf;
};

#endif
