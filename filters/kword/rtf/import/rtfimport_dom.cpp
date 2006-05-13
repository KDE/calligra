/*
   This file is part of the KDE project
   Copyright (C) 2001 Ewald Snel <ewald@rambo.its.tudelft.nl>
   Copyright (C) 2001 Tomasz Grobelny <grotk@poczta.onet.pl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/

#include <stdio.h>

#include <QDateTime>
#include <QTextCodec>
#include <QColor>
//Added by qt3to4:
#include <QByteArray>

#include <kdebug.h>

#include "rtfimport_dom.h"

/**
 * Escape the special XML characters and be careful to catch any unallowed control character
 */
QString CheckAndEscapeXmlText(const QString& strText)
{
    QString strReturn(strText);
    QChar ch;

    for (uint i=0; i<strReturn.length(); i++)
    {
        ch = strReturn[i];
        const int test = ch.unicode();

        // The i+= is for the additional characters
        if (test == 38) { strReturn.replace(i, 1, "&amp;"); i+=4; } // &
        else if (test == 60) { strReturn.replace(i, 1, "&lt;"); i+=3; } // <
        else if (test == 62) { strReturn.replace(i, 1, "&gt;"); i+=3; } // >
        else if (test == 34) { strReturn.replace(i, 1, "&quot;"); i+=5; } // "
        else if (test == 39) { strReturn.replace(i, 1, "&apos;"); i+=5; } // '
        else if (test >= 32) continue; // Normal character (from space on)
        else if ((test == 9) || (test == 10) || (test == 13) ) continue; // Allowed control characters: TAB, LF, CR
        else
        {
            // An unallowed control character:
            // - could be a bug in the RTF file
            // - could be not supported encoding.
            // In any case, we must replace this character.
            kDebug(30515) << "Control character in XML stream: " << test << endl;
            strReturn.replace(i, 1, '?'); // Replacement character
        }
    }

    return strReturn;
}


DomNode::DomNode()
{
    clear(0);
}

/**
 * Creates a new document.
 * @param doctype the document type (tag)
 */
DomNode::DomNode( const char *doctype )
{
    documentLevel	= 1;
    hasChildren		= false;
    hasAttributes	= false;
    str += "<?xml version = '1.0' encoding = 'UTF-8'?><!DOCTYPE " ;
    str += doctype;
    str += " >\n<";
    str += doctype;
}

/**
 * Creates a new document node (no memory allocation).
 * @param level the document depth of the node
 */
void DomNode::clear( int level )
{
    str = QString::null;
    documentLevel	= level;
    hasChildren		= true;
    hasAttributes	= false;
}

/**
 * Adds a new node.
 * @param name the name of the new node (tag)
 */
void DomNode::addNode( const char *name )
{
    closeTag( true );
    str += " <";
    str += name;
    hasChildren = false;
    ++documentLevel;
}

/**
 * Adds a text node.
 * @param text the text to write into the document node
 */
void DomNode::addTextNode( const char *text, QTextCodec* codec )
{
    closeTag( false );

    if (!codec)
    {
        kError(30515) << "No QTextCodec available!" << endl;
        return;
    }

    str += CheckAndEscapeXmlText(codec->toUnicode(text));

}

/**
 * Add border to existing frameset (see KWord DTD).
 */
void DomNode::addBorder( int id, const QColor &color, int style, double width )
{
    char attr[16];
    sprintf( attr, "%cRed", id );
    setAttribute( attr, color.red() );
    sprintf( attr, "%cGreen", id );
    setAttribute( attr, color.green() );
    sprintf( attr, "%cBlue", id );
    setAttribute( attr, color.blue() );
    sprintf( attr, "%cStyle", id );
    setAttribute( attr, style );
    sprintf( attr, "%cWidth", id );
    setAttribute( attr, width );
}

/**
 * Add color attributes to document node.
 * @param color the color
 */
void DomNode::addColor( const QColor &color )
{
    setAttribute( "red", color.red() );
    setAttribute( "green", color.green() );
    setAttribute( "blue", color.blue() );
}

/**
 * Add rectangle attributes to document node.
 */
void DomNode::addRect( int left, int top, int right, int bottom )
{
    setAttribute( "left", .05*left );
    setAttribute( "top", .05*top );
    setAttribute( "right", .05*right );
    setAttribute( "bottom", .05*bottom );
}

