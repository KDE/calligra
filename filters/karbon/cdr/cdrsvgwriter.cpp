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
    mDocument = document;

    mXmlWriter.addCompleteElement("<?xml version=\"1.0\" standalone=\"no\"?>\n");
    mXmlWriter.addCompleteElement("<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 20010904//EN\" "
                                  "\"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">\n");

    // add some PR.  one line is more than enough.
    mXmlWriter.addCompleteElement("<!-- Created using Karbon, part of Calligra: http://www.calligra.org/karbon -->\n");
    mXmlWriter.startElement("svg");

    foreach( const CdrPage* page, mDocument->pages() )
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

    foreach( const CdrAbstractObject* object, layer->objects() )
        writeObject( object );

    mXmlWriter.endElement(); // g
}

void
CdrSvgWriter::writeObject( const CdrAbstractObject* object )
{
    const CdrObjectTypeId typeId = object->typeId();
    mXmlWriter.startElement("g");

    QString tfString;
    foreach(const CdrAbstractTransformation* transformation, object->transformations())
    {
        const CdrNormalTransformation* normalTrafo =
            dynamic_cast<const CdrNormalTransformation*>(transformation);

        if( normalTrafo )
            tfString.append( QString::fromLatin1("translate(%1,%2) ").arg(normalTrafo->x()).arg(normalTrafo->y()) );
    }
    mXmlWriter.addAttribute( "transform", tfString );

    if( typeId == PathObjectId )
        writePathObject( static_cast<const CdrPathObject*>(object) );
    else if( typeId == RectangleObjectId )
        writeRectangleObject( static_cast<const CdrRectangleObject*>(object) );
    else if( typeId == EllipseObjectId )
        writeEllipseObject( static_cast<const CdrEllipseObject*>(object) );
    else if( typeId == TextObjectId )
        writeTextObject( static_cast<const CdrTextObject*>(object) );
    else if( typeId == GroupObjectId )
        writeGroupObject( static_cast<const CdrGroupObject*>(object) );
    mXmlWriter.endElement(); // g
}

void
CdrSvgWriter::writeGroupObject( const CdrGroupObject* groupObject )
{
    mXmlWriter.startElement("g");

    foreach( const CdrAbstractObject* object, groupObject->objects() )
        writeObject( object );

    mXmlWriter.endElement(); // g
}

void
CdrSvgWriter::writeRectangleObject( const CdrRectangleObject* object )
{
    mXmlWriter.startElement("rect");

//     mXmlWriter.addAttribute("x", x);
//     mXmlWriter.addAttribute("y", y);
    mXmlWriter.addAttribute("width", object->cornerPoint().x() );
    mXmlWriter.addAttribute("height", object->cornerPoint().y() );
//     mXmlWriter.addAttribute("rx", object->cornerRoundness());
//     mXmlWriter.addAttribute("ry", object->cornerRoundness());
    writeStrokeWidth( object->outlineId() );
    writeStrokeColor( object->outlineId() );
    writeFillColor( object->fillId() );

    mXmlWriter.endElement(); // rect
}

void
CdrSvgWriter::writeEllipseObject( const CdrEllipseObject* object )
{
    mXmlWriter.startElement("ellipse");

    mXmlWriter.addAttribute("cx", object->centerPoint().x());
    mXmlWriter.addAttribute("cy", -object->centerPoint().y());
    mXmlWriter.addAttribute("rx", object->xRadius());
    mXmlWriter.addAttribute("ry", object->yRadius());
    writeStrokeWidth( object->outlineId() );
    writeStrokeColor( object->outlineId() );
    writeFillColor( object->fillId() );

    mXmlWriter.endElement(); // ellipse
}

void
CdrSvgWriter::writePathObject( const CdrPathObject* pathObject )
{
    mXmlWriter.startElement("path");

    const QVector<CdrPathPoint>& pathPoints = pathObject->pathPoints();
    QString pathData;
    for( int j=0; j<pathPoints.count(); j++ )
    {
        if(j==0) // is first point
            pathData.append( QString::fromLatin1("M %1 %2 ").arg( pathPoints[0].mPoint.x() ).arg( -pathPoints[0].mPoint.y() ) );
        else
        {
            const bool isLineStarting = (pathPoints[j].mType == 0x0C);

            if( isLineStarting )
                pathData.append( QString::fromLatin1("M %1 %2 ").arg( pathPoints[j].mPoint.x() ).arg( -pathPoints[j].mPoint.y() ) );
            else
            {
                pathData.append( QString::fromLatin1("L %1 %2 ").arg( pathPoints[j].mPoint.x() ).arg( -pathPoints[j].mPoint.y() ) );

                const bool isLineEnding = (pathPoints[j].mType == 0x48);
                if( isLineEnding )
                    pathData.append( QLatin1String("z ") );
            }
        }
    }
    mXmlWriter.addAttribute( "d", pathData );
    writeStrokeWidth( pathObject->outlineId() );
    writeStrokeColor( pathObject->outlineId() );
    writeFillColor( pathObject->fillId() );

    mXmlWriter.endElement(); // path
}

void
CdrSvgWriter::writeTextObject( const CdrTextObject* object )
{
    mXmlWriter.startElement("text");

//     writeStrokeWidth( object->outlineId() );
    writeStrokeColor( object->outlineId() );
    writeFillColor( object->fillId() );
    writeFont( object->styleId() );
    mXmlWriter.addTextNode( object->text() );

    mXmlWriter.endElement(); // text
}

void
CdrSvgWriter::writeFillColor( quint32 fillId )
{
    CdrAbstractFill* fill = mDocument->fill( fillId );
    const QString colorName = ( fill && fill->id() == CdrAbstractFill::Solid ) ?
        static_cast<CdrSolidFill*>( fill )->color().name() :
        QString::fromLatin1("none");
    mXmlWriter.addAttribute("fill", colorName);
}

void
CdrSvgWriter::writeStrokeColor( quint32 outlineId )
{
    CdrOutline* outline = mDocument->outline( outlineId );
    const QString colorName = ( outline ) ?
        outline->color().name() :
        QString::fromLatin1("none");
    mXmlWriter.addAttribute("stroke", colorName);
}

void
CdrSvgWriter::writeStrokeWidth( quint32 outlineId )
{
    CdrOutline* outline = mDocument->outline( outlineId );
    const quint16 lineWidth = ( outline ) ? outline->lineWidth() : 0;
    mXmlWriter.addAttribute("stroke-width", QString::number(lineWidth) );
}

void
CdrSvgWriter::writeFont( quint16 styleId )
{
    CdrStyle* style = mDocument->style( styleId );
    const quint16 fontSize = ( style ) ? style->fontSize() : 18; // TODO: default font size?
    mXmlWriter.addAttribute("font-size", QString::number(fontSize) );
    if( style )
    {
        CdrFont* font = mDocument->font( style->fontId() );
        if( font )
            mXmlWriter.addAttribute("font-family", font->name() );
    }
}
