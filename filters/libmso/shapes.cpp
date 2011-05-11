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
#include "generated/leinputstream.h"

#include <KoXmlWriter.h>
#include <kdebug.h>

#include <QTransform>
#include <qbuffer.h>

#include <cmath>


using namespace MSO;

namespace
{
void equation(Writer& out, const char* name, const char* formula)
{
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:name", name);
    out.xml.addAttribute("draw:formula", formula);
    out.xml.endElement();
}
}

qint16
ODrawToOdf::normalizeRotation(qreal rotation)
{
    qint16 angle = ((qint16)rotation) % 360;
    if (angle < 0) {
        angle = angle + 360;
    }
    return angle;
}

/**
 * Return the bounding rectangle for this object.
 **/
QRectF
ODrawToOdf::getRect(const OfficeArtSpContainer &o)
{
    if (o.childAnchor) {
        const OfficeArtChildAnchor& r = *o.childAnchor;
        return QRect(r.xLeft, r.yTop, r.xRight - r.xLeft, r.yBottom - r.yTop);
    } else if (o.clientAnchor && client) {
        return client->getRect(*o.clientAnchor);
    }
    return QRect(0, 0, 1, 1);
}

QRectF
ODrawToOdf::processRect(const quint16 shapeType, const qreal rotation, QRectF &rect)
{
    bool transform_anchor = false;
    qint16 nrotation = normalizeRotation(rotation);

    //TODO: Add other shapes here!

    switch (shapeType) {
    case msosptNotPrimitive:
        if ( ((nrotation >= 45) && (nrotation < 135)) ||
             ((nrotation >= 225) && (nrotation < 315)))
        {
            transform_anchor = true;
        }
        break;
    default:
        break;
    }
    if (transform_anchor) {
        QPointF center = rect.center();
        QTransform transform;
        transform.rotate(90);
        rect = transform.mapRect(rect.translated(-center)).translated(center);
    }
    return rect;
}

void ODrawToOdf::processEllipse(const OfficeArtSpContainer& o, Writer& out)
{
    out.xml.startElement("draw:ellipse");
    processStyleAndText(o, out);
    out.xml.endElement(); // draw:ellipse
}

void ODrawToOdf::processRectangle(const OfficeArtSpContainer& o, Writer& out)
{
    out.xml.startElement("draw:frame");
    processStyle(o, out);
    out.xml.startElement("draw:text-box");
    processText(o, out);
    out.xml.endElement(); // draw:text-box
    out.xml.endElement(); // draw:frame
}

void ODrawToOdf::processRoundRectangle(const OfficeArtSpContainer& o, Writer& out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:type", "round-rectangle");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:path-stretchpoint-x", "10800");
    out.xml.addAttribute("draw:path-stretchpoint-y", "10800");

    const AdjustValue* adjustValue = get<AdjustValue>(o);
    if (adjustValue) {
        out.xml.addAttribute("draw:modifiers", adjustValue->adjustvalue);
    }

    out.xml.addAttribute("draw:enhanced-path", "M ?f7 0 X 0 ?f8 L 0 ?f9 Y ?f7 21600 L ?f10 21600 X 21600 ?f9 L 21600 ?f8 Y ?f10 0 Z N");

    equation(out, "f0", "45");
    equation(out, "f1", "$0 *sin(?f0 *(pi/180))");
    equation(out, "f2", "?f1 *3163/7636");
    equation(out, "f3", "left+?f2");
    equation(out, "f4", "top+?f2");
    equation(out, "f5", "right-?f2");
    equation(out, "f6", "bottom-?f2");
    equation(out, "f7", "left+$0");
    equation(out, "f8", "top+$0");
    equation(out, "f9", "bottom-$0");
    equation(out, "f10", "right-$0");
    out.xml.endElement(); // draw:enhanced-geometry
    out.xml.endElement(); // draw:custom-shape
}

void ODrawToOdf::processDiamond(const OfficeArtSpContainer& o, Writer& out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:type", "diamond");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:glue-points", "10800 0 0 10800 10800 21600 21600 10800");
    out.xml.addAttribute("draw:enhanced-path", "M 10800 0 L 21600 10800 10800 21600 0 10800 10800 0 Z N");
    out.xml.addAttribute("draw:text-areas", "5400 5400 16200 16200");
    out.xml.endElement();
    out.xml.endElement();
}

void ODrawToOdf::processTriangle(const OfficeArtSpContainer& o, Writer& out)
{
    /* draw IsocelesTriangle or RightTriangle */
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");

    setShapeMirroring(o, out);

    if (o.shapeProp.rh.recInstance == msosptRightTriangle) {
        out.xml.addAttribute("draw:type", "right-triangle");
        out.xml.addAttribute("draw:glue-points","10800 0 5400 10800 0 21600 10800 21600 21600 21600 16200 10800");
        out.xml.addAttribute("draw:text-areas","1900 12700 12700 19700");
        out.xml.addAttribute("draw:enhanced-path","M 0 0 L 21600 21600 0 21600 0 0 Z N");
    } else if (o.shapeProp.rh.recInstance == msosptIsocelesTriangle) {
        out.xml.addAttribute("draw:type", "isosceles-triangle");
        out.xml.addAttribute("draw:glue-points", "10800 0 ?f1 10800 0 21600 10800 21600 21600 21600 ?f7 10800");
        out.xml.addAttribute("draw:text-areas","?f1 10800 ?f2 18000 ?f3 7200 ?f4 21600");
        out.xml.addAttribute("draw:enhanced-path","M ?f0 0 L 21600 21600 0 21600 Z N");

        QList<int> defaultModifierValue;
        defaultModifierValue << 10800;
        processModifiers(o, out, defaultModifierValue);

        equation(out, "f0", "$0");
        equation(out, "f1", "$0 /2");
        equation(out, "f2", "?f1 +10800");
        equation(out, "f3", "$0 *2/3");
        equation(out, "f4", "?f3 +7200");
        equation(out, "f5", "21600-?f0");
        equation(out, "f6", "?f5 /2");
        equation(out, "f7", "21600-?f6");

        out.xml.startElement("draw:handle");
        out.xml.addAttribute("draw:handle-range-x-maximum", 21600);
        out.xml.addAttribute("draw:handle-range-x-minimum", 0);
        out.xml.addAttribute("draw:handle-position", "$0 top");
        out.xml.endElement();
    }
    out.xml.endElement();    // enhanced-geometry
    out.xml.endElement(); // custom-shape
}

void ODrawToOdf::processTrapezoid(const OfficeArtSpContainer& o, Writer& out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:type", "trapezoid");
    out.xml.addAttribute("draw:glue-points", "?f6 10800 10800 21600 ?f5 10800 10800 0");
    setShapeMirroring(o, out);
    out.xml.addAttribute("draw:text-areas","?f3 ?f3 ?f4 ?f4");
    out.xml.addAttribute("draw:enhanced-path","M 0 0 L 21600 0 ?f0 21600 ?f1 21600 Z N");

    QList<int> defaultModifierValue;
    defaultModifierValue << 5400;
    processModifiers(o, out, defaultModifierValue);

    equation(out, "f0", "21600-$0");
    equation(out, "f1", "$0");
    equation(out, "f2", "$0 *10/18");
    equation(out, "f3", "?f2 +1750");
    equation(out, "f4", "21600-?f3");
    equation(out, "f5", "$0 /2");
    equation(out, "f6", "21600-?f5");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-range-x-maximum", 10800);
    out.xml.addAttribute("draw:handle-range-x-minimum", 0);
    out.xml.addAttribute("draw:handle-position", "$0 bottom");
    out.xml.endElement(); // handle
    out.xml.endElement(); // enhanced-geometry
    out.xml.endElement(); // custom-shape
}

void ODrawToOdf::processParallelogram(const OfficeArtSpContainer& o, Writer& out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:type", "parallelogram");
    out.xml.addAttribute("draw:glue-points", "?f6 0 10800 ?f8 ?f11 10800 ?f9 21600 10800 ?f10 ?f5 10800");
    out.xml.addAttribute("draw:text-areas", "?f3 ?f3 ?f4 ?f4");
    setShapeMirroring(o, out);

    QList<int> defaultModifierValue;
    defaultModifierValue << 5400;
    processModifiers(o, out, defaultModifierValue);

    out.xml.addAttribute("draw:enhanced-path","M ?f0 0 L 21600 0 ?f1 21600 0 21600 Z N");
    equation(out, "f0", "$0");
    equation(out, "f1", "21600-$0");
    equation(out, "f2", "$0 *10/24");
    equation(out, "f3", "?f2 +1750");
    equation(out, "f4", "21600-?f3");
    equation(out, "f5", "?f0 /2");
    equation(out, "f6", "10800+?f5");
    equation(out, "f7", "?f0-10800");
    equation(out, "f8", "if(?f7,?f12,0");
    equation(out, "f9", "10800-?f5");
    equation(out, "f10", "if(?f7, ?f12, 21600");
    equation(out, "f11", "21600-?f5");
    equation(out, "f12", "21600*10800/?f0");
    equation(out, "f13", "21600-?f12");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-range-x-maximum", 21600);
    out.xml.addAttribute("draw:handle-range-x-minimum", 0);
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.endElement();
    out.xml.endElement(); // enhanced-geometry
    out.xml.endElement(); // custom-shape
}

void ODrawToOdf::processHexagon(const OfficeArtSpContainer& o, Writer& out)
{
    const OfficeArtDggContainer * drawingGroup = 0;
    if (client) {
        drawingGroup = client->getOfficeArtDggContainer();
    }
    const OfficeArtSpContainer* master = 0;
    const DrawStyle ds(drawingGroup, master, &o);

    // draw:modifier by default is 5400 for this shape
    QList<int> defaultModifierValue;
    defaultModifierValue << 5400;

    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:type", "hexagon");
    out.xml.addAttribute("draw:text-areas", "?f3 ?f3 ?f4 ?f4");
    processModifiers(o, out, defaultModifierValue);
    out.xml.addAttribute("svg:viewBox","0 0 21600 21600");
    out.xml.addAttribute("draw:glue-points", "5 0 0 5 5 10 10 5");
    out.xml.addAttribute("draw:enhanced-path","M ?f0 0 L ?f1 0 21600 10800 ?f1 21600 ?f0 21600 0 10800 Z N");
    equation(out, "f0", "$0");
    equation(out, "f1", "21600-$0");
    equation(out, "f2", "$0 *100/234");
    equation(out, "f3", "?f2 +1700");
    equation(out, "f4", "21600-?f3");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-range-x-maximum", 10800);
    out.xml.addAttribute("draw:handle-range-x-minimum", 0);
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.endElement();
    out.xml.endElement(); // enhanced-geometry
    out.xml.endElement(); // custom-shape
}

void ODrawToOdf::processPlus(const OfficeArtSpContainer& o, Writer& out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:type", "cross");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:glue-points", "10800 0 0 10800 10800 21600 21600 10800");
    out.xml.addAttribute("draw:path-stretchpoint-x","10800");
    out.xml.addAttribute("draw:path-stretchpoint-y","10800");
    out.xml.addAttribute("draw:text-areas","?f1 ?f1 ?f2 ?f3");
    setShapeMirroring(o,out);

    QList<int> defaultModifierValue;
    defaultModifierValue << 5400;
    processModifiers(o, out, defaultModifierValue);

    out.xml.addAttribute("draw:enhanced-path","M ?f1 0 L ?f2 0 ?f2 ?f1 21600 ?f1 21600 ?f3 ?f2 ?f3 ?f2 21600 ?f1 21600 ?f1 ?f3 0 ?f3 0 ?f1 ?f1 ?f1 ?f1 0 Z N");
    equation(out, "f0","$0 *10799/10800");
    equation(out, "f1","?f0 ");
    equation(out, "f2","right-?f0 ");
    equation(out, "f3","bottom-?f0 ");

    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-range-x-maximum", 10800);
    out.xml.addAttribute("draw:handle-range-x-minimum", 0);
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-switched","true");
    out.xml.endElement(); // handle
    out.xml.endElement(); // enhanced-geometry
    out.xml.endElement(); // custom-shape
}


void ODrawToOdf::processOctagon(const OfficeArtSpContainer& o, Writer& out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:type", "octagon");
    out.xml.addAttribute("draw:glue-points", "5 0 0 4.782 5 10 10 4.782");
    equation(out, "f0", "left+$0");
    equation(out, "f1", "top+$0");
    equation(out, "f2", "right-$0");
    equation(out, "f3", "bottom-$0");
    equation(out, "f4", "$0 /2");
    equation(out, "f5", "left+?f4");
    equation(out, "f6", "top+?f4");
    equation(out, "f7", "right-?f4");
    equation(out, "f8", "bottom-?f4");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-range-x-maximum", 10800);
    out.xml.addAttribute("draw:handle-range-x-minimum", 0);
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.endElement();
    out.xml.endElement(); // enhanced-geometry
    out.xml.endElement(); // custom-shape
}

