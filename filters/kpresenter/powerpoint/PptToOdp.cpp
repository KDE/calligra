/* This file is part of the KDE project
   Copyright (C) 2005 Yolla Indria <yolla.indria@gmail.com>
   Copyright (C) 2010 KO GmbH <jos.van.den.oever@kogmbh.com>

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
 * Boston, MA 02110-1301, USA.
*/

#include "PptToOdp.h"
#include "pictures.h"

#include <kdebug.h>
#include <KoOdf.h>
#include <KoOdfWriteStore.h>
#include <KoXmlWriter.h>

#include <QtCore/QBuffer>

#include <cmath>

using namespace PPT;

enum {
    msosptMin = 0,
    msosptNotPrimitive = msosptMin,
    msosptRectangle = 1,
    msosptRoundRectangle = 2,
    msosptEllipse = 3,
    msosptDiamond = 4,
    msosptIsoscelesTriangle = 5,
    msosptRightTriangle = 6,
    msosptParallelogram = 7,
    msosptTrapezoid = 8,
    msosptHexagon = 9,
    msosptOctagon = 10,
    msosptPlus = 11,
    msosptStar = 12,
    msosptArrow = 13,
    msosptThickArrow = 14,
    msosptHomePlate = 15,
    msosptCube = 16,
    msosptBalloon = 17,
    msosptSeal = 18,
    msosptArc = 19,
    msosptLine = 20,
    msosptPlaque = 21,
    msosptCan = 22,
    msosptDonut = 23,
    msosptTextSimple = 24,
    msosptTextOctagon = 25,
    msosptTextHexagon = 26,
    msosptTextCurve = 27,
    msosptTextWave = 28,
    msosptTextRing = 29,
    msosptTextOnCurve = 30,
    msosptTextOnRing = 31,
    msosptStraightConnector1 = 32,
    msosptBentConnector2 = 33,
    msosptBentConnector3 = 34,
    msosptBentConnector4 = 35,
    msosptBentConnector5 = 36,
    msosptCurvedConnector2 = 37,
    msosptCurvedConnector3 = 38,
    msosptCurvedConnector4 = 39,
    msosptCurvedConnector5 = 40,
    msosptCallout1 = 41,
    msosptCallout2 = 42,
    msosptCallout3 = 43,
    msosptAccentCallout1 = 44,
    msosptAccentCallout2 = 45,
    msosptAccentCallout3 = 46,
    msosptBorderCallout1 = 47,
    msosptBorderCallout2 = 48,
    msosptBorderCallout3 = 49,
    msosptAccentBorderCallout1 = 50,
    msosptAccentBorderCallout2 = 51,
    msosptAccentBorderCallout3 = 52,
    msosptRibbon = 53,
    msosptRibbon2 = 54,
    msosptChevron = 55,
    msosptPentagon = 56,
    msosptNoSmoking = 57,
    msosptSeal8 = 58,
    msosptSeal16 = 59,
    msosptSeal32 = 60,
    msosptWedgeRectCallout = 61,
    msosptWedgeRRectCallout = 62,
    msosptWedgeEllipseCallout = 63,
    msosptWave = 64,
    msosptFoldedCorner = 65,
    msosptLeftArrow = 66,
    msosptDownArrow = 67,
    msosptUpArrow = 68,
    msosptLeftRightArrow = 69,
    msosptUpDownArrow = 70,
    msosptIrregularSeal1 = 71,
    msosptIrregularSeal2 = 72,
    msosptLightningBolt = 73,
    msosptHeart = 74,
    msosptPictureFrame = 75,
    msosptQuadArrow = 76,
    msosptLeftArrowCallout = 77,
    msosptRightArrowCallout = 78,
    msosptUpArrowCallout = 79,
    msosptDownArrowCallout = 80,
    msosptLeftRightArrowCallout = 81,
    msosptUpDownArrowCallout = 82,
    msosptQuadArrowCallout = 83,
    msosptBevel = 84,
    msosptLeftBracket = 85,
    msosptRightBracket = 86,
    msosptLeftBrace = 87,
    msosptRightBrace = 88,
    msosptLeftUpArrow = 89,
    msosptBentUpArrow = 90,
    msosptBentArrow = 91,
    msosptSeal24 = 92,
    msosptStripedRightArrow = 93,
    msosptNotchedRightArrow = 94,
    msosptBlockArc = 95,
    msosptSmileyFace = 96,
    msosptVerticalScroll = 97,
    msosptHorizontalScroll = 98,
    msosptCircularArrow = 99,
    msosptNotchedCircularArrow = 100,
    msosptUturnArrow = 101,
    msosptCurvedRightArrow = 102,
    msosptCurvedLeftArrow = 103,
    msosptCurvedUpArrow = 104,
    msosptCurvedDownArrow = 105,
    msosptCloudCallout = 106,
    msosptEllipseRibbon = 107,
    msosptEllipseRibbon2 = 108,
    msosptFlowChartProcess = 109,
    msosptFlowChartDecision = 110,
    msosptFlowChartInputOutput = 111,
    msosptFlowChartPredefinedProcess = 112,
    msosptFlowChartInternalStorage = 113,
    msosptFlowChartDocument = 114,
    msosptFlowChartMultidocument = 115,
    msosptFlowChartTerminator = 116,
    msosptFlowChartPreparation = 117,
    msosptFlowChartManualInput = 118,
    msosptFlowChartManualOperation = 119,
    msosptFlowChartConnector = 120,
    msosptFlowChartPunchedCard = 121,
    msosptFlowChartPunchedTape = 122,
    msosptFlowChartSummingJunction = 123,
    msosptFlowChartOr = 124,
    msosptFlowChartCollate = 125,
    msosptFlowChartSort = 126,
    msosptFlowChartExtract = 127,
    msosptFlowChartMerge = 128,
    msosptFlowChartOfflineStorage = 129,
    msosptFlowChartOnlineStorage = 130,
    msosptFlowChartMagneticTape = 131,
    msosptFlowChartMagneticDisk = 132,
    msosptFlowChartMagneticDrum = 133,
    msosptFlowChartDisplay = 134,
    msosptFlowChartDelay = 135,
    msosptTextPlainText = 136,
    msosptTextStop = 137,
    msosptTextTriangle = 138,
    msosptTextTriangleInverted = 139,
    msosptTextChevron = 140,
    msosptTextChevronInverted = 141,
    msosptTextRingInside = 142,
    msosptTextRingOutside = 143,
    msosptTextArchUpCurve = 144,
    msosptTextArchDownCurve = 145,
    msosptTextCircleCurve = 146,
    msosptTextButtonCurve = 147,
    msosptTextArchUpPour = 148,
    msosptTextArchDownPour = 149,
    msosptTextCirclePour = 150,
    msosptTextButtonPour = 151,
    msosptTextCurveUp = 152,
    msosptTextCurveDown = 153,
    msosptTextCascadeUp = 154,
    msosptTextCascadeDown = 155,
    msosptTextWave1 = 156,
    msosptTextWave2 = 157,
    msosptTextWave3 = 158,
    msosptTextWave4 = 159,
    msosptTextInflate = 160,
    msosptTextDeflate = 161,
    msosptTextInflateBottom = 162,
    msosptTextDeflateBottom = 163,
    msosptTextInflateTop = 164,
    msosptTextDeflateTop = 165,
    msosptTextDeflateInflate = 166,
    msosptTextDeflateInflateDeflate = 167,
    msosptTextFadeRight = 168,
    msosptTextFadeLeft = 169,
    msosptTextFadeUp = 170,
    msosptTextFadeDown = 171,
    msosptTextSlantUp = 172,
    msosptTextSlantDown = 173,
    msosptTextCanUp = 174,
    msosptTextCanDown = 175,
    msosptFlowChartAlternateProcess = 176,
    msosptFlowChartOffpageConnector = 177,
    msosptCallout90 = 178,
    msosptAccentCallout90 = 179,
    msosptBorderCallout90 = 180,
    msosptAccentBorderCallout90 = 181,
    msosptLeftRightUpArrow = 182,
    msosptSun = 183,
    msosptMoon = 184,
    msosptBracketPair = 185,
    msosptBracePair = 186,
    msosptSeal4 = 187,
    msosptDoubleWave = 188,
    msosptActionButtonBlank = 189,
    msosptActionButtonHome = 190,
    msosptActionButtonHelp = 191,
    msosptActionButtonInformation = 192,
    msosptActionButtonForwardNext = 193,
    msosptActionButtonBackPrevious = 194,
    msosptActionButtonEnd = 195,
    msosptActionButtonBeginning = 196,
    msosptActionButtonReturn = 197,
    msosptActionButtonDocument = 198,
    msosptActionButtonSound = 199,
    msosptActionButtonMovie = 200,
    msosptHostControl = 201,
    msosptTextBox = 202,
    msosptMax,
    msosptNil = 0x0FFF
} ;

namespace
{
static const QString mm("%1mm");

/**
 * Retrieve an option from an options containing class B
 *
 * @p b must have a member fopt that is an array of
 * type OfficeArtFOPTEChoice.
 * A is the type of the required option. The option containers
 * in PPT have only one instance of each option in a option container.
 * @param b class that contains options.
 * @return pointer to the option of type A or 0 if there is none.
 */
template <typename A, typename B>
const A*
get(const B& b)
{
    foreach(const OfficeArtFOPTEChoice& a, b.fopt) {
        const A *ptr = a.anon.get<A>();
        if (ptr) return ptr;
    }
    return 0;
}
/**
 * Retrieve an option from an OfficeArtSpContainer
 *
 * Look in all option containers in @p o for an option of type A.
 * @param o OfficeArtSpContainer instance which contains options.
 * @return pointer to the option of type A or 0 if there is none.
 */
template <typename A>
const A*
get(const OfficeArtSpContainer& o)
{
    const A* a = 0;
    if (o.shapePrimaryOptions) a = get<A>(*o.shapePrimaryOptions);
    if (!a && o.shapeSecondaryOptions1) a = get<A>(*o.shapeSecondaryOptions1);
    if (!a && o.shapeSecondaryOptions2) a = get<A>(*o.shapeSecondaryOptions2);
    if (!a && o.shapeTertiaryOptions1) a = get<A>(*o.shapeTertiaryOptions1);
    if (!a && o.shapeTertiaryOptions2) a = get<A>(*o.shapeTertiaryOptions2);
    return a;
}
/**
 * Retrieve an option from an OfficeArtDggContainer
 *
 * Look in all option containers in @p o for an option of type A.
 * @param o OfficeArtDggContainer instance which contains options.
 * @return pointer to the option of type A or 0 if there is none.
 */
template <typename A>
const A*
get(const OfficeArtDggContainer& o)
{
    const A* a = get<A>(o.drawingPrimaryOptions);
    if (!a && o.drawingTertiaryOptions) a = get<A>(*o.drawingTertiaryOptions);
    return a;
}
/**
 * Convert FixedPoint to a qreal
 */
qreal
toFloat(const FixedPoint& f)
{
    return f.integral + f.fractional / 65536.0;
}
/**
 * Return the bounding rectangle for this object.
 **/
QRect
getRect(const OfficeArtFSPGR &r)
{
    return QRect(r.xLeft, r.yTop, r.xRight - r.xLeft, r.yBottom - r.yTop);
}
/**
 * Return the bounding rectangle for this object.
 **/
QRect
getRect(const OfficeArtClientAnchor &a)
{
    if (a.rect1) {
        const SmallRectStruct &r = *a.rect1;
        return QRect(r.left, r.top, r.right - r.left, r.bottom - r.top);
    } else {
        const RectStruct &r = *a.rect2;
        return QRect(r.left, r.top, r.right - r.left, r.bottom - r.top);
    }
}
/**
 * Return the bounding rectangle for this object.
 **/
QRect
getRect(const OfficeArtSpContainer &o)
{
    if (o.childAnchor) {
        const OfficeArtChildAnchor& r = *o.childAnchor;
        return QRect(r.xLeft, r.yTop, r.xRight - r.xLeft, r.yBottom - r.yTop);
    } else if (o.clientAnchor) {
        return getRect(*o.clientAnchor);
    }
    return QRect(0, 0, 1, 1);
}

}

PptToOdp::Writer::Writer(KoXmlWriter& xmlWriter) : xOffset(0),
        yOffset(0),
        scaleX(25.4 / 576),
        scaleY(25.4 / 576),
        xml(xmlWriter)
{
}

PptToOdp::Writer
PptToOdp::Writer::transform(const QRectF& oldCoords, const QRectF &newCoords) const
{
    Writer w(xml);
    w.xOffset = xOffset + oldCoords.x() * scaleX;
    w.yOffset = yOffset + oldCoords.y() * scaleY;
    w.scaleX = scaleX * oldCoords.width() / newCoords.width();
    w.scaleY = scaleY * oldCoords.height() / newCoords.height();
    w.xOffset -= w.scaleX * newCoords.x();
    w.yOffset -= w.scaleY * newCoords.y();
    return w;
}
QString PptToOdp::Writer::vLength(qreal length)
{
    return mm.arg(length*scaleY);
}

QString PptToOdp::Writer::hLength(qreal length)
{
    return mm.arg(length*scaleX);
}

QString PptToOdp::Writer::vOffset(qreal offset)
{
    return mm.arg(yOffset + offset*scaleY);
}

QString PptToOdp::Writer::hOffset(qreal offset)
{
    return mm.arg(xOffset + offset*scaleX);
}

PptToOdp::PptToOdp() : p(0)
{
}

PptToOdp::~PptToOdp()
{
    delete p;
}

