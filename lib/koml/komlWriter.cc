#include "komlWriter.h"

int g_iLevel = 0;

ostream& otag( ostream& outs )
{
  for( int i = 0; i < g_iLevel; i++ )
    outs << ' ';

  g_iLevel++;
  
  return outs;
}

ostream& etag( ostream& outs )
{
  g_iLevel--;

  for( int i = 0; i < g_iLevel; i++ )
     outs << ' ';
  
  return outs;
}

ostream& indent( ostream& outs )
{
  for( int i = 0; i < g_iLevel; i++ )
    outs << ' ';
  
  return outs;
}

void intToHexStr( char *_buffer, int _val )
{
    const char *conv = "0123456789abcdef";
  
  _buffer[0] = conv[ ( _val >> 28 ) ];
  _buffer[1] = conv[ ( _val >> 24 ) & 0xF ];
  _buffer[2] = conv[ ( _val >> 20 ) & 0xF ];
  _buffer[3] = conv[ ( _val >> 16 ) & 0xF ];
  _buffer[4] = conv[ ( _val >> 12 ) & 0xF ];
  _buffer[5] = conv[ ( _val >> 8 ) & 0xF ];
  _buffer[6] = conv[ ( _val >> 4 ) & 0xF ];
  _buffer[7] = conv[ _val & 0xF ];
  _buffer[8] = 0;
}

int hexStrToInt( const char *_buffer )
{
  int value = 0;
  
  for( int i = 0; i < 8; i++ )
  {
    value <<= 4;
    
    int v;
    
    if ( _buffer[i] >= '0' && _buffer[i] <= '9' )
      v = _buffer[i] - '0';
    else
      v = 10 + _buffer[i] - 'a';

    value |= v;
  }
  
  return value;
}

void shortToHexStr( char *_buffer, short _val )
{
  const char *conv = "0123456789abcdef";
  
  _buffer[0] = conv[ ( _val >> 4 ) & 0xF ];
  _buffer[1] = conv[ _val & 0xF ];
  _buffer[2] = 0;
}

short hexStrToShort( const char *_buffer )
{
  short value = 0;
  
  for( int i = 0; i < 2; i++ )
  {
    value <<= 4;
    
    short v;
    
    if ( _buffer[i] >= '0' && _buffer[i] <= '9' )
      v = _buffer[i] - '0';
    else
      v = 10 + _buffer[i] - 'a';

    value |= v;
  }
  
  return value;
}