void ODrawToOdf::processArrow(const OfficeArtSpContainer& o, Writer& out)
{
    // defaults for this shape
    QList<int> arrowModifierDefaults;
    arrowModifierDefaults << 5400 << 5400;

    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");

    if (o.shapeProp.rh.recInstance == msosptLeftArrow) {
        if (o.shapeProp.fFlipH)
            out.xml.addAttribute("draw:type", "right-arrow");
        else
            out.xml.addAttribute("draw:type", "left-arrow");
    } else if (o.shapeProp.rh.recInstance == msosptUpArrow) {
        out.xml.addAttribute("draw:type", "up-arrow");
    } else if (o.shapeProp.rh.recInstance == msosptDownArrow) {
        out.xml.addAttribute("draw:type", "down-arrow");
    }


    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:text-areas","?f7 ?f0 21600 ?f2");
    processModifiers(o, out, arrowModifierDefaults);
    out.xml.addAttribute("draw:enhanced-path","M 21600 ?f0 L ?f1 ?f0 ?f1 0 0 10800 ?f1 21600 ?f1 ?f2 21600 ?f2 Z N");

    equation(out, "f0", "$1");
    equation(out, "f1", "$0");
    equation(out, "f2", "21600-$1");
    equation(out, "f3", "21600-?f1");
    equation(out, "f4", "?f3 *?f0 /10800");
    equation(out, "f5", "?f1 +?f4");
    equation(out, "f6", "?f1 *?f0 /10800");
    equation(out, "f7", "?f1 -?f6");

    out.xml.startElement("draw:handle");
    if (o.shapeProp.rh.recInstance == msosptLeftArrow) {
        out.xml.addAttribute("draw:handle-range-x-maximum", 21600);
        out.xml.addAttribute("draw:handle-range-x-minimum", 0);
        out.xml.addAttribute("draw:handle-position", "$0 $1");
        out.xml.addAttribute("draw:handle-range-y-maximum", 10800);
        out.xml.addAttribute("draw:handle-range-y-minimum", 0);
    } else if (o.shapeProp.rh.recInstance == msosptUpArrow || o.shapeProp.rh.recInstance == msosptDownArrow) {
        out.xml.addAttribute("draw:handle-range-x-maximum", 10800);
        out.xml.addAttribute("draw:handle-range-x-minimum", 0);
        out.xml.addAttribute("draw:handle-position", "$1 $0");
        out.xml.addAttribute("draw:handle-range-y-maximum", 21600);
        out.xml.addAttribute("draw:handle-range-y-minimum", 0);
    }
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // draw:enhanced-geometry
    out.xml.endElement(); // draw:custom-shape
}


void ODrawToOdf::processLeftRightArrow(const MSO::OfficeArtSpContainer& o, Writer& out)
{
    QList<int> arrowModifierDefaults;
    arrowModifierDefaults << 5400 << 5400;

    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:type", "left-right-arrow");
    processModifiers(o, out, arrowModifierDefaults);
    out.xml.addAttribute("svg:viewBox","0 0 21600 21600");
    out.xml.addAttribute("draw:text-areas","?f5 ?f1 ?f6 ?f3");
    out.xml.addAttribute("draw:enhanced-path","M 0 10800 L ?f0 0 ?f0 ?f1 ?f2 ?f1 ?f2 0 21600 10800 ?f2 21600 ?f2 ?f3 ?f0 ?f3 ?f0 21600 Z N");

    equation(out, "f0" ,"$0 ");
    equation(out, "f1" ,"$1 ");
    equation(out, "f2" ,"21600-$0 ");
    equation(out, "f3" ,"21600-$1 ");
    equation(out, "f4" ,"10800-$1 ");
    equation(out, "f5" ,"$0 *?f4 /10800");
    equation(out, "f6" ,"21600-?f5 ");
    equation(out, "f7" ,"10800-$0 ");
    equation(out, "f8" ,"$1 *?f7 /10800");
    equation(out, "f9" ,"21600-?f8 ");

    out.xml.startElement("draw:handle");

    out.xml.addAttribute("draw:handle-position","$0 $1");
    out.xml.addAttribute("draw:handle-range-x-minimum","0");
    out.xml.addAttribute("draw:handle-range-x-maximum","10800");
    out.xml.addAttribute("draw:handle-range-y-minimum","0");
    out.xml.addAttribute("draw:handle-range-y-maximum","10800");

    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // draw:enhanced-geometry
    out.xml.endElement(); // draw:custom-shape
}



void ODrawToOdf::processLine(const OfficeArtSpContainer& o, Writer& out)
{
    const QRectF rect = getRect(o);
    qreal x1 = rect.x();
    qreal y1 = rect.y();
    qreal x2 = rect.x() + rect.width();
    qreal y2 = rect.y() + rect.height();

    if (o.shapeProp.fFlipV) {
        qSwap(y1, y2);
    }
    if (o.shapeProp.fFlipH) {
        qSwap(x1, x2);
    }

    out.xml.startElement("draw:line");
    out.xml.addAttribute("svg:y1", client->formatPos(out.vOffset(y1)));
    out.xml.addAttribute("svg:y2", client->formatPos(out.vOffset(y2)));
    out.xml.addAttribute("svg:x1", client->formatPos(out.hOffset(x1)));
    out.xml.addAttribute("svg:x2", client->formatPos(out.hOffset(x2)));
    addGraphicStyleToDrawElement(out, o);
    out.xml.addAttribute("draw:layer", "layout");
    processText(o, out);

    out.xml.endElement();
}

void ODrawToOdf::processStraightConnector1(const OfficeArtSpContainer& o, Writer& out)
{
    const QRectF rect = getRect(o);
    qreal x1 = rect.x();
    qreal y1 = rect.y();
    qreal x2 = rect.x() + rect.width();
    qreal y2 = rect.y() + rect.height();

    if (o.shapeProp.fFlipV) {
        qSwap(y1, y2);
    }
    if (o.shapeProp.fFlipH) {
        qSwap(x1, x2);
    }

    out.xml.startElement("draw:connector");
    out.xml.addAttribute("svg:x1", client->formatPos(out.hOffset(x1)));
    out.xml.addAttribute("svg:y1", client->formatPos(out.vOffset(y1)));
    out.xml.addAttribute("svg:x2", client->formatPos(out.hOffset(x2)));
    out.xml.addAttribute("svg:y2", client->formatPos(out.vOffset(y2)));
    addGraphicStyleToDrawElement(out, o);
    out.xml.addAttribute("draw:layer", "layout");
    out.xml.addAttribute("draw:type", "line");
    processText(o, out);
    out.xml.endElement();

}

void ODrawToOdf::processBentConnector3(const OfficeArtSpContainer& o, Writer& out)
{
    const OfficeArtDggContainer * drawingGroup = 0;
    if (client) {
        drawingGroup = client->getOfficeArtDggContainer();
    }

    const OfficeArtSpContainer* master = 0;
    const DrawStyle ds(drawingGroup, master, &o);
    qreal rotation = toQReal( ds.rotation() );

    const QRectF rect = getRect(o);
    qreal x1 = rect.x();
    qreal y1 = rect.y();
    qreal x2 = rect.x() + rect.width();
    qreal y2 = rect.y() + rect.height();

    QRectF shapeRect = rect;

    qreal sx1 = x1;
    qreal sy1 = y1;
    qreal sx2 = x2;
    qreal sy2 = y2;

    if (rotation != 0.0) {
        QTransform m;
        m.rotate( -rotation );
        shapeRect = m.mapRect(rect.translated(-rect.center())).translated(rect.center());

        sx1 = shapeRect.topLeft().x();
        sy1 = shapeRect.topLeft().y();
        sx2 = shapeRect.bottomRight().x();
        sy2 = shapeRect.bottomRight().y();
    }

    // compute path
    QPainterPath shapePath;
    shapePath.moveTo(sx1,sy1);
    shapePath.lineTo((sx1 + sx2)/2.0, sy1);
    shapePath.lineTo((sx1 + sx2)/2.0, sy2);
    shapePath.lineTo(sx2,sy2);

    // transform the path according the shape properties like flip and rotation
    QTransform m;
    m.reset();
    m.translate( -shapeRect.center().x(), -shapeRect.center().y() );

    if (o.shapeProp.fFlipH){
        m.scale(-1,1);
    }

    if (o.shapeProp.fFlipV){
        m.scale(1,-1);
    }

    if (rotation != 0) {
        m.rotate(rotation);
    }

    m.translate( shapeRect.center().x(), shapeRect.center().y() );
    shapePath = m.map(shapePath);

    // translate the QPainterPath into svg:d attribute
    QString path = path2svg(shapePath);

    out.xml.startElement("draw:connector");
    addGraphicStyleToDrawElement(out, o);
    out.xml.addAttribute("draw:layer", "layout");
    out.xml.addAttribute("svg:x1", client->formatPos(out.hOffset(x1)));
    out.xml.addAttribute("svg:y1", client->formatPos(out.vOffset(y1)));
    out.xml.addAttribute("svg:x2", client->formatPos(out.hOffset(x2)));
    out.xml.addAttribute("svg:y2", client->formatPos(out.vOffset(y2)));
    if (!path.isEmpty()) {
        out.xml.addAttribute("svg:d", path);
    }

    processText(o, out);
    out.xml.endElement();

}




void ODrawToOdf::processSmiley(const OfficeArtSpContainer& o, Writer& out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:type", "smiley");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:glue-points", "10800 0 3163 3163 0 10800 3163 18437 10800 21600 18437 18437 21600 10800 18437 3163");
    out.xml.addAttribute("draw:text-areas", "3163 3163 18437 18437");
    out.xml.addAttribute("draw:modifiers", "17520");
    out.xml.addAttribute("draw:enhanced-path", "U 10800 10800 10800 10800 0 360 Z N U 7305 7515 1165 1165 0 360 Z N U 14295 7515 1165 1165 0 360 Z N M 4870 ?f1 C 8680 ?f2 12920 ?f2 16730 ?f1 F N");
    equation(out, "f0", "$0-15510");
    equation(out, "f1", "17520-?f0");
    equation(out, "f2", "15510+?f0");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-range-y-maximum", 17520);
    out.xml.addAttribute("draw:handle-range-y-minimum", 15510);
    out.xml.addAttribute("draw:handle-position", "10800 $0");
    out.xml.endElement();
    out.xml.endElement(); // enhanced-geometry
    out.xml.endElement(); // custom-shape
}

void ODrawToOdf::processHeart(const OfficeArtSpContainer& o, Writer& out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:type", "heart");
    out.xml.addAttribute("draw:glue-points", "5 1 1.43 5 5 10 8.553 5");

    out.xml.endElement(); // enhanced-geometry
    out.xml.endElement(); // custom-shape
}

void ODrawToOdf::processWedgeRectCallout(const MSO::OfficeArtSpContainer& o, Writer& out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:type", "rectangular-callout");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:glue-points", "10800 0 0 10800 10800 21600 21600 10800 ?f40 ?f41");
    out.xml.addAttribute("draw:text-areas", "0 0 21600 21600");
    out.xml.addAttribute("draw:modifiers", "514 25920");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L 0 3590 ?f2 ?f3 0 8970 0 12630 ?f4 ?f5 0 18010 0 21600 3590 21600 ?f6 ?f7 8970 21600 12630 21600 ?f8 ?f9 18010 21600 21600 21600 21600 18010 ?f10 ?f11 21600 12630 21600 8970 ?f12 ?f13 21600 3590 21600 0 18010 0 ?f14 ?f15 12630 0 8970 0 ?f16 ?f17 3590 0 0 0 Z N");
    equation(out, "f0", "$0 -10800");
    equation(out, "f1", "$1 -10800");
    equation(out, "f2", "if(?f18 ,$0 ,0)");
    equation(out, "f3", "if(?f18 ,$1 ,6280)");
    equation(out, "f4", "if(?f23 ,$0 ,0)");
    equation(out, "f5", "if(?f23 ,$1 ,15320)");
    equation(out, "f6", "if(?f26 ,$0 ,6280)");
    equation(out, "f7", "if(?f26 ,$1 ,21600)");
    equation(out, "f8", "if(?f29 ,$0 ,15320)");
    equation(out, "f9", "if(?f29 ,$1 ,21600)");
    equation(out, "f10", "if(?f32 ,$0 ,21600)");
    equation(out, "f11", "if(?f32 ,$1 ,15320)");
    equation(out, "f12", "if(?f34 ,$0 ,21600)");
    equation(out, "f13", "if(?f34 ,$1 ,6280)");
    equation(out, "f14", "if(?f36 ,$0 ,15320)");
    equation(out, "f15", "if(?f36 ,$1 ,0)");
    equation(out, "f16", "if(?f38 ,$0 ,6280)");
    equation(out, "f17", "if(?f38 ,$1 ,0)");
    equation(out, "f18", "if($0 ,-1,?f19 )");
    equation(out, "f19", "if(?f1 ,-1,?f22 )");
    equation(out, "f20", "abs(?f0 )");
    equation(out, "f21", "abs(?f1 )");
    equation(out, "f22", "?f20 -?f21");
    equation(out, "f23", "if($0 ,-1,?f24 )");
    equation(out, "f24", "if(?f1 ,?f22 ,-1)");
    equation(out, "f25", "$1 -21600");
    equation(out, "f26", "if(?f25 ,?f27 ,-1)");
    equation(out, "f27", "if(?f0 ,-1,?f28 )");
    equation(out, "f28", "?f21 -?f20 ");
    equation(out, "f29", "if(?f25 ,?f30 ,-1)");
    equation(out, "f30", "if(?f0 ,?f28 ,-1)");
    equation(out, "f31", "$0 -21600");
    equation(out, "f32", "if(?f31 ,?f33 ,-1)");
    equation(out, "f33", "if(?f1 ,?f22 ,-1)");
    equation(out, "f34", "if(?f31 ,?f35 ,-1)");
    equation(out, "f35", "if(?f1 ,-1,?f22 )");
    equation(out, "f36", "if($1 ,-1,?f37 )");
    equation(out, "f37", "if(?f0 ,?f28 ,-1)");
    equation(out, "f38", "if($1 ,-1,?f39 )");
    equation(out, "f39", "if(?f0 ,-1,?f28 )");
    equation(out, "f40", "$0");
    equation(out, "f41", "$1");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 $1");
    out.xml.endElement();
    out.xml.endElement();
    out.xml.endElement(); // custom-shape
}