QMap<QByteArray, QString>
createPictures(POLE::Storage& storage, KoStore* store, KoXmlWriter* manifest)
{
    QMap<QByteArray, QString> fileNames;
    POLE::Stream* stream = new POLE::Stream(&storage, "/Pictures");
    while (!stream->eof() && !stream->fail()
            && stream->tell() < stream->size()) {

        PictureReference ref = savePicture(*stream, store);
        if (ref.name.length() == 0) break;
        manifest->addManifestEntry("Pictures/" + ref.name, ref.mimetype);
        fileNames[ref.uid] = ref.name;
    }
    storage.close();
    delete stream;
    return fileNames;
}
bool
PptToOdp::parse(POLE::Storage& storage)
{
    delete p;
    p = 0;
    ParsedPresentation* pp = new ParsedPresentation();
    if (!pp->parse(storage)) {
        delete pp;
        return false;
    }
    p = pp;
    return true;
}
KoFilter::ConversionStatus PptToOdp::convert(const QString& inputFile,
        const QString& to,
        KoStore::Backend storeType)
{
    // open inputFile
    POLE::Storage storage(inputFile.toLocal8Bit());
    if (!storage.open()) {
        qDebug() << "Cannot open " << inputFile;
        return KoFilter::StupidError;
    }
    if (!parse(storage)) {
        qDebug() << "Parsing and setup failed.";
        return KoFilter::StupidError;
    }
    // create output store
    KoStore* storeout = KoStore::createStore(to, KoStore::Write,
                        KoOdf::mimeType(KoOdf::Presentation), storeType);
    if (!storeout) {
        kWarning() << "Couldn't open the requested file.";
        return KoFilter::FileNotFound;
    }

    KoFilter::ConversionStatus status = doConversion(storage, storeout);
    delete storeout;
    return status;
}

KoFilter::ConversionStatus PptToOdp::convert(POLE::Storage& storage,
        KoStore* storeout)
{
    if (!parse(storage)) {
        qDebug() << "Parsing and setup failed.";
        return KoFilter::StupidError;
    }
    return doConversion(storage, storeout);
}

KoFilter::ConversionStatus PptToOdp::doConversion(POLE::Storage& storage,
        KoStore* storeout)
{
    KoOdfWriteStore odfWriter(storeout);
    KoXmlWriter* manifest = odfWriter.manifestWriter(
                                KoOdf::mimeType(KoOdf::Presentation));

    // store the images from the 'Pictures' stream
    storeout->disallowNameExpansion();
    storeout->enterDirectory("Pictures");
    pictureNames = createPictures(storage,
                                  storeout, manifest);
    storeout->leaveDirectory();

    KoGenStyles styles;
    createMainStyles(styles);

    // store document content
    if (!storeout->open("content.xml")) {
        kWarning() << "Couldn't open the file 'content.xml'.";
        delete p;
        p = 0;
        return KoFilter::CreationError;
    }
    storeout->write(createContent(styles));
    storeout->close();
    manifest->addManifestEntry("content.xml", "text/xml");

    // store document styles
    styles.saveOdfStylesDotXml(storeout, manifest);

    odfWriter.closeManifestWriter();

    delete p;
    p = 0;
    return KoFilter::OK;
}

void
addElement(KoGenStyle& style, const char* name,
           const QMap<const char*, QString>& m,
           const QMap<const char*, QString>& mtext)
{
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    KoXmlWriter elementWriter(&buffer);
    elementWriter.startElement(name);
    QMapIterator<const char*, QString> i(m);
    while (i.hasNext()) {
        i.next();
        elementWriter.addAttribute(i.key(), i.value());
    }
    if (mtext.size()) {
        elementWriter.startElement("style:text-properties");
        QMapIterator<const char*, QString> j(mtext);
        while (j.hasNext()) {
            j.next();
            elementWriter.addAttribute(j.key(), j.value());
        }
        elementWriter.endElement();
    }
    elementWriter.endElement();
    style.addChildElement(name,
                          QString::fromUtf8(buffer.buffer(), buffer.buffer().size()));
}
template <typename T>
void addFillBlips(const T& fopt, QSet<quint32>& fillBlips)
{
    foreach(const OfficeArtFOPTEChoice& f, fopt.fopt) {
        // complex blips are ignored for now
        const FillBlip* fb = f.anon.get<FillBlip>();
        if (fb && !fb->opid.fComplex && fb->fillBlip) {
            fillBlips.insert(fb->fillBlip);
        }
    }
}
void addFillBlips(const OfficeArtSpContainer& sp, QSet<quint32>& fillBlips)
{
    if (sp.shapePrimaryOptions)
        addFillBlips(*sp.shapePrimaryOptions, fillBlips);
    if (sp.shapeSecondaryOptions1)
        addFillBlips(*sp.shapeSecondaryOptions1, fillBlips);
    if (sp.shapeSecondaryOptions2)
        addFillBlips(*sp.shapeSecondaryOptions2, fillBlips);
    if (sp.shapeTertiaryOptions1)
        addFillBlips(*sp.shapeTertiaryOptions1, fillBlips);
    if (sp.shapeTertiaryOptions2)
        addFillBlips(*sp.shapeTertiaryOptions2, fillBlips);
}
void addFillBlips(const OfficeArtSpgrContainerFileBlock& spgr, QSet<quint32>& fillBlips);
void addFillBlips(const OfficeArtSpgrContainer& spgr, QSet<quint32>& fillBlips)
{
    foreach(const OfficeArtSpgrContainerFileBlock& o, spgr.rgfb) {
        addFillBlips(o, fillBlips);
    }
}
void addFillBlips(const OfficeArtDgContainer& dg, QSet<quint32>& fillBlips)
{
    addFillBlips(dg.groupShape, fillBlips);
    if (dg.shape)
        addFillBlips(*dg.shape, fillBlips);
    foreach(const OfficeArtSpgrContainerFileBlock& o, dg.deletedShapes) {
        addFillBlips(o, fillBlips);
    }
}
void addFillBlips(const OfficeArtSpgrContainerFileBlock& spgr, QSet<quint32>& fillBlips)
{
    if (spgr.anon.is<OfficeArtSpContainer>())
        addFillBlips(*spgr.anon.get<OfficeArtSpContainer>(), fillBlips);
    if (spgr.anon.is<OfficeArtSpgrContainer>())
        addFillBlips(*spgr.anon.get<OfficeArtSpgrContainer>(), fillBlips);
}
void PptToOdp::createFillImages(KoGenStyles& styles)
{
    // loop over all objects to find all "fillPib" numbers
    QSet<quint32> fillBlips;
    // get blips from default options
    const DrawingGroupContainer& dg = p->documentContainer->drawingGroup;
    addFillBlips(dg.OfficeArtDgg.drawingPrimaryOptions, fillBlips);
    if (dg.OfficeArtDgg.drawingTertiaryOptions)
        addFillBlips(*dg.OfficeArtDgg.drawingTertiaryOptions, fillBlips);
    // get blips from masters
    foreach(const MasterOrSlideContainer* master, p->masters) {
        if (master->anon.is<SlideContainer>())
            addFillBlips(master->anon.get<SlideContainer>()->drawing.OfficeArtDg, fillBlips);
        if (master->anon.is<MainMasterContainer>())
            addFillBlips(master->anon.get<MainMasterContainer>()->drawing.OfficeArtDg, fillBlips);
    }
    // get blips from slides
    foreach(const SlideContainer* slide, p->slides) {
        addFillBlips(slide->drawing.OfficeArtDg, fillBlips);
    }
    // get blips from notes
    foreach(const NotesContainer* notes, p->notes) {
        addFillBlips(notes->drawing.OfficeArtDg, fillBlips);
    }
    foreach(quint32 pib, fillBlips) {
        KoGenStyle fillImage(KoGenStyle::StyleFillImage);
        fillImage.addAttribute("xlink:href", getPicturePath(pib));
        styles.lookup(fillImage, "fillImage" + QString::number(pib),
                      KoGenStyles::DontForceNumbering);
    }
}

void PptToOdp::createMainStyles(KoGenStyles& styles)
{
    int x = 0;
    int y = 0;

    // x and y are given in master units (1/576 inches)
    QString pageWidth = QString("%1in").arg(
                            p->documentContainer->documentAtom.slideSize.x / 576);
    QString pageHeight = QString("%1in").arg(
                             p->documentContainer->documentAtom.slideSize.y / 576);

    //KoGenStyle defaultStyle(KoGenStyle::StyleUser, "graphic");
    KoGenStyle defaultStyle(KoGenStyle::StyleGraphicAuto, "graphic");
    //defaultStyle.setDefaultStyle(true);
    defaultStyle.setAutoStyleInStylesDotXml(true);
    const OfficeArtDggContainer& drawingGroup
    = p->documentContainer->drawingGroup.OfficeArtDgg;
    processGraphicStyle(defaultStyle, drawingGroup);
    // add the defaults that were not set yet
    if (!get<LineWidth>(drawingGroup)) {
        defaultStyle.addProperty("svg:stroke-width",
                                 QString("%1pt").arg(0x2535 / 12700.f),
                                 KoGenStyle::GraphicType);
    }
    styles.lookup(defaultStyle, "pptDefaults", KoGenStyles::DontForceNumbering);

    KoGenStyle marker(KoGenStyle::StyleMarker);
    marker.addAttribute("draw:display-name", "msArrowEnd 5");
    marker.addAttribute("svg:viewBox", "0 0 210 210");
    marker.addAttribute("svg:d", "m105 0 105 210h-210z");
    styles.lookup(marker, "msArrowEnd_20_5");

    // add all the fill image definitions
    createFillImages(styles);

    KoGenStyle pl(KoGenStyle::StylePageLayout);
    pl.setAutoStyleInStylesDotXml(true);
    pl.addAttribute("style:page-usage", "all");
    pl.addProperty("fo:margin-bottom", "0pt");
    pl.addProperty("fo:margin-left", "0pt");
    pl.addProperty("fo:margin-right", "0pt");
    pl.addProperty("fo:margin-top", "0pt");
    pl.addProperty("fo:page-height", pageHeight);
    pl.addProperty("fo:page-width", pageWidth);
    pl.addProperty("style:print-orientation", "landscape");
    styles.lookup(pl, "pm");

    KoGenStyle dp(KoGenStyle::StyleDrawingPage, "drawing-page");
    dp.setAutoStyleInStylesDotXml(true);
    dp.addProperty("draw:background-size", "border");
    dp.addProperty("draw:fill", "solid");
    dp.addProperty("draw:fill-color", "#ffffff");
    styles.lookup(dp, "dp");

    KoGenStyle pa(KoGenStyle::StyleAuto, "paragraph");
    pa.setAutoStyleInStylesDotXml(true);
    pa.addProperty("fo:margin-left", "0cm");
    pa.addProperty("fo:margin-right", "0cm");
    pa.addProperty("fo:text-indent", "0cm");
    pa.addProperty("fo:font-size", "14pt", KoGenStyle::TextType);
    pa.addProperty("style:font-size-asian", "14pt", KoGenStyle::TextType);
    pa.addProperty("style:font-size-complex", "14pt", KoGenStyle::TextType);
    styles.lookup(pa, "P");

    KoGenStyle l(KoGenStyle::StyleListAuto);
    l.setAutoStyleInStylesDotXml(true);
    QMap<const char*, QString> lmap;
    lmap["text:level"] = "1";
    const char bullet[4] = {0xe2, 0x97, 0x8f, 0};
    lmap["text:bullet-char"] = QString::fromUtf8(bullet);//  "●";
    QMap<const char*, QString> ltextmap;
    ltextmap["fo:font-family"] = "StarSymbol";
    ltextmap["style:font-pitch"] = "variable";
    ltextmap["fo:color"] = "#000000";
    ltextmap["fo:font-size"] = "45%";
    addElement(l, "text:list-level-style-bullet", lmap, ltextmap);
    styles.lookup(l, "L");

    // Creating dateTime class object
    if (getSlideHF()) {
        int dateTimeFomatId = getSlideHF()->hfAtom.formatId;
        bool hasTodayDate = getSlideHF()->hfAtom.fHasTodayDate;
        bool hasUserDate = getSlideHF()->hfAtom.fHasUserDate;
        dateTime = DateTimeFormat(dateTimeFomatId);
        dateTime.addDateTimeAutoStyles(styles, hasTodayDate, hasUserDate);
    }

    KoGenStyle text(KoGenStyle::StyleTextAuto, "text");
    text.setAutoStyleInStylesDotXml(true);
    text.addProperty("fo:font-size", "12pt");
    text.addProperty("fo:language", "en");
    text.addProperty("fo:country", "US");
    text.addProperty("style:font-size-asian", "12pt");
    text.addProperty("style:font-size-complex", "12pt");

    KoGenStyle Mpr(KoGenStyle::StylePresentationAuto, "presentation");
    Mpr.setAutoStyleInStylesDotXml(true);
    Mpr.addProperty("draw:stroke", "none");
    Mpr.addProperty("draw:fill", "none");
    Mpr.addProperty("draw:fill-color", "#bbe0e3");
    Mpr.addProperty("draw:textarea-horizontal-align", "justify");
    Mpr.addProperty("draw:textarea-vertical-align", "top");
    Mpr.addProperty("fo:wrap-option", "wrap");
    styles.lookup(Mpr, "Mpr");

    KoGenStyle s(KoGenStyle::StyleMaster);
    s.addAttribute("style:page-layout-name", styles.lookup(pl));
    s.addAttribute("draw:style-name", styles.lookup(dp));

    x = p->documentContainer->documentAtom.slideSize.x - 50;
    y = p->documentContainer->documentAtom.slideSize.y - 50;

    addFrame(s, "page-number", "20pt", "20pt",

             QString("%1pt").arg(x), QString("%1pt").arg(y),
             styles.lookup(pa), styles.lookup(text));
    addFrame(s, "date-time", "200pt", "20pt", "20pt",
             QString("%1pt").arg(y), styles.lookup(pa), styles.lookup(text));
    styles.lookup(s, "Standard", KoGenStyles::DontForceNumbering);
}

