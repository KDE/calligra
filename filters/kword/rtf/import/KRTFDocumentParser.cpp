/**
 * $Id$
 */

#include "KRTFDocumentParser.h"
#include "KRTFToken.h"
#include "KRTFTokenizer.h"
#include "qdom.h"

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
               NewParagraph();
	    parseSubgroup();

	}	    
	token = _tokenizer->nextToken();
    }
    return true;
}

bool KRTFDocumentParser::NewParagraph() {
NewFormat();
if (_currentText.length()) {
   QDomElement __text=_dom->createElement("TEXT");          //     <TEXT>_currentText</TEXT>
   __text.appendChild(_dom->createTextNode(_currentText));  //
   _currentParagraph.appendChild(__text);                          //
   _currentParagraph.appendChild(_currentFormats);             //    <FORMATS>blabla</FORMATS>
   _dom->elementsByTagName("FRAMESET").item(0).appendChild(_currentParagraph); // huidige paragraaf invoegen
}
 _currentParagraph=_dom->createElement("PARAGRAPH"); // new paragraph
 _currentFormats=_dom->createElement("FORMATS");      // formats ready for new paragraph
 _currentText=QString::null;				// no text
}

bool KRTFDocumentParser::NewFormat() {
if (_currentFormat.attribute("pos") != QString::null) {
     _currentFormats.appendChild(_currentFormat);
}

_currentFormat=_dom->createElement("FORMAT");

}


bool KRTFDocumentParser::parseSubgroup() {
    KRTFToken *token = _tokenizer->nextToken();
    NewFormat();
    while (token->_type != CloseGroup) {
     switch (token->_type) {
       case OpenGroup: parseSubgroup();
			break;
       case ControlWord:
                                 break;
       case ::PlainText:
			_currentFormat.setAttribute("id","1");
			_currentFormat.setAttribute("pos",QString("%1").arg(_currentText.length()));
			_currentFormat.setAttribute("len",QString("%1").arg(token->_text.length()));
			_currentText+=token->_text;
			break;
       case ControlSymbol:
                                  break;

     }

      token=_tokenizer->nextToken();
   }

}