#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "parag.h"
#include "kword_doc.h"

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
  
  KWFormat *format = document->getFormatCollection()->getFormat(_format);
  KWCharFormat *f = new KWCharFormat(format);

  for (unsigned int i = 0;i < _len;i++)
    {
      if (text.data()[_pos + i].attrib && text.data()[_pos + i].attrib->getClassId() == ID_KWCharFormat)
	{
	  ((KWCharFormat*)text.data()[_pos + i].attrib)->getFormat()->decRef();
	  ((KWCharFormat*)text.data()[_pos + i].attrib)->setFormat(0L);
	}
      freeChar( text.data()[ _pos + i] );
      text.data()[ _pos ].attrib = f;
    }
}




