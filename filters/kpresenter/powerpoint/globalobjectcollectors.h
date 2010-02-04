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
#ifndef GLOBALOBJECTCOLLECTORS_H
#define GLOBALOBJECTCOLLECTORS_H

#include "PptToOdp.h"

/**
 * Report global objects belonging to global options to the collector.
 */
template <typename C, typename T>
        void collectGlobalObjects(C& collector, const PPT::DrawingGroupContainer& c,
                          const T& fopt)
{
    foreach(const PPT::OfficeArtFOPTEChoice& f, fopt.fopt) {
        collector.add(c, f);
    }
}
/**
 * Report global objects belonging to incidental options to the collector.
 */
template <typename C, typename T>
void collectGlobalObjects(C& collector, const PPT::OfficeArtSpContainer& sp,
                          const T& fopt)
{
    foreach(const PPT::OfficeArtFOPTEChoice& f, fopt.fopt) {
        collector.add(sp, f);
    }
}
template <typename C>
void collectGlobalObjects(C& collector, const PPT::OfficeArtSpContainer& sp)
{
    if (sp.shapePrimaryOptions)
        collectGlobalObjects(collector, sp, *sp.shapePrimaryOptions);
    if (sp.shapeSecondaryOptions1)
        collectGlobalObjects(collector, sp, *sp.shapeSecondaryOptions1);
    if (sp.shapeSecondaryOptions2)
        collectGlobalObjects(collector, sp, *sp.shapeSecondaryOptions2);
    if (sp.shapeTertiaryOptions1)
        collectGlobalObjects(collector, sp, *sp.shapeTertiaryOptions1);
    if (sp.shapeTertiaryOptions2)
        collectGlobalObjects(collector, sp, *sp.shapeTertiaryOptions2);
}
template <typename C>
void collectGlobalObjects(C& collector,
                          const PPT::OfficeArtSpgrContainerFileBlock& spgr);
template <typename C>
void collectGlobalObjects(C& collector,
                          const PPT::OfficeArtSpgrContainer& spgr)
{
    foreach(const PPT::OfficeArtSpgrContainerFileBlock& o, spgr.rgfb) {
        collectGlobalObjects(collector, o);
    }
}
template <typename C>
void collectGlobalObjects(C& collector, const PPT::OfficeArtDgContainer& dg)
{
    collectGlobalObjects(collector, dg.groupShape);
    if (dg.shape)
        collectGlobalObjects(collector, *dg.shape);
    foreach(const PPT::OfficeArtSpgrContainerFileBlock& o, dg.deletedShapes) {
        collectGlobalObjects(collector, o);
    }
}
template <class C>
void collectGlobalObjects(C& collector,
                          const PPT::OfficeArtSpgrContainerFileBlock& spgr)
{
    if (spgr.anon.is<PPT::OfficeArtSpContainer>())
        collectGlobalObjects(collector, *spgr.anon.get<PPT::OfficeArtSpContainer>());
    if (spgr.anon.is<PPT::OfficeArtSpgrContainer>())
        collectGlobalObjects(collector, *spgr.anon.get<PPT::OfficeArtSpgrContainer>());
}
template <class C>
void collectGlobalObjects(C& collector, const ParsedPresentation& p) {
    // loop over all objects to find all OfficeArtFOPTE instances and feed them
    // into the collector
    // get object from default options
    const PPT::DrawingGroupContainer& dg = p.documentContainer->drawingGroup;
    collectGlobalObjects(collector, dg, dg.OfficeArtDgg.drawingPrimaryOptions);
    if (dg.OfficeArtDgg.drawingTertiaryOptions)
        collectGlobalObjects(collector, dg,
                             *dg.OfficeArtDgg.drawingTertiaryOptions);
    // get objects from masters
    foreach(const PPT::MasterOrSlideContainer* master, p.masters) {
        if (master->anon.is<PPT::SlideContainer>())
            collectGlobalObjects(collector,
                                 master->anon.get<PPT::SlideContainer>()->drawing.OfficeArtDg);
        if (master->anon.is<PPT::MainMasterContainer>())
            collectGlobalObjects(collector,
                                 master->anon.get<PPT::MainMasterContainer>()->drawing.OfficeArtDg);
    }
    // get objects from slides
    foreach(const PPT::SlideContainer* slide, p.slides) {
        collectGlobalObjects(collector, slide->drawing.OfficeArtDg);
    }
    // get objects from notes
    foreach(const PPT::NotesContainer* notes, p.notes) {
        collectGlobalObjects(collector, notes->drawing.OfficeArtDg);
    }
}
/**
  * Collector that retrieves all fill images.
  **/
class FillImageCollector {
public:
    KoGenStyles& styles;
    const PptToOdp& pto;
    QMap<const PPT::DrawingGroupContainer*, QString> globalFillImageNames;
    QMap<const PPT::OfficeArtSpContainer*, QString> fillImageNames;

    FillImageCollector(KoGenStyles& s, const PptToOdp& p) :styles(s), pto(p) {}

