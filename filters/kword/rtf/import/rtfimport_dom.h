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

#include <qbuffer.h>
#include <qcolor.h>
#include <qcstring.h>
#include <qdatetime.h>

class DomNode : public QBuffer
{
public:
    DomNode();
    DomNode( const char *doctype );

    void clear( int level=0 );
    void addNode( const char *name );
    void addTextNode( const char *text );
    void addBorder( int id, QColor &color, int style, double width );
    void addColor( QColor &color );
    void addRect( int left, int top, int right, int bottom );
    void addKey( QDateTime dt, const char *filename, const char *name = 0L );
    void addFrameSet( const char *name, int frameType, int frameInfo );
    void addFrame( int left, int top, int right, int bottom,
		   int autoCreateNewFrame, int newFrameBehaviour,
		   int sheetSide );
    void setAttribute( const char *name, const char *value );
    void setAttribute( const char *name, int value );
    void setAttribute( const char *name, double value );
    void closeNode( const char *name );
    void closeTag( bool nl );
    void appendNode( DomNode &child );
    bool isEmpty();
    QByteArray &data();

private:
    QByteArray array;
    int documentLevel;
    bool hasChildren;
    bool hasAttributes;
};

#endif
