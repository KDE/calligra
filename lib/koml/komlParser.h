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

#ifndef __parser_h__
#define __parser_h__

#include <config.h>
#include <string.h>
#include <vector.h>

#include "torben.h"
#include "komlFeed.h"

struct KOMLAttrib
{
  string m_strName;
  string m_strValue;
};

class KOMLParser
{
public:
  KOMLParser( KOMLFeed* _feed );
  virtual ~KOMLParser();

  /**
   * @param _search is the opening tag we are searching for. If this is 0L
   *                every opening tag is matched.
   * @param _tag holds the matched tag.
   */
  bool open( const char *_search, string &_tag );
  bool close( string&, bool _emit = false );

  bool readText( string& );

  static bool parseTag( const char *_tag, string& name, vector<KOMLAttrib>& _attribs );

  static void encode(string &_str);
  
protected:
  void unreadTag( string& );
  bool readTag( string& );
  bool findTagStart();
  bool findTagEnd();
  bool pull();

  void free( KOMLData* _data );

  KOMLFeed *m_pFeed;
  list<KOMLData*> m_lstData;

  int m_iPos;
  bool m_bEOF;
  const char *m_pData;
  int m_iLen;

  vector<string> m_vecStack;

  bool m_bEmit;
  int m_iEmitStart;

  bool m_bUnreadTag;
  string m_strUnreadTag;

  bool m_bCloseSingleTag;
};

#endif
