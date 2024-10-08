/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <jos.van.den.oever@kogmbh.com>
   SPDX-FileCopyrightText: 2010, 2011 Matus Uzak <matus.uzak@ixonos.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DRAWSTYLE_H
#define DRAWSTYLE_H

#include "generated/simpleParser.h"

class IMsoArray
{
public:
    quint16 nElems;
    quint16 nElemsAlloc;
    quint16 cbElem;
    QByteArray data;
    IMsoArray()
        : nElems(0)
        , nElemsAlloc(0)
        , cbElem(0)
    {
    }
};

class DrawStyle
{
private:
    const MSO::OfficeArtDggContainer *d;
    const MSO::OfficeArtSpContainer *mastersp;
    const MSO::OfficeArtSpContainer *sp;

public:
    explicit DrawStyle(const MSO::OfficeArtDggContainer *d_ = nullptr,
                       const MSO::OfficeArtSpContainer *mastersp_ = nullptr,
                       const MSO::OfficeArtSpContainer *sp_ = nullptr)
        : d(d_)
        , mastersp(mastersp_)
        , sp(sp_)
    {
    }

    /**
     * @return the OfficeArtSpContainer record specifying the shape container.
     */
    const MSO::OfficeArtSpContainer *shapeContainer() const
    {
        return sp;
    };

    /**
     * @return the shape type that MUST be an MSOSPT enumeration value.
     */
    quint16 shapeType() const;

    // Shape property set
    quint32 hspMaster() const;
    quint32 cxstyle() const;
    quint32 bWMode() const;
    // Shape Boolean Properties
    bool fBackground() const;
    bool fInitiator() const;
    bool fLockShapeType() const;
    bool fPreferRelativeResize() const;
    bool fOleIcon() const;
    bool fFlipVOverride() const;
    bool fFlipHOverride() const;
    bool fPolicyBarcode() const;
    bool fPolicyLabel() const;

    // Group Shape property set
    quint32 pWrapPolygonVertices() const;
    IMsoArray pWrapPolygonVertices_complex() const;
    qint32 dxWrapDistLeft() const;
    qint32 dyWrapDistTop() const;
    qint32 dxWrapDistRight() const;
    qint32 dyWrapDistBottom() const;
    quint32 lidRegroup() const;
    quint32 posH() const;
    quint32 posRelH() const;
    quint32 posV() const;
    quint32 posRelV() const;
    quint32 pctHR() const;
    quint32 alignHR() const;
    qint32 dxHeightHR() const;
    qint32 dxWidthHR() const;
    MSO::OfficeArtCOLORREF borderTopColor() const;
    MSO::OfficeArtCOLORREF borderLeftColor() const;
    MSO::OfficeArtCOLORREF borderBottomColor() const;
    MSO::OfficeArtCOLORREF borderRightColor() const;
    // Group Shape Boolean Properties
    bool fPrint() const;
    bool fHidden() const;
    bool fOneD() const;
    bool fIsButton() const;
    bool fOnDblClickNotify() const;
    bool fBehindDocument() const;
    bool fEditedWrap() const;
    bool fScriptAnchor() const;
    bool fReallyHidden() const;
    bool fAllowOverlap() const;
    bool fUserDrawn() const;
    bool fHorizRule() const;
    bool fNoshadeHR() const;
    bool fStandardHR() const;
    bool fIsBullet() const;
    bool fLayoutInCell() const;
    bool fUsefPrint() const;
    bool fUsefHidden() const;
    bool fUsefOneD() const;
    bool fUsefIsButton() const;
    bool fUsefOnDblClickNotify() const;
    bool fUsefBehindDocument() const;
    bool fUsefEditedWrap() const;
    bool fUsefScriptAnchor() const;
    bool fUsefReallyHidden() const;
    bool fUsefAllowOverlap() const;
    bool fUsefUserDrawn() const;
    bool fUsefHorizRule() const;
    bool fUsefNoshadeHR() const;
    bool fUsefStandardHR() const;
    bool fUsefIsBullet() const;
    bool fUsefLayoutInCell() const;

