#include <string.h>
#include <stdio.h>

#include "parag.h"
#include "kword_doc.h"

KWParag::KWParag( KWordDocument_impl *_doc, KWParag* _prev, KWParag* _next, KWParagLayout* _paragLayout )
{
    prev = _prev;
    next = _next;
    paragLayout = _paragLayout;
    document = _doc;
    
    if ( prev )
	prev->setNext( this );
    else
	document->setFirstParag( this );
    
    if ( next )
	next->setPrev( this );

    maxTextLen = 2048;
    text = new char[ maxTextLen ];
    textLen = 0;
    
    startPage = 1;
    startColumn = 1;
    endColumn = 1;
    ptYStart = 0;
    ptYEnd = 0;
}

KWParag::~KWParag()
{
    if ( text )
	delete []text;
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

void KWParag::insertText( unsigned int _pos, const char *_text )
{
    int len = strlen( _text );
    if ( textLen + len > maxTextLen )
    {
	maxTextLen = textLen + len + 1024;
	char *p = new char[ maxTextLen ];
	memcpy( p, text, _pos );
	memcpy( p + _pos, _text, len );
	memcpy( p + _pos + len, text + _pos, textLen - _pos );
	textLen += len;
	delete []text;
	text = p;
    }
    else
    {
	memmove( text + _pos + len, text + _pos, textLen - _pos );
	memcpy( text + _pos, _text, len );
	textLen += len;
    }
}

