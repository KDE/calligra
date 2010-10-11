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

#include "ODrawToOdf.h"
#include "drawstyle.h"
#include <KoXmlWriter.h>
#include <QtCore/QtDebug>
#include <QtGui/QColor>
#include <qbuffer.h>
#include "generated/leinputstream.h"

#include <cmath>

using namespace MSO;

/**
 * Return the bounding rectangle for this object.
 **/
QRectF
ODrawToOdf::getRect(const OfficeArtFSPGR &r)
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
        if (sp->clientAnchor && sp->shapeGroup) {
            oldCoords = client->getRect(*sp->clientAnchor);
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
    }
}
void ODrawToOdf::processDrawing(const OfficeArtSpgrContainerFileBlock& of,
                                Writer& out)
{
    if (of.anon.is<OfficeArtSpgrContainer>()) {
        processGroup(*of.anon.get<OfficeArtSpgrContainer>(), out);
    } else { // OfficeArtSpContainer
        processDrawingObject(*of.anon.get<OfficeArtSpContainer>(), out);
    }
}
void ODrawToOdf::processGroup(const MSO::OfficeArtSpgrContainer& o, Writer& out)
{
    if (o.rgfb.size() < 2) return;
    out.xml.startElement("draw:g");
    /* if the first OfficeArtSpContainer has a clientAnchor,
       a new coordinate system is introduced.
       */
    const OfficeArtSpContainer* first
    = o.rgfb[0].anon.get<OfficeArtSpContainer>();
    QRectF oldCoords;
    if (first && first->shapeGroup && first->clientAnchor) {
        oldCoords = client->getRect(*first->clientAnchor);
    }
    if (oldCoords.isValid()) {
        QRectF newCoords = getRect(*first->shapeGroup);
        Writer transformedOut = out.transform(oldCoords, newCoords);
        for (int i = 1; i < o.rgfb.size(); ++i) {
            processDrawing(o.rgfb[i], transformedOut);
        }
    } else {
        for (int i = 1; i < o.rgfb.size(); ++i) {
            processDrawing(o.rgfb[i], out);
        }
    }
    out.xml.endElement(); // draw:g
}
void ODrawToOdf::addGraphicStyleToDrawElement(Writer& out,
        const OfficeArtSpContainer& o)
{
    KoGenStyle style;
    const OfficeArtDggContainer* drawingGroup = 0;
    const OfficeArtSpContainer* master = 0;

    if (client) {
        style = client->createGraphicStyle(o.clientTextbox.data(),
                                           o.clientData.data(), out);
        drawingGroup = client->getOfficeArtDggContainer();
    }
    if (!drawingGroup) return;

    //locate the OfficeArtSpContainer of the master shape
    if (o.shapeProp.fHaveMaster) {
        if (client) {
            const DrawStyle tmp(*drawingGroup, &o);
            quint32 spid = tmp.hspMaster();
            master = client->getMasterShapeContainer(spid);
        }
    }
    const DrawStyle ds(*drawingGroup, master, &o);
    defineGraphicProperties(style, ds, out.styles);

    client->addTextStyles(o.clientTextbox.data(),
                          o.clientData.data(), out, style);
}

