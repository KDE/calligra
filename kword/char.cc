#include "char.h"

#include <string.h>
#include <assert.h>

#include <koIMR.h>
#include <komlMime.h>
#include <strstream>
#include <fstream>
#include <unistd.h>

#include "kword_doc.h"
#include "frame.h"

KWString::KWString(QString _str)
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
  _data_ = copy(_string._data_,_string._len_);
  _len_ = _string._len_;
  _max_ = _string._max_;
}

KWString &KWString::operator=(const KWString &_string)
{
  _data_ = copy(_string._data_,_string._len_);
  _len_ = _string._len_;
  _max_ = _string._max_;

  return *this;
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

void KWString::insert( unsigned int _pos,QString _text)
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

void KWString::insert(unsigned int _pos,KWCharTab *_tab)
{
  assert(_pos <= _len_);

  unsigned int l = _len_;

  resize(_len_ + 1);

  if (_pos < l)
    memmove(_data_ + _pos + 1,_data_ + _pos,sizeof(KWChar) * (l - _pos));

  _data_[ _pos ].c = 0;
  _data_[ _pos ].attrib = _tab;
}

void KWString::insert(unsigned int _pos,KWCharVariable *_var)
{
  assert(_pos <= _len_);

  unsigned int l = _len_;

  resize(_len_ + 1);

  if (_pos < l)
    memmove(_data_ + _pos + 1,_data_ + _pos,sizeof(KWChar) * (l - _pos));

  _data_[ _pos ].c = 0;
  _data_[ _pos ].attrib = _var;
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

QString KWString::toString(unsigned int _pos,unsigned int _len)
{
  QString str = "";
  char c = 1;

  if (_pos + _len <= _len_)
    {
      for (unsigned int i = _pos;i <= _len + _pos;i++)
	{
	  if (static_cast<int>(i) > static_cast<int>(size() - 1)) break;
	  if (_data_[i].c != 0)
	    str += _data_[i].c;
	  else
	    str += c;
	}
    }

  return QString(str);
}

void KWString::saveFormat(ostream &out)
{
  unsigned int start = 0;

  for (unsigned int i = 0;i < _len_;i++)
    {
      if (_data_[i].attrib->getClassId() != ID_KWCharFormat)
	{
	  if (start < i)
	    {
	      out << otag << "<FORMAT id=\"" << _data_[start].attrib->getClassId() << "\" pos=\"" << start
		  << "\" len=\"" << i - start << "\">" << endl;
	      _data_[start].attrib->save(out);
	      out << etag << "</FORMAT>" << endl;
	    }
	  switch (_data_[i].attrib->getClassId())
	    {
	    case ID_KWCharImage:
	      {
		out << otag << "<FORMAT id=\"" << _data_[i].attrib->getClassId() << "\" pos=\"" << i << "\">" << endl;
		_data_[i].attrib->save(out);
		out << etag << "</FORMAT>" << endl;
	      } break;
	    case ID_KWCharTab:
	      {
		out << otag << "<FORMAT id=\"" << _data_[i].attrib->getClassId() << "\" pos=\"" << i << "\">" << endl;
		out << etag << "</FORMAT>" << endl;
	      } break;
	    default: break;
	    }
	  start = i + 1;
	}
      else if (i > 0 && _data_[i].attrib->getClassId() == ID_KWCharFormat &&
	       !(*((KWCharFormat*)_data_[i].attrib) == *((KWCharFormat*)_data_[i - 1].attrib)))
	{
	  if (start < i)
	    {
	      out << otag << "<FORMAT id=\"" << _data_[start].attrib->getClassId() << "\" pos=\"" << start
		  << "\" len=\"" << i - start << "\">" << endl;
	      _data_[start].attrib->save(out);
	      out << etag << "</FORMAT>" << endl;
	    }
	  start = i;
	}
    }

  if (start < _len_)
    {
      out << otag << "<FORMAT id=\"" << _data_[start].attrib->getClassId() << "\" pos=\"" << start
	  << "\" len=\"" << _len_ - start << "\">" << endl;
      _data_[start].attrib->save(out);
      out << etag << "</FORMAT>" << endl;
    }
}

void KWString::loadFormat(KOMLParser& parser,vector<KOMLAttrib>& lst,KWordDocument *_doc,KWTextFrameSet *_frameset)
{
  string tag;
  string name;

  while (parser.open(0L,tag))
    {
      KOMLParser::parseTag(tag.c_str(),name,lst);
	
      // format
      if (name == "FORMAT")
	{
	  ClassIDs _id = ID_KWCharNone;
	  unsigned int __pos = 0,__len = 0;
	  KWFormat *_format = 0L,*format = 0L;
	  KWImage *_image = 0L,*image = 0L;
	  KWCharImage *_kwimage = 0L;
	  KWCharFormat *_kwformat = 0L;
	  KWCharTab *_kwtab = 0L;
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  bool _load = false;
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "id")
		{
		  _id = static_cast<ClassIDs>(atoi((*it).m_strValue.c_str()));
		  _load = true;
		}
	      else if ((*it).m_strName == "pos")
		__pos = atoi((*it).m_strValue.c_str());
	      else if ((*it).m_strName == "len")
		__len = atoi((*it).m_strValue.c_str());
	    }
	  if (_load)
	    {
	      switch (_id)
		{
		case ID_KWCharFormat:
		  {
		    _format = new KWFormat();
		    _format->load(parser,lst,_doc);
		    format = _doc->getFormatCollection()->getFormat(*_format);
		    for (unsigned int i = __pos;i < __pos + __len;i++)
		      {
			if (static_cast<int>(i) > static_cast<int>(size() - 1)) break;
			freeChar(_data_[i]);
			_kwformat = new KWCharFormat(format);
			_data_[i].attrib = _kwformat;
			format->incRef();
		      }
		    format->decRef();
		    delete _format;
		    _format = 0;
		  } break;
		case ID_KWCharImage:
		  {
		    _image = new KWImage();
		    _image->load(parser,lst,_doc);
		    QString key;
		    image = _doc->getImageCollection()->getImage(*_image,key);
		    _kwimage = new KWCharImage(image);
		    freeChar(_data_[__pos]);
		    _data_[__pos].c = 0;
		    _data_[__pos].attrib = _kwimage;
		    delete _image;
		    _image = 0;
		  } break;
		case ID_KWCharTab:
		  {
		    _kwtab = new KWCharTab();
		    freeChar(_data_[__pos]);
		    _data_[__pos].c = 0;
		    _data_[__pos].attrib = _kwtab;
		  } break;
		default: break;
		}
	      _load = false;
	    }
	}

      if (!parser.close(tag))
	{
	  cerr << "ERR: Closing Child" << endl;
	  return;
	}
    }
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
		attrib->getFormat()->incRef();
		KWCharFormat *f = new KWCharFormat(attrib->getFormat());
		__data[i].attrib = f;
	      } break;
	    case ID_KWCharImage:
	      {
		KWCharImage *attrib = (KWCharImage*)_data[i].attrib;
		attrib->getImage()->incRef();
		KWCharImage *f = new KWCharImage(attrib->getImage());
		__data[i].attrib = f;
	      } break;
	    case ID_KWCharTab:
	      {
		KWCharTab *f = new KWCharTab();
		__data[i].attrib = f;
	      } break;
	    case ID_KWCharVariable:
	      {
		KWCharFormat *attrib = dynamic_cast<KWCharVariable*>(_data[i].attrib);
		attrib->getFormat()->incRef();
		KWCharVariable *f = new KWCharVariable(dynamic_cast<KWCharVariable*>(_data[i].attrib)->getVar()->copy());
		f->setFormat(attrib->getFormat());
		__data[i].attrib = f;
	      } break;
	    }
	}
      else __data[i].attrib = 0L;
    }
  return __data;
}