void ODrawToOdf::processWedgeEllipseCallout(const MSO::OfficeArtSpContainer& o, Writer& out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:type", "round-callout");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:glue-points", "10800 0 3160 3160 0 10800 3160 18440 10800 21600 18440 18440 21600 10800 18440 3160 ?f14 ?f15");
    out.xml.addAttribute("draw:text-areas", "3200 3200 18400 18400");
    out.xml.addAttribute("draw:modifiers", "0 11500");
    out.xml.addAttribute("draw:enhanced-path", "W 0 0 21600 21600 ?f22 ?f23 ?f18 ?f19 L ?f14 ?f15 Z N");
    equation(out, "f0", "$0 -10800");
    equation(out, "f1", "$1 -10800");
    equation(out, "f2", "?f0 *?f0");
    equation(out, "f3", "?f1 *?f1");
    equation(out, "f4", "?f2 +?f3");
    equation(out, "f5", "sqrt(?f4 )");
    equation(out, "f6", "?f5 -10800");
    equation(out, "f7", "atan2(?f1 ,?f0 )/(pi/180)");
    equation(out, "f8", "?f7 -10");
    equation(out, "f9", "?f7 +10");
    equation(out, "f10", "10800*cos(?f7 *(pi/180))");
    equation(out, "f11", "10800*sin(?f7 *(pi/180))");
    equation(out, "f12", "?f10 +10800");
    equation(out, "f13", "?f11 +10800");
    equation(out, "f14", "if(?f6 ,$0 ,?f12 )");
    equation(out, "f15", "if(?f6 ,$1 ,?f13 )");
    equation(out, "f16", "10800*cos(?f8 *(pi/180))");
    equation(out, "f17", "10800*sin(?f8 *(pi/180))");
    equation(out, "f18", "?f16 +10800");
    equation(out, "f19", "?f17 +10800");
    equation(out, "f20", "10800*cos(?f9 *(pi/180))");
    equation(out, "f21", "10800*sin(?f9 *(pi/180))");
    equation(out, "f22", "?f20 +10800");
    equation(out, "f23", "?f21 +10800");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 $1");
    out.xml.endElement();
    out.xml.endElement();
    out.xml.endElement(); // custom-shape
}

void ODrawToOdf::processCloudCallout(const MSO::OfficeArtSpContainer& o, Writer& out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:type", "cloud-callout");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:text-areas", "3000 3320 17110 17330");
    out.xml.addAttribute("draw:modifiers", "9683 13275");
    out.xml.addAttribute("draw:enhanced-path", "M 1930 7160 C 1530 4490 3400 1970 5270 1970 5860 1950 6470 2210 6970 2600 7450 1390 8340 650 9340 650 10004 690 10710 1050 11210 1700 11570 630 12330 0 13150 0 13840 0 14470 460 14870 1160 15330 440 16020 0 16740 0 17910 0 18900 1130 19110 2710 20240 3150 21060 4580 21060 6220 21060 6720 21000 7200 20830 7660 21310 8460 21600 9450 21600 10460 21600 12750 20310 14680 18650 15010 18650 17200 17370 18920 15770 18920 15220 18920 14700 18710 14240 18310 13820 20240 12490 21600 11000 21600 9890 21600 8840 20790 8210 19510 7620 20000 7930 20290 6240 20290 4850 20290 3570 19280 2900 17640 1300 17600 480 16300 480 14660 480 13900 690 13210 1070 12640 380 12160 0 11210 0 10120 0 8590 840 7330 1930 7160 Z N M 1930 7160 C 1950 7410 2040 7690 2090 7920 F N M 6970 2600 C 7200 2790 7480 3050 7670 3310 F N M 11210 1700 C 11130 1910 11080 2160 11030 2400 F N M 14870 1160 C 14720 1400 14640 1720 14540 2010 F N M 19110 2710 C 19130 2890 19230 3290 19190 3380 F N M 20830 7660 C 20660 8170 20430 8620 20110 8990 F N M 18660 15010 C 18740 14200 18280 12200 17000 11450 F N M 14240 18310 C 14320 17980 14350 17680 14370 17360 F N M 8220 19510 C 8060 19250 7960 18950 7860 18640 F N M 2900 17640 C 3090 17600 3280 17540 3460 17450 F N M 1070 12640 C 1400 12900 1780 13130 2330 13040 F N U ?f17 ?f18 1800 1800 0 360 Z N U ?f19 ?f20 1200 1200 0 360 Z N U ?f13 ?f14 700 700 0 360 Z N");
    equation(out, "f0", "$0 -10800");
    equation(out, "f1", "$1 -10800");
    equation(out, "f2", "atan2(?f1 ,?f0 )/(pi/180)");
    equation(out, "f3", "10800*cos(?f2 *(pi/180))");
    equation(out, "f4", "10800*sin(?f2 *(pi/180))");
    equation(out, "f5", "?f3 +10800");
    equation(out, "f6", "?f4 +10800");
    equation(out, "f7", "$0 -?f5 ");
    equation(out, "f8", "$1 -?f6 ");
    equation(out, "f9", "?f7 /3");
    equation(out, "f10", "?f8 /3");
    equation(out, "f11", "?f7 *2/3");
    equation(out, "f12", "?f8 *2/3");
    equation(out, "f13", "$0 ");
    equation(out, "f14", "$1 ");
    equation(out, "f15", "?f3 /12");
    equation(out, "f16", "?f4 /12");
    equation(out, "f17", "?f9 +?f5 -?f15 ");
    equation(out, "f18", "?f10 +?f6 -?f16 ");
    equation(out, "f19", "?f11 +?f5 ");
    equation(out, "f20", "?f12 +?f6 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 $1");
    out.xml.endElement();
    out.xml.endElement();
    out.xml.endElement(); // custom-shape
}

void ODrawToOdf::processQuadArrow(const OfficeArtSpContainer& o, Writer& out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:text-areas", "0 0 21600 21600");
    out.xml.addAttribute("draw:type", "quad-arrow");
    out.xml.addAttribute("draw:modifiers", "6500 8600 4300");
    out.xml.addAttribute("draw:enhanced-path", "M 0 10800 L ?f0 ?f1 ?f0 ?f2 ?f2 ?f2 ?f2 ?f0 ?f1 ?f0 10800 0 ?f3 ?f0 ?f4 ?f0 ?f4 ?f2 ?f5 ?f2 ?f5 ?f1 21600 10800 ?f5 ?f3 ?f5 ?f4 ?f4 ?f4 ?f4 ?f5 ?f3 ?f5 10800 21600 ?f1 ?f5 ?f2 ?f5 ?f2 ?f4 ?f0 ?f4 ?f0 ?f3 Z N");
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:name", "f0");
    out.xml.addAttribute("draw:formula", "$2");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:name", "f1");
    out.xml.addAttribute("draw:formula", "$0");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:name", "f2");
    out.xml.addAttribute("draw:formula", "$1");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:name", "f3");
    out.xml.addAttribute("draw:formula", "21600-$0");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:name", "f4");
    out.xml.addAttribute("draw:formula", "21600-$1");
    out.xml.endElement();
    out.xml.startElement("draw:equation");
    out.xml.addAttribute("draw:name", "f5");
    out.xml.addAttribute("draw:formula", "21600-$2");
    out.xml.endElement();
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$1 $2");
    out.xml.addAttribute("draw:handle-range-x-minimum", "$0");
    out.xml.addAttribute("draw:handle-range-x-maximum", "10800");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "$0");
    out.xml.endElement();
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-minimum", "$2");
    out.xml.addAttribute("draw:handle-range-x-maximum", "$1");
    out.xml.endElement();
    out.xml.endElement();
    out.xml.endElement(); // draw:custom-shape
}

void ODrawToOdf::processUturnArrow(const MSO::OfficeArtSpContainer& o, Writer& out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:text-areas", "0 8280 6110 21600");
    out.xml.addAttribute("draw:type", "mso-spt101");
    out.xml.addAttribute("draw:enhanced-path", "M 0 21600 L 0 8550 C 0 3540 4370 0 9270 0 13890 0 18570 3230 18600 8300 L 21600 8300 15680 14260 9700 8300 12500 8300 C 12320 6380 10870 5850 9320 5850 7770 5850 6040 6410 6110 8520 L 6110 21600 Z N");
    out.xml.endElement();
    out.xml.endElement(); // draw:custom-shape
}

void ODrawToOdf::processCircularArrow(const MSO::OfficeArtSpContainer &o, Writer &out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:text-areas", "0 0 26110 21600");
    out.xml.addAttribute("draw:type", "circular-arrow");
    out.xml.addAttribute("draw:modifiers", "-130 -80 7200"); // TODO: get from odraw shape
    out.xml.addAttribute("draw:enhanced-path", "B ?f3 ?f3 ?f20 ?f20 ?f19 ?f18 ?f17 ?f16 W 0 0 21600 21600 ?f9 ?f8 ?f11 ?f10 L ?f24 ?f23 ?f47 ?f46 ?f29 ?f28 Z N");
    equation(out, "f0", "$0 ");
    equation(out, "f1", "$1 ");
    equation(out, "f2", "$2 ");
    equation(out, "f3", "10800+$2 ");
    equation(out, "f4", "10800*sin($0 *(pi/180))");
    equation(out, "f5", "10800*cos($0 *(pi/180))");
    equation(out, "f6", "10800*sin($1 *(pi/180))");
    equation(out, "f7", "10800*cos($1 *(pi/180))");
    equation(out, "f8", "?f4 +10800");
    equation(out, "f9", "?f5 +10800");
    equation(out, "f10", "?f6 +10800");
    equation(out, "f11", "?f7 +10800");
    equation(out, "f12", "?f3 *sin($0 *(pi/180))");
    equation(out, "f13", "?f3 *cos($0 *(pi/180))");
    equation(out, "f14", "?f3 *sin($1 *(pi/180))");
    equation(out, "f15", "?f3 *cos($1 *(pi/180))");
    equation(out, "f16", "?f12 +10800");
    equation(out, "f17", "?f13 +10800");
    equation(out, "f18", "?f14 +10800");
    equation(out, "f19", "?f15 +10800");
    equation(out, "f20", "21600-?f3 ");
    equation(out, "f21", "13500*sin($1 *(pi/180))");
    equation(out, "f22", "13500*cos($1 *(pi/180))");
    equation(out, "f23", "?f21 +10800");
    equation(out, "f24", "?f22 +10800");
    equation(out, "f25", "$2 -2700");
    equation(out, "f26", "?f25 *sin($1 *(pi/180))");
    equation(out, "f27", "?f25 *cos($1 *(pi/180))");
    equation(out, "f28", "?f26 +10800");
    equation(out, "f29", "?f27 +10800");
    equation(out, "f30", "?f29 -?f24 ");
    equation(out, "f31", "?f29 -?f24 ");
    equation(out, "f32", "?f30 *?f31 ");
    equation(out, "f33", "?f28 -?f23 ");
    equation(out, "f34", "?f28 -?f23 ");
    equation(out, "f35", "?f33 *?f34 ");
    equation(out, "f36", "?f32 +?f35 ");
    equation(out, "f37", "sqrt(?f36 )");
    equation(out, "f38", "$1 +45");
    equation(out, "f39", "?f37 *sin(?f38 *(pi/180))");
    equation(out, "f40", "$1 +45");
    equation(out, "f41", "?f37 *cos(?f40 *(pi/180))");
    equation(out, "f42", "45");
    equation(out, "f43", "?f39 *sin(?f42 *(pi/180))");
    equation(out, "f44", "45");
    equation(out, "f45", "?f41 *sin(?f44 *(pi/180))");
    equation(out, "f46", "?f28 +?f43 ");
    equation(out, "f47", "?f29 +?f45 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "10800 $0");
    out.xml.addAttribute("draw:handle-polar", "10800 10800");
    out.xml.addAttribute("draw:handle-radius-range-minimum", "10800");
    out.xml.addAttribute("draw:handle-radius-range-maximum", "10800");
    out.xml.endElement();
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$2 $1");
    out.xml.addAttribute("draw:handle-polar", "10800 10800");
    out.xml.addAttribute("draw:handle-radius-range-minimum", "0");
    out.xml.addAttribute("draw:handle-radius-range-maximum", "10800");
    out.xml.endElement();
    out.xml.endElement(); // draw:enhanced-geometry
    out.xml.endElement(); // draw:custom-shape
}

void ODrawToOdf::processIrregularSeal1(const MSO::OfficeArtSpContainer &o, Writer &out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:type", "mso-spt71");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:glue-points", "14623 106 106 8718 8590 21600 21600 13393");
    out.xml.addAttribute("draw:text-areas", "4680 6570 16140 13280");
    out.xml.addAttribute("draw:enhanced-path", "M 10901 5905 L 8458 2399 7417 6425 476 2399 4732 7722 106 8718 3828 11880 243 14689 5772 14041 4868 17719 7819 15730 8590 21600 10637 15038 13349 19840 14125 14561 18248 18195 16938 13044 21600 13393 17710 10579 21198 8242 16806 7417 18482 4560 14257 5429 14623 106 10901 5905 Z N");
    out.xml.endElement(); // draw:enhanced-geometry
    out.xml.endElement(); // draw:custom-shape
}

