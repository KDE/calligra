/*
   This file is part of the KDE project
   Copyright (C) 2001 Ewald Snel <ewald@rambo.its.tudelft.nl>
   Copyright (C) 2001 Tomasz Grobelny <grotk@poczta.onet.pl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/

#include <qdatetime.h>
#include <stdio.h>
#include <string.h>
#include "rtfimport_dom.h"


DomNode::DomNode()
{
    setBuffer( array );
    clear();
}

/**
 * Creates a new document.
 * @param doctype the document type (tag)
 */
DomNode::DomNode( const char *doctype )
{
    setBuffer( array );
    open( IO_WriteOnly );
    documentLevel	= 1;
    hasChildren		= false;
    hasAttributes	= false;
    writeBlock( "<?xml version = '1.0' encoding = 'UTF-8'?><!DOCTYPE ", 52 );
    writeBlock( doctype, strlen( doctype ) );
    writeBlock( " ><", 3 );
    writeBlock( doctype, strlen( doctype ) );
}

/**
 * Creates a new document node (no memory allocation).
 * @param level the document depth of the node
 */
void DomNode::clear( int level )
{
    close();
    array.truncate( 0 );
    setBuffer( array );
    open( IO_WriteOnly | IO_Truncate );
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
    writeBlock( " <", 2 );
    writeBlock( name, strlen( name ) );
    hasChildren = false;
    ++documentLevel;
}

/**
 * Adds a text node.
 * @param text the text to write into the document node
 */
void DomNode::addTextNode( const char *text )
{
    closeTag( false );

    if (text == 0)
    {
	text = "";
    }
    char *amp = strchr( text, '&' );
    char *lt = strchr( text, '<' );

    // Escape &amp; and &lt;
    while (amp || lt)
    {
	if (amp && (!lt || amp < lt))
	{
	    writeBlock( text, (amp - text) );
	    writeBlock( "&amp;", 5 );
	    text = (amp + 1);
	    amp = strchr( text, '&' );
	}
	else
	{
	    writeBlock( text, (lt - text) );
	    writeBlock( "&lt;", 4 );
	    text = (lt + 1);
	    lt = strchr( text, '<' );
	}
    }
    writeBlock( text, strlen( text ) );
}

/**
 * Add border to existing frameset (see KWord DTD).
 */
void DomNode::addBorder( int id, QColor &color, int style, double width )
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
void DomNode::addColor( QColor &color )
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
void DomNode::addKey( QDateTime dt, const char *filename, const char *name )
{
    QDate date = dt.date();
    QTime time = dt.time();

    addNode( "KEY" );
    setAttribute( "filename", filename );
    setAttribute( "year", date.year() );
    setAttribute( "month", date.month() );
    setAttribute( "day", date.day() );
    setAttribute( "hour", time.hour() );
    setAttribute( "minute", time.minute() );
    setAttribute( "second", time.second() );
    setAttribute( "msec", time.msec() );

    if (name)
    {
	setAttribute( "name", name );
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
    putch( ' ' );
    writeBlock( attribute, strlen( attribute ) );
    putch( '=' );
    putch( '"' );
    writeBlock( value, strlen( value ) );
    putch( '"' );
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
	    putch( ' ' );
	}
	putch( '/' );
    }
    else
    {
	writeBlock( "</", 2 );
	writeBlock( name, strlen( name ) );
    }
    writeBlock( ">\n", 2 );

    for (int i=--documentLevel; i > 1; i--)
    {
	putch( ' ' );
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
	    putch( ' ' );
	}
	putch( '>' );

	if (nl)
	{
	    putch( '\n' );

	    for (int i=documentLevel; i > 1; i--)
	    {
		putch( ' ' );
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
void DomNode::appendNode( DomNode &child )
{
    QByteArray &arr = child.data();
    closeTag( (arr.size() >= 2 && (arr[0] == '<' || arr[1] == '<')) );
    writeBlock( arr );
}

/**
 * Returns true if node is empty.
 */
bool DomNode::isEmpty(  )
{
    return array.isEmpty();
}

/**
 * Returns the data of the document node.
 */
QByteArray &DomNode::data()
{
    return array;
}
