/* This file is part of the KDE project
   Copyright (C) 2004 David Faure <faure@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "koxmlwriter.h"
#include <kglobal.h> // kMin
#include <kdebug.h>
#include <float.h>

static const unsigned int s_indentBufferLength = 100;

KoXmlWriter::KoXmlWriter( QIODevice* dev )
    : m_dev( dev )
{
    init();
}

void KoXmlWriter::init()
{
    m_indentBuffer = new char[ s_indentBufferLength ];
    memset( m_indentBuffer, ' ', s_indentBufferLength );
    *m_indentBuffer = '\n'; // write newline before indentation, in one go

    m_escapeBuffer = new char[s_escapeBufferLen];
}

KoXmlWriter::KoXmlWriter( QIODevice* dev, const char* rootElemName )
    : m_dev( dev )
{
    init();

    startDocument( rootElemName );
    startElement( rootElemName );
    addAttribute( "xmlns:office", "urn:oasis:names:tc:openoffice:xmlns:office:1.0" );
    addAttribute( "xmlns:meta", "urn:oasis:names:tc:openoffice:xmlns:meta:1.0" );

    if ( qstrcmp( rootElemName, "office:document-meta" ) != 0 ) {
        addAttribute( "xmlns:config", "urn:oasis:names:tc:openoffice:xmlns:config:1.0" );
        addAttribute( "xmlns:text", "urn:oasis:names:tc:openoffice:xmlns:text:1.0" );
        addAttribute( "xmlns:table", "urn:oasis:names:tc:openoffice:xmlns:table:1.0" );
        addAttribute( "xmlns:draw", "urn:oasis:names:tc:openoffice:xmlns:drawing:1.0" );
        addAttribute( "xmlns:presentation", "urn:oasis:names:tc:openoffice:xmlns:presentation:1.0" );
        addAttribute( "xmlns:dr3d", "urn:oasis:names:tc:openoffice:xmlns:dr3d:1.0" );
        addAttribute( "xmlns:chart", "urn:oasis:names:tc:openoffice:xmlns:chart:1.0" );
        addAttribute( "xmlns:form", "urn:oasis:names:tc:openoffice:xmlns:form:1.0" );
        addAttribute( "xmlns:script", "urn:oasis:names:tc:openoffice:xmlns:script:1.0" );
        addAttribute( "xmlns:style", "urn:oasis:names:tc:openoffice:xmlns:style:1.0" );
        addAttribute( "xmlns:number", "urn:oasis:names:tc:openoffice:xmlns:datastyle:1.0" );
    }
    // missing: office:version="1.0"

    addAttribute( "xmlns:dc", "http://purl.org/dc/elements/1.1/" );
    addAttribute( "xmlns:xlink", "http://www.w3.org/1999/xlink" );
    addAttribute( "xmlns:math", "http://www.w3.org/1998/Math/MathML" );
    addAttribute( "xmlns:fo", "http://www.w3.org/1999/XSL/Format" );
    addAttribute( "xmlns:svg", "http://www.w3.org/2000/svg" );
}

KoXmlWriter::~KoXmlWriter()
{
    delete[] m_indentBuffer;
    delete[] m_escapeBuffer;
}

void KoXmlWriter::startDocument( const char* rootElemName, const char* publicId, const char* systemId )
{
    Q_ASSERT( m_tags.isEmpty() );
    writeCString( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" );
    writeCString( "<!DOCTYPE " );
    writeCString( rootElemName );
    if ( publicId ) {
        writeCString( " PUBLIC \"" );
        writeCString( publicId );
        writeCString( "\" \"" );
        writeCString( systemId );
        writeCString( "\"" );
    }
    writeCString( ">\n" );
}

void KoXmlWriter::endDocument()
{
    // just to do exactly like QDom does (newline at end of file).
    writeChar( '\n' );
    Q_ASSERT( m_tags.isEmpty() );
}

void KoXmlWriter::startElement( const char* tagName )
{
    Q_ASSERT( tagName != 0 );

    // Tell parent that it has children
    if ( !m_tags.isEmpty() ) {
        Tag& parent = m_tags.top();
        if ( !parent.hasChildren ) {
            closeStartElement( parent );
            parent.hasChildren = true;
        }
        // this is for cases like <a>\n<b>\n<c>foo</c>\n</b>text</a> (no \n around text)
        if ( !parent.lastChildIsText ) {
            writeIndent();
        }
        parent.lastChildIsText = false;
    }

    m_tags.push( Tag( tagName ) );
    writeChar( '<' );
    writeCString( tagName );
    //kdDebug() << k_funcinfo << tagName << endl;
}

void KoXmlWriter::endElement()
{
    Q_ASSERT( !m_tags.isEmpty() );
    Tag tag = m_tags.pop();
    //kdDebug() << k_funcinfo << " tagName=" << tag.tagName << " hasChildren=" << tag.hasChildren << endl;
    if ( !tag.hasChildren ) {
        writeCString( "/>" );
    }
    else {
        if ( !tag.lastChildIsText ) {
            writeIndent();
        }
        writeCString( "</" );
        Q_ASSERT( tag.tagName != 0 );
        writeCString( tag.tagName );
        writeChar( '>' );
    }
}

void KoXmlWriter::addTextNode( const char* cstr )
{
    Tag& parent = m_tags.top();
    if ( !parent.hasChildren ) {
        closeStartElement( parent );
        parent.hasChildren = true;
    }
    parent.lastChildIsText = true;

    char* escaped = escapeForXML( cstr );
    writeCString( escaped );
    if(escaped != m_escapeBuffer)
        delete[] escaped;
}

void KoXmlWriter::addAttribute( const char* attrName, const char* value )
{
    writeChar( ' ' );
    writeCString( attrName );
    writeCString("=\"");
    char* escaped = escapeForXML( value );
    writeCString( escaped );
    if(escaped != m_escapeBuffer)
        delete[] escaped;
    writeChar( '"' );
}

void KoXmlWriter::addAttribute( const char* attrName, double value )
{
    QCString str;
    str.setNum( value, 'g', DBL_DIG );
    addAttribute( attrName, str.data() );
}

void KoXmlWriter::addAttributePt( const char* attrName, double value )
{
    QCString str;
    str.setNum( value, 'g', DBL_DIG );
    str += "pt";
    addAttribute( attrName, str.data() );
}

void KoXmlWriter::writeIndent()
{
    // +1 because of the leading '\n'
    m_dev->writeBlock( m_indentBuffer, kMin( m_tags.size()+1, s_indentBufferLength ) );
}

void KoXmlWriter::writeString( const QString& str )
{
    // cachegrind says .utf8() is where most of the time is spent
    QCString cstr = str.utf8();
    m_dev->writeBlock( cstr.data(), cstr.size() - 1 );
}

// In case of a reallocation (ret value != m_buffer), the caller owns the return value,
// it must delete it (with [])
char* KoXmlWriter::escapeForXML( const char* source ) const
{
    // we're going to be pessimistic on char length; so lets make the outputLength less
    // the amount one char can take: 6
    char* destBoundary = m_escapeBuffer + s_escapeBufferLen - 6;
    char* destination = m_escapeBuffer;
    char* output = m_escapeBuffer;
    for ( ;; ) {
        if(destination >= destBoundary) {
            // When we come to realize that our escaped string is going to
            // be bigger than the escape buffer (this shouldn't happen very often...),
            // we drop the idea of using it, and we allocate a bigger buffer.
            uint len = qstrlen( source ); // expensive...
            uint newLength = len * 6 + 1; // worst case. 6 is due to &quot; and &apos;
            char* buffer = new char[ newLength ];
            destBoundary = buffer + newLength;
            uint amountOfCharsAlreadyCopied = destination - m_escapeBuffer;
            memcpy( buffer, m_escapeBuffer, amountOfCharsAlreadyCopied );
            output = buffer;
            destination = buffer + amountOfCharsAlreadyCopied;
        }
        switch( *source ) {
        case 60: // <
            memcpy( destination, "&lt;", 4 );
            destination += 4;
            break;
        case 62: // >
            memcpy( destination, "&gt;", 4 );
            destination += 4;
            break;
        case 34: // "
            memcpy( destination, "&quot;", 6 );
            destination += 6;
            break;
#if 0 // needed?
        case 39: // '
            memcpy( destination, "&apos;", 6 );
            destination += 6;
            break;
#endif
        case 38: // &
            memcpy( destination, "&amp;", 5 );
            destination += 5;
            break;
        case 0:
            *destination = '\0';
            return output;
        default:
            *destination++ = *source++;
            continue;
        }
        ++source;
    }
    // NOTREACHED (see case 0)
    return output;
}
