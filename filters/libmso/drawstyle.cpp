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
#include "drawstyle.h"

namespace
{
const MSO::OfficeArtCOLORREF white()
{
    MSO::OfficeArtCOLORREF w;
    w.red = w.green = w.blue = 0xFF;
    w.fPaletteIndex = w.fPaletteRGB = w.fSystemRGB = w.fSchemeIndex
                                      = w.fSysIndex = false;
    return w;
}
// The default value for this property is 0x20000000
const MSO::OfficeArtCOLORREF crmodDefault()
{
    MSO::OfficeArtCOLORREF w;
    w.red = w.green = w.blue = 0x00;
    w.fPaletteIndex = w.fPaletteRGB = w.fSchemeIndex = w.fSysIndex = false;
    w.fSystemRGB = true;
    return w;
}
const MSO::OfficeArtCOLORREF black()
{
    MSO::OfficeArtCOLORREF b;
    b.red = b.green = b.blue = 0;
    b.fPaletteIndex = b.fPaletteRGB = b.fSystemRGB = b.fSchemeIndex
                                      = b.fSysIndex = false;
    return b;
}
const MSO::OfficeArtCOLORREF gray()
{
    MSO::OfficeArtCOLORREF b;
    b.red = b.green = b.blue = 0x80;
    b.fPaletteIndex = b.fPaletteRGB = b.fSystemRGB = b.fSchemeIndex
                                      = b.fSysIndex = false;
    return b;
}
const MSO::FixedPoint one()
{
    MSO::FixedPoint one;
    one.integral = 1;
    one.fractional = 0;
    return one;
}
const MSO::FixedPoint zero()
{
    MSO::FixedPoint zero;
    zero.integral = 0;
    zero.fractional = 0;
    return zero;
}
//NOTE: msohadeDefault is not defined in MS-ODRAW, just guessing
// const MSO::MSOSHADETYPE msoshadeDefault() {
//     MSO::MSOSHADETYPE tmp;
//     tmp.msoshadeNone = 0;
//     tmp.msoshadeGamma = 0;
//     tmp.msoshadeSigma = 0;
//     tmp.msoshadeBand = 0;
//     tmp.msoshadeOneColor = 0;
//     return tmp;
// }
}

#define GETTER(TYPE, FOPT, NAME, DEFAULT) \
    TYPE DrawStyle::NAME() const \
    { \
        const MSO::FOPT* p = 0; \
        if (sp) { \
            p = get<MSO::FOPT>(*sp); \
        } \
        if (!p && mastersp) { \
            p = get<MSO::FOPT>(*mastersp); \
        } \
        if (!p) { \
            p = get<MSO::FOPT>(d); \
        } \
        if (p) { \
            return p->NAME; \
        } \
        return DEFAULT; \
    }

