/**
 * $Id$
 */

#ifndef __KRTFDOCUMENTPARSER_H__
#define __KRTFDOCUMENTPARSER_H__

#include "KRTFParser.h"
#include "koStore.h"
#include <qdom.h>
class KRTFTokenizer;

class KRTFDocumentParser : public KRTFParser
{
public:
    KRTFDocumentParser( KRTFTokenizer* tokenizer, KoStore *store, QDomDocument *dom) {
      	_tokenizer = tokenizer;
        _store = store;
        _dom = dom;
    }

    bool parse();
    bool parseSubgroup();
    bool NewParagraph();
    bool NewFormat();
    bool FormatAttribute(QString name, QString attrName, QString attr);
    bool LayoutAttribute(QString name, QString attrName, QString attr);

    QDomDocument *_dom;
    QDomElement _currentParagraph;
    QDomElement _currentFormats;
    QString    _currentText;
    QDomElement _currentFormat;
    QDomElement _currentLayout;
    
private:
};

#endif
