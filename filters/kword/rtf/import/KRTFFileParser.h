/**
 * $Id$
 */

#ifndef __KRTFFILEPARSER_H__
#define __KRTFFILEPARSER_H__

#include "KRTFParser.h"

class KRTFTokenizer;
class QDomDocument;

class KRTFFileParser : public KRTFParser
{
public:
    KRTFFileParser( KRTFTokenizer* tokenizer, KoStore *store) {
	_tokenizer = tokenizer;
        _store = store;
    }

    bool parse();
private:
};

#endif
