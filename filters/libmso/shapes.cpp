/* This file is part of the KDE project
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
#include "ODrawToOdf.h"
#include "drawstyle.h"

#include <KoXmlWriter.h>
#include <kdebug.h>

#include <QTransform>

#include <cmath>

using namespace MSO;

namespace {
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

void equation(Writer& out, const char* name, const char* formula)
{
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:name", name);
    out.xml.addAttribute("draw:formula", formula);
    out.xml.endElement();
}
}

/**
 * Return the bounding rectangle for this object.
 **/
QRectF
ODrawToOdf::getRect(const OfficeArtSpContainer &o)
{
    if (o.childAnchor) {
        const OfficeArtChildAnchor& r = *o.childAnchor;
        return QRect(r.xLeft, r.yTop, r.xRight - r.xLeft, r.yBottom - r.yTop);
    } else if (o.clientAnchor && client) {
        return client->getRect(*o.clientAnchor);
    }
    return QRect(0, 0, 1, 1);
}

void ODrawToOdf::processEllipse(const OfficeArtSpContainer& o, Writer& out)
{
    out.xml.startElement("draw:ellipse");
    processStyleAndText(o, out);
    out.xml.endElement(); // draw:ellipse
}

void ODrawToOdf::processRectangle(const OfficeArtSpContainer& o, Writer& out)
{
    out.xml.startElement("draw:frame");
    processStyle(o, out);
    out.xml.startElement("draw:text-box");
    processText(o, out);
    out.xml.endElement(); // draw:text-box
    out.xml.endElement(); // draw:frame
}

void ODrawToOdf::processRoundRectangle(const OfficeArtSpContainer& o, Writer& out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:type", "round-rectangle");
    equation(out, "f0", "$0 /3");
    equation(out, "f1", "right-?f0");
    equation(out, "f2", "bottom-?f0");
    equation(out, "f3", "left+?f0");
    equation(out, "f4", "top+?f0");
    out.xml.endElement(); // draw:enhanced-geometry
    out.xml.endElement(); // draw:custom-shape
}

void ODrawToOdf::processDiamond(const OfficeArtSpContainer& o, Writer& out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:type", "diamond");
    out.xml.addAttribute("draw:glue-points", "5 0 0 5 5 10 10 5");
    out.xml.endElement();
    out.xml.endElement();
}

void ODrawToOdf::processTriangle(const OfficeArtSpContainer& o, Writer& out)
{
    /* draw IsocelesTriangle or RightTriangle */
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "5 0 2.5 5 0 10 5 10 10 10 7.5 5");

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
        equation(out, "f0", "$0");
        equation(out, "f1", "$0 /2");
        equation(out, "f2", "?f1 +10800");
        equation(out, "f3", "$0 *2/3");
        equation(out, "f4", "?f3 +7200");
        equation(out, "f5", "21600-?f0");
        equation(out, "f6", "?f5 /2");
        equation(out, "f7", "21600-?f6");
        out.xml.startElement("draw:handle");
        out.xml.addAttribute("draw:handle-range-x-maximum", 21600);
        out.xml.addAttribute("draw:handle-range-x-minimum", 0);
        out.xml.addAttribute("draw:handle-position", "$0 top");
        out.xml.endElement();
    }

    out.xml.endElement();    // enhanced-geometry
    out.xml.endElement(); // custom-shape
}

void ODrawToOdf::processTrapezoid(const OfficeArtSpContainer& o, Writer& out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:type", "trapezoid");
    out.xml.addAttribute("draw:glue-points", "5 0 2.5 5 0 10 5 10");
    if (o.shapeProp.fFlipV) {
        out.xml.addAttribute("draw:mirror-vertical", "true");
    }
    if (o.shapeProp.fFlipH) {
        out.xml.addAttribute("draw:mirror-horizontal", "true");
    }
    equation(out, "f0", "21600-$0");
    equation(out, "f1", "$0");
    equation(out, "f2", "$0 *10/18");
    equation(out, "f3", "?f2 +1750");
    equation(out, "f4", "21600-?f3");
    equation(out, "f5", "$0 /2");
    equation(out, "f6", "21600-?f5");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-range-x-maximum", 10800);
    out.xml.addAttribute("draw:handle-range-x-minimum", 0);
    out.xml.addAttribute("draw:handle-position", "$0 bottom");
    out.xml.endElement();
    out.xml.endElement(); // enhanced-geometry
    out.xml.endElement(); // custom-shape
}

