/**
 * $Id$
 */

#ifndef __KRTFFILEPARSER_H__
#define __KRTFFILEPARSER_H__

#include "KRTFParser.h"

class KRTFTokenizer;

class KRTFFileParser : public KRTFParser
{
public:
    KRTFFileParser( KRTFTokenizer* tokenizer ) {
	_tokenizer = tokenizer;
    }

    bool parse();
    
private:
};

#endif
