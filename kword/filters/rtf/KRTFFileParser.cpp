/**
 * $Id$
 */

#include "KRTFFileParser.h"
#include "KRTFToken.h"
#include "KRTFTokenizer.h"
#include "KRTFHeaderParser.h"
#include "KRTFDocumentParser.h"

bool KRTFFileParser::parse()
{
    // Syntax for a RTF file is:
    // '{' <header> <document> '}'
    KRTFToken* token = _tokenizer->nextToken();
    if( token->_type != OpenGroup )
	return false;
    KRTFHeaderParser header( _tokenizer );
    if( !header.parse() )
	return false;
    KRTFDocumentParser document( _tokenizer );
    if( !document.parse() )
	return false;
    token = _tokenizer->nextToken();
    if( token->_type != CloseGroup )
	return false;
    return true;
}
