/**
 * $Id$
 */

#ifndef __KRTFHEADERPARSER_H__
#define __KRTFHEADERPARSER_H__

#include "KRTFParser.h"

class KRTFTokenizer;

class KRTFHeaderParser : public KRTFParser
{
public:
    KRTFHeaderParser( KRTFTokenizer* tokenizer ) {
	_tokenizer = tokenizer;
    }

    bool parse();
    
private:
};

#endif