void ODrawToOdf::processParallelogram(const OfficeArtSpContainer& o, Writer& out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:type", "parallelogram");
    out.xml.addAttribute("draw:glue-points", "6.25 0 4.5 0 8.75 5 3.75 10 5 10 1.25 5");
    if (o.shapeProp.fFlipV) {
        out.xml.addAttribute("draw:mirror-vertical", "true");
    }
    if (o.shapeProp.fFlipH) {
        out.xml.addAttribute("draw:mirror-horizontal", "true");
    }
    equation(out, "f0", "$0");
    equation(out, "f1", "21600-$0");
    equation(out, "f2", "$0 *10/24");
    equation(out, "f3", "?f2 +1750");
    equation(out, "f4", "21600-?f3");
    equation(out, "f5", "?f0 /2");
    equation(out, "f6", "10800+?f5");
    equation(out, "f7", "?f0-10800");
    equation(out, "f8", "if(?f7,?f12,0");
    equation(out, "f9", "10800-?f5");
    equation(out, "f10", "if(?f7, ?f12, 21600");
    equation(out, "f11", "21600-?f5");
    equation(out, "f12", "21600*10800/?f0");
    equation(out, "f13", "21600-?f12");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-range-x-maximum", 21600);
    out.xml.addAttribute("draw:handle-range-x-minimum", 0);
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.endElement();
    out.xml.endElement(); // enhanced-geometry
    out.xml.endElement(); // custom-shape
}

void ODrawToOdf::processHexagon(const OfficeArtSpContainer& o, Writer& out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:type", "hexagon");
    out.xml.addAttribute("draw:glue-points", "5 0 0 5 5 10 10 5");
    equation(out, "f0", "$0");
    equation(out, "f1", "21600-$0");
    equation(out, "f2", "$0 *100/234");
    equation(out, "f3", "?f2 +1700");
    equation(out, "f4", "21600-?f3");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-range-x-maximum", 10800);
    out.xml.addAttribute("draw:handle-range-x-minimum", 0);
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.endElement();
    out.xml.endElement(); // enhanced-geometry
    out.xml.endElement(); // custom-shape
}

void ODrawToOdf::processOctagon(const OfficeArtSpContainer& o, Writer& out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:type", "octagon");
    out.xml.addAttribute("draw:glue-points", "5 0 0 4.782 5 10 10 4.782");
    equation(out, "f0", "left+$0");
    equation(out, "f1", "top+$0");
    equation(out, "f2", "right-$0");
    equation(out, "f3", "bottom-$0");
    equation(out, "f4", "$0 /2");
    equation(out, "f5", "left+?f4");
    equation(out, "f6", "top+?f4");
    equation(out, "f7", "right-?f4");
    equation(out, "f8", "bottom-?f4");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-range-x-maximum", 10800);
    out.xml.addAttribute("draw:handle-range-x-minimum", 0);
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.endElement();
    out.xml.endElement(); // enhanced-geometry
    out.xml.endElement(); // custom-shape
}

