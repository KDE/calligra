/**
 * $Id$
 */

#ifndef __KRTFDOCUMENTPARSER_H__
#define __KRTFDOCUMENTPARSER_H__

#include "KRTFParser.h"

class KRTFTokenizer;

class KRTFDocumentParser : public KRTFParser
{
public:
    KRTFDocumentParser( KRTFTokenizer* tokenizer ) {
	_tokenizer = tokenizer;
    }

    bool parse();
    
private:
};

#endif