namespace
{
const char* dashses[11] = {
    "", "Dash_20_2", "Dash_20_3", "Dash_20_2", "Dash_20_2", "Dash_20_2",
    "Dash_20_4", "Dash_20_6", "Dash_20_5", "Dash_20_7", "Dash_20_8"
};
const char* arrowHeads[6] = {
    "", "msArrowEnd_20_5", "msArrowStealthEnd_20_5", "msArrowDiamondEnd_20_5",
    "msArrowOvalEnd_20_5", "msArrowOpenEnd_20_5"
};
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
    // draw:auto-grow-width
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
        if (fillType == 0 && client) {
            QColor tmp = processOfficeArtCOLORREF(ds.fillColor(), ds);
            style.addProperty("draw:fill-color", tmp.name(), gt);
        }
        // draw:fill-gradient-name
        else if ((fillType >=4 && fillType <=8) && client) {
            KoGenStyle gs(KoGenStyle::LinearGradientStyle);
            defineGradientStyle(gs, ds);
            QString tmp = styles.insert(gs);
            style.addProperty("draw:fill-gradient-name", tmp, gt);
        }
        // draw:fill-hatch-name
        // draw:fill-hatch-solid
        // draw:fill-image-height
        // draw:fill-image-name
        quint32 fillBlip = ds.fillBlip();
        QString fillImagePath;
        if (client) {
            fillImagePath = client->getPicturePath(fillBlip);
        }
        if (!fillImagePath.isEmpty()) {
            style.addProperty("draw:fill-image-name",
                              "fillImage" + QString::number(fillBlip), gt);
        }
        // draw:fill-image-ref-point
        // draw:fill-image-ref-point-x
        // draw:fill-image-ref-point-y
        // draw:fill-image-width
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
    qreal lineWidthPt = 0;
    if (ds.fLine()) {
        // draw:marker-end
        quint32 lineEndArrowhead = ds.lineEndArrowhead();
        if (lineEndArrowhead > 0 && lineEndArrowhead < 6) {
            style.addProperty("draw:marker-end", arrowHeads[lineEndArrowhead], gt);
        }
        // draw:marker-end-center
        // draw:marker-end-width
        lineWidthPt = ds.lineWidth() / 12700.;
        style.addProperty("draw:marker-end-width",
                          pt(lineWidthPt*4*(1+ds.lineEndArrowWidth())), gt);
        // draw:marker-start
        quint32 lineStartArrowhead = ds.lineStartArrowhead();
        if (lineStartArrowhead > 0 && lineStartArrowhead < 6) {
            style.addProperty("draw:marker-start", arrowHeads[lineStartArrowhead], gt);
        }
        // draw:marker-start-center
        // draw:marker-start-width
        style.addProperty("draw:marker-start-width",
                          pt(lineWidthPt*4*(1+ds.lineStartArrowWidth())), gt);
    }
    // draw:measure-align
    // draw:measure-vertical-align
    // draw:ole-draw-aspect
    // draw:opacity
    style.addProperty("draw:opacity",
                      percent(100.0 * toQReal(ds.fillOpacity()) / 0x10000), gt);
    // draw:opacity-name
    // draw:parallel
    // draw:placing
    // draw:red
    // draw:secondary-fill-color

    // NOTE: fShadow property specifies whether the shape has a shadow.
    if (ds.fShadow()) {
        // draw:shadow
        style.addProperty("draw:shadow", "visible", gt);
        // draw:shadow-color
        OfficeArtCOLORREF clr = ds.shadowColor();
        style.addProperty("draw:fill-color", QColor(clr.red, clr.green, clr.blue).name(), gt);
        // draw:shadow-offset-x
        style.addProperty("draw:shadow-offset-x", pt(ds.shadowOffsetX()/12700.),gt);
        // draw:shadow-offset-y
        style.addProperty("draw:shadow-offset-y", pt(ds.shadowOffsetY()/12700.),gt);
        // draw:shadow-opacity
        float shadowOpacity = toQReal(ds.shadowOpacity());
        style.addProperty("draw:shadow-opacity", percent(100*shadowOpacity), gt);
    }
    // draw:show-unit
    // draw:start-guide
    // draw:start-line-spacing-horizontal
    // draw:start-line-spacing-vertical