void ODrawToOdf::processLightningBolt(const MSO::OfficeArtSpContainer &o, Writer &out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:glue-points","8458 0 0 3923 4993 9720 9987 14934 21600 21600 16558 12016 12831 6120");
    out.xml.addAttribute("draw:text-areas","8680 7410 13970 14190");
    out.xml.addAttribute("draw:type","lightning");
    out.xml.addAttribute("draw:enhanced-path","M 8458 0 L 0 3923 7564 8416 4993 9720 12197 13904 9987 14934 21600 21600 14768 12911 16558 12016 11030 6840 12831 6120 8458 0 Z N");
    out.xml.endElement(); // draw:enhanced-geometry
    out.xml.endElement(); // draw:custom-shape
}

void ODrawToOdf::processSeal16(const MSO::OfficeArtSpContainer &o, Writer &out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:type", "mso-spt59");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:text-areas", "?f1 ?f2 ?f3 ?f4");
    out.xml.addAttribute("draw:modifiers", "2500");
    out.xml.addAttribute("draw:enhanced-path", "M ?f5 ?f6 L ?f7 ?f8 ?f9 ?f10 ?f11 ?f12 ?f13 ?f14 ?f15 ?f16 ?f17 ?f18 ?f19 ?f20 ?f21 ?f22 ?f23 ?f24 ?f25 ?f26 ?f27 ?f28 ?f29 ?f30 ?f31 ?f32 ?f33 ?f34 ?f35 ?f36 ?f37 ?f38 ?f39 ?f40 ?f41 ?f42 ?f43 ?f44 ?f45 ?f46 ?f47 ?f48 ?f49 ?f50 ?f51 ?f52 ?f53 ?f54 ?f55 ?f56 ?f57 ?f58 ?f59 ?f60 ?f61 ?f62 ?f63 ?f64 ?f65 ?f66 ?f67 ?f68 ?f5 ?f6 Z N");
    equation(out, "f0", "$0 ");
    equation(out, "f1", "(cos(315*(pi/180))*(?f0 -10800)+sin(315*(pi/180))*(10800-10800))+10800");
    equation(out, "f2", "-(sin(315*(pi/180))*(?f0 -10800)-cos(315*(pi/180))*(10800-10800))+10800");
    equation(out, "f3", "(cos(135*(pi/180))*(?f0 -10800)+sin(135*(pi/180))*(10800-10800))+10800");
    equation(out, "f4", "-(sin(135*(pi/180))*(?f0 -10800)-cos(135*(pi/180))*(10800-10800))+10800");
    equation(out, "f5", "(cos(0*(pi/180))*(0-10800)+sin(0*(pi/180))*(10800-10800))+10800");
    equation(out, "f6", "-(sin(0*(pi/180))*(0-10800)-cos(0*(pi/180))*(10800-10800))+10800");
    equation(out, "f7", "(cos(11*(pi/180))*(?f0 -10800)+sin(11*(pi/180))*(10800-10800))+10800");
    equation(out, "f8", "-(sin(11*(pi/180))*(?f0 -10800)-cos(11*(pi/180))*(10800-10800))+10800");
    equation(out, "f9", "(cos(22*(pi/180))*(0-10800)+sin(22*(pi/180))*(10800-10800))+10800");
    equation(out, "f10", "-(sin(22*(pi/180))*(0-10800)-cos(22*(pi/180))*(10800-10800))+10800");
    equation(out, "f11", "(cos(33*(pi/180))*(?f0 -10800)+sin(33*(pi/180))*(10800-10800))+10800");
    equation(out, "f12", "-(sin(33*(pi/180))*(?f0 -10800)-cos(33*(pi/180))*(10800-10800))+10800");
    equation(out, "f13", "(cos(45*(pi/180))*(0-10800)+sin(45*(pi/180))*(10800-10800))+10800");
    equation(out, "f14", "-(sin(45*(pi/180))*(0-10800)-cos(45*(pi/180))*(10800-10800))+10800");
    equation(out, "f15", "(cos(56*(pi/180))*(?f0 -10800)+sin(56*(pi/180))*(10800-10800))+10800");
    equation(out, "f16", "-(sin(56*(pi/180))*(?f0 -10800)-cos(56*(pi/180))*(10800-10800))+10800");
    equation(out, "f17", "(cos(67*(pi/180))*(0-10800)+sin(67*(pi/180))*(10800-10800))+10800");
    equation(out, "f18", "-(sin(67*(pi/180))*(0-10800)-cos(67*(pi/180))*(10800-10800))+10800");
    equation(out, "f19", "(cos(78*(pi/180))*(?f0 -10800)+sin(78*(pi/180))*(10800-10800))+10800");
    equation(out, "f20", "-(sin(78*(pi/180))*(?f0 -10800)-cos(78*(pi/180))*(10800-10800))+10800");
    equation(out, "f21", "(cos(90*(pi/180))*(0-10800)+sin(90*(pi/180))*(10800-10800))+10800");
    equation(out, "f22", "-(sin(90*(pi/180))*(0-10800)-cos(90*(pi/180))*(10800-10800))+10800");
    equation(out, "f23", "(cos(101*(pi/180))*(?f0 -10800)+sin(101*(pi/180))*(10800-10800))+10800");
    equation(out, "f24", "-(sin(101*(pi/180))*(?f0 -10800)-cos(101*(pi/180))*(10800-10800))+10800");
    equation(out, "f25", "(cos(112*(pi/180))*(0-10800)+sin(112*(pi/180))*(10800-10800))+10800");
    equation(out, "f26", "-(sin(112*(pi/180))*(0-10800)-cos(112*(pi/180))*(10800-10800))+10800");
    equation(out, "f27", "(cos(123*(pi/180))*(?f0 -10800)+sin(123*(pi/180))*(10800-10800))+10800");
    equation(out, "f28", "-(sin(123*(pi/180))*(?f0 -10800)-cos(123*(pi/180))*(10800-10800))+10800");
    equation(out, "f29", "(cos(135*(pi/180))*(0-10800)+sin(135*(pi/180))*(10800-10800))+10800");
    equation(out, "f30", "-(sin(135*(pi/180))*(0-10800)-cos(135*(pi/180))*(10800-10800))+10800");
    equation(out, "f31", "(cos(146*(pi/180))*(?f0 -10800)+sin(146*(pi/180))*(10800-10800))+10800");
    equation(out, "f32", "-(sin(146*(pi/180))*(?f0 -10800)-cos(146*(pi/180))*(10800-10800))+10800");
    equation(out, "f33", "(cos(157*(pi/180))*(0-10800)+sin(157*(pi/180))*(10800-10800))+10800");
    equation(out, "f34", "-(sin(157*(pi/180))*(0-10800)-cos(157*(pi/180))*(10800-10800))+10800");
    equation(out, "f35", "(cos(168*(pi/180))*(?f0 -10800)+sin(168*(pi/180))*(10800-10800))+10800");
    equation(out, "f36", "-(sin(168*(pi/180))*(?f0 -10800)-cos(168*(pi/180))*(10800-10800))+10800");
    equation(out, "f37", "(cos(180*(pi/180))*(0-10800)+sin(180*(pi/180))*(10800-10800))+10800");
    equation(out, "f38", "-(sin(180*(pi/180))*(0-10800)-cos(180*(pi/180))*(10800-10800))+10800");
    equation(out, "f39", "(cos(191*(pi/180))*(?f0 -10800)+sin(191*(pi/180))*(10800-10800))+10800");
    equation(out, "f40", "-(sin(191*(pi/180))*(?f0 -10800)-cos(191*(pi/180))*(10800-10800))+10800");
    equation(out, "f41", "(cos(202*(pi/180))*(0-10800)+sin(202*(pi/180))*(10800-10800))+10800");
    equation(out, "f42", "-(sin(202*(pi/180))*(0-10800)-cos(202*(pi/180))*(10800-10800))+10800");
    equation(out, "f43", "(cos(213*(pi/180))*(?f0 -10800)+sin(213*(pi/180))*(10800-10800))+10800");
    equation(out, "f44", "-(sin(213*(pi/180))*(?f0 -10800)-cos(213*(pi/180))*(10800-10800))+10800");
    equation(out, "f45", "(cos(225*(pi/180))*(0-10800)+sin(225*(pi/180))*(10800-10800))+10800");
    equation(out, "f46", "-(sin(225*(pi/180))*(0-10800)-cos(225*(pi/180))*(10800-10800))+10800");
    equation(out, "f47", "(cos(236*(pi/180))*(?f0 -10800)+sin(236*(pi/180))*(10800-10800))+10800");
    equation(out, "f48", "-(sin(236*(pi/180))*(?f0 -10800)-cos(236*(pi/180))*(10800-10800))+10800");
    equation(out, "f49", "(cos(247*(pi/180))*(0-10800)+sin(247*(pi/180))*(10800-10800))+10800");
    equation(out, "f50", "-(sin(247*(pi/180))*(0-10800)-cos(247*(pi/180))*(10800-10800))+10800");
    equation(out, "f51", "(cos(258*(pi/180))*(?f0 -10800)+sin(258*(pi/180))*(10800-10800))+10800");
    equation(out, "f52", "-(sin(258*(pi/180))*(?f0 -10800)-cos(258*(pi/180))*(10800-10800))+10800");
    equation(out, "f53", "(cos(270*(pi/180))*(0-10800)+sin(270*(pi/180))*(10800-10800))+10800");
    equation(out, "f54", "-(sin(270*(pi/180))*(0-10800)-cos(270*(pi/180))*(10800-10800))+10800");
    equation(out, "f55", "(cos(281*(pi/180))*(?f0 -10800)+sin(281*(pi/180))*(10800-10800))+10800");
    equation(out, "f56", "-(sin(281*(pi/180))*(?f0 -10800)-cos(281*(pi/180))*(10800-10800))+10800");
    equation(out, "f57", "(cos(292*(pi/180))*(0-10800)+sin(292*(pi/180))*(10800-10800))+10800");
    equation(out, "f58", "-(sin(292*(pi/180))*(0-10800)-cos(292*(pi/180))*(10800-10800))+10800");
    equation(out, "f59", "(cos(303*(pi/180))*(?f0 -10800)+sin(303*(pi/180))*(10800-10800))+10800");
    equation(out, "f60", "-(sin(303*(pi/180))*(?f0 -10800)-cos(303*(pi/180))*(10800-10800))+10800");
    equation(out, "f61", "(cos(315*(pi/180))*(0-10800)+sin(315*(pi/180))*(10800-10800))+10800");
    equation(out, "f62", "-(sin(315*(pi/180))*(0-10800)-cos(315*(pi/180))*(10800-10800))+10800");
    equation(out, "f63", "(cos(326*(pi/180))*(?f0 -10800)+sin(326*(pi/180))*(10800-10800))+10800");
    equation(out, "f64", "-(sin(326*(pi/180))*(?f0 -10800)-cos(326*(pi/180))*(10800-10800))+10800");
    equation(out, "f65", "(cos(337*(pi/180))*(0-10800)+sin(337*(pi/180))*(10800-10800))+10800");
    equation(out, "f66", "-(sin(337*(pi/180))*(0-10800)-cos(337*(pi/180))*(10800-10800))+10800");
    equation(out, "f67", "(cos(348*(pi/180))*(?f0 -10800)+sin(348*(pi/180))*(10800-10800))+10800");
    equation(out, "f68", "-(sin(348*(pi/180))*(?f0 -10800)-cos(348*(pi/180))*(10800-10800))+10800");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 10800");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.addAttribute("draw:handle-range-x-maximum", "10800");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // draw:enhanced-geometry
    out.xml.endElement(); // draw:custom-shape

}

