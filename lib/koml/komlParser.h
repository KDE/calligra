#ifndef __parser_h__
#define __parser_h__

#include <config.h>
#include <string.h>
#ifdef HAVE_MINI_STL
#include <ministl/vector.h>
#else
#include <vector.h>
#endif

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
