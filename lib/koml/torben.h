#ifndef __torben_h__
#define __torben_h__

#include <list.h>
#include <iostream.h>
#include <string>

typedef list<string> strlist;

class tstring : public string
{
public:
  tstring() : string() { }
  tstring (const string& str) : string( str ) { }
  tstring (const string& str, size_type pos, size_type n = npos )
    : string( str, pos, n ) { }
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