void PptToOdp::addFrame(KoGenStyle& style, const char* presentationClass,
                        QString width, QString height,
                        QString x, QString y, QString pStyle, QString tStyle)
{
    QBuffer buffer;
    //int  headerFooterAtomFlags = 0;
    //Slide *master = presentation->masterSlide();
    // if (master)
    //    headerFooterAtomFlags = master->headerFooterFlags();
    //QString datetime;

    buffer.open(QIODevice::WriteOnly);
    KoXmlWriter xmlWriter(&buffer);

    xmlWriter.startElement("draw:frame");
    xmlWriter.addAttribute("presentation:style-name", "Mpr1");
    xmlWriter.addAttribute("draw:layer", "layout");
    xmlWriter.addAttribute("svg:width", width);
    xmlWriter.addAttribute("svg:height", height);
    xmlWriter.addAttribute("svg:x", x);
    xmlWriter.addAttribute("svg:y", y);
    xmlWriter.addAttribute("presentation:class", presentationClass);
    xmlWriter.startElement("draw:text-box");
    xmlWriter.startElement("text:p");
    xmlWriter.addAttribute("text:style-name", pStyle);

    if (strcmp(presentationClass, "page-number") == 0) {
        xmlWriter.startElement("text:span");
        xmlWriter.addAttribute("text:style-name", tStyle);
        xmlWriter.startElement("text:page-number");
        xmlWriter.addTextNode("<number>");
        xmlWriter.endElement();//text:page-number
        xmlWriter.endElement(); // text:span
    }

    if (strcmp(presentationClass, "date-time") == 0) {
        //Same DateTime object so no need to pass style name for date time
        if (getSlideHF()) {
            if (getSlideHF()->hfAtom.fHasTodayDate) {
                dateTime.addMasterDateTimeSection(xmlWriter, tStyle);
            } else if (getSlideHF()->hfAtom.fHasUserDate) {
                //Future FixedDate format
            }
        }
    }

    xmlWriter.endElement(); // text:p

    xmlWriter.endElement(); // draw:text-box
    xmlWriter.endElement(); // draw:frame
    style.addChildElement("draw:frame",
                          QString::fromUtf8(buffer.buffer(), buffer.buffer().size()));
}

QByteArray PptToOdp::createContent(KoGenStyles& styles)
{
    QByteArray contentData;
    QBuffer contentBuffer(&contentData);

    contentBuffer.open(QIODevice::WriteOnly);
    KoXmlWriter contentWriter(&contentBuffer);

    contentWriter.startDocument("office:document-content");
    contentWriter.startElement("office:document-content");
    contentWriter.addAttribute("xmlns:fo", "urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0");
    contentWriter.addAttribute("xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0");
    contentWriter.addAttribute("xmlns:style", "urn:oasis:names:tc:opendocument:xmlns:style:1.0");
    contentWriter.addAttribute("xmlns:text", "urn:oasis:names:tc:opendocument:xmlns:text:1.0");
    contentWriter.addAttribute("xmlns:draw", "urn:oasis:names:tc:opendocument:xmlns:drawing:1.0");
    contentWriter.addAttribute("xmlns:presentation", "urn:oasis:names:tc:opendocument:xmlns:presentation:1.0");
    contentWriter.addAttribute("xmlns:svg", "urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0");
    contentWriter.addAttribute("xmlns:xlink", "http://www.w3.org/1999/xlink");
    contentWriter.addAttribute("office:version", "1.0");

    // office:automatic-styles
    processDocStyles(styles);

    for (int c = 0; c < p->slides.size(); c++) {
        processSlideForStyle(c, styles);
    }

    styles.saveOdfAutomaticStyles(&contentWriter, false);

    // office:body

    contentWriter.startElement("office:body");
    contentWriter.startElement("office:presentation");

    if (getSlideHF()) {
        if (getSlideHF()->hfAtom.fHasTodayDate) {
            contentWriter.startElement("presentation:date-time-decl");
            contentWriter.addAttribute("presentation:name", "dtd1");
            contentWriter.addAttribute("presentation:source", "current-date");
            //contentWriter.addAttribute("style:data-style-name", "Dt1");
            contentWriter.endElement();  // presentation:date-time-decl
        } else if (getSlideHF()->hfAtom.fHasUserDate) {
            contentWriter.startElement("presentation:date-time-decl");
            contentWriter.addAttribute("presentation:name", "dtd1");
            contentWriter.addAttribute("presentation:source", "fixed");
            //Future - Add Fixed date data here
            contentWriter.endElement();  //presentation:date-time-decl
        }
    }

    for (int c = 0; c < p->slides.size(); c++) {
        processSlideForBody(c, contentWriter);
    }

    contentWriter.endElement();  // office:presentation

    contentWriter.endElement();  // office:body

    contentWriter.endElement();  // office:document-content
    contentWriter.endDocument();
    return contentData;
}

void PptToOdp::processDocStyles(KoGenStyles &styles)
{
    KoGenStyle dp(KoGenStyle::StyleDrawingPage, "drawing-page");
    dp.addProperty("presentation:background-objects-visible", "true");

    if (getSlideHF() && getSlideHF()->hfAtom.fHasSlideNumber)
        dp.addProperty("presentation:display-page-number", "true");
    else
        dp.addProperty("presentation:display-page-number", "false");

    if (getSlideHF() && getSlideHF()->hfAtom.fHasDate)
        dp.addProperty("presentation:display-date-time" , "true");
    else
        dp.addProperty("presentation:display-date-time" , "false");

    styles.lookup(dp, "dp");

    masterStyleName = styles.lookup(dp);
}


void PptToOdp::processEllipse(const OfficeArtSpContainer& o, Writer& out)
{
    const QRect rect = getRect(o);
    out.xml.startElement("draw:ellipse");
    out.xml.addAttribute("draw:style-name", getGraphicStyleName(o));
    out.xml.addAttribute("svg:width", out.hLength(rect.width()));
    out.xml.addAttribute("svg:height", out.vLength(rect.height()));
    out.xml.addAttribute("svg:x", out.hOffset(rect.x()));
    out.xml.addAttribute("svg:y", out.vOffset(rect.y()));
    out.xml.addAttribute("draw:layer", "layout");
    out.xml.endElement(); // draw:ellipse
}

void PptToOdp::processRectangle(const OfficeArtSpContainer& o, Writer& out)
{
    const QRect rect = getRect(o);
    static const QString rotate("rotate (%1) translate (%2mm %3mm)");
    out.xml.startElement("draw:rect");
    out.xml.addAttribute("draw:style-name", getGraphicStyleName(o));
    out.xml.addAttribute("svg:width", out.hLength(rect.width()));
    out.xml.addAttribute("svg:height", out.vLength(rect.height()));
    const Rotation* rotation = get<Rotation>(o);
    if (rotation) {
        qreal rotAngle = toFloat(rotation->rotation);
        qreal xMid = (rect.left() + 0.5 * rect.width());
        qreal yMid = (rect.top() + 0.5 * rect.height());
        qreal xVec = rect.left() - xMid;
        qreal yVec = yMid - rect.top();

        qreal xNew = xVec * cos(rotAngle) - yVec * sin(rotAngle);
        qreal yNew = xVec * sin(rotAngle) + yVec * cos(rotAngle);
        QString rot = rotate.arg(rotAngle).arg(xNew + xMid).arg(yMid - yNew);
        out.xml.addAttribute("draw:transform", rot);
    } else {
        out.xml.addAttribute("svg:x", out.hOffset(rect.x()));
        out.xml.addAttribute("svg:y", out.vOffset(rect.y()));
    }
    out.xml.addAttribute("draw:layer", "layout");
    out.xml.endElement(); // draw:rect
}

void PptToOdp::processRoundRectangle(const OfficeArtSpContainer& o, Writer& out)
{
    const QRect rect = getRect(o);
    out.xml.startElement("draw:custom-shape");
    out.xml.addAttribute("draw:style-name", getGraphicStyleName(o));

    const Rotation* rotation = get<Rotation>(o);
    if (rotation) {
        qreal rotAngle = toFloat(rotation->rotation);
        if (rotAngle > 0.785399) { // > 45 deg
            out.xml.addAttribute("svg:width", out.vLength(rect.height()));
            out.xml.addAttribute("svg:height", out.hLength(rect.width()));
            double xMid = (rect.x() - 0.5 * rect.height());
            double yMid = (rect.y() + 0.5 * rect.width());
            double xVec = rect.x() - xMid;
            double yVec =  rect.y() - yMid;

            double xNew = xVec * cos(rotAngle) - yVec * sin(rotAngle);
            double yNew = xVec * sin(rotAngle) + yVec * cos(rotAngle);
            QString rot = QString("rotate (%1) translate (%2mm %3mm)").arg(rotAngle).arg(xNew + xMid).arg(yMid + yNew);
            out.xml.addAttribute("draw:transform", rot);
        } else {
            out.xml.addAttribute("svg:width", out.hLength(rect.width()));
            out.xml.addAttribute("svg:height", out.vLength(rect.height()));
            double xMid = (rect.x() + 0.5 * rect.width());
            double yMid = (rect.y() + 0.5 * rect.height());
            double xVec = rect.x() - xMid;
            double yVec = yMid - rect.y();

            double xNew = xVec * cos(rotAngle) - yVec * sin(rotAngle);
            double yNew = xVec * sin(rotAngle) + yVec * cos(rotAngle);
            QString rot = QString("rotate (%1) translate (%2mm %3mm)").arg(rotAngle).arg(xNew + xMid).arg(yMid - yNew);
            out.xml.addAttribute("draw:transform", rot);
        }


    } else {
        out.xml.addAttribute("svg:width", out.hLength(rect.width()));
        out.xml.addAttribute("svg:height", out.vLength(rect.height()));
        out.xml.addAttribute("svg:x", out.hOffset(rect.x()));
        out.xml.addAttribute("svg:y", out.vOffset(rect.y()));
    }
// out.xml.addAttribute( "svg:x", out.hOffset(rect.x()) );
// out.xml.addAttribute( "svg:y", out.vOffset(rect.y()) );

    out.xml.addAttribute("draw:layer", "layout");
    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:type", "round-rectangle");
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "$0 /3");
    out.xml.addAttribute("draw:name", "f0");
    out.xml.endElement(); // draw:equation
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "right-?f0 ");
    out.xml.addAttribute("draw:name", "f1");
    out.xml.endElement(); // draw:equation
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "bottom-?f0 ");
    out.xml.addAttribute("draw:name", "f2");
    out.xml.endElement(); // draw:equation
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "left+?f0 ");
    out.xml.addAttribute("draw:name", "f3");
    out.xml.endElement(); // draw:equation
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "top+?f0 ");
    out.xml.addAttribute("draw:name", "f4");
    out.xml.endElement(); // draw:equation
    out.xml.endElement(); // draw:enhanced-geometry
    out.xml.endElement(); // draw:custom-shape
}

void PptToOdp::processDiamond(const OfficeArtSpContainer& o, Writer& out)
{
    const QRect rect = getRect(o);
    out.xml.startElement("draw:custom-shape");
    out.xml.addAttribute("draw:style-name", getGraphicStyleName(o));
    out.xml.addAttribute("svg:width", out.hLength(rect.width()));
    out.xml.addAttribute("svg:height", out.vLength(rect.height()));
    out.xml.addAttribute("svg:x", out.hOffset(rect.x()));
    out.xml.addAttribute("svg:y", out.vOffset(rect.y()));
    out.xml.startElement("draw:glue-point");
    out.xml.addAttribute("svg:x", 5);
    out.xml.addAttribute("svg:y", 0);
    out.xml.endElement();
    out.xml.startElement("draw:glue-point");
    out.xml.addAttribute("svg:x", 0);
    out.xml.addAttribute("svg:y", 5);
    out.xml.endElement();
    out.xml.startElement("draw:glue-point");
    out.xml.addAttribute("svg:x", 5);
    out.xml.addAttribute("svg:y", 10);
    out.xml.endElement();
    out.xml.startElement("draw:glue-point");
    out.xml.addAttribute("svg:x", 10);
    out.xml.addAttribute("svg:y", 5);
    out.xml.endElement();
    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:type", "diamond");
    out.xml.endElement();
    out.xml.addAttribute("draw:layer", "layout");
    out.xml.endElement();
}

void PptToOdp::processTriangle(const OfficeArtSpContainer& o, Writer& out)
{
    const QRect rect = getRect(o);
    /* draw IsocelesTriangle or RightTriangle */
    out.xml.startElement("draw:custom-shape");
    out.xml.addAttribute("draw:style-name", getGraphicStyleName(o));
    out.xml.addAttribute("svg:width", out.hLength(rect.width()));
    out.xml.addAttribute("svg:height", out.vLength(rect.height()));
    out.xml.addAttribute("svg:x", out.hOffset(rect.x()));
    out.xml.addAttribute("svg:y", out.vOffset(rect.y()));
    out.xml.addAttribute("draw:layer", "layout");
    out.xml.startElement("draw:glue-point");
    out.xml.addAttribute("svg:x", 5);
    out.xml.addAttribute("svg:y", 0);
    out.xml.endElement();
    out.xml.startElement("draw:glue-point");
    out.xml.addAttribute("svg:x", 2.5);
    out.xml.addAttribute("svg:y", 5);
    out.xml.endElement();
    out.xml.startElement("draw:glue-point");
    out.xml.addAttribute("svg:x", 0);
    out.xml.addAttribute("svg:y", 10);
    out.xml.endElement();
    out.xml.startElement("draw:glue-point");
    out.xml.addAttribute("svg:x", 5);
    out.xml.addAttribute("svg:y", 10);
    out.xml.endElement();
    out.xml.startElement("draw:glue-point");
    out.xml.addAttribute("svg:x", 10);
    out.xml.addAttribute("svg:y", 10);
    out.xml.endElement();
    out.xml.startElement("draw:glue-point");
    out.xml.addAttribute("svg:x", 7.5);
    out.xml.addAttribute("svg:y", 5);
    out.xml.endElement();

    out.xml.startElement("draw:enhanced-geometry");

    if (o.shapeProp.fFlipV) {
        out.xml.addAttribute("draw:mirror-vertical", "true");
    }
    if (o.shapeProp.fFlipH) {
        out.xml.addAttribute("draw:mirror-horizontal", "true");
    }
    const Rotation* rotation = get<Rotation>(o);
    if (rotation) { // draw:transform="rotate (1.5707963267946) translate (6.985cm 14.181cm)"
        double rotAngle = toFloat(rotation->rotation);
        double xMid = (rect.x() + 0.5 * rect.width());
        double yMid = (rect.y() + 0.5 * rect.height());
        QString rot = QString("rotate (%1) translate (%2cm %3cm)").arg(rotAngle).arg(xMid).arg(yMid);
        out.xml.addAttribute("draw:transform", rot);
    }
    if (o.shapeProp.rh.recInstance == msosptRightTriangle) {
        out.xml.addAttribute("draw:type", "right-triangle");
    } else if (o.shapeProp.rh.recInstance == msosptIsoscelesTriangle) {
        out.xml.addAttribute("draw:type", "isosceles-triangle");
        out.xml.startElement("draw:equation");
        out.xml.addAttribute("draw:formula", "$0 ");
        out.xml.addAttribute("draw:name", "f0");
        out.xml.endElement();
        out.xml.startElement("draw:equation");
        out.xml.addAttribute("draw:formula", "$0 /2");
        out.xml.addAttribute("draw:name", "f1");
        out.xml.endElement();
        out.xml.startElement("draw:equation");
        out.xml.addAttribute("draw:formula", "?f1 +10800");
        out.xml.addAttribute("draw:name", "f2");
        out.xml.endElement();
        out.xml.startElement("draw:equation");
        out.xml.addAttribute("draw:formula", "$0 *2/3");
        out.xml.addAttribute("draw:name", "f3");
        out.xml.endElement();
        out.xml.startElement("draw:equation");
        out.xml.addAttribute("draw:formula", "?f3 +7200");
        out.xml.addAttribute("draw:name", "f4");
        out.xml.endElement();
        out.xml.startElement("draw:equation");
        out.xml.addAttribute("draw:formula", "21600-?f0 ");
        out.xml.addAttribute("draw:name", "f5");
        out.xml.endElement();
        out.xml.startElement("draw:equation");
        out.xml.addAttribute("draw:formula", "?f5 /2");
        out.xml.addAttribute("draw:name", "f6");
        out.xml.endElement();
        out.xml.startElement("draw:equation");
        out.xml.addAttribute("draw:formula", "21600-?f6 ");
        out.xml.addAttribute("draw:name", "f7");
        out.xml.endElement();
        out.xml.startElement("draw:handle");
        out.xml.addAttribute("draw:handle-range-x-maximum", 21600);
        out.xml.addAttribute("draw:handle-range-x-minimum", 0);
        out.xml.addAttribute("draw:handle-position", "$0 top");
        out.xml.endElement();
    }

    out.xml.endElement();    // enhanced-geometry
    out.xml.endElement(); // custom-shape
}