void ODrawToOdf::processArrow(const OfficeArtSpContainer& o, Writer& out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");

    if (o.shapeProp.rh.recInstance == msosptLeftArrow) {
        if (o.shapeProp.fFlipH)
            out.xml.addAttribute("draw:type", "right-arrow");
        else
            out.xml.addAttribute("draw:type", "left-arrow");
    } else if (o.shapeProp.rh.recInstance == msosptUpArrow) {
        out.xml.addAttribute("draw:type", "up-arrow");
    } else if (o.shapeProp.rh.recInstance == msosptDownArrow) {
        out.xml.addAttribute("draw:type", "down-arrow");
    }
    equation(out, "f0", "$1");
    equation(out, "f1", "$0");
    equation(out, "f2", "21600-$1");
    equation(out, "f3", "21600-?f1");
    equation(out, "f4", "?f3 *?f0 /10800");
    equation(out, "f5", "?f1 +?f4");
    equation(out, "f6", "?f1 *?f0 /10800");
    equation(out, "f7", "?f1 -?f6");
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

void ODrawToOdf::processLine(const OfficeArtSpContainer& o, Writer& out)
{
    const QRectF rect = getRect(o);
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
    out.xml.addAttribute("svg:y1", client->formatPos(out.vOffset(y1)));
    out.xml.addAttribute("svg:y2", client->formatPos(out.vOffset(y2)));
    out.xml.addAttribute("svg:x1", client->formatPos(out.hOffset(x1)));
    out.xml.addAttribute("svg:x2", client->formatPos(out.hOffset(x2)));
    addGraphicStyleToDrawElement(out, o);
    out.xml.addAttribute("draw:layer", "layout");
    processText(o, out);

    out.xml.endElement();
}

void ODrawToOdf::processSmiley(const OfficeArtSpContainer& o, Writer& out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:type", "smiley");
    out.xml.addAttribute("draw:glue-points", "5 0 1.461 1.461 0 5 1.461 8.536 10 5 8.536 1.461");
    equation(out, "f0", "$0-15510");
    equation(out, "f1", "17520-?f0");
    equation(out, "f2", "15510+?f0");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:position", 10800);
    out.xml.addAttribute("draw:handle-range-y-maximum", 17520);
    out.xml.addAttribute("draw:handle-range-y-minimum", 15510);
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.endElement();
    out.xml.endElement(); // enhanced-geometry
    out.xml.endElement(); // custom-shape
}

void ODrawToOdf::processHeart(const OfficeArtSpContainer& o, Writer& out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:type", "heart");
    out.xml.addAttribute("draw:glue-points", "5 1 1.43 5 5 10 8.553 5");

    out.xml.endElement(); // enhanced-geometry
    out.xml.endElement(); // custom-shape
}

void ODrawToOdf::processWedgeRectCallout(const MSO::OfficeArtSpContainer& o, Writer& out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:type", "rectangular-callout");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:glue-points", "10800 0 0 10800 10800 21600 21600 10800 ?f40 ?f41");
    out.xml.addAttribute("draw:text-areas", "0 0 21600 21600");
    out.xml.addAttribute("draw:modifiers", "514 25920");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L 0 3590 ?f2 ?f3 0 8970 0 12630 ?f4 ?f5 0 18010 0 21600 3590 21600 ?f6 ?f7 8970 21600 12630 21600 ?f8 ?f9 18010 21600 21600 21600 21600 18010 ?f10 ?f11 21600 12630 21600 8970 ?f12 ?f13 21600 3590 21600 0 18010 0 ?f14 ?f15 12630 0 8970 0 ?f16 ?f17 3590 0 0 0 Z N");
    equation(out, "f0", "$0 -10800");
    equation(out, "f1", "$1 -10800");
    equation(out, "f2", "if(?f18 ,$0 ,0)");
    equation(out, "f3", "if(?f18 ,$1 ,6280)");
    equation(out, "f4", "if(?f23 ,$0 ,0)");
    equation(out, "f5", "if(?f23 ,$1 ,15320)");
    equation(out, "f6", "if(?f26 ,$0 ,6280)");
    equation(out, "f7", "if(?f26 ,$1 ,21600)");
    equation(out, "f8", "if(?f29 ,$0 ,15320)");
    equation(out, "f9", "if(?f29 ,$1 ,21600)");
    equation(out, "f10", "if(?f32 ,$0 ,21600)");
    equation(out, "f11", "if(?f32 ,$1 ,15320)");
    equation(out, "f12", "if(?f34 ,$0 ,21600)");
    equation(out, "f13", "if(?f34 ,$1 ,6280)");
    equation(out, "f14", "if(?f36 ,$0 ,15320)");
    equation(out, "f15", "if(?f36 ,$1 ,0)");
    equation(out, "f16", "if(?f38 ,$0 ,6280)");
    equation(out, "f17", "if(?f38 ,$1 ,0)");
    equation(out, "f18", "if($0 ,-1,?f19 )");
    equation(out, "f19", "if(?f1 ,-1,?f22 )");
    equation(out, "f20", "abs(?f0 )");
    equation(out, "f21", "abs(?f1 )");
    equation(out, "f22", "?f20 -?f21");
    equation(out, "f23", "if($0 ,-1,?f24 )");
    equation(out, "f24", "if(?f1 ,?f22 ,-1)");
    equation(out, "f25", "$1 -21600");
    equation(out, "f26", "if(?f25 ,?f27 ,-1)");
    equation(out, "f27", "if(?f0 ,-1,?f28 )");
    equation(out, "f28", "?f21 -?f20 ");
    equation(out, "f29", "if(?f25 ,?f30 ,-1)");
    equation(out, "f30", "if(?f0 ,?f28 ,-1)");
    equation(out, "f31", "$0 -21600");
    equation(out, "f32", "if(?f31 ,?f33 ,-1)");
    equation(out, "f33", "if(?f1 ,?f22 ,-1)");
    equation(out, "f34", "if(?f31 ,?f35 ,-1)");
    equation(out, "f35", "if(?f1 ,-1,?f22 )");
    equation(out, "f36", "if($1 ,-1,?f37 )");
    equation(out, "f37", "if(?f0 ,?f28 ,-1)");
    equation(out, "f38", "if($1 ,-1,?f39 )");
    equation(out, "f39", "if(?f0 ,-1,?f28 )");
    equation(out, "f40", "$0");
    equation(out, "f41", "$1");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 $1");
    out.xml.endElement();
    out.xml.endElement();
    out.xml.endElement(); // custom-shape
}

