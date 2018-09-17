/* This file is part of the KDE project
   Copyright (C) 2010 KO GmbH <jos.van.den.oever@kogmbh.com>
   Copyright (C) 2011 Lukáš Tvrdý <lukas.tvrdy@ixonos.com>

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
#include "msodraw.h"
#include <KoGenStyles.h>
#include <KoXmlWriter.h>
#include <QColor>
#include <QBuffer>
#include "generated/leinputstream.h"

#include <cmath>

//! Converts EMU (English Metric Unit) value (integer or double) to points
#define EMU_TO_POINT(emu) ((emu)/12700.0)

using namespace MSO;

/**
 * Return the bounding rectangle for this object.
 **/
QRectF ODrawToOdf::getRect(const OfficeArtFSPGR &r)
{
    return QRect(r.xLeft, r.yTop, r.xRight - r.xLeft, r.yBottom - r.yTop);
}

void ODrawToOdf::processGroupShape(const MSO::OfficeArtSpgrContainer& o, Writer& out)
{
    if (o.rgfb.size() < 2) return;

    //The first container MUST be an OfficeArtSpContainer record, which
    //MUST contain shape information for the group.  MS-ODRAW, 2.2.16
    const OfficeArtSpContainer* sp = o.rgfb[0].anon.get<OfficeArtSpContainer>();

    //An OfficeArtFSPGR record specifies the coordinate system of the group
    //shape.  The anchors of the child shape are expressed in this coordinate
    //system.  This record’s container MUST be a group shape.
    if (sp && sp->shapeProp.fGroup) {
        QRectF oldCoords;
        if (!sp->shapeProp.fPatriarch) {
            out.xml.startElement("draw:g");

            //TODO: rotation and flipping of group shapes
            const DrawStyle ds(0, 0, sp);
            qreal rotation = toQReal(ds.rotation());
            out.g_rotation += rotation;
            out.g_flipH = sp->shapeProp.fFlipH;
            out.g_flipV = sp->shapeProp.fFlipV;

            if (sp->clientAnchor && sp->shapeGroup) {
                oldCoords = client->getRect(*sp->clientAnchor);
            }
        }
        if (oldCoords.isValid()) {
            Writer out_trans = out.transform(oldCoords, getRect(*sp->shapeGroup));
            for (int i = 1; i < o.rgfb.size(); ++i) {
                processDrawing(o.rgfb[i], out_trans);
            }
        } else {
            for (int i = 1; i < o.rgfb.size(); ++i) {
                processDrawing(o.rgfb[i], out);
            }
        }
        if (!sp->shapeProp.fPatriarch) {
            out.xml.endElement(); //draw:g
        }
    }
}

void ODrawToOdf::processDrawing(const OfficeArtSpgrContainerFileBlock& of,
                                Writer& out)
{
    if (of.anon.is<OfficeArtSpgrContainer>()) {
        processGroupShape(*of.anon.get<OfficeArtSpgrContainer>(), out);
    } else {
        processDrawingObject(*of.anon.get<OfficeArtSpContainer>(), out);
    }
}
void ODrawToOdf::addGraphicStyleToDrawElement(Writer& out,
        const OfficeArtSpContainer& o)
{
    KoGenStyle style;
    const OfficeArtDggContainer* drawingGroup = 0;
    const OfficeArtSpContainer* master = 0;

    if (client) {
        drawingGroup = client->getOfficeArtDggContainer();

        //locate the OfficeArtSpContainer of the master shape
        if (o.shapeProp.fHaveMaster) {
            const DrawStyle tmp(0, &o);
            quint32 spid = tmp.hspMaster();
            master = client->getMasterShapeContainer(spid);
        }
    }
    const DrawStyle ds(drawingGroup, master, &o);
    if (client) {
        style = client->createGraphicStyle(o.clientTextbox.data(),
                                           o.clientData.data(), ds, out);
    }
    defineGraphicProperties(style, ds, out.styles);

    if (client) {
        client->addTextStyles(o.clientTextbox.data(),
                              o.clientData.data(), style, out);
    }
}

namespace
{
    QString format(double v)
    {
        static const QString f("%1");
        static const QString e("");
        static const QRegExp r("\\.?0+$");
        return f.arg(v, 0, 'f').replace(r, e);
    }

    QString pt(double v)
    {
        static const QString pt("pt");
        return format(v) + pt;
    }

    QString percent(double v)
    {
        return format(v) + '%';
    }
} //namespace