int KWString::find(QString _expr,KWSearchDia::KWSearchEntry *_format,int _index,bool _cs,bool _whole)
{
  QString str = toString(0,size());
  int res = str.find(_expr,_index,_cs);

  if (res != -1)
    {
      if (!_format && !_whole) return res;

      if (!_format && _whole)
	{
	  if ((res == 0 || res > 0 && str[res - 1] == ' ') &&
	      (res + static_cast<int>(_expr.length()) == static_cast<int>(_len_) || res + static_cast<int>(_expr.length()) <
	       static_cast<int>(_len_) && str[res + _expr.length()] == ' '))
	    return res;
	  return -2;
	}

      KWFormat *format;
      for (unsigned int i = 0;i < _expr.length();i++)
	{	
	  if (_data_[i + res].attrib->getClassId() != ID_KWCharFormat)
	    return -2;
	
	  format = dynamic_cast<KWCharFormat*>(_data_[i + res].attrib)->getFormat();
	
	  if (_format->checkFamily && _format->family != format->getUserFont()->getFontName())
	    return -2;
	  if (_format->checkColor && _format->color != format->getColor())
	    return -2;
	  if (_format->checkSize && _format->size != format->getPTFontSize())
	    return -2;
	  if (_format->checkBold && _format->bold != (format->getWeight() == QFont::Bold))
	    return -2;
	  if (_format->checkItalic && _format->italic != format->getItalic())
	    return -2;
	  if (_format->checkUnderline && _format->underline != format->getUnderline())
	    return -2;
	  if (_format->checkVertAlign && _format->vertAlign != format->getVertAlign())
	    return -2;
	}

      if (!_whole)
	return res;
      else
	{
	  if ((res == 0 || res > 0 && str[res - 1] == ' ') &&
	      (res + static_cast<int>(_expr.length()) == static_cast<int>(_len_) || res + static_cast<int>(_expr.length()) <
	       static_cast<int>(_len_) && str[res + _expr.length()] == ' '))
	    return res;
	  return -2;
	}
    }
  else return -1;
}

