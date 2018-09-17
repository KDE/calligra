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
#include "ODrawToOdf.h"

#include <KoGenStyles.h>

/**
 * Report global objects belonging to global options to the collector.
 */
template <typename C, typename T>
void collectGlobalObjects(C& collector, const MSO::DrawingGroupContainer& c,
                          const T& fopt)
{
    foreach(const MSO::OfficeArtFOPTEChoice& f, fopt.fopt) {
        collector.add(c, f);
    }
}
/**
 * Report global objects belonging to incidental options to the collector.
 */
template <typename C, typename T>
void collectGlobalObjects(C& collector, const MSO::OfficeArtSpContainer& sp,
                          const T& fopt)
{
    foreach(const MSO::OfficeArtFOPTEChoice& f, fopt.fopt) {
        collector.add(sp, f);
    }
}
template <typename C>
void collectGlobalObjects(C& collector, const MSO::OfficeArtSpContainer& sp)
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
                          const MSO::OfficeArtSpgrContainerFileBlock& spgr);
template <typename C>
void collectGlobalObjects(C& collector,
                          const MSO::OfficeArtSpgrContainer& spgr)
{
    foreach(const MSO::OfficeArtSpgrContainerFileBlock& o, spgr.rgfb) {
        collectGlobalObjects(collector, o);
    }
}
template <typename C>
void collectGlobalObjects(C& collector, const MSO::OfficeArtDgContainer& dg)
{
    if (dg.groupShape) {
        collectGlobalObjects(collector, *dg.groupShape);
    }
    if (dg.shape) {
        collectGlobalObjects(collector, *dg.shape);
    }
    foreach(const MSO::OfficeArtSpgrContainerFileBlock& o, dg.deletedShapes) {
        collectGlobalObjects(collector, o);
    }
}
template <class C>
void collectGlobalObjects(C& collector,
                          const MSO::OfficeArtSpgrContainerFileBlock& spgr)
{
    if (spgr.anon.is<MSO::OfficeArtSpContainer>())
        collectGlobalObjects(collector, *spgr.anon.get<MSO::OfficeArtSpContainer>());
    if (spgr.anon.is<MSO::OfficeArtSpgrContainer>())
        collectGlobalObjects(collector, *spgr.anon.get<MSO::OfficeArtSpgrContainer>());
}
template <class C>
void collectGlobalObjects(C& collector, const ParsedPresentation& p) {
    // loop over all objects to find all OfficeArtFOPTE instances and feed them
    // into the collector
    // get object from default options
    const MSO::DrawingGroupContainer& dg = p.documentContainer->drawingGroup;
    if (dg.OfficeArtDgg.drawingPrimaryOptions)
        collectGlobalObjects(collector, dg,
                             *dg.OfficeArtDgg.drawingPrimaryOptions);
    if (dg.OfficeArtDgg.drawingTertiaryOptions)
        collectGlobalObjects(collector, dg,
                             *dg.OfficeArtDgg.drawingTertiaryOptions);
    // get objects from masters
    foreach(const MSO::MasterOrSlideContainer* master, p.masters) {
        const MSO::SlideContainer* sc = master->anon.get<MSO::SlideContainer>();
        const MSO::MainMasterContainer* sm
                = master->anon.get<MSO::MainMasterContainer>();
        if (sc) {
            collectGlobalObjects(collector, sc->drawing.OfficeArtDg);
        }
        if (sm) {
            collectGlobalObjects(collector, sm->drawing.OfficeArtDg);
        }
    }
    // get objects from slides
    foreach(const MSO::SlideContainer* slide, p.slides) {
        collectGlobalObjects(collector, slide->drawing.OfficeArtDg);
    }
    // get objects from notes
    foreach(const MSO::NotesContainer* notes, p.notes) {
        if (notes) {
            collectGlobalObjects(collector, notes->drawing.OfficeArtDg);
        }
    }
}
/**
  * Collector that retrieves all fill images.
  **/
