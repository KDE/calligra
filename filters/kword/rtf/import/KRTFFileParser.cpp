/**
 * $Id$
 */

#include "KRTFFileParser.h"
#include "KRTFToken.h"
#include "KRTFTokenizer.h"
#include "KRTFHeaderParser.h"
#include "KRTFDocumentParser.h"
#include "qdom.h"

bool KRTFFileParser::parse()
{
    // Syntax for a RTF file is:
    // '{' <header> <document> '}'
    QDomDocument _dom("root");
    KRTFToken* token = _tokenizer->nextToken();
    if( token->_type != OpenGroup )
	{ qWarning((token->_text +"_type\n").latin1()); return false; }
    KRTFHeaderParser header( _tokenizer , _store,&_dom);
    if( !header.parse() )
	{ qWarning((token->_text +" headerparse\n").latin1()); return false; }
    KRTFDocumentParser document( _tokenizer, _store,&_dom );
    if( !document.parse() )
	{ qWarning((token->_text +" documentparse\n").latin1()); return false; }
    token = _tokenizer->nextToken();
    if( token->_type != CloseGroup )
	{ qWarning((token->_text +" no closegroup\n").latin1());  }

   QCString strOut=_dom.toCString();
   _store->write((const char *)strOut, strOut.length());
    return true;
}
