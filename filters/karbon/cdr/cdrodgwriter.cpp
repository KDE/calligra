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

#include "cdrodgwriter.h"

// filter
#include "cdrdocument.h"
// Calligra
#include <KoXmlWriter.h>
#include <KoXmlNS.h>
#include <KoOdf.h>
#include <KoStore.h>
#include <KoStoreDevice.h>
#include <KoDocumentInfo.h>
// Qt
#include <QtCore/QString>


CdrOdgWriter::CdrOdgWriter( KoStore* outputStore )
  : mOdfWriteStore( outputStore )
  , mOutputStore( outputStore )
  , mPageCount( 0 )
{
    mManifestWriter = mOdfWriteStore.manifestWriter( KoOdf::mimeType(KoOdf::Graphics) );
}

CdrOdgWriter::~CdrOdgWriter()
{
    mOdfWriteStore.closeManifestWriter();
    delete mOutputStore;
}

bool
CdrOdgWriter::write( CdrDocument* document )
{
    mDocument = document;

    writeThumbnailFile();

    writePixelImageFiles();

    // Create content.xml
    writeContentFile();

    // Create the styles.xml file
    mStyleCollector.saveOdfStylesDotXml( mOutputStore, mManifestWriter );

    // Create settings.xml
    writeDocumentSettingsFile();

    // Create meta.xml
    writeDocumentInfoFile();

    return true;
}

void
CdrOdgWriter::writeThumbnailFile()
{
    // TODO: could be static strings
    const QString thumbnailDirPath = QLatin1String( "Thumbnails/" );
    const QString thumbnailFilePath = QLatin1String( "Thumbnails/thumbnail.png" );
//     mOutputStore->open( thumbnailFilePath );
//     mOutputStore->write(*preview);
//     mOutputStore->close();
    // TODO: path really needed as entry?
//     mManifestWriter->addManifestEntry( thumbnailDirPath, QString() );
    // TODO: really no mimetype?
//     mManifestWriter->addManifestEntry( thumbnailFilePath, QString() );
}

void
CdrOdgWriter::writePixelImageFiles()
{
#if 0
    // TODO: as mManifestWriter needs full rel path, perhaps drop enterDirectory/leaveDirectory
    mOutputStore->enterDirectory( QLatin1String("Pictures") );

    foreach( const CdrPixelImage* image, mDocument->pixelImages() )
    {
        mOutputStore->open( fileName );
        mOutputStore->write( *image );
        mOutputStore->close();

        mManifestWriter->addManifestEntry( filePath, mediaType );
    }

    mOutputStore->leaveDirectory();
#endif
}

void
CdrOdgWriter::writeDocumentInfoFile()
{
    KoDocumentInfo documentInfo;
    // TODO
//     documentInfo.load( m_documentInfo );

    const QString documentInfoFilePath = QLatin1String( "meta.xml" );

    mOutputStore->open( documentInfoFilePath );
    documentInfo.saveOasis( mOutputStore );
    mOutputStore->close();
    // TODO: "text/xml" could be a static string
    mManifestWriter->addManifestEntry( documentInfoFilePath, QLatin1String("text/xml") );
}

void
CdrOdgWriter::writeDocumentSettingsFile()
{
    const QString documentSettingsFilePath = QLatin1String( "settings.xml" );

    mOutputStore->open( documentSettingsFilePath );

    KoStoreDevice device( mOutputStore );
    KoXmlWriter* settingsWriter =
        KoOdfWriteStore::createOasisXmlWriter( &device, "office:document-settings" );

        settingsWriter->startElement( "config:config-item-set" );
            settingsWriter->addAttribute( "config:name", "ooo:configuration-settings" );
            settingsWriter->startElement( "config:config-item" );
                settingsWriter->addAttribute( "config:name", "TabsRelativeToIndent" );
                settingsWriter->addAttribute( "config:type", "boolean" );
                settingsWriter->addTextSpan( QLatin1String("false") ); // ODF=true, MSOffice=false
            settingsWriter->endElement(); // config-item
        settingsWriter->endElement(); // config-item-set

    settingsWriter->endElement();//office:document-settings
    settingsWriter->endDocument();

    mOutputStore->close();

    // TODO: "text/xml" could be a static string
    mManifestWriter->addManifestEntry( documentSettingsFilePath, QLatin1String("text/xml") );
}

