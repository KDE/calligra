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

    // kervel: basic document header.
    // @kervel: I don't think that using QDomDocument that way makes any sense
    // here. It won't work if you mix setContent and "real" dom node creation.
    // Therefore I suggest either just use QDomDocument and really create nodes and stuff
    // or just pass around a string. (Werner)
    /*
    QString str;
    str += "<?xml version=\"1.0\" encoding=\"UTF-8\"?><!DOCTYPE DOC >\n";
    str += "<DOC  editor=\"KWord\" mime=\"application/x-kword\">\n";
    str += "<PAPER format=\"1\" width=\"595\" height=\"841\" orientation=\"0\" columns=\"1\" hType=\"0\" fType=\"0\" >\n";
    str += "<PAPERBORDERS left=\"28\" top=\"42\" right=\"28\" bottom=\"42\" />\n";
    str += "</PAPER>\n";
    str += "<ATTRIBUTES processing=\"0\" standardpage=\"1\" hasHeader=\"0\" hasFooter=\"0\" unit=\"mm\"/>\n";
    str += "<FRAMESETS>\n";
    str += "<FRAMESET frameType=\"1\" autoCreateNewFrame=\"1\" frameInfo=\"0\" removeable=\"0\">\n";
    str += "<FRAME left=\"28\" top=\"42\" right=\"566\" bottom=\"798\" />\n";
    str += "</FRAMESET></FRAMESETS></DOC>\n";
    _dom->setContent(str);
    */

    // the currently read token should already be the end of the header,
    // no need to overread anything

    return true;
}
