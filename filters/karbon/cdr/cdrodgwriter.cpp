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
#include <QtGui/QTextDocument>
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

// The coord system of CDR has 0,0 in the middle of the page and
// the positive x coordinate increasing upwards,
// while ODF has 0,0 in the upperleft corner of the page and
// the positive x coordinate increasing downwards.
// The transformation from CDR coordinates to ODF coordinates is done in two steps:
// the scaling of the values is done directly on the x/y/width/length values
// of the objects, the translation of the offset only as a "transform" operation as
// attribute. The reason for this is that the CDR transformations use absolute values,
// so doing the offset adaption already in the written values might complicate things
// when adapting the values of the CDR transformations.

static inline
double
ptUnit( qint32 x )
{
    return static_cast<qreal>(x) * cdr2PtFactor;
}

/// Returns the CDR length as ODF length (in pt)
static inline
double
odfLength( qint32 length )
{
    return ptUnit( length );
}

/// Returns the CDR x coord as ODF x coord (in pt), no offset adaption
static inline
double
odfXCoord( qint32 x )
{
    return ptUnit( x );
}

/// Returns the CDR y coord as ODF y coord (in pt), no offset adaption
static inline
double
odfYCoord( qint32 y )
{
    return ptUnit( y );
}



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
                                "xmlns:xlink=\"http://www.w3.org/1999/xlink\">"
                                /*" width=\"") << textObject->width() << QLatin1String("pt\""
                                " height=\"") << textObject->height() << QLatin1String("pt\">"*/) << endl;

    svgStream << QLatin1String("<text transform=\"scale(1 -1)\"");

    CdrAbstractFill* fill = mDocument->fill( textObject->fillId() );
    const QString fillColorName = ( fill && fill->id() == CdrAbstractFill::Solid ) ?
        static_cast<CdrSolidFill*>( fill )->color().name() :
        QString::fromLatin1("none");
    svgStream << QLatin1String(" fill=\"") << fillColorName<<QLatin1Char('\"');

    const CdrOutline* const outline = mDocument->outline( textObject->outlineId() );
    if( outline )
        svgStream << QLatin1String(" stroke=\"") << outline->color().name() << QLatin1Char('\"');

//     const quint16 lineWidth = ( outline ) ? outline->lineWidth() : 0;
//     svgStream << QLatin1String(" stroke-width=\"") << QString::number(lineWidth)<<QLatin1Char('\"');

    CdrStyle* style = mDocument->style( textObject->styleId() );
    const quint16 fontSize = ( style ) ? style->fontSize() : 18; // TODO: default font size?
    svgStream << QLatin1String(" font-size=\"") << QString::number(odfLength(fontSize)) << QLatin1String("pt\"");
    if( style )
    {
        CdrFont* font = mDocument->font( style->fontId() );
        if( font )
            svgStream << QLatin1String(" font-family=\"") << font->name() << QLatin1Char('\"');
        const CdrTextAlignment textAlignment = style->textAlignment();
        if( textAlignment != CdrTextAlignmentUnknown )
        {
            static struct {const char* name;} alignmentName[3] =
            { {"start"}, {"middle"}, {"end"} };
            const QString anchor = QLatin1String( alignmentName[textAlignment-1].name );
            svgStream << QLatin1String(" text-anchor=\"") << anchor << QLatin1Char('\"');
        }
    }
    svgStream << QLatin1Char('>') << endl;

    const QStringList textLines = textObject->text().split( QLatin1String("\x0D\x0A") );
    double y = 0.0;
    for( int i = 0; i < textLines.count(); ++i )
    {
        svgStream << QLatin1String("<tspan x=\"0pt\" y=\"") << QString::number(y) << ("pt\">")
                  << Qt::escape(textLines.at(i)) << QLatin1String("</tspan>") << endl;

        y += odfLength( fontSize );
    }

    svgStream << endl << QLatin1String("</text></svg>") << endl;
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

    mBodyWriter->addAttribute( "xml:id", QLatin1String("page")+QString::number(mPageCount++) );
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
    else if( typeId == BlockTextObjectId )
        writeBlockTextObject( static_cast<const CdrBlockTextObject*>(object) );
    else if( typeId == GroupObjectId )
        writeGroupObject( static_cast<const CdrGroupObject*>(object) );
}

