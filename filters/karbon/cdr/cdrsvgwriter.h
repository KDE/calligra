/* This file is part of the Calligra project, made within the KDE community.

   Copyright 2012 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef CDRSVGWRITER_H
#define CDRSVGWRITER_H

// Calligra
#include <KoXmlWriter.h>

class CdrDocument;
class CdrPage;
class CdrLayer;
class CdrGroupObject;
class CdrRectangleObject;
class CdrEllipseObject;
class CdrPathObject;
class CdrTextObject;
class CdrAbstractObject;


class CdrSvgWriter
{
public:
    explicit CdrSvgWriter( QIODevice* device );
    ~CdrSvgWriter();

public:
    bool write( CdrDocument* document );

private:
    void writePage( const CdrPage* page );
    void writeLayer( const CdrLayer* layer );
    void writeObject( const CdrAbstractObject* object );
    void writeGroupObject( const CdrGroupObject* groupObject );
    void writeRectangleObject( const CdrRectangleObject* object );
    void writeEllipseObject( const CdrEllipseObject* object );
    void writePathObject( const CdrPathObject* pathObject );
    void writeTextObject( const CdrTextObject* object );

    void writeFillColor( quint32 fillId );
    void writeStrokeColor( quint32 outlineId );
    void writeStrokeWidth( quint32 outlineId );
    void writeFont( quint16 styleId );

private:
    KoXmlWriter mXmlWriter;

    CdrDocument* mDocument;
};

#endif
