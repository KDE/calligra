/* This file is part of the Calligra project
   Copyright (C) 2010, 2011 Matus Uzak <matus.uzak@ixonos.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the Library GNU General Public
   version 2 of the License, or (at your option) version 3 or,
   at the discretion of KDE e.V (which shall act as a proxy as in
   section 14 of the GPLv3), any later version..

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef MSODRAW_H
#define MSODRAW_H

/**
 * The MSOSPT enumeration specifies the preset shapes and preset text shape
 * geometries that will be used for a shape.  An enumeration of this type is
 * used so that a custom geometry does not need to be specified but can instead
 * be automatically constructed by the generating application.
 * [MS-ODRAW] — v20101219
 */
enum MSOSPT
{
    msosptMin = 0x0, //libmso specific
    msosptNotPrimitive = 0x0,
    msosptRectangle = 0x1,
    msosptRoundRectangle = 0x2,
    msosptEllipse = 0x3,
    msosptDiamond = 0x4,
    msosptIsocelesTriangle = 0x5,
    msosptRightTriangle = 0x6,
    msosptParallelogram = 0x7,
    msosptTrapezoid = 0x8,
    msosptHexagon = 0x9,
    msosptOctagon = 0x0a,
    msosptPlus = 0x0b,
    msosptStar = 0x0c,
    msosptArrow = 0x0d,
    msosptThickArrow = 0x0e,
    msosptHomePlate = 0x0f,

    msosptCube = 0x10,
    msosptBaloon = 0x11,
    msosptSeal = 0x12,
    msosptArc = 0x13,
    msosptLine = 0x14,
    msosptPlaque = 0x15,
    msosptCan = 0x16,
    msosptDonut = 0x17,
    msosptTextSimple = 0x18,
    msosptTextOctagon = 0x19,
    msosptTextHexagon = 0x1a,
    msosptTextCurve = 0x1b,
    msosptTextWave = 0x1c,
    msosptTextRing = 0x1d,
    msosptTextOnCurve = 0x1e,
    msosptTextOnRing = 0x1f,

    msosptStraightConnector1 = 0x20,
    msosptBentConnector2 = 0x21,
    msosptBentConnector3 = 0x22,
    msosptBentConnector4 = 0x23,
    msosptBentConnector5 = 0x24,
    msosptCurvedConnector2 = 0x25,
    msosptCurvedConnector3 = 0x26,
    msosptCurvedConnector4 = 0x27,
    msosptCurvedConnector5 = 0x28,
    msosptCallout1 = 0x29,
    msosptCallout2 = 0x2a,
    msosptCallout3 = 0x2b,
    msosptAccentCallout1 = 0x2c,
    msosptAccentCallout2 = 0x2d,
    msosptAccentCallout3 = 0x2e,
    msosptBorderCallout1 = 0x2f,

    msosptBorderCallout2 = 0x30,
    msosptBorderCallout3 = 0x31,
    msosptAccentBorderCallout1 = 0x32,
    msosptAccentBorderCallout2 = 0x33,
    msosptAccentBorderCallout3 = 0x34,
    msosptRibbon = 0x35,
    msosptRibbon2 = 0x36,
    msosptChevron = 0x37,
    msosptPentagon = 0x38,
    msosptNoSmoking = 0x39,
    msosptSeal8 = 0x3a,
    msosptSeal16 = 0x3b,
    msosptSeal32 = 0x3c,
    msosptWedgeRectCallout = 0x3d,
    msosptWedgeRRectCallout = 0x3e,
    msosptWedgeEllipseCallout = 0x3f,

    msosptWave = 0x40,
    msosptFoldedCorner = 0x41,
    msosptLeftArrow = 0x42,
    msosptDownArrow = 0x43,
    msosptUpArrow = 0x44,
    msosptLeftRightArrow = 0x45,
    msosptUpDownArrow = 0x46,
    msosptIrregularSeal1 = 0x47,
    msosptIrregularSeal2 = 0x48,
    msosptLightningBolt = 0x49,
    msosptHeart = 0x4a,
    msosptPictureFrame = 0x4b,
    msosptQuadArrow = 0x4c,
    msosptLeftArrowCallout = 0x4d,
    msosptRightArrowCallout = 0x4e,
    msosptUpArrowCallout = 0x4f,