void
CdrOdgWriter::writeGroupObject( const CdrGroupObject* groupObject )
{
    mBodyWriter->startElement("draw:g");

    // TODO: why are there transformations on groups if they seem not needed to be applied?
    // (and draw:g does not allow draw:transform anyway)
//     writeTransformation( groupObject->transformations(), QString(), NoGlobalTransformation );
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
    writeCornerPoint(object->cornerPoint());
//     mBodyWriter->addAttribute("rx", object->cornerRoundness());
//     mBodyWriter->addAttribute("ry", object->cornerRoundness());
    mBodyWriter->addAttribute("draw:layer", mLayerId );

    KoGenStyle style( KoGenStyle::GraphicAutoStyle, "graphic" );
    writeStroke( style, object->outlineId() );
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
    writeCornerPoint( object->cornerPoint() );
    if( object->startAngle() != object->endAngle() )
    {
        mBodyWriter->addAttribute( "draw:start-angle", static_cast<qreal>(object->startAngle())/10.0 );
        mBodyWriter->addAttribute( "draw:end-angle",   static_cast<qreal>(object->endAngle())/10.0 );
    }
    mBodyWriter->addAttribute( "draw:layer", mLayerId );

    KoGenStyle style( KoGenStyle::GraphicAutoStyle, "graphic" );
    writeStroke( style, object->outlineId() );
    writeFill( style, object->fillId() );
    const QString styleName = mStyleCollector.insert( style, QLatin1String("ellipseStyle") );
    mBodyWriter->addAttribute( "draw:style-name", styleName );

    mBodyWriter->endElement(); // draw:ellipse
}

void
CdrOdgWriter::writePathObject( const CdrPathObject* pathObject )
{
    mBodyWriter->startElement( "draw:path" );

    mBodyWriter->addAttribute( "draw:layer", mLayerId );

    writeTransformation( pathObject->transformations() );
    const QVector<CdrPathPoint>& pathPoints = pathObject->pathPoints();

    CdrPoint curveControlPoints[2];
    int curveControlPointCount = 0;
    CdrCoord minX;
    CdrCoord minY;
    CdrCoord maxX;
    CdrCoord maxY;

    QString pathData;
    if( pathPoints.count() > 0 )
    {
        const CdrPoint point = pathPoints.at(0).mPoint;

        pathData = QLatin1Char('M') + QString::number(point.x()) + QLatin1Char(' ') +
                   QString::number(point.y());

        minX = point.x();
        maxX = point.x();
        minY = point.y();
        maxY = point.y();
    }
    for( int j=1; j<pathPoints.count(); j++ )
    {
        const CdrPathPoint& pathPoint = pathPoints.at(j);
        const CdrCoord x = pathPoint.mPoint.x();
        const CdrCoord y = pathPoint.mPoint.y();
        if( x < minX )
            minX = x;
        else if( maxX < x )
            maxX = x;
        if( y < minY )
            minY = y;
        else if( maxY < y )
            maxY = y;

        const quint8 strokeToPointType = (pathPoint.mType&0xC0);

        // subpath start?
        if( strokeToPointType == 0x00 ) // no line to point?
        {
            if(( pathPoint.mType&0x04) == 0x04 )  // real point?
            {
                pathData = pathData + QLatin1String(" M") +
                        QString::number(x) + QLatin1Char(' ') +
                        QString::number(y);
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
                        QString::number(curveControlPoints[0].x()) + QLatin1Char(' ') +
                        QString::number(curveControlPoints[0].y()) + QLatin1Char(' ') +
                        QString::number(curveControlPoints[1].x()) + QLatin1Char(' ') +
                        QString::number(curveControlPoints[1].y()) + QLatin1Char(' ') +
                        QString::number(x) + QLatin1Char(' ') +
                        QString::number(y);
                curveControlPointCount = 0;
            }
            else //if( strokeToPointType == 0x40 )
                pathData = pathData + QLatin1String(" L") +
                        QString::number(x) + QLatin1Char(' ') +
                        QString::number(y);
            const bool isSubpathClosing = ((pathPoint.mType&0x0C) == 0x08);
            if( isSubpathClosing )
                pathData.append( QLatin1Char('z') );
        }
    }

    const CdrCoord width = maxX - minX +1;
    const CdrCoord height = maxY- minY +1;
    const QString viewBoxString =
        QString::number(minX) + QLatin1Char(' ') + QString::number(minY) + QLatin1Char(' ') +
        QString::number(width) + QLatin1Char(' ') + QString::number(height);
    mBodyWriter->addAttributePt("svg:x", odfXCoord(minX));
    mBodyWriter->addAttributePt("svg:y", odfYCoord(minY));
    mBodyWriter->addAttributePt("svg:width", odfLength(width) );
    mBodyWriter->addAttributePt("svg:height", odfLength(height) );
    mBodyWriter->addAttribute( "svg:viewBox", viewBoxString );

    mBodyWriter->addAttribute( "svg:d", pathData ) ;

    KoGenStyle style( KoGenStyle::GraphicAutoStyle, "graphic" );
    writeStroke( style, pathObject->outlineId() );
    writeFill( style, pathObject->fillId() );
    const QString styleName = mStyleCollector.insert( style, QLatin1String("pathStyle") );
    mBodyWriter->addAttribute( "draw:style-name", styleName );

    mBodyWriter->endElement(); // draw:path
}