void PptToOdp::processTrapezoid(const OfficeArtSpContainer& o, Writer& out)
{
    const QRect rect = getRect(o);
    out.xml.startElement("draw:custom-shape");
    out.xml.addAttribute("draw:style-name", getGraphicStyleName(o));
    out.xml.addAttribute("svg:width", out.hLength(rect.width()));
    out.xml.addAttribute("svg:height", out.vLength(rect.height()));
    out.xml.addAttribute("svg:x", out.hOffset(rect.x()));
    out.xml.addAttribute("svg:y", out.vOffset(rect.y()));
    out.xml.addAttribute("draw:layer", "layout");

    out.xml.startElement("draw:glue-point");
    out.xml.addAttribute("svg:x", 5);
    out.xml.addAttribute("svg:y", 0);
    out.xml.endElement();
    out.xml.startElement("draw:glue-point");
    out.xml.addAttribute("svg:x", 2.5);
    out.xml.addAttribute("svg:y", 5);
    out.xml.endElement();
    out.xml.startElement("draw:glue-point");
    out.xml.addAttribute("svg:x", 0);
    out.xml.addAttribute("svg:y", 10);
    out.xml.endElement();
    out.xml.startElement("draw:glue-point");
    out.xml.addAttribute("svg:x", 5);
    out.xml.addAttribute("svg:y", 10);
    out.xml.endElement();
    out.xml.startElement("draw:enhanced-geometry");
    if (o.shapeProp.fFlipV) {
        out.xml.addAttribute("draw:mirror-vertical", "true");
    }
    if (o.shapeProp.fFlipH) {
        out.xml.addAttribute("draw:mirror-horizontal", "true");
    }
    out.xml.addAttribute("draw:type", "trapezoid");
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "21600-$0 ");
    out.xml.addAttribute("draw:name", "f0");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "$0");
    out.xml.addAttribute("draw:name", "f1");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "$0 *10/18");
    out.xml.addAttribute("draw:name", "f2");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "?f2 +1750");
    out.xml.addAttribute("draw:name", "f3");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "21600-?f3");
    out.xml.addAttribute("draw:name", "f4");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "$0 /2");
    out.xml.addAttribute("draw:name", "f5");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "21600-?f5");
    out.xml.addAttribute("draw:name", "f6");
    out.xml.endElement();
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-range-x-maximum", 10800);
    out.xml.addAttribute("draw:handle-range-x-minimum", 0);
    out.xml.addAttribute("draw:handle-position", "$0 bottom");
    out.xml.endElement();
    out.xml.endElement(); // enhanced-geometry
    out.xml.endElement(); // custom-shape
}

void PptToOdp::processParallelogram(const OfficeArtSpContainer& o, Writer& out)
{
    const QRect rect = getRect(o);
    out.xml.startElement("draw:custom-shape");
    out.xml.addAttribute("draw:style-name", getGraphicStyleName(o));
    out.xml.addAttribute("svg:width", out.hLength(rect.width()));
    out.xml.addAttribute("svg:height", out.vLength(rect.height()));
    out.xml.addAttribute("svg:x", out.hOffset(rect.x()));
    out.xml.addAttribute("svg:y", out.vOffset(rect.y()));
    out.xml.addAttribute("draw:layer", "layout");
    out.xml.startElement("draw:glue-point");
    out.xml.addAttribute("svg:x", 6.25);
    out.xml.addAttribute("svg:y", 0);
    out.xml.endElement();
    out.xml.startElement("draw:glue-point");
    out.xml.addAttribute("svg:x", 4.5);
    out.xml.addAttribute("svg:y", 0);
    out.xml.endElement();
    out.xml.startElement("draw:glue-point");
    out.xml.addAttribute("svg:x", 8.75);
    out.xml.addAttribute("svg:y", 5);
    out.xml.endElement();
    out.xml.startElement("draw:glue-point");
    out.xml.addAttribute("svg:x", 3.75);
    out.xml.addAttribute("svg:y", 10);
    out.xml.endElement();
    out.xml.startElement("draw:glue-point");
    out.xml.addAttribute("svg:x", 5);
    out.xml.addAttribute("svg:y", 10);
    out.xml.endElement();
    out.xml.startElement("draw:glue-point");
    out.xml.addAttribute("svg:x", 1.25);
    out.xml.addAttribute("svg:y", 5);
    out.xml.endElement();
    out.xml.startElement("draw:enhanced-geometry");
    if (o.shapeProp.fFlipV) {
        out.xml.addAttribute("draw:mirror-vertical", "true");
    }
    if (o.shapeProp.fFlipH) {
        out.xml.addAttribute("draw:mirror-horizontal", "true");
    }
    out.xml.addAttribute("draw:type", "parallelogram");
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "$0 ");
    out.xml.addAttribute("draw:name", "f0");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "21600-$0");
    out.xml.addAttribute("draw:name", "f1");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "$0 *10/24");
    out.xml.addAttribute("draw:name", "f2");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "?f2 +1750");
    out.xml.addAttribute("draw:name", "f3");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "21600-?f3");
    out.xml.addAttribute("draw:name", "f4");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "?f0 /2");
    out.xml.addAttribute("draw:name", "f5");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "10800+?f5");
    out.xml.addAttribute("draw:name", "f6");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "?f0-10800 ");
    out.xml.addAttribute("draw:name", "f7");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "if(?f7,?f12,0");
    out.xml.addAttribute("draw:name", "f8");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "10800-?f5");
    out.xml.addAttribute("draw:name", "f9");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "if(?f7, ?f12, 21600");
    out.xml.addAttribute("draw:name", "f10");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "21600-?f5");
    out.xml.addAttribute("draw:name", "f11");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "21600*10800/?f0");
    out.xml.addAttribute("draw:name", "f12");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "21600-?f12");
    out.xml.addAttribute("draw:name", "f13");
    out.xml.endElement();
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-range-x-maximum", 21600);
    out.xml.addAttribute("draw:handle-range-x-minimum", 0);
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.endElement();
    out.xml.endElement(); // enhanced-geometry
    out.xml.endElement(); // custom-shape
}

void PptToOdp::processHexagon(const OfficeArtSpContainer& o, Writer& out)
{
    const QRect rect = getRect(o);
    out.xml.startElement("draw:custom-shape");
    out.xml.addAttribute("draw:style-name", getGraphicStyleName(o));
    out.xml.addAttribute("svg:width", out.hLength(rect.width()));
    out.xml.addAttribute("svg:height", out.vLength(rect.height()));
    out.xml.addAttribute("svg:x", out.hOffset(rect.x()));
    out.xml.addAttribute("svg:y", out.vOffset(rect.y()));
    out.xml.addAttribute("draw:layer", "layout");
    out.xml.startElement("draw:glue-point");
    out.xml.addAttribute("svg:x", 5);
    out.xml.addAttribute("svg:y", 0);
    out.xml.endElement();
    out.xml.startElement("draw:glue-point");
    out.xml.addAttribute("svg:x", 0);
    out.xml.addAttribute("svg:y", 5);
    out.xml.endElement();
    out.xml.startElement("draw:glue-point");
    out.xml.addAttribute("svg:x", 5);
    out.xml.addAttribute("svg:y", 10);
    out.xml.endElement();
    out.xml.startElement("draw:glue-point");
    out.xml.addAttribute("svg:x", 10);
    out.xml.addAttribute("svg:y", 5);
    out.xml.endElement();
    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:type", "hexagon");
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "$0 ");
    out.xml.addAttribute("draw:name", "f0");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "21600-$0");
    out.xml.addAttribute("draw:name", "f1");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "$0 *100/234");
    out.xml.addAttribute("draw:name", "f2");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "?f2 +1700");
    out.xml.addAttribute("draw:name", "f3");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "21600-?f3");
    out.xml.addAttribute("draw:name", "f4");
    out.xml.endElement();
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-range-x-maximum", 10800);
    out.xml.addAttribute("draw:handle-range-x-minimum", 0);
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.endElement();
    out.xml.endElement(); // enhanced-geometry
    out.xml.endElement(); // custom-shape
}

void PptToOdp::processOctagon(const OfficeArtSpContainer& o, Writer& out)
{
    const QRect rect = getRect(o);
    out.xml.startElement("draw:custom-shape");
    out.xml.addAttribute("draw:style-name", getGraphicStyleName(o));
    out.xml.addAttribute("svg:width", out.hLength(rect.width()));
    out.xml.addAttribute("svg:height", out.vLength(rect.height()));
    out.xml.addAttribute("svg:x", out.hOffset(rect.x()));
    out.xml.addAttribute("svg:y", out.vOffset(rect.y()));
    out.xml.addAttribute("draw:layer", "layout");
    out.xml.startElement("draw:glue-point");
    out.xml.addAttribute("svg:x", 5);
    out.xml.addAttribute("svg:y", 0);
    out.xml.endElement();
    out.xml.startElement("draw:glue-point");
    out.xml.addAttribute("svg:x", 0);
    out.xml.addAttribute("svg:y", 4.782);
    out.xml.endElement();
    out.xml.startElement("draw:glue-point");
    out.xml.addAttribute("svg:x", 5);
    out.xml.addAttribute("svg:y", 10);
    out.xml.endElement();
    out.xml.startElement("draw:glue-point");
    out.xml.addAttribute("svg:x", 10);
    out.xml.addAttribute("svg:y", 4.782);
    out.xml.endElement();
    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:type", "octagon");
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "left+$0 ");
    out.xml.addAttribute("draw:name", "f0");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "top+$0 ");
    out.xml.addAttribute("draw:name", "f1");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "right-$0 ");
    out.xml.addAttribute("draw:name", "f2");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "bottom-$0 ");
    out.xml.addAttribute("draw:name", "f3");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "$0 /2");
    out.xml.addAttribute("draw:name", "f4");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "left+?f4 ");
    out.xml.addAttribute("draw:name", "f5");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "top+?f4 ");
    out.xml.addAttribute("draw:name", "f6");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "right-?f4 ");
    out.xml.addAttribute("draw:name", "f7");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "bottom-?f4 ");
    out.xml.addAttribute("draw:name", "f8");
    out.xml.endElement();
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-range-x-maximum", 10800);
    out.xml.addAttribute("draw:handle-range-x-minimum", 0);
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.endElement();
    out.xml.endElement(); // enhanced-geometry
    out.xml.endElement(); // custom-shape
}

void PptToOdp::processArrow(const OfficeArtSpContainer& o, Writer& out)
{
    const QRect rect = getRect(o);
    out.xml.startElement("draw:custom-shape");
    out.xml.addAttribute("draw:style-name", getGraphicStyleName(o));
    out.xml.addAttribute("svg:width", out.hLength(rect.width()));
    out.xml.addAttribute("svg:height", out.vLength(rect.height()));
    out.xml.addAttribute("svg:x", out.hOffset(rect.x()));
    out.xml.addAttribute("svg:y", out.vOffset(rect.y()));
    out.xml.addAttribute("draw:layer", "layout");
    out.xml.startElement("draw:enhanced-geometry");

    if (o.shapeProp.rh.recInstance == msosptLeftArrow) {
        if (o.shapeProp.fFlipH)
            out.xml.addAttribute("draw:type", "right-arrow");
        else
            out.xml.addAttribute("draw:type", "left-arrow");
    } else if (o.shapeProp.rh.recInstance == msosptUpArrow)
        out.xml.addAttribute("draw:type", "up-arrow");
    else if (o.shapeProp.rh.recInstance == msosptDownArrow)
        out.xml.addAttribute("draw:type", "down-arrow");
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "$1");
    out.xml.addAttribute("draw:name", "f0");
    out.xml.endElement(); // draw:equation
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "$0");
    out.xml.addAttribute("draw:name", "f1");
    out.xml.endElement(); // draw:equation
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "21600-$1");
    out.xml.addAttribute("draw:name", "f2");
    out.xml.endElement(); // draw:equation
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "21600-?f1");
    out.xml.addAttribute("draw:name", "f3");
    out.xml.endElement(); // draw:equation
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "?f3 *?f0 /10800");
    out.xml.addAttribute("draw:name", "f4");
    out.xml.endElement(); // draw:equation
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "?f1 +?f4 ");
    out.xml.addAttribute("draw:name", "f5");
    out.xml.endElement(); // draw:equation
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "?f1 *?f0 /10800");
    out.xml.addAttribute("draw:name", "f6");
    out.xml.endElement(); // draw:equation
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "?f1 -?f6 ");
    out.xml.addAttribute("draw:name", "f7");
    out.xml.endElement(); // draw:equation
    out.xml.startElement("draw:handle");
    if (o.shapeProp.rh.recInstance == msosptLeftRightArrow || o.shapeProp.rh.recInstance == msosptLeftArrow) {
        out.xml.addAttribute("draw:handle-range-x-maximum", 21600);
        out.xml.addAttribute("draw:handle-range-x-minimum", 0);
        out.xml.addAttribute("draw:handle-position", "$0 $1");
        out.xml.addAttribute("draw:handle-range-y-maximum", 10800);
        out.xml.addAttribute("draw:handle-range-y-minimum", 0);
    } else if (o.shapeProp.rh.recInstance == msosptUpArrow || o.shapeProp.rh.recInstance == msosptDownArrow) {
        out.xml.addAttribute("draw:handle-range-x-maximum", 10800);
        out.xml.addAttribute("draw:handle-range-x-minimum", 0);
        out.xml.addAttribute("draw:handle-position", "$1 $0");
        out.xml.addAttribute("draw:handle-range-y-maximum", 21600);
        out.xml.addAttribute("draw:handle-range-y-minimum", 0);
    }
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // draw:enhanced-geometry
    out.xml.endElement(); // draw:custom-shape
}

