/**
 * $Id$
 */

#ifndef __KRTFHEADERPARSER_H__
#define __KRTFHEADERPARSER_H__

#include "KRTFParser.h"
#include "koStore.h"
#include <qdom.h>
class KRTFTokenizer;


class KRTFHeaderParser : public KRTFParser
{
public:
    KRTFHeaderParser( KRTFTokenizer* tokenizer, KoStore *store, QDomDocument *dom ) {
        _tokenizer = tokenizer;
        _store = store;
        _dom = dom;
    }

    bool parse();
    QDomDocument *_dom;

private:
};

#endif