void ODrawToOdf::defineGraphicProperties(KoGenStyle& style, const DrawStyle& ds, KoGenStyles& styles)
{
    const KoGenStyle::PropertyType gt = KoGenStyle::GraphicType;
    // dr3d:ambient-color
    // dr3d:back-scale
    // dr3d:backface-culling
    // dr3d:close-back
    // dr3d:close-front
    // dr3d:depth
    // dr3d:diffuse-color
    // dr3d:edge-rounding
    // dr3d:edge-rounding-mode
    // dr3d:emissive-color
    // dr3d:end-angle
    // dr3d:horizontal-segments
    // dr3d:lighting-mode
    // dr3d:normals-direction
    // dr3d:normals-kind
    // dr3d:shadow
    // dr3d:shininess
    // dr3d:specular-color
    // dr3d:texture-filter
    // dr3d:texture-generation-mode-x
    // dr3d:texture-generation-mode-y
    // dr3d:texture-kind
    // dr3d:texture-mode
    // dr3d:vertical-segments
    // draw:auto-grow-height
    style.addProperty("draw:auto-grow-height", ds.fFitShapeToText(), gt);
    // draw:auto-grow-width
    style.addProperty("draw:auto-grow-width", ds.fFitShapeToText() && ds.wrapText()==msowrapNone, gt);
    // draw:blue
    // draw:caption-angle
    // draw:caption-angle-type
    // draw:caption-escape
    // draw:caption-escape-direction
    // draw:caption-fit-line-length
    // draw:caption-gap
    // draw:caption-line-length
    // draw:caption-type
    // draw:color-inversion
    // draw:color-mode
    if (ds.fPictureBiLevel()) {
        style.addProperty("draw:color-mode", "mono", gt);
    } else if (ds.fPictureGray()) {
        style.addProperty("draw:color-mode", "greyscale", gt);
    }
    // draw:contrast
    // draw:decimal-places
    // draw:end-guide
    // draw:end-line-spacing-horizontal
    // draw:end-line-spacing-vertical

    // NOTE: fFilled specifies whether fill of the shape is render based on the
    // properties of the "fill style" property set.
    if (ds.fFilled()) {
        // draw:fill ("bitmap", "gradient", "hatch", "none" or "solid")
        qint32 fillType = ds.fillType();
        style.addProperty("draw:fill", getFillType(fillType), gt);
        // draw:fill-color
        // only set the color if the fill type is 'solid' because OOo ignores
        // fill='none' if the color is set
        switch (fillType) {
        case msofillSolid:
        {
            if (!client) break;
            QColor color = processOfficeArtCOLORREF(ds.fillColor(), ds);
            style.addProperty("draw:fill-color", color.name(), gt);
            break;
        }
        // draw:fill-gradient-name
        case msofillShade:
        case msofillShadeCenter:
        case msofillShadeShape:
        case msofillShadeScale:
        case msofillShadeTitle:
        {
            if (!client) break;
            KoGenStyle gs(KoGenStyle::LinearGradientStyle);
            defineGradientStyle(gs, ds);
            QString gname = styles.insert(gs);
            style.addProperty("draw:fill-gradient-name", gname, gt);
            break;
        }
        // draw:fill-hatch-name
        // draw:fill-hatch-solid
        // draw:fill-image-height
        // draw:fill-image-name
        case msofillPattern:
        case msofillTexture:
        case msofillPicture:
        {
            if (!client) break;
            quint32 fillBlip = ds.fillBlip();
            QString fillImagePath;
            fillImagePath = client->getPicturePath(fillBlip);
            if (!fillImagePath.isEmpty()) {
                style.addProperty("draw:fill-image-name",
                                  "fillImage" + QString::number(fillBlip), gt);

                style.addProperty("style:repeat", getRepeatStyle(fillType), gt);
            }
            break;
        }
        case msofillBackground:
        default:
            break;
        }
        // draw:fill-image-ref-point
        // draw:fill-image-ref-point-x
        // draw:fill-image-ref-point-y
        // draw:fill-image-width
        // draw:opacity
        style.addProperty("draw:opacity",
                          percent(100.0 * toQReal(ds.fillOpacity())), gt);
        // draw:opacity-name
    } else {
        style.addProperty("draw:fill", "none", gt);
    }
    // draw:fit-to-contour
    // draw:fit-to-size
    // draw:frame-display-border
    // draw:frame-display-scrollbar
    // draw:frame-margin-horizontal
    // draw:frame-margin-vertical
    // draw:gamma
    // draw:gradient-step-count
    // draw:green
    // draw:guide-distance
    // draw:guide-overhang
    // draw:image-opacity
    // draw:line-distance
    // draw:luminance

    // TODO: improve marker width calculation
    qreal lineWidthPt = EMU_TO_POINT(ds.lineWidth());
    // markers are shape specific and thus do NOT belong into the
    // default graphic style
    if (ds.fLine() && ( ds.shapeType() != msosptNil )) {
        quint32 lineEndArrowhead = ds.lineEndArrowhead();
        if (lineEndArrowhead > 0 && lineEndArrowhead < 6) {
            // draw:marker-end
            style.addProperty("draw:marker-end", defineMarkerStyle(styles, lineEndArrowhead), gt);
            // draw:marker-end-center
            style.addProperty("draw:marker-end-center", "false", gt);
            // draw:marker-end-width
            style.addPropertyPt("draw:marker-end-width",
                                lineWidthPt * 2 + ds.lineEndArrowWidth(), gt);
        }
        quint32 lineStartArrowhead = ds.lineStartArrowhead();
        if (lineStartArrowhead > 0 && lineStartArrowhead < 6) {
            // draw:marker-start
            style.addProperty("draw:marker-start", defineMarkerStyle(styles, lineStartArrowhead), gt);
            // draw:marker-start-center
            style.addProperty("draw:marker-start-center", "false", gt);
            // draw:marker-start-width
            style.addPropertyPt("draw:marker-start-width",
                                lineWidthPt * 2 + ds.lineStartArrowWidth(), gt);
        }
    }
    // draw:measure-align
    // draw:measure-vertical-align
    // draw:ole-draw-aspect
    // draw:parallel
    // draw:placing
    // draw:red
    // draw:secondary-fill-color
    if (ds.fShadow()) {
        // draw:shadow
        style.addProperty("draw:shadow", "visible", gt);
        // draw:shadow-color
        if (client) {
            QColor clr = processOfficeArtCOLORREF(ds.shadowColor(), ds);
            style.addProperty("draw:shadow-color", clr.name(), gt);
        }
        // NOTE: shadowOffset* properties MUST exist if shadowType
        // property equals msoshadowOffset or msoshadowDouble,
        // otherwise MUST be ignored, MS-ODRAW 2.3.13.6
        quint32 type = ds.shadowType();
        if ((type == 0) || (type == 1)) {
            // draw:shadow-offset-x
            style.addPropertyPt("draw:shadow-offset-x", EMU_TO_POINT(ds.shadowOffsetX()), gt);
            // draw:shadow-offset-y
            style.addPropertyPt("draw:shadow-offset-y", EMU_TO_POINT(ds.shadowOffsetY()), gt);
        }
        // draw:shadow-opacity
        float shadowOpacity = toQReal(ds.shadowOpacity());
        style.addProperty("draw:shadow-opacity", percent(100*shadowOpacity), gt);
    } else {
        style.addProperty("draw:shadow", "hidden", gt);
    }
    // draw:show-unit
    // draw:start-guide
    // draw:start-line-spacing-horizontal
    // draw:start-line-spacing-vertical
    // draw:stroke ('dash', 'none' or 'solid')
    if (ds.fLine()) {
        quint32 lineDashing = ds.lineDashing();

        // NOTE: OOo interprets solid line of width 0 as hairline, so
        // stroke *must* be "none" in order to be NOT display in OOo.
        if (lineWidthPt == 0) {
            style.addProperty("draw:stroke", "none", gt);
        } else if (lineDashing > 0 && lineDashing < 11) {
            // NOTE: A "dash" looks wrong in Calligra/LO when
            // svg:stroke-linecap is applied.
            style.addProperty("draw:stroke", "dash", gt);
            style.addProperty("draw:stroke-dash", defineDashStyle(styles, lineDashing), gt);
        } else {
            style.addProperty("draw:stroke", "solid", gt);
            // svg:stroke-linecap
            style.addProperty("svg:stroke-linecap", getStrokeLineCap(ds.lineEndCapStyle()), gt);
        }
        if (lineWidthPt != 0) {
            // draw:stroke-linejoin
            style.addProperty("draw:stroke-linejoin", getStrokeLineJoin(ds.lineJoinStyle()), gt);
            // svg:stroke-color
            if (client) {
                QColor clr = processOfficeArtCOLORREF(ds.lineColor(), ds);
                style.addProperty("svg:stroke-color", clr.name(), gt);
            }
            // svg:stroke-opacity
            style.addProperty("svg:stroke-opacity", percent(100.0 * ds.lineOpacity() / 0x10000), gt);
            // svg:stroke-width
            style.addProperty("svg:stroke-width", pt(lineWidthPt), gt);
        }
    }
    // NOTE: Seems to be only relevant for master shapes. A usecase
    // would be a user editing a master slide.
    // else if (ds.fNoLineDrawDash()) {
    //     style.addProperty("draw:stroke", "dash", gt);
    //     style.addProperty("draw:stroke-dash", defineDashStyle(styles, msolineDashSys), gt);
    // }
    else {
        style.addProperty("draw:stroke", "none", gt);
    }
    // draw:stroke-dash-names
    // draw:symbol-color
    // draw:textarea-horizontal-align
    style.addProperty("draw:textarea-horizontal-align", getHorizontalAlign(ds.anchorText()), gt);
    // draw:textarea-vertical-align
    style.addProperty("draw:textarea-vertical-align", getVerticalAlign(ds.anchorText()), gt);
    // draw:tile-repeat-offset
    // draw:unit
    // draw:visible-area-height
    // draw:visible-area-left
    // draw:visible-area-top
    // draw:visible-area-width
    // draw:wrap-influence-on-position
    // fo:background-color
    // fo:border
    // fo:border-bottom
    // fo:border-left
    // fo:border-right
    // fo:border-top
    // fo:clip
    // fo:margin
    // fo:margin-bottom
    // fo:margin-left
    // fo:margin-right
    // fo:margin-top
    style.addPropertyPt("fo:margin-bottom", EMU_TO_POINT(ds.dyWrapDistBottom()), gt);
    style.addPropertyPt("fo:margin-left", EMU_TO_POINT(ds.dxWrapDistLeft()), gt);
    style.addPropertyPt("fo:margin-right", EMU_TO_POINT(ds.dxWrapDistRight()), gt);
    style.addPropertyPt("fo:margin-top", EMU_TO_POINT(ds.dyWrapDistTop()), gt);
    // fo:max-height
    // fo:max-width
    // fo:min-height
    // fo:min-width
    // fo:padding
    // fo:padding-left
    // fo:padding-top
    // fo:padding-right
    // fo:padding-bottom
    if (!ds.fAutoTextMargin()) {
        style.addPropertyPt("fo:padding-left", EMU_TO_POINT(ds.dxTextLeft()), gt);
        style.addPropertyPt("fo:padding-top", EMU_TO_POINT(ds.dyTextTop()), gt);
        style.addPropertyPt("fo:padding-right", EMU_TO_POINT(ds.dxTextRight()), gt);
        style.addPropertyPt("fo:padding-bottom", EMU_TO_POINT(ds.dyTextBottom()), gt);
    } else {
        // default internal margins for text on shapes
        style.addPropertyPt("fo:padding-left", EMU_TO_POINT(0x00016530), gt);
        style.addPropertyPt("fo:padding-top", EMU_TO_POINT(0x0000B298), gt);
        style.addPropertyPt("fo:padding-right", EMU_TO_POINT(0x00016530), gt);
        style.addPropertyPt("fo:padding-bottom", EMU_TO_POINT(0x0000B298), gt);
    }
    // fo:wrap-option
    // style:background-transparency
    // style:border-line-width
    // style:border-line-width-bottom
    // style:border-line-width-left
    // style:border-line-width-right
    // style:border-line-width-top
    // style:editable
    // style:flow-with-text
    style.addProperty("style:flow-with-text", ds.fLayoutInCell(), gt);
    // style:horizontal-pos
    // NOTE: tests on PPT, XLS required
//     style.addProperty("style:horizontal-pos", getHorizontalPos(ds.posH()), gt);
    // style:horizontal-rel
    // NOTE: tests on PPT, XLS required
//     style.addProperty("style:horizontal-rel", getHorizontalRel(ds.posRelH()), gt);
    // style:mirror
    // style:number-wrapped-paragraphs
    // style:overflow-behavior
    // style:print-content
    // style:protect
    // style:rel-height
    // style:rel-width
    // style:repeat // handled for image see draw:fill-image-name
    // style:run-through
    // style:shadow
    // style:shrink-to-fit
    // style:vertical-pos
    // NOTE: tests on PPT, XLS required
//     style.addProperty("style:vertical-pos", getVerticalPos(ds.posV()), gt);
    // style:vertical-rel
    // NOTE: tests on PPT, XLS required
//     style.addProperty("style:vertical-rel", getVerticalRel(ds.posRelV()), gt);
    // style:wrap
    // style:wrap-contour
    // style:wrap-contour-mode
    // style:wrap-dynamic-threshold
    // style:writing-mode
    // svg:fill-rule
    QString fillRule(getFillRule(ds.shapeType()));
    if (!fillRule.isEmpty()) {
        style.addProperty("svg:fill-rule" ,fillRule, gt);
    }
    // svg:height
    // svg:width
    // svg:x
    // svg:y
    // text:anchor-page-number
    // text:anchor-type
    // text:animation
    // text:animation-delay
    // text:animation-direction
    // text:animation-repeat
    // text:animation-start-inside
    // text:animation-steps
    // text:animation-stop-inside
}