void ODrawToOdf::processSeal24(const MSO::OfficeArtSpContainer &o, Writer &out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:type", "star24");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:text-areas", "?f1 ?f2 ?f3 ?f4");
    out.xml.addAttribute("draw:modifiers", "2500");
    out.xml.addAttribute("draw:enhanced-path", "M ?f5 ?f6 L ?f7 ?f8 ?f9 ?f10 ?f11 ?f12 ?f13 ?f14 ?f15 ?f16 ?f17 ?f18 ?f19 ?f20 ?f21 ?f22 ?f23 ?f24 ?f25 ?f26 ?f27 ?f28 ?f29 ?f30 ?f31 ?f32 ?f33 ?f34 ?f35 ?f36 ?f37 ?f38 ?f39 ?f40 ?f41 ?f42 ?f43 ?f44 ?f45 ?f46 ?f47 ?f48 ?f49 ?f50 ?f51 ?f52 ?f53 ?f54 ?f55 ?f56 ?f57 ?f58 ?f59 ?f60 ?f61 ?f62 ?f63 ?f64 ?f65 ?f66 ?f67 ?f68 ?f69 ?f70 ?f71 ?f72 ?f73 ?f74 ?f75 ?f76 ?f77 ?f78 ?f79 ?f80 ?f81 ?f82 ?f83 ?f84 ?f85 ?f86 ?f87 ?f88 ?f89 ?f90 ?f91 ?f92 ?f93 ?f94 ?f95 ?f96 ?f97 ?f98 ?f99 ?f100 ?f5 ?f6 Z N");
    equation(out, "f0", "$0 ");
    equation(out, "f1", "(cos(315*(pi/180))*(?f0 -10800)+sin(315*(pi/180))*(10800-10800))+10800");
    equation(out, "f2", "-(sin(315*(pi/180))*(?f0 -10800)-cos(315*(pi/180))*(10800-10800))+10800");
    equation(out, "f3", "(cos(135*(pi/180))*(?f0 -10800)+sin(135*(pi/180))*(10800-10800))+10800");
    equation(out, "f4", "-(sin(135*(pi/180))*(?f0 -10800)-cos(135*(pi/180))*(10800-10800))+10800");
    equation(out, "f5", "(cos(0*(pi/180))*(0-10800)+sin(0*(pi/180))*(10800-10800))+10800");
    equation(out, "f6", "-(sin(0*(pi/180))*(0-10800)-cos(0*(pi/180))*(10800-10800))+10800");
    equation(out, "f7", "(cos(7*(pi/180))*(?f0 -10800)+sin(7*(pi/180))*(10800-10800))+10800");
    equation(out, "f8", "-(sin(7*(pi/180))*(?f0 -10800)-cos(7*(pi/180))*(10800-10800))+10800");
    equation(out, "f9", "(cos(15*(pi/180))*(0-10800)+sin(15*(pi/180))*(10800-10800))+10800");
    equation(out, "f10", "-(sin(15*(pi/180))*(0-10800)-cos(15*(pi/180))*(10800-10800))+10800");
    equation(out, "f11", "(cos(22*(pi/180))*(?f0 -10800)+sin(22*(pi/180))*(10800-10800))+10800");
    equation(out, "f12", "-(sin(22*(pi/180))*(?f0 -10800)-cos(22*(pi/180))*(10800-10800))+10800");
    equation(out, "f13", "(cos(30*(pi/180))*(0-10800)+sin(30*(pi/180))*(10800-10800))+10800");
    equation(out, "f14", "-(sin(30*(pi/180))*(0-10800)-cos(30*(pi/180))*(10800-10800))+10800");
    equation(out, "f15", "(cos(37*(pi/180))*(?f0 -10800)+sin(37*(pi/180))*(10800-10800))+10800");
    equation(out, "f16", "-(sin(37*(pi/180))*(?f0 -10800)-cos(37*(pi/180))*(10800-10800))+10800");
    equation(out, "f17", "(cos(45*(pi/180))*(0-10800)+sin(45*(pi/180))*(10800-10800))+10800");
    equation(out, "f18", "-(sin(45*(pi/180))*(0-10800)-cos(45*(pi/180))*(10800-10800))+10800");
    equation(out, "f19", "(cos(52*(pi/180))*(?f0 -10800)+sin(52*(pi/180))*(10800-10800))+10800");
    equation(out, "f20", "-(sin(52*(pi/180))*(?f0 -10800)-cos(52*(pi/180))*(10800-10800))+10800");
    equation(out, "f21", "(cos(60*(pi/180))*(0-10800)+sin(60*(pi/180))*(10800-10800))+10800");
    equation(out, "f22", "-(sin(60*(pi/180))*(0-10800)-cos(60*(pi/180))*(10800-10800))+10800");
    equation(out, "f23", "(cos(67*(pi/180))*(?f0 -10800)+sin(67*(pi/180))*(10800-10800))+10800");
    equation(out, "f24", "-(sin(67*(pi/180))*(?f0 -10800)-cos(67*(pi/180))*(10800-10800))+10800");
    equation(out, "f25", "(cos(75*(pi/180))*(0-10800)+sin(75*(pi/180))*(10800-10800))+10800");
    equation(out, "f26", "-(sin(75*(pi/180))*(0-10800)-cos(75*(pi/180))*(10800-10800))+10800");
    equation(out, "f27", "(cos(82*(pi/180))*(?f0 -10800)+sin(82*(pi/180))*(10800-10800))+10800");
    equation(out, "f28", "-(sin(82*(pi/180))*(?f0 -10800)-cos(82*(pi/180))*(10800-10800))+10800");
    equation(out, "f29", "(cos(90*(pi/180))*(0-10800)+sin(90*(pi/180))*(10800-10800))+10800");
    equation(out, "f30", "-(sin(90*(pi/180))*(0-10800)-cos(90*(pi/180))*(10800-10800))+10800");
    equation(out, "f31", "(cos(97*(pi/180))*(?f0 -10800)+sin(97*(pi/180))*(10800-10800))+10800");
    equation(out, "f32", "-(sin(97*(pi/180))*(?f0 -10800)-cos(97*(pi/180))*(10800-10800))+10800");
    equation(out, "f33", "(cos(105*(pi/180))*(0-10800)+sin(105*(pi/180))*(10800-10800))+10800");
    equation(out, "f34", "-(sin(105*(pi/180))*(0-10800)-cos(105*(pi/180))*(10800-10800))+10800");
    equation(out, "f35", "(cos(112*(pi/180))*(?f0 -10800)+sin(112*(pi/180))*(10800-10800))+10800");
    equation(out, "f36", "-(sin(112*(pi/180))*(?f0 -10800)-cos(112*(pi/180))*(10800-10800))+10800");
    equation(out, "f37", "(cos(120*(pi/180))*(0-10800)+sin(120*(pi/180))*(10800-10800))+10800");
    equation(out, "f38", "-(sin(120*(pi/180))*(0-10800)-cos(120*(pi/180))*(10800-10800))+10800");
    equation(out, "f39", "(cos(127*(pi/180))*(?f0 -10800)+sin(127*(pi/180))*(10800-10800))+10800");
    equation(out, "f40", "-(sin(127*(pi/180))*(?f0 -10800)-cos(127*(pi/180))*(10800-10800))+10800");
    equation(out, "f41", "(cos(135*(pi/180))*(0-10800)+sin(135*(pi/180))*(10800-10800))+10800");
    equation(out, "f42", "-(sin(135*(pi/180))*(0-10800)-cos(135*(pi/180))*(10800-10800))+10800");
    equation(out, "f43", "(cos(142*(pi/180))*(?f0 -10800)+sin(142*(pi/180))*(10800-10800))+10800");
    equation(out, "f44", "-(sin(142*(pi/180))*(?f0 -10800)-cos(142*(pi/180))*(10800-10800))+10800");
    equation(out, "f45", "(cos(150*(pi/180))*(0-10800)+sin(150*(pi/180))*(10800-10800))+10800");
    equation(out, "f46", "-(sin(150*(pi/180))*(0-10800)-cos(150*(pi/180))*(10800-10800))+10800");
    equation(out, "f47", "(cos(157*(pi/180))*(?f0 -10800)+sin(157*(pi/180))*(10800-10800))+10800");
    equation(out, "f48", "-(sin(157*(pi/180))*(?f0 -10800)-cos(157*(pi/180))*(10800-10800))+10800");
    equation(out, "f49", "(cos(165*(pi/180))*(0-10800)+sin(165*(pi/180))*(10800-10800))+10800");
    equation(out, "f50", "-(sin(165*(pi/180))*(0-10800)-cos(165*(pi/180))*(10800-10800))+10800");
    equation(out, "f51", "(cos(172*(pi/180))*(?f0 -10800)+sin(172*(pi/180))*(10800-10800))+10800");
    equation(out, "f52", "-(sin(172*(pi/180))*(?f0 -10800)-cos(172*(pi/180))*(10800-10800))+10800");
    equation(out, "f53", "(cos(180*(pi/180))*(0-10800)+sin(180*(pi/180))*(10800-10800))+10800");
    equation(out, "f54", "-(sin(180*(pi/180))*(0-10800)-cos(180*(pi/180))*(10800-10800))+10800");
    equation(out, "f55", "(cos(187*(pi/180))*(?f0 -10800)+sin(187*(pi/180))*(10800-10800))+10800");
    equation(out, "f56", "-(sin(187*(pi/180))*(?f0 -10800)-cos(187*(pi/180))*(10800-10800))+10800");
    equation(out, "f57", "(cos(195*(pi/180))*(0-10800)+sin(195*(pi/180))*(10800-10800))+10800");
    equation(out, "f58", "-(sin(195*(pi/180))*(0-10800)-cos(195*(pi/180))*(10800-10800))+10800");
    equation(out, "f59", "(cos(202*(pi/180))*(?f0 -10800)+sin(202*(pi/180))*(10800-10800))+10800");
    equation(out, "f60", "-(sin(202*(pi/180))*(?f0 -10800)-cos(202*(pi/180))*(10800-10800))+10800");
    equation(out, "f61", "(cos(210*(pi/180))*(0-10800)+sin(210*(pi/180))*(10800-10800))+10800");
    equation(out, "f62", "-(sin(210*(pi/180))*(0-10800)-cos(210*(pi/180))*(10800-10800))+10800");
    equation(out, "f63", "(cos(217*(pi/180))*(?f0 -10800)+sin(217*(pi/180))*(10800-10800))+10800");
    equation(out, "f64", "-(sin(217*(pi/180))*(?f0 -10800)-cos(217*(pi/180))*(10800-10800))+10800");
    equation(out, "f65", "(cos(225*(pi/180))*(0-10800)+sin(225*(pi/180))*(10800-10800))+10800");
    equation(out, "f66", "-(sin(225*(pi/180))*(0-10800)-cos(225*(pi/180))*(10800-10800))+10800");
    equation(out, "f67", "(cos(232*(pi/180))*(?f0 -10800)+sin(232*(pi/180))*(10800-10800))+10800");
    equation(out, "f68", "-(sin(232*(pi/180))*(?f0 -10800)-cos(232*(pi/180))*(10800-10800))+10800");
    equation(out, "f69", "(cos(240*(pi/180))*(0-10800)+sin(240*(pi/180))*(10800-10800))+10800");
    equation(out, "f70", "-(sin(240*(pi/180))*(0-10800)-cos(240*(pi/180))*(10800-10800))+10800");
    equation(out, "f71", "(cos(247*(pi/180))*(?f0 -10800)+sin(247*(pi/180))*(10800-10800))+10800");
    equation(out, "f72", "-(sin(247*(pi/180))*(?f0 -10800)-cos(247*(pi/180))*(10800-10800))+10800");
    equation(out, "f73", "(cos(255*(pi/180))*(0-10800)+sin(255*(pi/180))*(10800-10800))+10800");
    equation(out, "f74", "-(sin(255*(pi/180))*(0-10800)-cos(255*(pi/180))*(10800-10800))+10800");
    equation(out, "f75", "(cos(262*(pi/180))*(?f0 -10800)+sin(262*(pi/180))*(10800-10800))+10800");
    equation(out, "f76", "-(sin(262*(pi/180))*(?f0 -10800)-cos(262*(pi/180))*(10800-10800))+10800");
    equation(out, "f77", "(cos(270*(pi/180))*(0-10800)+sin(270*(pi/180))*(10800-10800))+10800");
    equation(out, "f78", "-(sin(270*(pi/180))*(0-10800)-cos(270*(pi/180))*(10800-10800))+10800");
    equation(out, "f79", "(cos(277*(pi/180))*(?f0 -10800)+sin(277*(pi/180))*(10800-10800))+10800");
    equation(out, "f80", "-(sin(277*(pi/180))*(?f0 -10800)-cos(277*(pi/180))*(10800-10800))+10800");
    equation(out, "f81", "(cos(285*(pi/180))*(0-10800)+sin(285*(pi/180))*(10800-10800))+10800");
    equation(out, "f82", "-(sin(285*(pi/180))*(0-10800)-cos(285*(pi/180))*(10800-10800))+10800");
    equation(out, "f83", "(cos(292*(pi/180))*(?f0 -10800)+sin(292*(pi/180))*(10800-10800))+10800");
    equation(out, "f84", "-(sin(292*(pi/180))*(?f0 -10800)-cos(292*(pi/180))*(10800-10800))+10800");
    equation(out, "f85", "(cos(300*(pi/180))*(0-10800)+sin(300*(pi/180))*(10800-10800))+10800");
    equation(out, "f86", "-(sin(300*(pi/180))*(0-10800)-cos(300*(pi/180))*(10800-10800))+10800");
    equation(out, "f87", "(cos(307*(pi/180))*(?f0 -10800)+sin(307*(pi/180))*(10800-10800))+10800");
    equation(out, "f88", "-(sin(307*(pi/180))*(?f0 -10800)-cos(307*(pi/180))*(10800-10800))+10800");
    equation(out, "f89", "(cos(315*(pi/180))*(0-10800)+sin(315*(pi/180))*(10800-10800))+10800");
    equation(out, "f90", "-(sin(315*(pi/180))*(0-10800)-cos(315*(pi/180))*(10800-10800))+10800");
    equation(out, "f91", "(cos(322*(pi/180))*(?f0 -10800)+sin(322*(pi/180))*(10800-10800))+10800");
    equation(out, "f92", "-(sin(322*(pi/180))*(?f0 -10800)-cos(322*(pi/180))*(10800-10800))+10800");
    equation(out, "f93", "(cos(330*(pi/180))*(0-10800)+sin(330*(pi/180))*(10800-10800))+10800");
    equation(out, "f94", "-(sin(330*(pi/180))*(0-10800)-cos(330*(pi/180))*(10800-10800))+10800");
    equation(out, "f95", "(cos(337*(pi/180))*(?f0 -10800)+sin(337*(pi/180))*(10800-10800))+10800");
    equation(out, "f96", "-(sin(337*(pi/180))*(?f0 -10800)-cos(337*(pi/180))*(10800-10800))+10800");
    equation(out, "f97", "(cos(345*(pi/180))*(0-10800)+sin(345*(pi/180))*(10800-10800))+10800");
    equation(out, "f98", "-(sin(345*(pi/180))*(0-10800)-cos(345*(pi/180))*(10800-10800))+10800");
    equation(out, "f99", "(cos(352*(pi/180))*(?f0 -10800)+sin(352*(pi/180))*(10800-10800))+10800");
    equation(out, "f100", "-(sin(352*(pi/180))*(?f0 -10800)-cos(352*(pi/180))*(10800-10800))+10800");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 10800");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.addAttribute("draw:handle-range-x-maximum", "10800");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // draw:enhanced-geometry
    out.xml.endElement(); // draw:custom-shape
}