void ODrawToOdf::processWedgeEllipseCallout(const MSO::OfficeArtSpContainer& o, Writer& out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:type", "round-callout");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:glue-points", "10800 0 3160 3160 0 10800 3160 18440 10800 21600 18440 18440 21600 10800 18440 3160 ?f14 ?f15");
    out.xml.addAttribute("draw:text-areas", "3200 3200 18400 18400");
    out.xml.addAttribute("draw:modifiers", "0 11500");
    out.xml.addAttribute("draw:enhanced-path", "W 0 0 21600 21600 ?f22 ?f23 ?f18 ?f19 L ?f14 ?f15 Z N");
    equation(out, "f0", "$0 -10800");
    equation(out, "f1", "$1 -10800");
    equation(out, "f2", "?f0 *?f0");
    equation(out, "f3", "?f1 *?f1");
    equation(out, "f4", "?f2 +?f3");
    equation(out, "f5", "sqrt(?f4 )");
    equation(out, "f6", "?f5 -10800");
    equation(out, "f7", "atan2(?f1 ,?f0 )/(pi/180)");
    equation(out, "f8", "?f7 -10");
    equation(out, "f9", "?f7 +10");
    equation(out, "f10", "10800*cos(?f7 *(pi/180))");
    equation(out, "f11", "10800*sin(?f7 *(pi/180))");
    equation(out, "f12", "?f10 +10800");
    equation(out, "f13", "?f11 +10800");
    equation(out, "f14", "if(?f6 ,$0 ,?f12 )");
    equation(out, "f15", "if(?f6 ,$1 ,?f13 )");
    equation(out, "f16", "10800*cos(?f8 *(pi/180))");
    equation(out, "f17", "10800*sin(?f8 *(pi/180))");
    equation(out, "f18", "?f16 +10800");
    equation(out, "f19", "?f17 +10800");
    equation(out, "f20", "10800*cos(?f9 *(pi/180))");
    equation(out, "f21", "10800*sin(?f9 *(pi/180))");
    equation(out, "f22", "?f20 +10800");
    equation(out, "f23", "?f21 +10800");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 $1");
    out.xml.endElement();
    out.xml.endElement();
    out.xml.endElement(); // custom-shape
}