    // Geometry property set
    qint32 geoLeft() const;
    qint32 geoTop() const;
    qint32 geoRight() const;
    qint32 geoBottom() const;
    quint32 shapePath() const;
    IMsoArray pVertices_complex() const;
    IMsoArray pSegmentInfo_complex() const;
    qint32 adjustvalue() const;
    qint32 adjust2value() const;
    qint32 adjust3value() const;
    qint32 adjust4value() const;
    qint32 adjust5value() const;
    qint32 adjust6value() const;
    qint32 adjust7value() const;
    qint32 adjust8value() const;
    // Geometry Boolean Properties
    bool fFillOk() const;
    bool fFillShadeShapeOK() const;
    bool fGtextOK() const;
    bool fLineOK() const;
    bool f3DOK() const;
    bool fShadowOK() const;

    // Fill Style property set
    quint32 fillType() const;
    MSO::OfficeArtCOLORREF fillColor() const;
    MSO::FixedPoint fillOpacity() const;
    MSO::OfficeArtCOLORREF fillBackColor() const;
    MSO::FixedPoint fillBackOpacity() const;
    MSO::OfficeArtCOLORREF fillCrMod() const;
    quint32 fillBlip() const;
    quint32 fillBlipName() const;
    QString fillBlipName_complex() const;
    quint32 fillBlipFlags() const;
    qint32 fillWidth() const;
    qint32 fillHeight() const;
    qint32 fillFocus() const;
    MSO::FixedPoint fillAngle() const;
    MSO::FixedPoint fillToLeft() const;
    MSO::FixedPoint fillToTop() const;
    MSO::FixedPoint fillToRight() const;
    MSO::FixedPoint fillToBottom() const;
    qint32 fillRectLeft() const;
    qint32 fillRectTop() const;
    qint32 fillRectRight() const;
    qint32 fillRectBottom() const;
    qint32 fillDztype() const;
    qint32 fillShadePreset() const;
    quint32 fillShadeColors() const;
    IMsoArray fillShadeColors_complex() const;
    MSO::FixedPoint fillOriginX() const;
    MSO::FixedPoint fillOriginY() const;
    MSO::FixedPoint fillShapeOriginX() const;
    MSO::FixedPoint fillShapeOriginY() const;
    /*     MSO::MSOSHADETYPE fillShadeType() const; */
    MSO::OfficeArtCOLORREF fillColorExt() const;
    MSO::OfficeArtCOLORREF fillBackColorExt() const;
    // Fill Style Boolean Properties
    bool fNoFillHitTest() const;
    bool fillUseRect() const;
    bool fillShape() const;
    bool fHitTestFill() const;
    bool fFilled() const;
    bool fUseShapeAnchor() const;
    bool fRecolorFillAsPicture() const;

    // Line Style property set
    MSO::OfficeArtCOLORREF lineColor() const;
    qint32 lineOpacity() const;
    quint32 lineWidth() const;
    quint32 lineDashing() const;
    quint32 lineStartArrowhead() const;
    quint32 lineEndArrowhead() const;
    quint32 lineStartArrowWidth() const;
    quint32 lineEndArrowWidth() const;
    quint32 lineEndArrowLength() const;
    quint32 lineJoinStyle() const;
    quint32 lineEndCapStyle() const;
    // Line Style Boolean Properties
    bool fNoLineDrawDash() const;
    bool fLineFillShape() const;
    bool fHitTestLine() const;
    bool fLine() const;
    bool fArrowHeadsOK() const;
    bool fInsetPenOK() const;
    bool fInsetPen() const;
    bool fLineOpaqueBackColor() const;

    // Shadow Style property set
    quint32 shadowType() const;
    MSO::OfficeArtCOLORREF shadowColor() const;
    MSO::FixedPoint shadowOpacity() const;
    qint32 shadowOffsetX() const;
    qint32 shadowOffsetY() const;
    // Shadow Style Boolean Properties
    bool fShadowObscured() const;
    bool fShadow() const;

    // Diagram property set
    // Diagram Boolean Properties
    bool fPseudoInline() const;
    bool fDoLayout() const;
    bool fReverse() const;
    bool fDoFormat() const;

    // Transformation property set
    MSO::FixedPoint rotation() const;

    // Text property set
    qint32 iTxid() const;
    qint32 dxTextLeft() const;
    qint32 dyTextTop() const;
    qint32 dxTextRight() const;
    qint32 dyTextBottom() const;
    quint32 wrapText() const;
    quint32 anchorText() const;
    quint32 txflTextFlow() const;
    quint32 cdirFont() const;
    quint32 hspNext() const;
    quint32 txdir() const;
    // Text Boolean Properties
    bool fFitShapeToText() const;
    bool fAutoTextMargin() const;
    bool fSelectText() const;

