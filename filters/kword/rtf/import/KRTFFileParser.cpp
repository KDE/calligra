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

    // @kervel: This code is "borrowed" from KWord (kwdoc.cc:1787)
    // I just added the default values you had in your "string"
    QDomDocument _dom("DOC");
    _dom.appendChild( _dom.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );
    QDomElement kwdoc = _dom.createElement( "DOC" );
    kwdoc.setAttribute( "editor", "RTF Import Filter" );
    kwdoc.setAttribute( "mime", "application/x-kword" );
    //m_syntaxVersion = CURRENT_SYNTAX_VERSION;
    //kwdoc.setAttribute( "syntaxVersion", m_syntaxVersion );
    _dom.appendChild( kwdoc );

    QDomElement paper = _dom.createElement( "PAPER" );
    kwdoc.appendChild( paper );
    paper.setAttribute( "format", 1 );
    paper.setAttribute( "width", 595 );
    paper.setAttribute( "height", 841 );
    paper.setAttribute( "orientation", 0 );
    paper.setAttribute( "columns", 1 );
    paper.setAttribute( "columnspacing", 3 );
    paper.setAttribute( "hType", 0 );
    paper.setAttribute( "fType", 0 );

    QDomElement borders = _dom.createElement( "PAPERBORDERS" );
    paper.appendChild( borders );
    borders.setAttribute( "left", 20 );
    borders.setAttribute( "top", 10 );
    borders.setAttribute( "right", 10 );
    borders.setAttribute( "bottom", 10 );

    QDomElement docattrs = _dom.createElement( "ATTRIBUTES" );
    kwdoc.appendChild( docattrs );
    docattrs.setAttribute( "processing", 0 );
    docattrs.setAttribute( "standardpage", 1 );
    docattrs.setAttribute( "hasHeader", 0 );
    docattrs.setAttribute( "hasFooter", 0 );
    docattrs.setAttribute( "unit", "mm" );

    KRTFToken* token = _tokenizer->nextToken();
    if( token->_type != OpenGroup )
	{ qWarning((token->_text +"_type\n").latin1()); return false; }
    KRTFHeaderParser header( _tokenizer , _store,&_dom);
    if( !header.parse() )
	{ qWarning((token->_text +" headerparse\n").latin1()); return false; }
    KRTFDocumentParser document( _tokenizer, _store,&_dom );
    if( !document.parse() )
	{ qWarning((token->_text +" error documentparse\n").latin1()); return false; }
    token = _tokenizer->nextToken();
    if( token->_type != CloseGroup )
	{ qWarning((token->_text +" warning: no closegroup\n").latin1());  }

   QCString strOut=_dom.toCString();
   _store->write((const char *)strOut, strOut.length());
    return true;
}
