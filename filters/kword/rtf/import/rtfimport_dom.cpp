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
#include <string.h>

#include <qdatetime.h>
#include <qtextcodec.h>
#include <qcolor.h>

#include <kdebug.h>

#include "rtfimport_dom.h"


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
    str += " ><";
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
        kdError(30515) << "No QTextCodec! Caller should have done it! (in DomNode::addTextNode)" << endl;
        return;
    }

    QString unicode(codec->toUnicode(text));

    unicode.replace('&',"&amp;")
        .replace('<',"&lt;")
        .replace('>',"&gt;");  // Needed for the sequence ]]>

    str += unicode;
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
void DomNode::addKey( const QDateTime& dt, const char *filename, const char *name )
{
    const QDate date ( dt.date() );
    const QTime time ( dt.time() );

    addNode( "KEY" );
    setAttribute( "filename", filename );// ### TODO: escape
    setAttribute( "year", date.year() );
    setAttribute( "month", date.month() );
    setAttribute( "day", date.day() );
    setAttribute( "hour", time.hour() );
    setAttribute( "minute", time.minute() );
    setAttribute( "second", time.second() );
    setAttribute( "msec", time.msec() );

    if (name)
    {
	setAttribute( "name", name );// ### TODO: escape
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
void DomNode::setAttribute( const char *attribute, const char *value )
{
    str += ' ';
    str += attribute;
    str += '=';
    str += '"';
    str += value;
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
        if (hasAttributes)
        {
            str += ' ';
        }
        str += '/';
    }
    else
    {
        str += "</";
        str += name;
    }
    str += ">\n";

    for (int i=--documentLevel; i > 1; i--)
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
        if (hasAttributes)
        {
            str += ' ';
        }
        str += '>';

        if (nl)
        {
            str += '\n';

            for (int i=0; i<documentLevel; i++)
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

void DomNode::append( const QCString& cstr)
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
