#include "PptToOdp.h"

#include <KoXmlWriter.h>
#include <kdebug.h>

#include <QMatrix>

#include <cmath>

using namespace MSO;

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
};

void PptToOdp::processEllipse(const OfficeArtSpContainer& o, Writer& out)
{
    const QRect rect = getRect(o);
    out.xml.startElement("draw:ellipse");
    addGraphicStyleToDrawElement(out, o);
    out.xml.addAttribute("draw:layer", "layout");
    set2dGeometry(o, out);
    out.xml.endElement(); // draw:ellipse
}

void PptToOdp::processRectangle(const OfficeArtSpContainer& o, Writer& out)
{
    const QRect rect = getRect(o);
    out.xml.startElement("draw:rect");
    addGraphicStyleToDrawElement(out, o);
    out.xml.addAttribute("draw:layer", "layout");
    set2dGeometry(o, out);

    out.xml.endElement(); // draw:rect
}

void PptToOdp::processRoundRectangle(const OfficeArtSpContainer& o, Writer& out)
{
    const QRect rect = getRect(o);
    out.xml.startElement("draw:custom-shape");
    addGraphicStyleToDrawElement(out, o);
    out.xml.addAttribute("draw:layer", "layout");
    set2dGeometry(o, out);

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
    addGraphicStyleToDrawElement(out, o);
    set2dGeometry(o, out);
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
    out.xml.addAttribute("draw:type", "diamond");
    out.xml.endElement();
    out.xml.endElement();
}

void PptToOdp::processTriangle(const OfficeArtSpContainer& o, Writer& out)
{
    const QRect rect = getRect(o);
    /* draw IsocelesTriangle or RightTriangle */
    out.xml.startElement("draw:custom-shape");
    addGraphicStyleToDrawElement(out, o);
    out.xml.addAttribute("draw:layer", "layout");
    set2dGeometry(o, out);

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
    addGraphicStyleToDrawElement(out, o);
    out.xml.addAttribute("draw:layer", "layout");
    set2dGeometry(o, out);

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
    addGraphicStyleToDrawElement(out, o);
    out.xml.addAttribute("draw:layer", "layout");
    set2dGeometry(o, out);

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
    addGraphicStyleToDrawElement(out, o);
    out.xml.addAttribute("draw:layer", "layout");
    set2dGeometry(o, out);

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
    addGraphicStyleToDrawElement(out, o);
    out.xml.addAttribute("draw:layer", "layout");
    set2dGeometry(o, out);

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
    addGraphicStyleToDrawElement(out, o);
    out.xml.addAttribute("draw:layer", "layout");
    set2dGeometry(o, out);

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
    qreal x1 = rect.x();
    qreal y1 = rect.y();
    qreal x2 = rect.x() + rect.width();
    qreal y2 = rect.y() + rect.height();

    if (o.shapeProp.fFlipV) {
        qSwap(y1, y2);
    }
    if (o.shapeProp.fFlipH) {
        qSwap(x1, x2);
    }

    out.xml.startElement("draw:line");
    addGraphicStyleToDrawElement(out, o);
    out.xml.addAttribute("svg:y1", out.vOffset(y1));
    out.xml.addAttribute("svg:y2", out.vOffset(y2));
    out.xml.addAttribute("svg:x1", out.hOffset(x1));
    out.xml.addAttribute("svg:x2", out.hOffset(x2));
    out.xml.addAttribute("draw:layer", "layout");

    out.xml.endElement();
}