void ODrawToOdf::processRibbon(const MSO::OfficeArtSpContainer &o, Writer &out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:type", "mso-spt53");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:glue-points", "?f17 ?f10 2700 ?f14 ?f17 21600 ?f18 ?f14");
    out.xml.addAttribute("draw:text-areas", "?f0 ?f10 ?f9 21600");
    out.xml.addAttribute("draw:modifiers", "5400 2700");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L ?f3 0 X ?f4 ?f11 L ?f4 ?f10 ?f5 ?f10 ?f5 ?f11 Y ?f6 0 L 21600 0 ?f18 ?f14 21600 ?f15 ?f9 ?f15 ?f9 ?f16 Y ?f8 21600 L ?f1 21600 X ?f0 ?f16 L ?f0 ?f15 0 ?f15 2700 ?f14 Z N M ?f4 ?f11 F Y ?f3 ?f12 L ?f1 ?f12 X ?f0 ?f13 ?f1 ?f10 L ?f4 ?f10 N M ?f5 ?f11 F Y ?f6 ?f12 L ?f8 ?f12 X ?f9 ?f13 ?f8 ?f10 L ?f5 ?f10 N M ?f0 ?f13 F L ?f0 ?f15 N M ?f9 ?f13 F L ?f9 ?f15 N");
    equation(out, "f0", "$0 ");
    equation(out, "f1", "?f0 +675");
    equation(out, "f2", "?f1 +675");
    equation(out, "f3", "?f2 +675");
    equation(out, "f4", "?f3 +675");
    equation(out, "f5", "21600-?f4 ");
    equation(out, "f6", "21600-?f3 ");
    equation(out, "f7", "21600-?f2 ");
    equation(out, "f8", "21600-?f1 ");
    equation(out, "f9", "21600-?f0 ");
    equation(out, "f10", "$1 ");
    equation(out, "f11", "?f10 /4");
    equation(out, "f12", "?f11 *2");
    equation(out, "f13", "?f11 *3");
    equation(out, "f14", "10800-?f12 ");
    equation(out, "f15", "21600-?f10 ");
    equation(out, "f16", "21600-?f11 ");
    equation(out, "f17", "21600/2");
    equation(out, "f18", "21600-2700");
    equation(out, "f19", "?f17 -2700");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-minimum", "2700");
    out.xml.addAttribute("draw:handle-range-x-maximum", "8100");
    out.xml.endElement();
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "10800 $1");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "7200");
    out.xml.endElement();
    out.xml.endElement(); // draw:enhanced-geometry
    out.xml.endElement(); // draw:custom-shape
}

void ODrawToOdf::processRibbon2(const MSO::OfficeArtSpContainer &o, Writer &out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:type", "mso-spt54");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:text-areas", "?f0 0 ?f19 ?f1");
    out.xml.addAttribute("draw:modifiers", "5400 18900");
    out.xml.addAttribute("draw:enhanced-path", "M ?f12 ?f1 L ?f12 ?f13 C ?f12 ?f14 ?f15 21600 ?f16 21600 L 0 21600 2750 ?f7 0 ?f2 ?f0 ?f2 ?f0 ?f4 C ?f0 ?f5 ?f10 0 ?f11 0 L ?f17 0 C ?f18 0 ?f19 ?f5 ?f19 ?f4 L ?f19 ?f2 21600 ?f2 18850 ?f7 21600 21600 ?f20 21600 C ?f21 21600 ?f22 ?f14 ?f22 ?f13 L ?f22 ?f1 Z N M ?f12 ?f1 L ?f12 ?f13 C ?f12 ?f23 ?f15 ?f24 ?f16 ?f24 L ?f11 ?f24 C ?f10 ?f24 ?f0 ?f26 ?f0 ?f25 ?f0 ?f27 ?f10 ?f1 ?f11 ?f1 Z N M ?f22 ?f1 L ?f22 ?f13 C ?f22 ?f23 ?f21 ?f24 ?f20 ?f24 L ?f17 ?f24 C ?f18 ?f24 ?f19 ?f26 ?f19 ?f25 ?f19 ?f27 ?f18 ?f1 ?f17 ?f1 Z N M ?f0 ?f25 L ?f0 ?f2 N M ?f19 ?f25 L ?f19 ?f2 N");
    equation(out, "f0", "$0 ");
    equation(out, "f1", "$1 ");
    equation(out, "f2", "21600-?f1 ");
    equation(out, "f3", "?f2 /2");
    equation(out, "f4", "?f3 /2");
    equation(out, "f5", "?f4 /2");
    equation(out, "f6", "?f1 /2");
    equation(out, "f7", "21600-?f6 ");
    equation(out, "f8", "420");
    equation(out, "f9", "?f8 *2");
    equation(out, "f10", "?f0 +?f8 ");
    equation(out, "f11", "?f0 +?f9 ");
    equation(out, "f12", "?f0 +2700");
    equation(out, "f13", "21600-?f4 ");
    equation(out, "f14", "21600-?f5 ");
    equation(out, "f15", "?f12 -?f8 ");
    equation(out, "f16", "?f12 -?f9 ");
    equation(out, "f17", "21600-?f11 ");
    equation(out, "f18", "21600-?f10 ");
    equation(out, "f19", "21600-?f0 ");
    equation(out, "f20", "21600-?f16 ");
    equation(out, "f21", "21600-?f15 ");
    equation(out, "f22", "21600-?f12 ");
    equation(out, "f23", "?f13 -?f5 ");
    equation(out, "f24", "?f1 +?f3 ");
    equation(out, "f25", "?f1 +?f4 ");
    equation(out, "f26", "?f25 +?f5 ");
    equation(out, "f27", "?f25 -?f5 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-minimum", "2700");
    out.xml.addAttribute("draw:handle-range-x-maximum", "8100");
    out.xml.endElement();
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "10800 $1");
    out.xml.addAttribute("draw:handle-range-y-minimum", "14400");
    out.xml.addAttribute("draw:handle-range-y-maximum", "21600");
    out.xml.endElement();
    out.xml.endElement(); // draw:enhanced-geometry
    out.xml.endElement(); // draw:custom-shape
}

void ODrawToOdf::processHorizontalScroll(const MSO::OfficeArtSpContainer& o, Writer& out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:path-stretchpoint-x", "10800");
    out.xml.addAttribute("draw:path-stretchpoint-y", "11000");
    out.xml.addAttribute("draw:text-areas", "?f0 ?f0 ?f3 ?f12");
    out.xml.addAttribute("draw:type", "horizontal-scroll"); // "mso-spt98"
    out.xml.addAttribute("draw:modifiers", "2700");
    out.xml.addAttribute("draw:enhanced-path", "M 0 ?f4 Y ?f1 ?f0 L ?f3 ?f0 ?f3 ?f1 Y ?f2 0 21600 ?f1 L 21600 ?f13 Y ?f2 ?f12 L ?f0 ?f12 ?f0 ?f11 Y ?f1 21600 0 ?f11 Z N M ?f1 ?f4 Y ?f9 ?f8 ?f0 ?f4 ?f1 ?f6 Z N M ?f2 ?f1 Y ?f3 ?f9 ?f3 ?f1 ?f2 0 X 21600 ?f1 ?f2 ?f0 Z N M ?f1 ?f6 X 0 ?f4 N M ?f2 ?f0 L ?f3 ?f0 N M ?f0 ?f4 L ?f0 ?f11 N");
    equation(out, "f0", "$0 ");
    equation(out, "f1", "?f0 /2");
    equation(out, "f2", "right-?f1 ");
    equation(out, "f3", "right-?f0 ");
    equation(out, "f4", "?f0 +?f1 ");
    equation(out, "f5", "right-?f4 ");
    equation(out, "f6", "?f0 *2");
    equation(out, "f7", "?f1 /2");
    equation(out, "f8", "?f0 +?f7 ");
    equation(out, "f9", "?f1 +?f7 ");
    equation(out, "f10", "bottom-?f9 ");
    equation(out, "f11", "bottom-?f1 ");
    equation(out, "f12", "bottom-?f0 ");
    equation(out, "f13", "bottom-?f4 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.addAttribute("draw:handle-range-x-maximum", "5400");
    out.xml.endElement();
    out.xml.endElement(); // draw:enhanced-geometry
    out.xml.endElement(); // draw:custom-shape
}

void ODrawToOdf::processDoubleWave(const MSO::OfficeArtSpContainer &o, Writer &out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:type", "mso-spt188");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:glue-points", "?f18 ?f0 ?f20 10800 ?f19 ?f1 ?f21 10800");
    out.xml.addAttribute("draw:text-areas", "?f5 ?f22 ?f11 ?f23");
    out.xml.addAttribute("draw:modifiers", "1400 10800");
    out.xml.addAttribute("draw:enhanced-path", "M ?f7 ?f0 C ?f15 ?f9 ?f30 ?f10 ?f18 ?f0 ?f31 ?f9 ?f16 ?f10 ?f12 ?f0 L ?f24 ?f1 C ?f25 ?f26 ?f33 ?f28 ?f19 ?f1 ?f32 ?f26 ?f27 ?f28 ?f29 ?f1 Z N");
    equation(out, "f0", "$0 ");
    equation(out, "f1", "21600-?f0 ");
    equation(out, "f2", "$1 ");
    equation(out, "f3", "?f2 -10800");
    equation(out, "f4", "?f3 *2");
    equation(out, "f5", "abs(?f4 )");
    equation(out, "f6", "4320-?f5 ");
    equation(out, "f7", "if(?f3 ,0,?f5 )");
    equation(out, "f8", "7900*?f0 /2230");
    equation(out, "f9", "?f0 -?f8 ");
    equation(out, "f10", "?f0 +?f8 ");
    equation(out, "f11", "21600-?f4 ");
    equation(out, "f12", "if(?f3 ,?f11 ,21600)");
    equation(out, "f13", "?f12 -?f7 ");
    equation(out, "f14", "?f5 /2");
    equation(out, "f15", "?f7 +3600-?f14 ");
    equation(out, "f16", "?f12 +?f14 -3600");
    equation(out, "f17", "?f13 /2");
    equation(out, "f18", "?f7 +?f17 ");
    equation(out, "f19", "21600-?f18 ");
    equation(out, "f20", "?f5 /2");
    equation(out, "f21", "21600-?f20 ");
    equation(out, "f22", "?f0 *2");
    equation(out, "f23", "21600-?f22 ");
    equation(out, "f24", "21600-?f7 ");
    equation(out, "f25", "21600-?f15 ");
    equation(out, "f26", "?f1 +?f8 ");
    equation(out, "f27", "21600-?f16 ");
    equation(out, "f28", "?f1 -?f8 ");
    equation(out, "f29", "21600-?f12 ");
    equation(out, "f30", "?f18 -?f14 ");
    equation(out, "f31", "?f18 +?f14 ");
    equation(out, "f32", "?f19 -?f14 ");
    equation(out, "f33", "?f19 +?f14 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "left $0");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "2230");
    out.xml.endElement();
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$1 21600");
    out.xml.addAttribute("draw:handle-range-x-minimum", "8640");
    out.xml.addAttribute("draw:handle-range-x-maximum", "12960");
    out.xml.endElement();
    out.xml.endElement(); // draw:enhanced-geometry
    out.xml.endElement(); // draw:custom-shape
}

void ODrawToOdf::processFlowChartTerminator(const MSO::OfficeArtSpContainer &o, Writer &out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:type", "flowchart-terminator");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:glue-points", "10800 0 0 10800 10800 21600 21600 10800");
    out.xml.addAttribute("draw:text-areas", "1060 3180 20540 18420");
    out.xml.addAttribute("draw:enhanced-path", "M 3470 21600 X 0 10800 3470 0 L 18130 0 X 21600 10800 18130 21600 Z N");
    out.xml.endElement(); // draw:enhanced-geometry
    out.xml.endElement(); // draw:custom-shape
}

void ODrawToOdf::processFlowChartProcess(const MSO::OfficeArtSpContainer &o, Writer &out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:type", "flowchart-process");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:glue-points", "10800 0 0 10800 10800 21600 21600 10800");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L 21600 0 21600 21600 0 21600 0 0 Z N");
    out.xml.endElement(); // draw:enhanced-geometry
    out.xml.endElement(); // draw:custom-shape
}

void ODrawToOdf::processFlowChartDecision(const MSO::OfficeArtSpContainer &o, Writer &out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:type", "flowchart-decision");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:glue-points", "10800 0 0 10800 10800 21600 21600 10800");
    out.xml.addAttribute("draw:text-areas", "5400 5400 16200 16200");
    out.xml.addAttribute("draw:enhanced-path", "M 0 10800 L 10800 0 21600 10800 10800 21600 0 10800 Z N");
    out.xml.endElement(); // draw:enhanced-geometry
    out.xml.endElement(); // draw:custom-shape
}

void ODrawToOdf::processFlowChartManualOperation(const MSO::OfficeArtSpContainer &o, Writer &out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:glue-points", "10800 0 2160 10800 10800 21600 19440 10800");
    out.xml.addAttribute("draw:text-areas", "4350 0 17250 21600");
    out.xml.addAttribute("draw:type", "flowchart-manual-operation");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L 21600 0 17250 21600 4350 21600 0 0 Z N");
    out.xml.endElement(); // draw:enhanced-geometry
    out.xml.endElement(); // draw:custom-shape
}


