#include "char.h"

#include <string.h>
#include <assert.h>

KWString::KWString( const char *_str )
{
  if ( _str == 0L )
  {
    data = 0L;
    len = 0;
    max = 0;
    return;
  }
  
  len = strlen( _str );
  max = len;
  
  data = alloc( len );
  
  unsigned int i = 0;
  while( _str[i] != 0L )
    data[i].c = _str[i++];
}

KWString::KWString(const KWString &_string)
{
  data = copy(_string.data,_string.len);
  len = _string.len;
  max = _string.max;
}

KWChar* KWString::alloc( unsigned int _len )
{
  KWChar *c = new KWChar[ _len ];

  KWChar *p = c;
  for( unsigned int i = 0; i < _len; ++i )
  {
    p->c = 0;
    p->attrib = 0L;
    p++;
  }
  
  return c;
}

void KWString::free( KWChar* _data, unsigned int _len )
{
  for( unsigned int i = 0; i < _len; ++i )
    freeChar( _data[ i ] );
}
  
void KWString::append(KWChar *_text,unsigned int _len)
{
  unsigned int oldlen = len;
  resize(_len + len);
  KWChar *_data = copy(_text,_len);

  for (unsigned int i = 0;i < _len;i++)
    {
      data[oldlen + i].c = _data[i].c;
      data[oldlen + i].attrib = _data[i].attrib;
    }
}

void KWString::insert( unsigned int _pos, const char *_text )
{
  assert( _pos <= len );
 
  unsigned int nl = strlen( _text );
  
  unsigned int l = len;

  resize( len + nl );
  
  if ( _pos < l )
    memmove( data + _pos + nl, data + _pos, sizeof(KWChar) * ( l - _pos ) );
  
  for( unsigned int i = 0; i < nl; ++i )
  {
    data[ _pos + i ].c = _text[i];
    data[ _pos + i ].attrib = 0L;
  }
}

void KWString::insert( unsigned int _pos, const char _c )
{
  assert( _pos <= len );

  unsigned int l = len;
  
  resize( len + 1 );
  
  if ( _pos < l )
    memmove( data + _pos + 1, data + _pos, sizeof(KWChar) * ( l - _pos ) );
  
  data[ _pos ].c = _c;
  data[ _pos ].attrib = 0L;  
}

bool KWString::remove( unsigned int _pos,unsigned int _len = 1 )
{
  if (_pos + _len <= len && (int)_pos >= 0)
    {
      memmove(data + _pos,data + _pos + _len,sizeof(KWChar) * (len - _pos - _len));
      resize(len - _len);
      return true;
    }
  return false;
}

void KWString::resize( unsigned int _size )
{
  if ( _size == len )
    return;
  
  if ( _size < len )
  {
    free( data + _size, len - _size );
    len = _size;

    return;
  }
  
  /* _size > len */
  if ( _size < max )
  {
    len = _size;
    return;
  }
  
  // Alloc some bytes more => faster when increasing size in steps of 1
  KWChar *d = alloc( _size + 10 );
  if ( data )
  {      
    memcpy( d, data, len * sizeof(KWChar) );
    delete []data;
  }

  data = d;
  len = _size;
  max = _size + 10;
}

KWChar* KWString::copy(KWChar *_data,unsigned int _len)
{
  KWChar *__data;
  
  __data = alloc(_len);
  
  unsigned int i = 0;
  for (i = 0;i < _len;i++)
    {
      __data[i].c = _data[i].c;
      if (_data[i].attrib)
	{
	  switch (_data[i].attrib->classId)
	    {
	    case ID_KWCharFormat: 
	      {
		KWCharFormat *f = new KWCharFormat(((KWCharFormat*)_data[i].attrib)->format);
		__data[i].attrib = &(f->type);
	      } break;
	    case ID_KWCharImage:
	      {
		KWCharImage *f = new KWCharImage();
		__data[i].attrib = &(f->type);
	      } break;
	    }
	}
      else __data[i].attrib = 0L;
    }
  return __data;
}

void freeChar( KWChar& _char )
{
  if ( _char.attrib )
  {
    switch( _char.attrib->classId )
      {
      case ID_KWCharFormat:
      case ID_KWCharImage:
	delete _char.attrib;
	break;
      default:
	assert( 0 );
      }
  }
}
