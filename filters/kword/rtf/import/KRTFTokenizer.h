/**
 * $Id$
 */

#ifndef __KRTFTOKENIZER_H__
#define __KRTFTOKENIZER_H__

#include "KRTFToken.h"
#include <qptrstack.h>
#include <qobject.h>

class QFile;

class KRTFTokenizer : public QObject
{
public:
    KRTFTokenizer( QFile* file );
    
    KRTFToken* nextToken();
    void pushBack( KRTFToken* token );

private:
    QFile* _file;
    QPtrStack<KRTFToken> _pushbackstack;
};

#endif
