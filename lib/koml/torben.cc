#include "torben.h"

#include <ctype.h>

string& tstring::stripWhiteSpace( string& _res ) const
{
  int len = length();
  int i = 0;
  while( i < len && isspace( data()[i] ) )
    i++;
  
  if ( i == len )
  {
    _res = "";
    return _res;
  }
  
  int j = len - 1;
  while( j >= 0 && isspace( data()[j] ) )
    j--;
  
  _res.assign( c_str() + i, j - i + 1 );
  return _res;
}

string& tstring::stripWhiteSpace()
{
  int len = length();
  int i = 0;
  while( i < len && isspace( data()[i] ) )
    i++;
 
  if ( i == len )
  {
    *this = "";
    return *this;
  }
  
  erase( 0, i );
  
  len = length();
  int j = len - 1;
  while( j >= 0 && isspace( data()[j] ) )
    j--;
  
  if ( j == len - 1 )
    return (*this);
  
  erase( j + 1, len - j - 1 );
  return (*this);
}

string& tstring::right( string &_res, int n ) const
{
  int len = length();
  if ( len >= n )
  {
    _res = *this;
    return _res;
  }
  
  _res.assign( c_str() + len - n );
  return _res;
}



