#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "parag.h"
#include "kword_doc.h"

#include <koIMR.h>
#include <komlMime.h>
#include <koStream.h>
#include <strstream>
#include <fstream>
#include <unistd.h>

KWParag::KWParag( KWordDocument_impl *_doc, KWParag* _prev, KWParag* _next, KWParagLayout* _paragLayout )
{
    prev = _prev;
    next = _next;
    paragLayout = new KWParagLayout(_doc);
    *paragLayout = *_paragLayout;
    document = _doc;
    
    if ( prev )
	prev->setNext( this );
    else
	document->setFirstParag( this );
    
    if ( next )
	next->setPrev( this );

    startPage = 1;
    startColumn = 1;
    endColumn = 1;
    ptYStart = 0;
    ptYEnd = 0;
}

KWParag::~KWParag()
{
}

void KWParag::updateCounters( KWFormatContext *_format )
{
    if ( paragLayout->getCounterNr() == -1 )
	return;
    
    KWParagLayout *pl = paragLayout->getNumberLikeParagLayout();

    for ( int i = 0; i < 10; i++ )
    {
	counters[i] = _format->getCounter( paragLayout->getCounterNr(), i );
	if ( pl )
	    numberLikeCounters[i] = _format->getCounter( pl->getCounterNr(), i );
    }
}

QString& KWParag::makeCounterText( QString& _str )
{
    char buffer[128];
    
    if ( paragLayout->getCounterLeftText() )
	_str = paragLayout->getCounterLeftText();
    else
	_str = "";

    if ( paragLayout->getNumberLikeParagLayout() )
    {
	int depth = paragLayout->getNumberLikeParagLayout()->getCounterDepth();
	for ( int i = 0; i <= depth; i++ )
	{
	    sprintf( buffer, "%i", numberLikeCounters[i] );
	    _str += buffer;
	    if ( i < depth || paragLayout->getCounterDepth() != -1 )
		_str += ".";
	}
    }

    int depth = paragLayout->getCounterDepth();
    for ( int i = 0; i <= depth; i++ )
    {
	sprintf( buffer, "%i", counters[i] );
	_str += buffer;
	if ( i < depth )
	    _str += ".";
    }
    
    if ( paragLayout->getCounterRightText() )
    {
	_str += paragLayout->getCounterRightText();
	_str += "   ";
    }
    else
	_str += "   ";

    return _str;
}

void KWParag::insertText( unsigned int _pos, const char *_text)
{
  text.insert( _pos, _text);
}

void KWParag::insertPictureAsChar(unsigned int _pos,QString _filename)
{
  KWImage _image = KWImage(document,_filename);
  QString key;

  KWImage *image = document->getImageCollection()->getImage(_image,key);
  KWCharImage *i = new KWCharImage(image);

  text.insert(_pos,i);
}

void KWParag::appendText(KWChar *_text,unsigned int _len)
{
  text.append(_text,_len);
}

bool KWParag::deleteText( unsigned int _pos, unsigned int _len = 1)
{
  return text.remove( _pos, _len );
}

void KWParag::setFormat( unsigned int _pos, unsigned int _len, const KWFormat &_format )
{
  assert( _pos < text.size() );
  
  for (unsigned int i = 0;i < _len;i++)
    {
      freeChar( text.data()[ _pos + i] );
      KWFormat *format = document->getFormatCollection()->getFormat(_format);
      KWCharFormat *f = new KWCharFormat(format);
      text.data()[ _pos + i ].attrib = f;
    }
}

void KWParag::save(ostream &out)
{
  out << indent << "<TEXT value=\"" << text << "\"/>" << endl;
  text.saveFormat(out);
  out << otag << "<LAYOUT>" << endl;
  paragLayout->save(out);
  out << etag << "</LAYOUT>" << endl;
}
