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

  string& tstring::stripWhiteSpace( string& _res ) const;
  string& tstring::stripWhiteSpace();
  string& right( string &_res, int n ) const;
};

#endif
