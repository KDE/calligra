/**
 * $Id$
 */

#include "KRTFDocumentParser.h"
#include "KRTFToken.h"
#include "KRTFTokenizer.h"

bool KRTFDocumentParser::parse()
{
    // Format of document:
    // <info>? <docfmt>* <section>+
    
    KRTFToken* token = _tokenizer->nextToken();
    if( token->_type == OpenGroup ) {
	// opening brace? this could be the info group
	KRTFToken* token = _tokenizer->nextToken();
	if( token->_type == ControlWord  && token->_text == "info" ) {
	    // PENDING(kalle) Understand info group instead
	    skipGroup();
	}
	else
	    _tokenizer->pushBack( token );
    }

    // loop through all the control words and groups that are
    // following, understand as many of them as possible
    token = _tokenizer->nextToken();
    while( token->_type == ControlWord || token->_type == OpenGroup ) {
	if( token->_type == ControlWord ) {
	    // PENDING(kalle) Understand at least some of them
	} else {
	    // group
	    // PENDING(kalle) Understand at least some of them
	    skipGroup();
	}	    
	token = _tokenizer->nextToken();
    }

    return true;
}