void PptToOdp::processSmiley(const OfficeArtSpContainer& o, Writer& out)
{
    const QRect rect = getRect(o);
    out.xml.startElement("draw:custom-shape");
    addGraphicStyleToDrawElement(out, o);
    out.xml.addAttribute("draw:layer", "layout");
    set2dGeometry(o, out);

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
    addGraphicStyleToDrawElement(out, o);
    out.xml.addAttribute("draw:layer", "layout");
    set2dGeometry(o, out);

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

void PptToOdp::processQuadArrow(const OfficeArtSpContainer& o, Writer& out)
{
    const QRect rect = getRect(o);
    out.xml.startElement("draw:custom-shape");
    addGraphicStyleToDrawElement(out, o);
    out.xml.addAttribute("draw:layer", "layout");
    set2dGeometry(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:text-areas", "0 0 21600 21600");
    out.xml.addAttribute("draw:type", "quad-arrow");
    out.xml.addAttribute("draw:modifiers", "6500 8600 4300");
    out.xml.addAttribute("draw:enhanced-path", "M 0 10800 L ?f0 ?f1 ?f0 ?f2 ?f2 ?f2 ?f2 ?f0 ?f1 ?f0 10800 0 ?f3 ?f0 ?f4 ?f0 ?f4 ?f2 ?f5 ?f2 ?f5 ?f1 21600 10800 ?f5 ?f3 ?f5 ?f4 ?f4 ?f4 ?f4 ?f5 ?f3 ?f5 10800 21600 ?f1 ?f5 ?f2 ?f5 ?f2 ?f4 ?f0 ?f4 ?f0 ?f3 Z N");
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:name", "f0");
    out.xml.addAttribute("draw:formula", "$2");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:name", "f1");
    out.xml.addAttribute("draw:formula", "$0");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:name", "f2");
    out.xml.addAttribute("draw:formula", "$1");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:name", "f3");
    out.xml.addAttribute("draw:formula", "21600-$0");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:name", "f4");
    out.xml.addAttribute("draw:formula", "21600-$1");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:name", "f5");
    out.xml.addAttribute("draw:formula", "21600-$2");
    out.xml.endElement();
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$1 $2");
    out.xml.addAttribute("draw:handle-range-x-minimum", "$0");
    out.xml.addAttribute("draw:handle-range-x-maximum", "10800");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "$0");
    out.xml.endElement();
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-minimum", "$2");
    out.xml.addAttribute("draw:handle-range-x-maximum", "$1");
    out.xml.endElement();
    out.xml.endElement();
    out.xml.endElement(); // draw:custom-shape
}

void PptToOdp::processUturnArrow(const MSO::OfficeArtSpContainer& o, Writer& out)
{
    const QRect rect = getRect(o);
    out.xml.startElement("draw:custom-shape");
    addGraphicStyleToDrawElement(out, o);
    out.xml.addAttribute("draw:layer", "layout");
    set2dGeometry(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:text-areas", "0 8280 6110 21600");
    out.xml.addAttribute("draw:type", "mso-spt101");
    out.xml.addAttribute("draw:enhanced-path", "M 0 21600 L 0 8550 C 0 3540 4370 0 9270 0 13890 0 18570 3230 18600 8300 L 21600 8300 15680 14260 9700 8300 12500 8300 C 12320 6380 10870 5850 9320 5850 7770 5850 6040 6410 6110 8520 L 6110 21600 Z N");
    out.xml.endElement();
    out.xml.endElement(); // draw:custom-shape
}

void PptToOdp::processFreeLine(const OfficeArtSpContainer& o, Writer& out)
{
    const QRect rect = getRect(o);
    out.xml.startElement("draw:path");
    addGraphicStyleToDrawElement(out, o);
    out.xml.addAttribute("draw:layer", "layout");
    set2dGeometry(o, out);
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
    QString url;
    const Pib* pib = get<Pib>(o);
    if (pib) {
        url = getPicturePath(pib->pib);
    }
    out.xml.startElement("draw:frame");
    addGraphicStyleToDrawElement(out, o);
    out.xml.addAttribute("draw:layer", "layout");
    set2dGeometry(o, out);
    if (url.isEmpty()) {
        // if the image cannot be found, just place an empty frame
        out.xml.endElement(); // frame
        return;
    }
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
    } else if (shapeType == msosptQuadArrow) {
        processQuadArrow(o, out);
    } else if (shapeType == msosptUturnArrow) {
        processUturnArrow(o, out);
        //} else if (shapeType == msosptMin) {
        //    processFreeLine(o, out);
    } else if (shapeType == msosptPictureFrame
               || shapeType == msosptHostControl) {
        processPictureFrame(o, out);
    } else if (shapeType == msosptTextBox) {
        qDebug() << "what's my name!' " << o.shapeProp.rh.recInstance;
        //processTextObjectForBody(o, , out);
    } else {
        qDebug() << "cannot handle object of type " << o.shapeProp.rh.recInstance;
    }
}

void PptToOdp::set2dGeometry(const OfficeArtSpContainer& o, Writer& out)
{
    const QRect rect = getRect(o);

    out.xml.addAttribute("svg:width", out.hLength(rect.width()));
    out.xml.addAttribute("svg:height", out.vLength(rect.height()));
 
    const Rotation* rotation = get<Rotation>(o);
    if (rotation) {
        //FIXME: I get wrong angle values, aren't they in radians?
        //FIXME: check weather how the degrees are measured (might need a to multiply by -1)
        qreal rotationAngle = toQReal(rotation->rotation);

        QMatrix matrix(cos(rotationAngle), -sin(rotationAngle), sin(rotationAngle), cos(rotationAngle), 0, 0);

        QPointF figureCenter(rect.width()/2.0, rect.height()/2.0);

        QPointF origenInDocument( rect.x(), rect.y() );

        qreal rotX = 0.0;
        qreal rotY = 0.0;

        matrix.map( figureCenter.x(), figureCenter.y(), &rotX, &rotY);

        QPointF rotatedCenterPoint( rotX, rotY );

        QPointF translatedPoint( figureCenter - rotatedCenterPoint + origenInDocument );

        static const QString transformString("rotate(%1) translate(%2 %3)");

        out.xml.addAttribute("draw:transform", transformString.arg(rotationAngle).arg( out.hOffset(translatedPoint.x())).arg(out.vOffset(translatedPoint.y())));
    }
    else {
        out.xml.addAttribute("svg:x", out.hOffset(rect.x()));
        out.xml.addAttribute("svg:y", out.vOffset(rect.y()));
    }
}
void defineArrow(KoGenStyles& styles)
{
    KoGenStyle marker(KoGenStyle::StyleMarker);
    marker.addAttribute("draw:display-name", "msArrowEnd 5");
    marker.addAttribute("svg:viewBox", "0 0 210 210");
    marker.addAttribute("svg:d", "m105 0 105 210h-210z");
    styles.lookup(marker, "msArrowEnd_20_5", KoGenStyles::DontForceNumbering);
    // TODO: define proper styles for these arrows
    int flags = KoGenStyles::DontForceNumbering | KoGenStyles::AllowDuplicates;
    styles.lookup(marker, "msArrowStealthEnd_20_5", flags);
    styles.lookup(marker, "msArrowDiamondEnd_20_5", flags);
    styles.lookup(marker, "msArrowOvalEnd_20_5", flags);
    styles.lookup(marker, "msArrowOpenEnd_20_5", flags);
}