    // Blip property set
    MSO::FixedPoint cropFromTop() const;
    MSO::FixedPoint cropFromBottom() const;
    MSO::FixedPoint cropFromLeft() const;
    MSO::FixedPoint cropFromRight() const;
    quint32 pib() const;
    quint32 pibName() const;
    QString pibName_complex() const;
    quint32 pibFlags() const;
    MSO::OfficeArtCOLORREF pictureTransparent() const;
    qint32 pictureContrast() const;
    qint32 pictureBrightness() const;
    // Blip Boolean Properties
    bool fPictureActive() const;
    bool fPictureBiLevel() const;
    bool fPictureGray() const;
    bool fNoHitTestPicture() const;
    bool fLooping() const;
    bool fRewind() const;
    bool fPicturePreserveGrays() const;
};

/**
 * Retrieve an option from an options containing class B
 *
 * @p b must have a member fopt that is an array of type OfficeArtFOPTEChoice.
 * A is the type of the required option. The option containers in PPT/DOC have
 * only one instance of each option in an option container.
 * @param b class that contains options.
 * @return pointer to the option of type A or 0 if there is none.
 */
template<typename A, typename B>
const A *get(const B &b)
{
    foreach (const MSO::OfficeArtFOPTEChoice &a, b.fopt) {
        const A *ptr = a.anon.get<A>();
        if (ptr)
            return ptr;
    }
    return nullptr;
}
/**
 * Retrieve an option from an OfficeArtSpContainer
 *
 * Look in all option containers in @p o for an option of type A.
 * @param o OfficeArtSpContainer instance which contains options.
 * @return pointer to the option of type A or 0 if there is none.
 */
template<typename A>
const A *get(const MSO::OfficeArtSpContainer &o)
{
    const A *a = nullptr;
    if (o.shapePrimaryOptions)
        a = get<A>(*o.shapePrimaryOptions);
    if (!a && o.shapeSecondaryOptions1)
        a = get<A>(*o.shapeSecondaryOptions1);
    if (!a && o.shapeSecondaryOptions2)
        a = get<A>(*o.shapeSecondaryOptions2);
    if (!a && o.shapeTertiaryOptions1)
        a = get<A>(*o.shapeTertiaryOptions1);
    if (!a && o.shapeTertiaryOptions2)
        a = get<A>(*o.shapeTertiaryOptions2);
    return a;
}
/**
 * Retrieve an option from an OfficeArtDggContainer
 *
 * Look in all option containers in @p o for an option of type A.
 * @param o OfficeArtDggContainer instance which contains options.
 * @return pointer to the option of type A or 0 if there is none.
 */
template<typename A>
const A *get(const MSO::OfficeArtDggContainer &o)
{
    const A *a = nullptr;
    if (o.drawingPrimaryOptions) {
        a = get<A>(*o.drawingPrimaryOptions);
    }
    if (!a && o.drawingTertiaryOptions)
        a = get<A>(*o.drawingTertiaryOptions);
    return a;
}
/**
 * Retrieve an option from a container
 *
 * Look in all option containers in @p o for an option of type A.
 * @param o OfficeArtDggContainer instance which contains options.
 * @return pointer to the option of type A or 0 if there is none.
 */
template<typename A, typename T>
const A *get(const T *o)
{
    return (o) ? get<A>(*o) : nullptr;
}
/**
 * Retrieve the complex data from an options containing class B
 *
 * @p b must have a member fopt that is an array of type OfficeArtFOPTEChoice.
 * A is the type of the required option.  The option containers in PPT/DOC have
 * only one instance of each option in an option container.
 *
 * @param b class that contains options.
 * @return IMsoArray storing complex data
 */