void PptToOdp::processLine(const OfficeArtSpContainer& o, Writer& out)
{
    const QRect rect = getRect(o);
    QString x1Str = QString("%1mm").arg(rect.x());
    QString y1Str = QString("%1mm").arg(rect.y());
    QString x2Str = QString("%1mm").arg(rect.x() + rect.width());
    QString y2Str = QString("%1mm").arg(rect.y() + rect.height());

    if (o.shapeProp.fFlipV) {
        QString temp = y1Str;
        y1Str = y2Str;
        y2Str = temp;
    }
    if (o.shapeProp.fFlipH) {
        QString temp = x1Str;
        x1Str = x2Str;
        x2Str = temp;
    }

    out.xml.startElement("draw:line");
    out.xml.addAttribute("draw:style-name", getGraphicStyleName(o));
    out.xml.addAttribute("svg:y1", y1Str);
    out.xml.addAttribute("svg:y2", y2Str);
    out.xml.addAttribute("svg:x1", x1Str);
    out.xml.addAttribute("svg:x2", x2Str);
    out.xml.addAttribute("draw:layer", "layout");

    out.xml.endElement();
}

void PptToOdp::processSmiley(const OfficeArtSpContainer& o, Writer& out)
{
    const QRect rect = getRect(o);
    out.xml.startElement("draw:custom-shape");
    out.xml.addAttribute("draw:style-name", getGraphicStyleName(o));
    out.xml.addAttribute("svg:width", out.hLength(rect.width()));
    out.xml.addAttribute("svg:height", out.vLength(rect.height()));
    out.xml.addAttribute("svg:x", out.hOffset(rect.x()));
    out.xml.addAttribute("svg:y", out.vOffset(rect.y()));
    out.xml.addAttribute("draw:layer", "layout");
    out.xml.startElement("draw:glue-point");
    out.xml.addAttribute("svg:x", 5);
    out.xml.addAttribute("svg:y", 0);
    out.xml.endElement();
    out.xml.startElement("draw:glue-point");
    out.xml.addAttribute("svg:x", 1.461);
    out.xml.addAttribute("svg:y", 1.461);
    out.xml.endElement();
    out.xml.startElement("draw:glue-point");
    out.xml.addAttribute("svg:x", 0);
    out.xml.addAttribute("svg:y", 5);
    out.xml.endElement();
    out.xml.startElement("draw:glue-point");
    out.xml.addAttribute("svg:x", 1.461);
    out.xml.addAttribute("svg:y", 8.536);
    out.xml.endElement();
    out.xml.startElement("draw:glue-point");
    out.xml.addAttribute("svg:x", 10);
    out.xml.addAttribute("svg:y", 5);
    out.xml.endElement();
    out.xml.startElement("draw:glue-point");
    out.xml.addAttribute("svg:x", 8.536);
    out.xml.addAttribute("svg:y", 1.461);
    out.xml.endElement();
    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:type", "smiley");
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "$0-15510 ");
    out.xml.addAttribute("draw:name", "f0");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "17520-?f0");
    out.xml.addAttribute("draw:name", "f1");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:formula", "15510+?f0");
    out.xml.addAttribute("draw:name", "f2");
    out.xml.endElement();
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:position", 10800);
    out.xml.addAttribute("draw:handle-range-y-maximum", 17520);
    out.xml.addAttribute("draw:handle-range-y-minimum", 15510);
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.endElement();
    out.xml.endElement(); // enhanced-geometry
    out.xml.endElement(); // custom-shape
}

void PptToOdp::processHeart(const OfficeArtSpContainer& o, Writer& out)
{
    const QRect rect = getRect(o);
    out.xml.startElement("draw:custom-shape");
    out.xml.addAttribute("draw:style-name", getGraphicStyleName(o));
    out.xml.addAttribute("svg:width", out.hLength(rect.width()));
    out.xml.addAttribute("svg:height", out.vLength(rect.height()));
    out.xml.addAttribute("svg:x", out.hOffset(rect.x()));
    out.xml.addAttribute("svg:y", out.vOffset(rect.y()));
    out.xml.addAttribute("draw:layer", "layout");
    out.xml.startElement("draw:glue-point");
    out.xml.addAttribute("svg:x", 5);
    out.xml.addAttribute("svg:y", 1);
    out.xml.endElement();
    out.xml.startElement("draw:glue-point");
    out.xml.addAttribute("svg:x", 1.43);
    out.xml.addAttribute("svg:y", 5);
    out.xml.endElement();
    out.xml.startElement("draw:glue-point");
    out.xml.addAttribute("svg:x", 5);
    out.xml.addAttribute("svg:y", 10);
    out.xml.endElement();
    out.xml.startElement("draw:glue-point");
    out.xml.addAttribute("svg:x", 8.553);
    out.xml.addAttribute("svg:y", 5);
    out.xml.endElement();
    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:type", "heart");

    out.xml.endElement(); // enhanced-geometry
    out.xml.endElement(); // custom-shape
}

void PptToOdp::processFreeLine(const OfficeArtSpContainer& o, Writer& out)
{
    const QRect rect = getRect(o);
    out.xml.startElement("draw:path");
    out.xml.addAttribute("draw:style-name", getGraphicStyleName(o));
    out.xml.addAttribute("svg:width", out.hLength(rect.width()));
    out.xml.addAttribute("svg:height", out.vLength(rect.height()));
    out.xml.addAttribute("svg:x", out.hOffset(rect.x()));
    out.xml.addAttribute("svg:y", out.vOffset(rect.y()));
    out.xml.addAttribute("draw:layer", "layout");
    out.xml.endElement(); // path
}

QString PptToOdp::getPicturePath(int pib) const
{
    int picturePosition = pib - 1;
    QByteArray rgbUid = getRgbUid(picturePosition);
    return rgbUid.length() ? "Pictures/" + pictureNames[rgbUid] : "";
}

void PptToOdp::processPictureFrame(const OfficeArtSpContainer& o, Writer& out)
{
    const QRect rect = getRect(o);
    QString url;
    const Pib* pib = get<Pib>(o);
    if (pib) {
        url = getPicturePath(pib->pib);
    }
    //Ima drawObject->getIntProperty("pib"));
    out.xml.startElement("draw:frame");
    out.xml.addAttribute("draw:style-name", getGraphicStyleName(o));
    out.xml.addAttribute("svg:width", out.hLength(rect.width()));
    out.xml.addAttribute("svg:height", out.vLength(rect.height()));
    out.xml.addAttribute("svg:x", out.hOffset(rect.x()));
    out.xml.addAttribute("svg:y", out.vOffset(rect.y()));
    out.xml.addAttribute("draw:layer", "layout");
    out.xml.startElement("draw:image");
    out.xml.addAttribute("xlink:href", url);
    out.xml.addAttribute("xlink:type", "simple");
    out.xml.addAttribute("xlink:show", "embed");
    out.xml.addAttribute("xlink:actuate", "onLoad");
    out.xml.endElement(); // image
    out.xml.endElement(); // frame
}

void PptToOdp::processDrawingObjectForBody(const OfficeArtSpContainer& o, Writer& out)
{
    quint16 shapeType = o.shapeProp.rh.recInstance;
    if (shapeType == msosptEllipse) {
        processEllipse(o, out);
    } else if (shapeType == msosptRectangle) {
        processRectangle(o, out);
    } else if (shapeType == msosptRoundRectangle) {
        processRoundRectangle(o, out);
    } else  if (shapeType == msosptDiamond) {
        processDiamond(o, out);
    } else  if (shapeType == msosptIsoscelesTriangle ||
                shapeType == msosptRightTriangle) {
        processTriangle(o, out);
    } else if (shapeType == msosptTrapezoid) {
        processTrapezoid(o, out);
    } else if (shapeType == msosptParallelogram) {
        processParallelogram(o, out);
    } else if (shapeType == msosptHexagon) {
        processHexagon(o, out);
    } else if (shapeType == msosptOctagon) {
        processOctagon(o, out);
    } else if (shapeType == msosptLeftArrow ||
               shapeType == msosptUpArrow ||
               shapeType == msosptDownArrow) {
        processArrow(o, out);
    } else if (shapeType == msosptLine) {
        processLine(o, out);
    } else if (shapeType == msosptSmileyFace) {
        processSmiley(o, out);
    } else if (shapeType == msosptHeart) {
        processHeart(o, out);
        //} else if (shapeType == msosptMin) {
        //    processFreeLine(o, out);
    } else if (shapeType == msosptPictureFrame
               || shapeType == msosptHostControl) {
        processPictureFrame(o, out);
    } else if (shapeType == msosptTextBox) {
        qDebug() << "what's my name!' " << o.shapeProp.rh.recInstance;
        //processTextObjectForBody(o, , out);
    } else {
        Q_ASSERT(o.shapeProp.rh.recInstance);
        qDebug() << "cannot handle object of type " << o.shapeProp.rh.recInstance;
    }
}

void PptToOdp::writeTextCFException(KoXmlWriter& xmlWriter,
                                    const TextCFException *cf,
                                    const TextPFException *pf,
                                    const QString &text)
{
    xmlWriter.startElement("text:span");
    xmlWriter.addAttribute("text:style-name", getTextStyleName(cf, pf));

    QString copy = text;
    copy.remove(QChar(11)); //Remove vertical tabs which appear in some ppt files
    xmlWriter.addTextSpan(copy);

    xmlWriter.endElement(); // text:span
}

const TextCFRun *findTextCFRun(const StyleTextPropAtom& style, unsigned int pos)
{
    quint32 counter = 0;
    foreach(const TextCFRun& cf, style.rgTextCFRun) {
        if (pos >= counter && pos < counter + cf.count) {
            return &cf;
        }
        counter += cf.count;
    }
    return 0;
}

const TextPFRun *findTextPFRun(const StyleTextPropAtom& style, unsigned int pos)
{
    quint32 counter = 0;
    foreach(const TextPFRun& pf, style.rgTextPFRun) {
        if (pos >= counter && pos < counter + pf.count) {
            return &pf;
        }
    }
    return 0;
}

void PptToOdp::writeTextLine(KoXmlWriter& xmlWriter,
                             const StyleTextPropAtom& style,
                             const TextPFException *pf,
                             const QString& text,
                             const unsigned int linePosition)
{
    QString part = "";
    const TextCFRun *cf = findTextCFRun(style, linePosition);
    if (!cf) {
        return;
    }

    if (text.isEmpty()) {
        writeTextCFException(xmlWriter, &cf->cf, pf, text);
        return;
    }

    //Iterate through all the characters in text
    for (int i = 0;i < text.length();i++) {
        const TextCFRun *nextCFRun = findTextCFRun(style, linePosition + i);

        //While character exception stays the same
        if (cf == nextCFRun) {
            //Catenate strings to our substring
            part += text[i];
        } else {
            /*
            When exception changes we write the text to xmlwriter unless the
            text style name stays the same, then we'll reuse the same
            stylename for the next character exception
            */
            if (nextCFRun &&
                    getTextStyleName(&cf->cf, pf) != getTextStyleName(&nextCFRun->cf, pf)) {
                writeTextCFException(xmlWriter, &cf->cf, pf, part);
                part = text[i];
            } else {
                part += text[i];
            }

            cf = nextCFRun;
        }
    }

    //If at the end we still have some text left, write it out
    if (!part.isEmpty()) {
        writeTextCFException(xmlWriter, &cf->cf, pf, part);
    }
}

void PptToOdp::writeTextObjectDeIndent(KoXmlWriter& xmlWriter,
                                       const unsigned int count, QStack<QString>& levels)
{
    while ((unsigned int)levels.size() > count) {
        // if the style name at the lowest level is empty, there is no
        // list there
        if (levels.size() > 1
                || (levels.size() == 1 && !levels.top().isNull())) {
            xmlWriter.endElement(); // text:list
        }
        levels.pop();
        if (levels.size() > 1
                || (levels.size() == 1 && !levels.top().isNull())) {
            xmlWriter.endElement(); // text:list-item
        }
    }
}
namespace
{
void addListElement(KoXmlWriter& xmlWriter, QStack<QString>& levels,
                    const QString& listStyle)
{
    if (!listStyle.isNull()) {
        // if the context is a text:list, a text:list-item is needed
        if (levels.size() > 1
                || (levels.size() == 1 && !levels.top().isNull())) {
            xmlWriter.startElement("text:list-item");
        }
        xmlWriter.startElement("text:list");
        if (!listStyle.isEmpty()) {
            xmlWriter.addAttribute("text:style-name", listStyle);
        }
    }
    levels.push(listStyle);
}
}

