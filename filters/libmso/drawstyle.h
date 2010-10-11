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

#ifndef DRAWSTYLE_H
#define DRAWSTYLE_H

#include "generated/simpleParser.h"

class IMsoArray {
public:
	quint16 nElems;
	quint16 nElemsAlloc;
	quint16 cbElem;
	QByteArray data;
	IMsoArray () :nElems(0), nElemsAlloc(0), cbElem(0) {}
};

class DrawStyle {
private:
    const MSO::OfficeArtDggContainer& d;
    const MSO::OfficeArtSpContainer* mastersp;
    const MSO::OfficeArtSpContainer* sp;
public:
    explicit DrawStyle(const MSO::OfficeArtDggContainer& d_,
                       const MSO::OfficeArtSpContainer* mastersp_ = 0,
                       const MSO::OfficeArtSpContainer* sp_ = 0)
        : d(d_), mastersp(mastersp_), sp(sp_) {}

    // Shape property set
    quint32 hspMaster() const;
    // ShapeBooleanProperties
    // Group Shape property set
    quint32 pWrapPolygonVertices() const;
    qint32 dxWrapDistLeft() const;
    qint32 dyWrapDistTop() const;
    qint32 dxWrapDistRight() const;
    qint32 dyWrapDistBottom() const;
    qint32 posH() const;
    qint32 posRelH() const;
    qint32 posV() const;
    qint32 posRelV() const;
    quint32 pctHR() const;
    quint32 alignHR() const;
    qint32 dxHeightHR() const;
    qint32 dxWidthHR() const;
    // GroupShapeBooleanProperties
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
    // Fill Style property set
    quint32 fillType() const;
    MSO::OfficeArtCOLORREF fillColor() const;
    MSO::FixedPoint        fillOpacity() const;
    MSO::OfficeArtCOLORREF fillBackColor() const;
    MSO::FixedPoint        fillBackOpacity() const;
    MSO::OfficeArtCOLORREF fillCrMod() const;
    quint32 fillBlip() const;
    quint32 fillBlipName() const;
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
    MSO::FixedPoint fillOriginX() const;
    MSO::FixedPoint fillOriginY() const;
    MSO::FixedPoint fillShapeOriginX() const;
    MSO::FixedPoint fillShapeOriginY() const;
/*     MSO::MSOSHADETYPE fillShadeType() const; */
    MSO::OfficeArtCOLORREF fillColorExt() const;
    MSO::OfficeArtCOLORREF fillBackColorExt() const;
    // FillStyleBooleanProperties
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
    // LineStyleBooleanProperties
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
    // ShadowStyleBooleanProperties
    bool fShadowObscured() const;
    bool fShadow() const;  
    // Text property set
    qint32 txflTextFlow() const;
    qint32 dxTextLeft() const;
    qint32 dyTextTop() const;
    qint32 dxTextRight() const;
    qint32 dyTextBottom() const;
    // Blip property set
    quint32 pib() const;

    IMsoArray fillShadeColors_complex() const;
    IMsoArray pVertices_complex() const;
    IMsoArray pSegmentInfo_complex() const;
    IMsoArray pWrapPolygonVertices_complex() const;
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
template <typename A, typename B>
const A*
get(const B& b)
{
    foreach(const MSO::OfficeArtFOPTEChoice& a, b.fopt) {
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
get(const MSO::OfficeArtSpContainer& o)
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
get(const MSO::OfficeArtDggContainer& o)
{
    const A* a = 0;
    if (o.drawingPrimaryOptions) {
        a = get<A>(*o.drawingPrimaryOptions);
    }
    if (!a && o.drawingTertiaryOptions) a = get<A>(*o.drawingTertiaryOptions);
    return a;
}
/**
 * Retrieve an option from a container
 *
 * Look in all option containers in @p o for an option of type A.
 * @param o OfficeArtDggContainer instance which contains options.
 * @return pointer to the option of type A or 0 if there is none.
 */
template <typename A, typename T>
const A*
get(const T* o)
{
    return (o) ?get<A>(*o) :0;
}
/**
 * Retrieve the complex data from an options containing class B
 *
 * @p b must have a member fopt that is an array of type OfficeArtFOPTEChoice.
 * A is the type of the required option.  The option containers in PPT/DOC have
 * only one instance of each option in an option container.
 *
 * @param b class that contains options.
 * @return pointer to an array storing the complex data or NULL if there were
 * no complex data or the option of type A at all.
 */
template <typename A, typename B>
IMsoArray
getComplexData(const B& b)
{
    MSO::OfficeArtFOPTE* p = NULL;
	IMsoArray a;
	const char* pData = b.complexData.data();
    uint offset = 0;

    foreach(const MSO::OfficeArtFOPTEChoice& _c, b.fopt) {
        p = (MSO::OfficeArtFOPTE*) _c.anon.data();
        if (p->opid.fComplex) {

            // there is wrong offset inside PVertices
            if (_c.anon.is<MSO::PVertices>()) {
                if (_c.anon.get<A>()) {
                	if (b.complexData.size() - offset >= 6) {
                    	a.nElems = *(quint16 *)(pData + offset);
                    	a.nElemsAlloc = *(quint16 *)(pData + offset +2);
                    	a.cbElem = *(quint16 *)(pData + offset + 4);
                    	a.data = b.complexData.mid(offset+6, p->op);
                        break;
                	}
                }
                else {
                offset += p->op +6;
                }
            }
            else {
                if (_c.anon.get<A>()) {
                	if (b.complexData.size() - offset >= 6) {
                    	a.nElems = *(quint16 *)(pData + offset);
                    	a.nElemsAlloc = *(quint16 *)(pData + offset +2);
                    	a.cbElem = *(quint16 *)(pData + offset + 4);
                    	a.data = b.complexData.mid(offset+6, p->op-6);
                        break;
                	}
                }
                else {
                offset += p->op;
                }
            }
        }
    }
    return a;
}

/**
 * Retrieve the complex data for an option from an OfficeArtSpContainer
 *
 * Look in all option containers in @p o for an option of type A.
 *
 * @param o OfficeArtSpContainer instance which contains options.

 * @return pointer to an array storing the complex data or NULL if there were
 * no complex data or the option of type A at all.  The caller takes the
 * ownership of the array reference.
 */
template <typename A>
IMsoArray
getComplexData(const MSO::OfficeArtSpContainer& o)
{
	IMsoArray a;
    if (o.shapePrimaryOptions) a = getComplexData<A>(*o.shapePrimaryOptions);
    if (!a.data.size() && o.shapeSecondaryOptions1) a = getComplexData<A>(*o.shapeSecondaryOptions1);
    if (!a.data.size() && o.shapeSecondaryOptions2) a = getComplexData<A>(*o.shapeSecondaryOptions2);
    if (!a.data.size() && o.shapeTertiaryOptions1) a = getComplexData<A>(*o.shapeTertiaryOptions1);
    if (!a.data.size() && o.shapeTertiaryOptions2) a = getComplexData<A>(*o.shapeTertiaryOptions2);
    return a;
}

#endif