void ODrawToOdf::processQuadArrow(const OfficeArtSpContainer& o, Writer& out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

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

void ODrawToOdf::processUturnArrow(const MSO::OfficeArtSpContainer& o, Writer& out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:text-areas", "0 8280 6110 21600");
    out.xml.addAttribute("draw:type", "mso-spt101");
    out.xml.addAttribute("draw:enhanced-path", "M 0 21600 L 0 8550 C 0 3540 4370 0 9270 0 13890 0 18570 3230 18600 8300 L 21600 8300 15680 14260 9700 8300 12500 8300 C 12320 6380 10870 5850 9320 5850 7770 5850 6040 6410 6110 8520 L 6110 21600 Z N");
    out.xml.endElement();
    out.xml.endElement(); // draw:custom-shape
}

void ODrawToOdf::processCircularArrow(const MSO::OfficeArtSpContainer &o, Writer &out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:text-areas", "0 0 26110 21600");
    out.xml.addAttribute("draw:type", "circular-arrow");
    out.xml.addAttribute("draw:modifiers", "-130 -80 7200"); // TODO: get from odraw shape
    out.xml.addAttribute("draw:enhanced-path", "B ?f3 ?f3 ?f20 ?f20 ?f19 ?f18 ?f17 ?f16 W 0 0 21600 21600 ?f9 ?f8 ?f11 ?f10 L ?f24 ?f23 ?f47 ?f46 ?f29 ?f28 Z N");
    equation(out, "f0", "$0 ");
    equation(out, "f1", "$1 ");
    equation(out, "f2", "$2 ");
    equation(out, "f3", "10800+$2 ");
    equation(out, "f4", "10800*sin($0 *(pi/180))");
    equation(out, "f5", "10800*cos($0 *(pi/180))");
    equation(out, "f6", "10800*sin($1 *(pi/180))");
    equation(out, "f7", "10800*cos($1 *(pi/180))");
    equation(out, "f8", "?f4 +10800");
    equation(out, "f9", "?f5 +10800");
    equation(out, "f10", "?f6 +10800");
    equation(out, "f11", "?f7 +10800");
    equation(out, "f12", "?f3 *sin($0 *(pi/180))");
    equation(out, "f13", "?f3 *cos($0 *(pi/180))");
    equation(out, "f14", "?f3 *sin($1 *(pi/180))");
    equation(out, "f15", "?f3 *cos($1 *(pi/180))");
    equation(out, "f16", "?f12 +10800");
    equation(out, "f17", "?f13 +10800");
    equation(out, "f18", "?f14 +10800");
    equation(out, "f19", "?f15 +10800");
    equation(out, "f20", "21600-?f3 ");
    equation(out, "f21", "13500*sin($1 *(pi/180))");
    equation(out, "f22", "13500*cos($1 *(pi/180))");
    equation(out, "f23", "?f21 +10800");
    equation(out, "f24", "?f22 +10800");
    equation(out, "f25", "$2 -2700");
    equation(out, "f26", "?f25 *sin($1 *(pi/180))");
    equation(out, "f27", "?f25 *cos($1 *(pi/180))");
    equation(out, "f28", "?f26 +10800");
    equation(out, "f29", "?f27 +10800");
    equation(out, "f30", "?f29 -?f24 ");
    equation(out, "f31", "?f29 -?f24 ");
    equation(out, "f32", "?f30 *?f31 ");
    equation(out, "f33", "?f28 -?f23 ");
    equation(out, "f34", "?f28 -?f23 ");
    equation(out, "f35", "?f33 *?f34 ");
    equation(out, "f36", "?f32 +?f35 ");
    equation(out, "f37", "sqrt(?f36 )");
    equation(out, "f38", "$1 +45");
    equation(out, "f39", "?f37 *sin(?f38 *(pi/180))");
    equation(out, "f40", "$1 +45");
    equation(out, "f41", "?f37 *cos(?f40 *(pi/180))");
    equation(out, "f42", "45");
    equation(out, "f43", "?f39 *sin(?f42 *(pi/180))");
    equation(out, "f44", "45");
    equation(out, "f45", "?f41 *sin(?f44 *(pi/180))");
    equation(out, "f46", "?f28 +?f43 ");
    equation(out, "f47", "?f29 +?f45 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "10800 $0");
    out.xml.addAttribute("draw:handle-polar", "10800 10800");
    out.xml.addAttribute("draw:handle-radius-range-minimum", "10800");
    out.xml.addAttribute("draw:handle-radius-range-maximum", "10800");
    out.xml.endElement();
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$2 $1");
    out.xml.addAttribute("draw:handle-polar", "10800 10800");
    out.xml.addAttribute("draw:handle-radius-range-minimum", "0");
    out.xml.addAttribute("draw:handle-radius-range-maximum", "10800");
    out.xml.endElement();
    out.xml.endElement(); // draw:enhanced-geometry
    out.xml.endElement(); // draw:custom-shape
}

void ODrawToOdf::processFreeLine(const OfficeArtSpContainer& o, Writer& out)
{
    out.xml.startElement("draw:path");
    processStyleAndText(o, out);
    out.xml.endElement(); // path
}

void ODrawToOdf::processPictureFrame(const OfficeArtSpContainer& o, Writer& out)
{
    QString url;
    const Pib* pib = get<Pib>(o);
    if (pib && client) {
        url = client->getPicturePath(pib->pib);
    }
    out.xml.startElement("draw:frame");
    processStyleAndText(o, out);
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
void ODrawToOdf::processDrawingObject(const OfficeArtSpContainer& o, Writer& out)
{
    quint16 shapeType = o.shapeProp.rh.recInstance;
    if (shapeType == msosptEllipse) {
        processEllipse(o, out);
    } else if (shapeType == msosptRectangle
               || shapeType == msosptTextBox) {
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
    } else if (shapeType == msosptWedgeRectCallout) {
        processWedgeRectCallout(o, out);
    } else if (shapeType == msosptWedgeEllipseCallout) {
        processWedgeEllipseCallout(o, out);
    } else if (shapeType == msosptSmileyFace) {
        processSmiley(o, out);
    } else if (shapeType == msosptHeart) {
        processHeart(o, out);
    } else if (shapeType == msosptQuadArrow) {
        processQuadArrow(o, out);
    } else if (shapeType == msosptUturnArrow) {
        processUturnArrow(o, out);
    } else if (shapeType == msosptCircularArrow) {
        processCircularArrow(o, out);
        //} else if (shapeType == msosptMin) {
        //    processFreeLine(o, out);
    } else if (shapeType == msosptPictureFrame
               || shapeType == msosptHostControl) {
        processPictureFrame(o, out);
    } else {
        qDebug() << "cannot handle object of type " << shapeType;
    }
}
void ODrawToOdf::processStyleAndText(const MSO::OfficeArtSpContainer& o,
                                     Writer& out)
{
    processStyle(o, out);
    processText(o, out);
}
void ODrawToOdf::processStyle(const MSO::OfficeArtSpContainer& o,
                                     Writer& out)
{
    addGraphicStyleToDrawElement(out, o);
    out.xml.addAttribute("draw:layer", "layout");
    set2dGeometry(o, out);
}
void ODrawToOdf::processText(const MSO::OfficeArtSpContainer& o,
                                         Writer& out)
{
    if (o.clientData && client && client->onlyClientData(*o.clientData)) {
        client->processClientData(*o.clientData, out);
    } else if (o.clientTextbox) {
        client->processClientTextBox(*o.clientTextbox,
                                 o.clientData.data(), out);
    }
}

void ODrawToOdf::set2dGeometry(const OfficeArtSpContainer& o, Writer& out)
{
    const QRectF rect = getRect(o);

    out.xml.addAttribute("svg:width", client->formatPos(out.hLength(rect.width())));
    out.xml.addAttribute("svg:height", client->formatPos(out.vLength(rect.height())));
 
    const Rotation* rotation = get<Rotation>(o);
    if (rotation) {
        qreal rotationAngle = toQReal(rotation->rotation) / 180 * M_PI;

        QTransform t;
        t.rotateRadians(-rotationAngle);

        QPointF figureCenter(rect.width()/2.0, rect.height()/2.0);

        QPointF originInDocument( rect.x(), rect.y() );

        QPointF rotatedCenterPoint = t.map( figureCenter );

        QPointF translatedPoint( figureCenter - rotatedCenterPoint + originInDocument );

        static const QString transformString("rotate(%1) translate(%2 %3)");

        out.xml.addAttribute("draw:transform", transformString.arg(rotationAngle).arg( client->formatPos(out.hOffset(translatedPoint.x()))).arg(client->formatPos(out.vOffset(translatedPoint.y()))));
    }
    else {
        out.xml.addAttribute("svg:x", client->formatPos(out.hOffset(rect.x())));
        out.xml.addAttribute("svg:y", client->formatPos(out.vOffset(rect.y())));
    }
}
void defineArrow(KoGenStyles& styles)
{
    KoGenStyle marker(KoGenStyle::MarkerStyle);
    marker.addAttribute("draw:display-name", "msArrowEnd 5");
    marker.addAttribute("svg:viewBox", "0 0 210 210");
    marker.addAttribute("svg:d", "m105 0 105 210h-210z");
    styles.insert(marker, "msArrowEnd_20_5", KoGenStyles::DontAddNumberToName);
    // TODO: define proper styles for these arrows
    KoGenStyles::InsertionFlags flags = KoGenStyles::DontAddNumberToName | KoGenStyles::AllowDuplicates;
    styles.insert(marker, "msArrowStealthEnd_20_5", flags);
    styles.insert(marker, "msArrowDiamondEnd_20_5", flags);
    styles.insert(marker, "msArrowOvalEnd_20_5", flags);
    styles.insert(marker, "msArrowOpenEnd_20_5", flags);
}
