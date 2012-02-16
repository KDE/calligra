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

    storeThumbnailFile();

    storePixelImageFiles();
    storeSvgImageFiles();

    // Create content.xml
    storeContentXml();

    // Create the styles.xml file
    mStyleCollector.saveOdfStylesDotXml( mOutputStore, mManifestWriter );

    // Create settings.xml
    storeSettingsXml();

    // Create meta.xml
    storeMetaXml();

    return true;
}

void
CdrOdgWriter::storeThumbnailFile()
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
CdrOdgWriter::storePixelImageFiles()
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

static
void
collectNonOdfObjects( QVector<const CdrAbstractObject*>& nonOdfObjects, const CdrAbstractObject* object )
{
    if( object->typeId() == GroupObjectId )
    {
        foreach( const CdrAbstractObject* object, static_cast<const CdrGroupObject*>(object)->objects() )
            collectNonOdfObjects( nonOdfObjects, object );
    }
    else if( object->typeId() == TextObjectId )
        nonOdfObjects.append( object );
}

void
CdrOdgWriter::writeGraphicTextSvg( QIODevice* device, const CdrTextObject* textObject )
{
    QTextStream svgStream( device );

    // standard header:
    svgStream << QLatin1String("<?xml version=\"1.0\" standalone=\"no\"?>") << endl;
    svgStream << QLatin1String("<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 20010904//EN\" "
                               "\"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">") << endl;

    svgStream << QLatin1String("<svg xmlns=\"http://www.w3.org/2000/svg\" "
                                "xmlns:xlink=\"http://www.w3.org/1999/xlink\""
                                " width=\"") << /*textObject->width()*/10 << QLatin1String("pt\""
                                " height=\"") << /*textObject->height()*/10 << QLatin1String("pt\">") << endl;

    svgStream << QLatin1String("<text");

    CdrAbstractFill* fill = mDocument->fill( textObject->fillId() );
    const QString fillColorName = ( fill && fill->id() == CdrAbstractFill::Solid ) ?
        static_cast<CdrSolidFill*>( fill )->color().name() :
        QString::fromLatin1("none");
    svgStream << QLatin1String(" fill=\"") << fillColorName<<QLatin1Char('\"');

    CdrOutline* outline = mDocument->outline( textObject->outlineId() );
    const QString outlineColorName = ( outline ) ?
        outline->color().name() :
        QString::fromLatin1("none");
    svgStream << QLatin1String(" stroke=\"") << outlineColorName << QLatin1Char('\"');

//     const quint16 lineWidth = ( outline ) ? outline->lineWidth() : 0;
//     svgStream << QLatin1String(" stroke-width=\"") << QString::number(lineWidth)<<QLatin1Char('\"');

    CdrStyle* style = mDocument->style( textObject->styleId() );
    const quint16 fontSize = ( style ) ? style->fontSize() : 18; // TODO: default font size?
    svgStream << QLatin1String(" font-size=\"") << QString::number(fontSize) << QLatin1Char('\"');
    if( style )
    {
        CdrFont* font = mDocument->font( style->fontId() );
        if( font )
            svgStream << QLatin1String(" font-family=\"") << font->name() << QLatin1Char('\"');
    }
    svgStream << QLatin1String("><tspan>")<<textObject->text() << QLatin1String("</tspan></text>");

    // end tag:
    svgStream << endl << QLatin1String("</svg>") << endl;
}

void
CdrOdgWriter::storeSvgImageFiles()
{
    // get
    QVector<const CdrAbstractObject*> nonOdfObjects;
    foreach( const CdrPage* page, mDocument->pages() )
        foreach( const CdrLayer* layer, page->layers() )
            foreach( const CdrAbstractObject* object, layer->objects() )
                collectNonOdfObjects( nonOdfObjects, object );

    const QString svgImagesName = QLatin1String( "SvgImages" );
    const QString mediaType = QLatin1String("text/svg+xml");

    mOutputStore->enterDirectory( svgImagesName );

    int imageIndex = 1;
    foreach( const CdrAbstractObject* object, nonOdfObjects )
    {
        const QString fileName = QLatin1String("Image") + QString::number(imageIndex++);
        mOutputStore->open( fileName );
        KoStoreDevice svgFile( mOutputStore );

        writeGraphicTextSvg( &svgFile, static_cast<const CdrTextObject*>(object) );
        mOutputStore->close();

        const QString filePath = svgImagesName + QLatin1Char('/') + fileName;
        mManifestWriter->addManifestEntry( filePath, mediaType );
        mSvgFilePathByObject.insert( object, filePath );
    }

    mOutputStore->leaveDirectory(); // "SvgImages"
}

void
CdrOdgWriter::storeMetaXml()
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
CdrOdgWriter::storeSettingsXml()
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
CdrOdgWriter::storeContentXml()
{
    KoXmlWriter* contentWriter = mOdfWriteStore.contentWriter();
    mBodyWriter = mOdfWriteStore.bodyWriter();

    mBodyWriter->startElement( "office:body" );
    mBodyWriter->startElement( KoOdf::bodyContentElement(KoOdf::Graphics, true));

    writeMasterPage();

    writePage(mDocument->pages().at(1));
//     foreach( const CdrPage* page, mDocument->pages() )
//         writePage( page );

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
        mStyleCollector.insert( drawingMasterPageStyle, QLatin1String("drawingMasterPageStyle") );

    masterPageStyle.addAttribute( QLatin1String("draw:style-name"), drawingMasterPageStyleName );

    mMasterPageStyleName =
        mStyleCollector.insert( masterPageStyle, QLatin1String("masterPageStyle") );
}

void
CdrOdgWriter::writePage( const CdrPage* page )
{
    mBodyWriter->startElement( "draw:page" );

    mBodyWriter->addAttribute( "draw:id", QLatin1String("page")+QString::number(mPageCount++) );
    mBodyWriter->addAttribute( "draw:master-page-name", mMasterPageStyleName );

    // layer set
    int layerCount = 0;
    mBodyWriter->startElement( "draw:layer-set" );
    foreach( const CdrLayer* layer, page->layers() )
    {
        const QString layerId = QLatin1String("Layer ")+QString::number(layerCount++);
        mBodyWriter->startElement( "draw:layer" );
        mBodyWriter->addAttribute( "draw:name", layerId );
        mBodyWriter->endElement(); //draw:layer
    }
    mBodyWriter->endElement(); //draw:layer-set

    // layer objects
    layerCount = 0;
    foreach( const CdrLayer* layer, page->layers() )
    {
        mLayerId = QLatin1String("Layer ")+QString::number(layerCount++);
        writeLayer( layer );
    }

    mBodyWriter->endElement(); //draw:page
}

void
CdrOdgWriter::writeLayer( const CdrLayer* layer )
{
//     mBodyWriter->startElement("g");

    foreach( const CdrAbstractObject* object, layer->objects() )
        writeObject( object );

//     mBodyWriter->endElement(); // g
}

void
CdrOdgWriter::writeObject( const CdrAbstractObject* object )
{
//     mBodyWriter->startElement("g");

#if 0
    QString tfString;
    foreach(const CdrAbstractTransformation* transformation, object->transformations())
    {
        const CdrNormalTransformation* normalTrafo =
            dynamic_cast<const CdrNormalTransformation*>(transformation);

        if( normalTrafo )
            tfString.append( QString::fromLatin1("translate(%1,%2) ").arg(normalTrafo->x()).arg(normalTrafo->y()) );
    }
    mBodyWriter->addAttribute( "transform", tfString );
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

//     mBodyWriter->endElement(); // g
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
    mBodyWriter->startElement("draw:rect");

//     mBodyWriter->addAttribute("x", x);
//     mBodyWriter->addAttribute("y", y);
    mBodyWriter->addAttribute("svg:width", object->cornerPoint().x() );
    mBodyWriter->addAttribute("svg:height", object->cornerPoint().y() );
//     mBodyWriter->addAttribute("rx", object->cornerRoundness());
//     mBodyWriter->addAttribute("ry", object->cornerRoundness());
    mBodyWriter->addAttribute("draw:layer", mLayerId );

    KoGenStyle style( KoGenStyle::GraphicAutoStyle, "graphic" );
    writeStrokeWidth( style, object->outlineId() );
    writeStrokeColor( style, object->outlineId() );
    writeFill( style, object->fillId() );
    const QString styleName = mStyleCollector.insert( style, QLatin1String("rectangleStyle") );
    mBodyWriter->addAttribute( "draw:style-name", styleName );

    mBodyWriter->endElement(); // draw:rect
}

void
CdrOdgWriter::writeEllipseObject( const CdrEllipseObject* object )
{
    mBodyWriter->startElement( "draw:ellipse" );

    mBodyWriter->addAttribute( "svg:cx", object->centerPoint().x() );
    mBodyWriter->addAttribute( "svg:cy", -object->centerPoint().y() );
    mBodyWriter->addAttribute( "svg:rx", object->xRadius() );
    mBodyWriter->addAttribute( "svg:ry", object->yRadius() );
    mBodyWriter->addAttribute( "draw:layer", mLayerId );

    KoGenStyle style( KoGenStyle::GraphicAutoStyle, "graphic" );
    writeStrokeWidth( style, object->outlineId() );
    writeStrokeColor( style, object->outlineId() );
    writeFill( style, object->fillId() );
    const QString styleName = mStyleCollector.insert( style, QLatin1String("ellipseStyle") );
    mBodyWriter->addAttribute( "draw:style-name", styleName );

    mBodyWriter->endElement(); // draw:ellipse
}

void
CdrOdgWriter::writePathObject( const CdrPathObject* pathObject )
{
    mBodyWriter->startElement( "draw:path" );

    const QVector<CdrPathPoint>& pathPoints = pathObject->pathPoints();

    QString pathData;
    if( pathPoints.count() > 0 )
        pathData = QLatin1Char('M') + QString::number(pathPoints[0].mPoint.x()) + QLatin1Char(' ') +
                   QString::number(pathPoints[0].mPoint.y());
    for( int j=1; j<pathPoints.count(); j++ )
    {
        const CdrPathPoint& pathPoint = pathPoints.at(j);

        const bool isLineStarting = (pathPoint.mType == 0x0C);

        pathData = pathData + QLatin1String(isLineStarting?" M":" L") +
                   QString::number(pathPoint.mPoint.x()) + QLatin1Char(' ') +
                   QString::number(pathPoint.mPoint.y());
        const bool isLineEnding = (pathPoint.mType == 0x48);
        if( isLineEnding )
            pathData.append( QLatin1Char('z') );
    }

    mBodyWriter->addAttribute( "svg:d", pathData ) ;

    KoGenStyle style( KoGenStyle::GraphicAutoStyle, "graphic" );
    writeStrokeWidth( style, pathObject->outlineId() );
    writeStrokeColor( style, pathObject->outlineId() );
    writeFill( style, pathObject->fillId() );
    const QString styleName = mStyleCollector.insert( style, QLatin1String("polylineStyle") );
    mBodyWriter->addAttribute( "draw:style-name", styleName );

    mBodyWriter->endElement(); // draw:path
}

void
CdrOdgWriter::writeTextObject( const CdrTextObject* object )
{
    mBodyWriter->startElement( "draw:frame" );
        mBodyWriter->startElement( "draw:image" );
        mBodyWriter->addAttribute( "xlink:type", QLatin1String("simple") );
        mBodyWriter->addAttribute( "xlink:show", QLatin1String("embed") );
        mBodyWriter->addAttribute( "xlink:actuate", QLatin1String("onLoad") );
        mBodyWriter->addAttribute( "xlink:href", mSvgFilePathByObject[object] );

        mBodyWriter->endElement(); // draw:frame
    mBodyWriter->endElement(); // draw:frame
}

void
CdrOdgWriter::writeBlockTextObject( const CdrBlockTextObject* object )
{
    mBodyWriter->startElement("draw:frame");
//     mBodyWriter->addAttribute( "draw:style-name", blockTextStyle );

    mBodyWriter->startElement("draw:text-box");

    //export every paragraph
//     foreach( const CdrParagraph* paragraph, paragraphs )
//         writeParagraph( paragraph );
//     }

    mBodyWriter->endElement();//draw:text-box
    mBodyWriter->endElement();//draw:frame
}

void
CdrOdgWriter::writeParagraph( const CdrParagraph* paragraph )
{
    mBodyWriter->startElement( "text:p", false );  //false: we should not indent the inner tags

    KoGenStyle paragraphStyle( KoGenStyle::ParagraphAutoStyle, "paragraph" );
    const QString paragraphStyleName =
        mStyleCollector.insert( paragraphStyle, QLatin1String("paragraphStyle") );

    mBodyWriter->addAttribute( "text:style-name", paragraphStyleName );

    KoGenStyle textStyle( KoGenStyle::TextAutoStyle, "text" );
    const QString textStyleName =
        mStyleCollector.insert( textStyle, "T" );

        mBodyWriter->startElement( "text:span" );

        mBodyWriter->addAttribute( "text:style-name", textStyleName );
//         mBodyWriter->addTextNode( text );

        mBodyWriter->endElement();//text:span

    mBodyWriter->endElement();//text:p
}

void
CdrOdgWriter::writeFill( KoGenStyle& odfStyle, quint32 fillId )
{
    CdrAbstractFill* fill = mDocument->fill( fillId );

    const bool isSolid = ( fill && fill->id() == CdrAbstractFill::Solid );

    odfStyle.addProperty( QLatin1String("draw:fill"), isSolid ? "solid" : "none" );

    if( isSolid )
        odfStyle.addProperty( QLatin1String("draw:fill-color"), static_cast<CdrSolidFill*>( fill )->color().name() );
}

void
CdrOdgWriter::writeStrokeColor( KoGenStyle& odfStyle, quint32 outlineId )
{
    CdrOutline* outline = mDocument->outline( outlineId );

    const QString colorName = ( outline ) ?
        outline->color().name() :
        QString::fromLatin1("none");
    odfStyle.addProperty( QLatin1String("svg:stroke-color"), colorName );
}

void
CdrOdgWriter::writeStrokeWidth( KoGenStyle& odfStyle, quint32 outlineId )
{
    CdrOutline* outline = mDocument->outline( outlineId );

    const quint16 lineWidth = ( outline ) ? outline->lineWidth() : 0;
    odfStyle.addProperty( QLatin1String("svg:stroke-width"), lineWidth );
    odfStyle.addProperty( QLatin1String("draw:stroke"), "solid" );
}

void
CdrOdgWriter::writeFont( KoGenStyle& odfStyle, quint16 styleId )
{
    CdrStyle* style = mDocument->style( styleId );

    const quint16 fontSize = ( style ) ? style->fontSize() : 18; // TODO: default font size?
    odfStyle.addPropertyPt( QLatin1String("fo:font-size"), fontSize );
    if( style )
    {
        CdrFont* font = mDocument->font( style->fontId() );
        if( font )
            odfStyle.addProperty( QLatin1String("style:font-name"), font->name() );
    }
}
