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

#include <ctype.h>
#include <iostream.h>

#include "komlParser.h"

#include <qstring.h>
#include <qregexp.h>

#define PULL if ( m_iPos == m_iLen ) { if ( !pull() ) return false; }

KOMLParser::KOMLParser( KOMLFeed* _feed )
{
  m_pFeed = _feed;
  m_iPos = 0;
  m_iLen = 0;
  m_bEOF = false;
  m_pData = 0L;
  m_bEmit = false;
  m_bUnreadTag = false;
  m_bCloseSingleTag = false;
}

KOMLParser::~KOMLParser()
{
}

bool KOMLParser::pull()
{
  if ( m_bEOF )
    return false;

  KOMLData* d = m_pFeed->read();
  if ( d == 0L )
  {
    m_bEOF = true;
    return false;
  }

  m_lstData.push_back( d );
  m_iPos = 0;
  m_pData = d->data();
  m_iLen = d->length();

  return true;
}

bool KOMLParser::open( const char *_search, string& tag )
{
  // Obviously there are no tags to open in a single-tag
  if ( m_bCloseSingleTag )
    return false;

  if ( m_bEOF )
    return false;

  int searchlen = 0;
  if ( _search )
    searchlen = strlen( _search );

  const char *end_tag = 0L;
  int endlen = 0;
  if ( m_vecStack.size() > 0 )
  {
    end_tag = m_vecStack.back().c_str();
    while ( end_tag[ endlen ] != ' ' && end_tag[ endlen ] != '\n' && end_tag[ endlen ] != '\t' &&
	    end_tag[ endlen ] != 0 )
      endlen++;
  }

  // Loop until we get EOF, or a closing tag or a match.
  do
  {
    if ( !readTag( tag ) )
      return false;

    int tl = tag.length();
    // Processing information  =>  Skip
    if ( tl > 0 && tag[ 0 ] == '?' )
      continue;
    // Single tag ?
    else if ( tl > 0 && tag[ tl - 1 ] == '/' )
    {
      // Do NOT put it on the stack, but make a remark.
      m_bCloseSingleTag = true;
      // Delete trailing '/'
#ifdef HAVE_MINI_STL
      tag.remove( tl - 1, 1 );
#else
      tag.erase( tl - 1, 1 );
#endif
      return true;
    }
    // Is it a closing tag ?
    else if ( tl > 0 && tag[0] == '/' )
    {
      // we can not open any tag here.
      unreadTag( tag );
      return false;
    }
    // A new opening tag but not the one we want ?  =>  We have to close it first
    else if ( _search != 0L && ( strncmp( _search, tag.c_str(), searchlen ) != 0 ||
				 ( tag[ searchlen ] != 0 && tag[ searchlen ] != ' ' &&
				   tag[ searchlen ] != '\t' && tag[ searchlen ] != '\n' ) ) )
    {
      m_vecStack.push_back( tag );
      if ( !close( tag ) )
	return false;
    }
    // This is the one we want. It is an opening tag  =>  Put it on the stack
    else
    {
      m_vecStack.push_back( tag );
      return true;
    }

  } while( 1 );
}

bool KOMLParser::readText( string &_str )
{
  if ( m_bUnreadTag )
  {
    _str = "";
    return true;
  }

  int start = m_iPos;

  while( m_lstData.size() > 1 )
  {
    free( m_lstData.front() );
    m_lstData.pop_front();
  }

  if ( m_bEOF )
  {
    _str = "";
    return false;
  }

  if ( m_lstData.size() == 0 )
  {
    if ( !pull() )
    {
      _str = "";
      return false;
    }
  }

  bool bend = false;

  do
  {
    while( m_iPos < m_iLen && m_pData[ m_iPos ] != '<' )
      m_iPos++;

    if ( m_iPos == m_iLen )
    {
      if ( !pull() )
	bend = true;
    }
    else
    {
      bend = true;
    }
  } while( !bend );

  int end = m_iPos;

  // KOMLData *d = m_lstData.front();

  int size = m_lstData.size();
  if ( size == 1 )
  {
    _str.assign( m_pData + start, end - start );
  }
  else
  {
    _str.assign( m_lstData.front()->data() + start, m_lstData.front()->length() - start );
    free( m_lstData.front() );
    m_lstData.pop_front();

    for( int i = 1; i < size; i++ )
    {
      if ( i == size - 1 )
	_str.append( m_lstData.front()->data(), end );
      else
      {
	_str.append( m_lstData.front()->data(), m_lstData.front()->length() );
	free( m_lstData.front() );
	m_lstData.pop_front();
      }
    }
  }

  encode(_str);
  
  return true;
}

