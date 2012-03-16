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

#include "xfigodgwriter.h"

// filter
#include "xfigdocument.h"
// Calligra
#include <KoXmlWriter.h>
#include <KoXmlNS.h>
#include <KoOdf.h>
#include <KoStore.h>
#include <KoStoreDevice.h>
#include <KoDocumentInfo.h>

// keep in sync with XFigPageSizeType
static const
struct PageSize
{
    const char* width;
    const char* height;
} pageSizeTable[15] =
{
    {  "8.5in",  "11in"}, // letter
    {  "8.5in",  "14in"}, // legal
    { "17in",    "11in"}, // ledger
    { "11in",    "17in"}, // tabloid
    {  "8.5in",  "11in"}, // A
    { "11in",    "17in"}, // B
    { "17in",    "22in"}, // C
    { "22in",    "34in"}, // D
    { "34in",    "44in"}, // E
    {"210mm",   "297mm"}, // A4
    {"297mm",   "420mm"}, // A3
    {"420mm",   "594mm"}, // A2
    {"594mm",   "841mm"}, // A1
    {"841mm",  "1189mm"}, // A0
    {"176mm",   "250mm"}, // B5
};


// Metric conversion
// 1 point = 1/72 inches
static const double inchtoPtFactor = 72;
// TODO: check that support for both inch and metric system works

static inline
double
ptUnit( double x )
{
    return x * inchtoPtFactor;
}

static inline
double
odfCornerRadius( qint32 xfigRadius )
{
    return ptUnit( static_cast<double>(xfigRadius) / 80);
}

static inline
double
odfLineThickness( qint32 xfigLineThickness )
{
    return ptUnit( static_cast<double>(xfigLineThickness) / 80);
}

double
XFigOdgWriter::odfLength( qint32 length ) const
{
    return ptUnit( static_cast<double>(length) / mDocument->resolution() );
}

double
XFigOdgWriter::odfXCoord( qint32 x ) const
{
    return ptUnit( static_cast<double>(x) / mDocument->resolution() );
}

double
XFigOdgWriter::odfYCoord( qint32 y ) const
{
    return ptUnit( static_cast<double>(y) / mDocument->resolution() );
}



XFigOdgWriter::XFigOdgWriter( KoStore* outputStore )
  : mCLocale(QLocale::c())
  , mOdfWriteStore( outputStore )
  , mOutputStore( outputStore )
  , mPageCount( 0 )
{
    mCLocale.setNumberOptions(QLocale::OmitGroupSeparator);
    mManifestWriter = mOdfWriteStore.manifestWriter( KoOdf::mimeType(KoOdf::Graphics) );
}

XFigOdgWriter::~XFigOdgWriter()
{
    mOdfWriteStore.closeManifestWriter();
    delete mOutputStore;
}

bool
XFigOdgWriter::write( XFigDocument* document )
{
    mDocument = document;

    storePixelImageFiles();

    // Create content.xml
    storeContentXml();

    // Create the styles.xml file
    mStyleCollector.saveOdfStylesDotXml( mOutputStore, mManifestWriter );

    // Create meta.xml
    storeMetaXml();

    return true;
}