//     TYPE                    FOPT                  NAME                  DEFAULT         ODRAW Ref
GETTER(quint32,                FillType,             fillType,             0)              // 2.3.7.1
GETTER(MSO::OfficeArtCOLORREF, FillColor,            fillColor,            white())        // 2.3.7.2
GETTER(MSO::FixedPoint,        FillOpacity,          fillOpacity,          one())          // 2.3.7.3
GETTER(MSO::OfficeArtCOLORREF, FillBackColor,        fillBackColor,        white())        // 2.3.7.4
GETTER(MSO::FixedPoint,        FillBackOpacity,      fillBackOpacity,      one())          // 2.3.7.5
GETTER(MSO::OfficeArtCOLORREF, FillCrMod,            fillCrMod,            crmodDefault()) // 2.3.7.6
GETTER(quint32,                FillBlip,             fillBlip,             0)              // 2.3.7.7
GETTER(quint32,                FillBlipName,         fillBlipName,         0)              // 2.3.7.9
GETTER(quint32,                FillBlipFlags,        fillBlipFlags,        0)              // 2.3.7.11
GETTER(qint32,                 FillWidth,            fillWidth,            0)              // 2.3.7.12
GETTER(qint32,                 FillHeight,           fillHeight,           0)              // 2.3.7.13
GETTER(MSO::FixedPoint,        FillAngle,            fillAngle,            zero())         // 2.3.7.14
GETTER(qint32,                 FillFocus,            fillFocus,            0)              // 2.3.7.15
GETTER(MSO::FixedPoint,        FillToLeft,           fillToLeft,           zero())         // 2.3.7.16
GETTER(MSO::FixedPoint,        FillToTop,            fillToTop,            zero())         // 2.3.7.17
GETTER(MSO::FixedPoint,        FillToRight,          fillToRight,          zero())         // 2.3.7.18
GETTER(MSO::FixedPoint,        FillToBottom,         fillToBottom,         zero())         // 2.3.7.19
GETTER(qint32,                 FillRectLeft,         fillRectLeft,         0)              // 2.3.7.20
GETTER(qint32,                 FillRectTop,          fillRectTop,          0)              // 2.3.7.21
GETTER(qint32,                 FillRectRight,        fillRectRight,        0)              // 2.3.7.22
GETTER(qint32,                 FillRectBottom,       fillRectBottom,       0)              // 2.3.7.23
GETTER(qint32,                 FillDztype,           fillDztype,           0)              // 2.3.7.24
GETTER(qint32,                 FillShadePreset,      fillShadePreset,      0)              // 2.3.7.25
GETTER(quint32,                FillShadeColors,      fillShadeColors,      0)              // 2.3.7.26
GETTER(MSO::FixedPoint,        FillOriginX,          fillOriginX,          0)              // 2.3.7.28
GETTER(MSO::FixedPoint,        FillOriginY,          fillOriginY,          0)              // 2.3.7.29
GETTER(MSO::FixedPoint,        FillShapeOriginX,     fillShapeOriginX,     0)              // 2.3.7.30
GETTER(MSO::FixedPoint,        FillShapeOriginY,     fillShapeOriginY,     0)              // 2.3.7.31
// GETTER(MSO::MSOSHADETYPE,      FillShadeType,        fillShadeType,        msoshadeDefault()) // 2.3.7.32
GETTER(MSO::OfficeArtCOLORREF, FillColorExt,         fillColorExt,         white())        // 2.3.7.33
GETTER(MSO::OfficeArtCOLORREF, FillBackColorExt,     fillBackColorExt,     white())        // 2.3.7.37
GETTER(quint32,                LineEndArrowhead,     lineEndArrowhead,     0)
GETTER(quint32,                LineStartArrowhead,   lineStartArrowhead,   0)
GETTER(quint32,                LineStartArrowWidth,  lineStartArrowWidth,  1)
GETTER(quint32,                LineEndArrowWidth,    lineEndArrowWidth,    1)
GETTER(quint32,                LineWidth,            lineWidth,            0x2535)
GETTER(quint32,                LineDashing,          lineDashing,          0)
GETTER(MSO::OfficeArtCOLORREF, LineColor,            lineColor,            black())
GETTER(qint32,                 LineOpacity,          lineOpacity,          0x10000)
GETTER(quint32,                ShadowType,           shadowType,           0)      // 2.3.13.1
GETTER(MSO::OfficeArtCOLORREF, ShadowColor,          shadowColor,          gray()) // 2.3.13.2
GETTER(MSO::FixedPoint,        ShadowOpacity,        shadowOpacity,        one())  // 2.3.13.5
GETTER(qint32,                 ShadowOffsetX,        shadowOffsetX,        0x6338) // 2.3.13.6
GETTER(qint32,                 ShadowOffsetY,        shadowOffsetY,        0x6338) // 2.3.13.7
GETTER(qint32,                 TxflTextFlow,         txflTextFlow,         0)
GETTER(qint32,                 PosH,                 posH,                 0)
GETTER(qint32,                 PosRelH,              posRelH,              2)
GETTER(qint32,                 PosV,                 posV,                 0)
GETTER(qint32,                 PosRelV,              posRelV,              2)
GETTER(quint32,                PctHR,                pctHR,                0x000003e8)
GETTER(quint32,                AlignHR,              alignHR,              0)
GETTER(qint32,                 DxHeightHR,           dxHeightHR,           0)
GETTER(qint32,                 DxWidthHR,            dxWidthHR,            0)
GETTER(quint32,                PWrapPolygonVertices, pWrapPolygonVertices, 0)
GETTER(qint32,                 DxWrapDistLeft,       dxWrapDistLeft,       0x0001be7c)
GETTER(qint32,                 DyWrapDistTop,        dyWrapDistTop,        0)
GETTER(qint32,                 DxWrapDistRight,      dxWrapDistRight,      0x0001be7c)
GETTER(qint32,                 DyWrapDistBottom,     dyWrapDistBottom,     0)
GETTER(qint32,                 DxTextLeft,           dxTextLeft,           0)
GETTER(qint32,                 DyTextTop,            dyTextTop,            0)
GETTER(qint32,                 DxTextRight,          dxTextRight,          0)
GETTER(qint32,                 DyTextBottom,         dyTextBottom,         0)
GETTER(quint32,                Pib,                  pib,                  0)
GETTER(quint32,                HspMaster,            hspMaster,            0) // 2.3.2.1
#undef GETTER