    void add(const PPT::DrawingGroupContainer& o, const PPT::OfficeArtFOPTEChoice& t) {
        const QString name = add(t);
        if (!name.isEmpty()) globalFillImageNames[&o] = name;
    }
    void add(const PPT::OfficeArtSpContainer& o, const PPT::OfficeArtFOPTEChoice& t) {
        const QString name = add(t);
        if (!name.isEmpty()) fillImageNames[&o] = name;
    }
    QString add(const PPT::OfficeArtFOPTEChoice& t) {
        const PPT::FillBlip* fb = t.anon.get<PPT::FillBlip>();
        if (!fb || fb->opid.fComplex || fb->fillBlip == 0) return QString();
        KoGenStyle fillImage(KoGenStyle::StyleFillImage);
        fillImage.addAttribute("xlink:href", pto.getPicturePath(fb->fillBlip));
        return styles.lookup(fillImage, "fillImage");
    }
};
/**
  * Collector that retrieves all dash styles used in the document.
  * TODO: handle LineDashStyle and top, left, rigth and bottom line dashes
  **/
class StrokeDashCollector {
public:
    KoGenStyles& styles;
    const PptToOdp& pto;
    QMap<const PPT::DrawingGroupContainer*, QString> globalStrokeDashNames;
    QMap<const PPT::OfficeArtSpContainer*, QString> strokeDashNames;

    StrokeDashCollector(KoGenStyles& s, const PptToOdp& p) :styles(s), pto(p) {}

    void add(const PPT::DrawingGroupContainer& o, const PPT::OfficeArtFOPTEChoice& t) {
        const QString name = add(t);
        if (!name.isEmpty()) globalStrokeDashNames[&o] = name;
    }
    void add(const PPT::OfficeArtSpContainer& o, const PPT::OfficeArtFOPTEChoice& t) {
        const QString name = add(t);
        if (!name.isEmpty()) strokeDashNames[&o] = name;
    }
    QString add(const PPT::OfficeArtFOPTEChoice& t) {
        quint32 lineDashing = 0;
        const PPT::LineDashing* ld1 = t.anon.get<PPT::LineDashing>();
        if (ld1) lineDashing = ld1->lineDashing;
        /* TODO
        const LineBottomDashing* ld2 = t.anon.get<LineBottomDashing>();
        if (ld2) lineDashing = ld2->lineDashing;
        const LineTopDashing* ld3 = t.anon.get<LineTopDashing>();
        if (ld3) lineDashing = ld3->lineDashing;
        const LineLeftDashing* ld4 = t.anon.get<LineLeftDashing>();
        if (ld4) lineDashing = ld4->lineDashing;
        const LineRightDashing* ld5 = t.anon.get<LineRightDashing>();
        if (ld5) lineDashing = ld5->lineDashing;
        */
        if (lineDashing <= 0 || lineDashing > 10) return QString();

        //const LineDashingStyle* lds = t.anon.get<LineDashingStyle>();
        KoGenStyle strokeDash(KoGenStyle::StyleStrokeDash);
        switch (lineDashing) {
        case 0: // msolineSolid, not a real stroke dash
            break;
        case 1: // msolineDashSys
            strokeDash.addAttribute("draw:dots1", "1");
            strokeDash.addAttribute("draw:dots1-length", "300%");
            strokeDash.addAttribute("draw:distance", "100%");
            break;
        case 2: // msolineDotSys
            strokeDash.addAttribute("draw:dots1", "1");
            strokeDash.addAttribute("draw:dots1-length", "200%");
            break;
        case 3: // msolineDashDotSys
            strokeDash.addAttribute("draw:dots1", "1");
            strokeDash.addAttribute("draw:dots1-length", "300%");
            strokeDash.addAttribute("draw:dots2", "1");
            strokeDash.addAttribute("draw:dots2-length", "100%");
            break;
        case 4: // msolineDashDotDotSys
            strokeDash.addAttribute("draw:dots1", "1");
            strokeDash.addAttribute("draw:dots1-length", "300%");
            strokeDash.addAttribute("draw:dots2", "1");
            strokeDash.addAttribute("draw:dots2-length", "100%");
            break;
        case 5: // msolineDotGEL
            strokeDash.addAttribute("draw:dots1", "1");
            strokeDash.addAttribute("draw:dots1-length", "100%");
            break;
        case 6: // msolineDashGEL
            strokeDash.addAttribute("draw:dots1", "4");
            strokeDash.addAttribute("draw:dots1-length", "100%");
            break;
        case 7: // msolineLongDashGEL
            strokeDash.addAttribute("draw:dots1", "8");
            strokeDash.addAttribute("draw:dots1-length", "100%");
            break;
        case 8: // msolineDashDotGEL
            strokeDash.addAttribute("draw:dots1", "1");
            strokeDash.addAttribute("draw:dots1-length", "300%");
            strokeDash.addAttribute("draw:dots2", "1");
            strokeDash.addAttribute("draw:dots2-length", "100%");
            break;
        case 9: // msolineLongDashDotGEL
            strokeDash.addAttribute("draw:dots1", "1");
            strokeDash.addAttribute("draw:dots1-length", "800%");
            strokeDash.addAttribute("draw:dots2", "1");
            strokeDash.addAttribute("draw:dots2-length", "100%");
            break;
        case 10: // msolineLongDashDotDotGEL
            strokeDash.addAttribute("draw:dots1", "1");
            strokeDash.addAttribute("draw:dots1-length", "800%");
            strokeDash.addAttribute("draw:dots2", "2");
            strokeDash.addAttribute("draw:dots2-length", "100%");
            break;
        };
        if (lineDashing < 5) {
            strokeDash.addAttribute("draw:distance", "100%");
        } else {
            strokeDash.addAttribute("draw:distance", "300%");
        }
        return styles.lookup(strokeDash, "strokeDash");
    }
};


#endif