    msosptDownArrowCallout = 0x50,
    msosptLeftRightArrowCallout = 0x51,
    msosptUpDownArrowCallout = 0x52,
    msosptQuadArrowCallout = 0x53,
    msosptBevel = 0x54,
    msosptLeftBracket = 0x55,
    msosptRightBracket = 0x56,
    msosptLeftBrace = 0x57,
    msosptRightBrace = 0x58,
    msosptLeftUpArrow = 0x59,
    msosptBentUpArrow = 0x5a,
    msosptBentArrow = 0x5b,
    msosptSeal24 = 0x5c,
    msosptStripedRightArrow = 0x5d,
    msosptNotchedRightArrow = 0x5e,
    msosptBlockArc = 0x5f,

    msosptSmileyFace = 0x60,
    msosptVerticalScroll = 0x61,
    msosptHorizontalScroll = 0x62,
    msosptCircularArrow = 0x63,
    msosptNotchedCircularArrow = 0x64,
    msosptUturnArrow = 0x65,
    msosptCurvedRightArrow = 0x66,
    msosptCurvedLeftArrow = 0x67,
    msosptCurvedUpArrow = 0x68,
    msosptCurvedDownArrow = 0x69,
    msosptCloudCallout = 0x6a,
    msosptEllipseRibbon = 0x6b,
    msosptEllipseRibbon2 = 0x6c,
    msosptFlowChartProcess = 0x6d,
    msosptFlowChartDecision = 0x6e,
    msosptFlowChartInputOutput = 0x6f,

    msosptFlowChartPredefinedProcess = 0x70,
    msosptFlowChartInternalStorage = 0x71,
    msosptFlowChartDocument = 0x72,
    msosptFlowChartMultidocument = 0x73,
    msosptFlowChartTerminator = 0x74,
    msosptFlowChartPreparation = 0x75,
    msosptFlowChartManualInput = 0x76,
    msosptFlowChartManualOperation = 0x77,
    msosptFlowChartConnector = 0x78,
    msosptFlowChartPunchedCard = 0x79,
    msosptFlowChartPunchedTape = 0x7a,
    msosptFlowChartSummingJunction = 0x7b,
    msosptFlowChartOr = 0x7c,
    msosptFlowChartCollate = 0x7d,
    msosptFlowChartSort = 0x7e,
    msosptFlowChartExtract = 0x7f,

    msosptFlowChartMerge = 0x80,
    msosptFlowChartOfflineStorage = 0x81,
    msosptFlowChartOnlineStorage = 0x82,
    msosptFlowChartMagneticTape = 0x83,
    msosptFlowChartMagneticDisk = 0x84,
    msosptFlowChartMagneticDrum = 0x85,
    msosptFlowChartDisplay = 0x86,
    msosptFlowChartDelay = 0x87,
    msosptTextPlainText = 0x88,
    msosptTextStop = 0x89,
    msosptTextTriangle = 0x8a,
    msosptTextTriangleInverted = 0x8b,
    msosptTextChevron = 0x8c,
    msosptTextChevronInverted = 0x8d,
    msosptTextRingInside = 0x8e,
    msosptTextRingOutside = 0x8f,

    msosptTextArchUpCurve = 0x90,
    msosptTextArchDownCurve = 0x91,
    msosptTextCircleCurve = 0x92,
    msosptTextButtonCurve = 0x93,
    msosptTextArchUpPour = 0x94,
    msosptTextArchDownPour = 0x95,
    msosptTextCirclePour = 0x96,
    msosptTextButtonPour = 0x97,
    msosptTextCurveUp = 0x98,
    msosptTextCurveDown = 0x99,
    msosptTextCascadeUp = 0x9a,
    msosptTextCascadeDown = 0x9b,
    msosptTextWave1 = 0x9c,
    msosptTextWave2 = 0x9d,
    msosptTextWave3 = 0x9e,
    msosptTextWave4 = 0x9f,

