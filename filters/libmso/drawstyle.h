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

class DrawStyle {
private:
    const MSO::OfficeArtDggContainer& d;
    const MSO::OfficeArtSpContainer* mastersp;
    const MSO::OfficeArtSpContainer* sp;
public:
    DrawStyle(const MSO::OfficeArtDggContainer& d_,
              const MSO::OfficeArtSpContainer* mastersp_ = 0,
              const MSO::OfficeArtSpContainer* sp_ = 0)
                  :d(d_), mastersp(mastersp_), sp(sp_) {}

    // FillStyleBooleanProperties
    bool fNoFillHitTest() const;
    bool fillUseRect() const;
    bool fillShape() const;
    bool fHitTestFill() const;
    bool fFilled() const;
    bool fUseShapeAnchor() const;
    bool fRecolorFillAsPicture() const;
    // FillType
    quint32 fillType() const;
    // FillColor
    MSO::OfficeArtCOLORREF fillColor() const;
    // FillBlip
    quint32 fillBlip() const;
    // FillDztype
    qint32 fillDztype() const;
    // LineEndArrowhead
    quint32 lineEndArrowhead() const;
    // LineStartArrowhead
    quint32 lineStartArrowhead() const;
    // LineStartArrowWidth
    quint32 lineStartArrowWidth() const;
    // LineEndArrowWidth
    quint32 lineEndArrowWidth() const;
    // LineWidth
    quint32 lineWidth() const;
    // ShadowOffsetX
    qint32 shadowOffsetX() const;
    // ShadowOffsetY
    qint32 shadowOffsetY() const;
    // ShadowOpacity
    MSO::FixedPoint shadowOpacity() const;
    // LineDashing
    quint32 lineDashing() const;
    // LineStyleBooleanProperties
    bool fNoLineDrawDash() const;
    bool fLineFillShape() const;
    bool fHitTestLine() const;
    bool fLine() const;
    bool fArrowHeadsOK() const;
    bool fInsetPenOK() const;
    bool fInsetPen() const;
    bool fLineOpaqueBackColor() const;
    // LineColor
    MSO::OfficeArtCOLORREF lineColor() const;
    // LineOpacity
    qint32 lineOpacity() const;
    // TxflTextFlow
    qint32 txflTextFlow() const;
    // PosH
    qint32 posH() const;
    // PosRelH
    qint32 posRelH() const;
    // PosV
    qint32 posV() const;
    // PosRelV
    qint32 posRelV() const;
};

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
        get<A>(*o.drawingPrimaryOptions);
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

#endif