void PptToOdp::writeTextPFException(KoXmlWriter& xmlWriter,
                                    const TextPFRun *pf,
                                    const TextContainer& tc,
                                    const QString& intext,
                                    const unsigned int textPos,
                                    QStack<QString>& levels)
{
    const StyleTextPropAtom* style = tc.style.data();
    if (!pf || !style) {
        return;
    }

    QString text = intext.mid(textPos, pf->count);
    //Text lines are separated with carriage return
    //There seems to be an extra carriage return at the end. We'll remove the last
    // carriage return so we don't end up with a single empty line in the end
    if (text.endsWith("\r")) {
        text = text.left(text.length() - 1);
    }

    //Then split the text into lines
    QStringList lines = text.split("\r");
    unsigned int linePos = textPos;

    //Indentation level paragraph wants
    unsigned int paragraphIndent = pf->indentLevel;
    //[MS-PPT].pdf says the itendation level can be 4 at most
    if (paragraphIndent > 4) paragraphIndent = 4;
    qint16 bullet = 0;
    //Check if this paragraph has a bullet
    if (pf->pf.masks.bulletChar) {
        bullet = pf->pf.bulletChar;
    } else {
        //If text paragraph exception doesn't have a definition on bullet
        //then we'll have to check master style with our indentation level
        const TextPFException *masterPF
                = masterTextPFException(tc.textHeaderAtom.textType,
                                        pf->indentLevel);
        if (masterPF && masterPF->masks.bulletChar) {
            bullet = masterPF->bulletChar;
        }
    }
    const TextCFRun *cf = findTextCFRun(&style, linePos);
    // find the name of the list style, if the current style is a list
    // if the style is null, there is no, list, if the style is an empty string
    // there is a list but it has no style
    QString listStyle;
    if (cf && (bullet || paragraphIndent > 0)) {
        listStyle = getListStyleName(&cf->cf, &pf->pf);
    }
    if (listStyle.isNull() && (bullet || paragraphIndent > 0)) {
        listStyle = "";
    }
    // remove levels until the top level is the right indentation
    if ((unsigned int)levels.size() > paragraphIndent
            && levels[paragraphIndent] == listStyle) {
        writeTextObjectDeIndent(xmlWriter, paragraphIndent + 2, levels);
    } else {
        writeTextObjectDeIndent(xmlWriter, paragraphIndent + 1, levels);
    }
    // add styleless levels up to the current level of indentation
    while ((unsigned int)levels.size() < paragraphIndent) {
        addListElement(xmlWriter, levels, "");
    }
    // at this point, levels.size() == paragraphIndent
    if (levels.size() == 0 || listStyle != levels.top()) {
        addListElement(xmlWriter, levels, listStyle);
    }
    bool listItem = levels.size() > 1 || !levels.top().isNull();
    if (listItem) {
        xmlWriter.startElement("text:list-item");
    }
    QString pstyle;
    if (cf) {
        pstyle = getParagraphStyleName(&cf->cf,                                     &pf->pf);
    }
    xmlWriter.startElement("text:p");
    if (pstyle.size() > 0) {
        xmlWriter.addAttribute("text:style-name", pstyle);
    }

    //Handle all lines
    for (int i = 0;i < lines.size();i++) {
        cf = findTextCFRun(*style, linePos);
        if (cf) {
            writeTextCFException(xmlWriter, &cf->cf, &pf->pf, lines[i]);
        }

        //Add +1 to line position to compensate for carriage return that is
        //missing from line due to the split method
        linePos += lines[i].size() + 1;
    }
    xmlWriter.endElement(); // text:p
    if (listItem) {
        xmlWriter.endElement(); // text:list-item
    }
}
namespace Text
{
enum {
    Title       = 0,
    Body        = 1,
    Notes       = 2,
    NotUsed     = 3,
    Other       = 4,  // text in a shape
    CenterBody  = 5,  // subtitle in title slide
    CenterTitle = 6,  // title in title slide
    HalfBody    = 7,  // body in two-column slide
    QuarterBody = 8   // body in four-body slide
};
}
QString
getText(const TextContainer& tc)
{
    if (tc.text.is<TextCharsAtom>()) {
        const QVector<quint16> textChars(tc.text.get<TextCharsAtom>()->textChars);
        return QString::fromUtf16(textChars.data(), textChars.size());
    } else if (tc.text.is<TextBytesAtom>()) {
        // each item represents the low byte of a UTF-16 Unicode character whose high byte is 0x00
        const QByteArray& textChars(tc.text.get<TextBytesAtom>()->textChars);
        return QString::fromAscii(textChars, textChars.size());
    }
    return QString();
}

void PptToOdp::processTextObjectForBody(const OfficeArtSpContainer& o, const PPT::TextContainer& tc, Writer& out)
{
    const QRect& rect = getRect(o);
    QString classStr = "subtitle";

    if (tc.textHeaderAtom.textType == Text::Title)
        classStr = "title";

    if (tc.textHeaderAtom.textType == Text::Body)
        classStr = "outline";

    out.xml.startElement("draw:frame");

    if (!getGraphicStyleName(o).isEmpty()) {
        out.xml.addAttribute("presentation:style-name",
                             getGraphicStyleName(o));
    }

    out.xml.addAttribute("draw:layer", "layout");

    out.xml.addAttribute("svg:width", out.hLength(rect.width()));
    out.xml.addAttribute("svg:height", out.vLength(rect.height()));
    out.xml.addAttribute("svg:x", out.hOffset(rect.x()));
    out.xml.addAttribute("svg:y", out.vOffset(rect.y()));
    out.xml.addAttribute("presentation:class", classStr);
    out.xml.startElement("draw:text-box");

    QString text = getText(tc);

    const StyleTextPropAtom *style = tc.style.data();
    if (style  && style->rgTextPFRun.size()) {
        //Paragraph formatting that applies to substring
        QStack<QString> levels;
        levels.reserve(5);
        int pos = 0;
        foreach(const TextPFRun& pf, style->rgTextPFRun) {
            writeTextPFException(out.xml, &pf, tc, text, pos, levels);
            pos += pf.count;
        }

        writeTextObjectDeIndent(out.xml, 0, levels);
    } else {
        out.xml.startElement("text:p");

        if (!getParagraphStyleName(0, 0).isEmpty()) {
            out.xml.addAttribute("text:style-name", getParagraphStyleName(0, 0));
        }

        out.xml.startElement("text:span");
        if (!getTextStyleName(0, 0).isEmpty()) {
            out.xml.addAttribute("text:style-name", getTextStyleName(0, 0));
        }

        out.xml.addTextSpan(text);
        out.xml.endElement(); // text:span
        out.xml.endElement(); // text:p
    }

    out.xml.endElement(); // draw:text-box
    out.xml.endElement(); // draw:frame
}

void PptToOdp::processObjectForBody(const OfficeArtSpgrContainerFileBlock& of, Writer& out)
{
    if (of.anon.is<OfficeArtSpgrContainer>()) {
        processObjectForBody(*of.anon.get<OfficeArtSpgrContainer>(), out);
    } else { // OfficeArtSpContainer
        processObjectForBody(*of.anon.get<OfficeArtSpContainer>(), out);
    }
}
void PptToOdp::processObjectForBody(const PPT::OfficeArtSpgrContainer& o, Writer& out)
{
    if (o.rgfb.size() < 2) return;
    out.xml.startElement("draw:g");
    /* if the first OfficeArtSpContainer has a clientAnchor,
       a new coordinate system is introduced.
       */
    const OfficeArtSpContainer* first
    = o.rgfb[0].anon.get<OfficeArtSpContainer>();
    if (first && first->clientAnchor && first->shapeGroup) {
        const QRect oldCoords = getRect(*first->clientAnchor);
        QRect newCoords = getRect(*first->shapeGroup);
        Writer transformedOut = out.transform(oldCoords, newCoords);
        for (int i = 1; i < o.rgfb.size(); ++i) {
            processObjectForBody(o.rgfb[i], transformedOut);
        }
    } else {
        for (int i = 1; i < o.rgfb.size(); ++i) {
            processObjectForBody(o.rgfb[i], out);
        }
    }
    out.xml.endElement(); // draw:g
}
void PptToOdp::processObjectForBody(const PPT::OfficeArtSpContainer& o, Writer& out)
{
    // text is process separately until drawing objects support it
    if (o.clientData && o.clientData->placeholderAtom) {
        const PlaceholderAtom* p = o.clientData->placeholderAtom.data();
        if (p->position >= 0 && p->position < currentSlideTexts->atoms.size()) {
            const TextContainer& tc = currentSlideTexts->atoms[p->position];
            processTextObjectForBody(o, tc, out);
        } else {
            processDrawingObjectForBody(o, out);
        }
    } else if (o.clientTextbox) { // TODO
        foreach(const TextClientDataSubContainerOrAtom& tc, o.clientTextbox->rgChildRec) {
            if (tc.anon.is<TextContainer>()) {
                processTextObjectForBody(o, *tc.anon.get<TextContainer>(), out);
            }
        }
    } else if (o.shapeProp.rh.recInstance) { // why is this check needed?
        processDrawingObjectForBody(o, out);
    }
}

void PptToOdp::processSlideForBody(unsigned slideNo, KoXmlWriter& xmlWriter)
{
    const SlideContainer* slide = p->slides[slideNo];
    const MasterOrSlideContainer* master = p->getMaster(slide);
    if (!master) return;
    int masterNumber = p->masters.indexOf(master);
    if (masterNumber == -1) return;

    QString nameStr;
    // take the slide name if present (usually it is not)
    if (slide->slideNameAtom) {
        nameStr = QString::fromUtf16(slide->slideNameAtom->slideName.data(),
                                     slide->slideNameAtom->slideName.size());
    }
    // look for a title on the slide
    if (nameStr.isEmpty()) {
        foreach(const TextContainer& tc, p->documentContainer->slideList->rgChildRec[slideNo].atoms) {
            if (tc.textHeaderAtom.textType == Text::Title) {
                nameStr = getText(tc);
            }
        }
    }

    if (nameStr.isEmpty())
        nameStr = QString("page%1").arg(slideNo + 1);

    xmlWriter.startElement("draw:page");
    xmlWriter.addAttribute("draw:master-page-name", "Default");
    xmlWriter.addAttribute("draw:name", nameStr);
    xmlWriter.addAttribute("draw:style-name", QString("master%1").arg(masterNumber));
    xmlWriter.addAttribute("presentation:presentation-page-layout-name", "AL1T0");

    const HeadersFootersAtom* headerFooterAtom = 0;
    if (master->anon.is<MainMasterContainer>()) {
        const MainMasterContainer* m = master->anon.get<MainMasterContainer>();
        if (m->perSlideHeadersFootersContainer) {
            headerFooterAtom = &m->perSlideHeadersFootersContainer->hfAtom;
        }
    } else {
        const SlideContainer* s = master->anon.get<SlideContainer>();
        if (s->perSlideHFContainer) {
            headerFooterAtom = &s->perSlideHFContainer->hfAtom;
        }
    }
    if (!headerFooterAtom && getSlideHF()) {
        headerFooterAtom = &getSlideHF()->hfAtom;
    }
    if (headerFooterAtom && headerFooterAtom->fHasTodayDate) {
        xmlWriter.addAttribute("presentation:use-date-time-name", "dtd1");
    }

    QRectF rect; // TODO add some geometry magic
    currentSlide = slide;
    currentSlideTexts = &p->documentContainer->slideList->rgChildRec[slideNo];

    Writer out(xmlWriter);
    foreach(const OfficeArtSpgrContainerFileBlock& co,
            slide->drawing.OfficeArtDg.groupShape.rgfb) {
        processObjectForBody(co, out);
    }
    if (slide->drawing.OfficeArtDg.shape) {
        // leave it out until it is understood
        //  processObjectForBody(*slide->drawing.OfficeArtDg.shape, out);
    }

    xmlWriter.endElement(); // draw:page
}

void PptToOdp::processSlideForStyle(int slideNo, KoGenStyles &styles)
{
    const SlideContainer* slide = p->slides[slideNo];
    processObjectForStyle(slide->drawing.OfficeArtDg.groupShape, styles);
    if (slide->drawing.OfficeArtDg.shape) {
        processObjectForStyle(*slide->drawing.OfficeArtDg.shape, styles);
    }
}
void PptToOdp::processObjectForStyle(const OfficeArtSpgrContainerFileBlock& of, KoGenStyles &styles)
{
    if (of.anon.is<OfficeArtSpgrContainer>()) {
        processObjectForStyle(*of.anon.get<OfficeArtSpgrContainer>(), styles);
    } else { // OfficeArtSpContainer
        processObjectForStyle(*of.anon.get<OfficeArtSpContainer>(), styles);
    }
}
void PptToOdp::processObjectForStyle(const PPT::OfficeArtSpgrContainer& o, KoGenStyles &styles)
{
    foreach(const OfficeArtSpgrContainerFileBlock& co, o.rgfb) {
        processObjectForStyle(co, styles);
    }
}
void PptToOdp::processObjectForStyle(const PPT::OfficeArtSpContainer& o, KoGenStyles &styles)
{
    // text is process separately until drawing objects support it
    if (o.clientTextbox) {
        foreach(const TextClientDataSubContainerOrAtom& tc, o.clientTextbox->rgChildRec) {
            if (tc.anon.is<TextContainer>()) {
                processTextObjectForStyle(o, *tc.anon.get<TextContainer>(), styles);
            }
        }
    }
    processDrawingObjectForStyle(o, styles);
}
QString PptToOdp::paraSpacingToCm(int value) const
{
    if (value < 0) {
        unsigned int temp = -value;
        return pptMasterUnitToCm(temp);
    }

    return pptMasterUnitToCm(value);
}

QString PptToOdp::pptMasterUnitToCm(unsigned int value) const
{
    qreal result = value;
    result *= 2.54;
    result /= 576;
    return QString("%1cm").arg(result);
}

QString PptToOdp::textAlignmentToString(unsigned int value) const
{
    switch (value) {
        /**
        Tx_ALIGNLeft            0x0000 For horizontal text, left aligned.
                                   For vertical text, top aligned.
        */
    case 0:
        return "left";
        /**
        Tx_ALIGNCenter          0x0001 For horizontal text, centered.
                                   For vertical text, middle aligned.
        */
    case 1:
        return "center";
        /**
        Tx_ALIGNRight           0x0002 For horizontal text, right aligned.
                                   For vertical text, bottom aligned.
        */
    case 2:
        return "right";

        /**
        Tx_ALIGNJustify         0x0003 For horizontal text, flush left and right.
                                   For vertical text, flush top and bottom.
        */
        return "justify";

        //TODO these were missing from ODF specification v1.1, but are
        //in [MS-PPT].pdf

        /**
        Tx_ALIGNDistributed     0x0004 Distribute space between characters.
        */
    case 4:

        /**
        Tx_ALIGNThaiDistributed 0x0005 Thai distribution justification.
        */
    case 5:

        /**
        Tx_ALIGNJustifyLow      0x0006 Kashida justify low.
        */
    case 6:
        return "";

        //TODO these two are in ODF specification v1.1 but are missing from
        //[MS-PPT].pdf
        //return "end";
        //return "start";
    }

    return QString();
}