namespace
{
    const char* const markerStyles[6] = {
        "", "msArrowEnd_20_5", "msArrowStealthEnd_20_5", "msArrowDiamondEnd_20_5",
        "msArrowOvalEnd_20_5", "msArrowOpenEnd_20_5"
    };
}

QString ODrawToOdf::defineMarkerStyle(KoGenStyles& styles, const quint32 arrowType)
{
    if ( !(arrowType > msolineNoEnd && arrowType < msolineArrowChevronEnd) ) {
        return QString();
    }

    const QString name(markerStyles[arrowType]);

    if (styles.style(name, "")) {
        return name;
    }

    KoGenStyle marker(KoGenStyle::MarkerStyle);
    marker.addAttribute("draw:display-name",  QString(markerStyles[arrowType]).replace("_20_", " "));

    // sync with LO
    switch (arrowType) {
    case msolineArrowStealthEnd:
        marker.addAttribute("svg:viewBox", "0 0 318 318");
        marker.addAttribute("svg:d", "m159 0 159 318-159-127-159 127z");
        break;
    case msolineArrowDiamondEnd:
        marker.addAttribute("svg:viewBox", "0 0 318 318");
        marker.addAttribute("svg:d", "m159 0 159 159-159 159-159-159z");
        break;
    case msolineArrowOvalEnd:
        marker.addAttribute("svg:viewBox", "0 0 318 318");
        marker.addAttribute("svg:d", "m318 0c0-87-72-159-159-159s-159 72-159 159 72 159 159 159 159-72 159-159z");
        break;
    case msolineArrowOpenEnd:
        marker.addAttribute("svg:viewBox", "0 0 477 477");
        marker.addAttribute("svg:d", "m239 0 238 434-72 43-166-305-167 305-72-43z");
        break;
    case msolineArrowEnd:
    default:
        marker.addAttribute("svg:viewBox", "0 0 318 318");
        marker.addAttribute("svg:d", "m159 0 159 318h-318z");
        break;
    }
    return styles.insert(marker, name, KoGenStyles::DontAddNumberToName);
}

