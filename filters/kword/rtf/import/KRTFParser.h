/**
 * $Id$
 */

#ifndef __KRTFPARSER_H__
#define __KRTFPARSER_H__

class KRTFTokenizer;
class KoStore;
class QString;
class KRTFParser
{
protected:
    void skipGroup();
    KRTFTokenizer* _tokenizer;
    KoStore* _store;
};


#endif