    // draw:stroke ('dash', 'none' or 'solid')
    // NOTE: OOo interprets solid line with width 0 as hairline, so if width ==
    // 0, stroke *must* be none to avoid OOo from displaying a line
    if (ds.fLine() || ds.fNoLineDrawDash()) {
        quint32 lineDashing = ds.lineDashing();
        if (lineWidthPt == 0) {
            style.addProperty("draw:stroke", "none", gt);
        } else if (lineDashing > 0 && lineDashing < 11) {
            style.addProperty("draw:stroke", "dash", gt);
            // draw:stroke-dash from 2.3.8.17 lineDashing
            style.addProperty("draw:stroke-dash", dashses[lineDashing], gt);
        } else {
            style.addProperty("draw:stroke", "solid", gt);
        }

    } else {
        style.addProperty("draw:stroke", "none", gt);
    }
    // draw:stroke-dash-names
    // draw:stroke-linejoin
    // draw:symbol-color
    // draw:textarea-horizontal-align
    // draw:textarea-vertical-align
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
    // fo:max-height
    // fo:max-width
    // fo:min-height
    // fo:min-width
    // fo:padding
    // fo:padding-bottom
    // fo:padding-left
    // fo:padding-right
    // fo:padding-top
    // fo:wrap-option
    // style:border-line-width
    // style:border-line-width-bottom
    // style:border-line-width-left
    // style:border-line-width-right
    // style:border-line-width-top
    // style:editable
    // style:flow-with-text
    // style:horizontal-pos
    // style:horizontal-rel
    // style:mirror
    // style:number-wrapped-paragraphs
    // style:overflow-behavior
    // style:print-content
    // style:protect
    // style:rel-height
    // style:rel-width
    // style:repeat
    // style:run-through
    // style:shadow
    // style:vertical-pos
    // style:vertical-rel
    // style:wrap
    // style:wrap-contour
    // style:wrap-contour-mode
    // style:wrap-dynamic-treshold
    // svg:fill-rule
    // svg:height
    if (ds.fLine() || ds.fNoLineDrawDash()) {
        if (client) {
            QColor tmp = processOfficeArtCOLORREF(ds.lineColor(), ds);
            // svg:stroke-color from 2.3.8.1 lineColor
            style.addProperty("svg:stroke-color", tmp.name(), gt);
        }
        // svg:stroke-opacity from 2.3.8.2 lineOpacity
        style.addProperty("svg:stroke-opacity",
                          percent(100.0 * ds.lineOpacity() / 0x10000), gt);
        // svg:stroke-width from 2.3.8.14 lineWidth
        style.addProperty("svg:stroke-width", pt(lineWidthPt), gt);
    }
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

void ODrawToOdf::defineGradientStyle(KoGenStyle& style, const DrawStyle& ds)
{
    // TODO another fill types

    // convert angle to two points representing crossing of the line with rectangle to use it in svg
    // size of rectangle is 100*100 with the middle in 0,0
    // line coordinates are x1,y1; 0,0; x2,y2
    int dx=0,dy=0;
    int angle = ((int)toQReal(ds.fillAngle())-90)%360;

    if (angle < 0) {
        angle = angle + 360;
    }

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
    style.addAttribute("svg:x1", QString("%1\%").arg(50 - dx));
    style.addAttribute("svg:x2", QString("%1\%").arg(50 + dx));
    style.addAttribute("svg:y1", QString("%1\%").arg(50 + dy));
    style.addAttribute("svg:y2", QString("%1\%").arg(50 - dy));

    QBuffer writerBuffer;
    writerBuffer.open(QIODevice::WriteOnly);
    KoXmlWriter elementWriter(&writerBuffer);

    // if fillShadeColors() is not empty use the colors and points defined inside
    // if it is empty use the colors defined inside fillColor() and fillBackColor

    if (ds.fillShadeColors()) {
        IMsoArray a = ds.fillShadeColors_complex();

        QBuffer streamBuffer(&a.data);
        streamBuffer.open(QIODevice::ReadOnly);
        LEInputStream in(&streamBuffer);

        OfficeArtCOLORREF color;
        FixedPoint fixedPoint;
        for (int i=0; i<a.nElems; i++) {
            parseOfficeArtCOLORREF(in,color);
            parseFixedPoint(in,fixedPoint);

            elementWriter.startElement("svg:stop");
            elementWriter.addAttribute("svg:offset", QString("%1").arg(toQReal(fixedPoint)));
            elementWriter.addAttribute("svg:stop-color", processOfficeArtCOLORREF(color, ds).name());
            elementWriter.endElement();
        }
        streamBuffer.close();
    } else {

        QColor fillColor = processOfficeArtCOLORREF(ds.fillColor(), ds);
        QColor backColor = processOfficeArtCOLORREF(ds.fillBackColor(), ds);

        // if the angle is negative the colors are swapped
        if (toQReal(ds.fillAngle()) >= 0) {
            QColor tempColor = fillColor;
            fillColor = backColor;
            backColor = tempColor;
        }

        // if fillFocus() is 0 or 100 than use only two colors for gradient fill
        // else place one of the colors at 0 and 1 position of gradient fill and the second color between them
        int fillFocus = ds.fillFocus();
        if (fillFocus == 100) {
            elementWriter.startElement("svg:stop");
            elementWriter.addAttribute("svg:offset", "0");
            elementWriter.addAttribute("svg:stop-color", backColor.name());
            elementWriter.endElement();

            elementWriter.startElement("svg:stop");
            elementWriter.addAttribute("svg:offset", "1");
            elementWriter.addAttribute("svg:stop-color", fillColor.name());
            elementWriter.endElement();
        } else if (fillFocus == 0) {
            elementWriter.startElement("svg:stop");
            elementWriter.addAttribute("svg:offset", "0");
            elementWriter.addAttribute("svg:stop-color", fillColor.name());
            elementWriter.endElement();

            elementWriter.startElement("svg:stop");
            elementWriter.addAttribute("svg:offset", "1");
            elementWriter.addAttribute("svg:stop-color", backColor.name());
            elementWriter.endElement();
        } else if (fillFocus < 0) {
            elementWriter.startElement("svg:stop");
            elementWriter.addAttribute("svg:offset", "0");
            elementWriter.addAttribute("svg:stop-color", fillColor.name());
            elementWriter.endElement();

            elementWriter.startElement("svg:stop");
            elementWriter.addAttribute("svg:offset", QString("%1").arg((fillFocus+100)/100.0));
            elementWriter.addAttribute("svg:stop-color", backColor.name());
            elementWriter.endElement();

            elementWriter.startElement("svg:stop");
            elementWriter.addAttribute("svg:offset", "1");
            elementWriter.addAttribute("svg:stop-color", fillColor.name());
            elementWriter.endElement();
        } else {
            elementWriter.startElement("svg:stop");
            elementWriter.addAttribute("svg:offset", "0");
            elementWriter.addAttribute("svg:stop-color", backColor.name());
            elementWriter.endElement();

            elementWriter.startElement("svg:stop");
            elementWriter.addAttribute("svg:offset", QString("%1").arg((fillFocus)/100.0));
            elementWriter.addAttribute("svg:stop-color", fillColor.name());
            elementWriter.endElement();

            elementWriter.startElement("svg:stop");
            elementWriter.addAttribute("svg:offset", "1");
            elementWriter.addAttribute("svg:stop-color", backColor.name());
            elementWriter.endElement();
        }
    }

    QString elementContents = QString::fromUtf8(writerBuffer.buffer(), writerBuffer.buffer().size());
    style.addChildElement("svg:stop", elementContents);
}

QColor ODrawToOdf::processOfficeArtCOLORREF(const MSO::OfficeArtCOLORREF& c, const DrawStyle& ds)
{
    //TODO: implement all cases!!!
    QColor ret;
    MSO::OfficeArtCOLORREF tmp;

    // A value of 0x1 specifies that green and red will be treated as an
    // unsigned 16-bit index into the system color table.  Values less than
    // 0x00F0 map directly to system colors.  Table [1] specifies values that
    // have special meaning, [1] MS-ODRAW 2.2.2
    if (c.fSysIndex) {

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
            qWarning() << "red: Unhandled fSysIndex!";
            break;
        // Use the fill background color of the shape.
        case 0xF5:
            tmp  = ds.fillBackColor();
            break;
        // TODO: Use the line background color of the shape.
        case 0xF6:
            qWarning() << "red: Unhandled fSysIndex!";
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
            qWarning() << "red: Unhandled fSysIndex!";
            break;
        }
        ret = client->toQColor(tmp);
        qreal p = c.blue / (qreal) 255;

        switch (c.green) {
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
        case 0x20:
        case 0x40:
        case 0x80:
        default:
            qWarning() << "green: Unhandled fSysIndex!";
            break;
        }
    } else {
        ret = client->toQColor(c);
    }
    return ret;
}