void ODrawToOdf::defineGradientStyle(KoGenStyle& style, const DrawStyle& ds)
{
    // TODO: another fill types

    // convert angle to two points representing crossing of
    // the line with rectangle to use it in svg
    // size of rectangle is 100*100 with the middle in 0,0
    // line coordinates are x1,y1; 0,0; x2,y2
    int dx=0,dy=0;
    int angle = (int)toQReal(ds.fillAngle());

    // from observations of the documents it seems
    // that angle is stored in -180,180 in MS 2003 documents
    if (angle < 0) {
        angle = angle + 180;
    }

    // 0 angle means that the angle is actually 90 degrees
    // From docs: Zero angle represents the vector from bottom to top. [MS-ODRAW:fillAngle], p.198
    angle = (angle + 90) % 360;

    qreal cosA = cos(angle * M_PI / 180);
    qreal sinA = sin(angle * M_PI / 180);

    if ((angle >= 0 && angle < 45) || (angle >= 315 && angle <= 360)) {
        dx = 50;
        dy = sinA/cosA * 50;
    } else if (angle >= 45 && angle < 135) {
        dy = 50;
        dx = cosA/sinA * 50;
    } else if  (angle >= 135 && angle < 225) {
        dx = -50;
        dy = sinA/cosA*(-50);
    } else {
        dy = -50;
        dx = cosA/sinA * (-50);
    }

    style.addAttribute("svg:spreadMethod", "reflect");

    int x1 = 50 - dx;
    int y1 = 50 + dy;
    int x2 = 50 + dx;
    int y2 = 50 - dy;

    if (ds.fillFocus() == 100) {
        qSwap(x1,x2);
        qSwap(y1,y2);
    } else if (ds.fillFocus() == 50) {
        int sx = (x2 - x1) * 0.5;
        int sy = (y2 - y1) * 0.5;
        x2 = x1 +  sx;
        y2 = y1 +  sy;

        // in one case don't swap the gradient vector
        if (angle != 90) {
            qSwap(x1,x2);
            qSwap(y1,y2);
        }
    } else if (ds.fillFocus() == -50) {
        int sx = (x2 - x1) * 0.5;
        int sy = (y2 - y1) * 0.5;
        x2 = x1 + sx;
        y2 = y1 + sy;
        // in this case we have to swap the gradient vector
        // check some gradient file from MS Office 2003
        if (angle == 90) {
            qSwap(x1,x2);
            qSwap(y1,y2);
        }
    }

    QBuffer writerBuffer;
    writerBuffer.open(QIODevice::WriteOnly);
    KoXmlWriter elementWriter(&writerBuffer);

    qreal fillOpacity = toQReal(ds.fillOpacity());
    qreal fillBackOpacity = toQReal(ds.fillBackOpacity());
    // if fillShadeColors() is not empty use the colors and points defined inside
    // if it is empty use the colors defined inside fillColor() and fillBackColor

    if (ds.fillShadeColors()) {
        style.addAttribute("svg:x1", QString("%1%").arg(x1));
        style.addAttribute("svg:y1", QString("%1%").arg(y1));
        style.addAttribute("svg:x2", QString("%1%").arg(x2));
        style.addAttribute("svg:y2", QString("%1%").arg(y2));

        IMsoArray a = ds.fillShadeColors_complex();

        QBuffer streamBuffer(&a.data);
        streamBuffer.open(QIODevice::ReadOnly);
        LEInputStream in(&streamBuffer);

        OfficeArtCOLORREF color;
        FixedPoint fixedPoint;
        for (int i = 0; i < a.nElems; i++) {
            try {
                parseOfficeArtCOLORREF(in,color);
            } catch (const IOException& e) {
                qDebug() << e.msg;
                break;
            } catch (...) {
                qDebug() << "Warning: Caught an unknown exception!";
                break;
            }
            try {
                parseFixedPoint(in,fixedPoint);
            } catch (const IOException& e) {
                qDebug() << e.msg;
                break;
            } catch (...) {
                qDebug() << "Warning: Caught an unknown exception!";
                break;
            }

            qreal offset = toQReal(fixedPoint);
            elementWriter.startElement("svg:stop");
            elementWriter.addAttribute("svg:offset", QString("%1").arg(offset));
            elementWriter.addAttribute("svg:stop-color", processOfficeArtCOLORREF(color, ds).name());
            qreal opacity = ((1.0 - offset) * fillBackOpacity + offset * fillOpacity);
            if (opacity != 1.0) {
                elementWriter.addAttribute("svg:stop-opacity", opacity);
            }
            elementWriter.endElement();
        }
        streamBuffer.close();
    } else {
        QColor fillColor = processOfficeArtCOLORREF(ds.fillColor(), ds);
        QColor backColor = processOfficeArtCOLORREF(ds.fillBackColor(), ds);

        if (ds.fillFocus() == 50){
            if (toQReal( ds.fillAngle() ) > 0){
                qSwap(x1,x2);
                qSwap(y1,y2);
            }
        }

        style.addAttribute("svg:x1", QString("%1%").arg(x1));
        style.addAttribute("svg:y1", QString("%1%").arg(y1));
        style.addAttribute("svg:x2", QString("%1%").arg(x2));
        style.addAttribute("svg:y2", QString("%1%").arg(y2));

        elementWriter.startElement("svg:stop");
        elementWriter.addAttribute("svg:offset", "0");
        elementWriter.addAttribute("svg:stop-color", fillColor.name());
        if (fillOpacity != 1.0) {
            elementWriter.addAttribute("svg:stop-opacity", fillOpacity);
        }
        elementWriter.endElement();

        elementWriter.startElement("svg:stop");
        elementWriter.addAttribute("svg:offset", "1");
        elementWriter.addAttribute("svg:stop-color", backColor.name());
        if (fillBackOpacity != 1.0) {
            elementWriter.addAttribute("svg:stop-opacity", fillBackOpacity);
        }
        elementWriter.endElement();
    }

    QString elementContents = QString::fromUtf8(writerBuffer.buffer(), writerBuffer.buffer().size());
    style.addChildElement("svg:stop", elementContents);
}

