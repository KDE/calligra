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
    KRTFToken token = _tokenizer->nextToken();
    if( token._type != ControlWord &&
	token._text != "rtf" ) {
	warning( "File does not start with \rtf" );
	return false;
    }
    if( token._param.left( 1 ) != "1" ) {
	warning( "Wrong version of RTF file, this reader can only read RTF 1.x files" );
	return false;
    }
    
    // PENDING(kalle): Understand the keywords that are possible here, like \ansi

    // skip over unknown keywords until we find the fonttbl group
    token = _tokenizer->nextToken();
    while( token._type == ControlWord ) {
	debug( "Skipping control word %s", token._text.data() );
	token = _tokenizer->nextToken();
    }

    
    // If there are group here, check whether it is one that we know of
    bool done = false;
    while( token._type == OpenGroup && !done ) {
	token = _tokenizer->nextToken();
	if( token._text == "fonttbl" ) {
	    // PENDING(kalle) parse fonttbl here instead
	    debug( "Skipping fonttbl group" );
	    skipGroup();
	    token = _tokenizer->nextToken();
	}
	else if( token._text == "filetbl" ) {
	    // PENDING(kalle) parse filetbl here instead
	    debug( "Skipping filetbl group" );
	    skipGroup();
	    token = _tokenizer->nextToken();
	} else if( token._text == "colortbl" ) {
	    // PENDING(kalle) parse colortbl here instead
	    debug( "Skipping colortbl group" );
	    skipGroup();
	    token = _tokenizer->nextToken();
	} else if( token._text == "stylesheet" ) {
	    // PENDING(kalle) parse stylesheet here instead
	    debug( "Skipping stylesheet group" );
	    skipGroup();
	    token = _tokenizer->nextToken();
	} else if( token._text == "listtables" ) {
	    // PENDING(kalle) parse listtables here instead
	    debug( "Skipping listtables group" );
	    skipGroup();
	    token = _tokenizer->nextToken();
	} else if( token._text == "revtables" ) {
	    // PENDING(kalle) parse revtables here instead
	    debug( "Skipping revtables group" );
	    skipGroup();
	    token = _tokenizer->nextToken();
	} else
	    // no group that we know of, next must be the document
	    done = true;
    }

    // the currently read token should already be the end of the header,
    // no need to overread anything

    return true;
}
