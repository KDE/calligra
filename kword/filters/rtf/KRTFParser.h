/**
 * $Id$
 */

#ifndef __KRTFPARSER_H__
#define __KRTFPARSER_H__

class KRTFTokenizer;

class KRTFParser
{
protected:
    void skipGroup();
    KRTFTokenizer* _tokenizer;
};


#endif
