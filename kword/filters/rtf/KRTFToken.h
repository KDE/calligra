/**
 * $Id:$
 */

#ifndef __KRTFTOKEN_H__
#define __KRTFTOKEN_H__

#include <qstring.h>

enum KRTFTokenType { TokenEOF = -1, Unknown, ControlWord, ControlSymbol, OpenGroup, CloseGroup, PlainText };

struct KRTFToken
{
    KRTFTokenType _type;
    QString _text;
    QString _param;

    KRTFToken() {
	_type = Unknown;
    }

    KRTFToken( const KRTFToken& token ) {
	_type = token._type;
	_text = token._text;
	_param = token._param;
    }
    KRTFToken& operator=( const KRTFToken& token ) {
	if( this != &token ) {
	    this->_type = token._type;
	    this->_text = token._text;
	    this->_param = token._param;
	}
	return *this;
    }
};



#endif