void ODrawToOdf::processFlowChartConnector(const MSO::OfficeArtSpContainer &o, Writer &out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:type", "flowchart-connector");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:glue-points", "10800 0 3163 3163 0 10800 3163 18437 10800 21600 18437 18437 21600 10800 18437 3163");
    out.xml.addAttribute("draw:text-areas", "3180 3180 18420 18420");
    out.xml.addAttribute("draw:enhanced-path", "U 10800 10800 10800 10800 0 360 Z N");

    out.xml.endElement(); // draw:enhanced-geometry
    out.xml.endElement(); // draw:custom-shape
}

void ODrawToOdf::processFlowChartMagneticTape(const MSO::OfficeArtSpContainer &o, Writer &out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:glue-points", "10800 0 0 10800 10800 21600 21600 10800");
    out.xml.addAttribute("draw:text-areas", "3100 3100 18500 18500");
    out.xml.addAttribute("draw:type", "flowchart-sequential-access");
    out.xml.addAttribute("draw:enhanced-path", "M 20980 18150 L 20980 21600 10670 21600 C 4770 21540 0 16720 0 10800 0 4840 4840 0 10800 0 16740 0 21600 4840 21600 10800 21600 13520 20550 16160 18670 18170 Z N");
    out.xml.endElement(); // draw:enhanced-geometry
    out.xml.endElement(); // draw:custom-shape
}

void ODrawToOdf::processFlowChartMagneticDisk(const MSO::OfficeArtSpContainer &o, Writer &out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("svg:viewBox","0 0 21600 21600");
    out.xml.addAttribute("draw:glue-points","10800 6800 10800 0 0 10800 10800 21600 21600 10800");
    out.xml.addAttribute("draw:text-areas","0 6800 21600 18200");
    setShapeMirroring(o, out);
    out.xml.addAttribute("draw:type","flowchart-magnetic-disk");
    out.xml.addAttribute("draw:enhanced-path","M 0 3400 Y 10800 0 21600 3400 L 21600 18200 Y 10800 21600 0 18200 Z N M 0 3400 Y 10800 6800 21600 3400 N");
    out.xml.endElement(); // draw:enhanced-geometry
    out.xml.endElement(); // draw:custom-shape
}



void ODrawToOdf::processCallout2(const MSO::OfficeArtSpContainer &o, Writer &out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:type", "mso-spt42");
    processModifiers(o, out);
    // TODO: uncomment the bit of the path once EnhancedPathShape supports S
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 M 21600 21600 "/*S L 21600 0 21600 21600 0 21600 Z N*/ "M ?f0 ?f1 L ?f2 ?f3 N M ?f2 ?f3 L ?f4 ?f5 N M");
    equation(out, "f0", "$0 ");
    equation(out, "f1", "$1 ");
    equation(out, "f2", "$2 ");
    equation(out, "f3", "$3 ");
    equation(out, "f4", "$4 ");
    equation(out, "f5", "$5 ");
    equation(out, "f6", "$6 ");
    equation(out, "f7", "$7 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 $1");
    out.xml.endElement();
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$2 $3");
    out.xml.endElement();
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$4 $5");
    out.xml.endElement();

    out.xml.endElement(); // draw:enhanced-geometry
    out.xml.endElement(); // draw:custom-shape
}

void ODrawToOdf::processDonut(const MSO::OfficeArtSpContainer &o, Writer &out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:type", "ring");
    out.xml.addAttribute("draw:glue-points", "10800 0 3163 3163 0 10800 3163 18437 10800 21600 18437 18437 21600 10800 18437 3163");
    out.xml.addAttribute("draw:text-areas", "3163 3163 18437 18437");
    processModifiers(o, out, QList<int>() << 5400);
    out.xml.addAttribute("draw:enhanced-path", "U 10800 10800 10800 10800 0 360 Z U 10800 10800 ?f1 ?f1 0 360 N");
    equation(out, "f0", "$0 ");
    equation(out, "f1", "10800-$0 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 10800");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.addAttribute("draw:handle-range-x-maximum", "10800");
    out.xml.endElement();


    out.xml.endElement(); // draw:enhanced-geometry
    out.xml.endElement(); // draw:custom-shape
}

void ODrawToOdf::processFlowChartDelay(const MSO::OfficeArtSpContainer& o, Writer& out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:type", "flowchart-delay");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:glue-points", "10800 0 0 10800 10800 21600 21600 10800");
    out.xml.addAttribute("draw:text-areas", "0 3100 18500 18500");
    setShapeMirroring(o, out);
    out.xml.addAttribute("draw:enhanced-path", "M 10800 0 X 21600 10800 10800 21600 L 0 21600 0 0 Z N");
    out.xml.endElement(); // draw:enhanced-geometry
    out.xml.endElement(); // draw:custom-shape
}

void ODrawToOdf::processFreeLine(const OfficeArtSpContainer& o, Writer& out)
{
    out.xml.startElement("draw:path");
    processStyleAndText(o, out);
    out.xml.endElement(); // path
}

void ODrawToOdf::processPictureFrame(const OfficeArtSpContainer& o, Writer& out)
{
    QString url;
    const Pib* pib = get<Pib>(o);
    if (pib && client) {
        url = client->getPicturePath(pib->pib);
    } else {
        // Does not make much sense to display an empty frame, following
        // PPT->ODP filters of both OOo and MS Office.
        return;
    }
    out.xml.startElement("draw:frame");
    processStyleAndText(o, out);

    // if the image cannot be found, just place an empty frame
    if (url.isEmpty()) {
        out.xml.endElement(); //draw:frame
        return;
    }
    out.xml.startElement("draw:image");
    out.xml.addAttribute("xlink:href", url);
    out.xml.addAttribute("xlink:type", "simple");
    out.xml.addAttribute("xlink:show", "embed");
    out.xml.addAttribute("xlink:actuate", "onLoad");
    out.xml.endElement(); // image
    out.xml.endElement(); // frame
}

void ODrawToOdf::processNotPrimitive(const OfficeArtSpContainer& o, Writer& out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    setEnhancedGeometry(o, out);
    out.xml.endElement(); //draw:enhanced-geometry

    out.xml.endElement(); //draw:custom-shape
}

void ODrawToOdf::processNotchedCircularArrow(const MSO::OfficeArtSpContainer& o, Writer& out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    setEnhancedGeometry(o, out);
    out.xml.endElement(); //draw:enhanced-geometry

    out.xml.endElement(); //draw:custom-shape
}

void ODrawToOdf::processActionButtonInformation(const MSO::OfficeArtSpContainer& o, Writer& out)
{
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:path-stretchpoint-x","10800");
    out.xml.addAttribute("draw:path-stretchpoint-y","10800");
    out.xml.addAttribute("draw:text-areas","?f1 ?f2 ?f3 ?f4");
    out.xml.addAttribute("draw:type","mso-spt192");
    processModifiers(o, out);
    out.xml.addAttribute("draw:enhanced-path","M 0 0 L 21600 0 21600 21600 0 21600 Z N M 0 0 L 21600 0 ?f3 ?f2 ?f1 ?f2 Z N M 21600 0 L 21600 21600 ?f3 ?f4 ?f3 ?f2 Z N M 21600 21600 L 0 21600 ?f1 ?f4 ?f3 ?f4 Z N M 0 21600 L 0 0 ?f1 ?f2 ?f1 ?f4 Z N M ?f7 ?f12 X ?f10 ?f8 ?f7 ?f16 ?f14 ?f8 ?f7 ?f12 Z N M ?f7 ?f20 X ?f18 ?f42 ?f7 ?f24 ?f22 ?f42 ?f7 ?f20 Z N M ?f26 ?f28 L ?f30 ?f28 ?f30 ?f32 ?f34 ?f32 ?f34 ?f36 ?f26 ?f36 ?f26 ?f32 ?f38 ?f32 ?f38 ?f40 ?f26 ?f40 Z N");

    equation(out, "f0" ,"$0 ");
    equation(out, "f1" ,"left+$0 ");
    equation(out, "f2" ,"top+$0 ");
    equation(out, "f3" ,"right-$0 ");
    equation(out, "f4" ,"bottom-$0 ");
    equation(out, "f5" ,"10800-$0 ");
    equation(out, "f6" ,"?f5 /10800");
    equation(out, "f7" ,"right/2");
    equation(out, "f8" ,"bottom/2");
    equation(out, "f9" ,"-8050*?f6 ");
    equation(out, "f10" ,"?f9 +?f7 ");
    equation(out, "f11" ,"-8050*?f6 ");
    equation(out, "f12" ,"?f11 +?f8 ");
    equation(out, "f13" ,"8050*?f6 ");
    equation(out, "f14" ,"?f13 +?f7 ");
    equation(out, "f15" ,"8050*?f6 ");
    equation(out, "f16" ,"?f15 +?f8 ");
    equation(out, "f17" ,"-2060*?f6 ");
    equation(out, "f18" ,"?f17 +?f7 ");
    equation(out, "f19" ,"-7620*?f6 ");
    equation(out, "f20" ,"?f19 +?f8 ");
    equation(out, "f21" ,"2060*?f6 ");
    equation(out, "f22" ,"?f21 +?f7 ");
    equation(out, "f23" ,"-3500*?f6 ");
    equation(out, "f24" ,"?f23 +?f8 ");
    equation(out, "f25" ,"-2960*?f6 ");
    equation(out, "f26" ,"?f25 +?f7 ");
    equation(out, "f27" ,"-2960*?f6 ");
    equation(out, "f28" ,"?f27 +?f8 ");
    equation(out, "f29" ,"1480*?f6 ");
    equation(out, "f30" ,"?f29 +?f7 ");
    equation(out, "f31" ,"5080*?f6 ");
    equation(out, "f32" ,"?f31 +?f8 ");
    equation(out, "f33" ,"2960*?f6 ");
    equation(out, "f34" ,"?f33 +?f7 ");
    equation(out, "f35" ,"6140*?f6 ");
    equation(out, "f36" ,"?f35 +?f8 ");
    equation(out, "f37" ,"-1480*?f6 ");
    equation(out, "f38" ,"?f37 +?f7 ");
    equation(out, "f39" ,"-1920*?f6 ");
    equation(out, "f40" ,"?f39 +?f8 ");
    equation(out, "f41" ,"-5560*?f6 ");
    equation(out, "f42" ,"?f41 +?f8 ");

    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position","$0 top");
    out.xml.addAttribute("draw:handle-switched","true");
    out.xml.addAttribute("draw:handle-range-x-minimum","0");
    out.xml.addAttribute("draw:handle-range-x-maximum","5400");

    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // draw:enhanced-geometry
    out.xml.endElement(); // draw:custom-shape
}

void ODrawToOdf::processDrawingObject(const OfficeArtSpContainer& o, Writer& out)
{
    quint16 shapeType = o.shapeProp.rh.recInstance;
    if (shapeType == msosptEllipse) {
        processEllipse(o, out);
    } else if (shapeType == msosptRectangle
               || shapeType == msosptTextBox) {
        processRectangle(o, out);
    } else if (shapeType == msosptRoundRectangle) {
        processRoundRectangle(o, out);
    } else  if (shapeType == msosptDiamond) {
        processDiamond(o, out);
    } else  if (shapeType == msosptIsocelesTriangle ||
                shapeType == msosptRightTriangle) {
        processTriangle(o, out);
    } else if (shapeType == msosptTrapezoid) {
        processTrapezoid(o, out);
    } else if (shapeType == msosptParallelogram) {
        processParallelogram(o, out);
    } else if (shapeType == msosptHexagon) {
        processHexagon(o, out);
    } else if (shapeType == msosptOctagon) {
        processOctagon(o, out);
    } else if (shapeType == msosptPlus) {
        processPlus(o, out);
    } else if (shapeType == msosptLeftArrow ||
               shapeType == msosptUpArrow ||
               shapeType == msosptDownArrow){
        processArrow(o, out);
    } else if (shapeType == msosptLeftRightArrow) {
        processLeftRightArrow(o, out);
    } else if (shapeType == msosptLine) {
        processLine(o, out);
    } else if (shapeType == msosptStraightConnector1) {
        processStraightConnector1(o, out);
    } else if (shapeType == msosptBentConnector3) {
        processBentConnector3(o, out);
    } else if (shapeType == msosptWedgeRectCallout) {
        processWedgeRectCallout(o, out);
    } else if (shapeType == msosptWedgeEllipseCallout) {
        processWedgeEllipseCallout(o, out);
    } else if (shapeType == msosptSmileyFace) {
        processSmiley(o, out);
    } else if (shapeType == msosptHeart) {
        processHeart(o, out);
    } else if (shapeType == msosptQuadArrow) {
        processQuadArrow(o, out);
    } else if (shapeType == msosptUturnArrow) {
        processUturnArrow(o, out);
    } else if (shapeType == msosptCircularArrow) {
        processCircularArrow(o, out);
    } else if (shapeType == msosptCloudCallout) {
        processCloudCallout(o, out);
    } else if (shapeType == msosptIrregularSeal1) {
        processIrregularSeal1(o, out);
    } else if (shapeType == msosptLightningBolt) {
        processLightningBolt(o, out);
    } else if (shapeType == msosptSeal16) {
        processSeal16(o, out);
    } else if (shapeType == msosptSeal24) {
        processSeal24(o, out);
    } else if (shapeType == msosptRibbon) {
        processRibbon(o, out);
    } else if (shapeType == msosptRibbon2) {
        processRibbon2(o, out);
    } else if (shapeType == msosptHorizontalScroll) {
        processHorizontalScroll(o, out);
    } else if (shapeType == msosptDoubleWave) {
        processDoubleWave(o, out);
    } else if (shapeType == msosptFlowChartTerminator) {
        processFlowChartTerminator(o, out);
    } else if (shapeType == msosptFlowChartProcess) {
        processFlowChartProcess(o, out);
    } else if (shapeType == msosptFlowChartDecision) {
        processFlowChartDecision(o, out);
    } else if (shapeType == msosptFlowChartManualOperation) {
        processFlowChartManualOperation(o, out);
    } else if (shapeType == msosptFlowChartConnector) {
        processFlowChartConnector(o, out);
    } else if (shapeType == msosptFlowChartMagneticTape) {
        processFlowChartMagneticTape(o, out);
    } else if (shapeType == msosptFlowChartMagneticDisk) {
        processFlowChartMagneticDisk(o, out);
    } else if (shapeType == msosptCallout2) {
        processCallout2(o, out);
    } else if (shapeType == msosptDonut) {
        processDonut(o, out);
        //} else if (shapeType == msosptMin) {
        //    processFreeLine(o, out);
    } else if (shapeType == msosptPictureFrame
               || shapeType == msosptHostControl) {
        processPictureFrame(o, out);
    } else if (shapeType == msosptNotPrimitive) {
        processNotPrimitive(o, out);
    } else if (shapeType == msosptNotchedCircularArrow) {
        processNotchedCircularArrow(o, out);
    } else if (shapeType == msosptFlowChartDelay) {
        processFlowChartDelay(o, out);
    } else if (shapeType == msosptActionButtonInformation) {
        processActionButtonInformation(o, out);
    } else {
        qDebug() << "cannot handle object of type " << shapeType;
    }
}
void ODrawToOdf::processStyleAndText(const MSO::OfficeArtSpContainer& o,
                                     Writer& out)
{
    processStyle(o, out);
    processText(o, out);
}

