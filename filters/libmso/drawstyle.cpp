/* This file is part of the KDE project
   Copyright (C) 2010 KO GmbH <jos.van.den.oever@kogmbh.com>
   Copyright (C) 2010, 2011 Matus Uzak <matus.uzak@ixonos.com>

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
#include "msodraw.h"

namespace
{
const MSO::OfficeArtCOLORREF ignore()
{
    MSO::OfficeArtCOLORREF w;
    w.red = w.green = w.blue = 0xFF;
    w.fPaletteIndex = w.fPaletteRGB = w.fSystemRGB = w.fSchemeIndex
                                      = w.fSysIndex = true;
    return w;
}
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
} //namespace

quint16 DrawStyle::shapeType() const
{
    if (!sp) {
        return msosptNil;
    } else {
        return sp->shapeProp.rh.recInstance;
    }
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
        if (!p && d) { \
            p = get<MSO::FOPT>(*d); \
        } \
        if (p) { \
            return p->NAME; \
        } \
        return DEFAULT; \
    }

//     TYPE                    FOPT                  NAME                  DEFAULT         ODRAW Ref
GETTER(quint32,                HspMaster,            hspMaster,            0)              // 2.3.2.1
GETTER(quint32,                Cxstyle,              cxstyle,              0x00000003)     // 2.3.2.2
GETTER(quint32,                BWMode,               bWMode,               1)              // 2.3.2.3
GETTER(quint32,                PWrapPolygonVertices, pWrapPolygonVertices, 0)              // 2.3.4.7
GETTER(qint32,                 DxWrapDistLeft,       dxWrapDistLeft,       0x0001be7c)     // 2.3.4.9
GETTER(qint32,                 DyWrapDistTop,        dyWrapDistTop,        0)              // 2.3.4.10
GETTER(qint32,                 DxWrapDistRight,      dxWrapDistRight,      0x0001be7c)     // 2.3.4.11
GETTER(qint32,                 DyWrapDistBottom,     dyWrapDistBottom,     0)              // 2.3.4.12
GETTER(quint32,                LidRegroup,           lidRegroup,           0)              // 2.3.4.13
GETTER(quint32,                PosH,                 posH,                 0)              // 2.3.4.19
GETTER(quint32,                PosRelH,              posRelH,              2)              // 2.3.4.20
GETTER(quint32,                PosV,                 posV,                 0)              // 2.3.4.21
GETTER(quint32,                PosRelV,              posRelV,              2)              // 2.3.4.22
GETTER(quint32,                PctHR,                pctHR,                0x000003e8)     // 2.3.4.23
GETTER(quint32,                AlignHR,              alignHR,              0)              // 2.3.4.24
GETTER(qint32,                 DxHeightHR,           dxHeightHR,           0)              // 2.3.4.25
GETTER(qint32,                 DxWidthHR,            dxWidthHR,            0)              // 2.3.4.26
GETTER(MSO::OfficeArtCOLORREF, BorderTopColor,       borderTopColor,       white())        // 2.3.4.32
GETTER(MSO::OfficeArtCOLORREF, BorderLeftColor,      borderLeftColor,      white())        // 2.3.4.33
GETTER(MSO::OfficeArtCOLORREF, BorderBottomColor,    borderBottomColor,    white())        // 2.3.4.34
GETTER(MSO::OfficeArtCOLORREF, BorderRightColor,     borderRightColor,     white())        // 2.3.4.35
GETTER(qint32,                 GeoLeft,              geoLeft,              0)              // 2.3.6.1
GETTER(qint32,                 GeoTop,               geoTop,               0)              // 2.3.6.2
GETTER(qint32,                 GeoRight,             geoRight,             0x00005460)     // 2.3.6.3
GETTER(qint32,                 GeoBottom,            geoBottom,            0x00005460)     // 2.3.6.4
GETTER(quint32,                ShapePath,            shapePath,            0x00000001)     // 2.3.6.5
GETTER(qint32,                 AdjustValue,          adjustvalue,          0)              // 2.3.6.10
GETTER(qint32,                 Adjust2Value,         adjust2value,         0)              // 2.3.6.11
GETTER(qint32,                 Adjust3Value,         adjust3value,         0)              // 2.3.6.12
GETTER(qint32,                 Adjust4Value,         adjust4value,         0)              // 2.3.6.13
GETTER(qint32,                 Adjust5Value,         adjust5value,         0)              // 2.3.6.14
GETTER(qint32,                 Adjust6Value,         adjust6value,         0)              // 2.3.6.15
GETTER(qint32,                 Adjust7Value,         adjust7value,         0)              // 2.3.6.16
GETTER(qint32,                 Adjust8Value,         adjust8value,         0)              // 2.3.6.17
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
GETTER(MSO::FixedPoint,        FillOriginX,          fillOriginX,          zero())         // 2.3.7.28
GETTER(MSO::FixedPoint,        FillOriginY,          fillOriginY,          zero())         // 2.3.7.29
GETTER(MSO::FixedPoint,        FillShapeOriginX,     fillShapeOriginX,     zero())         // 2.3.7.30
GETTER(MSO::FixedPoint,        FillShapeOriginY,     fillShapeOriginY,     zero())         // 2.3.7.31
// GETTER(MSO::MSOSHADETYPE,      FillShadeType,        fillShadeType,        msoshadeDefault()) // 2.3.7.32
GETTER(MSO::OfficeArtCOLORREF, FillColorExt,         fillColorExt,         white())        // 2.3.7.33
GETTER(MSO::OfficeArtCOLORREF, FillBackColorExt,     fillBackColorExt,     white())        // 2.3.7.37
GETTER(MSO::OfficeArtCOLORREF, LineColor,            lineColor,            black())        // 2.3.8.1
GETTER(qint32,                 LineOpacity,          lineOpacity,          0x10000)        // 2.3.8.2
GETTER(quint32,                LineWidth,            lineWidth,            0x2535)         // 2.3.8.14
GETTER(quint32,                LineDashing,          lineDashing,          0)              // 2.3.8.17
GETTER(quint32,                LineStartArrowhead,   lineStartArrowhead,   0)              // 2.3.8.20
GETTER(quint32,                LineEndArrowhead,     lineEndArrowhead,     0)              // 2.3.8.21
GETTER(quint32,                LineStartArrowWidth,  lineStartArrowWidth,  1)              // 2.3.8.22
GETTER(quint32,                LineEndArrowWidth,    lineEndArrowWidth,    1)              // 2.3.8.24
GETTER(quint32,                LineEndArrowLength,   lineEndArrowLength,   1)              // 2.3.8.25
GETTER(quint32,                LineJoinStyle,        lineJoinStyle,        2)              // 2.3.8.26
GETTER(quint32,                LineEndCapStyle,      lineEndCapStyle,      2)              // 2.3.8.27
GETTER(quint32,                ShadowType,           shadowType,           0)              // 2.3.13.1
GETTER(MSO::OfficeArtCOLORREF, ShadowColor,          shadowColor,          gray())         // 2.3.13.2
GETTER(MSO::FixedPoint,        ShadowOpacity,        shadowOpacity,        one())          // 2.3.13.5
GETTER(qint32,                 ShadowOffsetX,        shadowOffsetX,        0x6338)         // 2.3.13.6
GETTER(qint32,                 ShadowOffsetY,        shadowOffsetY,        0x6338)         // 2.3.13.7
GETTER(MSO::FixedPoint,        Rotation,             rotation,             zero())         // 2.3.18.5
GETTER(qint32,                 ITxid,                iTxid,                0)              // 2.3.21.1
GETTER(qint32,                 DxTextLeft,           dxTextLeft,           0x00016530)     // 2.3.21.2
GETTER(qint32,                 DyTextTop,            dyTextTop,            0x0000B298)     // 2.3.21.3
GETTER(qint32,                 DxTextRight,          dxTextRight,          0x00016530)     // 2.3.21.4
GETTER(qint32,                 DyTextBottom,         dyTextBottom,         0x0000B298)     // 2.3.21.5
GETTER(quint32,                WrapText,             wrapText,             0)              // 2.3.21.6
GETTER(quint32,                AnchorText,           anchorText,           0)              // 2.3.21.8
GETTER(quint32,                TxflTextFlow,         txflTextFlow,         0)              // 2.3.21.9
GETTER(quint32,                CdirFont,             cdirFont,             0)              // 2.3.21.10
GETTER(quint32,                HspNext,              hspNext,              0)              // 2.3.21.11
GETTER(quint32,                Txdir,                txdir,                0)              // 2.3.21.12
GETTER(MSO::FixedPoint,        CropFromTop,          cropFromTop,          zero())         // 2.3.23.1
GETTER(MSO::FixedPoint,        CropFromBottom,       cropFromBottom,       zero())         // 2.3.23.2
GETTER(MSO::FixedPoint,        CropFromLeft,         cropFromLeft,         zero())         // 2.3.23.3
GETTER(MSO::FixedPoint,        CropFromRight,        cropFromRight,        zero())         // 2.3.23.4
GETTER(quint32,                Pib,                  pib,                  0)              // 2.3.23.5
GETTER(quint32,                PibName,              pibName,              0)              // 2.3.23.7
GETTER(quint32,                PibFlags,             pibFlags,             0)              // 2.3.23.9
GETTER(MSO::OfficeArtCOLORREF, PictureTransparent,   pictureTransparent,   ignore())       // 2.3.23.10
GETTER(qint32,                 PictureContrast,      pictureContrast,      0x00010000)     // 2.3.23.11
GETTER(qint32,                 PictureBrightness,    pictureBrightness,    0)              // 2.3.23.12
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
        if (d) { \
            p = get<MSO::FOPT>(d); \
            if (p && p->TEST) { \
                return p->NAME; \
            } \
        } \
        return DEFAULT; \
    }

//TODO: CalloutBooleanProperties, ProtectionBooleanProperties

// FOPT        NAME           TEST                       DEFAULT
#define FOPT ShapeBooleanProperties
GETTER(fBackground,           fUsefBackground,           false)
GETTER(fInitiator,            fUsefInitiator,            false)
GETTER(fLockShapeType,        fUsefLockShapeType,        false)
GETTER(fPreferRelativeResize, fusePreferrelativeResize,  false)
GETTER(fOleIcon,              fUsefOleIcon,              false)
GETTER(fFlipVOverride,        fUsefFlipVOverride,        false)
GETTER(fFlipHOverride,        fUsefFlipHOverride,        false)
GETTER(fPolicyBarcode,        fUsefPolicyBarcode,        false)
GETTER(fPolicyLabel,          fUsefPolicyLabel,          false)
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
#define FOPT GeometryBooleanProperties
GETTER(fFillOk,                fUsefFillOK,              true)
GETTER(fFillShadeShapeOK,      fUsefFillShadeShapeOK,    false)
GETTER(fGtextOK,               fUsefGtextOK,             false)
GETTER(fLineOK,                fUsefLineOK,              true)
GETTER(f3DOK,                  fUsef3DOK,                true)
GETTER(fShadowOK,              fUsefShadowOK,            true)
#undef FOPT
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
// GETTER(fLine,                 fUsefLine,                 true)
GETTER(fArrowHeadsOK,         fUsefArrowHeadsOK,         false)
GETTER(fInsetPenOK,           fUseInsetPenOK,            true)
GETTER(fInsetPen,             fUseInsetPen,              false)
GETTER(fLineOpaqueBackColor,  fUsefLineOpaqueBackColor,  false)
#undef FOPT
#define FOPT ShadowStyleBooleanProperties
GETTER(fShadowObscured,       fUsefShadowObscured,       false)
GETTER(fShadow,               fUsefShadow,               false)
#undef FOPT
#define FOPT DiagramBooleanProperties
GETTER(fPseudoInline,         fUsefPseudoInline,         false)
GETTER(fDoLayout,             fUsefDoLayout,             true)
GETTER(fReverse,              fUsefReverse,              false)
GETTER(fDoFormat,             fUsefDoFormat,             false)
#undef FOPT
#define FOPT TextBooleanProperties
GETTER(fFitShapeToText,       fUsefFitShapeToText,       false)
GETTER(fAutoTextMargin,       fUsefAutoTextMargin,       false)
GETTER(fSelectText,           fUsefSelectText,           true)
#undef FOPT
#define FOPT BlipBooleanProperties
GETTER(fPictureActive,        fUsefPictureActive,        false)
GETTER(fPictureBiLevel,       fUsefPictureBiLevel,       false)
GETTER(fPictureGray,          fUsefPictureGray,          false)
GETTER(fNoHitTestPicture,     fUsefNoHitTestPicture,     false)
GETTER(fLooping,              fUsefLooping,              false)
GETTER(fRewind,               fUsefRewind,               false)
GETTER(fPicturePreserveGrays, fUsefPicturePreserveGrays, false)
#undef FOPT
#undef GETTER

// The override was discussed at Office File Formats Forum:
// http://social.msdn.microsoft.com/Forums/en-US/os_binaryfile/thread/a1cf51a7-fb93-4028-b3ac-3ed2fd77a94b
bool DrawStyle::fLine() const
{
    const MSO::LineStyleBooleanProperties* p = 0;
    quint16 shapeType = msosptNil;

    if (sp) {
        shapeType = sp->shapeProp.rh.recInstance;
        p = get<MSO::LineStyleBooleanProperties>(*sp);
        if (p && p->fUsefLine) {
            return p->fLine;
        }
    }
    if (mastersp) {
        p = get<MSO::LineStyleBooleanProperties>(*mastersp);
        if (p && p->fUsefLine) {
            return p->fLine;
        }
    }
    if (shapeType == msosptPictureFrame) {
        return false;
    } else {
        return true;
    }
}

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
// FOPT                       NAME
COMPLEX(FillShadeColors,      fillShadeColors_complex)
COMPLEX(PVertices,            pVertices_complex)
COMPLEX(PSegmentInfo,         pSegmentInfo_complex)
COMPLEX(PWrapPolygonVertices, pWrapPolygonVertices_complex)
#undef COMPLEX

#define COMPLEX_NAME(FOPT, NAME) \
    QString DrawStyle::NAME() const \
    { \
        QString a;\
        if (sp) { \
            a = getComplexName<MSO::FOPT>(*sp); \
            if (!a.isNull()) return a; \
        } \
        if (mastersp) { \
            a = getComplexName<MSO::FOPT>(*mastersp); \
            if (!a.isNull()) return a; \
        } \
        return a;\
    }
// FOPT                       NAME
COMPLEX_NAME(PibName,         pibName_complex)
COMPLEX_NAME(FillBlipName,    fillBlipName_complex)
#undef COMPLEX_NAME