void
XFigOdgWriter::storePixelImageFiles()
{
    // TODO: store pixel files linked from XFigPictureBoxObject
#if 0
    // TODO: as mManifestWriter needs full rel path, perhaps drop enterDirectory/leaveDirectory
    mOutputStore->enterDirectory( QLatin1String("Pictures") );

    foreach( const XFigPixelImage* image, mDocument->pixelImages() )
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
XFigOdgWriter::storeMetaXml()
{
    KoDocumentInfo documentInfo;
    documentInfo.setOriginalGenerator(QLatin1String("Calligra XFig filter"));
    documentInfo.setAboutInfo(QLatin1String("comments"), mDocument->comment());

    const QString documentInfoFilePath = QLatin1String( "meta.xml" );

    mOutputStore->open( documentInfoFilePath );
    documentInfo.saveOasis( mOutputStore );
    mOutputStore->close();

    // TODO: "text/xml" could be a static string
    mManifestWriter->addManifestEntry( documentInfoFilePath, QLatin1String("text/xml") );
}

void
XFigOdgWriter::storeContentXml()
{
    KoXmlWriter* contentWriter = mOdfWriteStore.contentWriter();
    mBodyWriter = mOdfWriteStore.bodyWriter();

    mBodyWriter->startElement( "office:body" );
    mBodyWriter->startElement( KoOdf::bodyContentElement(KoOdf::Graphics, true));

    writeMasterPage();

    foreach( const XFigPage* page, mDocument->pages() )
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
XFigOdgWriter::writeMasterPage()
{
    KoGenStyle masterPageStyle( KoGenStyle::MasterPageStyle );

    KoGenStyle masterPageLayoutStyle( KoGenStyle::PageLayoutStyle );
    masterPageLayoutStyle.setAutoStyleInStylesDotXml( true );

    if (mDocument->pageSizeType() != XFigPageSizeUnknown) {
        const PageSize& pageSize = pageSizeTable[mDocument->pageSizeType()-1];
        masterPageLayoutStyle.addProperty( QLatin1String("fo:page-width"), pageSize.width );
        masterPageLayoutStyle.addProperty( QLatin1String("fo:page-height"), pageSize.height );
    }

    const char* const orientation =
        (mDocument->pageOrientation()==XFigPagePortrait) ?  "portrait":
        (mDocument->pageOrientation()==XFigPageLandscape) ? "landscape":
                                                            0;
    if( orientation != 0 )
        masterPageLayoutStyle.addProperty( QLatin1String("style:print-orientation"), orientation );

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
XFigOdgWriter::writePage( const XFigPage* page )
{
    mBodyWriter->startElement( "draw:page" );

    mBodyWriter->addAttribute( "xml:id", QLatin1String("page")+QString::number(mPageCount++) );
    mBodyWriter->addAttribute( "draw:master-page-name", mMasterPageStyleName );

    // there is only one layer
    mLayerId = QLatin1String("Layer");
    mBodyWriter->startElement( "draw:layer-set" );
        mBodyWriter->startElement( "draw:layer" );
        mBodyWriter->addAttribute( "draw:name", mLayerId );
        mBodyWriter->endElement(); //draw:layer
    mBodyWriter->endElement(); //draw:layer-set

    // objects
    foreach( const XFigAbstractObject* object, page->objects() )
        writeObject( object );

    mBodyWriter->endElement(); //draw:page
}

void
XFigOdgWriter::writeObject( const XFigAbstractObject* object )
{
    const XFigAbstractObject::TypeId typeId = object->typeId();

    if (typeId == XFigAbstractObject::EllipseId)
        writeEllipseObject( static_cast<const XFigEllipseObject*>(object) );
    else if (typeId == XFigAbstractObject::PolylineId)
        writePolylineObject( static_cast<const XFigPolylineObject*>(object) );
    else if (typeId == XFigAbstractObject::PolygonId)
        writePolygonObject( static_cast<const XFigPolygonObject*>(object) );
    else if (typeId == XFigAbstractObject::BoxId)
        writeBoxObject( static_cast<const XFigBoxObject*>(object) );
    else if (typeId == XFigAbstractObject::PictureBoxId)
        writePictureBoxObject( static_cast<const XFigPictureBoxObject*>(object) );
    else if (typeId == XFigAbstractObject::SplineId)
        writeSplineObject( static_cast<const XFigSplineObject*>(object) );
    else if (typeId == XFigAbstractObject::ArcId)
        writeArcObject( static_cast<const XFigArcObject*>(object) );
    else if (typeId == XFigAbstractObject::TextId)
        writeTextObject( static_cast<const XFigTextObject*>(object) );
    else if( typeId == XFigAbstractObject::CompoundId )
        writeCompoundObject( static_cast<const XFigCompoundObject*>(object) );
}

void
XFigOdgWriter::writeCompoundObject( const XFigCompoundObject* groupObject )
{
    mBodyWriter->startElement("draw:g");
    mBodyWriter->addAttribute("draw:layer", mLayerId );
    writeComment(groupObject);

    foreach( const XFigAbstractObject* object, groupObject->objects() )
        writeObject( object );

    mBodyWriter->endElement(); //draw:g
}

void
XFigOdgWriter::writeEllipseObject(const XFigEllipseObject* ellipseObject)
{
    mBodyWriter->startElement("draw:ellipse");

    writeZIndex(ellipseObject);

    const XFigPoint centerPoint = ellipseObject->centerPoint();
    mBodyWriter->addAttribute("svg:cx", "0");
    mBodyWriter->addAttribute("svg:cy", "0");
    mBodyWriter->addAttributePt("svg:rx", odfLength(ellipseObject->xRadius()));
    mBodyWriter->addAttributePt("svg:ry", odfLength(ellipseObject->yRadius()));

    const QString transformationString =
        QLatin1String("rotate(") + mCLocale.toString(ellipseObject->xAxisAngle()) +
        QLatin1String(")translate(") +
        mCLocale.toString(odfXCoord(centerPoint.x())) + QLatin1String("pt ") +
        mCLocale.toString(odfYCoord(centerPoint.y())) + QLatin1String("pt)");
    mBodyWriter->addAttribute("draw:transform", transformationString);

    {
        KoGenStyle ellipseStyle(KoGenStyle::GraphicAutoStyle, "graphic");
        writeStroke(ellipseStyle, ellipseObject );
        writeFill(ellipseStyle, ellipseObject );
        const QString ellipseStyleName = mStyleCollector.insert(ellipseStyle, QLatin1String("ellipseStyle"));
        mBodyWriter->addAttribute("draw:style-name", ellipseStyleName);
    }

    writeComment(ellipseObject);

    mBodyWriter->endElement(); // draw:ellipse
}

void
XFigOdgWriter::writePolylineObject(const XFigPolylineObject* polylineObject)
{
    mBodyWriter->startElement("draw:polyline");

    writeZIndex(polylineObject);

    writePoints(polylineObject->points());

    {
        KoGenStyle polylineStyle(KoGenStyle::GraphicAutoStyle, "graphic");
        writeStroke(polylineStyle, polylineObject);
        writeFill(polylineStyle, polylineObject);
        writeJoinType(polylineStyle, polylineObject->joinType());
        writeCapType(polylineStyle, polylineObject->capType());
        writeArrow(polylineStyle, polylineObject->backwardArrow(), LineStart);
        writeArrow(polylineStyle, polylineObject->forwardArrow(), LineEnd);
        const QString polylineStyleName =
            mStyleCollector.insert(polylineStyle, QLatin1String("polylineStyle"));
        mBodyWriter->addAttribute("draw:style-name", polylineStyleName);
    }

    writeComment(polylineObject);

    mBodyWriter->endElement(); // draw:polyline
}

void
XFigOdgWriter::writePolygonObject( const XFigPolygonObject* polygonObject )
{
    mBodyWriter->startElement("draw:polygon");

    writeZIndex(polygonObject);

    writePoints(polygonObject->points());

    {
        KoGenStyle polygonStyle(KoGenStyle::GraphicAutoStyle, "graphic");
        writeStroke(polygonStyle, polygonObject);
        writeFill(polygonStyle, polygonObject);
        writeJoinType(polygonStyle, polygonObject->joinType());
        const QString polygonStyleName =
            mStyleCollector.insert(polygonStyle, QLatin1String("polygonStyle"));
        mBodyWriter->addAttribute("draw:style-name", polygonStyleName);
    }

    writeComment(polygonObject);

    mBodyWriter->endElement(); // draw:polygon
}

void
XFigOdgWriter::writeBoxObject( const XFigBoxObject* boxObject )
{
    mBodyWriter->startElement("draw:rect");

    writeZIndex( boxObject );

    const XFigPoint upperleft = boxObject->upperLeft();
    mBodyWriter->addAttributePt("svg:x", odfXCoord(upperleft.x()));
    mBodyWriter->addAttributePt("svg:y", odfYCoord(upperleft.y()));
    mBodyWriter->addAttributePt("svg:width", odfLength(boxObject->width()));
    mBodyWriter->addAttributePt("svg:height", odfLength(boxObject->height()));

    const qint32 radius = boxObject->radius();
    if (radius != 0) {
        const double odfRadius = odfCornerRadius(radius);
        mBodyWriter->addAttributePt("svg:rx", odfRadius);
        mBodyWriter->addAttributePt("svg:ry", odfRadius);
    }

    {
        KoGenStyle boxStyle(KoGenStyle::GraphicAutoStyle, "graphic");
        writeStroke(boxStyle, boxObject);
        writeFill(boxStyle, boxObject);
        writeJoinType(boxStyle, boxObject->joinType());
        const QString boxStyleName = mStyleCollector.insert(boxStyle, QLatin1String("boxStyle"));
        mBodyWriter->addAttribute("draw:style-name", boxStyleName);
    }

    writeComment(boxObject);

    mBodyWriter->endElement(); // draw:rect
}

void
XFigOdgWriter::writePictureBoxObject( const XFigPictureBoxObject* /*pictureBoxObject*/ )
{
    // TODO
}

void
XFigOdgWriter::writeSplineObject( const XFigSplineObject* /*object*/ )
{
}

void
XFigOdgWriter::writeArcObject( const XFigArcObject* /*object*/ )
{
    // TODO
}


void
XFigOdgWriter::writeTextObject( const XFigTextObject* textObject )
{
    mBodyWriter->startElement("draw:frame");

    writeZIndex(textObject);

    const XFigPoint point = textObject->baselineStartPoint();
    mBodyWriter->addAttributePt( "svg:x", odfXCoord(point.x()));
    mBodyWriter->addAttributePt( "svg:y", odfYCoord(point.y())-textObject->fontData().mSize); // TODO: get baseline
    mBodyWriter->addAttributePt( "svg:width", odfLength(textObject->length()) );
    mBodyWriter->addAttributePt( "svg:height", odfLength(textObject->height()) );

    KoGenStyle frameStyle( KoGenStyle::GraphicAutoStyle, "graphic" );
    frameStyle.addProperty( QLatin1String("style:overflow-behavior"), "clip" );
    const QString frameStyleName =
        mStyleCollector.insert( frameStyle, QLatin1String("frameStyle") );
    mBodyWriter->addAttribute( "draw:style-name", frameStyleName );

    mBodyWriter->startElement("draw:text-box");

    mBodyWriter->startElement( "text:p", false );  //false: we should not indent the inner tags

    KoGenStyle paragraphStyle( KoGenStyle::ParagraphAutoStyle, "paragraph" );
    writeParagraphStyle( paragraphStyle, textObject );

    const QString paragraphStyleName =
        mStyleCollector.insert( paragraphStyle, QLatin1String("paragraphStyle") );
    mBodyWriter->addAttribute( "text:style-name", paragraphStyleName );

    mBodyWriter->startElement( "text:span" );

    KoGenStyle textSpanStyle( KoGenStyle::TextAutoStyle, "text" );
    writeFont( textSpanStyle, textObject );

    const QString textSpanStyleName =
        mStyleCollector.insert( textSpanStyle, QLatin1String("textSpanStyle") );
    mBodyWriter->addAttribute( "text:style-name", textSpanStyleName );

    mBodyWriter->addTextNode( textObject->text() );

    mBodyWriter->endElement(); //text:span
    mBodyWriter->endElement(); //text:p

    mBodyWriter->endElement();//draw:text-box

    writeComment(textObject);

    mBodyWriter->endElement();//draw:frame
}

void
XFigOdgWriter::writeZIndex( const XFigAbstractGraphObject* graphObject )
{
    mBodyWriter->addAttribute( "draw:z-index", (1000-graphObject->depth()) );
    mBodyWriter->addAttribute( "draw:layer", mLayerId );
}

void
XFigOdgWriter::writePoints( const QVector<XFigPoint>& points )
{
    QString pointsString;
    int i = 0;
    while (true) {
        const XFigPoint& point = points.at(i);
        const double x = odfXCoord(point.x());
        const double y = odfXCoord(point.y());
        pointsString +=  mCLocale.toString(x)+QLatin1Char(',')+mCLocale.toString(y);
        ++i;
        if (i >= points.count())
            break;
        pointsString += QLatin1Char(' ');
    }
    mBodyWriter->addAttribute("draw:points", pointsString );
}

void XFigOdgWriter::writeComment(const XFigAbstractObject* object)
{
    const QString& comment = object->comment();
    if (comment.isEmpty())
        return;

    mBodyWriter->startElement("svg:desc");
    mBodyWriter->addTextNode(comment);
    mBodyWriter->endElement(); // svg:desc
}

void
XFigOdgWriter::writeFill( KoGenStyle& odfStyle, const XFigFillable* fillable )
{
    // TODO: support for fill patterns not yet done, mapping to solid fill for now
    const qint32 fillStyleId = (fillable->fillStyleId() > 20) ? 20 : fillable->fillStyleId();

    const bool isNotFilled = ( fillStyleId == -1 );

    odfStyle.addProperty( QLatin1String("draw:fill"), isNotFilled ? "none" : "solid" );

    if (! isNotFilled) {
        const qint32 fillColorId = fillable->fillColorId();

        QString colorString;
        // BLACK or DEFAULT color?
        if (fillColorId < 1) {
            // 0: white, 20: black, 1..19 shades of grey, from lighter to darker
            const int value = qRound((20 - fillStyleId) * 255.0 / 20.0);
            colorString = QColor(value, value, value).name();
        // WHITE color ?
        } else if (fillColorId == 7) {
            // 0: black, 20: white, 1..19 shades of grey, from darker to lighter
            const int value = qRound(fillStyleId * 255.0 / 20.0);
            colorString = QColor(value, value, value).name();
        } else {
            //TODO: tint blackness/whiteness of color
            const QColor* color = mDocument->color(fillColorId);
            if (color != 0)
                colorString = color->name();
        }

        odfStyle.addProperty( QLatin1String("draw:fill-color"), colorString );
    }
}

void
XFigOdgWriter::writeDotDash( KoGenStyle& odfStyle, int lineType, double distance )
{
    const double odfDistance = odfLineThickness(distance);
    odfStyle.addAttribute(QLatin1String("draw:style"), "rect");
    odfStyle.addAttribute(QLatin1String("draw:distance"),
                          mCLocale.toString(odfDistance)+QLatin1String("pt"));

    const char* displayName = 0;
    bool isFirstDot = false;
    const char* secondDotsNumber = 0;

    switch (lineType) {
    case XFigLineDashed:
        displayName = "Dashed";
        break;
    case XFigLineDotted:
        displayName = "Dotted";
        isFirstDot = true;
        break;
    case XFigLineDashDotted:
        displayName = "1 Dot 1 Dash";
        secondDotsNumber = "1";
        break;
    case XFigLineDashDoubleDotted:
        displayName = "1 Dash 2 Dots";
        secondDotsNumber = "2";
        break;
    case XFigLineDashTrippleDotted:
        displayName = "1 Dash 3 Dots";
        secondDotsNumber = "3";
        break;
    }

    odfStyle.addAttribute(QLatin1String("draw:display-name"), displayName);
    odfStyle.addAttribute(QLatin1String("draw:dots1"), "1");
    odfStyle.addAttribute(QLatin1String("draw:dots1-length"),
                          isFirstDot ? QString::fromLatin1("100%") :
                                       mCLocale.toString(odfDistance)+QLatin1String("pt"));
    if (secondDotsNumber!=0) {
        odfStyle.addAttribute(QLatin1String("draw:dots2"), QLatin1String(secondDotsNumber));
        odfStyle.addAttribute(QLatin1String("draw:dots2-length"), "100%");
    }
}

void
XFigOdgWriter::writeStroke( KoGenStyle& odfStyle, const XFigLineable* lineable )
{
    const qint32 colorId = lineable->lineColorId();
    if (colorId >= 0) {
        const QColor* color = mDocument->color(colorId);
        if (color != 0)
            odfStyle.addProperty( QLatin1String("svg:stroke-color"), color->name() );
    }

    odfStyle.addPropertyPt( QLatin1String("svg:stroke-width"), odfLineThickness(lineable->lineThickness()) );

    const XFigLineType lineType = lineable->lineType();
    const bool isDashed = (lineType != XFigLineSolid) && (lineType != XFigLineDefault);
    odfStyle.addProperty( QLatin1String("draw:stroke"), (isDashed) ? "dash" : "solid" );
    if (isDashed) {
        KoGenStyle dashStyle(KoGenStyle::StrokeDashStyle);
        writeDotDash(dashStyle, lineType, lineable->lineStyleValue());
        const QString dashStyleName = mStyleCollector.insert( dashStyle, QLatin1String("dashStyle") );
        odfStyle.addProperty(QLatin1String("draw:stroke-dash"), dashStyleName);
    }
}

void
XFigOdgWriter::writeJoinType(KoGenStyle& odfStyle, int joinType)
{
    const char* const linejoin =
        (joinType == XFigJoinRound) ?  "round" :
        (joinType == XFigJoinBevel) ?  "bevel" :
                                       "miter";
    odfStyle.addProperty(QLatin1String("draw:stroke-linejoin"), linejoin);
}

void
XFigOdgWriter::writeCapType(KoGenStyle& odfStyle, int capType)
{
    const char* const linecap =
        (capType == XFigCapRound) ?      "round" :
        (capType == XFigCapProjecting) ? "square" :
                                         "butt";
    odfStyle.addProperty(QLatin1String("svg:stroke-linecap"), linecap);
}

void
XFigOdgWriter::writeArrow(KoGenStyle& odfStyle, const XFigArrowHead* arrow, LineEndType lineEndType)
{
    if (arrow != 0) {
        KoGenStyle arrowStyle(KoGenStyle::MarkerStyle);
        // TODO: support all arrow types. this is just a tmp. substitute copied from kpr2odf
        const char* const displayName = "Line Arrow";
        const char* const viewBox = "0 0 1122 2243";
        const char* const d = "m0 2108v17 17l12 42 30 34 38 21 43 4 29-8 30-21 25-26 13-34 343-1532 339 1520 13 42 29 34 39 21 42 4 42-12 34-30 21-42v-39-12l-4 4-440-1998-9-42-25-39-38-25-43-8-42 8-38 25-26 39-8 42z";

        arrowStyle.addAttribute(QLatin1String("draw:display-name"), displayName);
        arrowStyle.addAttribute(QLatin1String("svg:viewBox"), viewBox);
        arrowStyle.addAttribute(QLatin1String("svg:d"), d);
        const QString arrowStyleName =
            mStyleCollector.insert(arrowStyle, QLatin1String("arrowStyle"));

        const char* const markerStart =
            (lineEndType==LineStart) ? "draw:marker-start" : "draw:marker-end";
        const char* const markerStartWidth =
            (lineEndType==LineStart) ? "draw:marker-start-width" : "draw:marker-end-width";
        odfStyle.addProperty(QLatin1String(markerStart), arrowStyleName);
        odfStyle.addPropertyPt(QLatin1String(markerStartWidth), odfLineThickness(arrow->thickness()));
    }
}

void
XFigOdgWriter::writeFont( KoGenStyle& odfStyle, const XFigTextObject* textObject )
{
    const XFigFontData& fontData = textObject->fontData();

    odfStyle.addPropertyPt(QLatin1String("fo:font-size"), fontData.mSize);
    const char* const weight =
        (fontData.mWeight == QFont::Bold) ?     "bold" :
        (fontData.mWeight == QFont::DemiBold) ? "600" :
                                                "normal";
    odfStyle.addProperty(QLatin1String("fo:font-weight"), weight);
    const char* const style =
        (fontData.mStyle == QFont::StyleItalic) ?  "italic" :
        (fontData.mStyle == QFont::StyleOblique) ? "oblique" :
                                                   "normal";
    odfStyle.addProperty(QLatin1String("fo:font-style"), style);
    if (! fontData.mFamily.isEmpty())
        odfStyle.addProperty( QLatin1String("fo:font-family"), fontData.mFamily );
}


void
XFigOdgWriter::writeParagraphStyle( KoGenStyle& odfStyle, const XFigTextObject* textObject )
{
    const XFigTextAlignment textAlignment = textObject->textAlignment();
    const char* const alignmentName =
        (textAlignment == XFigTextCenterAligned) ? "center" :
        (textAlignment == XFigTextRightAligned) ?  "right" :
        /* XFigTextLeftAligned */                  "left";
    odfStyle.addProperty( QLatin1String("fo:text-align"), QLatin1String(alignmentName) );
}
