/**
 * $Id$
 */

#include "KRTFParser.h"
#include "KRTFToken.h"
#include "KRTFTokenizer.h"

/*!
 * Reads and discards all tokens until the current group is closed.
 */
void KRTFParser::skipGroup()
{
    int bracelevel = 1; // first brace has already been opened

    while( bracelevel > 0 ) {
	KRTFToken* token = _tokenizer->nextToken();
	if( token->_type == OpenGroup ) {
	    bracelevel++;
	}
	else if( token->_type == CloseGroup ) {
	    bracelevel--;
	}
	else if( token->_type == ControlWord ) {}
	    
	// don´t need to do anything with the other token types
    };
}

