/**
 * $Id$
 */

#include "KRTFTokenizer.h"

#include <qfile.h>
#include <ctype.h>

KRTFTokenizer::KRTFTokenizer( QFile* file )
{
    _file = file;
}


KRTFToken* KRTFTokenizer::nextToken()
{
    // first check whether there is anything on the pushback stack and
    // use that instead
    if( !_pushbackstack.isEmpty() ) {
	return _pushbackstack.pop();
    }

    QString text;
    QString param;
    KRTFToken* token = new KRTFToken( this );
    if( _file->atEnd() ) {
	token->_type = TokenEOF;
	return token;
    }
    int ch = _file->getch();
    // skip leading whitespace
    while( (isspace( ch )) && (!_file->atEnd()) )
	ch=_file->getch();



    // first find out what this is
    if( ch == '\\' ) {
	// type is either control word or control symbol
	ch = _file->getch();
	if( !isalnum( ch ) ) {
	    // control symbol
	    token->_type = ControlSymbol;
	    token->_text += (char)ch;
	} else {
	    // control word
	    token->_type = ControlWord;
	    _file->ungetch( ch );
	}
    } else if( ch == '{' ) {
	token->_type = OpenGroup;
    } else if( ch == '}' ) {
	token->_type = CloseGroup;
    } else {
	token->_type = ::PlainText;
	_file->ungetch( ch );
    }

    // Lump together what might be needed additionally.
    switch( token->_type ) {
    case ControlWord:
	ch = _file->getch();
	while( isalpha( ch ) ) {
	    text += (char)ch;
	    ch = _file->getch();
	}
	// ch is not alpha, control word is over, let´s see what we do about it
	if( ch == ' ' ) {
	    // space: part of the control word, which ends here FIXME really ?
	    //text += ' ';
	} else if( isdigit( ch ) || ( ch == '-' ) ) {
	    // digit or hyphen, build up numeric parameter
	    param += (char)ch;
	    ch = _file->getch();
	    while( isalnum( ch ) ) {
   		param += (char)ch;
	   	ch = _file->getch();
	    }
       _file->ungetch(ch);
	} else {
       _file->ungetch(ch);
   }

	token->_text = text;
	token->_param = param;
	break;
    case ::PlainText:
	// everything until next backslash, opener or closer
	ch = _file->getch();
	while( ch != '\\' && ch != '{'  && ch != '}' && !_file->atEnd() ) {
	    text += (char)ch;
	    ch = _file->getch();
	}
	token->_text = text;
	// give back last char
	_file->ungetch( ch );
    }
#if 0
    QString typenam="no type";
    if (token->_type==ControlWord) typenam=("controlword");
    if (token->_type==ControlSymbol) typenam=("controlsymbol");
    if (token->_type==OpenGroup) typenam=("opengroup");
    if (token->_type==CloseGroup) typenam=("closegroup");
    if (token->_type==::PlainText) typenam=("plaintext");
    if (token->_type==Unknown) typenam=("unknown");
    if (token->_type==TokenEOF) typenam=("eof");
    char a[2];
    a[0]=ch;
    a[1]=0;
    qWarning((tr("TOKEN type=\"")+typenam+"\" text=\""+token->_text+"\" params=\"" +token->_param+"\"").latin1());
#endif
    return token;
}


void KRTFTokenizer::pushBack( KRTFToken* token )
{
    _pushbackstack.push( token );
}

