/**
 * $Id:$
 */

#include "KRTFHeaderParser.h"
#include "KRTFToken.h"
#include "KRTFTokenizer.h"

bool KRTFHeaderParser::parse()
{
    // Syntax for <header> is:
    // \rtf <charset> \deff? <fonttbl> <filetbl>? <colortbl>? <stylesheet>? <listtables>? <revtbl>?

    // First keyword must be \rtf
    KRTFToken* token = _tokenizer->nextToken();
    if( token->_type != ControlWord &&
	token->_text != "rtf" ) {
	warning( "File does not start with \rtf" );
	delete token;
	return false;
    }
    if( token->_param.left( 1 ) != "1" ) {
	warning( "Wrong version of RTF file, this reader can only read RTF 1.x files" );
	delete token;
	return false;
    }
    
    // PENDING(kalle): Understand the keywords that are possible here, like \ansi
    delete token;
    // skip over unknown keywords until we find the fonttbl group
    token = _tokenizer->nextToken();
    
    

}