int KWString::find(QRegExp _regexp,KWSearchDia::KWSearchEntry *_format,int _index,int &_len,bool _cs,bool _wildcard = false)
{
  QString str = toString(0,size());
  _regexp.setWildcard(_wildcard);
  _regexp.setCaseSensitive(_cs);
  int res = _regexp.match(str,_index,&_len);

  if (res != -1)
    {
      if (!_format) return res;

      KWFormat *format;
      for (int i = 0;i < _len;i++)
	{	
	  if (_data_[i + res].attrib->getClassId() != ID_KWCharFormat)
	    return -2;
	
	  format = dynamic_cast<KWCharFormat*>(_data_[i + res].attrib)->getFormat();
	
	  if (_format->checkFamily && _format->family != format->getUserFont()->getFontName())
	    return -2;
	  if (_format->checkColor && _format->color != format->getColor())
	    return -2;
	  if (_format->checkSize && _format->size != format->getPTFontSize())
	    return -2;
	  if (_format->checkBold && _format->bold != (format->getWeight() == QFont::Bold))
	    return -2;
	  if (_format->checkItalic && _format->italic != format->getItalic())
	    return -2;
	  if (_format->checkUnderline && _format->underline != format->getUnderline())
	    return -2;
	  if (_format->checkVertAlign && _format->vertAlign != format->getVertAlign())
	    return -2;
	}

      return res;
    }
  else return -1;
}

int KWString::findRev(QString _expr,KWSearchDia::KWSearchEntry *_format,int _index,bool _cs,bool _whole)
{
  QString str = toString(0,size());
  int res = str.findRev(_expr,_index,_cs);

  if (res != -1)
    {
      if (!_format && !_whole) return res;

      if (!_format && _whole)
	{
	  if ((res == 0 || res > 0 && str[res - 1] == ' ') &&
	      (res + static_cast<int>(_expr.length()) == static_cast<int>(_len_) || res + static_cast<int>(_expr.length()) <
	       static_cast<int>(_len_) && str[res + _expr.length()] == ' '))
	    return res;
	  return -2;
	}

      KWFormat *format;
      for (unsigned int i = 0;i < _expr.length();i++)
	{	
	  if (_data_[i + res].attrib->getClassId() != ID_KWCharFormat)
	    return -2;
	
	  format = dynamic_cast<KWCharFormat*>(_data_[i + res].attrib)->getFormat();
	
	  if (_format->checkFamily && _format->family != format->getUserFont()->getFontName())
	    return -2;
	  if (_format->checkColor && _format->color != format->getColor())
	    return -2;
	  if (_format->checkSize && _format->size != format->getPTFontSize())
	    return -2;
	  if (_format->checkBold && _format->bold != (format->getWeight() == QFont::Bold))
	    return -2;
	  if (_format->checkItalic && _format->italic != format->getItalic())
	    return -2;
	  if (_format->checkUnderline && _format->underline != format->getUnderline())
	    return -2;
	  if (_format->checkVertAlign && _format->vertAlign != format->getVertAlign())
	    return -2;
	}

      if (!_whole)
	return res;
      else
	{
	  if ((res == 0 || res > 0 && str[res - 1] == ' ') &&
	      (res + static_cast<int>(_expr.length()) == static_cast<int>(_len_) || res + static_cast<int>(_expr.length()) <
	       static_cast<int>(_len_) && str[res + _expr.length()] == ' '))
	    return res;
	  return -2;
	}
    }
  else return -1;
}

int KWString::findRev(QRegExp _regexp,KWSearchDia::KWSearchEntry *_format,int _index,int &_len,bool _cs,bool _wildcard = false)
{
  return -1;
}

void freeChar( KWChar& _char )
{
  if ( _char.attrib )
  {
    switch( _char.attrib->getClassId() )
      {
      case ID_KWCharFormat:
      case ID_KWCharImage:
      case ID_KWCharTab:
      case ID_KWCharVariable:
	delete _char.attrib;
	break;
      default:
	assert( 0 );
      }
    _char.attrib = 0L;
  }
}

ostream& operator<<(ostream &out,KWString &_string)
{
  char c = 1;

  for (unsigned int i = 0;i < _string.size();i++)
    {
      if (_string.data()[i].c != 0)
	out << _string.data()[i].c;
      else
	out << c;
    }

  return out;
}