void
CdrOdgWriter::writeGraphicTextObject( const CdrGraphicTextObject* object )
{
    mBodyWriter->startElement( "draw:frame" );
    mBodyWriter->addAttribute( "draw:layer", mLayerId );
    mBodyWriter->addAttributePt( "svg:x", odfXCoord(0));
    mBodyWriter->addAttributePt( "svg:y", odfYCoord(0));
    writeTransformation( object->transformations() );
        mBodyWriter->startElement( "draw:image" );
        mBodyWriter->addAttribute( "xlink:type", QLatin1String("simple") );
        mBodyWriter->addAttribute( "xlink:show", QLatin1String("embed") );
        mBodyWriter->addAttribute( "xlink:actuate", QLatin1String("onLoad") );
        mBodyWriter->addAttribute( "xlink:href", mSvgFilePathByObject.value(object) );

        mBodyWriter->endElement(); // draw:frame
    mBodyWriter->endElement(); // draw:frame
}

void
CdrOdgWriter::writeBlockTextObject( const CdrBlockTextObject* blockTextObject )
{
    const CdrBlockText* const blockText =
        mDocument->blockTextForObject( blockTextObject->objectId() );
    if( blockText == 0 )
        return; // TODO: rather check on parsing

    mBodyWriter->startElement("draw:frame");
    mBodyWriter->addAttribute( "draw:layer", mLayerId );
    mBodyWriter->addAttributePt( "svg:x", odfXCoord(0));
    mBodyWriter->addAttributePt( "svg:y", odfYCoord(0));
    mBodyWriter->addAttributePt( "svg:width", odfLength(blockTextObject->width()) );
    mBodyWriter->addAttributePt( "svg:height", odfLength(blockTextObject->height()) );
    writeTransformation( blockTextObject->transformations(), QLatin1String("scale(1 -1)") );

    KoGenStyle frameStyle( KoGenStyle::GraphicAutoStyle, "graphic" );
    frameStyle.addProperty( QLatin1String("style:overflow-behavior"), "clip" );
    const QString frameStyleName =
        mStyleCollector.insert( frameStyle, QLatin1String("frameStyle") );
    mBodyWriter->addAttribute( "draw:style-name", frameStyleName );

    mBodyWriter->startElement("draw:text-box");
    mBodyWriter->addAttributePt( "fo:max-width", odfLength(blockTextObject->width()) );
    mBodyWriter->addAttributePt( "fo:max-height", odfLength(blockTextObject->height()) );

    foreach( const CdrParagraph* paragraph, blockText->paragraphs() )
        writeParagraph( paragraph, blockTextObject );

    mBodyWriter->endElement();//draw:text-box
    mBodyWriter->endElement();//draw:frame
}