    msosptTextInflate = 0xa0,
    msosptTextDeflate = 0xa1,
    msosptTextInflateBottom = 0xa2,
    msosptTextDeflateBottom = 0xa3,
    msosptTextInflateTop = 0xa4,
    msosptTextDeflateTop = 0xa5,
    msosptTextDeflateInflate = 0xa6,
    msosptTextDeflateInflateDeflate = 0xa7,
    msosptTextFadeRight = 0xa8,
    msosptTextFadeLeft = 0xa9,
    msosptTextFadeUp = 0xaa,
    msosptTextFadeDown = 0xab,
    msosptTextSlantUp = 0xac,
    msosptTextSlantDown = 0xad,
    msosptTextCanUp = 0xae,
    msosptTextCanDown = 0xaf,

    msosptFlowChartAlternateProcess = 0xb0,
    msosptFlowChartOffpageConnector = 0xb1,
    msosptCallout90 = 0xb2,
    msosptAccentCallout90 = 0xb3,
    msosptBorderCallout90 = 0xb4,
    msosptAccentBorderCallout90 = 0xb5,
    msosptLeftRightUpArrow = 0xb6,
    msosptSun = 0xb7,
    msosptMoon = 0xb8,
    msosptBracketPair = 0xb9,
    msosptBracePair = 0xba,
    msosptSeal4 = 0xbb,
    msosptDoubleWave = 0xbc,
    msosptActionButtonBlank = 0xbd,
    msosptActionButtonHome = 0xbe,
    msosptActionButtonHelp = 0xbf,

    msosptActionButtonInformation = 0xc0,
    msosptActionButtonForwardNext = 0xc1,
    msosptActionButtonBackPrevious = 0xc2,
    msosptActionButtonEnd = 0xc3,
    msosptActionButtonBeginning = 0xc4,
    msosptActionButtonReturn = 0xc5,
    msosptActionButtonDocument = 0xc6,
    msosptActionButtonSound = 0xc7,
    msosptActionButtonMovie = 0xc8,
    msosptHostControl = 0xc9,
    msosptTextBox = 0xca,
    msosptNil = 0x0FFF //libmso specific
};

/**
 * The MSOLINESTYLE enumeration specifies the type of line style that will be
 * used.  [MS-ODRAW] — v20101219
 */
enum MSOLINESTYLE
{
    msolineSimple,      //0x00  A simple line.
    msolineDouble,      //0x01  A double line.
    msolineThickThin,   //0x02  A thick line and a thin line.
    msolineThinThick,   //0x03  A thin line and a thick line.
    msolineTripl        //0x04  A triple line.
};

/**
 * The MSOLINEEND enumeration specifies the line end decorations that
 * appear at the ends of lines.
 */
enum MSOLINEEND
{
    msolineNoEnd,
    msolineArrowEnd,
    msolineArrowStealthEnd,
    msolineArrowDiamondEnd,
    msolineArrowOvalEnd,
    msolineArrowOpenEnd,
    msolineArrowChevronEnd,
    msolineArrowDoubleChevronEnd
};

/**
 * The MSOLINECAP enumeration specifies ways to cap the ends of lines
 * in the document.
 */
enum MSOLINECAP
{
    msolineEndCapRound,
    msolineEndCapSquare,
    msolineEndCapFlat
};

/**
 * The MSOLINEJOIN enumeration specifies the types of join styles that
 * can be applied to lines in the document.
 */
enum MSOLINEJOIN
{
    msolineJoinBevel,
    msolineJoinMiter,
    msolineJoinRound
};

/**
 * The MSOLINEDASHING enumeration, as shown in the following table, specifies
 * preset dashed-line values. Each style corresponds to a precise binary
 * representation of the repeating dash style. Each 1 corresponds to a line
 * segment, and each 0 corresponds to a space.  [MS-ODRAW] — v20101219
 */