bool KOMLParser::readTag( string &_str )
{
  if ( m_bUnreadTag )
  {
    m_bUnreadTag = false;
    _str = m_strUnreadTag;
    return true;
  }

  if ( !findTagStart() )
    return false;

  int start = m_iPos;

  while( m_lstData.size() > 1 )
  {
    free( m_lstData.front() );
    m_lstData.pop_front();
  }

  if ( !findTagEnd() )
    return false;

  int end = m_iPos;

  // KOMLData *d = m_lstData.front();

  int size = m_lstData.size();
  if ( size == 1 )
  {
    _str.assign( m_pData + start, end - start );
  }
  else
  {
    _str.assign( m_lstData.front()->data() + start, m_lstData.front()->length() - start );
    free( m_lstData.front() );
    m_lstData.pop_front();

    for( int i = 1; i < size; i++ )
    {
      if ( i == size - 1 )
	_str.append( m_lstData.front()->data(), end );
      else
      {
	_str.append( m_lstData.front()->data(), m_lstData.front()->length() );
	free( m_lstData.front() );
	m_lstData.pop_front();
      }
    }
  }
#ifdef HAVE_MINI_STL
  _str.remove( _str.length() - 1, 1 );
#else
  _str.erase( _str.length() - 1, 1 );
#endif
  return true;
}

void KOMLParser::unreadTag( string &_tag )
{
  m_bUnreadTag = true;
  m_strUnreadTag = _tag;
}

bool KOMLParser::findTagStart()
{
  if ( m_bEOF )
    return false;

  if ( m_lstData.size() == 0 )
  {
    if ( !pull() )
      return false;
  }

  bool bend = false;

  do
  {
    while( m_iPos < m_iLen && m_pData[ m_iPos ] != '<' )
      m_iPos++;

    if ( m_iPos == m_iLen )
    {
      if ( !pull() )
	return false;
    }
    else
    {
      bend = true;
      m_iPos++;
    }
  } while( !bend );

  return true;
}

bool KOMLParser::findTagEnd()
{
  do
  {
    PULL;
    if ( m_pData[ m_iPos ] == '>' )
    {
      m_iPos++;

      if ( m_iPos == m_iLen )
	pull();

      return true;
    }
    m_iPos++;

  } while( 1 );
}

#define END_EMIT  if ( _emit ) m_bEmit = false;