QColor PptToOdp::toQColor(const ColorIndexStruct &color)
{
    if (color.index == 0xFE) {
        return QColor(color.red, color.green, color.blue);
    }
    if (color.index == 0xFF) { // color is undefined
        return QColor();
    }

    const QList<ColorStruct>* colorScheme;
    // TODO: use the current master
    const MasterOrSlideContainer* m = p->masters[0];
    if (m->anon.is<MainMasterContainer>()) {
        const MainMasterContainer* n = m->anon.get<MainMasterContainer>();
        colorScheme = &n->slideSchemeColorSchemeAtom.rgSchemeColor;
    } else {
        const SlideContainer* n = m->anon.get<SlideContainer>();
        colorScheme = &n->slideSchemeColorSchemeAtom.rgSchemeColor;
    }
    if (colorScheme->size() > color.index) {
        const ColorStruct c = colorScheme->at(color.index);
        return QColor(c.red, c.green, c.blue);
    }
    return QColor();
}
QColor PptToOdp::toQColor(const OfficeArtCOLORREF& c)
{
    if (c.fSchemeIndex) {
        ColorStruct cs;
        // This should get the color from the color scheme of the current slide
        // or if slideContainer/slideAtom/slideFlags/fMasterScheme == true
        // from the slides masters color scheme.
        // TODO: use the current master
        const MasterOrSlideContainer* m = p->masters[0];
        if (m->anon.is<MainMasterContainer>()) {
            const MainMasterContainer* n = m->anon.get<MainMasterContainer>();
            if (n->slideSchemeColorSchemeAtom.rgSchemeColor.size() <= c.red) {
                return QColor();
            }
            cs = n->slideSchemeColorSchemeAtom.rgSchemeColor[c.red];
        } else {
            const SlideContainer* n = m->anon.get<SlideContainer>();
            if (n->slideSchemeColorSchemeAtom.rgSchemeColor.size() <= c.red) {
                return QColor();
            }
            cs = n->slideSchemeColorSchemeAtom.rgSchemeColor.value(c.red);
        }
        return QColor(cs.red, cs.green, cs.blue);
    }
    return QColor(c.red, c.green, c.blue);
}

const TextMasterStyleLevel *
getTextMasterStyleLevel(quint16 type, quint16 level, const MasterOrSlideContainer* m)
{
    const TextMasterStyleAtom* masterStyle = 0;
    if (m->anon.is<MainMasterContainer>()) {
        const MainMasterContainer* n = m->anon.get<MainMasterContainer>();
        if (n->rgTextMasterStyle.size() > type) {
            masterStyle = &n->rgTextMasterStyle[type];
        }
    } else {
        const MainMasterContainer* n = m->anon.get<MainMasterContainer>();
        if (n->rgTextMasterStyle.size() > type) {
            masterStyle = &n->rgTextMasterStyle[type];
        }
    }
    if (!masterStyle) {
        return 0;
    }
    const TextMasterStyleLevel *l = 0;
    switch (level) {
    case 0: if (masterStyle->lstLvl1) l = masterStyle->lstLvl1.data();break;
    case 1: if (masterStyle->lstLvl2) l = masterStyle->lstLvl2.data();break;
    case 2: if (masterStyle->lstLvl3) l = masterStyle->lstLvl3.data();break;
    case 3: if (masterStyle->lstLvl4) l = masterStyle->lstLvl4.data();break;
    case 4: if (masterStyle->lstLvl5) l = masterStyle->lstLvl5.data();break;
    }
    return l;
}

const TextPFException *PptToOdp::masterTextPFException(quint16 type, quint16 level)
{
    // TODO look for the right master, not just the first one
    const MasterOrSlideContainer* m = p->masters[0];
    const TextMasterStyleLevel *l = getTextMasterStyleLevel(type, level, m);
    return (l) ? &l->pf : 0;
}

const TextCFException *PptToOdp::masterTextCFException(int type, unsigned int level)
{
    // TODO look for the right master, not just the first one
    const MasterOrSlideContainer* m = p->masters[0];
    const TextMasterStyleLevel *l = getTextMasterStyleLevel(type, level, m);
    return (l) ? &l->cf : 0;
}

void PptToOdp::processTextExceptionsForStyle(const TextCFRun *cf,
        const TextPFRun *pf,
        KoGenStyles &styles,
        const TextContainer& tc)
{
    qint16 indentLevel = 0;
    int indent = 0;
    if (pf) {
        indentLevel = pf->indentLevel;
        if (pf->pf.masks.indent) {
            indent = pf->pf.indent;
        }
    }

    qint16 textType = tc.textHeaderAtom.textType;

    /**
    TODO I had some ppt files where the text headers of slide body's text
    where defined as Tx_TYPE_CENTERBODY and title as Tx_TYPE_CENTERTITLE
    but their true style definitions (when compared to MS Powerpoint)
    where in Tx_TYPE_BODY and Tx_TYPE_TITLE TextMasterStyleAtoms.
    Either the text type is loaded incorrectly or there is some logic behind
    this that needs to be figured out.
    */
    if (textType == 5) {
        //Replace Tx_TYPE_CENTERBODY with Tx_TYPE_BODY
        textType = 1;
    } else if (textType == 6) {
        //and Tx_TYPE_CENTERTITLE with Tx_TYPE_TITLE
        textType = 0;
    }

    //Master character/paragraph styles are fetched for the textobjects type
    //using paragraph's indentation level
    const TextCFException *masterCF = masterTextCFException(textType,
                                      indentLevel);

    const TextPFException *masterPF = masterTextPFException(textType,
                                      indentLevel);

    //As mentioned in previous TODO we'll check if the text types
    //were placeholder types (2.13.33 TextTypeEnum in [MS-PPT].pdf
    //and use them for some styles
    const TextCFException *placeholderCF = 0;
    const TextPFException *placeholderPF = 0;

    if (textType == 5 || textType == 6) {
        placeholderPF = masterTextPFException(textType, indentLevel);
        placeholderCF = masterTextCFException(textType, indentLevel);
    }

    KoGenStyle styleParagraph(KoGenStyle::StyleAuto, "paragraph");
    if (pf && pf->pf.masks.leftMargin) {
        styleParagraph.addProperty("fo:margin-left",
                                   paraSpacingToCm(pf->pf.leftMargin),
                                   KoGenStyle::ParagraphType);
    } else {
        if (masterPF && masterPF->masks.leftMargin) {
            styleParagraph.addProperty("fo:margin-left",
                                       paraSpacingToCm(masterPF->leftMargin),
                                       KoGenStyle::ParagraphType);
        }
    }

    if (pf && pf->pf.masks.spaceBefore) {
        styleParagraph.addProperty("fo:margin-top",
                                   paraSpacingToCm(pf->pf.spaceBefore),
                                   KoGenStyle::ParagraphType);
    } else {
        if (masterPF && masterPF->masks.spaceBefore) {
            styleParagraph.addProperty("fo:margin-top",
                                       paraSpacingToCm(masterPF->spaceBefore),
                                       KoGenStyle::ParagraphType);
        }
    }

    if (pf && pf->pf.masks.spaceAfter) {
        styleParagraph.addProperty("fo:margin-bottom",
                                   paraSpacingToCm(pf->pf.spaceAfter),
                                   KoGenStyle::ParagraphType);
    } else {
        if (masterPF && masterPF->masks.spaceAfter) {
            styleParagraph.addProperty("fo:margin-bottom",
                                       paraSpacingToCm(masterPF->spaceAfter),
                                       KoGenStyle::ParagraphType);
        }
    }

    if (pf && pf->pf.masks.indent) {
        styleParagraph.addProperty("fo:text-indent",
                                   pptMasterUnitToCm(pf->pf.indent),
                                   KoGenStyle::ParagraphType);
    } else {
        if (masterPF && masterPF->masks.indent) {
            styleParagraph.addProperty("fo:text-indent",
                                       pptMasterUnitToCm(masterPF->indent),
                                       KoGenStyle::ParagraphType);
        }
    }


    /**
    TODO When previous TODO about Tx_TYPE_CENTERBODY and Tx_TYPE_CENTERTITLE
    is fixed, correct the logic here. Here is added an extra if to use the
    placeholderPF which in turn contained the correct value.
    */
    if (pf && pf->pf.masks.align) {
        styleParagraph.addProperty("fo:text-align",
                                   textAlignmentToString(pf->pf.textAlignment),
                                   KoGenStyle::ParagraphType);
    } else if (placeholderPF && placeholderPF->masks.align) {
        styleParagraph.addProperty("fo:text-align",
                                   textAlignmentToString(placeholderPF->textAlignment),
                                   KoGenStyle::ParagraphType);
    } else {
        if (masterPF && masterPF->masks.align) {
            styleParagraph.addProperty("fo:text-align",
                                       textAlignmentToString(masterPF->textAlignment),
                                       KoGenStyle::ParagraphType);
        }
    }

    //Text style
    KoGenStyle styleText(KoGenStyle::StyleTextAuto, "text");
    if (cf && cf->cf.color) {
        QColor color = toQColor(*cf->cf.color);
        if (color.isValid()) {
            styleText.addProperty("fo:color", color.name(),
                                  KoGenStyle::TextType);
        }
    } else {
        //Make sure that character formatting has color aswell
        if (masterCF && masterCF->color) {
            QColor color = toQColor(*masterCF->color);
            if (color.isValid()) {
                styleText.addProperty("fo:color", color.name(),
                                      KoGenStyle::TextType);
            }
        }
    }

    if (cf && cf->cf.masks.size) {
        styleText.addProperty("fo:font-size",
                              QString("%1pt").arg(cf->cf.fontSize),
                              KoGenStyle::TextType);
    } else {
        if (masterCF && masterCF->masks.size) {
            styleText.addProperty("fo:font-size",
                                  QString("%1pt").arg(masterCF->fontSize),
                                  KoGenStyle::TextType);
        }
    }

    if (cf && cf->cf.masks.italic && cf->cf.fontStyle) {
        if (cf->cf.fontStyle->italic) {
            styleText.addProperty("fo:font-style",
                                  "italic",
                                  KoGenStyle::TextType);
        }
    } else {
        if (masterCF && masterCF->masks.italic && masterCF->fontStyle
                && masterCF->fontStyle->italic) {
            styleText.addProperty("fo:font-style",
                                  "italic",
                                  KoGenStyle::TextType);
        }
    }

    if (cf && cf->cf.masks.bold && cf->cf.fontStyle) {
        if (cf->cf.fontStyle->bold) {
            styleText.addProperty("fo:font-weight",
                                  "bold",
                                  KoGenStyle::TextType);
        }
    } else {
        if (masterCF && masterCF->masks.bold && masterCF->fontStyle && masterCF->fontStyle->bold) {
            styleText.addProperty("fo:font-weight",
                                  "bold",
                                  KoGenStyle::TextType);
        }
    }

    const FontEntityAtom* font = 0;
    if (cf && cf->cf.masks.typeface) {
        font = getFont(cf->cf.fontRef);
    } else {
        if (masterCF && masterCF->masks.typeface) {
            font = getFont(masterCF->fontRef);
        }
    }

    if (font) {
        styleText.addProperty("fo:font-family",
                              QString::fromUtf16(font->lfFaceName.data(), font->lfFaceName.size()));
        font = 0;
    }

    if (cf && cf->cf.masks.position) {
        styleText.addProperty("style:text-position",
                              QString("%1%").arg(cf->cf.position),
                              KoGenStyle::TextType);

    } else {
        if (masterCF && masterCF->masks.position) {
            styleText.addProperty("style:text-position",
                                  QString("%1%").arg(masterCF->position),
                                  KoGenStyle::TextType);
        }
    }

    bool underline = false;
    if (cf && cf->cf.masks.underline) {
        underline = cf->cf.fontStyle->underline;
    } else {
        if (masterCF && masterCF->masks.underline) {
            underline = masterCF->fontStyle->underline;
        }
    }

    if (underline) {
        styleText.addProperty("style:text-underline-style",
                              "solid",
                              KoGenStyle::TextType);

        styleText.addProperty("style:text-underline-width",
                              "auto",
                              KoGenStyle::TextType);

        styleText.addProperty("style:text-underline-color",
                              "font-color",
                              KoGenStyle::TextType);
    }

    bool emboss = false;
    if (cf && cf->cf.masks.emboss) {
        emboss = cf->cf.fontStyle->emboss;
    } else {
        if (masterCF && masterCF->masks.emboss) {
            emboss = masterCF->fontStyle->emboss;
        }
    }

    if (emboss) {
        styleText.addProperty("style:font-relief",
                              "embossed",
                              KoGenStyle::TextType);
    }

    bool shadow = false;
    if (cf && cf->cf.masks.shadow) {
        shadow = cf->cf.fontStyle->shadow;
    } else {
        if (masterCF && masterCF->masks.shadow) {
            shadow = masterCF->fontStyle->shadow;
        }
    }

    if (shadow) {
        styleText.addProperty("fo:text-shadow", 0, KoGenStyle::TextType);
    }

    KoGenStyle styleList(KoGenStyle::StyleListAuto, 0);

    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    KoXmlWriter elementWriter(&buffer);    // TODO pass indentation level

    const TextPFException9 *pf9 = 0;
    const StyleTextProp9 *prop9 = 0;

    if (cf) {
        // TODO how does one find this structure?
        prop9 = 0;//textObject->findStyleTextProp9(cf->textCFException());
    }

    for (int i = 0;i < indent + 1;i++) {
        //TextCFException *levelCF = masterTextCFException(type, i);
        const TextPFException *levelPF = masterTextPFException(textType, i);

        if (prop9) {
            pf9 = &prop9->pf9;
        }

        bool isListLevelStyleNumber =
            (levelPF && levelPF->masks.bulletFont)
            || (pf9 && pf9->masks.bulletHasScheme);
        if (isListLevelStyleNumber) {
            elementWriter.startElement("text:list-level-style-number");
        } else {
            elementWriter.startElement("text:list-level-style-bullet");
            // every text:list-level-style-bullet must have a text:bullet-char
            const char bullet[4] = {0xe2, 0x97, 0x8f, 0};
            QString bulletChar(QString::fromUtf8(bullet)); //  "●";
            if (pf && pf->pf.masks.indent && i == pf->pf.indent &&
                    pf->pf.masks.bulletChar) {
                bulletChar = pf->pf.bulletChar;
            } else if (!isListLevelStyleNumber
                       && levelPF && levelPF->masks.bulletChar) {
                bulletChar = levelPF->bulletChar;
            } else {
                QString::fromUtf8(bullet);//  "●";
            }
            elementWriter.addAttribute("text:bullet-char", bulletChar);
        }

        elementWriter.addAttribute("text:level", i + 1);

        if (pf9 && pf9->masks.bulletHasScheme) {
            QString numFormat = 0;
            QString numSuffix = 0;
            QString numPrefix = 0;
            processTextAutoNumberScheme(pf9->bulletAutoNumberScheme->scheme, numFormat, numSuffix,
                                        numPrefix);

            if (numPrefix != 0) {
                elementWriter.addAttribute("style:num-prefix", numPrefix);
            }

            if (numSuffix != 0) {
                elementWriter.addAttribute("style:num-suffix", numSuffix);
            }

            elementWriter.addAttribute("style:num-format", numFormat);
        }

        elementWriter.startElement("style:list-level-properties");

        bool hasIndent = pf && pf->pf.masks.indent && i == pf->pf.indent;
        if (hasIndent && pf->pf.masks.spaceBefore) {
            elementWriter.addAttribute("text:space-before",
                                       paraSpacingToCm(pf->pf.spaceBefore));
        } else if (levelPF && levelPF->masks.spaceBefore) {
            elementWriter.addAttribute("text:space-before",
                                       paraSpacingToCm(levelPF->spaceBefore));
        }
        elementWriter.endElement(); // style:list-level-properties

        elementWriter.startElement("style:text-properties");

        if (hasIndent && pf->pf.masks.bulletFont) {
            font = getFont(pf->pf.bulletFontRef);
        } else if (levelPF && levelPF->masks.bulletFont) {
            font = getFont(levelPF->bulletFontRef);
        }

        if (font) {
            elementWriter.addAttribute("fo:font-family",
                                       QString::fromUtf16(font->lfFaceName.data(),
                                                          font->lfFaceName.size()));
        }

        if (hasIndent && pf->pf.masks.bulletColor) {
            elementWriter.addAttribute("fo:color",
                                       toQColor(*pf->pf.bulletColor).name());
        } else {
            if (levelPF && levelPF->masks.bulletColor) {
                elementWriter.addAttribute("fo:color",
                                           toQColor(*levelPF->bulletColor).name());
            }
        }

        if (hasIndent && pf->pf.masks.bulletSize) {
            elementWriter.addAttribute("fo:font-size",
                                       QString("%1%").arg(pf->pf.bulletSize));
        } else {
            if (levelPF && levelPF->masks.bulletSize) {
                elementWriter.addAttribute("fo:font-size",
                                           QString("%1%").arg(levelPF->bulletSize));
            } else {
                elementWriter.addAttribute("fo:font-size", "100%");
            }
        }

        elementWriter.endElement(); // style:text-properties

        elementWriter.endElement();  // text:list-level-style-bullet


        styleList.addChildElement("text:list-level-style-bullet",
                                  QString::fromUtf8(buffer.buffer(),
                                                    buffer.buffer().size()));
    }

    addStyleNames((cf) ? &cf->cf : 0, (pf) ? &pf->pf : 0,
                  styles.lookup(styleText),
                  styles.lookup(styleParagraph),
                  styles.lookup(styleList));
}

