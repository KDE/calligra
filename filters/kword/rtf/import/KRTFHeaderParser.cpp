/**
 * $Id$
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
    KRTFToken* token2;
    if( token->_type != ControlWord &&
	token->_text != "rtf" ) {
	qWarning( "File does not start with \rtf" );
	return false;
    }
    if( token->_param.left( 1 ) != "1" ) {
	qWarning( "Wrong version of RTF file, this reader can only read RTF 1.x files" );
	return false;
    }
    
    // PENDING(kalle): Understand the keywords that are possible here, like \ansi

    // skip over unknown keywords until we find the fonttbl group
    token = _tokenizer->nextToken();
    while( token->_type == ControlWord ) {
	token = _tokenizer->nextToken();
    }

    
    // If there are group here, check whether it is one that we know of
    bool done = false;
    token2 = token;
    while( token->_type == OpenGroup && !done ) {
	token = _tokenizer->nextToken();
	if( token->_text == "fonttbl" ) {
	    // PENDING(kalle) parse fonttbl here instead
	    skipGroup();
	    token = _tokenizer->nextToken();
	}
	else if( token->_text == "filetbl" ) {
	    // PENDING(kalle) parse filetbl here instead
	    skipGroup();
	    token = _tokenizer->nextToken();
	} else if( token->_text == "colortbl" ) {
	    // PENDING(kalle) parse colortbl here instead
	    skipGroup();
	    token = _tokenizer->nextToken();
	} else if( token->_text == "stylesheet" ) {
	    // PENDING(kalle) parse stylesheet here instead
	    skipGroup();
	    token = _tokenizer->nextToken();
	} else if( token->_text == "listtables" ) {
	    // PENDING(kalle) parse listtables here instead
	    skipGroup();
	    token = _tokenizer->nextToken();
	} else if( token->_text == "revtables" ) {
	    // PENDING(kalle) parse revtables here instead
	    skipGroup();
	    token = _tokenizer->nextToken();
	} else
	    // no group that we know of, next must be the document
	    done = true;
    }

    // Return the last token that was unrightfully taken from somewhere else
    _tokenizer->pushBack( token );
    _tokenizer->pushBack( token2 );

    // the currently read token should already be the end of the header,
    // no need to overread anything

    return true;
}