void
CdrOdgWriter::writeContentFile()
{
    KoXmlWriter* contentWriter = mOdfWriteStore.contentWriter();
    mBodyWriter = mOdfWriteStore.bodyWriter();

    mBodyWriter->startElement( "office:body" );
    mBodyWriter->startElement( KoOdf::bodyContentElement(KoOdf::Graphics, true));

    writeMasterPage();

    foreach( const CdrPage* page, mDocument->pages() )
        writePage( page );

    mBodyWriter->endElement(); //office:drawing
    mBodyWriter->endElement(); //office:body
    mBodyWriter->endDocument();

    mStyleCollector.saveOdfStyles( KoGenStyles::DocumentAutomaticStyles, contentWriter );

    mOdfWriteStore.closeContentWriter();

    // TODO: mOdfWriteStore.closeContentWriter() should do that, or? also "text/xml" could be a static string
    mManifestWriter->addManifestEntry( QLatin1String("content.xml"), QLatin1String("text/xml") );
}


void
CdrOdgWriter::writeMasterPage()
{
    KoGenStyle masterPageStyle( KoGenStyle::MasterPageStyle );

    KoGenStyle masterPageLayoutStyle( KoGenStyle::PageLayoutStyle );
    masterPageLayoutStyle.setAutoStyleInStylesDotXml( true );

//     masterPageLayoutStyle.addPropertyPt( QLatin1String("fo:page-width"), mDocument->width() );
//     masterPageLayoutStyle.addPropertyPt( QLatin1String("fo:page-height"), mDocument->height() );
//     masterPageLayoutStyle.addProperty( QLatin1String("style:print-orientation"), "landscape" );

    const QString masterPageLayoutStyleName =
        mStyleCollector.insert( masterPageLayoutStyle, QLatin1String("masterPageLayoutStyle") );

    masterPageStyle.addAttribute( QLatin1String("style:page-layout-name"), masterPageLayoutStyleName );

    KoGenStyle drawingMasterPageStyle( KoGenStyle::DrawingPageStyle, "drawing-page" );
    drawingMasterPageStyle.setAutoStyleInStylesDotXml( true );

    drawingMasterPageStyle.addProperty( QLatin1String("draw:fill"), "none" );

    const QString drawingMasterPageStyleName =
        mStyleCollector.insert( drawingMasterPageStyle, "drawingMasterPageStyle");

    masterPageStyle.addAttribute( QLatin1String("draw:style-name"), drawingMasterPageStyleName );

    mMasterPageStyleName =
        mStyleCollector.insert( masterPageStyle, QLatin1String("masterPageStyle") );
}

void
CdrOdgWriter::writePage( const CdrPage* page )
{
    mBodyWriter->startElement("draw:page");

    mBodyWriter->addAttribute( "draw:id", QLatin1String("page")+QString::number(mPageCount++) );
    mBodyWriter->addAttribute( "draw:master-page-name", mMasterPageStyleName );

    foreach( const CdrLayer* layer, page->layers() )
        writeLayer( layer );

    mBodyWriter->endElement(); //draw:page
}

void
CdrOdgWriter::writeLayer( const CdrLayer* layer )
{
//     mXmlWriter.startElement("g");

    foreach( const CdrAbstractObject* object, layer->objects() )
        writeObject( object );

//     mXmlWriter.endElement(); // g
}

