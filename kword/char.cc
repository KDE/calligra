#include "char.h"

#include <string.h>
#include <assert.h>

KWString::KWString(const char *_str)
{
  if (_str == 0L)
  {
    _data_ = 0L;
    _len_ = 0;
    _max_ = 0;
    return;
  }
  
  _len_ = strlen( _str );
  _max_ = _len_;
  
  _data_ = alloc( _len_ );
  
  unsigned int i = 0;
  while( _str[i] != 0L )
    _data_[i].c = _str[i++];
}

KWString::KWString(const KWString &_string)
{
  _data_ = copy(static_cast<KWString>(_string).data(),static_cast<KWString>(_string).size());
  _len_ = static_cast<KWString>(_string).size();
  _max_ = static_cast<KWString>(_string).max();
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
  unsigned int oldlen = _len_;
  resize(_len + _len_);
  KWChar *_data = copy(_text,_len);

  for (unsigned int i = 0;i < _len;i++)
    {
      _data_[oldlen + i].c = _data[i].c;
      _data_[oldlen + i].attrib = _data[i].attrib;
    }
}

void KWString::insert( unsigned int _pos, const char *_text)
{
  assert( _pos <= _len_ );
 
  unsigned int nl = strlen( _text );
  
  unsigned int l = _len_;

  resize( _len_ + nl );
  
  if ( _pos < l )
    memmove( _data_ + _pos + nl, _data_ + _pos, sizeof(KWChar) * ( l - _pos ) );
  
  for( unsigned int i = 0; i < nl; ++i )
    {
      _data_[ _pos + i ].c = _text[i];
      _data_[ _pos + i ].attrib = 0L;
    }
}

void KWString::insert( unsigned int _pos, const char _c )
{
  assert( _pos <= _len_ );

  unsigned int l = _len_;
  
  resize( _len_ + 1 );
  
  if ( _pos < l )
    memmove( _data_ + _pos + 1, _data_ + _pos, sizeof(KWChar) * ( l - _pos ) );
  
  _data_[ _pos ].c = _c;
  _data_[ _pos ].attrib = 0L;  
}

void KWString::insert(unsigned int _pos,KWCharImage *_image)
{
  assert(_pos <= _len_);

  unsigned int l = _len_;
  
  resize(_len_ + 1);
  
  if (_pos < l)
    memmove(_data_ + _pos + 1,_data_ + _pos,sizeof(KWChar) * (l - _pos));
  
  _data_[ _pos ].c = 0;
  _data_[ _pos ].attrib = _image;
}

bool KWString::remove( unsigned int _pos,unsigned int _len = 1 )
{
  if (_pos + _len <= _len_ && (int)_pos >= 0)
    {
      for (unsigned int i = _pos;i < _pos + _len;i++)
	freeChar(_data_[i]);

      memmove(_data_ + _pos,_data_ + _pos + _len,sizeof(KWChar) * (_len_ - _pos - _len));
      resize(_len_ - _len,false);

      return true;
    }
  return false;
}

KWChar* KWString::split(unsigned int _pos)
{
  KWChar *_data,*__data;
  _data = alloc(_len_ - _pos);
  for (unsigned int i = _pos;i < _len_;i++)
    {
      _data[i - _pos].c = _data_[i].c;
      _data[i - _pos].attrib = _data_[i].attrib;
    }

  __data = copy(_data,_len_ - _pos);
  resize(_pos);
  return __data;
}

void KWString::resize(unsigned int _size,bool del = true)
{
  if ( _size == _len_ )
    return;
  
  if ( _size < _len_ )
  {
    if (del) free( _data_ + _size, _len_ - _size );
    _len_ = _size;

    return;
  }
  
  /* _size > _len_ */
  if ( _size < _max_ )
  {
    _len_ = _size;
    return;
  }
  
  // Alloc some bytes more => faster when increasing size in steps of 1
  KWChar *d = alloc( _size + 10 );
  if ( _data_ )
  {      
    memcpy( d, _data_, _len_ * sizeof(KWChar) );
    delete []_data_;
  }

  _data_ = d;
  _len_ = _size;
  _max_ = _size + 10;
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
	  switch (_data[i].attrib->getClassId())
	    {
	    case ID_KWCharFormat: 
	      {
		KWCharFormat *attrib = (KWCharFormat*)_data[i].attrib; 
		dynamic_cast<KWCharFormat*>(attrib)->getFormat()->incRef();
		KWCharFormat *f = new KWCharFormat(attrib->getFormat());
		__data[i].attrib = f;
	      } break;
	    case ID_KWCharImage:
	      {
		KWCharImage *f = new KWCharImage();
		__data[i].attrib = f;
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
    switch( _char.attrib->getClassId() )
      {
      case ID_KWCharFormat:
      case ID_KWCharImage:
	delete _char.attrib;
	break;
      default:
	assert( 0 );
      }
    _char.attrib = 0L;
  }
}
