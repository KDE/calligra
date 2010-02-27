/* This file is part of the wvWare 2 project
   Copyright (C) 2010 by Nokia

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02111-1307, USA.
*/

#ifndef MS_ODRAW_H
#define MS_ODRAW_H

namespace wvWare
{

typedef enum
{
    msosptNotPrimitive=0,
    msosptRectangle=1,
    msosptRoundRectangle=2,
    msosptEllipse=3,
    msosptDiamond=4,
    msosptIsocelesTriangle=5,
    msosptRightTriangle=6,
    msosptParallelogram=7,
    msosptTrapezoid=8,
    msosptHexagon=9,
    msosptOctagon=0x0a,
    msosptPlus=0x0b,
    msosptStar=0x0c,
    msosptArrow=0x0d,
    msosptThickArrow=0x0e,
    msosptHomePlate=0x0f,

    msosptCube=0x10,
    msosptBaloon=0x11,
    msosptSeal=0x12,
    msosptArc=0x13,
    msosptLine=0x14,
    msosptPlaque=0x15,
    msosptCan=0x16,
    msosptDonut=0x17,
    msosptTextSimple=0x18,
    msosptTextOctagon=0x19,
    msosptTextHexagon=0x1a,
    msosptTextCurve=0x1b,
    msosptTextWave=0x1c,
    msosptTextRing=0x1d,
    msosptTextOnCurve=0x1e,
    msosptTextOnRing=0x1f,

    msosptStraightConnector1=0x20,
    msosptBentConnector2=0x21,
    msosptBentConnector3=0x22,
    msosptBentConnector4=0x23,
    msosptBentConnector5=0x24,
    msosptCurvedConnector2=0x25,
    msosptCurvedConnector3=0x26,
    msosptCurvedConnector4=0x27,
    msosptCurvedConnector5=0x28,
    msosptCallout1=0x29,
    msosptCallout2=0x2a,
    msosptCallout3=0x2b,
    msosptAccentCallout1=0x2c,
    msosptAccentCallout2=0x2d,
    msosptAccentCallout3=0x2e,
    msosptBorderCallout1=0x2f,

    msosptBorderCallout2=0x30,
    msosptBorderCallout3=0x31,
    msosptAccentBorderCallout1=0x32,
    msosptAccentBorderCallout2=0x33,
    msosptAccentBorderCallout3=0x34,
    msosptRibbon=0x35,
    msosptRibbon2=0x36,
    msosptChevron=0x37,
    msosptPentagon=0x38,
    msosptNoSmoking=0x39,
    msosptSeal8=0x3a,
    msosptSeal16=0x3b,
    msosptSeal32=0x3c,
    msosptWedgeRectCallout=0x3d,
    msosptWedgeRRectCalloud=0x3e,
    msosptWedgeEllipseCallout=0x3f,

    // 0x40
    msosptWave=0x40,
    msosptFoldedCorner=0x41,
    msosptLeftArrow=0x42,
    msosptDownArrow=0x43,
    msosptUpArrow=0x44,
    msosptLeftRightArrow=0x45,
    msosptUpDownArrow=0x46,
    msosptIrregularSeal1=0x47,
    msosptIrregularSeal2=0x48,
    msosptLIghtningBolt=0x49,
    msosptHeart=0x4a,
    msosptPictureFrame=0x4b,
    msosptQuadArrow=0x4c,
    msosptLeftArrowCallout=0x4d,
    msosptRightArrowCallout=0x4e,
    msosptUpArrowCallout=0x4f,

    // 0x50
    msosptDownArrowCallout=0x50,
    msosptLeftRightArrowCallout=0x51,
    msosptUpDownArrowCallout=0x52,
    msosptQuadArrowCallout=0x53,
    msosptBevel=0x54,
    msosptLeftBracket=0x55,
    msosptRightBracket=0x56,
    msosptLeftBrace=0x57,
    msosptRightBrace=0x58,
    msosptLeftUpArrow=0x59,
    msosptBentUpArrow=0x5a,
    msosptBentArrow=0x5b,
    msosptSeal24=0x5c,
    msosptStripedRightArrow=0x5d,
    msosptNotchedRightArrow=0x5e,
    msosptBlockArc=0x5f,

    // 0x60
    msosptSmileyFace=0x60,
    msosptVerticalScroll=0x61,
    msosptHorizontalScroll=0x62,
    msosptCircularArrow=0x63,
    msosptNotchedCircularArrow=0x64,
    msosptUturnArrow=0x65,
    msosptCurvedRightArrow=0x66,
    msosptCurvedLeftArrow=0x67,
    msosptCurvedUpArrow=0x68,
    msosptCurvedDownArrow=0x69,
    msosptCloudCallout=0x6a,
    msosptEllipseRibbon=0x6b,
    msosptEllipseRibbon2=0x6c,
    msosptFlowChartProcess=0x6d,
    msosptFlowChartDecision=0x6e,
    msosptFlowChartInputOutput=0x6f,

    // 0x70
    msosptFlowChartPredefinedProcess=0x70,
    msosptFlowChartInternalStorage=0x71,
    msosptFlowChartDocument=0x72,
    msosptFlowChartMultidocument=0x73,
    msosptFlowChartTerminator=0x74,
    msosptFlowChartPreparation=0x75,
    msosptFlowChartManualInput=0x76,
    msosptFlowChartManualOperation=0x77,
    msosptFlowChartConnector=0x78,
    msosptFlowChartPunchedCard=0x79,
    msosptFlowChartPunchedTape=0x7a,
    msosptFlowChartSummingJunction=0x7b,
    msosptFlowChartOr=0x7c,
    msosptFlowChartCollate=0x7d,
    msosptFlowChartSort=0x7e,
    msosptFlowChartExtract=0x7f,


    // 0x80
    msosptFlowChartMerge=0x80,
    msosptFlowChartOfflineStorage=0x81,
    msosptFlowChartOnlineStorage=0x82,
    msosptFlowChartMagneticTape=0x83,
    msosptFlowChartMagneticDisk=0x84,
    msosptMagneticDrum=0x85,
    msosptFlowChartDisplay=0x86,
    msosptFlowChartDelay=0x87,
    msosptTextPlainText=0x88,
    msosptTextStop=0x89,
    msosptTextTriangle=0x8a,
    msosptTextTriangleInverted=0x8b,
    msosptTextChevron=0x8c,
    msosptTextChevronInverted=0x8d,
    msosptTextRingInside=0x8e,
    msosptTextRingOutside=0x8f,

    // 0x90
    msosptTextArchUpCurve=0x90,
    msosptTextArchDownCurve=0x91,
    msosptTextCircleCurve=0x92,
    msosptTextButtonCurve=0x93,
    msosptTextArchUpPour=0x94,
    msosptTextArchDownPour=0x95,
    msosptTextCirclePour=0x96,
    msosptTextButtonPour=0x97,
    msosptTextCurveUp=0x98,
    msosptTextCurveDown=0x99,
    msosptTextCascadeUp=0x9a,
    msosptTextCascadeDown=0x9b,
    msosptTextWave1=0x9c,
    msosptTextWave2=0x9d,
    msosptTextWave3=0x9e,
    msosptTextWave4=0x9f,

    // 0xa0
    msosptTextInflate=0xa0,
    msosptTextDeflate=0xa1,
    msosptTextInflateBottom=0xa2,
    msosptTextDeflateBottom=0xa3,
    msosptTextInflateTop=0xa4,
    msosptTextDeflateTop=0xa5,
    msosptTextDeflateInflate=0xa6,
    msosptTextDeflateInflateDeflate=0xa7,
    msosptTextFadeRight=0xa8,
    msosptTextFadeLeft=0xa9,
    msosptTextFadeUp=0xaa,
    msosptTextFadeDown=0xab,
    msosptTextSlantUp=0xac,
    msosptTextSlantDown=0xad,
    msosptTextCanUp=0xae,
    msosptTextCanDown=0xaf,

    // 0xb0
    msosptFlowChartAlternateprocess=0xb0,
    msosptFlowChartOffpageConnector=0xb1,
    msosptCallout90=0xb2,
    msosptAccentCallout90=0xb3,
    msosptBorderCallout90=0xb4,
    msosptAccentBorderCallout90=0xb5,
    msosptLeftRightUpArrow=0xb6,
    msosptSun=0xb7,
    msosptMoon=0xb8,
    msosptBracketPair=0xb9,
    msosptBracePair=0xba,
    msosptSeal4=0xbb,
    msosptDoubleWave=0xbc,
    msosptActionButtonBlank=0xbd,
    msosptActionButtonHome=0xbe,
    msosptActionButtonHelp=0xbf,

    // 0xc0
    msosptActionButtonInformation=0xc0,
    msosptActionButtonForwardNext=0xc1,
    msosptActionButtonBackPrevious=0xc2,
    msosptActionButtonEnd=0xc3,
    msosptActionButtonBeginning=0xc4,
    msosptActionButtonReturn=0xc5,
    msosptActionButtonDocument=0xc6,
    msosptActionButtonSound=0xc7,
    msosptActionButtonMovie=0xc8,
    msosptHostControl=0xc9,
    msosptTextBox=0xca
} MSOSPT;                   // MSOSPT Enumeration according to MS-ODRAW, page 521 of 621

typedef enum
{
    opidFillCollor        = 0x0181,       // MS-ODRAW, page 193 of 621
    opidFillStyle         = 0x01bf,       // MS-ODRAW, page 218 of 621
    opidLineStyle         = 0x01ff,       // MS-ODRAW, page 240 of 621
    opidPctHR             = 0x0393,       // MS-ODRAW, page 153 of 621
    opidAlignHR           = 0x0394,       // MS-ODRAW, page 153 of 621
    opidDxHeightHR        = 0x0395,       // MS-ODRAW, page 155 of 621
    opidDxWidthHR         = 0x0396,       // MS-ODRAW, page 155 of 621
    opidGroupShapeProps   = 0x03bf        // MS-ODRAW, page 165 of 621
} OPID;

typedef enum
{
    msofillSolid            = 0x00,
    msofillPattern          = 0x01,
    msofillTexture          = 0x02,
    msofillPicture          = 0x03,
    msofillShade            = 0x04,
    msofillShadeCenter      = 0x05,
    msofillShadeShape       = 0x06,
    msofillShadeScale       = 0x07,
    msofillShadeTitle       = 0x08,
    msofillBackground       = 0x09
} MSOFILLTYPE;              // MSOFILLTYPE enumeration according MS-ODRAW, page 512 of 621

typedef enum
{
    hrAlignLeft = 0,
    hrAlignCenter = 1,
    hrAlignRight = 2
} HRALIGN;

typedef struct
{
    U8 r;
    U8 g;
    U8 b;
} OfficeArtCOLORREF;

typedef struct
{
    MSOSPT                  shapeType;

    OfficeArtCOLORREF       color;
    HRALIGN                 align;
    float                   height;
    float                   width;
} OfficeArtProperties;

}


#endif