void ODrawToOdf::processStyle(const MSO::OfficeArtSpContainer& o,
                              Writer& out)
{
    addGraphicStyleToDrawElement(out, o);
    set2dGeometry(o, out);
}

void ODrawToOdf::processText(const MSO::OfficeArtSpContainer& o,
                             Writer& out)
{
    if (o.clientData && client && client->onlyClientData(*o.clientData)) {
        client->processClientData(o.clientTextbox.data(), *o.clientData, out);
    } else if (o.clientTextbox) {
        client->processClientTextBox(*o.clientTextbox, o.clientData.data(), out);
    }
}

void ODrawToOdf::processModifiers(const MSO::OfficeArtSpContainer &o, Writer &out, const QList<int>& defaults)
{
    const AdjustValue* val1 = get<AdjustValue>(o);
    if (!val1 && defaults.isEmpty()) return;
    const Adjust2Value* val2 = get<Adjust2Value>(o);
    const Adjust3Value* val3 = get<Adjust3Value>(o);
    const Adjust4Value* val4 = get<Adjust4Value>(o);
    const Adjust5Value* val5 = get<Adjust5Value>(o);
    const Adjust6Value* val6 = get<Adjust6Value>(o);
    const Adjust7Value* val7 = get<Adjust7Value>(o);
    const Adjust8Value* val8 = get<Adjust8Value>(o);

    QString modifiers = QString::number(val1 ? val1->adjustvalue : defaults[0]);
    if (val2 || defaults.size() > 1) {
        modifiers += QString(" %1").arg(val2 ? val2->adjust2value : defaults[1]);
        if (val3 || defaults.size() > 2) {
            modifiers += QString(" %1").arg(val3 ? val3->adjust3value : defaults[2]);
            if (val4 || defaults.size() > 3) {
                modifiers += QString(" %1").arg(val4 ? val4->adjust4value : defaults[3]);
                if (val5 || defaults.size() > 4) {
                    modifiers += QString(" %1").arg(val5 ? val5->adjust5value : defaults[4]);
                    if (val6 || defaults.size() > 5) {
                        modifiers += QString(" %1").arg(val6 ? val6->adjust6value : defaults[5]);
                        if (val7 || defaults.size() > 6) {
                            modifiers += QString(" %1").arg(val7 ? val7->adjust7value : defaults[6]);
                            if (val8 || defaults.size() > 7) {
                                modifiers += QString(" %1").arg(val8 ? val8->adjust8value : defaults[7]);
                            }
                        }
                    }
                }
            }
        }
    }

    out.xml.addAttribute("draw:modifiers", modifiers);
}

// Position the shape into the slide or into a group shape
void ODrawToOdf::set2dGeometry(const OfficeArtSpContainer& o, Writer& out)
{
    const OfficeArtDggContainer* dgg = 0;
    const OfficeArtSpContainer* master = 0;
    const DrawStyle ds(dgg, master, &o);
    const qreal rotation = toQReal(ds.rotation());

    //transform the rectangle into the coordinate system of the group shape
    QRectF rect = getRect(o);
    QRectF trect (out.hOffset(rect.x()), out.vOffset(rect.y()),
                  out.hLength(rect.width()), out.vLength(rect.height()));

    //draw:caption-id
    //draw:class-names
    //draw:data
    //draw:engine
    //draw:id
    //draw:layer
    out.xml.addAttribute("draw:layer", "layout");
    //draw:name
    //draw:style-name
    //draw:text-style-name
    //draw:transform
    if (rotation) {

        const quint16 shapeType = o.shapeProp.rh.recInstance;
        const quint16 nrotation = normalizeRotation(rotation);
        const qreal angle = (nrotation / (qreal)180) * M_PI;

        trect = processRect(shapeType, rotation, trect);

        static const QString transform_str("translate(%1 %2) rotate(%3) translate(%4 %5)");
        const QPointF center = trect.center();
        const qreal height = trect.height();
        const qreal width = trect.width();

        out.xml.addAttribute("draw:transform",
                             transform_str.arg(client->formatPos(-width/2)).arg(client->formatPos(-height/2)).arg(-angle).arg(client->formatPos(center.x())).arg(client->formatPos(center.y())));
    }
    //svg:x
    //svg:y
    else {
        out.xml.addAttribute("svg:x", client->formatPos(trect.x()));
        out.xml.addAttribute("svg:y", client->formatPos(trect.y()));
    }
    //draw:z-index
    //presentation:class-names
    //presentation:style-name
    //svg:height
    out.xml.addAttribute("svg:height", client->formatPos(trect.height()));
    //svg:width
    out.xml.addAttribute("svg:width", client->formatPos(trect.width()));
    //table:end-cell-address
    //table:end-x
    //table:end-y
    //table:table-background
    //text:anchor-page-number
    //text:anchor-type
    //xml:id
}

void ODrawToOdf::setEnhancedGeometry(const MSO::OfficeArtSpContainer& o, Writer& out)
{
    const OfficeArtDggContainer* drawingGroup = 0;
    const OfficeArtSpContainer* master = 0;
    const DrawStyle ds(drawingGroup, master, &o);

    IMsoArray _v = ds.pVertices_complex();
    IMsoArray _c = ds.pSegmentInfo_complex();

    if (!_v.data.isEmpty() && !_c.data.isEmpty()) {

        QVector<QPoint> verticesPoints;

        //_v.data is an array of POINTs, MS-ODRAW, page 89
        QByteArray xArray(sizeof(int), 0), yArray(sizeof(int), 0);
        int step = _v.cbElem;
        if (step == 0xfff0) {
            step = 4;
        }

        int maxX = 0, minX = INT_MAX, maxY = 0, minY = INT_MAX;
        int x,y;

        //get vertice points
        for (int i = 0, offset = 0; i < _v.nElems; i++, offset += step) {
            // x coordinate of this point
            xArray.replace(0, step/2, _v.data.mid(offset, step/2));
            x = *(int*) xArray.data();

            // y coordinate of this point
            yArray.replace(0, step/2, _v.data.mid(offset + step/2, step/2));
            y = *(int*) yArray.data();

            verticesPoints.append(QPoint(x, y));

            // find maximum and minimum coordinates
            if (maxY < y) {
                maxY = y;
            }
            if (minY > y) {
                minY = y ;
            }
            if (maxX < x) {
                maxX = x;
            }
            if (minX > x) {
                minX = x;
            }
        }

        //TODO: geoLeft, geoTop, geoRight, geoBottom
        QString viewBox = QString::number(minX) + ' ' + QString::number(minY) + ' ' +
                          QString::number(maxX) + ' ' + QString::number(maxY);

        // combine segmentationInfoData and verticePoints into enhanced-path string
        QString enhancedPath;
        ushort msopathtype;
        bool nOffRange = false;

        for (int i = 0, n = 0; ((i < _c.nElems) && !nOffRange); i++) {

            msopathtype = (((*(ushort *)(_c.data.data() + i * 2)) >> 13) & 0x7);

            switch (msopathtype) {
            case msopathLineTo:
            {
                if (n >= verticesPoints.size()) {
                    qDebug() << "EnhancedGeometry: index into verticesPoints out of range!";
                    nOffRange = true;
                    break;
                }
                enhancedPath = enhancedPath + "L " + QString::number(verticesPoints[n].x()) + ' ' +
                               QString::number(verticesPoints[n].y()) + ' ';
                n++;
                break;
            }
            case msopathCurveTo:
            {
                if (n + 2 > verticesPoints.size()) {
                    qDebug() << "EnhancedGeometry: index into verticesPoints out of range!";
                    nOffRange = true;
                    break;
                }
                QPoint pt1 = verticesPoints.at(n);
                QPoint pt2 = verticesPoints.at(n + 1);
                QPoint pt3 = verticesPoints.at(n + 2);

                enhancedPath = enhancedPath + "C " +
                        QString::number(pt1.x()) + ' ' +
                        QString::number(pt1.y()) + ' ' +
                        QString::number(pt2.x()) + ' ' +
                        QString::number(pt2.y()) + ' ' +
                        QString::number(pt3.x()) + ' ' +
                        QString::number(pt3.y()) + ' ';
                n = n + 3;
                break;
            }
            case msopathMoveTo:
            {
                if (n >= verticesPoints.size()) {
                    qDebug() << "EnhancedGeometry: index into verticesPoints out of range!";
                    nOffRange = true;
                    break;
                }
                enhancedPath = enhancedPath + "M " + QString::number(verticesPoints[n].x()) + ' ' +
                               QString::number(verticesPoints[n].y()) + ' ';
                n++;
                break;
            }
            case msopathClose:
                enhancedPath = enhancedPath + "Z ";
                break;
            case msopathEnd:
                enhancedPath = enhancedPath + "N ";
                break;
            case msopathEscape:
            case msopathClientEscape:
                 break;
            }
        }
        //dr3d:projection
        //dr3d:shade-mode
        //draw:concentric-gradient-fill-allowed
        //draw:enhanced-path
        out.xml.addAttribute("draw:enhanced-path", enhancedPath);
        //draw:extrusion
        //draw:extrusion-allowed
        //draw:extrusion-brightness
        //draw:extrusion-color
        //draw:extrusion-depth
        //draw:extrusion-diffusion
        //draw:extrusion-first-light-direction
        //draw:extrusion-first-light-harsh
        //draw:extrusion-first-light-level
        //draw:extrusion-light-face
        //draw:extrusion-metal
        //draw:extrusion-number-of-line-segments
        //draw:extrusion-origin
        //draw:extrusion-rotation-angle
        //draw:extrusion-rotation-center
        //draw:extrusion-second-light-direction
        //draw:extrusion-second-light-harsh
        //draw:extrusion-second-light-level
        //draw:extrusion-shininess
        //draw:extrusion-skew
        //draw:extrusion-specularity
        //draw:extrusion-viewpoint
        //draw:glue-point-leaving-directions
        //draw:glue-points
        //draw:glue-point-type
        //draw:mirror-horizontal
        if (o.shapeProp.fFlipH) {
            out.xml.addAttribute("draw:mirror-horizontal", "true");
        }
        //draw:mirror-vertical
        if (o.shapeProp.fFlipV) {
            out.xml.addAttribute("draw:mirror-vertical", "true");
        }
        //draw:modifiers
        //draw:path-stretchpoint-x
        //draw:path-stretchpoint-y
        //draw:text-areas
        //draw:text-path
        //draw:text-path-allowed
        //draw:text-path-mode
        //draw:text-path-same-letter-heights
        //draw:text-path-scale
        //draw:text-rotate-angle
        //draw:type
        out.xml.addAttribute("draw:type", "non-primitive");
        //svg:viewBox
        out.xml.addAttribute("svg:viewBox", viewBox);
    }
}

QString ODrawToOdf::path2svg(const QPainterPath &path)
{
    QString d;

    int count = path.elementCount();
    for (int i = 0; i < count; i++) {

        QPainterPath::Element e = path.elementAt(i);
        switch(e.type){
            case QPainterPath::MoveToElement:{
                d.append(QString("M %1 %2").arg(e.x).arg(e.y));
                break;
            }
            case QPainterPath::LineToElement:{
                d.append(QString("L %1 %2").arg(e.x).arg(e.y));
                break;
            }
            default:{
                //TODO CurveToElement, CurveToElementDataElement
                qDebug() << "This element unhandled";
            }
        }
    }
    return d;
}

void ODrawToOdf::setShapeMirroring(const MSO::OfficeArtSpContainer& o, Writer& out)
{
    if (o.shapeProp.fFlipV) {
        out.xml.addAttribute("draw:mirror-vertical", "true");
    }
    if (o.shapeProp.fFlipH) {
        out.xml.addAttribute("draw:mirror-horizontal", "true");
    }
}
