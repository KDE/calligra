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


    QDomElement framesets = _dom->createElement("FRAMESETS");
    _dom->elementsByTagName("DOC").item(0).appendChild(framesets);
    QDomElement mainframeset = _dom->createElement("FRAMESET");
    framesets.appendChild(mainframeset);
    mainframeset.setAttribute("frameType",1);
    mainframeset.setAttribute("frameInfo",0);
    mainframeset.setAttribute("name","Text-frameset 1");
    mainframeset.setAttribute("visible","1");
    QDomElement mainframe = _dom->createElement("FRAME");
    mainframeset.appendChild(mainframe);
    mainframe.setAttribute("runaround",1);
    mainframe.setAttribute("copy",0);
    mainframe.setAttribute("right",567);
    mainframe.setAttribute("left",28);
    mainframe.setAttribute("newFrameBehaviour",0);
    mainframe.setAttribute("bottom",799);
    mainframe.setAttribute("runaroundGap",2);
    mainframe.setAttribute("top",42);

    NewParagraph(); NewFormat();
    // ----------------------
	    parseSubgroup();
    NewParagraph();

    return true;
}

bool KRTFDocumentParser::NewParagraph() {
NewFormat();
_currentFormats=_dom->createElement("FORMATS");
_currentFormat=_dom->createElement("FORMAT");
_currentLayout=_dom->createElement("LAYOUT");

if (_currentText.length()) {
   QDomElement __text=_dom->createElement("TEXT");          //     <TEXT>_currentText</TEXT>
   __text.appendChild(_dom->createTextNode(_currentText));  //
   _currentParagraph.appendChild(__text);                          //
   _dom->elementsByTagName("FRAMESET").item(0).appendChild(_currentParagraph); // huidige paragraaf invoegen
}
 _currentParagraph=_dom->createElement("PARAGRAPH"); // new paragraph
 _currentFormats=_dom->createElement("FORMATS");      // formats ready for new paragraph
 _currentParagraph.appendChild(_currentFormats);             //    <FORMATS>blabla</FO
 _currentParagraph.appendChild(_currentLayout);
 _currentText=QString::null;				// no text
}

bool KRTFDocumentParser::NewFormat() {
if (_currentFormat.attribute("id") != QString::null) {
     _currentFormats.appendChild(_currentFormat);
    //_currentFormat=_currentFormat.cloneNode().toElement();
  } else {
  if (_currentFormat.tagName() != "FORMAT") {
       qWarning(" ----------------------------------------------- ");
       _currentFormat=_dom->createElement("FORMAT");
   }
 }
}

bool KRTFDocumentParser::FormatAttribute(QString name, QString attrName, QString attr) {
QDomNodeList qdnl= _currentFormat.elementsByTagName(name);

if (qdnl.length()) {
  QDomElement el;
  el=qdnl.item(0).toElement();
  el.setAttribute(attrName,attr);
} else {
  QDomElement al=_dom->createElement(name);
  _currentFormat.appendChild(al);
  al.setAttribute(attrName,attr);
}
}

bool KRTFDocumentParser::LayoutAttribute(QString name, QString attrName, QString attr) {
QDomNodeList qdnl= _currentLayout.elementsByTagName(name);

if (qdnl.length()) {
  QDomElement el;
  el=qdnl.item(0).toElement();
  el.setAttribute(attrName,attr);
} else {
  QDomElement al=_dom->createElement(name);
  _currentLayout.appendChild(al);
  al.setAttribute(attrName,attr);
}
}


bool KRTFDocumentParser::parseSubgroup() {
    KRTFToken *token = _tokenizer->nextToken();
    QDomElement oldFormat;

    while (token->_type != CloseGroup) {
     switch (token->_type) {
       case OpenGroup:
                       oldFormat=_currentFormat;
                       _currentFormat=_currentFormat.cloneNode().toElement();
                       NewFormat();
                       parseSubgroup();
                       _currentFormat=oldFormat;
			break;
       case ControlWord:  if (token->_text == "par") { NewParagraph(); }

                                  // bold , italic, underlined
                       if (token->_text == "b") {
                                       if ((token->_param) == "0") { FormatAttribute("WEIGHT","value","50"); }
                               else { FormatAttribute("WEIGHT","value","75"); }
                       }
                       if (token->_text == "i") {
                                       if ((token->_param) == "0") { FormatAttribute("ITALIC","value","0"); }
                               else { FormatAttribute("ITALIC","value","1"); }
                       }
                       if (token->_text == "ul") {
                                       if ((token->_param) == "0") { FormatAttribute("UNDERLINE","value","0"); }
                               else { FormatAttribute("UNDERLINE","value","1"); }
                       }

                       //  center , ...
                       if (token->_text=="qc") {
                          LayoutAttribute("FLOW","align","center");
                       }
                                  if (token->_text=="ql") {
                          LayoutAttribute("FLOW","align","left");
                       }
                       if (token->_text=="qr") {
                          LayoutAttribute("FLOW","align","right");
                       }
                                 break;
       case ::PlainText:
			_currentFormat.setAttribute("id","1");
			_currentFormat.setAttribute("pos",QString("%1").arg(_currentText.length()));
			_currentFormat.setAttribute("len",QString("%1").arg(token->_text.length()));
			_currentText+=token->_text;
			NewFormat();
			break;
       case ControlSymbol:
                                  break;
       case TokenEOF:
                       return true;
                       break;

     }

      token=_tokenizer->nextToken();
   }
	NewFormat();
}