void PptToOdp::processTextAutoNumberScheme(int val, QString& numFormat, QString& numSuffix, QString& numPrefix)
{
    switch (val) {

    case ANM_AlphaLcPeriod:         //Example: a., b., c., ...Lowercase Latin character followed by a period.
        numFormat = "a";
        numSuffix = ".";
        break;

    case ANM_AlphaUcPeriod:        //Example: A., B., C., ...Uppercase Latin character followed by a period.
        numFormat = "A";
        numSuffix = ".";
        break;

    case ANM_ArabicParenRight:     //Example: 1), 2), 3), ...Arabic numeral followed by a closing parenthesis.
        numFormat = "1";
        numSuffix = ")";
        break;

    case ANM_ArabicPeriod :        //Example: 1., 2., 3., ...Arabic numeral followed by a period.
        numFormat = "1";
        numSuffix = ".";
        break;

    case ANM_RomanLcParenBoth:     //Example: (i), (ii), (iii), ...Lowercase Roman numeral enclosed in parentheses.
        numPrefix = "(";
        numFormat = "i";
        numSuffix = ")";
        break;

    case ANM_RomanLcParenRight:    //Example: i), ii), iii), ... Lowercase Roman numeral followed by a closing parenthesis.
        numFormat = "i";
        numSuffix = ")";
        break;

    case ANM_RomanLcPeriod :        //Example: i., ii., iii., ...Lowercase Roman numeral followed by a period.
        numFormat = "i";
        numSuffix = ".";
        break;

    case ANM_RomanUcPeriod:         //Example: I., II., III., ...Uppercase Roman numeral followed by a period.
        numFormat = "I";
        numSuffix = ".";
        break;

    case ANM_AlphaLcParenBoth:      //Example: (a), (b), (c), ...Lowercase alphabetic character enclosed in parentheses.
        numPrefix = "(";
        numFormat = "a";
        numSuffix = ")";
        break;

    case ANM_AlphaLcParenRight:     //Example: a), b), c), ...Lowercase alphabetic character followed by a closing
        numFormat = "a";
        numSuffix = ")";
        break;

    case ANM_AlphaUcParenBoth:      //Example: (A), (B), (C), ...Uppercase alphabetic character enclosed in parentheses.
        numPrefix = "(";
        numFormat = "A";
        numSuffix = ")";
        break;

    case ANM_AlphaUcParenRight:     //Example: A), B), C), ...Uppercase alphabetic character followed by a closing
        numFormat = "A";
        numSuffix = ")";
        break;

    case ANM_ArabicParenBoth:       //Example: (1), (2), (3), ...Arabic numeral enclosed in parentheses.
        numPrefix = "(";
        numFormat = "1";
        numSuffix = ")";
        break;

    case ANM_ArabicPlain:           //Example: 1, 2, 3, ...Arabic numeral.
        numFormat = "1";
        break;

    case ANM_RomanUcParenBoth:      //Example: (I), (II), (III), ...Uppercase Roman numeral enclosed in parentheses.
        numPrefix = "(";
        numFormat = "I";
        numSuffix = ")";
        break;

    case ANM_RomanUcParenRight:     //Example: I), II), III), ...Uppercase Roman numeral followed by a closing parenthesis.
        numFormat = "I";
        numSuffix = ")";
        break;

    default:
        numFormat = "i";
        numSuffix = ".";
        break;
    }
}

void PptToOdp::processTextObjectForStyle(const PPT::OfficeArtSpContainer& o,
        const PPT::TextContainer& tc,
        KoGenStyles &styles)
{
    const StyleTextPropAtom* style  = tc.style.data();
    if (!style) {
        processTextExceptionsForStyle(0, 0, styles, tc);
        return;
    }

    //What paragraph/character exceptions were used last
    const TextPFRun *pf = 0;
    const TextCFRun *cf = 0;

    //TODO this can be easily optimized by calculating proper increments to i
    //from both exception's character count
    QString text = getText(tc);
    for (int i = 0;i < text.length(); i++) {
        if (cf == findTextCFRun(*style, i) && pf == findTextPFRun(*style, i) && i > 0) {
            continue;
        }

        pf = findTextPFRun(*style, i);
        cf = findTextCFRun(*style, i);

        processTextExceptionsForStyle(cf, pf, styles, tc);
    }

    // set the presentation style
    QString styleName;
    KoGenStyle kostyle(KoGenStyle::StyleGraphicAuto, "presentation");
    processGraphicStyle(kostyle, o);
    styleName = styles.lookup(kostyle);
    setGraphicStyleName(o, styleName);
}
namespace
{
const char* dashses[11] = {
    "", "Dash_20_2", "Dash_20_3", "Dash_20_2", "Dash_20_2", "Dash_20_2",
    "Dash_20_4", "Dash_20_6", "Dash_20_5", "Dash_20_7", "Dash_20_8"
};
const char* arrowHeads[6] = {
    "", "msArrowEnd_20_5", "msArrowStealthEnd_20_5", "msArrowDiamondEnd_20_5",
    "msArrowOvalEnd_20_5", "msArrowOpenEnd_20_5"
};
}
template <typename T>
void PptToOdp::processGraphicStyle(KoGenStyle& style, T& o)
{
    /** 2.3.8 Line Style **/
    // 2.3.8.1 lineColor
    const LineColor* lc = get<LineColor>(o);
    if (lc) {
        style.addProperty("svg:stroke-color", toQColor(lc->lineColor).name(),
                          KoGenStyle::GraphicType);
    }
    // 2.3.8.2 lineOpacity
    const LineOpacity* lo = get<LineOpacity>(o);
    if (lo) {
        style.addProperty("svg:stroke-opacity", lo->lineOpacity / 0x10000f);
    }
    // 2.3.8.5 lineType
    // solid, pattern or texture
    // 2.3.8.14 lineWidth
    const LineWidth* lw = get<LineWidth>(o);
    if (lw) {
        style.addProperty("svg:stroke-width",
                          QString("%1pt").arg(lw->lineWidth / 12700.f),
                          KoGenStyle::GraphicType);
    }
    // 2.3.8.16 lineStyle
    // single, double, tiple etc line
    // 2.3.8.17 lineDashing
    const LineDashing* ld = get<LineDashing>(o);

    // This is not nearly complete. left, right, top and bottom lines not yes
    // supported.
    // for now, go by the assumption that there is only a line
    // when fUsefLine and fLine are true
    const LineStyleBooleanProperties* bp = get<LineStyleBooleanProperties>(o);
    if (bp && bp->fUsefLine && bp->fLine) {
        if (bp->fNoLineDrawDash) {
            style.addProperty("draw:stroke", "none", KoGenStyle::GraphicType);
        } else if (ld) {
            if (ld->lineDashing == 0 || ld->lineDashing >= 11) { // solid
                style.addProperty("draw:stroke", "solid", KoGenStyle::GraphicType);
            } else {
                style.addProperty("draw:stroke", "solid", KoGenStyle::GraphicType);
                style.addProperty("draw:stroke-dash", dashses[ld->lineDashing],
                                  KoGenStyle::GraphicType);
            }
        } else {
            // default style is a solid line, this must be set explicitly as long
            // as kpresenter takes draw:stroke="none" as default
            style.addProperty("draw:stroke", "solid", KoGenStyle::GraphicType);
        }
    }
    style.addProperty("draw:stroke", "solid", KoGenStyle::GraphicType);


    const LineStartArrowhead* lsa = get<LineStartArrowhead>(o);
    if (lsa && lsa->lineStartArrowhead > 0 && lsa->lineStartArrowhead < 6) {
        style.addProperty("draw:marker-start",
                          arrowHeads[lsa->lineStartArrowhead], KoGenStyle::GraphicType);
    }

    const LineEndArrowhead* lea = get<LineEndArrowhead>(o);
    if (lea && lea->lineEndArrowhead > 0 && lea->lineEndArrowhead < 6) {
        style.addProperty("draw:marker-end",
                          arrowHeads[lea->lineEndArrowhead], KoGenStyle::GraphicType);
    }

    const LineStartArrowWidth* lsw = get<LineStartArrowWidth>(o);
    if (lw && lsw) {
        style.addProperty("draw:marker-start-width", QString("%1cm").arg(
                              lw->lineWidth*lsw->lineStartArrowWidth), KoGenStyle::GraphicType);
    }

    const LineEndArrowWidth* lew = get<LineEndArrowWidth>(o);
    if (lw && lew) {
        style.addProperty("draw:marker-end-width", QString("%1cm").arg(lw->lineWidth*lew->lineEndArrowWidth), KoGenStyle::GraphicType);
    }

    const FillBlip* fb = get<FillBlip>(o);
    const FillStyleBooleanProperties* fs = get<FillStyleBooleanProperties>(o);
    if (fb) {
        style.addProperty("draw:fill", "bitmap");
        style.addProperty("draw:fill-image-name",
                          "fillImage" + QString::number(fb->fillBlip));
    } else if (fs && fs->fUseFilled) {
        style.addProperty("draw:fill", fs->fFilled ? "solid" : "none",
                          KoGenStyle::GraphicType);
    }
    const FillColor* fc = get<FillColor>(o);
    if (fc) {
        style.addProperty("draw:fill-color", toQColor(fc->fillColor).name(),
                          KoGenStyle::GraphicType);
    } else {
        style.addProperty("draw:fill-color", "#99ccff",
                          KoGenStyle::GraphicType);
    }

#if 0
    if (drawObject->hasProperty("draw:shadow-color")) {
        elementWriter.addAttribute("draw:shadow", "visible");
        Color shadowColor = drawObject->getColorProperty("draw:shadow-color");
        style.addProperty("draw:shadow-color", hexname(shadowColor), KoGenStyle::GraphicType);
    } else {
        style.addProperty("draw:shadow", "hidden", KoGenStyle::GraphicType);
    }
#endif

    const ShadowOpacity* so = get<ShadowOpacity>(o);
    if (so) {
        float opacity = so->shadowOpacity.integral + so->shadowOpacity.fractional / 65535.0;
        style.addProperty("draw:shadow-opacity", QString("%1%").arg(opacity), KoGenStyle::GraphicType);
    }

    const ShadowOffsetX* sox =  get<ShadowOffsetX>(o);
    if (sox) {
        style.addProperty("draw:shadow-offset-x", QString("%1cm").arg(sox->shadowOffsetX), KoGenStyle::GraphicType);
    }

    const ShadowOffsetY* soy =  get<ShadowOffsetY>(o);
    if (soy) {
        style.addProperty("draw:shadow-offset-y", QString("%1cm").arg(soy->shadowOffsetY), KoGenStyle::GraphicType);
    }
}
void PptToOdp::processDrawingObjectForStyle(const PPT::OfficeArtSpContainer& o, KoGenStyles &styles)
{
    KoGenStyle style(KoGenStyle::StyleGraphicAuto, "graphic");
    style.setParentName("pptDefaults");
    processGraphicStyle(style, o);
    setGraphicStyleName(o, styles.lookup(style));
}
