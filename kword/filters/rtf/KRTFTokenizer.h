/**
 * $Id$
 */

#ifndef __KRTFTOKENIZER_H__
#define __KRTFTOKENIZER_H__

#include "KRTFToken.h"

class QFile;

class KRTFTokenizer
{
public:
    KRTFTokenizer( QFile* file );
    
    KRTFToken nextToken();

private:
    QFile* _file;
};

#endif