const char* getFillType(quint32 fillType)
{
    switch (fillType) {
    case 1: // msofillPattern
        // NOTE: there's usually a DIB file used for the pattern, check also
        // draw:fill="hatch" and <draw:hatch> in ODF specification
    case 2: // msofillTexture
    case 3: // msofillPicture
        return "bitmap";
    case 4: // msofillShade
    case 5: // msofillShadeCenter
    case 6: // msofillShadeShape
    case 7: // msofillShadeScale
    case 8: // msofillShadeTitle
        return "gradient";
    case 9: // msofillBackground
        return "none";
    case 0: // msofillSolid
    default:
        return "solid";
    }
}

const char* getRepeatStyle(quint32 fillType)
{
    switch (fillType) {
    case 3: // msofillPicture
    case 7: // msofillShadeScale
        return "stretch";
    case 0: // msofillSolid
    case 4: // msofillShade
    case 5: // msofillShadeCenter
    case 6: // msofillShadeShape
    case 8: // msofillShadeTitle
    case 9: // msofillBackground
        return "no-repeat";
    case 1: // msofillPattern
    case 2: // msofillTexture
    default:
        return "repeat";
    }
}

const char* getGradientRendering(quint32 fillType)
{
    //TODO: Add the logic!!!
    switch (fillType) {
    case 0: //msofillSolid
    case 1: //msofillPattern
    case 2: //msofillTexture
    case 3: //msofillPicture
    case 4: //msofillShade
    case 5: //msofillShadeCenter
    case 6: //msofillShadeShape
    case 7: //msofillShadeScale
    case 8: //msofillShadeTitle
    case 9: //msofillBackground
    default:
        return "axial";
    }
}