#define GETTER(NAME, TEST, DEFAULT) \
    bool DrawStyle::NAME() const \
    { \
        const MSO::FOPT* p = 0; \
        if (sp) { \
            p = get<MSO::FOPT>(*sp); \
            if (p && p->TEST) { \
                return p->NAME; \
            } \
        } \
        if (mastersp) { \
            p = get<MSO::FOPT>(*mastersp); \
            if (p && p->TEST) { \
                return p->NAME; \
            } \
        } \
        p = get<MSO::FOPT>(d); \
        if (p && p->TEST) { \
            return p->NAME; \
        } \
        return DEFAULT; \
    }
// FOPT        NAME           TEST                       DEFAULT
#define FOPT FillStyleBooleanProperties
GETTER(fNoFillHitTest,        fUseNoFillHitTest,         false)
GETTER(fillUseRect,           fUseFillUseRect,           false)
GETTER(fillShape,             fUseFillShape,             true)
GETTER(fHitTestFill,          fUseHitTestFill,           true)
GETTER(fFilled,               fUseFilled,                true)
GETTER(fUseShapeAnchor,       fUseUseShapeAnchor,        false)
GETTER(fRecolorFillAsPicture, fUsefRecolorFillAsPicture, false)
#undef FOPT
#define FOPT LineStyleBooleanProperties
GETTER(fNoLineDrawDash,       fUseNoLineDrawDash,        false)
GETTER(fLineFillShape,        fUseLineFillShape,         false)
GETTER(fHitTestLine,          fUseHitTestLine,           true)
GETTER(fLine,                 fUsefLine,                 true)
GETTER(fArrowHeadsOK,         fUsefArrowHeadsOK,         false)
GETTER(fInsetPenOK,           fUseInsetPenOK,            true)
GETTER(fInsetPen,             fUseInsetPen,              false)
GETTER(fLineOpaqueBackColor,  fUsefLineOpaqueBackColor,  false)
#undef FOPT
#define FOPT GroupShapeBooleanProperties
GETTER(fPrint,                fUsefPrint,                true)
GETTER(fHidden,               fUsefHidden,               false)
GETTER(fOneD,                 fUsefOneD,                 false)
GETTER(fIsButton,             fUsefIsButton,             false)
GETTER(fOnDblClickNotify,     fUsefOnDblClickNotify,     false)
GETTER(fBehindDocument,       fUsefBehindDocument,       false)
GETTER(fEditedWrap,           fUsefEditedWrap,           false)
GETTER(fScriptAnchor,         fUsefScriptAnchor,         false)
GETTER(fReallyHidden,         fUsefReallyHidden,         false)
GETTER(fAllowOverlap,         fUsefAllowOverlap,         true)
GETTER(fUserDrawn,            fUsefUserDrawn,            false)
GETTER(fHorizRule,            fUsefHorizRule,            false)
GETTER(fNoshadeHR,            fUsefNoshadeHR,            false)
GETTER(fStandardHR,           fUsefStandardHR,           false)
GETTER(fIsBullet,             fUsefIsBullet,             false)
GETTER(fLayoutInCell,         fUsefLayoutInCell,         true)
#undef FOPT
#define FOPT ShadowStyleBooleanProperties
GETTER(fShadowObscured,       fUsefShadowObscured,       false)
GETTER(fShadow,               fUsefShadow,               false)
#undef FOPT

#define COMPLEX(FOPT, NAME) \
    IMsoArray DrawStyle::NAME() const \
    { \
        IMsoArray a;\
        if (sp) { \
            a = getComplexData<MSO::FOPT>(*sp); \
            return a;\
        } \
        if (mastersp) { \
            a = getComplexData<MSO::FOPT>(*mastersp); \
            return a;\
        } \
        return a;\
    }
//FOPT                //NAME
COMPLEX(FillShadeColors,      fillShadeColors_complex)
COMPLEX(PVertices,            pVertices_complex)
COMPLEX(PSegmentInfo,         pSegmentInfo_complex)
COMPLEX(PWrapPolygonVertices, pWrapPolygonVertices_complex)

#undef COMPLEX