/**
 * Add pixmap or clipart key.
 * @param dt date/time
 * @param filename the filename of the image
 * @param name the relative path to the image in the store (optional)
 */
void DomNode::addKey( const QDateTime& dt, const QString& filename, const QString& name )
{
    const QDate date ( dt.date() );
    const QTime time ( dt.time() );

    addNode( "KEY" );
    setAttribute( "filename", CheckAndEscapeXmlText(filename) );
    setAttribute( "year", date.year() );
    setAttribute( "month", date.month() );
    setAttribute( "day", date.day() );
    setAttribute( "hour", time.hour() );
    setAttribute( "minute", time.minute() );
    setAttribute( "second", time.second() );
    setAttribute( "msec", time.msec() );

    if (!name.isEmpty())
    {
        setAttribute( "name", CheckAndEscapeXmlText(name) );
    }
    closeNode( "KEY" );
}

/**
 * Add frameset to document (see KWord DTD).
 */
void DomNode::addFrameSet( const char *name, int frameType, int frameInfo )
{
    addNode( "FRAMESET" );
    setAttribute( "name", name );
    setAttribute( "frameType", frameType );
    setAttribute( "frameInfo", frameInfo );
    setAttribute( "removable", 0 );
    setAttribute( "visible", 1 );
}

/**
 * Add frame to existing frameset (see KWord DTD).
 */
void DomNode::addFrame( int left, int top, int right, int bottom,
			int autoCreateNewFrame, int newFrameBehaviour,
			int sheetSide )
{
    addNode( "FRAME" );
    addRect( left, top, right, bottom );
    setAttribute( "runaround", 1 );
    setAttribute( "runaroundGap", 2 );
    setAttribute( "autoCreateNewFrame", autoCreateNewFrame );
    setAttribute( "newFrameBehaviour", newFrameBehaviour );
    setAttribute( "sheetSide", sheetSide );
}

/**
 * Sets a new attribute to a string value.
 */
void DomNode::setAttribute( const QString& attribute, const QString& value )
{
    str += ' ';
    str += attribute;
    str += '=';
    str += '"';
    str += CheckAndEscapeXmlText( value );
    str += '"';
    hasAttributes = true;
}

/**
 * Sets a new attribute to an integer value.
 */
void DomNode::setAttribute( const char *attribute, int value )
{
    char strvalue[32];
    sprintf( strvalue, "%d", value );
    setAttribute( attribute, (const char *)strvalue );
}

/**
 * Sets a new attribute to a double value.
 */
void DomNode::setAttribute( const char *attribute, double value )
{
    char strvalue[32];
    sprintf( strvalue, "%f", value );
    setAttribute( attribute, (const char *)strvalue );
}

/**
 * Closes a document node.
 * @param name the node (tag) to close
 */
void DomNode::closeNode( const char *name )
{
    if (!hasChildren)
    {
        str += '/';
    }
    else
    {
        str += "</";
        str += name;
    }
    str += ">\n";

    --documentLevel;
    for (int i=documentLevel-1; i>0; i--)
    {
        str += ' ';
    }
    hasChildren = true;
}

/**
 * Closes the current XML tag (if open).
 * @param nl add a newline
 */
void DomNode::closeTag( bool nl )
{
    if (!hasChildren)
    {
        str += '>';

        if (nl)
        {
            str += '\n';

            for (int i=documentLevel-1; i>0; i--)
            {
                str += ' ';
            }
        }
        hasChildren = true;
    }
    hasAttributes = false;
}

/**
 * Appends a child node.
 * @param child the node to append to this document node
 */
void DomNode::appendNode( const DomNode &child )
{
    const QString childStr ( child.toString() );
    closeTag( (childStr.length() >= 2 && (childStr[0] == '<' || childStr[1] == '<')) );
    str += childStr;
}

/**
 * Appends XML text to node
 */
void DomNode::append( const QString& _str)
{
    str += _str;
}

void DomNode::append( const QByteArray& cstr)
{
    str += QString::fromUtf8(cstr);
}

void DomNode::append( const char ch)
{
    str += ch;
}

/**
 * Returns true if node is empty.
 */
bool DomNode::isEmpty( void ) const
{
    return str.isEmpty();
}

/**
 * Returns the data of the document node.
 */
QString DomNode::toString( void ) const
{
    return str;
}