void
CdrOdgWriter::writeParagraph( const CdrParagraph* paragraph, const CdrBlockTextObject* blockTextObject )
{
    mBodyWriter->startElement( "text:p", false );  //false: we should not indent the inner tags

    foreach( const CdrParagraphLine* paragraphLine, paragraph->paragraphLines() )
    {
        foreach( const CdrAbstractTextSpan* textSpan, paragraphLine->textSpans() )
        {
            mBodyWriter->startElement( "text:span" );

            KoGenStyle textSpanStyle( KoGenStyle::TextAutoStyle, "text" );
            // block text global style
            writeFont( textSpanStyle, blockTextObject->styleId() );
            // span specific style
            if( textSpan->id() == CdrAbstractTextSpan::Styled )
                writeFont( textSpanStyle, static_cast<const CdrStyledTextSpan*>(textSpan) );

            const QString textSpanStyleName =
                mStyleCollector.insert( textSpanStyle, QLatin1String("textSpanStyle") );
            mBodyWriter->addAttribute( "text:style-name", textSpanStyleName );

            mBodyWriter->addTextNode( textSpan->text() );

            mBodyWriter->endElement(); //text:span
        }
    }

    mBodyWriter->endElement(); //text:p
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
CdrOdgWriter::writeStroke( KoGenStyle& odfStyle, quint32 outlineId )
{
    const CdrOutline* const outline = mDocument->outline( outlineId );
    if( outline == 0 ) // TODO: check when this can happen
        return;

    odfStyle.addProperty( QLatin1String("svg:stroke-color"), outline->color().name() );

    odfStyle.addPropertyPt( QLatin1String("svg:stroke-width"), odfLength(outline->strokeWidth()) );
    odfStyle.addProperty( QLatin1String("draw:stroke"), "solid" );

    const char* const linejoin =
        (outline->strokeJoinType() == CdrStrokeRoundJoin) ?  "round" :
        (outline->strokeJoinType() == CdrStrokeBevelJoin) ?  "bevel" :
                                                             "miter";
    odfStyle.addProperty( QLatin1String("draw:stroke-linejoin"), linejoin );

    const char* const linecap =
        (outline->strokeCapType() == CdrStrokeRoundCap) ?  "round" :
        (outline->strokeCapType() == CdrStrokeSquareCap) ? "square" :
                                                           "butt";
    odfStyle.addProperty( QLatin1String("svg:stroke-linecap"), linecap );
}

void
CdrOdgWriter::writeFont( KoGenStyle& odfStyle, quint16 styleId )
{
    const CdrStyle* const style = mDocument->style( styleId );
    if( style == 0 ) // TODO: check when this can happen
        return;

    odfStyle.addPropertyPt( QLatin1String("fo:font-size"), odfLength(style->fontSize()) );
    const char* const weight =
        (style->fontWeight() == CdrFontBold) ?  "bold" :
                                                "normal";
    odfStyle.addProperty( QLatin1String("fo:font-weight"), weight );
    const CdrFont* const font = mDocument->font( style->fontId() );
    if( font )
        odfStyle.addProperty( QLatin1String("style:font-name"), font->name() );
}

void
CdrOdgWriter::writeFont( KoGenStyle& odfStyle, const CdrStyledTextSpan* textSpan )
{
    odfStyle.addPropertyPt( QLatin1String("fo:font-size"), odfLength(textSpan->fontSize()) );
    const char* const weight =
        (textSpan->fontWeight() == CdrFontBold) ?  "bold" :
                                                   "normal";
    odfStyle.addProperty( QLatin1String("fo:font-weight"), weight );
    const CdrFont* const font = mDocument->font( textSpan->fontId() );
    if( font )
        odfStyle.addProperty( QLatin1String("style:font-name"), font->name() );
}


static inline
void
appendMatrix( QString& transformationString,
              double f1, double f2, double x, double f3, double f4, double y,
              const QLocale& locale )
{
    transformationString =
        transformationString + QLatin1String("matrix(") +
        locale.toString(f1) + QLatin1Char(' ') +
        locale.toString(f3) + QLatin1Char(' ') +
        locale.toString(f2) + QLatin1Char(' ') +
        locale.toString(f4) + QLatin1Char(' ') +
        locale.toString(x) + QLatin1String("pt ") +
        locale.toString(y) + QLatin1String("pt)");
}

static inline
void
appendTranslation( QString& transformationString,
                   double x, double y,
                   const QLocale& locale )
{
    transformationString =
        transformationString + QLatin1String("translate(") +
        locale.toString(x) + QLatin1String("pt ") +
        locale.toString(y) + QLatin1String("pt)");
}

void
CdrOdgWriter::writeTransformation( const QVector<CdrAbstractTransformation*>& transformations,
                                   const QString& baseTransformationString )
{
    QLocale cLocale(QLocale::c());
    cLocale.setNumberOptions(QLocale::OmitGroupSeparator);

    QString transformationString = baseTransformationString;
    foreach( const CdrAbstractTransformation* transformation, transformations )
    {
        const CdrNormalTransformation* normalTrafo =
            dynamic_cast<const CdrNormalTransformation*>(transformation);

        if( normalTrafo )
        {
            appendMatrix( transformationString,
                          normalTrafo->a(), normalTrafo->c(),
                          odfXCoord(normalTrafo->e()),
                          normalTrafo->b(), normalTrafo->d(),
                          odfYCoord(normalTrafo->f()),
                          cLocale );
        }
    }

    // finally transformation between cdr and odf
    appendMatrix( transformationString,
                  1.0, 0.0,
                  odfLength(mDocument->width())*0.5,
                  0.0, -1.0,
                  odfLength(mDocument->height())*0.5,
                  cLocale );

    mBodyWriter->addAttribute( "draw:transform", transformationString );
}

void
CdrOdgWriter::writeCornerPoint( CdrPoint cornerPoint )
{
    const CdrCoord x = (cornerPoint.x() >= 0)? 0 : cornerPoint.x();
    const CdrCoord y = (cornerPoint.y() >= 0)? 0 : cornerPoint.y();
    const CdrCoord width = qAbs(cornerPoint.x());
    const CdrCoord height = qAbs(cornerPoint.y());
    mBodyWriter->addAttributePt("svg:x", odfXCoord(x));
    mBodyWriter->addAttributePt("svg:y", odfYCoord(y));
    mBodyWriter->addAttributePt("svg:width", odfLength(width) );
    mBodyWriter->addAttributePt("svg:height", odfLength(height) );
}