class FillImageCollector {
public:
    KoGenStyles& styles;
    const PptToOdp& pto;
    QMap<const MSO::DrawingGroupContainer*, QString> globalFillImageNames;
    QMap<const MSO::OfficeArtSpContainer*, QString> fillImageNames;

    FillImageCollector(KoGenStyles& s, const PptToOdp& p) :styles(s), pto(p) {}

    void add(const MSO::DrawingGroupContainer& o, const MSO::OfficeArtFOPTEChoice& t) {
        const QString name = add(t);
        if (!name.isEmpty()) globalFillImageNames[&o] = name;
    }
    void add(const MSO::OfficeArtSpContainer& o, const MSO::OfficeArtFOPTEChoice& t) {
        const QString name = add(t);
        if (!name.isEmpty()) fillImageNames[&o] = name;
    }
    QString add(const MSO::OfficeArtFOPTEChoice& t) {
        const MSO::FillBlip* fb = t.anon.get<MSO::FillBlip>();
        if (!fb || fb->opid.fComplex || fb->fillBlip == 0) return QString();
        KoGenStyle fillImage(KoGenStyle::FillImageStyle);
        fillImage.addAttribute("xlink:href", pto.getPicturePath(fb->fillBlip));
        fillImage.addAttribute("xlink:type", "simple");
        return styles.insert(fillImage,
                             QString("fillImage%1").arg(fb->fillBlip),
                             KoGenStyles::DontAddNumberToName);
    }
};

// NOTE: OBSOLETE

/* /\** */
/*  * Collector that retrieves all dash styles used in the document. */
/*  * TODO: handle LineDashStyle and top, left, right and bottom line dashes */
/*  *\/ */
/* class StrokeDashCollector { */
/* public: */
/*     KoGenStyles& styles; */
/*     const PptToOdp& pto; */
/*     QMap<const MSO::DrawingGroupContainer*, QString> globalStrokeDashNames; */
/*     QMap<const MSO::OfficeArtSpContainer*, QString> strokeDashNames; */

/*     StrokeDashCollector(KoGenStyles& s, const PptToOdp& p) :styles(s), pto(p) {} */

/*     void add(const MSO::DrawingGroupContainer& o, const MSO::OfficeArtFOPTEChoice& t) { */
/*         const QString name = add(t); */
/*         if (!name.isEmpty()) globalStrokeDashNames[&o] = name; */
/*     } */
/*     void add(const MSO::OfficeArtSpContainer& o, const MSO::OfficeArtFOPTEChoice& t) { */
/*         const QString name = add(t); */
/*         if (!name.isEmpty()) strokeDashNames[&o] = name; */
/*     } */
/*     QString add(const MSO::OfficeArtFOPTEChoice& t) { */
/*         quint32 lineDashing = 0; */
/*         const MSO::LineDashing* ld1 = t.anon.get<MSO::LineDashing>(); */
/*         if (ld1) lineDashing = ld1->lineDashing; */
/*         /\* TODO */
/*         const LineBottomDashing* ld2 = t.anon.get<LineBottomDashing>(); */
/*         if (ld2) lineDashing = ld2->lineDashing; */
/*         const LineTopDashing* ld3 = t.anon.get<LineTopDashing>(); */
/*         if (ld3) lineDashing = ld3->lineDashing; */
/*         const LineLeftDashing* ld4 = t.anon.get<LineLeftDashing>(); */
/*         if (ld4) lineDashing = ld4->lineDashing; */
/*         const LineRightDashing* ld5 = t.anon.get<LineRightDashing>(); */
/*         if (ld5) lineDashing = ld5->lineDashing; */
/*         *\/ */
/*         //const LineDashingStyle* lds = t.anon.get<LineDashingStyle>(); */
/*         return dashStyle(lineDashing, styles); */
/*     } */
/* }; */


#endif
