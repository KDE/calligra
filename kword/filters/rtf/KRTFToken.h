/**
 * $Id$
 */

#ifndef __KRTFTOKEN_H__
#define __KRTFTOKEN_H__

#include <qstring.h>
#include <qobject.h>

enum KRTFTokenType { TokenEOF = -1, Unknown, ControlWord, ControlSymbol, OpenGroup, CloseGroup, PlainText };

class KRTFToken : public QObject
{
public:
    KRTFTokenType _type;
    QString _text;
    QString _param;

    KRTFToken( QObject* parent ) :
	QObject( parent ) 
	{
	    _type = Unknown;
	}
};



#endif