QString ODrawToOdf::defineDashStyle(KoGenStyles& styles, const quint32 lineDashing)
{
    if (lineDashing <= 0 || lineDashing > 10) {
        return QString();
    }

    KoGenStyle strokeDash(KoGenStyle::StrokeDashStyle);
    switch (lineDashing) {
    case msolineSolid:
        break;
    case msolineDashSys:
        strokeDash.addAttribute("draw:dots1", "1");
        strokeDash.addAttribute("draw:dots1-length", "300%");
        strokeDash.addAttribute("draw:distance", "100%");
        break;
    case msolineDotSys:
        strokeDash.addAttribute("draw:dots1", "1");
        strokeDash.addAttribute("draw:dots1-length", "200%");
        break;
    case msolineDashDotSys:
        strokeDash.addAttribute("draw:dots1", "1");
        strokeDash.addAttribute("draw:dots1-length", "300%");
        strokeDash.addAttribute("draw:dots2", "1");
        strokeDash.addAttribute("draw:dots2-length", "100%");
        break;
    case msolineDashDotDotSys:
        strokeDash.addAttribute("draw:dots1", "1");
        strokeDash.addAttribute("draw:dots1-length", "300%");
        strokeDash.addAttribute("draw:dots2", "1");
        strokeDash.addAttribute("draw:dots2-length", "100%");
        break;
    case msolineDotGEL:
        strokeDash.addAttribute("draw:dots1", "1");
        strokeDash.addAttribute("draw:dots1-length", "100%");
        break;
    case msolineDashGEL:
        strokeDash.addAttribute("draw:dots1", "4");
        strokeDash.addAttribute("draw:dots1-length", "100%");
        break;
    case msolineLongDashGEL:
        strokeDash.addAttribute("draw:dots1", "8");
        strokeDash.addAttribute("draw:dots1-length", "100%");
        break;
    case msolineDashDotGEL:
        strokeDash.addAttribute("draw:dots1", "1");
        strokeDash.addAttribute("draw:dots1-length", "300%");
        strokeDash.addAttribute("draw:dots2", "1");
        strokeDash.addAttribute("draw:dots2-length", "100%");
        break;
    case msolineLongDashDotGEL:
        strokeDash.addAttribute("draw:dots1", "1");
        strokeDash.addAttribute("draw:dots1-length", "800%");
        strokeDash.addAttribute("draw:dots2", "1");
        strokeDash.addAttribute("draw:dots2-length", "100%");
        break;
    case msolineLongDashDotDotGEL:
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
    return styles.insert(strokeDash, QString("Dash_20_%1").arg(lineDashing),
                         KoGenStyles::DontAddNumberToName);
}

QColor ODrawToOdf::processOfficeArtCOLORREF(const MSO::OfficeArtCOLORREF& c, const DrawStyle& ds)
{
    static const QRgb systemColors[25] = {
        0xc0c0c0, 0x008080, 0x000080, 0x808080, 0xc0c0c0, 0xffffff, 0x000000,
        0x000000, 0x000000, 0xffffff, 0xc0c0c0, 0xc0c0c0, 0x808080, 0x000080,
        0xffffff, 0xc0c0c0, 0x808080, 0x808080, 0x000000, 0xc0c0c0, 0xffffff,
        0x000000, 0xc0c0c0, 0x000000, 0xffffc0
    };
    //TODO: implement all cases!!!
    QColor ret;
    MSO::OfficeArtCOLORREF tmp;

    // A value of 0x1 specifies that green and red will be treated as an
    // unsigned 16-bit index into the system color table.  Values less than
    // 0x00F0 map directly to system colors.  Table [1] specifies values that
    // have special meaning, [1] MS-ODRAW 2.2.2
    if (c.fSysIndex) {
        if (c.red >= 0xF0) {
            switch (c.red) {
            // Use the fill color of the shape.
            case 0xF0:
                tmp = ds.fillColor();
                break;
            // If the shape contains a line, use the line color of the
            // shape. Otherwise, use the fill color.
            case 0xF1:
            {
                if (ds.fLine()) {
                    tmp = ds.lineColor();
                } else {
                    tmp = ds.fillColor();
                }
                break;
            }
            // Use the line color of the shape.
            case 0xF2:
                tmp = ds.lineColor();
                break;
            // Use the shadow color of the shape.
            case 0xF3:
                tmp = ds.shadowColor();
                break;
            // TODO: Use the current, or last-used, color.
            case 0xF4:
                qWarning() << "red: Unhandled fSysIndex 0xF4!";
                break;
            // Use the fill background color of the shape.
            case 0xF5:
                tmp  = ds.fillBackColor();
                break;
            // TODO: Use the line background color of the shape.
            case 0xF6:
                qWarning() << "red: Unhandled fSysIndex 0xF6!";
                break;
            // If the shape contains a fill, use the fill color of the
            // shape. Otherwise, use the line color.
            case 0xF7:
            {
                if (ds.fFilled()) {
                    tmp = ds.fillColor();
                } else {
                    tmp = ds.lineColor();
                }
                break;
            }
            default:
                qWarning() << "red: Unhandled fSysIndex!" << c.red;
                break;
            }
        } else if (c.green == 0) {
            tmp = c;
            // system colors
            if (c.red < 25) {
                const QRgb& col = systemColors[c.red];
                tmp.red = qRed(col);
                tmp.green = qGreen(col);
                tmp.blue = qBlue(col);
            } else {
                qWarning() << "red: Unhandled system color" << c.red;
            }
        }

        ret = client->toQColor(tmp);
        qreal p = c.blue / (qreal) 255;

        switch (c.green & 0xF) {
        case 0x00: break; // do nothing
        // Darken the color by the value that is specified in the blue field.
        // A blue value of 0xFF specifies that the color is to be left
        // unchanged, whereas a blue value of 0x00 specifies that the color is
        // to be completely darkened.
        case 0x01:
        {
            if (c.blue == 0x00) {
                ret = ret.darker(800);
            } else if (c.blue != 0xFF) {
                ret.setRed(ceil(p * ret.red()));
                ret.setGreen(ceil(p * ret.green()));
                ret.setBlue(ceil(p * ret.blue()));
            }
            break;
        }
        // Lighten the color by the value that is specified in the blue field.
        // A blue value of 0xFF specifies that the color is to be left
        // unchanged, whereas a blue value of 0x00 specifies that the color is
        // to be completely lightened.
        case 0x02:
        {
            if (c.blue == 0x00) {
                ret = ret.lighter(150);
            } else if (c.blue != 0xFF) {
                ret.setRed(ret.red() + ceil(p * ret.red()));
                ret.setGreen(ret.green() + ceil(p * ret.green()));
                ret.setBlue(ret.blue() + ceil(p * ret.blue()));
            }
            break;
	}
        //TODO:
        case 0x03:
        case 0x04:
        case 0x05:
        case 0x06:
        default:
            qWarning() << "green: Unhandled fSysIndex!" << c.green;
            break;
        }
        // TODO
        if (c.green & 0x20) {
            qWarning() << "green: unhandled 0x20";
        }
        if (c.green & 0x40) {
            qWarning() << "green: unhandled 0x40";
        }
        if (c.green & 0x80) {
            qWarning() << "green: unhandled 0x80";
        }
    } else {
        ret = client->toQColor(c);
    }
    return ret;
}

const char* getFillRule(quint16 shapeType)
{
    switch (shapeType) {
    case msosptDonut:
    case msosptNoSmoking:
    case msosptActionButtonBlank:
    case msosptActionButtonHome:
    case msosptActionButtonHelp:
    case msosptActionButtonInformation:
    case msosptActionButtonForwardNext:
    case msosptActionButtonBackPrevious:
    case msosptActionButtonEnd:
    case msosptActionButtonBeginning:
    case msosptActionButtonReturn:
    case msosptActionButtonDocument:
    case msosptActionButtonSound:
    case msosptActionButtonMovie:
        return "evenodd";
    default:
        return "";
    }
}

const char* getFillType(quint32 fillType)
{
    switch (fillType) {
    case msofillPattern:
        // NOTE: there's usually a DIB file used for the pattern, check also
        // draw:fill="hatch" and <draw:hatch> in ODF specification
    case msofillTexture:
    case msofillPicture:
        return "bitmap";
    case msofillShade:
    case msofillShadeCenter:
    case msofillShadeShape:
    case msofillShadeScale:
    case msofillShadeTitle:
        return "gradient";
    case msofillBackground:
        return "none";
    case msofillSolid:
    default:
        return "solid";
    }
}

const char* getRepeatStyle(quint32 fillType)
{
    switch (fillType) {
    case msofillPicture:
    case msofillShadeScale:
        return "stretch";
    case msofillSolid:
    case msofillShade:
    case msofillShadeCenter:
    case msofillShadeShape:
    case msofillShadeTitle:
    case msofillBackground:
        return "no-repeat";
    case msofillPattern:
    case msofillTexture:
    default:
        return "repeat";
    }
}

const char* getGradientRendering(quint32 fillType)
{
    //TODO: Add the logic!!!
    switch (fillType) {
    case msofillSolid:
    case msofillPattern:
    case msofillTexture:
    case msofillPicture:
    case msofillShade:
    case msofillShadeCenter:
    case msofillShadeShape:
    case msofillShadeScale:
    case msofillShadeTitle:
    case msofillBackground:
    default:
        return "axial";
    }
}

const char* getHorizontalPos(quint32 posH)
{
    switch (posH) {
    case 0: // msophAbs
        return "from-left";
    case 1: // msophLeft
        return "left";
    case 2: // msophCenter
        return "center";
    case 3: // msophRight
        return "right";
    case 4: // msophInside
        return "inside";
    case 5: // msophOutside
        return "outside";
    default:
        return "from-left";
    }
}

const char* getHorizontalRel(quint32 posRelH)
{
    switch (posRelH) {
    case 0: //msoprhMargin
        return "page-content";
    case 1: //msoprhPage
        return "page";
    case 2: //msoprhText
        return "paragraph";
    case 3: //msoprhChar
        return "char";
    default:
        return "page-content";
    }
}

const char* getVerticalPos(quint32 posV)
{
    switch (posV) {
    case 0: // msophAbs
        return "from-top";
    case 1: // msophTop
        return "top";
    case 2: // msophCenter
        return "middle";
    case 3: // msophBottom
        return "bottom";
    case 4: // msophInside - not compatible with ODF
        return "top";
    case 5: // msophOutside - not compatible with ODF
        return "bottom";
    default:
        return "from-top";
    }
}

const char* getVerticalRel(quint32 posRelV)
{
    switch (posRelV) {
    case 0: //msoprvMargin
        return "page-content";
    case 1: //msoprvPage
        return "page";
    case 2: //msoprvText
        return "paragraph";
    case 3: //msoprvLine
        return "char";
    default:
        return "page-content";
    }
}

const char* getHorizontalAlign(quint32 anchorText)
{
    switch (anchorText) {
    case msoanchorTop:
    case msoanchorTopBaseline:
    case msoanchorMiddle:
    case msoanchorBottom:
    case msoanchorBottomBaseline:
        return "left";
    case msoanchorTopCentered:
    case msoanchorTopCenteredBaseline:
    case msoanchorMiddleCentered:
    case msoanchorBottomCentered:
    case msoanchorBottomCenteredBaseline:
        return "justify";
    default:
        return "left";
    }
}

const char* getVerticalAlign(quint32 anchorText)
{
    switch (anchorText) {
    case msoanchorTop:
    case msoanchorTopCentered:
    case msoanchorTopBaseline: //not compatible with ODF
    case msoanchorTopCenteredBaseline: //not compatible with ODF
        return "top";
    case msoanchorMiddle:
    case msoanchorMiddleCentered:
        return "middle";
    case msoanchorBottom:
    case msoanchorBottomCentered:
    case msoanchorBottomBaseline: //not compatible with ODF
    case msoanchorBottomCenteredBaseline: //not compatible with ODF
        return "bottom";
    default:
        return "top";
    }
}

const char* getStrokeLineCap(quint32 capStyle)
{
    switch (capStyle) {
    case msolineEndCapRound:
        return "round";
    case msolineEndCapSquare:
        return "square";
    case msolineEndCapFlat:
    default:
        return "butt";
    }
}

const char* getStrokeLineJoin(quint32 joinStyle)
{
    switch (joinStyle) {
    case msolineJoinBevel:
        return "bevel";
    case msolineJoinMiter:
        return "miter";
    case msolineJoinRound:
    default:
        return "round";
    }
}