void
CdrOdgWriter::writeObject( const CdrAbstractObject* object )
{
//     mXmlWriter.startElement("g");

#if 0
    QString tfString;
    foreach(const CdrAbstractTransformation* transformation, object->transformations())
    {
        const CdrNormalTransformation* normalTrafo =
            dynamic_cast<const CdrNormalTransformation*>(transformation);

        if( normalTrafo )
            tfString.append( QString::fromLatin1("translate(%1,%2) ").arg(normalTrafo->x()).arg(normalTrafo->y()) );
    }
    mXmlWriter.addAttribute( "transform", tfString );
#endif
    const CdrObjectTypeId typeId = object->typeId();

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

//     mXmlWriter.endElement(); // g
}

void
CdrOdgWriter::writeGroupObject( const CdrGroupObject* groupObject )
{
    mBodyWriter->startElement("draw:g");

//     set2DGeometry(mBodyWriter, objectElement);
//     mBodyWriter->addAttribute("draw:style-name", createGraphicStyle(objectElement));

    foreach( const CdrAbstractObject* object, groupObject->objects() )
        writeObject( object );

    mBodyWriter->endElement(); //draw:g
}

void
CdrOdgWriter::writeRectangleObject( const CdrRectangleObject* object )
{
#if 0
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
#endif
}

void
CdrOdgWriter::writeEllipseObject( const CdrEllipseObject* object )
{
#if 0
    mXmlWriter.startElement("ellipse");

    mXmlWriter.addAttribute("cx", object->centerPoint().x());
    mXmlWriter.addAttribute("cy", -object->centerPoint().y());
    mXmlWriter.addAttribute("rx", object->xRadius());
    mXmlWriter.addAttribute("ry", object->yRadius());
    writeStrokeWidth( object->outlineId() );
    writeStrokeColor( object->outlineId() );
    writeFillColor( object->fillId() );

    mXmlWriter.endElement(); // ellipse
#endif
}

void
CdrOdgWriter::writePathObject( const CdrPathObject* pathObject )
{
#if 0
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
#endif
}

void
CdrOdgWriter::writeTextObject( const CdrTextObject* object )
{
#if 0
    mXmlWriter.startElement("text");

//     writeStrokeWidth( object->outlineId() );
    writeStrokeColor( object->outlineId() );
    writeFillColor( object->fillId() );
    writeFont( object->styleId() );
    mXmlWriter.addTextNode( object->text() );

    mXmlWriter.endElement(); // text
#endif
}

void
CdrOdgWriter::writeFillColor( quint32 fillId )
{
#if 0
    CdrAbstractFill* fill = mDocument->fill( fillId );
    const QString colorName = ( fill && fill->id() == CdrAbstractFill::Solid ) ?
        static_cast<CdrSolidFill*>( fill )->color().name() :
        QString::fromLatin1("none");
    mXmlWriter.addAttribute("fill", colorName);
#endif
}

void
CdrOdgWriter::writeStrokeColor( quint32 outlineId )
{
#if 0
    CdrOutline* outline = mDocument->outline( outlineId );
    const QString colorName = ( outline ) ?
        outline->color().name() :
        QString::fromLatin1("none");
    mXmlWriter.addAttribute("stroke", colorName);
#endif
}

void
CdrOdgWriter::writeStrokeWidth( quint32 outlineId )
{
#if 0
    CdrOutline* outline = mDocument->outline( outlineId );
    const quint16 lineWidth = ( outline ) ? outline->lineWidth() : 0;
    mXmlWriter.addAttribute("stroke-width", QString::number(lineWidth) );
#endif
}

void
CdrOdgWriter::writeFont( quint16 styleId )
{
#if 0
    CdrStyle* style = mDocument->style( styleId );
    const quint16 fontSize = ( style ) ? style->fontSize() : 18; // TODO: default font size?
    mXmlWriter.addAttribute("font-size", QString::number(fontSize) );
    if( style )
    {
        CdrFont* font = mDocument->font( style->fontId() );
        if( font )
            mXmlWriter.addAttribute("font-family", font->name() );
    }
#endif
}