template<typename A, typename B>
IMsoArray getComplexData(const B &b)
{
    MSO::OfficeArtFOPTE *p = nullptr;
    IMsoArray a;
    const char *pData = b.complexData.data();
    uint offset = 0;

    foreach (const MSO::OfficeArtFOPTEChoice &_c, b.fopt) {
        p = (MSO::OfficeArtFOPTE *)_c.anon.data();
        if (p->opid.fComplex) {
            // there is wrong offset inside PVertices
            if (_c.anon.is<MSO::PVertices>()) {
                if (_c.anon.get<A>()) {
                    if (b.complexData.size() - offset >= 6) {
                        memcpy(&a.nElems, pData + offset, sizeof(quint16));
                        memcpy(&a.nElemsAlloc, pData + offset + 2, sizeof(quint16));
                        memcpy(&a.cbElem, pData + offset + 4, sizeof(quint16));
                        a.data = b.complexData.mid(offset + 6, p->op);
                        break;
                    }
                } else {
                    offset += p->op + 6;
                }
            } else {
                if (_c.anon.get<A>()) {
                    if (b.complexData.size() - offset >= 6) {
                        memcpy(&a.nElems, pData + offset, sizeof(quint16));
                        memcpy(&a.nElemsAlloc, pData + offset + 2, sizeof(quint16));
                        memcpy(&a.cbElem, pData + offset + 4, sizeof(quint16));
                        a.data = b.complexData.mid(offset + 6, p->op - 6);
                        break;
                    }
                } else {
                    offset += p->op;
                }
            }
        }
    }
    return a;
}

/**
 * Retrieve the complex data, which represent an IMsoArray for an option from
 * an OfficeArtSpContainer.
 *
 * Look in all option containers in @p o for an option of type A.
 *
 * @param o OfficeArtSpContainer instance which contains options
 * @return IMsoArray storing complex data
 */
template<typename A>
IMsoArray getComplexData(const MSO::OfficeArtSpContainer &o)
{
    IMsoArray a;
    if (o.shapePrimaryOptions)
        a = getComplexData<A>(*o.shapePrimaryOptions);
    if (!a.data.size() && o.shapeSecondaryOptions1)
        a = getComplexData<A>(*o.shapeSecondaryOptions1);
    if (!a.data.size() && o.shapeSecondaryOptions2)
        a = getComplexData<A>(*o.shapeSecondaryOptions2);
    if (!a.data.size() && o.shapeTertiaryOptions1)
        a = getComplexData<A>(*o.shapeTertiaryOptions1);
    if (!a.data.size() && o.shapeTertiaryOptions2)
        a = getComplexData<A>(*o.shapeTertiaryOptions2);
    return a;
}

/**
 * Retrieve the complex data, which represent a null-terminated unicode string
 * from an options containing class B.
 *
 * @p b must have a member fopt that is an array of type OfficeArtFOPTEChoice.
 * A is the type of the required option.  The option containers in PPT/DOC have
 * only one instance of each option in an option container.
 *
 * @param b class that contains options.
 * @return QString storing complex data
 */
template<typename A, typename B>
QString getComplexName(const B &b)
{
    MSO::OfficeArtFOPTE *p = nullptr;
    uint offset = 0;
    QString a;

    foreach (const MSO::OfficeArtFOPTEChoice &_c, b.fopt) {
        p = (MSO::OfficeArtFOPTE *)_c.anon.data();
        if (p->opid.fComplex) {
            if (_c.anon.get<A>()) {
                a.append(b.complexData.mid(offset, p->op));
                break;
            } else {
                offset += p->op;
            }
        }
    }
    return a;
}

/**
 * Retrieve the complex data, which represent a null-terminated unicode string
 * for an option from an OfficeArtSpContainer.
 *
 * Look in all option containers in @p o for an option of type A.
 *
 * @param o OfficeArtSpContainer instance which contains options
 * @return QString storing complex data
 */
template<typename A>
QString getComplexName(const MSO::OfficeArtSpContainer &o)
{
    QString a;
    if (o.shapePrimaryOptions)
        a = getComplexName<A>(*o.shapePrimaryOptions);
    if (!a.isEmpty() && o.shapeSecondaryOptions1)
        a = getComplexName<A>(*o.shapeSecondaryOptions1);
    if (!a.isEmpty() && o.shapeSecondaryOptions2)
        a = getComplexName<A>(*o.shapeSecondaryOptions2);
    if (!a.isEmpty() && o.shapeTertiaryOptions1)
        a = getComplexName<A>(*o.shapeTertiaryOptions1);
    if (!a.isEmpty() && o.shapeTertiaryOptions2)
        a = getComplexName<A>(*o.shapeTertiaryOptions2);
    return a;
}

#endif