bool KOMLParser::close( string& _tag, bool _emit )
{
  if ( m_bCloseSingleTag )
  {
    m_bCloseSingleTag = false;
    _tag = "/";
    return true;
  }

  // cerr << "... Closing '" << m_vecStack.back() << "'" << endl;

  if ( _emit )
  {
    m_bEmit = true;
    m_iEmitStart = m_iPos;
  }

  do
  {
    if ( m_bEOF )
    {
      END_EMIT;
      return false;
    }

    int prev = m_iPos;

    if ( !readTag( _tag ) )
    {
      END_EMIT;
      return false;
    }

    // Which tag do we have to close ?
    const char *end_tag = m_vecStack.back().c_str();
    int endlen = 0;
    while ( end_tag[ endlen ] != ' ' && end_tag[ endlen ] != '\n' && end_tag[ endlen ] != '\t' &&
	    end_tag[ endlen ] != 0 )
      endlen++;

    // Is this a tag that closes itself like <color rgb=#0f0f0f />  =>  Loop
    int len = _tag.length();
    while( len > 1 && _tag[ len - 1 ] == '/' )
    {
      prev = m_iPos;

      if ( !readTag( _tag ) )
      {
	END_EMIT;
	return false;
      }
      len = _tag.length();
    }

    // Is this the closing tag we are searching for ?
    if ( _tag == "/" || ( len > 1 && _tag[0] == '/' && strncmp( _tag.c_str() + 1, end_tag, endlen ) == 0 &&
	 ( _tag[ endlen + 1 ] == ' ' || _tag[ endlen + 1 ] == '\t' || _tag[ endlen + 1 ] == '\n' || _tag[ endlen + 1 ] == 0 ) ) )
    {
      m_vecStack.pop_back();
      if ( _emit )
      {
	string tmp;
	if ( m_iEmitStart == -1 )
	  tmp.assign( m_pData, prev );
	else
	  tmp.assign( m_pData + m_iEmitStart, prev - m_iEmitStart );
	// cerr << "EL:" << tmp << endl;	
      }

      END_EMIT;
      return true;
    }

    // We found another opening tag. We have to close this one first.
    m_vecStack.push_back( _tag );
    close( _tag );
  } while( 1 );
}

void KOMLParser::free( KOMLData* _data )
{
  if ( !m_bEmit )
  {
    m_pFeed->free( _data );
    return;
  }

  if ( m_iEmitStart != -1 )
  {
    // cerr << "EF:" << _data->data() + m_iEmitStart << endl;
    m_iEmitStart = -1;
  }
  // else
  // cerr << "E:" << _data->data() << endl;

  m_pFeed->free( _data );
}

bool KOMLParser::parseTag( const char *_tag, string& name, vector<KOMLAttrib>& _attribs )
{
  _attribs.erase( _attribs.begin(), _attribs.end() );

  const char *p = _tag;
  int l = 0;

  while( isalnum( *p ) )
  {
    l++;
    p++;
  }

  if ( *p == 0 )
  {
    name = _tag;
    return true;
  }

  name.assign( _tag, l );

  while( *p == ' ' || *p == '\t' || *p == '\n' )
    p++;

  if ( *p == 0 )
    return true;

  do
  {
    int len = 0;
    const char *start = p;
    while( isalnum( *p ) )
    {
      len++;
      p++;
    }

    while( *p == ' ' || *p == '\t' || *p == '\n' )
      p++;

    char last = *p;

    if ( last == '=' )
    {
      p++;

      while( *p == ' ' || *p == '\t' || *p == '\n' )
	p++;

      if ( *p == 0 )
	return false;

      const char* start2 = p;
      int len2 = 0;

      if ( *p == '"' )
      {
	p++;
	start2 = p;
	while( *p != '"' )
	{
	  p++;
	  len2++;
	}
	p++;
      }
      else
      {
	start2 = p;
	while( *p != ' ' && *p != '\t' && *p != '\n' && *p != '/' && *p != '>' && *p != 0 )
	{
	  p++;
	  len2++;
	}
      }

      KOMLAttrib attrib;
      attrib.m_strName.assign( start, len );
      attrib.m_strValue.assign( start2, len2 );
      encode(attrib.m_strValue);
      _attribs.push_back( attrib );
    }
    else if ( isalnum( last ) || last == 0 )
    {
      KOMLAttrib attrib;
      attrib.m_strName.assign( start, len );
      attrib.m_strValue = "";
      _attribs.push_back( attrib );
    }
    else
      return false;

    while( *p == ' ' || *p == '\t' || *p == '\n' )
      p++;

    if ( *p == 0 )
      return true;
  } while( 1 );
}

void KOMLParser::encode(string &_str)
{
  QString str(_str.c_str());
  str.replace(QRegExp("&lt;"),"<");
  str.replace(QRegExp("&gt;"),">");
  _str = str.ascii();
}
