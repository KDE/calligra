/*
   This file is part of the KDE project
   Copyright (C) 2001 Ewald Snel <ewald@rambo.its.tudelft.nl>
   Copyright (C) 2001 Tomasz Grobelny <grotk@poczta.onet.pl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/

#ifndef __RTFIMPORT_DOM_H__
#define __RTFIMPORT_DOM_H__

#include <qstring.h>
//Added by qt3to4:
#include <Q3CString>

class QColor;
class QDateTime;
class QTextCodec;

/**
 * Escape the special XML characters and be careful to catch any unallowed control character
 */
QString CheckAndEscapeXmlText(const QString& strText);

class DomNode
{
public:
    DomNode();
    /**
     * Creates a new document.
     * @param doctype the document type (tag)
     */
    DomNode( const char *doctype );
    /**
     * Creates a new document node (no memory allocation).
     * @param level the document depth of the node
     */
    void clear( int level=0 );
    /**
     * Adds a new node.
     * @param name the name of the new node (tag)
     */
    void addNode( const char *name );
    /**
     * Adds a text node.
     * @param text the text to write into the document node
     * @param codec the codec from the source document, to be used for encoding.
     */
    void addTextNode( const char *text, QTextCodec* codec );
    /**
     * Add border to existing frameset (see KWord DTD).
     */
    void addBorder( int id, const QColor &color, int style, double width );
    /**
     * Add color attributes to document node.
     * @param color the color
     */
    void addColor( const QColor &color );
    /**
    * Add rectangle attributes to document node.
    */
    void addRect( int left, int top, int right, int bottom );
    /**
     * Add pixmap or clipart key.
     * @param dt date/time
     * @param filename the filename of the image
     * @param name the relative path to the image in the store (optional)
     */
    void addKey( const QDateTime& dt, const QString& filename, const QString& name = QString::null );
    /**
     * Add frameset to document (see KWord DTD).
     */
    void addFrameSet( const char *name, int frameType, int frameInfo );
    /**
     * Add frame to existing frameset (see KWord DTD).
     */
    void addFrame( int left, int top, int right, int bottom,
		   int autoCreateNewFrame, int newFrameBehaviour,
		   int sheetSide );
    /**
     * Sets a new attribute to a string value.
     */
    void setAttribute( const QString& attribute, const QString& value );
    /**
     * Sets a new attribute to an integer value.
     */
    void setAttribute( const char *name, int value );
    /**
     * Sets a new attribute to a double value.
     */
    void setAttribute( const char *name, double value );
    /**
     * Closes a document node.
     * @param name the node (tag) to close
     */
    void closeNode( const char *name );
    /**
     * Closes the current XML tag (if open).
     * @param nl add a newline
     */
    void closeTag( bool nl );
    /**
     * Appends a child node.
     * @param child the node to append to this document node
     */
    void appendNode( const DomNode &child );
    /**
     * Appends XML text to node
     */
    void append( const Q3CString& cstr);
    void append( const QString& _str);
    void append( const char ch);
    /**
     * Returns true if node is empty.
     */
    bool isEmpty() const;
    /**
     * Returns the data of the document node.
     */
    QString toString() const;

private:
    QString str;
    int documentLevel;
    bool hasChildren;
    bool hasAttributes;
};

#endif
