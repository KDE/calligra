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

#ifndef XMLWRITER_H
#define XMLWRITER_H

#include <qiodevice.h>
#include <qstring.h>
#include <qvaluestack.h>

class KoXmlWriter
{
public:
    KoXmlWriter( QIODevice* dev );
    ~KoXmlWriter();

    void startElement( const char* xmlName );
    inline void addAttribute( const char* attrName, const QString& value ) {
        addAttribute( attrName, value.utf8() );
    }
    inline void addAttribute( const char* attrName, int value ) {
        QCString str;
        str.setNum( value );
        addAttribute( attrName, str.data() );
    }
    inline void addAttribute( const char* attrName, const QCString& value ) {
        addAttribute( attrName, value.data() );
    }
    // Overload for fixed-value attributes
    void addAttribute( const char* attrName, const char* value );
    void endElement();
    inline void addTextNode( const QString& str ) {
        addTextNode( str.utf8() );
    }
    inline void addTextNode( const QCString& cstr ) {
        addTextNode( cstr.data() );
    }
    void addTextNode( const char* cstr );
    // ## To be replaced with the full thing
    void writeDocType( const char* elemName );

private:
    struct Tag {
        Tag( const char* t = 0 ) : tagName( t ), hasChildren( false ),
                                   openingTagClosed( false ), lastChildIsText( false ) {}
        const char* tagName;
        bool hasChildren; // element or text children
        bool openingTagClosed; // true once the '>' in <xml a="b"> is written out
        bool lastChildIsText;
    };

    /// Write out \n followed by the number of spaces required.
    void writeIndent();

    // writeCString is much faster than writeString.
    // Try to use it as much as possible, especially with constants.
    void writeString( const QString& str );

    // unused and possibly incorrect if length != size
    //inline void writeCString( const QCString& cstr ) {
    //    m_dev->writeBlock( cstr.data(), cstr.size() - 1 );
    //}

    inline void writeCString( const char* cstr ) {
        m_dev->writeBlock( cstr, qstrlen( cstr ) );
    }
    inline void writeChar( char c ) {
        m_dev->putch( c );
    }
    inline void closeStartElement( Tag& tag ) {
        if ( !tag.openingTagClosed ) {
            tag.openingTagClosed = true;
            writeChar( '>' );
        }
    }
    char* escapeForXML( const char* source ) const;

    QIODevice* m_dev;
    QValueStack<Tag> m_tags;

    char* m_indentBuffer;
    char* m_escapeBuffer;
    static const int s_escapeBufferLen = 10000;
};

#endif /* XMLWRITER_H */

