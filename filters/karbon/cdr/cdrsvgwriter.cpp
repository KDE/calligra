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

#include "cdrsvgwriter.h"

// filter
#include "cdrdocument.h"


CdrSvgWriter::CdrSvgWriter( QIODevice* device )
  : mXmlWriter( device )
{
}

CdrSvgWriter::~CdrSvgWriter()
{
}

bool
CdrSvgWriter::write( CdrDocument* document )
{
    mXmlWriter.addCompleteElement("<?xml version=\"1.0\" standalone=\"no\"?>\n");
    mXmlWriter.addCompleteElement("<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 20010904//EN\" " \
                                   "\"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">\n");

    // add some PR.  one line is more than enough.
    mXmlWriter.addCompleteElement("<!-- Created using Karbon, part of Calligra: http://www.calligra.org/karbon -->\n");
    mXmlWriter.startElement("svg");

    foreach( const CdrPage* page, document->pages() )
        writePage( page );

    mXmlWriter.endElement(); // svg
    mXmlWriter.endDocument();
    return true;
}

void
CdrSvgWriter::writePage( const CdrPage* page )
{
    foreach( const CdrLayer* layer, page->layers() )
        writeLayer( layer );
}

void
CdrSvgWriter::writeLayer( const CdrLayer* layer )
{
    mXmlWriter.startElement("g");

    foreach( const CdrObject* object, layer->objects() )
        writeObject( object );

    mXmlWriter.endElement(); // g
}

void
CdrSvgWriter::writeObject( const CdrObject* object )
{
    const CdrObjectId id = object->id();

    if( id == PathObjectId )
        writePathObject( dynamic_cast<const CdrPathObject*>(object) );
    else if( id == GroupObjectId )
        writeGroupObject( dynamic_cast<const CdrGroupObject*>(object) );
}

void
CdrSvgWriter::writeGroupObject( const CdrGroupObject* groupObject )
{
    mXmlWriter.startElement("g");

    foreach( const CdrObject* object, groupObject->objects() )
        writeObject( object );

    mXmlWriter.endElement(); // g
}

void
CdrSvgWriter::writePathObject( const CdrPathObject* pathObject )
{
    mXmlWriter.startElement("path");

    const QVector<Cdr4PathPoint>& pathPoints = pathObject->pathPoints();
    QString pathData;
    for (unsigned int j=0; j<pathPoints.count(); j++)
    {
        if(j==0) // is first point
            pathData.append( QString::fromLatin1("M %1 %2 ").arg( pathPoints[0].mPoint.mX ).arg( pathPoints[0].mPoint.mY ) );
        else
        {
            const bool isLineStarting = (pathPoints[j].mType == 0x0C);

            if( isLineStarting )
                pathData.append( QString::fromLatin1("M %1 %2 ").arg( pathPoints[j].mPoint.mX ).arg( pathPoints[j].mPoint.mY ) );
            else
            {
                pathData.append( QString::fromLatin1("L %1 %2 ").arg( pathPoints[j].mPoint.mX ).arg( pathPoints[j].mPoint.mY ) );

                const bool isLineEnding = (pathPoints[j].mType == 0x48);
                if( isLineEnding )
                    pathData.append( QLatin1String("z ") );
            }
        }
    }
    mXmlWriter.addAttribute( "d", pathData );

    mXmlWriter.endElement(); // path
}
