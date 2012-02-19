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
#include <QtCore/QLocale>
#include <QtCore/QString>


// Metric conversion
// DIN A4 page:
//     cdr     mm     inches      odf (points)
// w    8268   210.0   8267,7165  595.28
// h   11694   297.0  11692,913   841.89
// Hints that CDRv4 uses 1/1000 inches as base unit
// (ignoring off-by-+1 rounding in height for now)
// 1 point = 1/72 inches
// thus:
// y point = x cdr <=>
// y * 1/72 inches = x * 1/1000 inches <=>
// y = 72/1000 * x
static const double cdr2PtFactor = 0.072;

static inline
double
ptUnit( CdrCoord x ){ return static_cast<qreal>(x) * cdr2PtFactor; }


/// Returns the CDR length as ODF length (in pt)
static inline
double
odfLength( CdrCoord x )
{ return ptUnit(x); }


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
    else if( object->typeId() == GraphicTextObjectId )
        nonOdfObjects.append( object );
}

void
CdrOdgWriter::writeGraphicTextSvg( QIODevice* device, const CdrGraphicTextObject* textObject )
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

        writeGraphicTextSvg( &svgFile, static_cast<const CdrGraphicTextObject*>(object) );
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

    masterPageLayoutStyle.addPropertyPt( QLatin1String("fo:page-width"), odfLength(mDocument->width()) );
    masterPageLayoutStyle.addPropertyPt( QLatin1String("fo:page-height"), odfLength(mDocument->height()) );
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
        foreach( const CdrAbstractObject* object, layer->objects() )
            writeObject( object );
    }

    mBodyWriter->endElement(); //draw:page
}

void
CdrOdgWriter::writeObject( const CdrAbstractObject* object )
{
    const CdrObjectTypeId typeId = object->typeId();

    if( typeId == PathObjectId )
        writePathObject( static_cast<const CdrPathObject*>(object) );
    else if( typeId == RectangleObjectId )
        writeRectangleObject( static_cast<const CdrRectangleObject*>(object) );
    else if( typeId == EllipseObjectId )
        writeEllipseObject( static_cast<const CdrEllipseObject*>(object) );
    else if( typeId == GraphicTextObjectId )
        writeGraphicTextObject( static_cast<const CdrGraphicTextObject*>(object) );
    else if( typeId == GroupObjectId )
        writeGroupObject( static_cast<const CdrGroupObject*>(object) );
}