enum MSOLINEDASHING
{
    msolineSolid,             //0x00  1
    msolineDashSys,           //0x01  1110
    msolineDotSys,            //0x02  10
    msolineDashDotSys,        //0x03  111010
    msolineDashDotDotSys,     //0x04  11101010
    msolineDotGEL,            //0x05  1000
    msolineDashGEL,           //0x06  1111000
    msolineLongDashGEL,       //0x07  11111111000
    msolineDashDotGEL,        //0x08  11110001000
    msolineLongDashDotGEL,    //0x09  111111110001000
    msolineLongDashDotDotGEL  //0x0A  1111111100010001000
};

/**
 * The MSOFILLTYPE enumeration specifies the fill types.
 * [MS-ODRAW] — v20101219
 */
enum MSOFILLTYPE
{
    msofillSolid = 0x0,
    msofillPattern = 0x1,
    msofillTexture = 0x2,
    msofillPicture = 0x3,
    msofillShade = 0x4,
    msofillShadeCenter = 0x5,
    msofillShadeShape = 0x6,
    msofillShadeScale = 0x7,
    msofillShadeTitle = 0x8,
    msofillBackground = 0x9
};

/**
 * The MSOPATHTYPE enumeration specifies how the individual pieces of a path
 * SHOULD be interpreted.  [MS-ODRAW] — v20101219
 */
enum MSOPATHTYPE
{
    msopathLineTo = 0,
    msopathCurveTo,
    msopathMoveTo,
    msopathClose,
    msopathEnd,
    msopathEscape,
    msopathClientEscape
};

/**
 * The MSOWRAPMODE enumeration specifies how text wraps 
 * SHOULD be interpreted.  [MS-ODRAW] — v20101219
 */
enum MSOWRAPMODE
{
    msowrapSquare = 0,
    msowrapByPoints,
    msowrapNone
};

/**
 * The MSOANCHOR enumeration specifies the suggested placement rule for a body
 * of text.  These enumeration values are relative to the orientation, text box
 * area, and margin sizes of the containing shape.  The exact placement of the
 * text is application dependent and varies to accommodate other languages and
 * text properties.  These enumeration values MAY be used.
 * [MS-ODRAW] — v20101219
 */
enum MSOANCHOR
{
    msoanchorTop = 0x0,
    msoanchorMiddle = 0x1,
    msoanchorBottom = 0x2,
    msoanchorTopCentered = 0x3,
    msoanchorMiddleCentered = 0x4,
    msoanchorBottomCentered = 0x5,
    msoanchorTopBaseline = 0x6,
    msoanchorBottomBaseline = 0x7,
    msoanchorTopCenteredBaseline = 0x8,
    msoanchorBottomCenteredBaseline = 0x9
};

/**
 * The POSH enumeration specifies the type of horizontal positioning to use for
 * a shape.  [MS-ODRAW] — v20101219
 */
enum POSH
{
    msophAbs = 0x0,
    msophLeft = 0x1,
    msophCenter = 0x2,
    msophRight = 0x3,
    msophInside = 0x4,
    msophOutside = 0x5
};

/**
 * The POSRELH enumeration specifies a page element relative to which a shape
 * is horizontally positioned.  [MS-ODRAW] — v20101219
 */
enum POSRELH
{
// NOTE: Based on our tests the enumeration should start from ZERO.
    msoprhMargin = 0x1,
    msoprhPage = 0x2,
    msoprhText = 0x3,
    msoprhChar = 0x4
};

/**
 * The POSV enumeration specifies the type of vertical positioning to use for a
 * shape.  [MS-ODRAW] — v20101219
 */
enum POSV
{
    msopvAbs = 0x0,
    msopvTop = 0x1,
    msopvCenter = 0x2,
    msopvBottom = 0x3,
    msopvInside = 0x4,
    msopvOutside = 0x5
};

/**
 * The POSRELV enumeration specifies a page element relative to which a shape
 * is vertically positioned.  [MS-ODRAW] — v20101219
 */

enum POSRELV
{
// NOTE: Based on our tests the enumeration should start from ZERO.
    msoprvMargin = 0x1,
    msoprvPage = 0x2,
    msoprvText = 0x3,
    msoprvLine = 0x4
};

#endif
