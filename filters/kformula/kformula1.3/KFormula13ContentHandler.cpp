/* This file is part of the KDE project
   Copyright (C) 2006 Martin Pfeiffer <hubipete@gmx.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "KFormula13ContentHandler.h"


KFormula13ContentHandler::KFormula13ContentHandler()
                         : QXmlDefaultHandler()
{
}

KFormula13ContentHandler::~KFormula13ContentHandler()
{
}

bool KFormula13ContentHandler::startDocument()
{
    
    QDomDocumentType dt = QDomImplementation().createDocumentType( "math",
                                         "-//W3C//DTD MathML 2.0//EN",
                                         "http://www.w3.org/TR/MathML2/dtd/mathml2.dtd");
    m_mathMlDocument = QDomDocument( dt );
    QDomProcessingInstruction in = m_mathMlDocument.createProcessingInstruction( "xml",
                                                  "version=\"1.0\" encoding=\"UTF-8\"" );
    m_mathMlDocument.insertBefore( in, m_mathMlDocument.documentElement() );
}

bool KFormula13ContentHandler::startElement( const QString&,
                                             const QString& localName,
                                             const QString&,
                                             const QXmlAttributes& atts )
{
    QDomElement element;
    m_currentElement.appendChild( element );
    m_currentElement = element;
    
    if( localName == "SEQUENCE" )
        m_currentElement.setTagName( "mrow" ); 
    else if( localName == "BRACKET" )
    {
        m_currentElement.setTagName( "mfenced" );
	writeBracketAttributes( atts );
    }
    else if( localName == "OVERLINE" )
    {
        m_currentElement.setTagName( "mover" );
	QDomElement tmp;
	tmp.setTagName( "mo" );
	tmp.setNodeValue( "&OverBar;" );
        m_currentElement.parentNode().appendChild( tmp );
    }
    else if( localName == "UNDERLINE" )
    {
        m_currentElement.setTagName( "munder" );
	QDomElement tmp;
	tmp.setTagName( "mo" );
	tmp.setNodeValue( "&UnderBar;" );
        m_currentElement.parentNode().appendChild( tmp );
    }
    else if( localName == "FRACTION" )
    {
        m_currentElement.setTagName( "mfrac" );
	if( atts.value( "NOLINE" ).toInt() == 1 )
            m_currentElement.setAttribute( "linethickness", "0" );
    }
}

bool KFormula13ContentHandler::endElement( const QString&,
                                           const QString&,
                                           const QString& )
{
    m_currentElement = m_currentElement.parentNode().toElement();
}

bool KFormula13ContentHandler::characters( const QString& ch ) 
{
    // TODO kddebug stuff here but normaly that should not be used
}

void KFormula13ContentHandler::writeBracketAttributes( const QXmlAttributes& atts )
{
    switch ( atts.value( "LEFT" ).toInt() )
    {
        case 91:
            m_currentElement.setAttribute( "open", "[" );
            break;
        case 123:
            m_currentElement.setAttribute( "open", "{" );
            break;
	case 256:
            m_currentElement.setAttribute( "open", "|" );
            break;
    }
    
    switch ( atts.value( "RIGHT" ).toInt() )
    {
        case 93:
            m_currentElement.setAttribute( "close", "]" );
            break;
        case 125:
            m_currentElement.setAttribute( "close", "}" );
            break;
	case 257:
            m_currentElement.setAttribute( "close", "|" );
            break;
    }   
}