void
CdrOdgWriter::writeGroupObject( const CdrGroupObject* groupObject )
{
    mBodyWriter->startElement("draw:g");

    writeTransformation( groupObject->transformations() );
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

    writeTransformation( object->transformations() );
    mBodyWriter->addAttributePt("svg:x", odfXCoord(0));
    mBodyWriter->addAttributePt("svg:y", odfYCoord(0));
    mBodyWriter->addAttributePt("svg:width", odfLength(object->cornerPoint().x()) );
    mBodyWriter->addAttributePt("svg:height", odfLength(object->cornerPoint().y()) );
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

    writeTransformation( object->transformations() );
    mBodyWriter->addAttributePt( "svg:x", odfXCoord(0) );
    mBodyWriter->addAttributePt( "svg:y", odfYCoord(0) );
    mBodyWriter->addAttributePt( "svg:width", odfLength(object->cornerPoint().x()) );
    mBodyWriter->addAttributePt( "svg:height", odfLength(object->cornerPoint().y()) );
    if( object->startAngle() != object->endAngle() )
    {
        mBodyWriter->addAttribute( "draw:start-angle", static_cast<qreal>(object->startAngle())/10.0 );
        mBodyWriter->addAttribute( "draw:end-angle",   static_cast<qreal>(object->endAngle())/10.0 );
    }
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

    writeTransformation( pathObject->transformations() );
    const QVector<CdrPathPoint>& pathPoints = pathObject->pathPoints();

    CdrPoint curveControlPoints[2];
    int curveControlPointCount = 0;

    QString pathData;
    if( pathPoints.count() > 0 )
    {
        const CdrPoint point = pathPoints.at(0).mPoint;
        pathData = QLatin1Char('M') + QString::number(odfXCoord(point.x())) + QLatin1Char(' ') +
                   QString::number(odfYCoord(point.y()));
    }
    for( int j=1; j<pathPoints.count(); j++ )
    {
        const CdrPathPoint& pathPoint = pathPoints.at(j);

        const quint8 strokeToPointType = (pathPoint.mType&0xC0);

        // subpath start?
        if( strokeToPointType == 0x00 ) // no line to point?
        {
            if(( pathPoint.mType&0x04) == 0x04 )  // real point?
            {
                pathData = pathData + QLatin1String(" M") +
                        QString::number(odfXCoord(pathPoint.mPoint.x())) + QLatin1Char(' ') +
                        QString::number(odfYCoord(pathPoint.mPoint.y()));
            }
        }
        else if( strokeToPointType == 0xC0 ) // control point?
        {
            curveControlPoints[curveControlPointCount] = pathPoint.mPoint;
            ++curveControlPointCount;
        }
        else
        {
            if( strokeToPointType == 0x80 ) // curve to point?
            {
                pathData = pathData + QLatin1String(" C") +
                        QString::number(odfXCoord(curveControlPoints[0].x())) + QLatin1Char(' ') +
                        QString::number(odfYCoord(curveControlPoints[0].y())) + QLatin1Char(' ') +
                        QString::number(odfXCoord(curveControlPoints[1].x())) + QLatin1Char(' ') +
                        QString::number(odfYCoord(curveControlPoints[1].y())) + QLatin1Char(' ') +
                        QString::number(odfXCoord(pathPoint.mPoint.x())) + QLatin1Char(' ') +
                        QString::number(odfYCoord(pathPoint.mPoint.y()));
                curveControlPointCount = 0;
            }
            else //if( strokeToPointType == 0x40 )
                pathData = pathData + QLatin1String(" L") +
                        QString::number(odfXCoord(pathPoint.mPoint.x())) + QLatin1Char(' ') +
                        QString::number(odfYCoord(pathPoint.mPoint.y()));
            const bool isSubpathClosing = ((pathPoint.mType&0x0C) == 0x08);
            if( isSubpathClosing )
                pathData.append( QLatin1Char('z') );
        }
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
CdrOdgWriter::writeGraphicTextObject( const CdrGraphicTextObject* object )
{
    mBodyWriter->startElement( "draw:frame" );
    mBodyWriter->addAttributePt( "svg:x", odfXCoord(0));
    mBodyWriter->addAttributePt( "svg:y", odfYCoord(0));
    writeTransformation( object->transformations() );
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
    writeTransformation( object->transformations() );
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

    const double lineWidth = ( outline ) ? odfLength(outline->lineWidth()) : 0.0;
    odfStyle.addPropertyPt( QLatin1String("svg:stroke-width"), lineWidth );
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


static inline
double
odfXTransformCoord( qint32 x )
{ return ptUnit(x); }

static inline
double
odfYTransformCoord( qint32 y )
{ return -ptUnit(y); }

void
CdrOdgWriter::writeTransformation( const QVector<CdrAbstractTransformation*>& transformations )
{
    QLocale cLocale(QLocale::c());
    cLocale.setNumberOptions(QLocale::OmitGroupSeparator);

    QString tfString;
    foreach( const CdrAbstractTransformation* transformation, transformations )
    {
        const CdrNormalTransformation* normalTrafo =
            dynamic_cast<const CdrNormalTransformation*>(transformation);

        if( normalTrafo )
            tfString = tfString + QLatin1String("translate(") +
                       cLocale.toString(odfXTransformCoord(normalTrafo->x())) + QLatin1Char(' ') +
                       cLocale.toString(odfYTransformCoord(normalTrafo->y())) + QLatin1Char(')');
    }
    if( ! tfString.isEmpty() )
        mBodyWriter->addAttribute( "draw:transform", tfString );
}

double
CdrOdgWriter::odfXCoord( CdrCoord x ) const
{ return odfLength(mDocument->width())*0.5 + ptUnit(x); }

double
CdrOdgWriter::odfYCoord( CdrCoord y ) const
{ return odfLength(mDocument->height())*0.5 - ptUnit(y); }
