/*
 *  Copyright (c) 2011 Lukáš Tvrdý lukas.tvrdy@ixonos.com
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "ODrawToOdf.h"

#include <KoXmlWriter.h>
#include "drawstyle.h"

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


/*void ODrawToOdf::processRectangle(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L 21600 0 21600 21600 0 21600 0 0 Z N");
    out.xml.addAttribute("draw:type", "rectangle");
    setShapeMirroring(o, out);
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}*/


void ODrawToOdf::processParallelogram(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "?f6 0 10800 ?f8 ?f11 10800 ?f9 21600 10800 ?f10 ?f5 10800");
    processModifiers(o, out, QList<int>() << 5400);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M ?f0 0 L 21600 0 ?f1 21600 0 21600 Z N");
    out.xml.addAttribute("draw:type", "parallelogram");
    out.xml.addAttribute("draw:text-areas", "?f3 ?f3 ?f4 ?f4");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","21600-$0 ");
    equation(out,"f2","$0 *10/24");
    equation(out,"f3","?f2 +1750");
    equation(out,"f4","21600-?f3 ");
    equation(out,"f5","?f0 /2");
    equation(out,"f6","10800+?f5 ");
    equation(out,"f7","?f0 -10800");
    equation(out,"f8","if(?f7 ,?f13 ,0)");
    equation(out,"f9","10800-?f5 ");
    equation(out,"f10","if(?f7 ,?f12 ,21600)");
    equation(out,"f11","21600-?f5 ");
    equation(out,"f12","21600*10800/?f0 ");
    equation(out,"f13","21600-?f12 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-maximum", "21600");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processTrapezoid(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "?f6 10800 10800 21600 ?f5 10800 10800 0");
    processModifiers(o, out, QList<int>() << 5400);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L 21600 0 ?f0 21600 ?f1 21600 Z N");
    out.xml.addAttribute("draw:type", "trapezoid");
    out.xml.addAttribute("draw:text-areas", "?f3 ?f3 ?f4 ?f4");
    setShapeMirroring(o, out);
    equation(out,"f0","21600-$0 ");
    equation(out,"f1","$0 ");
    equation(out,"f2","$0 *10/18");
    equation(out,"f3","?f2 +1750");
    equation(out,"f4","21600-?f3 ");
    equation(out,"f5","$0 /2");
    equation(out,"f6","21600-?f5 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 bottom");
    out.xml.addAttribute("draw:handle-range-x-maximum", "10800");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processDiamond(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 0 0 10800 10800 21600 21600 10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 10800 0 L 21600 10800 10800 21600 0 10800 10800 0 Z N");
    out.xml.addAttribute("draw:type", "diamond");
    out.xml.addAttribute("draw:text-areas", "5400 5400 16200 16200");
    setShapeMirroring(o, out);
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processRoundRectangle(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 3600);
    out.xml.addAttribute("draw:path-stretchpoint-x", "10800");
    out.xml.addAttribute("draw:path-stretchpoint-y", "10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M ?f7 0 X 0 ?f8 L 0 ?f9 Y ?f7 21600 L ?f10 21600 X 21600 ?f9 L 21600 ?f8 Y ?f10 0 Z N");
    out.xml.addAttribute("draw:type", "round-rectangle");
    out.xml.addAttribute("draw:text-areas", "?f3 ?f4 ?f5 ?f6");
    setShapeMirroring(o, out);
    equation(out,"f0","45");
    equation(out,"f1","$0 *sin(?f0 *(pi/180))");
    equation(out,"f2","?f1 *3163/7636");
    equation(out,"f3","left+?f2 ");
    equation(out,"f4","top+?f2 ");
    equation(out,"f5","right-?f2 ");
    equation(out,"f6","bottom-?f2 ");
    equation(out,"f7","left+$0 ");
    equation(out,"f8","top+$0 ");
    equation(out,"f9","bottom-$0 ");
    equation(out,"f10","right-$0 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-maximum", "10800");
    out.xml.addAttribute("draw:handle-switched", "true");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processOctagon(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 0 0 10800 10800 21600 21600 10800");
    processModifiers(o, out, QList<int>() << 5000);
    out.xml.addAttribute("draw:path-stretchpoint-x", "10800");
    out.xml.addAttribute("draw:path-stretchpoint-y", "10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M ?f0 0 L ?f2 0 21600 ?f1 21600 ?f3 ?f2 21600 ?f0 21600 0 ?f3 0 ?f1 Z N");
    out.xml.addAttribute("draw:type", "octagon");
    out.xml.addAttribute("draw:text-areas", "?f5 ?f6 ?f7 ?f8");
    setShapeMirroring(o, out);
    equation(out,"f0","left+$0 ");
    equation(out,"f1","top+$0 ");
    equation(out,"f2","right-$0 ");
    equation(out,"f3","bottom-$0 ");
    equation(out,"f4","$0 /2");
    equation(out,"f5","left+?f4 ");
    equation(out,"f6","top+?f4 ");
    equation(out,"f7","right-?f4 ");
    equation(out,"f8","bottom-?f4 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-maximum", "10800");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processIsocelesTriangle(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 0 ?f1 10800 0 21600 10800 21600 21600 21600 ?f7 10800");
    processModifiers(o, out, QList<int>() << 10800);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M ?f0 0 L 21600 21600 0 21600 Z N");
    out.xml.addAttribute("draw:type", "isosceles-triangle");
    out.xml.addAttribute("draw:text-areas", "?f1 10800 ?f2 18000 ?f3 7200 ?f4 21600");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","$0 /2");
    equation(out,"f2","?f1 +10800");
    equation(out,"f3","$0 *2/3");
    equation(out,"f4","?f3 +7200");
    equation(out,"f5","21600-?f0 ");
    equation(out,"f6","?f5 /2");
    equation(out,"f7","21600-?f6 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-maximum", "21600");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processRightTriangle(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 0 5400 10800 0 21600 10800 21600 21600 21600 16200 10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L 21600 21600 0 21600 0 0 Z N");
    out.xml.addAttribute("draw:type", "right-triangle");
    out.xml.addAttribute("draw:text-areas", "1900 12700 12700 19700");
    setShapeMirroring(o, out);
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processEllipse(const MSO::OfficeArtSpContainer& o, Writer& out)
{
    out.xml.startElement("draw:ellipse");
    processStyleAndText(o, out);
    out.xml.endElement(); // draw:ellipse
}

/*
void ODrawToOdf::processEllipse(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 0 3163 3163 0 10800 3163 18437 10800 21600 18437 18437 21600 10800 18437 3163");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "U 10800 10800 10800 10800 0 360 Z N");
    out.xml.addAttribute("draw:type", "ellipse");
    out.xml.addAttribute("draw:text-areas", "3163 3163 18437 18437");
    setShapeMirroring(o, out);
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}
*/

void ODrawToOdf::processHexagon(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 0 0 10800 10800 21600 21600 10800");
    processModifiers(o, out, QList<int>() << 5400);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M ?f0 0 L ?f1 0 21600 10800 ?f1 21600 ?f0 21600 0 10800 Z N");
    out.xml.addAttribute("draw:type", "hexagon");
    out.xml.addAttribute("draw:text-areas", "?f3 ?f3 ?f4 ?f4");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","21600-$0 ");
    equation(out,"f2","$0 *100/234");
    equation(out,"f3","?f2 +1700");
    equation(out,"f4","21600-?f3 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-maximum", "10800");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processPlus(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 0 0 10800 10800 21600 21600 10800");
    processModifiers(o, out, QList<int>() << 5400);
    out.xml.addAttribute("draw:path-stretchpoint-x", "10800");
    out.xml.addAttribute("draw:path-stretchpoint-y", "10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M ?f1 0 L ?f2 0 ?f2 ?f1 21600 ?f1 21600 ?f3 ?f2 ?f3 ?f2 21600 ?f1 21600 ?f1 ?f3 0 ?f3 0 ?f1 ?f1 ?f1 ?f1 0 Z N");
    out.xml.addAttribute("draw:type", "cross");
    out.xml.addAttribute("draw:text-areas", "?f1 ?f1 ?f2 ?f3");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 *10799/10800");
    equation(out,"f1","?f0 ");
    equation(out,"f2","right-?f0 ");
    equation(out,"f3","bottom-?f0 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-maximum", "10800");
    out.xml.addAttribute("draw:handle-switched", "true");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processPentagon(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 0 0 8260 4230 21600 10800 21600 17370 21600 21600 8260");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 10800 0 L 0 8260 4230 21600 17370 21600 21600 8260 10800 0 Z N");
    out.xml.addAttribute("draw:type", "pentagon");
    out.xml.addAttribute("draw:text-areas", "4230 5080 17370 21600");
    setShapeMirroring(o, out);
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processCan(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "44 ?f6 44 0 0 10800 44 21600 88 10800");
    processModifiers(o, out, QList<int>() << 5400);
    out.xml.addAttribute("svg:viewBox", "0 0 88 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 44 0 C 20 0 0 ?f2 0 ?f0 L 0 ?f3 C 0 ?f4 20 21600 44 21600 68 21600 88 ?f4 88 ?f3 L 88 ?f0 C 88 ?f2 68 0 44 0 Z N M 44 0 C 20 0 0 ?f2 0 ?f0 0 ?f5 20 ?f6 44 ?f6 68 ?f6 88 ?f5 88 ?f0 88 ?f2 68 0 44 0 Z N");
    out.xml.addAttribute("draw:type", "can");
    out.xml.addAttribute("draw:text-areas", "0 ?f6 88 ?f3");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 *2/4");
    equation(out,"f1","?f0 *6/11");
    equation(out,"f2","?f0 -?f1 ");
    equation(out,"f3","21600-?f0 ");
    equation(out,"f4","?f3 +?f1 ");
    equation(out,"f5","?f0 +?f1 ");
    equation(out,"f6","$0 *2/2");
    equation(out,"f7","44");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "?f7 $0");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "10800");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processCube(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "?f7 0 ?f6 ?f1 0 ?f10 ?f6 21600 ?f4 ?f10 21600 ?f9");
    processModifiers(o, out, QList<int>() << 5400);
    out.xml.addAttribute("draw:path-stretchpoint-x", "10800");
    out.xml.addAttribute("draw:path-stretchpoint-y", "10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 ?f12 L 0 ?f1 ?f2 0 ?f11 0 ?f11 ?f3 ?f4 ?f12 Z N M 0 ?f1 L ?f2 0 ?f11 0 ?f4 ?f1 Z N M ?f4 ?f12 L ?f4 ?f1 ?f11 0 ?f11 ?f3 Z N");
    out.xml.addAttribute("draw:type", "cube");
    out.xml.addAttribute("draw:text-areas", "0 ?f1 ?f4 ?f12");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","top+?f0 ");
    equation(out,"f2","left+?f0 ");
    equation(out,"f3","bottom-?f0 ");
    equation(out,"f4","right-?f0 ");
    equation(out,"f5","right-?f2 ");
    equation(out,"f6","?f5 /2");
    equation(out,"f7","?f2 +?f6 ");
    equation(out,"f8","bottom-?f1 ");
    equation(out,"f9","?f8 /2");
    equation(out,"f10","?f1 +?f9 ");
    equation(out,"f11","right");
    equation(out,"f12","bottom");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "left $0");
    out.xml.addAttribute("draw:handle-switched", "true");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "21600");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processBevel(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 2700);
    out.xml.addAttribute("draw:path-stretchpoint-x", "10800");
    out.xml.addAttribute("draw:path-stretchpoint-y", "10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L 21600 0 21600 21600 0 21600 Z N M 0 0 L 21600 0 ?f1 ?f0 ?f0 ?f0 Z N M 21600 0 L 21600 21600 ?f1 ?f2 ?f1 ?f0 Z N M 21600 21600 L 0 21600 ?f0 ?f2 ?f1 ?f2 Z N M 0 21600 L 0 0 ?f0 ?f0 ?f0 ?f2 Z N");
    out.xml.addAttribute("draw:type", "quad-bevel");
    out.xml.addAttribute("draw:text-areas", "?f0 ?f0 ?f1 ?f2");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 *21599/21600");
    equation(out,"f1","right-?f0 ");
    equation(out,"f2","bottom-?f0 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-maximum", "10800");
    out.xml.addAttribute("draw:handle-switched", "true");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processFoldedCorner(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 0 0 10800 10800 21600 21600 10800");
    processModifiers(o, out, QList<int>() << 18900);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L 21600 0 21600 ?f0 ?f0 21600 0 21600 Z N M ?f0 21600 L ?f3 ?f0 C ?f8 ?f9 ?f10 ?f11 21600 ?f0 Z N");
    out.xml.addAttribute("draw:type", "paper");
    out.xml.addAttribute("draw:text-areas", "0 0 21600 ?f11");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","21600-?f0 ");
    equation(out,"f2","?f1 *8000/10800");
    equation(out,"f3","21600-?f2 ");
    equation(out,"f4","?f1 /2");
    equation(out,"f5","?f1 /4");
    equation(out,"f6","?f1 /7");
    equation(out,"f7","?f1 /16");
    equation(out,"f8","?f3 +?f5 ");
    equation(out,"f9","?f0 +?f6 ");
    equation(out,"f10","21600-?f4 ");
    equation(out,"f11","?f0 +?f7 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 bottom");
    out.xml.addAttribute("draw:handle-range-x-maximum", "21600");
    out.xml.addAttribute("draw:handle-range-x-minimum", "10800");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processSmileyFace(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 0 3163 3163 0 10800 3163 18437 10800 21600 18437 18437 21600 10800 18437 3163");
    processModifiers(o, out, QList<int>() << 17520);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "U 10800 10800 10800 10800 0 360 Z N U 7305 7515 1165 1165 0 360 Z N U 14295 7515 1165 1165 0 360 Z N M 4870 ?f1 C 8680 ?f2 12920 ?f2 16730 ?f1 F N");
    out.xml.addAttribute("draw:type", "smiley");
    out.xml.addAttribute("draw:text-areas", "3163 3163 18437 18437");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 -15510");
    equation(out,"f1","17520-?f0 ");
    equation(out,"f2","15510+?f0 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "10800 $0");
    out.xml.addAttribute("draw:handle-range-y-minimum", "15510");
    out.xml.addAttribute("draw:handle-range-y-maximum", "17520");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processDonut(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 0 3163 3163 0 10800 3163 18437 10800 21600 18437 18437 21600 10800 18437 3163");
    processModifiers(o, out, QList<int>() << 5400);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "U 10800 10800 10800 10800 0 360 Z U 10800 10800 ?f1 ?f1 0 360 N");
    out.xml.addAttribute("draw:type", "ring");
    out.xml.addAttribute("draw:text-areas", "3163 3163 18437 18437");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","10800-$0 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 10800");
    out.xml.addAttribute("draw:handle-range-x-maximum", "10800");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processNoSmoking(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 0 3163 3163 0 10800 3163 18437 10800 21600 18437 18437 21600 10800 18437 3163");
    processModifiers(o, out, QList<int>() << 2700);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "U 10800 10800 10800 10800 0 360 Z B ?f0 ?f0 ?f1 ?f1 ?f9 ?f10 ?f11 ?f12 Z B ?f0 ?f0 ?f1 ?f1 ?f13 ?f14 ?f15 ?f16 Z N");
    out.xml.addAttribute("draw:type", "forbidden");
    out.xml.addAttribute("draw:text-areas", "3163 3163 18437 18437");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","21600-$0 ");
    equation(out,"f2","10800-$0 ");
    equation(out,"f3","$0 /2");
    equation(out,"f4","sqrt(?f2 *?f2 -?f3 *?f3 )");
    equation(out,"f5","10800-?f3 ");
    equation(out,"f6","10800+?f3 ");
    equation(out,"f7","10800-?f4 ");
    equation(out,"f8","10800+?f4 ");
    equation(out,"f9","(cos(45*(pi/180))*(?f5 -10800)+sin(45*(pi/180))*(?f7 -10800))+10800");
    equation(out,"f10","-(sin(45*(pi/180))*(?f5 -10800)-cos(45*(pi/180))*(?f7 -10800))+10800");
    equation(out,"f11","(cos(45*(pi/180))*(?f5 -10800)+sin(45*(pi/180))*(?f8 -10800))+10800");
    equation(out,"f12","-(sin(45*(pi/180))*(?f5 -10800)-cos(45*(pi/180))*(?f8 -10800))+10800");
    equation(out,"f13","(cos(45*(pi/180))*(?f6 -10800)+sin(45*(pi/180))*(?f8 -10800))+10800");
    equation(out,"f14","-(sin(45*(pi/180))*(?f6 -10800)-cos(45*(pi/180))*(?f8 -10800))+10800");
    equation(out,"f15","(cos(45*(pi/180))*(?f6 -10800)+sin(45*(pi/180))*(?f7 -10800))+10800");
    equation(out,"f16","-(sin(45*(pi/180))*(?f6 -10800)-cos(45*(pi/180))*(?f7 -10800))+10800");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 10800");
    out.xml.addAttribute("draw:handle-range-x-maximum", "7200");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processBlockArc(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 180<< 5400);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "B 0 0 21600 21600 ?f4 ?f3 ?f2 ?f3 W ?f5 ?f5 ?f6 ?f6 ?f2 ?f3 ?f4 ?f3 Z N");
    out.xml.addAttribute("draw:type", "block-arc");
    setShapeMirroring(o, out);
    equation(out,"f0","10800*cos($0 *(pi/180))");
    equation(out,"f1","10800*sin($0 *(pi/180))");
    equation(out,"f2","?f0 +10800");
    equation(out,"f3","?f1 +10800");
    equation(out,"f4","21600-?f2 ");
    equation(out,"f5","10800-$1 ");
    equation(out,"f6","10800+$1 ");
    equation(out,"f7","?f5 *cos($0 *(pi/180))");
    equation(out,"f8","?f5 *sin($0 *(pi/180))");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$1 $0");
    out.xml.addAttribute("draw:handle-radius-range-minimum", "0");
    out.xml.addAttribute("draw:handle-radius-range-maximum", "10800");
    out.xml.addAttribute("draw:handle-polar", "10800 10800");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processHeart(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 2180 3090 10800 10800 21600 18490 10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21615 21602");
    out.xml.addAttribute("draw:enhanced-path", "M 10800 21599 L 321 6886 70 6036 C -9 5766 -1 5474 2 5192 6 4918 43 4641 101 4370 159 4103 245 3837 353 3582 460 3326 591 3077 741 2839 892 2598 1066 2369 1253 2155 1443 1938 1651 1732 1874 1543 2097 1351 2337 1174 2587 1014 2839 854 3106 708 3380 584 3656 459 3945 350 4237 264 4533 176 4838 108 5144 66 5454 22 5771 1 6086 3 6407 7 6731 35 7048 89 7374 144 7700 226 8015 335 8344 447 8667 590 8972 756 9297 932 9613 1135 9907 1363 10224 1609 10504 1900 10802 2169 L 11697 1363 C 11971 1116 12304 934 12630 756 12935 590 13528 450 13589 335 13901 226 14227 144 14556 89 14872 35 15195 7 15517 3 15830 0 16147 22 16458 66 16764 109 17068 177 17365 264 17658 349 17946 458 18222 584 18496 708 18762 854 19015 1014 19264 1172 19504 1349 19730 1543 19950 1731 20158 1937 20350 2155 20536 2369 20710 2598 20861 2839 21010 3074 21143 3323 21251 3582 21357 3835 21443 4099 21502 4370 21561 4639 21595 4916 21600 5192 21606 5474 21584 5760 21532 6036 21478 6326 21366 6603 21282 6887 L 10802 21602 Z N");
    out.xml.addAttribute("draw:type", "heart");
    out.xml.addAttribute("draw:text-areas", "5080 2540 16520 13550");
    setShapeMirroring(o, out);
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processLightningBolt(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "8458 0 0 3923 4993 9720 9987 14934 21600 21600 16558 12016 12831 6120");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 8458 0 L 0 3923 7564 8416 4993 9720 12197 13904 9987 14934 21600 21600 14768 12911 16558 12016 11030 6840 12831 6120 8458 0 Z N");
    out.xml.addAttribute("draw:type", "lightning");
    out.xml.addAttribute("draw:text-areas", "8680 7410 13970 14190");
    setShapeMirroring(o, out);
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processSun(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 0 0 10800 10800 21600 21600 10800");
    processModifiers(o, out, QList<int>() << 5400);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 10800 L ?f4 ?f8 ?f4 ?f9 Z N M ?f10 ?f11 L ?f12 ?f13 ?f14 ?f15 Z N M ?f16 ?f17 L ?f18 ?f19 ?f20 ?f21 Z N M ?f22 ?f23 L ?f24 ?f25 ?f26 ?f27 Z N M ?f28 ?f29 L ?f30 ?f31 ?f32 ?f33 Z N M ?f34 ?f35 L ?f36 ?f37 ?f38 ?f39 Z N M ?f40 ?f41 L ?f42 ?f43 ?f44 ?f45 Z N M ?f46 ?f47 L ?f48 ?f49 ?f50 ?f51 Z N U 10800 10800 ?f54 ?f54 0 360 Z N");
    out.xml.addAttribute("draw:type", "sun");
    out.xml.addAttribute("draw:text-areas", "?f52 ?f52 ?f53 ?f53");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","21600-$0 ");
    equation(out,"f2","$0 -2700");
    equation(out,"f3","?f2 *5080/7425");
    equation(out,"f4","?f3 +2540");
    equation(out,"f5","10125-$0 ");
    equation(out,"f6","?f5 *2120/7425");
    equation(out,"f7","?f6 +210");
    equation(out,"f8","10800+?f7 ");
    equation(out,"f9","10800-?f7 ");
    equation(out,"f10","(cos(45*(pi/180))*(0-10800)+sin(45*(pi/180))*(10800-10800))+10800");
    equation(out,"f11","-(sin(45*(pi/180))*(0-10800)-cos(45*(pi/180))*(10800-10800))+10800");
    equation(out,"f12","(cos(45*(pi/180))*(?f4 -10800)+sin(45*(pi/180))*(?f8 -10800))+10800");
    equation(out,"f13","-(sin(45*(pi/180))*(?f4 -10800)-cos(45*(pi/180))*(?f8 -10800))+10800");
    equation(out,"f14","(cos(45*(pi/180))*(?f4 -10800)+sin(45*(pi/180))*(?f9 -10800))+10800");
    equation(out,"f15","-(sin(45*(pi/180))*(?f4 -10800)-cos(45*(pi/180))*(?f9 -10800))+10800");
    equation(out,"f16","(cos(90*(pi/180))*(0-10800)+sin(90*(pi/180))*(10800-10800))+10800");
    equation(out,"f17","-(sin(90*(pi/180))*(0-10800)-cos(90*(pi/180))*(10800-10800))+10800");
    equation(out,"f18","(cos(90*(pi/180))*(?f4 -10800)+sin(90*(pi/180))*(?f8 -10800))+10800");
    equation(out,"f19","-(sin(90*(pi/180))*(?f4 -10800)-cos(90*(pi/180))*(?f8 -10800))+10800");
    equation(out,"f20","(cos(90*(pi/180))*(?f4 -10800)+sin(90*(pi/180))*(?f9 -10800))+10800");
    equation(out,"f21","-(sin(90*(pi/180))*(?f4 -10800)-cos(90*(pi/180))*(?f9 -10800))+10800");
    equation(out,"f22","(cos(135*(pi/180))*(0-10800)+sin(135*(pi/180))*(10800-10800))+10800");
    equation(out,"f23","-(sin(135*(pi/180))*(0-10800)-cos(135*(pi/180))*(10800-10800))+10800");
    equation(out,"f24","(cos(135*(pi/180))*(?f4 -10800)+sin(135*(pi/180))*(?f8 -10800))+10800");
    equation(out,"f25","-(sin(135*(pi/180))*(?f4 -10800)-cos(135*(pi/180))*(?f8 -10800))+10800");
    equation(out,"f26","(cos(135*(pi/180))*(?f4 -10800)+sin(135*(pi/180))*(?f9 -10800))+10800");
    equation(out,"f27","-(sin(135*(pi/180))*(?f4 -10800)-cos(135*(pi/180))*(?f9 -10800))+10800");
    equation(out,"f28","(cos(180*(pi/180))*(0-10800)+sin(180*(pi/180))*(10800-10800))+10800");
    equation(out,"f29","-(sin(180*(pi/180))*(0-10800)-cos(180*(pi/180))*(10800-10800))+10800");
    equation(out,"f30","(cos(180*(pi/180))*(?f4 -10800)+sin(180*(pi/180))*(?f8 -10800))+10800");
    equation(out,"f31","-(sin(180*(pi/180))*(?f4 -10800)-cos(180*(pi/180))*(?f8 -10800))+10800");
    equation(out,"f32","(cos(180*(pi/180))*(?f4 -10800)+sin(180*(pi/180))*(?f9 -10800))+10800");
    equation(out,"f33","-(sin(180*(pi/180))*(?f4 -10800)-cos(180*(pi/180))*(?f9 -10800))+10800");
    equation(out,"f34","(cos(225*(pi/180))*(0-10800)+sin(225*(pi/180))*(10800-10800))+10800");
    equation(out,"f35","-(sin(225*(pi/180))*(0-10800)-cos(225*(pi/180))*(10800-10800))+10800");
    equation(out,"f36","(cos(225*(pi/180))*(?f4 -10800)+sin(225*(pi/180))*(?f8 -10800))+10800");
    equation(out,"f37","-(sin(225*(pi/180))*(?f4 -10800)-cos(225*(pi/180))*(?f8 -10800))+10800");
    equation(out,"f38","(cos(225*(pi/180))*(?f4 -10800)+sin(225*(pi/180))*(?f9 -10800))+10800");
    equation(out,"f39","-(sin(225*(pi/180))*(?f4 -10800)-cos(225*(pi/180))*(?f9 -10800))+10800");
    equation(out,"f40","(cos(270*(pi/180))*(0-10800)+sin(270*(pi/180))*(10800-10800))+10800");
    equation(out,"f41","-(sin(270*(pi/180))*(0-10800)-cos(270*(pi/180))*(10800-10800))+10800");
    equation(out,"f42","(cos(270*(pi/180))*(?f4 -10800)+sin(270*(pi/180))*(?f8 -10800))+10800");
    equation(out,"f43","-(sin(270*(pi/180))*(?f4 -10800)-cos(270*(pi/180))*(?f8 -10800))+10800");
    equation(out,"f44","(cos(270*(pi/180))*(?f4 -10800)+sin(270*(pi/180))*(?f9 -10800))+10800");
    equation(out,"f45","-(sin(270*(pi/180))*(?f4 -10800)-cos(270*(pi/180))*(?f9 -10800))+10800");
    equation(out,"f46","(cos(315*(pi/180))*(0-10800)+sin(315*(pi/180))*(10800-10800))+10800");
    equation(out,"f47","-(sin(315*(pi/180))*(0-10800)-cos(315*(pi/180))*(10800-10800))+10800");
    equation(out,"f48","(cos(315*(pi/180))*(?f4 -10800)+sin(315*(pi/180))*(?f8 -10800))+10800");
    equation(out,"f49","-(sin(315*(pi/180))*(?f4 -10800)-cos(315*(pi/180))*(?f8 -10800))+10800");
    equation(out,"f50","(cos(315*(pi/180))*(?f4 -10800)+sin(315*(pi/180))*(?f9 -10800))+10800");
    equation(out,"f51","-(sin(315*(pi/180))*(?f4 -10800)-cos(315*(pi/180))*(?f9 -10800))+10800");
    equation(out,"f52","(cos(45*(pi/180))*($0 -10800)+sin(45*(pi/180))*(10800-10800))+10800");
    equation(out,"f53","(cos(225*(pi/180))*($0 -10800)+sin(225*(pi/180))*(10800-10800))+10800");
    equation(out,"f54","10800-$0 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 10800");
    out.xml.addAttribute("draw:handle-range-x-maximum", "10125");
    out.xml.addAttribute("draw:handle-range-x-minimum", "2700");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processMoon(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "21600 0 0 10800 21600 21600 ?f0 10800");
    processModifiers(o, out, QList<int>() << 10800);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 21600 0 C ?f3 ?f4 ?f0 5080 ?f0 10800 ?f0 16520 ?f3 ?f5 21600 21600 9740 21600 0 16730 0 10800 0 4870 9740 0 21600 0 Z N");
    out.xml.addAttribute("draw:type", "moon");
    out.xml.addAttribute("draw:text-areas", "?f9 ?f8 ?f0 ?f10");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","21600-$0 ");
    equation(out,"f2","?f1 /2");
    equation(out,"f3","?f2 +$0 ");
    equation(out,"f4","$0 *1794/10000");
    equation(out,"f5","21600-?f4 ");
    equation(out,"f6","$0 *400/18900");
    equation(out,"f7","(cos(?f6 *(pi/180))*(0-10800)+sin(?f6 *(pi/180))*(10800-10800))+10800");
    equation(out,"f8","-(sin(?f6 *(pi/180))*(0-10800)-cos(?f6 *(pi/180))*(10800-10800))+10800");
    equation(out,"f9","?f7 +?f7 ");
    equation(out,"f10","21600-?f8 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 10800");
    out.xml.addAttribute("draw:handle-range-x-maximum", "18900");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}

void ODrawToOdf::processNotchedCircularArrow(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");

    // custom way for processing modifieres as this shape
    // return the AdjustValue, AdjustValue2 16-bit shifted
    const AdjustValue* val1 = get<AdjustValue>(o);
    const Adjust2Value* val2 = get<Adjust2Value>(o);
    QString modifiers = QString::number(val1 ? val1->adjustvalue >> 16 : 270);
    modifiers += QString(" %1").arg(val2 ? val2->adjust2value >> 16 : 0);
    out.xml.addAttribute("draw:modifiers", modifiers);

    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "W 0 0 21600 21600 ?f3 ?f1 ?f7 ?f5 S L 10800 10800 Z N W 0 0 21600 21600 ?f3 ?f1 ?f7 ?f5 F N");
    out.xml.addAttribute("draw:type", "mso-spt100");
    out.xml.addAttribute("draw:text-areas", "10799 0 21599 10799");
    setShapeMirroring(o, out);
    equation(out,"f0","10800*sin($0 *(pi/180))");
    equation(out,"f1","?f0 +10800");
    equation(out,"f2","10800*cos($0 *(pi/180))");
    equation(out,"f3","?f2 +10800");
    equation(out,"f4","10800*sin($1 *(pi/180))");
    equation(out,"f5","?f4 +10800");
    equation(out,"f6","10800*cos($1 *(pi/180))");
    equation(out,"f7","?f6 +10800");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "10800 $0");
    out.xml.addAttribute("draw:handle-radius-range-minimum", "10800");
    out.xml.addAttribute("draw:handle-radius-range-maximum", "10800");
    out.xml.addAttribute("draw:handle-polar", "10800 10800");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "10800 $1");
    out.xml.addAttribute("draw:handle-radius-range-minimum", "10800");
    out.xml.addAttribute("draw:handle-radius-range-maximum", "10800");
    out.xml.addAttribute("draw:handle-polar", "10800 10800");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}

void ODrawToOdf::processBracketPair(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 0 0 10800 10800 21600 21600 10800");
    processModifiers(o, out, QList<int>() << 3700);
    out.xml.addAttribute("draw:path-stretchpoint-x", "10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M ?f0 0 X 0 ?f1 L 0 ?f2 Y ?f0 21600 N M ?f3 21600 X 21600 ?f2 L 21600 ?f1 Y ?f3 0 N");
    out.xml.addAttribute("draw:type", "bracket-pair");
    out.xml.addAttribute("draw:text-areas", "?f8 ?f9 ?f10 ?f11");
    setShapeMirroring(o, out);
    equation(out,"f0","left+$0 ");
    equation(out,"f1","top+$0 ");
    equation(out,"f2","bottom-$0 ");
    equation(out,"f3","right-$0 ");
    equation(out,"f4","-(sin(45*(pi/180))*($0 -10800)-cos(45*(pi/180))*(0-10800))+10800");
    equation(out,"f5","?f4 -10800");
    equation(out,"f6","-$0 ");
    equation(out,"f7","?f6 -?f5 ");
    equation(out,"f8","left-?f7 ");
    equation(out,"f9","top-?f7 ");
    equation(out,"f10","right+?f7 ");
    equation(out,"f11","bottom+?f7 ");
    equation(out,"f12","left-?f5 ");
    equation(out,"f13","top-?f5 ");
    equation(out,"f14","right+?f5 ");
    equation(out,"f15","bottom+?f5 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-maximum", "10800");
    out.xml.addAttribute("draw:handle-switched", "true");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processBracePair(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 0 0 10800 10800 21600 21600 10800");
    processModifiers(o, out, QList<int>() << 1800);
    out.xml.addAttribute("draw:path-stretchpoint-x", "10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M ?f4 0 X ?f0 ?f1 L ?f0 ?f6 Y 0 10800 X ?f0 ?f7 L ?f0 ?f2 Y ?f4 21600 N M ?f8 21600 X ?f3 ?f2 L ?f3 ?f7 Y 21600 10800 X ?f3 ?f6 L ?f3 ?f1 Y ?f8 0 N");
    out.xml.addAttribute("draw:type", "brace-pair");
    out.xml.addAttribute("draw:text-areas", "?f11 ?f12 ?f13 ?f14");
    setShapeMirroring(o, out);
    equation(out,"f0","left+$0 ");
    equation(out,"f1","top+$0 ");
    equation(out,"f2","bottom-$0 ");
    equation(out,"f3","right-$0 ");
    equation(out,"f4","?f0 *2");
    equation(out,"f5","$0 *2");
    equation(out,"f6","10800-$0 ");
    equation(out,"f7","21600-?f6 ");
    equation(out,"f8","right-?f5 ");
    equation(out,"f9","$0 /3");
    equation(out,"f10","?f9 +$0 ");
    equation(out,"f11","left+?f10 ");
    equation(out,"f12","top+?f9 ");
    equation(out,"f13","right-?f10 ");
    equation(out,"f14","bottom-?f9 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "left $0");
    out.xml.addAttribute("draw:handle-switched", "true");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "5400");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processPlaque(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 0 0 10800 10800 21600 21600 10800");
    processModifiers(o, out, QList<int>() << 3600);
    out.xml.addAttribute("draw:path-stretchpoint-x", "10800");
    out.xml.addAttribute("draw:path-stretchpoint-y", "10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M ?f0 0 Y 0 ?f1 L 0 ?f2 X ?f0 21600 L ?f3 21600 Y 21600 ?f2 L 21600 ?f1 X ?f3 0 Z N");
    out.xml.addAttribute("draw:type", "mso-spt21");
    out.xml.addAttribute("draw:text-areas", "?f12 ?f13 ?f14 ?f15");
    setShapeMirroring(o, out);
    equation(out,"f0","left+$0 ");
    equation(out,"f1","top+$0 ");
    equation(out,"f2","bottom-$0 ");
    equation(out,"f3","right-$0 ");
    equation(out,"f4","-(sin(45*(pi/180))*($0 -10800)-cos(45*(pi/180))*(0-10800))+10800");
    equation(out,"f5","?f4 -10800");
    equation(out,"f6","-$0 ");
    equation(out,"f7","?f6 -?f5 ");
    equation(out,"f8","left-?f7 ");
    equation(out,"f9","top-?f7 ");
    equation(out,"f10","right+?f7 ");
    equation(out,"f11","bottom+?f7 ");
    equation(out,"f12","left-?f5 ");
    equation(out,"f13","top-?f5 ");
    equation(out,"f14","right+?f5 ");
    equation(out,"f15","bottom+?f5 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-maximum", "10800");
    out.xml.addAttribute("draw:handle-switched", "true");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processLeftBracket(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "21600 0 0 10800 21600 21600");
    processModifiers(o, out, QList<int>() << 1800);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 21600 0 C 10800 0 0 ?f3 0 ?f1 L 0 ?f2 C 0 ?f4 10800 21600 21600 21600 N");
    out.xml.addAttribute("draw:type", "left-bracket");
    out.xml.addAttribute("draw:text-areas", "6350 ?f3 21600 ?f4");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 /2");
    equation(out,"f1","top+$0 ");
    equation(out,"f2","bottom-$0 ");
    equation(out,"f3","top+?f0 ");
    equation(out,"f4","bottom-?f0 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "left $0");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "10800");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processRightBracket(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "0 0 0 21600 21600 10800");
    processModifiers(o, out, QList<int>() << 1800);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 C 10800 0 21600 ?f3 21600 ?f1 L 21600 ?f2 C 21600 ?f4 10800 21600 0 21600 N");
    out.xml.addAttribute("draw:type", "right-bracket");
    out.xml.addAttribute("draw:text-areas", "0 ?f3 15150 ?f4");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 /2");
    equation(out,"f1","top+$0 ");
    equation(out,"f2","bottom-$0 ");
    equation(out,"f3","top+?f0 ");
    equation(out,"f4","bottom-?f0 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "right $0");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "10800");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processLeftBrace(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "21600 0 0 10800 21600 21600");
    processModifiers(o, out, QList<int>() << 1800<< 10800);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 21600 0 C 16200 0 10800 ?f0 10800 ?f1 L 10800 ?f2 C 10800 ?f3 5400 ?f4 0 ?f4 5400 ?f4 10800 ?f5 10800 ?f6 L 10800 ?f7 C 10800 ?f8 16200 21600 21600 21600 N");
    out.xml.addAttribute("draw:type", "left-brace");
    out.xml.addAttribute("draw:text-areas", "13800 ?f9 21600 ?f10");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 /2");
    equation(out,"f1","$0 ");
    equation(out,"f2","?f4 -$0 ");
    equation(out,"f3","?f4 -?f0 ");
    equation(out,"f4","$1 ");
    equation(out,"f5","?f4 +?f0 ");
    equation(out,"f6","?f4 +$0 ");
    equation(out,"f7","21600-$0 ");
    equation(out,"f8","21600-?f0 ");
    equation(out,"f9","$0 *10000/31953");
    equation(out,"f10","21600-?f9 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "10800 $0");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "5400");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "left $1");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "21600");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processRightBrace(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "0 0 0 21600 21600 10800");
    processModifiers(o, out, QList<int>() << 1800<< 10800);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 C 5400 0 10800 ?f0 10800 ?f1 L 10800 ?f2 C 10800 ?f3 16200 ?f4 21600 ?f4 16200 ?f4 10800 ?f5 10800 ?f6 L 10800 ?f7 C 10800 ?f8 5400 21600 0 21600 N");
    out.xml.addAttribute("draw:type", "right-brace");
    out.xml.addAttribute("draw:text-areas", "0 ?f9 7800 ?f10");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 /2");
    equation(out,"f1","$0 ");
    equation(out,"f2","?f4 -$0 ");
    equation(out,"f3","?f4 -?f0 ");
    equation(out,"f4","$1 ");
    equation(out,"f5","?f4 +?f0 ");
    equation(out,"f6","?f4 +$0 ");
    equation(out,"f7","21600-$0 ");
    equation(out,"f8","21600-?f0 ");
    equation(out,"f9","$0 *10000/31953");
    equation(out,"f10","21600-?f9 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "10800 $0");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "5400");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "right $1");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "21600");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processArrow(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 16200<< 5400);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 ?f0 L ?f1 ?f0 ?f1 0 21600 10800 ?f1 21600 ?f1 ?f2 0 ?f2 Z N");
    out.xml.addAttribute("draw:type", "right-arrow");
    out.xml.addAttribute("draw:text-areas", "0 ?f0 ?f5 ?f2");
    setShapeMirroring(o, out);
    equation(out,"f0","$1 ");
    equation(out,"f1","$0 ");
    equation(out,"f2","21600-$1 ");
    equation(out,"f3","21600-?f1 ");
    equation(out,"f4","?f3 *?f0 /10800");
    equation(out,"f5","?f1 +?f4 ");
    equation(out,"f6","?f1 *?f0 /10800");
    equation(out,"f7","?f1 -?f6 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 $1");
    out.xml.addAttribute("draw:handle-range-x-maximum", "21600");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "10800");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processLeftArrow(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 5400<< 5400);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 21600 ?f0 L ?f1 ?f0 ?f1 0 0 10800 ?f1 21600 ?f1 ?f2 21600 ?f2 Z N");
    out.xml.addAttribute("draw:type", "left-arrow");
    out.xml.addAttribute("draw:text-areas", "?f7 ?f0 21600 ?f2");
    setShapeMirroring(o, out);
    equation(out,"f0","$1 ");
    equation(out,"f1","$0 ");
    equation(out,"f2","21600-$1 ");
    equation(out,"f3","21600-?f1 ");
    equation(out,"f4","?f3 *?f0 /10800");
    equation(out,"f5","?f1 +?f4 ");
    equation(out,"f6","?f1 *?f0 /10800");
    equation(out,"f7","?f1 -?f6 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 $1");
    out.xml.addAttribute("draw:handle-range-x-maximum", "21600");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "10800");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processUpArrow(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 5400<< 5400);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M ?f0 21600 L ?f0 ?f1 0 ?f1 10800 0 21600 ?f1 ?f2 ?f1 ?f2 21600 Z N");
    out.xml.addAttribute("draw:type", "up-arrow");
    out.xml.addAttribute("draw:text-areas", "?f0 ?f7 ?f2 21600");
    setShapeMirroring(o, out);
    equation(out,"f0","$1 ");
    equation(out,"f1","$0 ");
    equation(out,"f2","21600-$1 ");
    equation(out,"f3","21600-?f1 ");
    equation(out,"f4","?f3 *?f0 /10800");
    equation(out,"f5","?f1 +?f4 ");
    equation(out,"f6","?f1 *?f0 /10800");
    equation(out,"f7","?f1 -?f6 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$1 $0");
    out.xml.addAttribute("draw:handle-range-x-maximum", "10800");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "21600");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processDownArrow(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 16200<< 5400);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M ?f0 0 L ?f0 ?f1 0 ?f1 10800 21600 21600 ?f1 ?f2 ?f1 ?f2 0 Z N");
    out.xml.addAttribute("draw:type", "down-arrow");
    out.xml.addAttribute("draw:text-areas", "?f0 0 ?f2 ?f5");
    setShapeMirroring(o, out);
    equation(out,"f0","$1 ");
    equation(out,"f1","$0 ");
    equation(out,"f2","21600-$1 ");
    equation(out,"f3","21600-?f1 ");
    equation(out,"f4","?f3 *?f0 /10800");
    equation(out,"f5","?f1 +?f4 ");
    equation(out,"f6","?f1 *?f0 /10800");
    equation(out,"f7","?f1 -?f6 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$1 $0");
    out.xml.addAttribute("draw:handle-range-x-maximum", "10800");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "21600");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processLeftRightArrow(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 4300<< 5400);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 10800 L ?f0 0 ?f0 ?f1 ?f2 ?f1 ?f2 0 21600 10800 ?f2 21600 ?f2 ?f3 ?f0 ?f3 ?f0 21600 Z N");
    out.xml.addAttribute("draw:type", "left-right-arrow");
    out.xml.addAttribute("draw:text-areas", "?f5 ?f1 ?f6 ?f3");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","$1 ");
    equation(out,"f2","21600-$0 ");
    equation(out,"f3","21600-$1 ");
    equation(out,"f4","10800-$1 ");
    equation(out,"f5","$0 *?f4 /10800");
    equation(out,"f6","21600-?f5 ");
    equation(out,"f7","10800-$0 ");
    equation(out,"f8","$1 *?f7 /10800");
    equation(out,"f9","21600-?f8 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 $1");
    out.xml.addAttribute("draw:handle-range-x-maximum", "10800");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "10800");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processUpDownArrow(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 5400<< 4300);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 ?f1 L 10800 0 21600 ?f1 ?f2 ?f1 ?f2 ?f3 21600 ?f3 10800 21600 0 ?f3 ?f0 ?f3 ?f0 ?f1 Z N");
    out.xml.addAttribute("draw:type", "up-down-arrow");
    out.xml.addAttribute("draw:text-areas", "?f0 ?f8 ?f2 ?f9");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","$1 ");
    equation(out,"f2","21600-$0 ");
    equation(out,"f3","21600-$1 ");
    equation(out,"f4","10800-$1 ");
    equation(out,"f5","$0 *?f4 /10800");
    equation(out,"f6","21600-?f5 ");
    equation(out,"f7","10800-$0 ");
    equation(out,"f8","$1 *?f7 /10800");
    equation(out,"f9","21600-?f8 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 $1");
    out.xml.addAttribute("draw:handle-range-x-maximum", "10800");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "10800");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processQuadArrow(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 6500<< 8600<< 4300);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 10800 L ?f0 ?f1 ?f0 ?f2 ?f2 ?f2 ?f2 ?f0 ?f1 ?f0 10800 0 ?f3 ?f0 ?f4 ?f0 ?f4 ?f2 ?f5 ?f2 ?f5 ?f1 21600 10800 ?f5 ?f3 ?f5 ?f4 ?f4 ?f4 ?f4 ?f5 ?f3 ?f5 10800 21600 ?f1 ?f5 ?f2 ?f5 ?f2 ?f4 ?f0 ?f4 ?f0 ?f3 Z N");
    out.xml.addAttribute("draw:type", "quad-arrow");
    out.xml.addAttribute("draw:text-areas", "0 0 21600 21600");
    setShapeMirroring(o, out);
    equation(out,"f0","$2 ");
    equation(out,"f1","$0 ");
    equation(out,"f2","$1 ");
    equation(out,"f3","21600-$0 ");
    equation(out,"f4","21600-$1 ");
    equation(out,"f5","21600-$2 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$1 $2");
    out.xml.addAttribute("draw:handle-range-x-maximum", "10800");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "$0");
    out.xml.addAttribute("draw:handle-range-x-minimum", "$0");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-maximum", "$1");
    out.xml.addAttribute("draw:handle-range-x-minimum", "$2");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processLeftRightUpArrow(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 6500<< 8600<< 6200);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 10800 0 L ?f3 ?f2 ?f4 ?f2 ?f4 ?f1 ?f5 ?f1 ?f5 ?f0 21600 10800 ?f5 ?f3 ?f5 ?f4 ?f2 ?f4 ?f2 ?f3 0 10800 ?f2 ?f0 ?f2 ?f1 ?f1 ?f1 ?f1 ?f2 ?f0 ?f2 Z N");
    out.xml.addAttribute("draw:type", "mso-spt182");
    out.xml.addAttribute("draw:text-areas", "0 0 21600 21600");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","$1 ");
    equation(out,"f2","?f3 *$2 /21600");
    equation(out,"f3","21600-$0 ");
    equation(out,"f4","21600-$1 ");
    equation(out,"f5","21600-?f2 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$1 $2");
    out.xml.addAttribute("draw:handle-range-x-maximum", "10800");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "$0");
    out.xml.addAttribute("draw:handle-range-x-minimum", "$0");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-maximum", "$1");
    out.xml.addAttribute("draw:handle-range-x-minimum", "$2");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processBentArrow(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 15100<< 2900);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 21600 L 0 12160 Y 12427 ?f1 L ?f0 ?f1 ?f0 0 21600 6079 ?f0 12158 ?f0 ?f2 12427 ?f2 X ?f4 12160 L ?f4 21600 Z N");
    out.xml.addAttribute("draw:type", "mso-spt91");
    out.xml.addAttribute("draw:text-areas", "0 0 21600 21600");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","$1 ");
    equation(out,"f2","12158-$1 ");
    equation(out,"f3","6079-$1 ");
    equation(out,"f4","?f3 *2");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 $1");
    out.xml.addAttribute("draw:handle-range-x-maximum", "21600");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "6079");
    out.xml.addAttribute("draw:handle-range-x-minimum", "12427");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processUturnArrow(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 21600 L 0 8550 C 0 3540 4370 0 9270 0 13890 0 18570 3230 18600 8300 L 21600 8300 15680 14260 9700 8300 12500 8300 C 12320 6380 10870 5850 9320 5850 7770 5850 6040 6410 6110 8520 L 6110 21600 Z N");
    out.xml.addAttribute("draw:type", "mso-spt101");
    out.xml.addAttribute("draw:text-areas", "0 8280 6110 21600");
    setShapeMirroring(o, out);
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processLeftUpArrow(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 9340<< 18500<< 6200);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 ?f5 L ?f2 ?f0 ?f2 ?f7 ?f7 ?f7 ?f7 ?f2 ?f0 ?f2 ?f5 0 21600 ?f2 ?f1 ?f2 ?f1 ?f1 ?f2 ?f1 ?f2 21600 Z N");
    out.xml.addAttribute("draw:type", "mso-spt89");
    out.xml.addAttribute("draw:text-areas", "?f2 ?f7 ?f1 ?f1 ?f7 ?f2 ?f1 ?f1");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","$1 ");
    equation(out,"f2","$2 ");
    equation(out,"f3","21600-$0 ");
    equation(out,"f4","?f3 /2");
    equation(out,"f5","$0 +?f4 ");
    equation(out,"f6","21600-$1 ");
    equation(out,"f7","$0 +?f6 ");
    equation(out,"f8","21600-?f6 ");
    equation(out,"f9","?f8 -?f6 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$1 $2");
    out.xml.addAttribute("draw:handle-range-x-maximum", "21600");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "$0");
    out.xml.addAttribute("draw:handle-range-x-minimum", "?f5");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-maximum", "?f9");
    out.xml.addAttribute("draw:handle-range-x-minimum", "$2");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processBentUpArrow(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 9340<< 18500<< 7200);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 ?f8 L ?f7 ?f8 ?f7 ?f2 ?f0 ?f2 ?f5 0 21600 ?f2 ?f1 ?f2 ?f1 21600 0 21600 Z N");
    out.xml.addAttribute("draw:type", "mso-spt90");
    out.xml.addAttribute("draw:text-areas", "?f2 ?f7 ?f1 ?f1 ?f7 ?f2 ?f1 ?f1");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","$1 ");
    equation(out,"f2","$2 ");
    equation(out,"f3","21600-$0 ");
    equation(out,"f4","?f3 /2");
    equation(out,"f5","$0 +?f4 ");
    equation(out,"f6","21600-$1 ");
    equation(out,"f7","$0 +?f6 ");
    equation(out,"f8","?f7 +?f6 ");
    equation(out,"f9","21600-?f6 ");
    equation(out,"f10","?f9 -?f6 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$1 $2");
    out.xml.addAttribute("draw:handle-range-x-maximum", "21600");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "$0");
    out.xml.addAttribute("draw:handle-range-x-minimum", "?f5");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-maximum", "?f10");
    out.xml.addAttribute("draw:handle-range-x-minimum", "$2");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processCurvedRightArrow(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "0 ?f17 ?f2 ?f14 ?f22 ?f8 ?f2 ?f12 ?f22 ?f16");
    processModifiers(o, out, QList<int>() << 12960<< 19440<< 14400);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "B 0 0 ?f23 ?f3 ?f22 0 0 ?f4 A 0 ?f15 ?f23 ?f1 0 ?f7 ?f2 ?f13 L ?f2 ?f14 ?f22 ?f8 ?f2 ?f12 W 0 0 ?f23 ?f3 ?f2 ?f11 ?f26 ?f17 0 ?f15 ?f23 ?f1 ?f26 ?f17 ?f22 ?f15 Z N B 0 0 ?f23 ?f3 0 ?f4 ?f26 ?f17 F N");
    out.xml.addAttribute("draw:type", "mso-spt102");
    out.xml.addAttribute("draw:text-areas", "?f47 ?f45 ?f48 ?f46");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","$1 ");
    equation(out,"f2","$2 ");
    equation(out,"f3","$0 +21600-$1 ");
    equation(out,"f4","?f3 /2");
    equation(out,"f5","$1 +$1 -21600");
    equation(out,"f6","?f5 +$1 -$0 ");
    equation(out,"f7","?f6 /2");
    equation(out,"f8","(21600+$0 )/2");
    equation(out,"f9","21600-$2 ");
    equation(out,"f10","?f4 *sqrt(1-(?f9 /21600)*(?f9 /21600))");
    equation(out,"f11","?f4 +?f10 ");
    equation(out,"f12","?f11 +$1 -21600");
    equation(out,"f13","?f7 +?f10 ");
    equation(out,"f14","?f12 +21600-$0 ");
    equation(out,"f15","?f5 -$0 ");
    equation(out,"f16","?f15 /2");
    equation(out,"f17","(?f4 +?f7 )/2");
    equation(out,"f18","$0 +$1 -21600");
    equation(out,"f19","?f18 /2");
    equation(out,"f20","?f17 -?f19 ");
    equation(out,"f21","21600");
    equation(out,"f22","21600");
    equation(out,"f23","21600*2");
    equation(out,"f24","?f17 -?f4 ");
    equation(out,"f25","21600*sqrt(1-(?f24 /?f4 )*(?f24 /?f4 ))");
    equation(out,"f26","21600-?f25 ");
    equation(out,"f27","?f8 +128");
    equation(out,"f28","?f5 /2");
    equation(out,"f29","?f5 -128");
    equation(out,"f30","$0 +?f17 -?f12 ");
    equation(out,"f31","21600*sqrt(1-(?f20 /?f4 )*(?f20 /?f4 ))");
    equation(out,"f32","21600-$0 ");
    equation(out,"f33","?f32 /2");
    equation(out,"f34","21600*21600");
    equation(out,"f35","?f9 *?f9 ");
    equation(out,"f36","?f34 -?f35 ");
    equation(out,"f37","sqrt(?f36 )");
    equation(out,"f38","?f37 +21600");
    equation(out,"f39","21600*21600/?f38 ");
    equation(out,"f40","?f39 +64");
    equation(out,"f41","$0 /2");
    equation(out,"f42","21600*sqrt(1-(?f33 /?f41 )*(?f33 /?f41 ))");
    equation(out,"f43","21600-?f42 ");
    equation(out,"f44","?f43 +64");
    equation(out,"f45","?f4 /2");
    equation(out,"f46","$1 -?f45 ");
    equation(out,"f47","21600*2195/16384");
    equation(out,"f48","21600*14189/16384");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "21600 $0");
    out.xml.addAttribute("draw:handle-range-x-maximum", "10800");
    out.xml.addAttribute("draw:handle-range-y-minimum", "?f40");
    out.xml.addAttribute("draw:handle-range-y-maximum", "?f29");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "21600 $1");
    out.xml.addAttribute("draw:handle-range-x-maximum", "10800");
    out.xml.addAttribute("draw:handle-range-y-minimum", "?f27");
    out.xml.addAttribute("draw:handle-range-y-maximum", "?f21");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$2 21600");
    out.xml.addAttribute("draw:handle-range-x-maximum", "?f22");
    out.xml.addAttribute("draw:handle-range-y-minimum", "3375");
    out.xml.addAttribute("draw:handle-range-y-maximum", "21600");
    out.xml.addAttribute("draw:handle-range-x-minimum", "?f44");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processCurvedLeftArrow(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "0 ?f15 ?f2 ?f11 0 ?f8 ?f2 ?f13 ?f21 ?f16");
    processModifiers(o, out, QList<int>() << 12960<< 19440<< 7200);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "V ?f22 0 ?f21 ?f3 0 0 ?f21 ?f4 W ?f22 ?f14 ?f21 ?f1 ?f21 ?f7 ?f2 ?f12 L ?f2 ?f13 0 ?f8 ?f2 ?f11 A ?f22 0 ?f21 ?f3 ?f2 ?f10 ?f24 ?f16 ?f22 ?f14 ?f21 ?f1 ?f24 ?f16 0 ?f14 Z N B ?f22 ?f14 ?f21 ?f1 ?f21 ?f7 ?f24 ?f16 F N");
    out.xml.addAttribute("draw:type", "mso-spt103");
    out.xml.addAttribute("draw:text-areas", "?f43 ?f41 ?f44 ?f42");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","$1 ");
    equation(out,"f2","$2 ");
    equation(out,"f3","$0 +21600-$1 ");
    equation(out,"f4","?f3 /2");
    equation(out,"f5","$1 +$1 -21600");
    equation(out,"f6","?f5 +$1 -$0 ");
    equation(out,"f7","?f6 /2");
    equation(out,"f8","(21600+$0 )/2");
    equation(out,"f9","?f4 *sqrt(1-($2 /21600)*($2 /21600))");
    equation(out,"f10","?f4 +?f9 ");
    equation(out,"f11","?f10 +$1 -21600");
    equation(out,"f12","?f7 +?f9 ");
    equation(out,"f13","?f11 +21600-$0 ");
    equation(out,"f14","?f5 -$0 ");
    equation(out,"f15","?f14 /2");
    equation(out,"f16","(?f4 +?f7 )/2");
    equation(out,"f17","$0 +$1 -21600");
    equation(out,"f18","?f17 /2");
    equation(out,"f19","?f16 -?f18 ");
    equation(out,"f20","21600");
    equation(out,"f21","21600");
    equation(out,"f22","-21600");
    equation(out,"f23","?f16 -?f4 ");
    equation(out,"f24","21600*sqrt(1-(?f23 /?f4 )*(?f23 /?f4 ))");
    equation(out,"f25","?f8 +128");
    equation(out,"f26","?f5 /2");
    equation(out,"f27","?f5 -128");
    equation(out,"f28","$0 +?f16 -?f11 ");
    equation(out,"f29","21600-$0 ");
    equation(out,"f30","?f29 /2");
    equation(out,"f31","21600*21600");
    equation(out,"f32","$2 *$2 ");
    equation(out,"f33","?f31 -?f32 ");
    equation(out,"f34","sqrt(?f33 )");
    equation(out,"f35","?f34 +21600");
    equation(out,"f36","21600*21600/?f35 ");
    equation(out,"f37","?f36 +64");
    equation(out,"f38","$0 /2");
    equation(out,"f39","21600*sqrt(1-(?f30 /?f38 )*(?f30 /?f38 ))");
    equation(out,"f40","?f39 -64");
    equation(out,"f41","?f4 /2");
    equation(out,"f42","$1 -?f41 ");
    equation(out,"f43","21600*2195/16384");
    equation(out,"f44","21600*14189/16384");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "left $0");
    out.xml.addAttribute("draw:handle-range-x-maximum", "10800");
    out.xml.addAttribute("draw:handle-range-y-minimum", "?f37");
    out.xml.addAttribute("draw:handle-range-y-maximum", "?f27");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "left $1");
    out.xml.addAttribute("draw:handle-range-x-maximum", "10800");
    out.xml.addAttribute("draw:handle-range-y-minimum", "?f25");
    out.xml.addAttribute("draw:handle-range-y-maximum", "?f20");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$2 21600");
    out.xml.addAttribute("draw:handle-range-x-maximum", "?f40");
    out.xml.addAttribute("draw:handle-range-y-minimum", "3375");
    out.xml.addAttribute("draw:handle-range-y-maximum", "21600");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processCurvedUpArrow(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "?f8 0 ?f11 ?f2 ?f15 0 ?f16 ?f21 ?f13 ?f2");
    processModifiers(o, out, QList<int>() << 12960<< 19440<< 7200);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "B 0 ?f22 ?f3 ?f21 0 0 ?f4 ?f21 A ?f14 ?f22 ?f1 ?f21 ?f7 ?f21 ?f12 ?f2 L ?f13 ?f2 ?f8 0 ?f11 ?f2 W 0 ?f22 ?f3 ?f21 ?f10 ?f2 ?f16 ?f24 ?f14 ?f22 ?f1 ?f21 ?f16 ?f24 ?f14 0 Z N V ?f14 ?f22 ?f1 ?f21 ?f7 ?f21 ?f16 ?f24 F N");
    out.xml.addAttribute("draw:type", "mso-spt104");
    out.xml.addAttribute("draw:text-areas", "?f41 ?f43 ?f42 ?f44");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","$1 ");
    equation(out,"f2","$2 ");
    equation(out,"f3","$0 +21600-$1 ");
    equation(out,"f4","?f3 /2");
    equation(out,"f5","$1 +$1 -21600");
    equation(out,"f6","?f5 +$1 -$0 ");
    equation(out,"f7","?f6 /2");
    equation(out,"f8","(21600+$0 )/2");
    equation(out,"f9","?f4 *sqrt(1-($2 /21600)*($2 /21600))");
    equation(out,"f10","?f4 +?f9 ");
    equation(out,"f11","?f10 +$1 -21600");
    equation(out,"f12","?f7 +?f9 ");
    equation(out,"f13","?f11 +21600-$0 ");
    equation(out,"f14","?f5 -$0 ");
    equation(out,"f15","?f14 /2");
    equation(out,"f16","(?f4 +?f7 )/2");
    equation(out,"f17","$0 +$1 -21600");
    equation(out,"f18","?f17 /2");
    equation(out,"f19","?f16 -?f18 ");
    equation(out,"f20","21600");
    equation(out,"f21","21600");
    equation(out,"f22","-21600");
    equation(out,"f23","?f16 -?f4 ");
    equation(out,"f24","21600*sqrt(1-(?f23 /?f4 )*(?f23 /?f4 ))");
    equation(out,"f25","?f8 +128");
    equation(out,"f26","?f5 /2");
    equation(out,"f27","?f5 -128");
    equation(out,"f28","$0 +?f16 -?f11 ");
    equation(out,"f29","21600-$0 ");
    equation(out,"f30","?f29 /2");
    equation(out,"f31","21600*21600");
    equation(out,"f32","$2 *$2 ");
    equation(out,"f33","?f31 -?f32 ");
    equation(out,"f34","sqrt(?f33 )");
    equation(out,"f35","?f34 +21600");
    equation(out,"f36","21600*21600/?f35 ");
    equation(out,"f37","?f36 +64");
    equation(out,"f38","$0 /2");
    equation(out,"f39","21600*sqrt(1-(?f30 /?f38 )*(?f30 /?f38 ))");
    equation(out,"f40","?f39 -64");
    equation(out,"f41","?f4 /2");
    equation(out,"f42","$1 -?f41 ");
    equation(out,"f43","21600*2195/16384");
    equation(out,"f44","21600*14189/16384");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-maximum", "?f27");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "10800");
    out.xml.addAttribute("draw:handle-range-x-minimum", "?f37");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$1 top");
    out.xml.addAttribute("draw:handle-range-x-maximum", "?f20");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "10800");
    out.xml.addAttribute("draw:handle-range-x-minimum", "?f25");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "21600 $2");
    out.xml.addAttribute("draw:handle-range-x-maximum", "21600");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "?f40");
    out.xml.addAttribute("draw:handle-range-x-minimum", "3375");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processCurvedDownArrow(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "?f17 0 ?f16 21600 ?f12 ?f2 ?f8 21600 ?f14 ?f2");
    processModifiers(o, out, QList<int>() << 12960<< 19440<< 14400);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "W 0 0 ?f3 ?f21 0 21600 ?f4 0 0 0 ?f3 ?f21 ?f4 0 ?f17 ?f24 A ?f15 0 ?f1 ?f21 ?f17 ?f24 ?f15 21600 Z N V ?f15 0 ?f1 ?f21 ?f7 0 ?f13 ?f2 L ?f14 ?f2 ?f8 21600 ?f12 ?f2 A 0 0 ?f3 ?f21 ?f11 ?f2 ?f17 ?f24 0 0 ?f3 ?f21 ?f17 ?f24 ?f4 0 Z N");
    out.xml.addAttribute("draw:type", "mso-spt105");
    out.xml.addAttribute("draw:text-areas", "?f43 ?f45 ?f44 ?f46");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","$1 ");
    equation(out,"f2","$2 ");
    equation(out,"f3","$0 +21600-$1 ");
    equation(out,"f4","?f3 /2");
    equation(out,"f5","$1 +$1 -21600");
    equation(out,"f6","?f5 +$1 -$0 ");
    equation(out,"f7","?f6 /2");
    equation(out,"f8","(21600+$0 )/2");
    equation(out,"f9","21600-$2 ");
    equation(out,"f10","?f4 *sqrt(1-(?f9 /21600)*(?f9 /21600))");
    equation(out,"f11","?f4 +?f10 ");
    equation(out,"f12","?f11 +$1 -21600");
    equation(out,"f13","?f7 +?f10 ");
    equation(out,"f14","?f12 +21600-$0 ");
    equation(out,"f15","?f5 -$0 ");
    equation(out,"f16","?f15 /2");
    equation(out,"f17","(?f4 +?f7 )/2");
    equation(out,"f18","$0 +$1 -21600");
    equation(out,"f19","?f18 /2");
    equation(out,"f20","?f17 -?f19 ");
    equation(out,"f21","21600*2");
    equation(out,"f22","?f17 -?f4 ");
    equation(out,"f23","21600*sqrt(1-(?f22 /?f4 )*(?f22 /?f4 ))");
    equation(out,"f24","21600-?f23 ");
    equation(out,"f25","?f8 +128");
    equation(out,"f26","?f5 /2");
    equation(out,"f27","?f5 -128");
    equation(out,"f28","$0 +?f17 -?f12 ");
    equation(out,"f29","21600*sqrt(1-(?f20 /?f4 )*(?f20 /?f4 ))");
    equation(out,"f30","21600-$0 ");
    equation(out,"f31","?f30 /2");
    equation(out,"f32","21600*21600");
    equation(out,"f33","?f9 *?f9 ");
    equation(out,"f34","?f32 -?f33 ");
    equation(out,"f35","sqrt(?f34 )");
    equation(out,"f36","?f35 +21600");
    equation(out,"f37","21600*21600/?f36 ");
    equation(out,"f38","?f37 +64");
    equation(out,"f39","$0 /2");
    equation(out,"f40","21600*sqrt(1-(?f31 /?f39 )*(?f31 /?f39 ))");
    equation(out,"f41","21600-?f40 ");
    equation(out,"f42","?f41 +64");
    equation(out,"f43","?f4 /2");
    equation(out,"f44","$1 -?f43 ");
    equation(out,"f45","21600*2195/16384");
    equation(out,"f46","21600*14189/16384");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 21600");
    out.xml.addAttribute("draw:handle-range-x-maximum", "?f27");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "10800");
    out.xml.addAttribute("draw:handle-range-x-minimum", "?f38");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$1 21600");
    out.xml.addAttribute("draw:handle-range-x-maximum", "21600");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "10800");
    out.xml.addAttribute("draw:handle-range-x-minimum", "?f25");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "21600 $2");
    out.xml.addAttribute("draw:handle-range-x-maximum", "21600");
    out.xml.addAttribute("draw:handle-range-y-minimum", "?f42");
    out.xml.addAttribute("draw:handle-range-y-maximum", "21600");
    out.xml.addAttribute("draw:handle-range-x-minimum", "3375");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processStripedRightArrow(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 16200<< 5400);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 3375 ?f0 L ?f1 ?f0 ?f1 0 21600 10800 ?f1 21600 ?f1 ?f2 3375 ?f2 Z N M 0 ?f0 L 675 ?f0 675 ?f2 0 ?f2 Z N M 1350 ?f0 L 2700 ?f0 2700 ?f2 1350 ?f2 Z N");
    out.xml.addAttribute("draw:type", "striped-right-arrow");
    out.xml.addAttribute("draw:text-areas", "3375 ?f0 ?f5 ?f2");
    setShapeMirroring(o, out);
    equation(out,"f0","$1 ");
    equation(out,"f1","$0 ");
    equation(out,"f2","21600-$1 ");
    equation(out,"f3","21600-?f1 ");
    equation(out,"f4","?f3 *?f0 /10800");
    equation(out,"f5","?f1 +?f4 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 $1");
    out.xml.addAttribute("draw:handle-range-x-maximum", "21600");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "10800");
    out.xml.addAttribute("draw:handle-range-x-minimum", "3375");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processNotchedRightArrow(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 16200<< 5400);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 ?f1 L ?f0 ?f1 ?f0 0 21600 10800 ?f0 21600 ?f0 ?f2 0 ?f2 ?f5 10800 0 ?f1 Z N");
    out.xml.addAttribute("draw:type", "notched-right-arrow");
    out.xml.addAttribute("draw:text-areas", "0 0 21600 21600");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","$1 ");
    equation(out,"f2","21600-$1 ");
    equation(out,"f3","21600-$0 ");
    equation(out,"f4","10800-$1 ");
    equation(out,"f5","?f3 *?f4 /10800");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 $1");
    out.xml.addAttribute("draw:handle-range-x-maximum", "21600");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "10800");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processHomePlate(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 16200);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L ?f0 0 21600 10800 ?f0 21600 0 21600 Z N");
    out.xml.addAttribute("draw:type", "pentagon-right");
    out.xml.addAttribute("draw:text-areas", "0 0 21600 21600");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-maximum", "21600");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processChevron(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 16200);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L ?f0 0 21600 10800 ?f0 21600 0 21600 ?f1 10800 Z N");
    out.xml.addAttribute("draw:type", "chevron");
    out.xml.addAttribute("draw:text-areas", "0 0 21600 21600");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","21600-?f0 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-maximum", "21600");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processRightArrowCallout(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 14400<< 5400<< 18000<< 8100);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L ?f0 0 ?f0 ?f3 ?f2 ?f3 ?f2 ?f1 21600 10800 ?f2 ?f4 ?f2 ?f5 ?f0 ?f5 ?f0 21600 0 21600 Z N");
    out.xml.addAttribute("draw:type", "right-arrow-callout");
    out.xml.addAttribute("draw:text-areas", "0 0 ?f0 21600");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","$1 ");
    equation(out,"f2","$2 ");
    equation(out,"f3","$3 ");
    equation(out,"f4","21600-?f1 ");
    equation(out,"f5","21600-?f3 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-maximum", "$2");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$2 $3");
    out.xml.addAttribute("draw:handle-range-x-maximum", "21600");
    out.xml.addAttribute("draw:handle-range-y-minimum", "$1");
    out.xml.addAttribute("draw:handle-range-y-maximum", "10800");
    out.xml.addAttribute("draw:handle-range-x-minimum", "$0");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "right $1");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "$3");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processLeftArrowCallout(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 7200<< 5400<< 3600<< 8100);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M ?f0 0 L 21600 0 21600 21600 ?f0 21600 ?f0 ?f5 ?f2 ?f5 ?f2 ?f4 0 10800 ?f2 ?f1 ?f2 ?f3 ?f0 ?f3 Z N");
    out.xml.addAttribute("draw:type", "left-arrow-callout");
    out.xml.addAttribute("draw:text-areas", "?f0 0 21600 21600");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","$1 ");
    equation(out,"f2","$2 ");
    equation(out,"f3","$3 ");
    equation(out,"f4","21600-?f1 ");
    equation(out,"f5","21600-?f3 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-maximum", "21600");
    out.xml.addAttribute("draw:handle-range-x-minimum", "$2");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$2 $3");
    out.xml.addAttribute("draw:handle-range-x-maximum", "$0");
    out.xml.addAttribute("draw:handle-range-y-minimum", "$1");
    out.xml.addAttribute("draw:handle-range-y-maximum", "10800");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "left $1");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "$3");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processUpArrowCallout(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 7200<< 5400<< 3600<< 8100);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 21600 ?f0 L 21600 21600 0 21600 0 ?f0 ?f3 ?f0 ?f3 ?f2 ?f1 ?f2 10800 0 ?f4 ?f2 ?f5 ?f2 ?f5 ?f0 Z N");
    out.xml.addAttribute("draw:type", "up-arrow-callout");
    out.xml.addAttribute("draw:text-areas", "0 ?f0 21600 21600");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","$1 ");
    equation(out,"f2","$2 ");
    equation(out,"f3","$3 ");
    equation(out,"f4","21600-?f1 ");
    equation(out,"f5","21600-?f3 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "left $0");
    out.xml.addAttribute("draw:handle-range-y-minimum", "$2");
    out.xml.addAttribute("draw:handle-range-y-maximum", "21600");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$3 $2");
    out.xml.addAttribute("draw:handle-range-x-maximum", "10800");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "$0");
    out.xml.addAttribute("draw:handle-range-x-minimum", "$1");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$1 top");
    out.xml.addAttribute("draw:handle-range-x-maximum", "$3");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processDownArrowCallout(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 14400<< 5400<< 18000<< 8100);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 ?f0 L 0 0 21600 0 21600 ?f0 ?f5 ?f0 ?f5 ?f2 ?f4 ?f2 10800 21600 ?f1 ?f2 ?f3 ?f2 ?f3 ?f0 Z N");
    out.xml.addAttribute("draw:type", "down-arrow-callout");
    out.xml.addAttribute("draw:text-areas", "0 0 21600 ?f0");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","$1 ");
    equation(out,"f2","$2 ");
    equation(out,"f3","$3 ");
    equation(out,"f4","21600-?f1 ");
    equation(out,"f5","21600-?f3 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "left $0");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "$2");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$3 $2");
    out.xml.addAttribute("draw:handle-range-x-maximum", "10800");
    out.xml.addAttribute("draw:handle-range-y-minimum", "$0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "21600");
    out.xml.addAttribute("draw:handle-range-x-minimum", "$1");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$1 bottom");
    out.xml.addAttribute("draw:handle-range-x-maximum", "$3");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processLeftRightArrowCallout(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 5400<< 5500<< 2700<< 8100);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M ?f0 0 L ?f4 0 ?f4 ?f3 ?f6 ?f3 ?f6 ?f1 21600 10800 ?f6 ?f5 ?f6 ?f7 ?f4 ?f7 ?f4 21600 ?f0 21600 ?f0 ?f7 ?f2 ?f7 ?f2 ?f5 0 10800 ?f2 ?f1 ?f2 ?f3 ?f0 ?f3 Z N");
    out.xml.addAttribute("draw:type", "left-right-arrow-callout");
    out.xml.addAttribute("draw:text-areas", "?f0 0 ?f4 21600");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","$1 ");
    equation(out,"f2","$2 ");
    equation(out,"f3","$3 ");
    equation(out,"f4","21600-?f0 ");
    equation(out,"f5","21600-?f1 ");
    equation(out,"f6","21600-?f2 ");
    equation(out,"f7","21600-?f3 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-maximum", "10800");
    out.xml.addAttribute("draw:handle-range-x-minimum", "$2");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$2 $3");
    out.xml.addAttribute("draw:handle-range-x-maximum", "$0");
    out.xml.addAttribute("draw:handle-range-y-minimum", "$1");
    out.xml.addAttribute("draw:handle-range-y-maximum", "10800");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "left $1");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "$3");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processUpDownArrowCallout(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 5400<< 5500<< 2700<< 8100);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 ?f0 L 0 ?f4 ?f3 ?f4 ?f3 ?f6 ?f1 ?f6 10800 21600 ?f5 ?f6 ?f7 ?f6 ?f7 ?f4 21600 ?f4 21600 ?f0 ?f7 ?f0 ?f7 ?f2 ?f5 ?f2 10800 0 ?f1 ?f2 ?f3 ?f2 ?f3 ?f0 Z N");
    out.xml.addAttribute("draw:type", "up-down-arrow-callout");
    out.xml.addAttribute("draw:text-areas", "0 ?f0 21600 ?f4");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","$1 ");
    equation(out,"f2","$2 ");
    equation(out,"f3","$3 ");
    equation(out,"f4","21600-?f0 ");
    equation(out,"f5","21600-?f1 ");
    equation(out,"f6","21600-?f2 ");
    equation(out,"f7","21600-?f3 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "left $0");
    out.xml.addAttribute("draw:handle-range-y-minimum", "$2");
    out.xml.addAttribute("draw:handle-range-y-maximum", "10800");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$3 $2");
    out.xml.addAttribute("draw:handle-range-x-maximum", "10800");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "$0");
    out.xml.addAttribute("draw:handle-range-x-minimum", "$1");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$1 top");
    out.xml.addAttribute("draw:handle-range-x-maximum", "$3");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processQuadArrowCallout(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 5400<< 8100<< 2700<< 9400);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M ?f0 ?f0 L ?f3 ?f0 ?f3 ?f2 ?f1 ?f2 10800 0 ?f5 ?f2 ?f7 ?f2 ?f7 ?f0 ?f4 ?f0 ?f4 ?f3 ?f6 ?f3 ?f6 ?f1 21600 10800 ?f6 ?f5 ?f6 ?f7 ?f4 ?f7 ?f4 ?f4 ?f7 ?f4 ?f7 ?f6 ?f5 ?f6 10800 21600 ?f1 ?f6 ?f3 ?f6 ?f3 ?f4 ?f0 ?f4 ?f0 ?f7 ?f2 ?f7 ?f2 ?f5 0 10800 ?f2 ?f1 ?f2 ?f3 ?f0 ?f3 Z N");
    out.xml.addAttribute("draw:type", "quad-arrow-callout");
    out.xml.addAttribute("draw:text-areas", "?f0 ?f0 ?f4 ?f4");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","$1 ");
    equation(out,"f2","$2 ");
    equation(out,"f3","$3 ");
    equation(out,"f4","21600-?f0 ");
    equation(out,"f5","21600-?f1 ");
    equation(out,"f6","21600-?f2 ");
    equation(out,"f7","21600-?f3 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "left $0");
    out.xml.addAttribute("draw:handle-range-y-minimum", "$2");
    out.xml.addAttribute("draw:handle-range-y-maximum", "$1");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$1 top");
    out.xml.addAttribute("draw:handle-range-x-maximum", "10800");
    out.xml.addAttribute("draw:handle-range-x-minimum", "$0");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$3 $2");
    out.xml.addAttribute("draw:handle-range-x-maximum", "10800");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "$0");
    out.xml.addAttribute("draw:handle-range-x-minimum", "$1");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processCircularArrow(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    // custom way for processing modifieres as this shape
    // return the AdjustValue, AdjustValue2 16-bit shifted
    // see bug 274684, file mp03_cycle_default.ppt
    const AdjustValue* val1 = get<AdjustValue>(o);
    const Adjust2Value* val2 = get<Adjust2Value>(o);
    const Adjust3Value* val3 = get<Adjust3Value>(o);
    QString modifiers = QString::number(val1 ? val1->adjustvalue >> 16 : 180) +
            QString(" %1").arg(val2 ? val2->adjust2value >> 16 : 0) +
            QString(" %1").arg(val3 ? val3->adjust3value : 5500);
    out.xml.addAttribute("draw:modifiers", modifiers);

    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "B ?f3 ?f3 ?f20 ?f20 ?f19 ?f18 ?f17 ?f16 W 0 0 21600 21600 ?f9 ?f8 ?f11 ?f10 L ?f24 ?f23 ?f47 ?f46 ?f29 ?f28 Z N");
    out.xml.addAttribute("draw:type", "circular-arrow");
    out.xml.addAttribute("draw:text-areas", "0 0 21600 21600");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","$1 ");
    equation(out,"f2","$2 ");
    equation(out,"f3","10800+$2 ");
    equation(out,"f4","10800*sin($0 *(pi/180))");
    equation(out,"f5","10800*cos($0 *(pi/180))");
    equation(out,"f6","10800*sin($1 *(pi/180))");
    equation(out,"f7","10800*cos($1 *(pi/180))");
    equation(out,"f8","?f4 +10800");
    equation(out,"f9","?f5 +10800");
    equation(out,"f10","?f6 +10800");
    equation(out,"f11","?f7 +10800");
    equation(out,"f12","?f3 *sin($0 *(pi/180))");
    equation(out,"f13","?f3 *cos($0 *(pi/180))");
    equation(out,"f14","?f3 *sin($1 *(pi/180))");
    equation(out,"f15","?f3 *cos($1 *(pi/180))");
    equation(out,"f16","?f12 +10800");
    equation(out,"f17","?f13 +10800");
    equation(out,"f18","?f14 +10800");
    equation(out,"f19","?f15 +10800");
    equation(out,"f20","21600-?f3 ");
    equation(out,"f21","13500*sin($1 *(pi/180))");
    equation(out,"f22","13500*cos($1 *(pi/180))");
    equation(out,"f23","?f21 +10800");
    equation(out,"f24","?f22 +10800");
    equation(out,"f25","$2 -2700");
    equation(out,"f26","?f25 *sin($1 *(pi/180))");
    equation(out,"f27","?f25 *cos($1 *(pi/180))");
    equation(out,"f28","?f26 +10800");
    equation(out,"f29","?f27 +10800");
    equation(out,"f30","?f29 -?f24 ");
    equation(out,"f31","?f29 -?f24 ");
    equation(out,"f32","?f30 *?f31 ");
    equation(out,"f33","?f28 -?f23 ");
    equation(out,"f34","?f28 -?f23 ");
    equation(out,"f35","?f33 *?f34 ");
    equation(out,"f36","?f32 +?f35 ");
    equation(out,"f37","sqrt(?f36 )");
    equation(out,"f38","$1 +45");
    equation(out,"f39","?f37 *sin(?f38 *(pi/180))");
    equation(out,"f40","$1 +45");
    equation(out,"f41","?f37 *cos(?f40 *(pi/180))");
    equation(out,"f42","45");
    equation(out,"f43","?f39 *sin(?f42 *(pi/180))");
    equation(out,"f44","45");
    equation(out,"f45","?f41 *sin(?f44 *(pi/180))");
    equation(out,"f46","?f28 +?f43 ");
    equation(out,"f47","?f29 +?f45 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "10800 $0");
    out.xml.addAttribute("draw:handle-radius-range-minimum", "10800");
    out.xml.addAttribute("draw:handle-radius-range-maximum", "10800");
    out.xml.addAttribute("draw:handle-polar", "10800 10800");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$2 $1");
    out.xml.addAttribute("draw:handle-radius-range-minimum", "0");
    out.xml.addAttribute("draw:handle-radius-range-maximum", "10800");
    out.xml.addAttribute("draw:handle-polar", "10800 10800");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processFlowChartProcess(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 0 0 10800 10800 21600 21600 10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L 21600 0 21600 21600 0 21600 0 0 Z N");
    out.xml.addAttribute("draw:type", "flowchart-process");
    setShapeMirroring(o, out);
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processFlowChartAlternateProcess(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 0 0 10800 10800 21600 21600 10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 ?f2 Y ?f0 0 L ?f1 0 X 21600 ?f2 L 21600 ?f3 Y ?f1 21600 L ?f0 21600 X 0 ?f3 Z N");
    out.xml.addAttribute("draw:type", "flowchart-alternate-process");
    out.xml.addAttribute("draw:text-areas", "?f4 ?f6 ?f5 ?f7");
    setShapeMirroring(o, out);
    equation(out,"f0","left+2540");
    equation(out,"f1","right-2540");
    equation(out,"f2","top+2540");
    equation(out,"f3","bottom-2540");
    equation(out,"f4","left+800");
    equation(out,"f5","right-800");
    equation(out,"f6","top+800");
    equation(out,"f7","bottom-800");
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processFlowChartDecision(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 0 0 10800 10800 21600 21600 10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 10800 L 10800 0 21600 10800 10800 21600 0 10800 Z N");
    out.xml.addAttribute("draw:type", "flowchart-decision");
    out.xml.addAttribute("draw:text-areas", "5400 5400 16200 16200");
    setShapeMirroring(o, out);
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processFlowChartInputOutput(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "12960 0 10800 0 2160 10800 8600 21600 10800 21600 19400 10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 4230 0 L 21600 0 17370 21600 0 21600 4230 0 Z N");
    out.xml.addAttribute("draw:type", "flowchart-data");
    out.xml.addAttribute("draw:text-areas", "4230 0 17370 21600");
    setShapeMirroring(o, out);
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processFlowChartPredefinedProcess(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L 21600 0 21600 21600 0 21600 Z N M 2540 0 L 2540 21600 N M 19060 0 L 19060 21600 N");
    out.xml.addAttribute("draw:type", "flowchart-predefined-process");
    out.xml.addAttribute("draw:text-areas", "2540 0 19060 21600");
    setShapeMirroring(o, out);
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processFlowChartInternalStorage(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L 21600 0 21600 21600 0 21600 Z N M 4230 0 L 4230 21600 N M 0 4230 L 21600 4230 N");
    out.xml.addAttribute("draw:type", "flowchart-internal-storage");
    out.xml.addAttribute("draw:text-areas", "4230 4230 21600 21600");
    setShapeMirroring(o, out);
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processFlowChartDocument(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 0 0 10800 10800 20320 21600 10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L 21600 0 21600 17360 C 13050 17220 13340 20770 5620 21600 2860 21100 1850 20700 0 20120 Z N");
    out.xml.addAttribute("draw:type", "flowchart-document");
    out.xml.addAttribute("draw:text-areas", "0 0 21600 17360");
    setShapeMirroring(o, out);
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processFlowChartMultidocument(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 0 0 10800 10800 19890 21600 10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 3600 L 1500 3600 1500 1800 3000 1800 3000 0 21600 0 21600 14409 20100 14409 20100 16209 18600 16209 18600 18009 C 11610 17893 11472 20839 4833 21528 2450 21113 1591 20781 0 20300 Z N M 1500 3600 F L 18600 3600 18600 16209 N M 3000 1800 F L 20100 1800 20100 14409 N");
    out.xml.addAttribute("draw:type", "flowchart-multidocument");
    out.xml.addAttribute("draw:text-areas", "0 3600 18600 18009");
    setShapeMirroring(o, out);
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processFlowChartTerminator(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 0 0 10800 10800 21600 21600 10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 3470 21600 X 0 10800 3470 0 L 18130 0 X 21600 10800 18130 21600 Z N");
    out.xml.addAttribute("draw:type", "flowchart-terminator");
    out.xml.addAttribute("draw:text-areas", "1060 3180 20540 18420");
    setShapeMirroring(o, out);
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processFlowChartPreparation(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 0 0 10800 10800 21600 21600 10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 4350 0 L 17250 0 21600 10800 17250 21600 4350 21600 0 10800 4350 0 Z N");
    out.xml.addAttribute("draw:type", "flowchart-preparation");
    out.xml.addAttribute("draw:text-areas", "4350 0 17250 21600");
    setShapeMirroring(o, out);
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processFlowChartManualInput(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 2150 0 10800 10800 19890 21600 10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 4300 L 21600 0 21600 21600 0 21600 0 4300 Z N");
    out.xml.addAttribute("draw:type", "flowchart-manual-input");
    out.xml.addAttribute("draw:text-areas", "0 4300 21600 21600");
    setShapeMirroring(o, out);
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processFlowChartManualOperation(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 0 2160 10800 10800 21600 19440 10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L 21600 0 17250 21600 4350 21600 0 0 Z N");
    out.xml.addAttribute("draw:type", "flowchart-manual-operation");
    out.xml.addAttribute("draw:text-areas", "4350 0 17250 21600");
    setShapeMirroring(o, out);
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processFlowChartConnector(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 0 3163 3163 0 10800 3163 18437 10800 21600 18437 18437 21600 10800 18437 3163");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "U 10800 10800 10800 10800 0 360 Z N");
    out.xml.addAttribute("draw:type", "flowchart-connector");
    out.xml.addAttribute("draw:text-areas", "3180 3180 18420 18420");
    setShapeMirroring(o, out);
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processFlowChartOffpageConnector(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 0 0 10800 10800 21600 21600 10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L 21600 0 21600 17150 10800 21600 0 17150 0 0 Z N");
    out.xml.addAttribute("draw:type", "flowchart-off-page-connector");
    out.xml.addAttribute("draw:text-areas", "0 0 21600 17150");
    setShapeMirroring(o, out);
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processFlowChartPunchedCard(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 0 0 10800 10800 21600 21600 10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 4300 0 L 21600 0 21600 21600 0 21600 0 4300 4300 0 Z N");
    out.xml.addAttribute("draw:type", "flowchart-card");
    out.xml.addAttribute("draw:text-areas", "0 4300 21600 21600");
    setShapeMirroring(o, out);
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processFlowChartPunchedTape(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 2020 0 10800 10800 19320 21600 10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 2230 C 820 3990 3410 3980 5370 4360 7430 4030 10110 3890 10690 2270 11440 300 14200 160 16150 0 18670 170 20690 390 21600 2230 L 21600 19420 C 20640 17510 18320 17490 16140 17240 14710 17370 11310 17510 10770 19430 10150 21150 7380 21290 5290 21600 3220 21250 610 21130 0 19420 Z N");
    out.xml.addAttribute("draw:type", "flowchart-punched-tape");
    out.xml.addAttribute("draw:text-areas", "0 4360 21600 17240");
    setShapeMirroring(o, out);
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processFlowChartSummingJunction(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 0 3163 3163 0 10800 3163 18437 10800 21600 18437 18437 21600 10800 18437 3163");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "U 10800 10800 10800 10800 0 360 Z N M 3100 3100 L 18500 18500 N M 3100 18500 L 18500 3100 N");
    out.xml.addAttribute("draw:type", "flowchart-summing-junction");
    out.xml.addAttribute("draw:text-areas", "3100 3100 18500 18500");
    setShapeMirroring(o, out);
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processFlowChartOr(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 0 3163 3163 0 10800 3163 18437 10800 21600 18437 18437 21600 10800 18437 3163");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "U 10800 10800 10800 10800 0 360 Z N M 0 10800 L 21600 10800 N M 10800 0 L 10800 21600 N");
    out.xml.addAttribute("draw:type", "flowchart-or");
    out.xml.addAttribute("draw:text-areas", "3100 3100 18500 18500");
    setShapeMirroring(o, out);
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processFlowChartCollate(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 0 10800 10800 10800 21600");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L 21600 21600 0 21600 21600 0 0 0 Z N");
    out.xml.addAttribute("draw:type", "flowchart-collate");
    out.xml.addAttribute("draw:text-areas", "5400 5400 16200 16200");
    setShapeMirroring(o, out);
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processFlowChartSort(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 10800 L 10800 0 21600 10800 10800 21600 Z N M 0 10800 L 21600 10800 N");
    out.xml.addAttribute("draw:type", "flowchart-sort");
    out.xml.addAttribute("draw:text-areas", "5400 5400 16200 16200");
    setShapeMirroring(o, out);
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processFlowChartExtract(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 0 5400 10800 10800 21600 16200 10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 10800 0 L 21600 21600 0 21600 10800 0 Z N");
    out.xml.addAttribute("draw:type", "flowchart-extract");
    out.xml.addAttribute("draw:text-areas", "5400 10800 16200 21600");
    setShapeMirroring(o, out);
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processFlowChartMerge(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 0 5400 10800 10800 21600 16200 10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L 21600 0 10800 21600 0 0 Z N");
    out.xml.addAttribute("draw:type", "flowchart-merge");
    out.xml.addAttribute("draw:text-areas", "5400 0 16200 10800");
    setShapeMirroring(o, out);
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processFlowChartOnlineStorage(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 0 0 10800 10800 21600 18000 10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 3600 21600 X 0 10800 3600 0 L 21600 0 X 18000 10800 21600 21600 Z N");
    out.xml.addAttribute("draw:type", "flowchart-stored-data");
    out.xml.addAttribute("draw:text-areas", "3600 0 18000 21600");
    setShapeMirroring(o, out);
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processFlowChartDelay(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 0 0 10800 10800 21600 21600 10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 10800 0 X 21600 10800 10800 21600 L 0 21600 0 0 Z N");
    out.xml.addAttribute("draw:type", "flowchart-delay");
    out.xml.addAttribute("draw:text-areas", "0 3100 18500 18500");
    setShapeMirroring(o, out);
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processFlowChartMagneticTape(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 0 0 10800 10800 21600 21600 10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 20980 18150 L 20980 21600 10670 21600 C 4770 21540 0 16720 0 10800 0 4840 4840 0 10800 0 16740 0 21600 4840 21600 10800 21600 13520 20550 16160 18670 18170 Z N");
    out.xml.addAttribute("draw:type", "flowchart-sequential-access");
    out.xml.addAttribute("draw:text-areas", "3100 3100 18500 18500");
    setShapeMirroring(o, out);
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processFlowChartMagneticDisk(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 6800 10800 0 0 10800 10800 21600 21600 10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 3400 Y 10800 0 21600 3400 L 21600 18200 Y 10800 21600 0 18200 Z N M 0 3400 Y 10800 6800 21600 3400 N");
    out.xml.addAttribute("draw:type", "flowchart-magnetic-disk");
    out.xml.addAttribute("draw:text-areas", "0 6800 21600 18200");
    setShapeMirroring(o, out);
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processFlowChartMagneticDrum(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 0 0 10800 10800 21600 14800 10800 21600 10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 18200 0 X 21600 10800 18200 21600 L 3400 21600 X 0 10800 3400 0 Z N M 18200 0 X 14800 10800 18200 21600 N");
    out.xml.addAttribute("draw:type", "flowchart-direct-access-storage");
    out.xml.addAttribute("draw:text-areas", "3400 0 14800 21600");
    setShapeMirroring(o, out);
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processFlowChartDisplay(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 0 0 10800 10800 21600 21600 10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 3600 0 L 17800 0 X 21600 10800 17800 21600 L 3600 21600 0 10800 Z N");
    out.xml.addAttribute("draw:type", "flowchart-display");
    out.xml.addAttribute("draw:text-areas", "3600 0 17800 21600");
    setShapeMirroring(o, out);
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processIrregularSeal1(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "14623 106 106 8718 8590 21600 21600 13393");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 10901 5905 L 8458 2399 7417 6425 476 2399 4732 7722 106 8718 3828 11880 243 14689 5772 14041 4868 17719 7819 15730 8590 21600 10637 15038 13349 19840 14125 14561 18248 18195 16938 13044 21600 13393 17710 10579 21198 8242 16806 7417 18482 4560 14257 5429 14623 106 10901 5905 Z N");
    out.xml.addAttribute("draw:type", "mso-spt71");
    out.xml.addAttribute("draw:text-areas", "4680 6570 16140 13280");
    setShapeMirroring(o, out);
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processIrregularSeal2(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "9722 1887 0 12875 11614 18844 21600 6646");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 11464 4340 L 9722 1887 8548 6383 4503 3626 5373 7816 1174 8270 3934 11592 0 12875 3329 15372 1283 17824 4804 18239 4918 21600 7525 18125 8698 19712 9871 17371 11614 18844 12178 15937 14943 17371 14640 14348 18878 15632 16382 12311 18270 11292 16986 9404 21600 6646 16382 6533 18005 3172 14524 5778 14789 0 11464 4340 Z N");
    out.xml.addAttribute("draw:type", "bang");
    out.xml.addAttribute("draw:text-areas", "5400 6570 14160 15290");
    setShapeMirroring(o, out);
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processSeal4(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 8100);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 10800 L ?f4 ?f4 10800 0 ?f3 ?f4 21600 10800 ?f3 ?f3 10800 21600 ?f4 ?f3 0 10800 Z N");
    out.xml.addAttribute("draw:type", "star4");
    out.xml.addAttribute("draw:text-areas", "?f4 ?f4 ?f3 ?f3");
    setShapeMirroring(o, out);
    equation(out,"f0","7600");
    equation(out,"f1","?f0 *$0 /10800");
    equation(out,"f2","?f0 -?f1 ");
    equation(out,"f3","10800+?f2 ");
    equation(out,"f4","10800-?f2 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 10800");
    out.xml.addAttribute("draw:handle-range-x-maximum", "10800");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processStar(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 10797 0 L 8278 8256 0 8256 6722 13405 4198 21600 10797 16580 17401 21600 14878 13405 21600 8256 13321 8256 10797 0 Z N");
    out.xml.addAttribute("draw:type", "star5");
    out.xml.addAttribute("draw:text-areas", "6722 8256 14878 15460");
    setShapeMirroring(o, out);
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processSeal8(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 2500);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M ?f5 ?f6 L ?f11 ?f12 ?f17 ?f18 ?f23 ?f24 ?f29 ?f30 ?f35 ?f36 ?f41 ?f42 ?f47 ?f48 ?f53 ?f54 ?f59 ?f60 ?f65 ?f66 ?f71 ?f72 ?f77 ?f78 ?f83 ?f84 ?f89 ?f90 ?f95 ?f96 ?f5 ?f6 Z N");
    out.xml.addAttribute("draw:type", "star8");
    out.xml.addAttribute("draw:text-areas", "?f1 ?f2 ?f3 ?f4");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","(cos(315*(pi/180))*(?f0 -10800)+sin(315*(pi/180))*(10800-10800))+10800");
    equation(out,"f2","-(sin(315*(pi/180))*(?f0 -10800)-cos(315*(pi/180))*(10800-10800))+10800");
    equation(out,"f3","(cos(135*(pi/180))*(?f0 -10800)+sin(135*(pi/180))*(10800-10800))+10800");
    equation(out,"f4","-(sin(135*(pi/180))*(?f0 -10800)-cos(135*(pi/180))*(10800-10800))+10800");
    equation(out,"f5","(cos(0*(pi/180))*(0-10800)+sin(0*(pi/180))*(10800-10800))+10800");
    equation(out,"f6","-(sin(0*(pi/180))*(0-10800)-cos(0*(pi/180))*(10800-10800))+10800");
    equation(out,"f7","(cos(7*(pi/180))*(?f0 -10800)+sin(7*(pi/180))*(10800-10800))+10800");
    equation(out,"f8","-(sin(7*(pi/180))*(?f0 -10800)-cos(7*(pi/180))*(10800-10800))+10800");
    equation(out,"f9","(cos(15*(pi/180))*(0-10800)+sin(15*(pi/180))*(10800-10800))+10800");
    equation(out,"f10","-(sin(15*(pi/180))*(0-10800)-cos(15*(pi/180))*(10800-10800))+10800");
    equation(out,"f11","(cos(22*(pi/180))*(?f0 -10800)+sin(22*(pi/180))*(10800-10800))+10800");
    equation(out,"f12","-(sin(22*(pi/180))*(?f0 -10800)-cos(22*(pi/180))*(10800-10800))+10800");
    equation(out,"f13","(cos(30*(pi/180))*(0-10800)+sin(30*(pi/180))*(10800-10800))+10800");
    equation(out,"f14","-(sin(30*(pi/180))*(0-10800)-cos(30*(pi/180))*(10800-10800))+10800");
    equation(out,"f15","(cos(37*(pi/180))*(?f0 -10800)+sin(37*(pi/180))*(10800-10800))+10800");
    equation(out,"f16","-(sin(37*(pi/180))*(?f0 -10800)-cos(37*(pi/180))*(10800-10800))+10800");
    equation(out,"f17","(cos(45*(pi/180))*(0-10800)+sin(45*(pi/180))*(10800-10800))+10800");
    equation(out,"f18","-(sin(45*(pi/180))*(0-10800)-cos(45*(pi/180))*(10800-10800))+10800");
    equation(out,"f19","(cos(52*(pi/180))*(?f0 -10800)+sin(52*(pi/180))*(10800-10800))+10800");
    equation(out,"f20","-(sin(52*(pi/180))*(?f0 -10800)-cos(52*(pi/180))*(10800-10800))+10800");
    equation(out,"f21","(cos(60*(pi/180))*(0-10800)+sin(60*(pi/180))*(10800-10800))+10800");
    equation(out,"f22","-(sin(60*(pi/180))*(0-10800)-cos(60*(pi/180))*(10800-10800))+10800");
    equation(out,"f23","(cos(67*(pi/180))*(?f0 -10800)+sin(67*(pi/180))*(10800-10800))+10800");
    equation(out,"f24","-(sin(67*(pi/180))*(?f0 -10800)-cos(67*(pi/180))*(10800-10800))+10800");
    equation(out,"f25","(cos(75*(pi/180))*(0-10800)+sin(75*(pi/180))*(10800-10800))+10800");
    equation(out,"f26","-(sin(75*(pi/180))*(0-10800)-cos(75*(pi/180))*(10800-10800))+10800");
    equation(out,"f27","(cos(82*(pi/180))*(?f0 -10800)+sin(82*(pi/180))*(10800-10800))+10800");
    equation(out,"f28","-(sin(82*(pi/180))*(?f0 -10800)-cos(82*(pi/180))*(10800-10800))+10800");
    equation(out,"f29","(cos(90*(pi/180))*(0-10800)+sin(90*(pi/180))*(10800-10800))+10800");
    equation(out,"f30","-(sin(90*(pi/180))*(0-10800)-cos(90*(pi/180))*(10800-10800))+10800");
    equation(out,"f31","(cos(97*(pi/180))*(?f0 -10800)+sin(97*(pi/180))*(10800-10800))+10800");
    equation(out,"f32","-(sin(97*(pi/180))*(?f0 -10800)-cos(97*(pi/180))*(10800-10800))+10800");
    equation(out,"f33","(cos(105*(pi/180))*(0-10800)+sin(105*(pi/180))*(10800-10800))+10800");
    equation(out,"f34","-(sin(105*(pi/180))*(0-10800)-cos(105*(pi/180))*(10800-10800))+10800");
    equation(out,"f35","(cos(112*(pi/180))*(?f0 -10800)+sin(112*(pi/180))*(10800-10800))+10800");
    equation(out,"f36","-(sin(112*(pi/180))*(?f0 -10800)-cos(112*(pi/180))*(10800-10800))+10800");
    equation(out,"f37","(cos(120*(pi/180))*(0-10800)+sin(120*(pi/180))*(10800-10800))+10800");
    equation(out,"f38","-(sin(120*(pi/180))*(0-10800)-cos(120*(pi/180))*(10800-10800))+10800");
    equation(out,"f39","(cos(127*(pi/180))*(?f0 -10800)+sin(127*(pi/180))*(10800-10800))+10800");
    equation(out,"f40","-(sin(127*(pi/180))*(?f0 -10800)-cos(127*(pi/180))*(10800-10800))+10800");
    equation(out,"f41","(cos(135*(pi/180))*(0-10800)+sin(135*(pi/180))*(10800-10800))+10800");
    equation(out,"f42","-(sin(135*(pi/180))*(0-10800)-cos(135*(pi/180))*(10800-10800))+10800");
    equation(out,"f43","(cos(142*(pi/180))*(?f0 -10800)+sin(142*(pi/180))*(10800-10800))+10800");
    equation(out,"f44","-(sin(142*(pi/180))*(?f0 -10800)-cos(142*(pi/180))*(10800-10800))+10800");
    equation(out,"f45","(cos(150*(pi/180))*(0-10800)+sin(150*(pi/180))*(10800-10800))+10800");
    equation(out,"f46","-(sin(150*(pi/180))*(0-10800)-cos(150*(pi/180))*(10800-10800))+10800");
    equation(out,"f47","(cos(157*(pi/180))*(?f0 -10800)+sin(157*(pi/180))*(10800-10800))+10800");
    equation(out,"f48","-(sin(157*(pi/180))*(?f0 -10800)-cos(157*(pi/180))*(10800-10800))+10800");
    equation(out,"f49","(cos(165*(pi/180))*(0-10800)+sin(165*(pi/180))*(10800-10800))+10800");
    equation(out,"f50","-(sin(165*(pi/180))*(0-10800)-cos(165*(pi/180))*(10800-10800))+10800");
    equation(out,"f51","(cos(172*(pi/180))*(?f0 -10800)+sin(172*(pi/180))*(10800-10800))+10800");
    equation(out,"f52","-(sin(172*(pi/180))*(?f0 -10800)-cos(172*(pi/180))*(10800-10800))+10800");
    equation(out,"f53","(cos(180*(pi/180))*(0-10800)+sin(180*(pi/180))*(10800-10800))+10800");
    equation(out,"f54","-(sin(180*(pi/180))*(0-10800)-cos(180*(pi/180))*(10800-10800))+10800");
    equation(out,"f55","(cos(187*(pi/180))*(?f0 -10800)+sin(187*(pi/180))*(10800-10800))+10800");
    equation(out,"f56","-(sin(187*(pi/180))*(?f0 -10800)-cos(187*(pi/180))*(10800-10800))+10800");
    equation(out,"f57","(cos(195*(pi/180))*(0-10800)+sin(195*(pi/180))*(10800-10800))+10800");
    equation(out,"f58","-(sin(195*(pi/180))*(0-10800)-cos(195*(pi/180))*(10800-10800))+10800");
    equation(out,"f59","(cos(202*(pi/180))*(?f0 -10800)+sin(202*(pi/180))*(10800-10800))+10800");
    equation(out,"f60","-(sin(202*(pi/180))*(?f0 -10800)-cos(202*(pi/180))*(10800-10800))+10800");
    equation(out,"f61","(cos(210*(pi/180))*(0-10800)+sin(210*(pi/180))*(10800-10800))+10800");
    equation(out,"f62","-(sin(210*(pi/180))*(0-10800)-cos(210*(pi/180))*(10800-10800))+10800");
    equation(out,"f63","(cos(217*(pi/180))*(?f0 -10800)+sin(217*(pi/180))*(10800-10800))+10800");
    equation(out,"f64","-(sin(217*(pi/180))*(?f0 -10800)-cos(217*(pi/180))*(10800-10800))+10800");
    equation(out,"f65","(cos(225*(pi/180))*(0-10800)+sin(225*(pi/180))*(10800-10800))+10800");
    equation(out,"f66","-(sin(225*(pi/180))*(0-10800)-cos(225*(pi/180))*(10800-10800))+10800");
    equation(out,"f67","(cos(232*(pi/180))*(?f0 -10800)+sin(232*(pi/180))*(10800-10800))+10800");
    equation(out,"f68","-(sin(232*(pi/180))*(?f0 -10800)-cos(232*(pi/180))*(10800-10800))+10800");
    equation(out,"f69","(cos(240*(pi/180))*(0-10800)+sin(240*(pi/180))*(10800-10800))+10800");
    equation(out,"f70","-(sin(240*(pi/180))*(0-10800)-cos(240*(pi/180))*(10800-10800))+10800");
    equation(out,"f71","(cos(247*(pi/180))*(?f0 -10800)+sin(247*(pi/180))*(10800-10800))+10800");
    equation(out,"f72","-(sin(247*(pi/180))*(?f0 -10800)-cos(247*(pi/180))*(10800-10800))+10800");
    equation(out,"f73","(cos(255*(pi/180))*(0-10800)+sin(255*(pi/180))*(10800-10800))+10800");
    equation(out,"f74","-(sin(255*(pi/180))*(0-10800)-cos(255*(pi/180))*(10800-10800))+10800");
    equation(out,"f75","(cos(262*(pi/180))*(?f0 -10800)+sin(262*(pi/180))*(10800-10800))+10800");
    equation(out,"f76","-(sin(262*(pi/180))*(?f0 -10800)-cos(262*(pi/180))*(10800-10800))+10800");
    equation(out,"f77","(cos(270*(pi/180))*(0-10800)+sin(270*(pi/180))*(10800-10800))+10800");
    equation(out,"f78","-(sin(270*(pi/180))*(0-10800)-cos(270*(pi/180))*(10800-10800))+10800");
    equation(out,"f79","(cos(277*(pi/180))*(?f0 -10800)+sin(277*(pi/180))*(10800-10800))+10800");
    equation(out,"f80","-(sin(277*(pi/180))*(?f0 -10800)-cos(277*(pi/180))*(10800-10800))+10800");
    equation(out,"f81","(cos(285*(pi/180))*(0-10800)+sin(285*(pi/180))*(10800-10800))+10800");
    equation(out,"f82","-(sin(285*(pi/180))*(0-10800)-cos(285*(pi/180))*(10800-10800))+10800");
    equation(out,"f83","(cos(292*(pi/180))*(?f0 -10800)+sin(292*(pi/180))*(10800-10800))+10800");
    equation(out,"f84","-(sin(292*(pi/180))*(?f0 -10800)-cos(292*(pi/180))*(10800-10800))+10800");
    equation(out,"f85","(cos(300*(pi/180))*(0-10800)+sin(300*(pi/180))*(10800-10800))+10800");
    equation(out,"f86","-(sin(300*(pi/180))*(0-10800)-cos(300*(pi/180))*(10800-10800))+10800");
    equation(out,"f87","(cos(307*(pi/180))*(?f0 -10800)+sin(307*(pi/180))*(10800-10800))+10800");
    equation(out,"f88","-(sin(307*(pi/180))*(?f0 -10800)-cos(307*(pi/180))*(10800-10800))+10800");
    equation(out,"f89","(cos(315*(pi/180))*(0-10800)+sin(315*(pi/180))*(10800-10800))+10800");
    equation(out,"f90","-(sin(315*(pi/180))*(0-10800)-cos(315*(pi/180))*(10800-10800))+10800");
    equation(out,"f91","(cos(322*(pi/180))*(?f0 -10800)+sin(322*(pi/180))*(10800-10800))+10800");
    equation(out,"f92","-(sin(322*(pi/180))*(?f0 -10800)-cos(322*(pi/180))*(10800-10800))+10800");
    equation(out,"f93","(cos(330*(pi/180))*(0-10800)+sin(330*(pi/180))*(10800-10800))+10800");
    equation(out,"f94","-(sin(330*(pi/180))*(0-10800)-cos(330*(pi/180))*(10800-10800))+10800");
    equation(out,"f95","(cos(337*(pi/180))*(?f0 -10800)+sin(337*(pi/180))*(10800-10800))+10800");
    equation(out,"f96","-(sin(337*(pi/180))*(?f0 -10800)-cos(337*(pi/180))*(10800-10800))+10800");
    equation(out,"f97","(cos(345*(pi/180))*(0-10800)+sin(345*(pi/180))*(10800-10800))+10800");
    equation(out,"f98","-(sin(345*(pi/180))*(0-10800)-cos(345*(pi/180))*(10800-10800))+10800");
    equation(out,"f99","(cos(352*(pi/180))*(?f0 -10800)+sin(352*(pi/180))*(10800-10800))+10800");
    equation(out,"f100","-(sin(352*(pi/180))*(?f0 -10800)-cos(352*(pi/180))*(10800-10800))+10800");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 10800");
    out.xml.addAttribute("draw:handle-range-x-maximum", "10800");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processSeal16(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 2500);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M ?f5 ?f6 L ?f7 ?f8 ?f9 ?f10 ?f11 ?f12 ?f13 ?f14 ?f15 ?f16 ?f17 ?f18 ?f19 ?f20 ?f21 ?f22 ?f23 ?f24 ?f25 ?f26 ?f27 ?f28 ?f29 ?f30 ?f31 ?f32 ?f33 ?f34 ?f35 ?f36 ?f37 ?f38 ?f39 ?f40 ?f41 ?f42 ?f43 ?f44 ?f45 ?f46 ?f47 ?f48 ?f49 ?f50 ?f51 ?f52 ?f53 ?f54 ?f55 ?f56 ?f57 ?f58 ?f59 ?f60 ?f61 ?f62 ?f63 ?f64 ?f65 ?f66 ?f67 ?f68 ?f5 ?f6 Z N");
    out.xml.addAttribute("draw:type", "mso-spt59");
    out.xml.addAttribute("draw:text-areas", "?f1 ?f2 ?f3 ?f4");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","(cos(315*(pi/180))*(?f0 -10800)+sin(315*(pi/180))*(10800-10800))+10800");
    equation(out,"f2","-(sin(315*(pi/180))*(?f0 -10800)-cos(315*(pi/180))*(10800-10800))+10800");
    equation(out,"f3","(cos(135*(pi/180))*(?f0 -10800)+sin(135*(pi/180))*(10800-10800))+10800");
    equation(out,"f4","-(sin(135*(pi/180))*(?f0 -10800)-cos(135*(pi/180))*(10800-10800))+10800");
    equation(out,"f5","(cos(0*(pi/180))*(0-10800)+sin(0*(pi/180))*(10800-10800))+10800");
    equation(out,"f6","-(sin(0*(pi/180))*(0-10800)-cos(0*(pi/180))*(10800-10800))+10800");
    equation(out,"f7","(cos(11*(pi/180))*(?f0 -10800)+sin(11*(pi/180))*(10800-10800))+10800");
    equation(out,"f8","-(sin(11*(pi/180))*(?f0 -10800)-cos(11*(pi/180))*(10800-10800))+10800");
    equation(out,"f9","(cos(22*(pi/180))*(0-10800)+sin(22*(pi/180))*(10800-10800))+10800");
    equation(out,"f10","-(sin(22*(pi/180))*(0-10800)-cos(22*(pi/180))*(10800-10800))+10800");
    equation(out,"f11","(cos(33*(pi/180))*(?f0 -10800)+sin(33*(pi/180))*(10800-10800))+10800");
    equation(out,"f12","-(sin(33*(pi/180))*(?f0 -10800)-cos(33*(pi/180))*(10800-10800))+10800");
    equation(out,"f13","(cos(45*(pi/180))*(0-10800)+sin(45*(pi/180))*(10800-10800))+10800");
    equation(out,"f14","-(sin(45*(pi/180))*(0-10800)-cos(45*(pi/180))*(10800-10800))+10800");
    equation(out,"f15","(cos(56*(pi/180))*(?f0 -10800)+sin(56*(pi/180))*(10800-10800))+10800");
    equation(out,"f16","-(sin(56*(pi/180))*(?f0 -10800)-cos(56*(pi/180))*(10800-10800))+10800");
    equation(out,"f17","(cos(67*(pi/180))*(0-10800)+sin(67*(pi/180))*(10800-10800))+10800");
    equation(out,"f18","-(sin(67*(pi/180))*(0-10800)-cos(67*(pi/180))*(10800-10800))+10800");
    equation(out,"f19","(cos(78*(pi/180))*(?f0 -10800)+sin(78*(pi/180))*(10800-10800))+10800");
    equation(out,"f20","-(sin(78*(pi/180))*(?f0 -10800)-cos(78*(pi/180))*(10800-10800))+10800");
    equation(out,"f21","(cos(90*(pi/180))*(0-10800)+sin(90*(pi/180))*(10800-10800))+10800");
    equation(out,"f22","-(sin(90*(pi/180))*(0-10800)-cos(90*(pi/180))*(10800-10800))+10800");
    equation(out,"f23","(cos(101*(pi/180))*(?f0 -10800)+sin(101*(pi/180))*(10800-10800))+10800");
    equation(out,"f24","-(sin(101*(pi/180))*(?f0 -10800)-cos(101*(pi/180))*(10800-10800))+10800");
    equation(out,"f25","(cos(112*(pi/180))*(0-10800)+sin(112*(pi/180))*(10800-10800))+10800");
    equation(out,"f26","-(sin(112*(pi/180))*(0-10800)-cos(112*(pi/180))*(10800-10800))+10800");
    equation(out,"f27","(cos(123*(pi/180))*(?f0 -10800)+sin(123*(pi/180))*(10800-10800))+10800");
    equation(out,"f28","-(sin(123*(pi/180))*(?f0 -10800)-cos(123*(pi/180))*(10800-10800))+10800");
    equation(out,"f29","(cos(135*(pi/180))*(0-10800)+sin(135*(pi/180))*(10800-10800))+10800");
    equation(out,"f30","-(sin(135*(pi/180))*(0-10800)-cos(135*(pi/180))*(10800-10800))+10800");
    equation(out,"f31","(cos(146*(pi/180))*(?f0 -10800)+sin(146*(pi/180))*(10800-10800))+10800");
    equation(out,"f32","-(sin(146*(pi/180))*(?f0 -10800)-cos(146*(pi/180))*(10800-10800))+10800");
    equation(out,"f33","(cos(157*(pi/180))*(0-10800)+sin(157*(pi/180))*(10800-10800))+10800");
    equation(out,"f34","-(sin(157*(pi/180))*(0-10800)-cos(157*(pi/180))*(10800-10800))+10800");
    equation(out,"f35","(cos(168*(pi/180))*(?f0 -10800)+sin(168*(pi/180))*(10800-10800))+10800");
    equation(out,"f36","-(sin(168*(pi/180))*(?f0 -10800)-cos(168*(pi/180))*(10800-10800))+10800");
    equation(out,"f37","(cos(180*(pi/180))*(0-10800)+sin(180*(pi/180))*(10800-10800))+10800");
    equation(out,"f38","-(sin(180*(pi/180))*(0-10800)-cos(180*(pi/180))*(10800-10800))+10800");
    equation(out,"f39","(cos(191*(pi/180))*(?f0 -10800)+sin(191*(pi/180))*(10800-10800))+10800");
    equation(out,"f40","-(sin(191*(pi/180))*(?f0 -10800)-cos(191*(pi/180))*(10800-10800))+10800");
    equation(out,"f41","(cos(202*(pi/180))*(0-10800)+sin(202*(pi/180))*(10800-10800))+10800");
    equation(out,"f42","-(sin(202*(pi/180))*(0-10800)-cos(202*(pi/180))*(10800-10800))+10800");
    equation(out,"f43","(cos(213*(pi/180))*(?f0 -10800)+sin(213*(pi/180))*(10800-10800))+10800");
    equation(out,"f44","-(sin(213*(pi/180))*(?f0 -10800)-cos(213*(pi/180))*(10800-10800))+10800");
    equation(out,"f45","(cos(225*(pi/180))*(0-10800)+sin(225*(pi/180))*(10800-10800))+10800");
    equation(out,"f46","-(sin(225*(pi/180))*(0-10800)-cos(225*(pi/180))*(10800-10800))+10800");
    equation(out,"f47","(cos(236*(pi/180))*(?f0 -10800)+sin(236*(pi/180))*(10800-10800))+10800");
    equation(out,"f48","-(sin(236*(pi/180))*(?f0 -10800)-cos(236*(pi/180))*(10800-10800))+10800");
    equation(out,"f49","(cos(247*(pi/180))*(0-10800)+sin(247*(pi/180))*(10800-10800))+10800");
    equation(out,"f50","-(sin(247*(pi/180))*(0-10800)-cos(247*(pi/180))*(10800-10800))+10800");
    equation(out,"f51","(cos(258*(pi/180))*(?f0 -10800)+sin(258*(pi/180))*(10800-10800))+10800");
    equation(out,"f52","-(sin(258*(pi/180))*(?f0 -10800)-cos(258*(pi/180))*(10800-10800))+10800");
    equation(out,"f53","(cos(270*(pi/180))*(0-10800)+sin(270*(pi/180))*(10800-10800))+10800");
    equation(out,"f54","-(sin(270*(pi/180))*(0-10800)-cos(270*(pi/180))*(10800-10800))+10800");
    equation(out,"f55","(cos(281*(pi/180))*(?f0 -10800)+sin(281*(pi/180))*(10800-10800))+10800");
    equation(out,"f56","-(sin(281*(pi/180))*(?f0 -10800)-cos(281*(pi/180))*(10800-10800))+10800");
    equation(out,"f57","(cos(292*(pi/180))*(0-10800)+sin(292*(pi/180))*(10800-10800))+10800");
    equation(out,"f58","-(sin(292*(pi/180))*(0-10800)-cos(292*(pi/180))*(10800-10800))+10800");
    equation(out,"f59","(cos(303*(pi/180))*(?f0 -10800)+sin(303*(pi/180))*(10800-10800))+10800");
    equation(out,"f60","-(sin(303*(pi/180))*(?f0 -10800)-cos(303*(pi/180))*(10800-10800))+10800");
    equation(out,"f61","(cos(315*(pi/180))*(0-10800)+sin(315*(pi/180))*(10800-10800))+10800");
    equation(out,"f62","-(sin(315*(pi/180))*(0-10800)-cos(315*(pi/180))*(10800-10800))+10800");
    equation(out,"f63","(cos(326*(pi/180))*(?f0 -10800)+sin(326*(pi/180))*(10800-10800))+10800");
    equation(out,"f64","-(sin(326*(pi/180))*(?f0 -10800)-cos(326*(pi/180))*(10800-10800))+10800");
    equation(out,"f65","(cos(337*(pi/180))*(0-10800)+sin(337*(pi/180))*(10800-10800))+10800");
    equation(out,"f66","-(sin(337*(pi/180))*(0-10800)-cos(337*(pi/180))*(10800-10800))+10800");
    equation(out,"f67","(cos(348*(pi/180))*(?f0 -10800)+sin(348*(pi/180))*(10800-10800))+10800");
    equation(out,"f68","-(sin(348*(pi/180))*(?f0 -10800)-cos(348*(pi/180))*(10800-10800))+10800");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 10800");
    out.xml.addAttribute("draw:handle-range-x-maximum", "10800");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processSeal24(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 2500);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M ?f5 ?f6 L ?f7 ?f8 ?f9 ?f10 ?f11 ?f12 ?f13 ?f14 ?f15 ?f16 ?f17 ?f18 ?f19 ?f20 ?f21 ?f22 ?f23 ?f24 ?f25 ?f26 ?f27 ?f28 ?f29 ?f30 ?f31 ?f32 ?f33 ?f34 ?f35 ?f36 ?f37 ?f38 ?f39 ?f40 ?f41 ?f42 ?f43 ?f44 ?f45 ?f46 ?f47 ?f48 ?f49 ?f50 ?f51 ?f52 ?f53 ?f54 ?f55 ?f56 ?f57 ?f58 ?f59 ?f60 ?f61 ?f62 ?f63 ?f64 ?f65 ?f66 ?f67 ?f68 ?f69 ?f70 ?f71 ?f72 ?f73 ?f74 ?f75 ?f76 ?f77 ?f78 ?f79 ?f80 ?f81 ?f82 ?f83 ?f84 ?f85 ?f86 ?f87 ?f88 ?f89 ?f90 ?f91 ?f92 ?f93 ?f94 ?f95 ?f96 ?f97 ?f98 ?f99 ?f100 ?f5 ?f6 Z N");
    out.xml.addAttribute("draw:type", "star24");
    out.xml.addAttribute("draw:text-areas", "?f1 ?f2 ?f3 ?f4");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","(cos(315*(pi/180))*(?f0 -10800)+sin(315*(pi/180))*(10800-10800))+10800");
    equation(out,"f2","-(sin(315*(pi/180))*(?f0 -10800)-cos(315*(pi/180))*(10800-10800))+10800");
    equation(out,"f3","(cos(135*(pi/180))*(?f0 -10800)+sin(135*(pi/180))*(10800-10800))+10800");
    equation(out,"f4","-(sin(135*(pi/180))*(?f0 -10800)-cos(135*(pi/180))*(10800-10800))+10800");
    equation(out,"f5","(cos(0*(pi/180))*(0-10800)+sin(0*(pi/180))*(10800-10800))+10800");
    equation(out,"f6","-(sin(0*(pi/180))*(0-10800)-cos(0*(pi/180))*(10800-10800))+10800");
    equation(out,"f7","(cos(7*(pi/180))*(?f0 -10800)+sin(7*(pi/180))*(10800-10800))+10800");
    equation(out,"f8","-(sin(7*(pi/180))*(?f0 -10800)-cos(7*(pi/180))*(10800-10800))+10800");
    equation(out,"f9","(cos(15*(pi/180))*(0-10800)+sin(15*(pi/180))*(10800-10800))+10800");
    equation(out,"f10","-(sin(15*(pi/180))*(0-10800)-cos(15*(pi/180))*(10800-10800))+10800");
    equation(out,"f11","(cos(22*(pi/180))*(?f0 -10800)+sin(22*(pi/180))*(10800-10800))+10800");
    equation(out,"f12","-(sin(22*(pi/180))*(?f0 -10800)-cos(22*(pi/180))*(10800-10800))+10800");
    equation(out,"f13","(cos(30*(pi/180))*(0-10800)+sin(30*(pi/180))*(10800-10800))+10800");
    equation(out,"f14","-(sin(30*(pi/180))*(0-10800)-cos(30*(pi/180))*(10800-10800))+10800");
    equation(out,"f15","(cos(37*(pi/180))*(?f0 -10800)+sin(37*(pi/180))*(10800-10800))+10800");
    equation(out,"f16","-(sin(37*(pi/180))*(?f0 -10800)-cos(37*(pi/180))*(10800-10800))+10800");
    equation(out,"f17","(cos(45*(pi/180))*(0-10800)+sin(45*(pi/180))*(10800-10800))+10800");
    equation(out,"f18","-(sin(45*(pi/180))*(0-10800)-cos(45*(pi/180))*(10800-10800))+10800");
    equation(out,"f19","(cos(52*(pi/180))*(?f0 -10800)+sin(52*(pi/180))*(10800-10800))+10800");
    equation(out,"f20","-(sin(52*(pi/180))*(?f0 -10800)-cos(52*(pi/180))*(10800-10800))+10800");
    equation(out,"f21","(cos(60*(pi/180))*(0-10800)+sin(60*(pi/180))*(10800-10800))+10800");
    equation(out,"f22","-(sin(60*(pi/180))*(0-10800)-cos(60*(pi/180))*(10800-10800))+10800");
    equation(out,"f23","(cos(67*(pi/180))*(?f0 -10800)+sin(67*(pi/180))*(10800-10800))+10800");
    equation(out,"f24","-(sin(67*(pi/180))*(?f0 -10800)-cos(67*(pi/180))*(10800-10800))+10800");
    equation(out,"f25","(cos(75*(pi/180))*(0-10800)+sin(75*(pi/180))*(10800-10800))+10800");
    equation(out,"f26","-(sin(75*(pi/180))*(0-10800)-cos(75*(pi/180))*(10800-10800))+10800");
    equation(out,"f27","(cos(82*(pi/180))*(?f0 -10800)+sin(82*(pi/180))*(10800-10800))+10800");
    equation(out,"f28","-(sin(82*(pi/180))*(?f0 -10800)-cos(82*(pi/180))*(10800-10800))+10800");
    equation(out,"f29","(cos(90*(pi/180))*(0-10800)+sin(90*(pi/180))*(10800-10800))+10800");
    equation(out,"f30","-(sin(90*(pi/180))*(0-10800)-cos(90*(pi/180))*(10800-10800))+10800");
    equation(out,"f31","(cos(97*(pi/180))*(?f0 -10800)+sin(97*(pi/180))*(10800-10800))+10800");
    equation(out,"f32","-(sin(97*(pi/180))*(?f0 -10800)-cos(97*(pi/180))*(10800-10800))+10800");
    equation(out,"f33","(cos(105*(pi/180))*(0-10800)+sin(105*(pi/180))*(10800-10800))+10800");
    equation(out,"f34","-(sin(105*(pi/180))*(0-10800)-cos(105*(pi/180))*(10800-10800))+10800");
    equation(out,"f35","(cos(112*(pi/180))*(?f0 -10800)+sin(112*(pi/180))*(10800-10800))+10800");
    equation(out,"f36","-(sin(112*(pi/180))*(?f0 -10800)-cos(112*(pi/180))*(10800-10800))+10800");
    equation(out,"f37","(cos(120*(pi/180))*(0-10800)+sin(120*(pi/180))*(10800-10800))+10800");
    equation(out,"f38","-(sin(120*(pi/180))*(0-10800)-cos(120*(pi/180))*(10800-10800))+10800");
    equation(out,"f39","(cos(127*(pi/180))*(?f0 -10800)+sin(127*(pi/180))*(10800-10800))+10800");
    equation(out,"f40","-(sin(127*(pi/180))*(?f0 -10800)-cos(127*(pi/180))*(10800-10800))+10800");
    equation(out,"f41","(cos(135*(pi/180))*(0-10800)+sin(135*(pi/180))*(10800-10800))+10800");
    equation(out,"f42","-(sin(135*(pi/180))*(0-10800)-cos(135*(pi/180))*(10800-10800))+10800");
    equation(out,"f43","(cos(142*(pi/180))*(?f0 -10800)+sin(142*(pi/180))*(10800-10800))+10800");
    equation(out,"f44","-(sin(142*(pi/180))*(?f0 -10800)-cos(142*(pi/180))*(10800-10800))+10800");
    equation(out,"f45","(cos(150*(pi/180))*(0-10800)+sin(150*(pi/180))*(10800-10800))+10800");
    equation(out,"f46","-(sin(150*(pi/180))*(0-10800)-cos(150*(pi/180))*(10800-10800))+10800");
    equation(out,"f47","(cos(157*(pi/180))*(?f0 -10800)+sin(157*(pi/180))*(10800-10800))+10800");
    equation(out,"f48","-(sin(157*(pi/180))*(?f0 -10800)-cos(157*(pi/180))*(10800-10800))+10800");
    equation(out,"f49","(cos(165*(pi/180))*(0-10800)+sin(165*(pi/180))*(10800-10800))+10800");
    equation(out,"f50","-(sin(165*(pi/180))*(0-10800)-cos(165*(pi/180))*(10800-10800))+10800");
    equation(out,"f51","(cos(172*(pi/180))*(?f0 -10800)+sin(172*(pi/180))*(10800-10800))+10800");
    equation(out,"f52","-(sin(172*(pi/180))*(?f0 -10800)-cos(172*(pi/180))*(10800-10800))+10800");
    equation(out,"f53","(cos(180*(pi/180))*(0-10800)+sin(180*(pi/180))*(10800-10800))+10800");
    equation(out,"f54","-(sin(180*(pi/180))*(0-10800)-cos(180*(pi/180))*(10800-10800))+10800");
    equation(out,"f55","(cos(187*(pi/180))*(?f0 -10800)+sin(187*(pi/180))*(10800-10800))+10800");
    equation(out,"f56","-(sin(187*(pi/180))*(?f0 -10800)-cos(187*(pi/180))*(10800-10800))+10800");
    equation(out,"f57","(cos(195*(pi/180))*(0-10800)+sin(195*(pi/180))*(10800-10800))+10800");
    equation(out,"f58","-(sin(195*(pi/180))*(0-10800)-cos(195*(pi/180))*(10800-10800))+10800");
    equation(out,"f59","(cos(202*(pi/180))*(?f0 -10800)+sin(202*(pi/180))*(10800-10800))+10800");
    equation(out,"f60","-(sin(202*(pi/180))*(?f0 -10800)-cos(202*(pi/180))*(10800-10800))+10800");
    equation(out,"f61","(cos(210*(pi/180))*(0-10800)+sin(210*(pi/180))*(10800-10800))+10800");
    equation(out,"f62","-(sin(210*(pi/180))*(0-10800)-cos(210*(pi/180))*(10800-10800))+10800");
    equation(out,"f63","(cos(217*(pi/180))*(?f0 -10800)+sin(217*(pi/180))*(10800-10800))+10800");
    equation(out,"f64","-(sin(217*(pi/180))*(?f0 -10800)-cos(217*(pi/180))*(10800-10800))+10800");
    equation(out,"f65","(cos(225*(pi/180))*(0-10800)+sin(225*(pi/180))*(10800-10800))+10800");
    equation(out,"f66","-(sin(225*(pi/180))*(0-10800)-cos(225*(pi/180))*(10800-10800))+10800");
    equation(out,"f67","(cos(232*(pi/180))*(?f0 -10800)+sin(232*(pi/180))*(10800-10800))+10800");
    equation(out,"f68","-(sin(232*(pi/180))*(?f0 -10800)-cos(232*(pi/180))*(10800-10800))+10800");
    equation(out,"f69","(cos(240*(pi/180))*(0-10800)+sin(240*(pi/180))*(10800-10800))+10800");
    equation(out,"f70","-(sin(240*(pi/180))*(0-10800)-cos(240*(pi/180))*(10800-10800))+10800");
    equation(out,"f71","(cos(247*(pi/180))*(?f0 -10800)+sin(247*(pi/180))*(10800-10800))+10800");
    equation(out,"f72","-(sin(247*(pi/180))*(?f0 -10800)-cos(247*(pi/180))*(10800-10800))+10800");
    equation(out,"f73","(cos(255*(pi/180))*(0-10800)+sin(255*(pi/180))*(10800-10800))+10800");
    equation(out,"f74","-(sin(255*(pi/180))*(0-10800)-cos(255*(pi/180))*(10800-10800))+10800");
    equation(out,"f75","(cos(262*(pi/180))*(?f0 -10800)+sin(262*(pi/180))*(10800-10800))+10800");
    equation(out,"f76","-(sin(262*(pi/180))*(?f0 -10800)-cos(262*(pi/180))*(10800-10800))+10800");
    equation(out,"f77","(cos(270*(pi/180))*(0-10800)+sin(270*(pi/180))*(10800-10800))+10800");
    equation(out,"f78","-(sin(270*(pi/180))*(0-10800)-cos(270*(pi/180))*(10800-10800))+10800");
    equation(out,"f79","(cos(277*(pi/180))*(?f0 -10800)+sin(277*(pi/180))*(10800-10800))+10800");
    equation(out,"f80","-(sin(277*(pi/180))*(?f0 -10800)-cos(277*(pi/180))*(10800-10800))+10800");
    equation(out,"f81","(cos(285*(pi/180))*(0-10800)+sin(285*(pi/180))*(10800-10800))+10800");
    equation(out,"f82","-(sin(285*(pi/180))*(0-10800)-cos(285*(pi/180))*(10800-10800))+10800");
    equation(out,"f83","(cos(292*(pi/180))*(?f0 -10800)+sin(292*(pi/180))*(10800-10800))+10800");
    equation(out,"f84","-(sin(292*(pi/180))*(?f0 -10800)-cos(292*(pi/180))*(10800-10800))+10800");
    equation(out,"f85","(cos(300*(pi/180))*(0-10800)+sin(300*(pi/180))*(10800-10800))+10800");
    equation(out,"f86","-(sin(300*(pi/180))*(0-10800)-cos(300*(pi/180))*(10800-10800))+10800");
    equation(out,"f87","(cos(307*(pi/180))*(?f0 -10800)+sin(307*(pi/180))*(10800-10800))+10800");
    equation(out,"f88","-(sin(307*(pi/180))*(?f0 -10800)-cos(307*(pi/180))*(10800-10800))+10800");
    equation(out,"f89","(cos(315*(pi/180))*(0-10800)+sin(315*(pi/180))*(10800-10800))+10800");
    equation(out,"f90","-(sin(315*(pi/180))*(0-10800)-cos(315*(pi/180))*(10800-10800))+10800");
    equation(out,"f91","(cos(322*(pi/180))*(?f0 -10800)+sin(322*(pi/180))*(10800-10800))+10800");
    equation(out,"f92","-(sin(322*(pi/180))*(?f0 -10800)-cos(322*(pi/180))*(10800-10800))+10800");
    equation(out,"f93","(cos(330*(pi/180))*(0-10800)+sin(330*(pi/180))*(10800-10800))+10800");
    equation(out,"f94","-(sin(330*(pi/180))*(0-10800)-cos(330*(pi/180))*(10800-10800))+10800");
    equation(out,"f95","(cos(337*(pi/180))*(?f0 -10800)+sin(337*(pi/180))*(10800-10800))+10800");
    equation(out,"f96","-(sin(337*(pi/180))*(?f0 -10800)-cos(337*(pi/180))*(10800-10800))+10800");
    equation(out,"f97","(cos(345*(pi/180))*(0-10800)+sin(345*(pi/180))*(10800-10800))+10800");
    equation(out,"f98","-(sin(345*(pi/180))*(0-10800)-cos(345*(pi/180))*(10800-10800))+10800");
    equation(out,"f99","(cos(352*(pi/180))*(?f0 -10800)+sin(352*(pi/180))*(10800-10800))+10800");
    equation(out,"f100","-(sin(352*(pi/180))*(?f0 -10800)-cos(352*(pi/180))*(10800-10800))+10800");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 10800");
    out.xml.addAttribute("draw:handle-range-x-maximum", "10800");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processSeal32(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 2500);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M ?f5 ?f6 L ?f7 ?f8 ?f9 ?f10 ?f11 ?f12 ?f13 ?f14 ?f15 ?f16 ?f17 ?f18 ?f19 ?f20 ?f21 ?f22 ?f23 ?f24 ?f25 ?f26 ?f27 ?f28 ?f29 ?f30 ?f31 ?f32 ?f33 ?f34 ?f35 ?f36 ?f37 ?f38 ?f39 ?f40 ?f41 ?f42 ?f43 ?f44 ?f45 ?f46 ?f47 ?f48 ?f49 ?f50 ?f51 ?f52 ?f53 ?f54 ?f55 ?f56 ?f57 ?f58 ?f59 ?f60 ?f61 ?f62 ?f63 ?f64 ?f65 ?f66 ?f67 ?f68 ?f69 ?f70 ?f71 ?f72 ?f73 ?f74 ?f75 ?f76 ?f77 ?f78 ?f79 ?f80 ?f81 ?f82 ?f83 ?f84 ?f85 ?f86 ?f87 ?f88 ?f89 ?f90 ?f91 ?f92 ?f93 ?f94 ?f95 ?f96 ?f97 ?f98 ?f99 ?f100 ?f101 ?f102 ?f103 ?f104 ?f105 ?f106 ?f107 ?f108 ?f109 ?f110 ?f111 ?f112 ?f113 ?f114 ?f115 ?f116 ?f117 ?f118 ?f119 ?f120 ?f121 ?f122 ?f123 ?f124 ?f125 ?f126 ?f127 ?f128 ?f129 ?f130 ?f131 ?f132 ?f5 ?f6 Z N");
    out.xml.addAttribute("draw:type", "mso-spt60");
    out.xml.addAttribute("draw:text-areas", "?f1 ?f2 ?f3 ?f4");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","(cos(315*(pi/180))*(?f0 -10800)+sin(315*(pi/180))*(10800-10800))+10800");
    equation(out,"f2","-(sin(315*(pi/180))*(?f0 -10800)-cos(315*(pi/180))*(10800-10800))+10800");
    equation(out,"f3","(cos(135*(pi/180))*(?f0 -10800)+sin(135*(pi/180))*(10800-10800))+10800");
    equation(out,"f4","-(sin(135*(pi/180))*(?f0 -10800)-cos(135*(pi/180))*(10800-10800))+10800");
    equation(out,"f5","(cos(0*(pi/180))*(0-10800)+sin(0*(pi/180))*(10800-10800))+10800");
    equation(out,"f6","-(sin(0*(pi/180))*(0-10800)-cos(0*(pi/180))*(10800-10800))+10800");
    equation(out,"f7","(cos(5*(pi/180))*(?f0 -10800)+sin(5*(pi/180))*(10800-10800))+10800");
    equation(out,"f8","-(sin(5*(pi/180))*(?f0 -10800)-cos(5*(pi/180))*(10800-10800))+10800");
    equation(out,"f9","(cos(11*(pi/180))*(0-10800)+sin(11*(pi/180))*(10800-10800))+10800");
    equation(out,"f10","-(sin(11*(pi/180))*(0-10800)-cos(11*(pi/180))*(10800-10800))+10800");
    equation(out,"f11","(cos(16*(pi/180))*(?f0 -10800)+sin(16*(pi/180))*(10800-10800))+10800");
    equation(out,"f12","-(sin(16*(pi/180))*(?f0 -10800)-cos(16*(pi/180))*(10800-10800))+10800");
    equation(out,"f13","(cos(22*(pi/180))*(0-10800)+sin(22*(pi/180))*(10800-10800))+10800");
    equation(out,"f14","-(sin(22*(pi/180))*(0-10800)-cos(22*(pi/180))*(10800-10800))+10800");
    equation(out,"f15","(cos(28*(pi/180))*(?f0 -10800)+sin(28*(pi/180))*(10800-10800))+10800");
    equation(out,"f16","-(sin(28*(pi/180))*(?f0 -10800)-cos(28*(pi/180))*(10800-10800))+10800");
    equation(out,"f17","(cos(33*(pi/180))*(0-10800)+sin(33*(pi/180))*(10800-10800))+10800");
    equation(out,"f18","-(sin(33*(pi/180))*(0-10800)-cos(33*(pi/180))*(10800-10800))+10800");
    equation(out,"f19","(cos(39*(pi/180))*(?f0 -10800)+sin(39*(pi/180))*(10800-10800))+10800");
    equation(out,"f20","-(sin(39*(pi/180))*(?f0 -10800)-cos(39*(pi/180))*(10800-10800))+10800");
    equation(out,"f21","(cos(45*(pi/180))*(0-10800)+sin(45*(pi/180))*(10800-10800))+10800");
    equation(out,"f22","-(sin(45*(pi/180))*(0-10800)-cos(45*(pi/180))*(10800-10800))+10800");
    equation(out,"f23","(cos(50*(pi/180))*(?f0 -10800)+sin(50*(pi/180))*(10800-10800))+10800");
    equation(out,"f24","-(sin(50*(pi/180))*(?f0 -10800)-cos(50*(pi/180))*(10800-10800))+10800");
    equation(out,"f25","(cos(56*(pi/180))*(0-10800)+sin(56*(pi/180))*(10800-10800))+10800");
    equation(out,"f26","-(sin(56*(pi/180))*(0-10800)-cos(56*(pi/180))*(10800-10800))+10800");
    equation(out,"f27","(cos(61*(pi/180))*(?f0 -10800)+sin(61*(pi/180))*(10800-10800))+10800");
    equation(out,"f28","-(sin(61*(pi/180))*(?f0 -10800)-cos(61*(pi/180))*(10800-10800))+10800");
    equation(out,"f29","(cos(67*(pi/180))*(0-10800)+sin(67*(pi/180))*(10800-10800))+10800");
    equation(out,"f30","-(sin(67*(pi/180))*(0-10800)-cos(67*(pi/180))*(10800-10800))+10800");
    equation(out,"f31","(cos(73*(pi/180))*(?f0 -10800)+sin(73*(pi/180))*(10800-10800))+10800");
    equation(out,"f32","-(sin(73*(pi/180))*(?f0 -10800)-cos(73*(pi/180))*(10800-10800))+10800");
    equation(out,"f33","(cos(78*(pi/180))*(0-10800)+sin(78*(pi/180))*(10800-10800))+10800");
    equation(out,"f34","-(sin(78*(pi/180))*(0-10800)-cos(78*(pi/180))*(10800-10800))+10800");
    equation(out,"f35","(cos(84*(pi/180))*(?f0 -10800)+sin(84*(pi/180))*(10800-10800))+10800");
    equation(out,"f36","-(sin(84*(pi/180))*(?f0 -10800)-cos(84*(pi/180))*(10800-10800))+10800");
    equation(out,"f37","(cos(90*(pi/180))*(0-10800)+sin(90*(pi/180))*(10800-10800))+10800");
    equation(out,"f38","-(sin(90*(pi/180))*(0-10800)-cos(90*(pi/180))*(10800-10800))+10800");
    equation(out,"f39","(cos(95*(pi/180))*(?f0 -10800)+sin(95*(pi/180))*(10800-10800))+10800");
    equation(out,"f40","-(sin(95*(pi/180))*(?f0 -10800)-cos(95*(pi/180))*(10800-10800))+10800");
    equation(out,"f41","(cos(101*(pi/180))*(0-10800)+sin(101*(pi/180))*(10800-10800))+10800");
    equation(out,"f42","-(sin(101*(pi/180))*(0-10800)-cos(101*(pi/180))*(10800-10800))+10800");
    equation(out,"f43","(cos(106*(pi/180))*(?f0 -10800)+sin(106*(pi/180))*(10800-10800))+10800");
    equation(out,"f44","-(sin(106*(pi/180))*(?f0 -10800)-cos(106*(pi/180))*(10800-10800))+10800");
    equation(out,"f45","(cos(112*(pi/180))*(0-10800)+sin(112*(pi/180))*(10800-10800))+10800");
    equation(out,"f46","-(sin(112*(pi/180))*(0-10800)-cos(112*(pi/180))*(10800-10800))+10800");
    equation(out,"f47","(cos(118*(pi/180))*(?f0 -10800)+sin(118*(pi/180))*(10800-10800))+10800");
    equation(out,"f48","-(sin(118*(pi/180))*(?f0 -10800)-cos(118*(pi/180))*(10800-10800))+10800");
    equation(out,"f49","(cos(123*(pi/180))*(0-10800)+sin(123*(pi/180))*(10800-10800))+10800");
    equation(out,"f50","-(sin(123*(pi/180))*(0-10800)-cos(123*(pi/180))*(10800-10800))+10800");
    equation(out,"f51","(cos(129*(pi/180))*(?f0 -10800)+sin(129*(pi/180))*(10800-10800))+10800");
    equation(out,"f52","-(sin(129*(pi/180))*(?f0 -10800)-cos(129*(pi/180))*(10800-10800))+10800");
    equation(out,"f53","(cos(135*(pi/180))*(0-10800)+sin(135*(pi/180))*(10800-10800))+10800");
    equation(out,"f54","-(sin(135*(pi/180))*(0-10800)-cos(135*(pi/180))*(10800-10800))+10800");
    equation(out,"f55","(cos(140*(pi/180))*(?f0 -10800)+sin(140*(pi/180))*(10800-10800))+10800");
    equation(out,"f56","-(sin(140*(pi/180))*(?f0 -10800)-cos(140*(pi/180))*(10800-10800))+10800");
    equation(out,"f57","(cos(146*(pi/180))*(0-10800)+sin(146*(pi/180))*(10800-10800))+10800");
    equation(out,"f58","-(sin(146*(pi/180))*(0-10800)-cos(146*(pi/180))*(10800-10800))+10800");
    equation(out,"f59","(cos(151*(pi/180))*(?f0 -10800)+sin(151*(pi/180))*(10800-10800))+10800");
    equation(out,"f60","-(sin(151*(pi/180))*(?f0 -10800)-cos(151*(pi/180))*(10800-10800))+10800");
    equation(out,"f61","(cos(157*(pi/180))*(0-10800)+sin(157*(pi/180))*(10800-10800))+10800");
    equation(out,"f62","-(sin(157*(pi/180))*(0-10800)-cos(157*(pi/180))*(10800-10800))+10800");
    equation(out,"f63","(cos(163*(pi/180))*(?f0 -10800)+sin(163*(pi/180))*(10800-10800))+10800");
    equation(out,"f64","-(sin(163*(pi/180))*(?f0 -10800)-cos(163*(pi/180))*(10800-10800))+10800");
    equation(out,"f65","(cos(168*(pi/180))*(0-10800)+sin(168*(pi/180))*(10800-10800))+10800");
    equation(out,"f66","-(sin(168*(pi/180))*(0-10800)-cos(168*(pi/180))*(10800-10800))+10800");
    equation(out,"f67","(cos(174*(pi/180))*(?f0 -10800)+sin(174*(pi/180))*(10800-10800))+10800");
    equation(out,"f68","-(sin(174*(pi/180))*(?f0 -10800)-cos(174*(pi/180))*(10800-10800))+10800");
    equation(out,"f69","(cos(180*(pi/180))*(0-10800)+sin(180*(pi/180))*(10800-10800))+10800");
    equation(out,"f70","-(sin(180*(pi/180))*(0-10800)-cos(180*(pi/180))*(10800-10800))+10800");
    equation(out,"f71","(cos(185*(pi/180))*(?f0 -10800)+sin(185*(pi/180))*(10800-10800))+10800");
    equation(out,"f72","-(sin(185*(pi/180))*(?f0 -10800)-cos(185*(pi/180))*(10800-10800))+10800");
    equation(out,"f73","(cos(191*(pi/180))*(0-10800)+sin(191*(pi/180))*(10800-10800))+10800");
    equation(out,"f74","-(sin(191*(pi/180))*(0-10800)-cos(191*(pi/180))*(10800-10800))+10800");
    equation(out,"f75","(cos(196*(pi/180))*(?f0 -10800)+sin(196*(pi/180))*(10800-10800))+10800");
    equation(out,"f76","-(sin(196*(pi/180))*(?f0 -10800)-cos(196*(pi/180))*(10800-10800))+10800");
    equation(out,"f77","(cos(202*(pi/180))*(0-10800)+sin(202*(pi/180))*(10800-10800))+10800");
    equation(out,"f78","-(sin(202*(pi/180))*(0-10800)-cos(202*(pi/180))*(10800-10800))+10800");
    equation(out,"f79","(cos(208*(pi/180))*(?f0 -10800)+sin(208*(pi/180))*(10800-10800))+10800");
    equation(out,"f80","-(sin(208*(pi/180))*(?f0 -10800)-cos(208*(pi/180))*(10800-10800))+10800");
    equation(out,"f81","(cos(213*(pi/180))*(0-10800)+sin(213*(pi/180))*(10800-10800))+10800");
    equation(out,"f82","-(sin(213*(pi/180))*(0-10800)-cos(213*(pi/180))*(10800-10800))+10800");
    equation(out,"f83","(cos(219*(pi/180))*(?f0 -10800)+sin(219*(pi/180))*(10800-10800))+10800");
    equation(out,"f84","-(sin(219*(pi/180))*(?f0 -10800)-cos(219*(pi/180))*(10800-10800))+10800");
    equation(out,"f85","(cos(225*(pi/180))*(0-10800)+sin(225*(pi/180))*(10800-10800))+10800");
    equation(out,"f86","-(sin(225*(pi/180))*(0-10800)-cos(225*(pi/180))*(10800-10800))+10800");
    equation(out,"f87","(cos(230*(pi/180))*(?f0 -10800)+sin(230*(pi/180))*(10800-10800))+10800");
    equation(out,"f88","-(sin(230*(pi/180))*(?f0 -10800)-cos(230*(pi/180))*(10800-10800))+10800");
    equation(out,"f89","(cos(236*(pi/180))*(0-10800)+sin(236*(pi/180))*(10800-10800))+10800");
    equation(out,"f90","-(sin(236*(pi/180))*(0-10800)-cos(236*(pi/180))*(10800-10800))+10800");
    equation(out,"f91","(cos(241*(pi/180))*(?f0 -10800)+sin(241*(pi/180))*(10800-10800))+10800");
    equation(out,"f92","-(sin(241*(pi/180))*(?f0 -10800)-cos(241*(pi/180))*(10800-10800))+10800");
    equation(out,"f93","(cos(247*(pi/180))*(0-10800)+sin(247*(pi/180))*(10800-10800))+10800");
    equation(out,"f94","-(sin(247*(pi/180))*(0-10800)-cos(247*(pi/180))*(10800-10800))+10800");
    equation(out,"f95","(cos(253*(pi/180))*(?f0 -10800)+sin(253*(pi/180))*(10800-10800))+10800");
    equation(out,"f96","-(sin(253*(pi/180))*(?f0 -10800)-cos(253*(pi/180))*(10800-10800))+10800");
    equation(out,"f97","(cos(258*(pi/180))*(0-10800)+sin(258*(pi/180))*(10800-10800))+10800");
    equation(out,"f98","-(sin(258*(pi/180))*(0-10800)-cos(258*(pi/180))*(10800-10800))+10800");
    equation(out,"f99","(cos(264*(pi/180))*(?f0 -10800)+sin(264*(pi/180))*(10800-10800))+10800");
    equation(out,"f100","-(sin(264*(pi/180))*(?f0 -10800)-cos(264*(pi/180))*(10800-10800))+10800");
    equation(out,"f101","(cos(270*(pi/180))*(0-10800)+sin(270*(pi/180))*(10800-10800))+10800");
    equation(out,"f102","-(sin(270*(pi/180))*(0-10800)-cos(270*(pi/180))*(10800-10800))+10800");
    equation(out,"f103","(cos(275*(pi/180))*(?f0 -10800)+sin(275*(pi/180))*(10800-10800))+10800");
    equation(out,"f104","-(sin(275*(pi/180))*(?f0 -10800)-cos(275*(pi/180))*(10800-10800))+10800");
    equation(out,"f105","(cos(281*(pi/180))*(0-10800)+sin(281*(pi/180))*(10800-10800))+10800");
    equation(out,"f106","-(sin(281*(pi/180))*(0-10800)-cos(281*(pi/180))*(10800-10800))+10800");
    equation(out,"f107","(cos(286*(pi/180))*(?f0 -10800)+sin(286*(pi/180))*(10800-10800))+10800");
    equation(out,"f108","-(sin(286*(pi/180))*(?f0 -10800)-cos(286*(pi/180))*(10800-10800))+10800");
    equation(out,"f109","(cos(292*(pi/180))*(0-10800)+sin(292*(pi/180))*(10800-10800))+10800");
    equation(out,"f110","-(sin(292*(pi/180))*(0-10800)-cos(292*(pi/180))*(10800-10800))+10800");
    equation(out,"f111","(cos(298*(pi/180))*(?f0 -10800)+sin(298*(pi/180))*(10800-10800))+10800");
    equation(out,"f112","-(sin(298*(pi/180))*(?f0 -10800)-cos(298*(pi/180))*(10800-10800))+10800");
    equation(out,"f113","(cos(303*(pi/180))*(0-10800)+sin(303*(pi/180))*(10800-10800))+10800");
    equation(out,"f114","-(sin(303*(pi/180))*(0-10800)-cos(303*(pi/180))*(10800-10800))+10800");
    equation(out,"f115","(cos(309*(pi/180))*(?f0 -10800)+sin(309*(pi/180))*(10800-10800))+10800");
    equation(out,"f116","-(sin(309*(pi/180))*(?f0 -10800)-cos(309*(pi/180))*(10800-10800))+10800");
    equation(out,"f117","(cos(315*(pi/180))*(0-10800)+sin(315*(pi/180))*(10800-10800))+10800");
    equation(out,"f118","-(sin(315*(pi/180))*(0-10800)-cos(315*(pi/180))*(10800-10800))+10800");
    equation(out,"f119","(cos(320*(pi/180))*(?f0 -10800)+sin(320*(pi/180))*(10800-10800))+10800");
    equation(out,"f120","-(sin(320*(pi/180))*(?f0 -10800)-cos(320*(pi/180))*(10800-10800))+10800");
    equation(out,"f121","(cos(326*(pi/180))*(0-10800)+sin(326*(pi/180))*(10800-10800))+10800");
    equation(out,"f122","-(sin(326*(pi/180))*(0-10800)-cos(326*(pi/180))*(10800-10800))+10800");
    equation(out,"f123","(cos(331*(pi/180))*(?f0 -10800)+sin(331*(pi/180))*(10800-10800))+10800");
    equation(out,"f124","-(sin(331*(pi/180))*(?f0 -10800)-cos(331*(pi/180))*(10800-10800))+10800");
    equation(out,"f125","(cos(337*(pi/180))*(0-10800)+sin(337*(pi/180))*(10800-10800))+10800");
    equation(out,"f126","-(sin(337*(pi/180))*(0-10800)-cos(337*(pi/180))*(10800-10800))+10800");
    equation(out,"f127","(cos(343*(pi/180))*(?f0 -10800)+sin(343*(pi/180))*(10800-10800))+10800");
    equation(out,"f128","-(sin(343*(pi/180))*(?f0 -10800)-cos(343*(pi/180))*(10800-10800))+10800");
    equation(out,"f129","(cos(348*(pi/180))*(0-10800)+sin(348*(pi/180))*(10800-10800))+10800");
    equation(out,"f130","-(sin(348*(pi/180))*(0-10800)-cos(348*(pi/180))*(10800-10800))+10800");
    equation(out,"f131","(cos(354*(pi/180))*(?f0 -10800)+sin(354*(pi/180))*(10800-10800))+10800");
    equation(out,"f132","-(sin(354*(pi/180))*(?f0 -10800)-cos(354*(pi/180))*(10800-10800))+10800");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 10800");
    out.xml.addAttribute("draw:handle-range-x-maximum", "10800");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processRibbon2(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 5400<< 18900);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M ?f12 ?f1 L ?f12 ?f13 C ?f12 ?f14 ?f15 21600 ?f16 21600 L 0 21600 2750 ?f7 0 ?f2 ?f0 ?f2 ?f0 ?f4 C ?f0 ?f5 ?f10 0 ?f11 0 L ?f17 0 C ?f18 0 ?f19 ?f5 ?f19 ?f4 L ?f19 ?f2 21600 ?f2 18850 ?f7 21600 21600 ?f20 21600 C ?f21 21600 ?f22 ?f14 ?f22 ?f13 L ?f22 ?f1 Z N M ?f12 ?f1 L ?f12 ?f13 C ?f12 ?f23 ?f15 ?f24 ?f16 ?f24 L ?f11 ?f24 C ?f10 ?f24 ?f0 ?f26 ?f0 ?f25 ?f0 ?f27 ?f10 ?f1 ?f11 ?f1 Z N M ?f22 ?f1 L ?f22 ?f13 C ?f22 ?f23 ?f21 ?f24 ?f20 ?f24 L ?f17 ?f24 C ?f18 ?f24 ?f19 ?f26 ?f19 ?f25 ?f19 ?f27 ?f18 ?f1 ?f17 ?f1 Z N M ?f0 ?f25 L ?f0 ?f2 N M ?f19 ?f25 L ?f19 ?f2 N");
    out.xml.addAttribute("draw:type", "mso-spt54");
    out.xml.addAttribute("draw:text-areas", "?f0 0 ?f19 ?f1");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","$1 ");
    equation(out,"f2","21600-?f1 ");
    equation(out,"f3","?f2 /2");
    equation(out,"f4","?f3 /2");
    equation(out,"f5","?f4 /2");
    equation(out,"f6","?f1 /2");
    equation(out,"f7","21600-?f6 ");
    equation(out,"f8","420");
    equation(out,"f9","?f8 *2");
    equation(out,"f10","?f0 +?f8 ");
    equation(out,"f11","?f0 +?f9 ");
    equation(out,"f12","?f0 +2700");
    equation(out,"f13","21600-?f4 ");
    equation(out,"f14","21600-?f5 ");
    equation(out,"f15","?f12 -?f8 ");
    equation(out,"f16","?f12 -?f9 ");
    equation(out,"f17","21600-?f11 ");
    equation(out,"f18","21600-?f10 ");
    equation(out,"f19","21600-?f0 ");
    equation(out,"f20","21600-?f16 ");
    equation(out,"f21","21600-?f15 ");
    equation(out,"f22","21600-?f12 ");
    equation(out,"f23","?f13 -?f5 ");
    equation(out,"f24","?f1 +?f3 ");
    equation(out,"f25","?f1 +?f4 ");
    equation(out,"f26","?f25 +?f5 ");
    equation(out,"f27","?f25 -?f5 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-maximum", "8100");
    out.xml.addAttribute("draw:handle-range-x-minimum", "2700");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "10800 $1");
    out.xml.addAttribute("draw:handle-range-y-minimum", "14400");
    out.xml.addAttribute("draw:handle-range-y-maximum", "21600");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processRibbon(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "?f17 ?f10 2700 ?f14 ?f17 21600 ?f18 ?f14");
    processModifiers(o, out, QList<int>() << 5400<< 2700);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L ?f3 0 X ?f4 ?f11 L ?f4 ?f10 ?f5 ?f10 ?f5 ?f11 Y ?f6 0 L 21600 0 ?f18 ?f14 21600 ?f15 ?f9 ?f15 ?f9 ?f16 Y ?f8 21600 L ?f1 21600 X ?f0 ?f16 L ?f0 ?f15 0 ?f15 2700 ?f14 Z N M ?f4 ?f11 F Y ?f3 ?f12 L ?f1 ?f12 X ?f0 ?f13 ?f1 ?f10 L ?f4 ?f10 N M ?f5 ?f11 F Y ?f6 ?f12 L ?f8 ?f12 X ?f9 ?f13 ?f8 ?f10 L ?f5 ?f10 N M ?f0 ?f13 F L ?f0 ?f15 N M ?f9 ?f13 F L ?f9 ?f15 N");
    out.xml.addAttribute("draw:type", "mso-spt53");
    out.xml.addAttribute("draw:text-areas", "?f0 ?f10 ?f9 21600");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","?f0 +675");
    equation(out,"f2","?f1 +675");
    equation(out,"f3","?f2 +675");
    equation(out,"f4","?f3 +675");
    equation(out,"f5","21600-?f4 ");
    equation(out,"f6","21600-?f3 ");
    equation(out,"f7","21600-?f2 ");
    equation(out,"f8","21600-?f1 ");
    equation(out,"f9","21600-?f0 ");
    equation(out,"f10","$1 ");
    equation(out,"f11","?f10 /4");
    equation(out,"f12","?f11 *2");
    equation(out,"f13","?f11 *3");
    equation(out,"f14","10800-?f12 ");
    equation(out,"f15","21600-?f10 ");
    equation(out,"f16","21600-?f11 ");
    equation(out,"f17","21600/2");
    equation(out,"f18","21600-2700");
    equation(out,"f19","?f17 -2700");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-maximum", "8100");
    out.xml.addAttribute("draw:handle-range-x-minimum", "2700");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "10800 $1");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "7200");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processEllipseRibbon2(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 5400<< 18900);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M ?f12 ?f1 L ?f12 ?f13 C ?f12 ?f14 ?f15 21600 ?f16 21600 L 0 21600 2750 ?f7 0 ?f2 ?f0 ?f2 ?f0 ?f4 C ?f0 ?f5 ?f10 0 ?f11 0 L ?f17 0 C ?f18 0 ?f19 ?f5 ?f19 ?f4 L ?f19 ?f2 21600 ?f2 18850 ?f7 21600 21600 ?f20 21600 C ?f21 21600 ?f22 ?f14 ?f22 ?f13 L ?f22 ?f1 Z N M ?f12 ?f1 L ?f12 ?f13 C ?f12 ?f23 ?f15 ?f24 ?f16 ?f24 L ?f11 ?f24 C ?f10 ?f24 ?f0 ?f26 ?f0 ?f25 ?f0 ?f27 ?f10 ?f1 ?f11 ?f1 Z N M ?f22 ?f1 L ?f22 ?f13 C ?f22 ?f23 ?f21 ?f24 ?f20 ?f24 L ?f17 ?f24 C ?f18 ?f24 ?f19 ?f26 ?f19 ?f25 ?f19 ?f27 ?f18 ?f1 ?f17 ?f1 Z N M ?f0 ?f25 L ?f0 ?f2 N M ?f19 ?f25 L ?f19 ?f2 N");
    out.xml.addAttribute("draw:type", "mso-spt108");
    out.xml.addAttribute("draw:text-areas", "?f0 0 ?f19 ?f1");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","$1 ");
    equation(out,"f2","21600-?f1 ");
    equation(out,"f3","?f2 /2");
    equation(out,"f4","?f3 /2");
    equation(out,"f5","?f4 /2");
    equation(out,"f6","?f1 /2");
    equation(out,"f7","21600-?f6 ");
    equation(out,"f8","420");
    equation(out,"f9","?f8 *2");
    equation(out,"f10","?f0 +?f8 ");
    equation(out,"f11","?f0 +?f9 ");
    equation(out,"f12","?f0 +2700");
    equation(out,"f13","21600-?f4 ");
    equation(out,"f14","21600-?f5 ");
    equation(out,"f15","?f12 -?f8 ");
    equation(out,"f16","?f12 -?f9 ");
    equation(out,"f17","21600-?f11 ");
    equation(out,"f18","21600-?f10 ");
    equation(out,"f19","21600-?f0 ");
    equation(out,"f20","21600-?f16 ");
    equation(out,"f21","21600-?f15 ");
    equation(out,"f22","21600-?f12 ");
    equation(out,"f23","?f13 -?f5 ");
    equation(out,"f24","?f1 +?f3 ");
    equation(out,"f25","?f1 +?f4 ");
    equation(out,"f26","?f25 +?f5 ");
    equation(out,"f27","?f25 -?f5 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-maximum", "8100");
    out.xml.addAttribute("draw:handle-range-x-minimum", "2700");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "10800 $1");
    out.xml.addAttribute("draw:handle-range-y-minimum", "14400");
    out.xml.addAttribute("draw:handle-range-y-maximum", "21600");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processEllipseRibbon(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "?f17 ?f10 2700 ?f14 ?f17 21600 ?f18 ?f14");
    processModifiers(o, out, QList<int>() << 5400<< 2700);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L ?f3 0 X ?f4 ?f11 L ?f4 ?f10 ?f5 ?f10 ?f5 ?f11 Y ?f6 0 L 21600 0 ?f18 ?f14 21600 ?f15 ?f9 ?f15 ?f9 ?f16 Y ?f8 21600 L ?f1 21600 X ?f0 ?f16 L ?f0 ?f15 0 ?f15 2700 ?f14 Z N M ?f4 ?f11 F Y ?f3 ?f12 L ?f1 ?f12 X ?f0 ?f13 ?f1 ?f10 L ?f4 ?f10 N M ?f5 ?f11 F Y ?f6 ?f12 L ?f8 ?f12 X ?f9 ?f13 ?f8 ?f10 L ?f5 ?f10 N M ?f0 ?f13 F L ?f0 ?f15 N M ?f9 ?f13 F L ?f9 ?f15 N");
    out.xml.addAttribute("draw:type", "mso-spt107");
    out.xml.addAttribute("draw:text-areas", "?f0 ?f10 ?f9 21600");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","?f0 +675");
    equation(out,"f2","?f1 +675");
    equation(out,"f3","?f2 +675");
    equation(out,"f4","?f3 +675");
    equation(out,"f5","21600-?f4 ");
    equation(out,"f6","21600-?f3 ");
    equation(out,"f7","21600-?f2 ");
    equation(out,"f8","21600-?f1 ");
    equation(out,"f9","21600-?f0 ");
    equation(out,"f10","$1 ");
    equation(out,"f11","?f10 /4");
    equation(out,"f12","?f11 *2");
    equation(out,"f13","?f11 *3");
    equation(out,"f14","10800-?f12 ");
    equation(out,"f15","21600-?f10 ");
    equation(out,"f16","21600-?f11 ");
    equation(out,"f17","21600/2");
    equation(out,"f18","21600-2700");
    equation(out,"f19","?f17 -2700");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-maximum", "8100");
    out.xml.addAttribute("draw:handle-range-x-minimum", "2700");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "10800 $1");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "7200");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processVerticalScroll(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 2700);
    out.xml.addAttribute("draw:path-stretchpoint-x", "11000");
    out.xml.addAttribute("draw:path-stretchpoint-y", "10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M ?f1 21600 X 0 ?f11 ?f1 ?f12 L ?f0 ?f12 ?f0 ?f1 Y ?f4 0 L ?f2 0 X 21600 ?f1 ?f2 ?f0 L ?f3 ?f0 ?f3 ?f11 Y ?f5 21600 Z N M ?f6 ?f1 Y ?f4 ?f0 X ?f8 ?f9 ?f4 ?f1 Z N M ?f0 ?f11 Y ?f1 21600 0 ?f11 ?f1 ?f12 X ?f9 ?f10 ?f1 ?f11 Z N M ?f4 0 X ?f6 ?f1 N M ?f0 ?f12 L ?f0 ?f11 N M ?f4 ?f0 L ?f2 ?f0 N");
    out.xml.addAttribute("draw:type", "vertical-scroll");
    out.xml.addAttribute("draw:text-areas", "?f0 ?f0 ?f3 ?f12");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","?f0 /2");
    equation(out,"f2","right-?f1 ");
    equation(out,"f3","right-?f0 ");
    equation(out,"f4","?f0 +?f1 ");
    equation(out,"f5","right-?f4 ");
    equation(out,"f6","?f0 *2");
    equation(out,"f7","?f1 /2");
    equation(out,"f8","?f0 +?f7 ");
    equation(out,"f9","?f1 +?f7 ");
    equation(out,"f10","bottom-?f9 ");
    equation(out,"f11","bottom-?f1 ");
    equation(out,"f12","bottom-?f0 ");
    equation(out,"f13","bottom-?f4 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "left $0");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "5400");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processHorizontalScroll(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 2700);
    out.xml.addAttribute("draw:path-stretchpoint-x", "10800");
    out.xml.addAttribute("draw:path-stretchpoint-y", "11000");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 ?f4 Y ?f1 ?f0 L ?f3 ?f0 ?f3 ?f1 Y ?f2 0 21600 ?f1 L 21600 ?f13 Y ?f2 ?f12 L ?f0 ?f12 ?f0 ?f11 Y ?f1 21600 0 ?f11 Z N M ?f1 ?f4 Y ?f9 ?f8 ?f0 ?f4 ?f1 ?f6 Z N M ?f2 ?f1 Y ?f3 ?f9 ?f3 ?f1 ?f2 0 X 21600 ?f1 ?f2 ?f0 Z N M ?f1 ?f6 X 0 ?f4 N M ?f2 ?f0 L ?f3 ?f0 N M ?f0 ?f4 L ?f0 ?f11 N");
    out.xml.addAttribute("draw:type", "horizontal-scroll");
    out.xml.addAttribute("draw:text-areas", "?f0 ?f0 ?f3 ?f12");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","?f0 /2");
    equation(out,"f2","right-?f1 ");
    equation(out,"f3","right-?f0 ");
    equation(out,"f4","?f0 +?f1 ");
    equation(out,"f5","right-?f4 ");
    equation(out,"f6","?f0 *2");
    equation(out,"f7","?f1 /2");
    equation(out,"f8","?f0 +?f7 ");
    equation(out,"f9","?f1 +?f7 ");
    equation(out,"f10","bottom-?f9 ");
    equation(out,"f11","bottom-?f1 ");
    equation(out,"f12","bottom-?f0 ");
    equation(out,"f13","bottom-?f4 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-maximum", "5400");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processWave(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "?f18 ?f0 ?f20 10800 ?f19 ?f1 ?f21 10800");
    processModifiers(o, out, QList<int>() << 1400<< 10800);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M ?f7 ?f0 C ?f15 ?f9 ?f16 ?f10 ?f12 ?f0 L ?f24 ?f1 C ?f25 ?f26 ?f27 ?f28 ?f29 ?f1 Z N");
    out.xml.addAttribute("draw:type", "mso-spt64");
    out.xml.addAttribute("draw:text-areas", "?f5 ?f22 ?f11 ?f23");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","21600-?f0 ");
    equation(out,"f2","$1 ");
    equation(out,"f3","?f2 -10800");
    equation(out,"f4","?f3 *2");
    equation(out,"f5","abs(?f4 )");
    equation(out,"f6","4320-?f5 ");
    equation(out,"f7","if(?f3 ,0,?f5 )");
    equation(out,"f8","15800*?f0 /4460");
    equation(out,"f9","?f0 -?f8 ");
    equation(out,"f10","?f0 +?f8 ");
    equation(out,"f11","21600-?f4 ");
    equation(out,"f12","if(?f3 ,?f11 ,21600)");
    equation(out,"f13","?f12 -?f7 ");
    equation(out,"f14","?f5 /2");
    equation(out,"f15","?f7 +7200-?f14 ");
    equation(out,"f16","?f12 +?f14 -7200");
    equation(out,"f17","?f13 /2");
    equation(out,"f18","?f7 +?f17 ");
    equation(out,"f19","21600-?f18 ");
    equation(out,"f20","?f5 /2");
    equation(out,"f21","21600-?f20 ");
    equation(out,"f22","?f0 *2");
    equation(out,"f23","21600-?f22 ");
    equation(out,"f24","21600-?f7 ");
    equation(out,"f25","21600-?f15 ");
    equation(out,"f26","?f1 +?f8 ");
    equation(out,"f27","21600-?f16 ");
    equation(out,"f28","?f1 -?f8 ");
    equation(out,"f29","21600-?f12 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "left $0");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "4460");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$1 21600");
    out.xml.addAttribute("draw:handle-range-x-maximum", "12960");
    out.xml.addAttribute("draw:handle-range-x-minimum", "8640");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processDoubleWave(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "?f18 ?f0 ?f20 10800 ?f19 ?f1 ?f21 10800");
    processModifiers(o, out, QList<int>() << 1400<< 10800);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M ?f7 ?f0 C ?f15 ?f9 ?f30 ?f10 ?f18 ?f0 ?f31 ?f9 ?f16 ?f10 ?f12 ?f0 L ?f24 ?f1 C ?f25 ?f26 ?f33 ?f28 ?f19 ?f1 ?f32 ?f26 ?f27 ?f28 ?f29 ?f1 Z N");
    out.xml.addAttribute("draw:type", "mso-spt188");
    out.xml.addAttribute("draw:text-areas", "?f5 ?f22 ?f11 ?f23");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","21600-?f0 ");
    equation(out,"f2","$1 ");
    equation(out,"f3","?f2 -10800");
    equation(out,"f4","?f3 *2");
    equation(out,"f5","abs(?f4 )");
    equation(out,"f6","4320-?f5 ");
    equation(out,"f7","if(?f3 ,0,?f5 )");
    equation(out,"f8","7900*?f0 /2230");
    equation(out,"f9","?f0 -?f8 ");
    equation(out,"f10","?f0 +?f8 ");
    equation(out,"f11","21600-?f4 ");
    equation(out,"f12","if(?f3 ,?f11 ,21600)");
    equation(out,"f13","?f12 -?f7 ");
    equation(out,"f14","?f5 /2");
    equation(out,"f15","?f7 +3600-?f14 ");
    equation(out,"f16","?f12 +?f14 -3600");
    equation(out,"f17","?f13 /2");
    equation(out,"f18","?f7 +?f17 ");
    equation(out,"f19","21600-?f18 ");
    equation(out,"f20","?f5 /2");
    equation(out,"f21","21600-?f20 ");
    equation(out,"f22","?f0 *2");
    equation(out,"f23","21600-?f22 ");
    equation(out,"f24","21600-?f7 ");
    equation(out,"f25","21600-?f15 ");
    equation(out,"f26","?f1 +?f8 ");
    equation(out,"f27","21600-?f16 ");
    equation(out,"f28","?f1 -?f8 ");
    equation(out,"f29","21600-?f12 ");
    equation(out,"f30","?f18 -?f14 ");
    equation(out,"f31","?f18 +?f14 ");
    equation(out,"f32","?f19 -?f14 ");
    equation(out,"f33","?f19 +?f14 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "left $0");
    out.xml.addAttribute("draw:handle-range-y-minimum", "0");
    out.xml.addAttribute("draw:handle-range-y-maximum", "2230");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$1 21600");
    out.xml.addAttribute("draw:handle-range-x-maximum", "12960");
    out.xml.addAttribute("draw:handle-range-x-minimum", "8640");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processWedgeRectCallout(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 0 0 10800 10800 21600 21600 10800 ?f40 ?f41");
    processModifiers(o, out, QList<int>() << 1400<< 25920);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L 0 3590 ?f2 ?f3 0 8970 0 12630 ?f4 ?f5 0 18010 0 21600 3590 21600 ?f6 ?f7 8970 21600 12630 21600 ?f8 ?f9 18010 21600 21600 21600 21600 18010 ?f10 ?f11 21600 12630 21600 8970 ?f12 ?f13 21600 3590 21600 0 18010 0 ?f14 ?f15 12630 0 8970 0 ?f16 ?f17 3590 0 0 0 Z N");
    out.xml.addAttribute("draw:type", "rectangular-callout");
    out.xml.addAttribute("draw:text-areas", "0 0 21600 21600");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 -10800");
    equation(out,"f1","$1 -10800");
    equation(out,"f2","if(?f18 ,$0 ,0)");
    equation(out,"f3","if(?f18 ,$1 ,6280)");
    equation(out,"f4","if(?f23 ,$0 ,0)");
    equation(out,"f5","if(?f23 ,$1 ,15320)");
    equation(out,"f6","if(?f26 ,$0 ,6280)");
    equation(out,"f7","if(?f26 ,$1 ,21600)");
    equation(out,"f8","if(?f29 ,$0 ,15320)");
    equation(out,"f9","if(?f29 ,$1 ,21600)");
    equation(out,"f10","if(?f32 ,$0 ,21600)");
    equation(out,"f11","if(?f32 ,$1 ,15320)");
    equation(out,"f12","if(?f34 ,$0 ,21600)");
    equation(out,"f13","if(?f34 ,$1 ,6280)");
    equation(out,"f14","if(?f36 ,$0 ,15320)");
    equation(out,"f15","if(?f36 ,$1 ,0)");
    equation(out,"f16","if(?f38 ,$0 ,6280)");
    equation(out,"f17","if(?f38 ,$1 ,0)");
    equation(out,"f18","if($0 ,-1,?f19 )");
    equation(out,"f19","if(?f1 ,-1,?f22 )");
    equation(out,"f20","abs(?f0 )");
    equation(out,"f21","abs(?f1 )");
    equation(out,"f22","?f20 -?f21 ");
    equation(out,"f23","if($0 ,-1,?f24 )");
    equation(out,"f24","if(?f1 ,?f22 ,-1)");
    equation(out,"f25","$1 -21600");
    equation(out,"f26","if(?f25 ,?f27 ,-1)");
    equation(out,"f27","if(?f0 ,-1,?f28 )");
    equation(out,"f28","?f21 -?f20 ");
    equation(out,"f29","if(?f25 ,?f30 ,-1)");
    equation(out,"f30","if(?f0 ,?f28 ,-1)");
    equation(out,"f31","$0 -21600");
    equation(out,"f32","if(?f31 ,?f33 ,-1)");
    equation(out,"f33","if(?f1 ,?f22 ,-1)");
    equation(out,"f34","if(?f31 ,?f35 ,-1)");
    equation(out,"f35","if(?f1 ,-1,?f22 )");
    equation(out,"f36","if($1 ,-1,?f37 )");
    equation(out,"f37","if(?f0 ,?f28 ,-1)");
    equation(out,"f38","if($1 ,-1,?f39 )");
    equation(out,"f39","if(?f0 ,-1,?f28 )");
    equation(out,"f40","$0 ");
    equation(out,"f41","$1 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 $1");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processWedgeRRectCallout(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 1400<< 25920);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 3590 0 X 0 3590 L ?f2 ?f3 0 8970 0 12630 ?f4 ?f5 0 18010 Y 3590 21600 L ?f6 ?f7 8970 21600 12630 21600 ?f8 ?f9 18010 21600 X 21600 18010 L ?f10 ?f11 21600 12630 21600 8970 ?f12 ?f13 21600 3590 Y 18010 0 L ?f14 ?f15 12630 0 8970 0 ?f16 ?f17 Z N");
    out.xml.addAttribute("draw:type", "round-rectangular-callout");
    out.xml.addAttribute("draw:text-areas", "800 800 20800 20800");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 -10800");
    equation(out,"f1","$1 -10800");
    equation(out,"f2","if(?f18 ,$0 ,0)");
    equation(out,"f3","if(?f18 ,$1 ,6280)");
    equation(out,"f4","if(?f23 ,$0 ,0)");
    equation(out,"f5","if(?f23 ,$1 ,15320)");
    equation(out,"f6","if(?f26 ,$0 ,6280)");
    equation(out,"f7","if(?f26 ,$1 ,21600)");
    equation(out,"f8","if(?f29 ,$0 ,15320)");
    equation(out,"f9","if(?f29 ,$1 ,21600)");
    equation(out,"f10","if(?f32 ,$0 ,21600)");
    equation(out,"f11","if(?f32 ,$1 ,15320)");
    equation(out,"f12","if(?f34 ,$0 ,21600)");
    equation(out,"f13","if(?f34 ,$1 ,6280)");
    equation(out,"f14","if(?f36 ,$0 ,15320)");
    equation(out,"f15","if(?f36 ,$1 ,0)");
    equation(out,"f16","if(?f38 ,$0 ,6280)");
    equation(out,"f17","if(?f38 ,$1 ,0)");
    equation(out,"f18","if($0 ,-1,?f19 )");
    equation(out,"f19","if(?f1 ,-1,?f22 )");
    equation(out,"f20","abs(?f0 )");
    equation(out,"f21","abs(?f1 )");
    equation(out,"f22","?f20 -?f21 ");
    equation(out,"f23","if($0 ,-1,?f24 )");
    equation(out,"f24","if(?f1 ,?f22 ,-1)");
    equation(out,"f25","$1 -21600");
    equation(out,"f26","if(?f25 ,?f27 ,-1)");
    equation(out,"f27","if(?f0 ,-1,?f28 )");
    equation(out,"f28","?f21 -?f20 ");
    equation(out,"f29","if(?f25 ,?f30 ,-1)");
    equation(out,"f30","if(?f0 ,?f28 ,-1)");
    equation(out,"f31","$0 -21600");
    equation(out,"f32","if(?f31 ,?f33 ,-1)");
    equation(out,"f33","if(?f1 ,?f22 ,-1)");
    equation(out,"f34","if(?f31 ,?f35 ,-1)");
    equation(out,"f35","if(?f1 ,-1,?f22 )");
    equation(out,"f36","if($1 ,-1,?f37 )");
    equation(out,"f37","if(?f0 ,?f28 ,-1)");
    equation(out,"f38","if($1 ,-1,?f39 )");
    equation(out,"f39","if(?f0 ,-1,?f28 )");
    equation(out,"f40","$0 ");
    equation(out,"f41","$1 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 $1");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processWedgeEllipseCallout(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    out.xml.addAttribute("draw:glue-points", "10800 0 3160 3160 0 10800 3160 18440 10800 21600 18440 18440 21600 10800 18440 3160 ?f14 ?f15");
    processModifiers(o, out, QList<int>() << 1350<< 25920);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "V 0 0 21600 21600 ?f22 ?f23 ?f18 ?f19 L ?f14 ?f15 Z N");
    out.xml.addAttribute("draw:type", "round-callout");
    out.xml.addAttribute("draw:text-areas", "3200 3200 18400 18400");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 -10800");
    equation(out,"f1","$1 -10800");
    equation(out,"f2","?f0 *?f0 ");
    equation(out,"f3","?f1 *?f1 ");
    equation(out,"f4","?f2 +?f3 ");
    equation(out,"f5","sqrt(?f4 )");
    equation(out,"f6","?f5 -10800");
    equation(out,"f7","atan2(?f1 ,?f0 )/(pi/180)");
    equation(out,"f8","?f7 -10");
    equation(out,"f9","?f7 +10");
    equation(out,"f10","10800*cos(?f7 *(pi/180))");
    equation(out,"f11","10800*sin(?f7 *(pi/180))");
    equation(out,"f12","?f10 +10800");
    equation(out,"f13","?f11 +10800");
    equation(out,"f14","if(?f6 ,$0 ,?f12 )");
    equation(out,"f15","if(?f6 ,$1 ,?f13 )");
    equation(out,"f16","10800*cos(?f8 *(pi/180))");
    equation(out,"f17","10800*sin(?f8 *(pi/180))");
    equation(out,"f18","?f16 +10800");
    equation(out,"f19","?f17 +10800");
    equation(out,"f20","10800*cos(?f9 *(pi/180))");
    equation(out,"f21","10800*sin(?f9 *(pi/180))");
    equation(out,"f22","?f20 +10800");
    equation(out,"f23","?f21 +10800");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 $1");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processCloudCallout(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 1350<< 25920);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 1930 7160 C 1530 4490 3400 1970 5270 1970 5860 1950 6470 2210 6970 2600 7450 1390 8340 650 9340 650 10004 690 10710 1050 11210 1700 11570 630 12330 0 13150 0 13840 0 14470 460 14870 1160 15330 440 16020 0 16740 0 17910 0 18900 1130 19110 2710 20240 3150 21060 4580 21060 6220 21060 6720 21000 7200 20830 7660 21310 8460 21600 9450 21600 10460 21600 12750 20310 14680 18650 15010 18650 17200 17370 18920 15770 18920 15220 18920 14700 18710 14240 18310 13820 20240 12490 21600 11000 21600 9890 21600 8840 20790 8210 19510 7620 20000 7930 20290 6240 20290 4850 20290 3570 19280 2900 17640 1300 17600 480 16300 480 14660 480 13900 690 13210 1070 12640 380 12160 0 11210 0 10120 0 8590 840 7330 1930 7160 Z N M 1930 7160 C 1950 7410 2040 7690 2090 7920 F N M 6970 2600 C 7200 2790 7480 3050 7670 3310 F N M 11210 1700 C 11130 1910 11080 2160 11030 2400 F N M 14870 1160 C 14720 1400 14640 1720 14540 2010 F N M 19110 2710 C 19130 2890 19230 3290 19190 3380 F N M 20830 7660 C 20660 8170 20430 8620 20110 8990 F N M 18660 15010 C 18740 14200 18280 12200 17000 11450 F N M 14240 18310 C 14320 17980 14350 17680 14370 17360 F N M 8220 19510 C 8060 19250 7960 18950 7860 18640 F N M 2900 17640 C 3090 17600 3280 17540 3460 17450 F N M 1070 12640 C 1400 12900 1780 13130 2330 13040 F N U ?f17 ?f18 1800 1800 0 360 Z N U ?f19 ?f20 1200 1200 0 360 Z N U ?f13 ?f14 700 700 0 360 Z N");
    out.xml.addAttribute("draw:type", "cloud-callout");
    out.xml.addAttribute("draw:text-areas", "3000 3320 17110 17330");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 -10800");
    equation(out,"f1","$1 -10800");
    equation(out,"f2","atan2(?f1 ,?f0 )/(pi/180)");
    equation(out,"f3","10800*cos(?f2 *(pi/180))");
    equation(out,"f4","10800*sin(?f2 *(pi/180))");
    equation(out,"f5","?f3 +10800");
    equation(out,"f6","?f4 +10800");
    equation(out,"f7","$0 -?f5 ");
    equation(out,"f8","$1 -?f6 ");
    equation(out,"f9","?f7 /3");
    equation(out,"f10","?f8 /3");
    equation(out,"f11","?f7 *2/3");
    equation(out,"f12","?f8 *2/3");
    equation(out,"f13","$0 ");
    equation(out,"f14","$1 ");
    equation(out,"f15","?f3 /12");
    equation(out,"f16","?f4 /12");
    equation(out,"f17","?f9 +?f5 -?f15 ");
    equation(out,"f18","?f10 +?f6 -?f16 ");
    equation(out,"f19","?f11 +?f5 ");
    equation(out,"f20","?f12 +?f6 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 $1");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processBorderCallout90(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << -1800<< 22950<< -1800<< 2700);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L 21600 0 21600 21600 0 21600 Z N M ?f0 ?f1 L ?f2 ?f3 N");
    out.xml.addAttribute("draw:type", "mso-spt180");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","$1 ");
    equation(out,"f2","$2 ");
    equation(out,"f3","$3 ");
    equation(out,"f4","$4 ");
    equation(out,"f5","$5 ");
    equation(out,"f6","$6 ");
    equation(out,"f7","$7 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 $1");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$2 $3");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processBorderCallout1(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << -8288<< 24500<< -1800<< 4000);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L 21600 0 21600 21600 0 21600 Z N M ?f0 ?f1 L ?f2 ?f3 N");
    out.xml.addAttribute("draw:type", "line-callout-1");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","$1 ");
    equation(out,"f2","$2 ");
    equation(out,"f3","$3 ");
    equation(out,"f4","$4 ");
    equation(out,"f5","$5 ");
    equation(out,"f6","$6 ");
    equation(out,"f7","$7 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 $1");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$2 $3");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processBorderCallout2(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << -10088<< 24500<< -3600<< 4000<< -1800<< 4000);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L 21600 0 21600 21600 0 21600 Z N M ?f0 ?f1 L ?f2 ?f3 N M ?f2 ?f3 L ?f4 ?f5 N M");
    out.xml.addAttribute("draw:type", "line-callout-2");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","$1 ");
    equation(out,"f2","$2 ");
    equation(out,"f3","$3 ");
    equation(out,"f4","$4 ");
    equation(out,"f5","$5 ");
    equation(out,"f6","$6 ");
    equation(out,"f7","$7 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 $1");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$2 $3");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$4 $5");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processBorderCallout3(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 23400<< 24413<< 25200<< 21600<< 25200<< 4000<< 23400<< 4000);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L 21600 0 21600 21600 0 21600 Z N M ?f6 ?f7 F L ?f4 ?f5 ?f2 ?f3 ?f0 ?f1 N");
    out.xml.addAttribute("draw:type", "mso-spt49");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","$1 ");
    equation(out,"f2","$2 ");
    equation(out,"f3","$3 ");
    equation(out,"f4","$4 ");
    equation(out,"f5","$5 ");
    equation(out,"f6","$6 ");
    equation(out,"f7","$7 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 $1");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$2 $3");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$4 $5");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$6 $7");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processAccentCallout90(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << -1800<< 22950<< -1800<< 2700);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 S L 21600 0 21600 21600 0 21600 Z N M ?f0 ?f1 L ?f2 ?f3 N");
    out.xml.addAttribute("draw:type", "mso-spt179");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","$1 ");
    equation(out,"f2","$2 ");
    equation(out,"f3","$3 ");
    equation(out,"f4","$4 ");
    equation(out,"f5","$5 ");
    equation(out,"f6","$6 ");
    equation(out,"f7","$7 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 $1");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$2 $3");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processAccentCallout1(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << -8288<< 24500<< -1800<< 4000);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 S L 21600 0 21600 21600 0 21600 Z N M ?f0 ?f1 L ?f2 ?f3 N M ?f2 0 L ?f2 21600 N");
    out.xml.addAttribute("draw:type", "mso-spt44");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","$1 ");
    equation(out,"f2","$2 ");
    equation(out,"f3","$3 ");
    equation(out,"f4","$4 ");
    equation(out,"f5","$5 ");
    equation(out,"f6","$6 ");
    equation(out,"f7","$7 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 $1");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$2 $3");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processAccentCallout2(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << -10088<< 24500<< -3600<< 4000<< -1800<< 4000);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 S L 21600 0 21600 21600 0 21600 Z N M ?f0 ?f1 L ?f2 ?f3 N M ?f2 ?f3 L ?f4 ?f5 N M ?f4 0 L ?f4 21600 N M");
    out.xml.addAttribute("draw:type", "mso-spt45");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","$1 ");
    equation(out,"f2","$2 ");
    equation(out,"f3","$3 ");
    equation(out,"f4","$4 ");
    equation(out,"f5","$5 ");
    equation(out,"f6","$6 ");
    equation(out,"f7","$7 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 $1");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$2 $3");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$4 $5");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processAccentCallout3(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 23400<< 24413<< 25200<< 21600<< 25200<< 4000<< 23400<< 4000);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 S L 21600 0 21600 21600 0 21600 Z N M ?f6 ?f7 F L ?f4 ?f5 ?f2 ?f3 ?f0 ?f1 N M ?f6 0 L ?f6 21600 N");
    out.xml.addAttribute("draw:type", "mso-spt46");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","$1 ");
    equation(out,"f2","$2 ");
    equation(out,"f3","$3 ");
    equation(out,"f4","$4 ");
    equation(out,"f5","$5 ");
    equation(out,"f6","$6 ");
    equation(out,"f7","$7 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 $1");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$2 $3");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$4 $5");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$6 $7");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processCallout90(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << -1800<< 22950<< -1800<< 2700);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 S L 21600 0 21600 21600 0 21600 Z N M ?f0 ?f1 L ?f2 ?f3 N");
    out.xml.addAttribute("draw:type", "mso-spt178");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","$1 ");
    equation(out,"f2","$2 ");
    equation(out,"f3","$3 ");
    equation(out,"f4","$4 ");
    equation(out,"f5","$5 ");
    equation(out,"f6","$6 ");
    equation(out,"f7","$7 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 $1");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$2 $3");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processCallout1(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << -8288<< 24500<< -1800<< 4000);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 S L 21600 0 21600 21600 0 21600 Z N M ?f0 ?f1 L ?f2 ?f3 N");
    out.xml.addAttribute("draw:type", "mso-spt41");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","$1 ");
    equation(out,"f2","$2 ");
    equation(out,"f3","$3 ");
    equation(out,"f4","$4 ");
    equation(out,"f5","$5 ");
    equation(out,"f6","$6 ");
    equation(out,"f7","$7 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 $1");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$2 $3");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processCallout2(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << -10088<< 24500<< -3600<< 4000<< -1800<< 4000);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 S L 21600 0 21600 21600 0 21600 Z N M ?f0 ?f1 L ?f2 ?f3 N M ?f2 ?f3 L ?f4 ?f5 N M");
    out.xml.addAttribute("draw:type", "mso-spt42");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","$1 ");
    equation(out,"f2","$2 ");
    equation(out,"f3","$3 ");
    equation(out,"f4","$4 ");
    equation(out,"f5","$5 ");
    equation(out,"f6","$6 ");
    equation(out,"f7","$7 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 $1");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$2 $3");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$4 $5");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processCallout3(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 23400<< 24413<< 25200<< 21600<< 25200<< 4000<< 23400<< 4000);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 S L 21600 0 21600 21600 0 21600 Z N M ?f6 ?f7 F L ?f4 ?f5 ?f2 ?f3 ?f0 ?f1 N");
    out.xml.addAttribute("draw:type", "mso-spt43");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","$1 ");
    equation(out,"f2","$2 ");
    equation(out,"f3","$3 ");
    equation(out,"f4","$4 ");
    equation(out,"f5","$5 ");
    equation(out,"f6","$6 ");
    equation(out,"f7","$7 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 $1");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$2 $3");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$4 $5");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$6 $7");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processAccentBorderCallout90(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << -1800<< 22950<< -1800<< 2700);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L 21600 0 21600 21600 0 21600 Z N M ?f0 ?f1 L ?f2 ?f3 N");
    out.xml.addAttribute("draw:type", "line-callout-3");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","$1 ");
    equation(out,"f2","$2 ");
    equation(out,"f3","$3 ");
    equation(out,"f4","$4 ");
    equation(out,"f5","$5 ");
    equation(out,"f6","$6 ");
    equation(out,"f7","$7 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 $1");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$2 $3");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processAccentBorderCallout1(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << -8288<< 24500<< -1800<< 4000);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L 21600 0 21600 21600 0 21600 Z N M ?f0 ?f1 L ?f2 ?f3 N M ?f2 0 L ?f2 21600 N");
    out.xml.addAttribute("draw:type", "mso-spt50");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","$1 ");
    equation(out,"f2","$2 ");
    equation(out,"f3","$3 ");
    equation(out,"f4","$4 ");
    equation(out,"f5","$5 ");
    equation(out,"f6","$6 ");
    equation(out,"f7","$7 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 $1");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$2 $3");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processAccentBorderCallout2(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << -10088<< 24500<< -3600<< 4000<< -1800<< 4000);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L 21600 0 21600 21600 0 21600 Z N M ?f0 ?f1 L ?f2 ?f3 N M ?f2 ?f3 L ?f4 ?f5 N M ?f4 0 L ?f4 21600 N M");
    out.xml.addAttribute("draw:type", "mso-spt51");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","$1 ");
    equation(out,"f2","$2 ");
    equation(out,"f3","$3 ");
    equation(out,"f4","$4 ");
    equation(out,"f5","$5 ");
    equation(out,"f6","$6 ");
    equation(out,"f7","$7 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 $1");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$2 $3");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$4 $5");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processAccentBorderCallout3(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 23400<< 24413<< 25200<< 21600<< 25200<< 4000<< 23400<< 4000);
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L 21600 0 21600 21600 0 21600 Z N M ?f6 ?f7 F L ?f4 ?f5 ?f2 ?f3 ?f0 ?f1 N M ?f6 0 L ?f6 21600 N");
    out.xml.addAttribute("draw:type", "mso-spt52");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","$1 ");
    equation(out,"f2","$2 ");
    equation(out,"f3","$3 ");
    equation(out,"f4","$4 ");
    equation(out,"f5","$5 ");
    equation(out,"f6","$6 ");
    equation(out,"f7","$7 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 $1");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$2 $3");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$4 $5");
    out.xml.endElement(); // draw:handle
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$6 $7");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processActionButtonBlank(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 1400);
    out.xml.addAttribute("draw:path-stretchpoint-x", "10800");
    out.xml.addAttribute("draw:path-stretchpoint-y", "10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L 21600 0 21600 21600 0 21600 Z N M 0 0 L 21600 0 ?f1 ?f0 ?f0 ?f0 Z N M 21600 0 L 21600 21600 ?f1 ?f2 ?f1 ?f0 Z N M 21600 21600 L 0 21600 ?f0 ?f2 ?f1 ?f2 Z N M 0 21600 L 0 0 ?f0 ?f0 ?f0 ?f2 Z N");
    out.xml.addAttribute("draw:type", "mso-spt189");
    out.xml.addAttribute("draw:text-areas", "?f0 ?f0 ?f1 ?f2");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 *21599/21600");
    equation(out,"f1","right-?f0 ");
    equation(out,"f2","bottom-?f0 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-maximum", "5400");
    out.xml.addAttribute("draw:handle-switched", "true");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processActionButtonHome(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 1400);
    out.xml.addAttribute("draw:path-stretchpoint-x", "10800");
    out.xml.addAttribute("draw:path-stretchpoint-y", "10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L 21600 0 21600 21600 0 21600 Z N M 0 0 L 21600 0 ?f3 ?f2 ?f1 ?f2 Z N M 21600 0 L 21600 21600 ?f3 ?f4 ?f3 ?f2 Z N M 21600 21600 L 0 21600 ?f1 ?f4 ?f3 ?f4 Z N M 0 21600 L 0 0 ?f1 ?f2 ?f1 ?f4 Z N M ?f7 ?f10 L ?f12 ?f14 ?f12 ?f16 ?f18 ?f16 ?f18 ?f20 ?f22 ?f8 ?f24 ?f8 ?f24 ?f26 ?f28 ?f26 ?f28 ?f8 ?f30 ?f8 Z N M ?f12 ?f14 L ?f12 ?f16 ?f18 ?f16 ?f18 ?f20 Z N M ?f32 ?f36 L ?f34 ?f36 ?f34 ?f26 ?f24 ?f26 ?f24 ?f8 ?f28 ?f8 ?f28 ?f26 ?f32 ?f26 Z N");
    out.xml.addAttribute("draw:type", "mso-spt190");
    out.xml.addAttribute("draw:text-areas", "?f1 ?f2 ?f3 ?f4");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","left+$0 ");
    equation(out,"f2","top+$0 ");
    equation(out,"f3","right-$0 ");
    equation(out,"f4","bottom-$0 ");
    equation(out,"f5","10800-$0 ");
    equation(out,"f6","?f5 /10800");
    equation(out,"f7","right/2");
    equation(out,"f8","bottom/2");
    equation(out,"f9","-8000*?f6 ");
    equation(out,"f10","?f9 +?f8 ");
    equation(out,"f11","2960*?f6 ");
    equation(out,"f12","?f11 +?f7 ");
    equation(out,"f13","-5000*?f6 ");
    equation(out,"f14","?f13 +?f8 ");
    equation(out,"f15","-7000*?f6 ");
    equation(out,"f16","?f15 +?f8 ");
    equation(out,"f17","5000*?f6 ");
    equation(out,"f18","?f17 +?f7 ");
    equation(out,"f19","-2960*?f6 ");
    equation(out,"f20","?f19 +?f8 ");
    equation(out,"f21","8000*?f6 ");
    equation(out,"f22","?f21 +?f7 ");
    equation(out,"f23","6100*?f6 ");
    equation(out,"f24","?f23 +?f7 ");
    equation(out,"f25","8260*?f6 ");
    equation(out,"f26","?f25 +?f8 ");
    equation(out,"f27","-6100*?f6 ");
    equation(out,"f28","?f27 +?f7 ");
    equation(out,"f29","-8000*?f6 ");
    equation(out,"f30","?f29 +?f7 ");
    equation(out,"f31","-1060*?f6 ");
    equation(out,"f32","?f31 +?f7 ");
    equation(out,"f33","1060*?f6 ");
    equation(out,"f34","?f33 +?f7 ");
    equation(out,"f35","4020*?f6 ");
    equation(out,"f36","?f35 +?f8 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-maximum", "5400");
    out.xml.addAttribute("draw:handle-switched", "true");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processActionButtonHelp(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 1400);
    out.xml.addAttribute("draw:path-stretchpoint-x", "10800");
    out.xml.addAttribute("draw:path-stretchpoint-y", "10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L 21600 0 21600 21600 0 21600 Z N M 0 0 L 21600 0 ?f3 ?f2 ?f1 ?f2 Z N M 21600 0 L 21600 21600 ?f3 ?f4 ?f3 ?f2 Z N M 21600 21600 L 0 21600 ?f1 ?f4 ?f3 ?f4 Z N M 0 21600 L 0 0 ?f1 ?f2 ?f1 ?f4 Z N M ?f7 ?f12 X ?f10 ?f62 ?f7 ?f16 ?f14 ?f62 ?f7 ?f12 Z N M ?f18 ?f20 L ?f18 ?f22 C ?f18 ?f24 ?f26 ?f8 ?f28 ?f8 ?f30 ?f8 ?f32 ?f34 ?f32 ?f36 ?f32 ?f38 ?f40 ?f42 ?f7 ?f42 ?f44 ?f42 ?f46 ?f38 ?f46 ?f36 L ?f48 ?f36 C ?f48 ?f50 ?f52 ?f54 ?f7 ?f54 ?f18 ?f54 ?f28 ?f50 ?f28 ?f36 ?f28 ?f56 ?f58 ?f60 ?f18 ?f60 ?f7 ?f60 ?f52 ?f8 ?f52 ?f22 L ?f52 ?f20 Z N");
    out.xml.addAttribute("draw:type", "mso-spt191");
    out.xml.addAttribute("draw:text-areas", "?f1 ?f2 ?f3 ?f4");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","left+$0 ");
    equation(out,"f2","top+$0 ");
    equation(out,"f3","right-$0 ");
    equation(out,"f4","bottom-$0 ");
    equation(out,"f5","10800-$0 ");
    equation(out,"f6","?f5 /10800");
    equation(out,"f7","right/2");
    equation(out,"f8","bottom/2");
    equation(out,"f9","-1690*?f6 ");
    equation(out,"f10","?f9 +?f7 ");
    equation(out,"f11","4600*?f6 ");
    equation(out,"f12","?f11 +?f8 ");
    equation(out,"f13","1690*?f6 ");
    equation(out,"f14","?f13 +?f7 ");
    equation(out,"f15","7980*?f6 ");
    equation(out,"f16","?f15 +?f8 ");
    equation(out,"f17","1270*?f6 ");
    equation(out,"f18","?f17 +?f7 ");
    equation(out,"f19","4000*?f6 ");
    equation(out,"f20","?f19 +?f8 ");
    equation(out,"f21","1750*?f6 ");
    equation(out,"f22","?f21 +?f8 ");
    equation(out,"f23","800*?f6 ");
    equation(out,"f24","?f23 +?f8 ");
    equation(out,"f25","1650*?f6 ");
    equation(out,"f26","?f25 +?f7 ");
    equation(out,"f27","2340*?f6 ");
    equation(out,"f28","?f27 +?f7 ");
    equation(out,"f29","3640*?f6 ");
    equation(out,"f30","?f29 +?f7 ");
    equation(out,"f31","4670*?f6 ");
    equation(out,"f32","?f31 +?f7 ");
    equation(out,"f33","-1570*?f6 ");
    equation(out,"f34","?f33 +?f8 ");
    equation(out,"f35","-3390*?f6 ");
    equation(out,"f36","?f35 +?f8 ");
    equation(out,"f37","-6050*?f6 ");
    equation(out,"f38","?f37 +?f8 ");
    equation(out,"f39","2540*?f6 ");
    equation(out,"f40","?f39 +?f7 ");
    equation(out,"f41","-8050*?f6 ");
    equation(out,"f42","?f41 +?f8 ");
    equation(out,"f43","-2540*?f6 ");
    equation(out,"f44","?f43 +?f7 ");
    equation(out,"f45","-4460*?f6 ");
    equation(out,"f46","?f45 +?f7 ");
    equation(out,"f47","-2330*?f6 ");
    equation(out,"f48","?f47 +?f7 ");
    equation(out,"f49","-4700*?f6 ");
    equation(out,"f50","?f49 +?f8 ");
    equation(out,"f51","-1270*?f6 ");
    equation(out,"f52","?f51 +?f7 ");
    equation(out,"f53","-5720*?f6 ");
    equation(out,"f54","?f53 +?f8 ");
    equation(out,"f55","-2540*?f6 ");
    equation(out,"f56","?f55 +?f8 ");
    equation(out,"f57","1800*?f6 ");
    equation(out,"f58","?f57 +?f7 ");
    equation(out,"f59","-1700*?f6 ");
    equation(out,"f60","?f59 +?f8 ");
    equation(out,"f61","6290*?f6 ");
    equation(out,"f62","?f61 +?f8 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-maximum", "5400");
    out.xml.addAttribute("draw:handle-switched", "true");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processActionButtonInformation(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 1400);
    out.xml.addAttribute("draw:path-stretchpoint-x", "10800");
    out.xml.addAttribute("draw:path-stretchpoint-y", "10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L 21600 0 21600 21600 0 21600 Z N M 0 0 L 21600 0 ?f3 ?f2 ?f1 ?f2 Z N M 21600 0 L 21600 21600 ?f3 ?f4 ?f3 ?f2 Z N M 21600 21600 L 0 21600 ?f1 ?f4 ?f3 ?f4 Z N M 0 21600 L 0 0 ?f1 ?f2 ?f1 ?f4 Z N M ?f7 ?f12 X ?f10 ?f8 ?f7 ?f16 ?f14 ?f8 ?f7 ?f12 Z N M ?f7 ?f20 X ?f18 ?f42 ?f7 ?f24 ?f22 ?f42 ?f7 ?f20 Z N M ?f26 ?f28 L ?f30 ?f28 ?f30 ?f32 ?f34 ?f32 ?f34 ?f36 ?f26 ?f36 ?f26 ?f32 ?f38 ?f32 ?f38 ?f40 ?f26 ?f40 Z N");
    out.xml.addAttribute("draw:type", "mso-spt192");
    out.xml.addAttribute("draw:text-areas", "?f1 ?f2 ?f3 ?f4");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","left+$0 ");
    equation(out,"f2","top+$0 ");
    equation(out,"f3","right-$0 ");
    equation(out,"f4","bottom-$0 ");
    equation(out,"f5","10800-$0 ");
    equation(out,"f6","?f5 /10800");
    equation(out,"f7","right/2");
    equation(out,"f8","bottom/2");
    equation(out,"f9","-8050*?f6 ");
    equation(out,"f10","?f9 +?f7 ");
    equation(out,"f11","-8050*?f6 ");
    equation(out,"f12","?f11 +?f8 ");
    equation(out,"f13","8050*?f6 ");
    equation(out,"f14","?f13 +?f7 ");
    equation(out,"f15","8050*?f6 ");
    equation(out,"f16","?f15 +?f8 ");
    equation(out,"f17","-2060*?f6 ");
    equation(out,"f18","?f17 +?f7 ");
    equation(out,"f19","-7620*?f6 ");
    equation(out,"f20","?f19 +?f8 ");
    equation(out,"f21","2060*?f6 ");
    equation(out,"f22","?f21 +?f7 ");
    equation(out,"f23","-3500*?f6 ");
    equation(out,"f24","?f23 +?f8 ");
    equation(out,"f25","-2960*?f6 ");
    equation(out,"f26","?f25 +?f7 ");
    equation(out,"f27","-2960*?f6 ");
    equation(out,"f28","?f27 +?f8 ");
    equation(out,"f29","1480*?f6 ");
    equation(out,"f30","?f29 +?f7 ");
    equation(out,"f31","5080*?f6 ");
    equation(out,"f32","?f31 +?f8 ");
    equation(out,"f33","2960*?f6 ");
    equation(out,"f34","?f33 +?f7 ");
    equation(out,"f35","6140*?f6 ");
    equation(out,"f36","?f35 +?f8 ");
    equation(out,"f37","-1480*?f6 ");
    equation(out,"f38","?f37 +?f7 ");
    equation(out,"f39","-1920*?f6 ");
    equation(out,"f40","?f39 +?f8 ");
    equation(out,"f41","-5560*?f6 ");
    equation(out,"f42","?f41 +?f8 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-maximum", "5400");
    out.xml.addAttribute("draw:handle-switched", "true");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processActionButtonBackPrevious(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 1400);
    out.xml.addAttribute("draw:path-stretchpoint-x", "10800");
    out.xml.addAttribute("draw:path-stretchpoint-y", "10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L 21600 0 21600 21600 0 21600 Z N M 0 0 L 21600 0 ?f3 ?f2 ?f1 ?f2 Z N M 21600 0 L 21600 21600 ?f3 ?f4 ?f3 ?f2 Z N M 21600 21600 L 0 21600 ?f1 ?f4 ?f3 ?f4 Z N M 0 21600 L 0 0 ?f1 ?f2 ?f1 ?f4 Z N M ?f10 ?f8 L ?f14 ?f12 ?f14 ?f16 Z N");
    out.xml.addAttribute("draw:type", "mso-spt194");
    out.xml.addAttribute("draw:text-areas", "?f1 ?f2 ?f3 ?f4");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","left+$0 ");
    equation(out,"f2","top+$0 ");
    equation(out,"f3","right-$0 ");
    equation(out,"f4","bottom-$0 ");
    equation(out,"f5","10800-$0 ");
    equation(out,"f6","?f5 /10800");
    equation(out,"f7","right/2");
    equation(out,"f8","bottom/2");
    equation(out,"f9","-8050*?f6 ");
    equation(out,"f10","?f9 +?f7 ");
    equation(out,"f11","-8050*?f6 ");
    equation(out,"f12","?f11 +?f8 ");
    equation(out,"f13","8050*?f6 ");
    equation(out,"f14","?f13 +?f7 ");
    equation(out,"f15","8050*?f6 ");
    equation(out,"f16","?f15 +?f8 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-maximum", "5400");
    out.xml.addAttribute("draw:handle-switched", "true");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processActionButtonForwardNext(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 1400);
    out.xml.addAttribute("draw:path-stretchpoint-x", "10800");
    out.xml.addAttribute("draw:path-stretchpoint-y", "10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L 21600 0 21600 21600 0 21600 Z N M 0 0 L 21600 0 ?f3 ?f2 ?f1 ?f2 Z N M 21600 0 L 21600 21600 ?f3 ?f4 ?f3 ?f2 Z N M 21600 21600 L 0 21600 ?f1 ?f4 ?f3 ?f4 Z N M 0 21600 L 0 0 ?f1 ?f2 ?f1 ?f4 Z N M ?f10 ?f12 L ?f14 ?f8 ?f10 ?f16 Z N");
    out.xml.addAttribute("draw:type", "mso-spt193");
    out.xml.addAttribute("draw:text-areas", "?f1 ?f2 ?f3 ?f4");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","left+$0 ");
    equation(out,"f2","top+$0 ");
    equation(out,"f3","right-$0 ");
    equation(out,"f4","bottom-$0 ");
    equation(out,"f5","10800-$0 ");
    equation(out,"f6","?f5 /10800");
    equation(out,"f7","right/2");
    equation(out,"f8","bottom/2");
    equation(out,"f9","-8050*?f6 ");
    equation(out,"f10","?f9 +?f7 ");
    equation(out,"f11","-8050*?f6 ");
    equation(out,"f12","?f11 +?f8 ");
    equation(out,"f13","8050*?f6 ");
    equation(out,"f14","?f13 +?f7 ");
    equation(out,"f15","8050*?f6 ");
    equation(out,"f16","?f15 +?f8 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-maximum", "5400");
    out.xml.addAttribute("draw:handle-switched", "true");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processActionButtonBeginning(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 1400);
    out.xml.addAttribute("draw:path-stretchpoint-x", "10800");
    out.xml.addAttribute("draw:path-stretchpoint-y", "10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L 21600 0 21600 21600 0 21600 Z N M 0 0 L 21600 0 ?f3 ?f2 ?f1 ?f2 Z N M 21600 0 L 21600 21600 ?f3 ?f4 ?f3 ?f2 Z N M 21600 21600 L 0 21600 ?f1 ?f4 ?f3 ?f4 Z N M 0 21600 L 0 0 ?f1 ?f2 ?f1 ?f4 Z N M ?f10 ?f8 L ?f14 ?f12 ?f14 ?f16 Z N M ?f18 ?f12 L ?f20 ?f12 ?f20 ?f16 ?f18 ?f16 Z N");
    out.xml.addAttribute("draw:type", "mso-spt196");
    out.xml.addAttribute("draw:text-areas", "?f1 ?f2 ?f3 ?f4");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","left+$0 ");
    equation(out,"f2","top+$0 ");
    equation(out,"f3","right-$0 ");
    equation(out,"f4","bottom-$0 ");
    equation(out,"f5","10800-$0 ");
    equation(out,"f6","?f5 /10800");
    equation(out,"f7","right/2");
    equation(out,"f8","bottom/2");
    equation(out,"f9","-4020*?f6 ");
    equation(out,"f10","?f9 +?f7 ");
    equation(out,"f11","-8050*?f6 ");
    equation(out,"f12","?f11 +?f8 ");
    equation(out,"f13","8050*?f6 ");
    equation(out,"f14","?f13 +?f7 ");
    equation(out,"f15","8050*?f6 ");
    equation(out,"f16","?f15 +?f8 ");
    equation(out,"f17","-8050*?f6 ");
    equation(out,"f18","?f17 +?f7 ");
    equation(out,"f19","-6140*?f6 ");
    equation(out,"f20","?f19 +?f7 ");
    equation(out,"f21","4020*?f6 ");
    equation(out,"f22","?f21 +?f7 ");
    equation(out,"f23","6140*?f6 ");
    equation(out,"f24","?f23 +?f7 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-maximum", "5400");
    out.xml.addAttribute("draw:handle-switched", "true");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processActionButtonEnd(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 1400);
    out.xml.addAttribute("draw:path-stretchpoint-x", "10800");
    out.xml.addAttribute("draw:path-stretchpoint-y", "10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L 21600 0 21600 21600 0 21600 Z N M 0 0 L 21600 0 ?f3 ?f2 ?f1 ?f2 Z N M 21600 0 L 21600 21600 ?f3 ?f4 ?f3 ?f2 Z N M 21600 21600 L 0 21600 ?f1 ?f4 ?f3 ?f4 Z N M 0 21600 L 0 0 ?f1 ?f2 ?f1 ?f4 Z N M ?f22 ?f8 L ?f18 ?f16 ?f18 ?f12 Z N M ?f24 ?f12 L ?f24 ?f16 ?f14 ?f16 ?f14 ?f12 Z N");
    out.xml.addAttribute("draw:type", "mso-spt195");
    out.xml.addAttribute("draw:text-areas", "?f1 ?f2 ?f3 ?f4");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","left+$0 ");
    equation(out,"f2","top+$0 ");
    equation(out,"f3","right-$0 ");
    equation(out,"f4","bottom-$0 ");
    equation(out,"f5","10800-$0 ");
    equation(out,"f6","?f5 /10800");
    equation(out,"f7","right/2");
    equation(out,"f8","bottom/2");
    equation(out,"f9","-4020*?f6 ");
    equation(out,"f10","?f9 +?f7 ");
    equation(out,"f11","-8050*?f6 ");
    equation(out,"f12","?f11 +?f8 ");
    equation(out,"f13","8050*?f6 ");
    equation(out,"f14","?f13 +?f7 ");
    equation(out,"f15","8050*?f6 ");
    equation(out,"f16","?f15 +?f8 ");
    equation(out,"f17","-8050*?f6 ");
    equation(out,"f18","?f17 +?f7 ");
    equation(out,"f19","-6140*?f6 ");
    equation(out,"f20","?f19 +?f7 ");
    equation(out,"f21","4020*?f6 ");
    equation(out,"f22","?f21 +?f7 ");
    equation(out,"f23","6140*?f6 ");
    equation(out,"f24","?f23 +?f7 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-maximum", "5400");
    out.xml.addAttribute("draw:handle-switched", "true");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processActionButtonReturn(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 1400);
    out.xml.addAttribute("draw:path-stretchpoint-x", "10800");
    out.xml.addAttribute("draw:path-stretchpoint-y", "10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L 21600 0 21600 21600 0 21600 Z N M 0 0 L 21600 0 ?f3 ?f2 ?f1 ?f2 Z N M 21600 0 L 21600 21600 ?f3 ?f4 ?f3 ?f2 Z N M 21600 21600 L 0 21600 ?f1 ?f4 ?f3 ?f4 Z N M 0 21600 L 0 0 ?f1 ?f2 ?f1 ?f4 Z N M ?f10 ?f12 L ?f14 ?f12 ?f14 ?f16 C ?f14 ?f18 ?f20 ?f22 ?f24 ?f22 L ?f7 ?f22 C ?f26 ?f22 ?f28 ?f18 ?f28 ?f16 L ?f28 ?f12 ?f7 ?f12 ?f30 ?f32 ?f34 ?f12 ?f36 ?f12 ?f36 ?f16 C ?f36 ?f38 ?f40 ?f42 ?f7 ?f42 L ?f24 ?f42 C ?f44 ?f42 ?f10 ?f38 ?f10 ?f16 Z N");
    out.xml.addAttribute("draw:type", "mso-spt197");
    out.xml.addAttribute("draw:text-areas", "?f1 ?f2 ?f3 ?f4");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","left+$0 ");
    equation(out,"f2","top+$0 ");
    equation(out,"f3","right-$0 ");
    equation(out,"f4","bottom-$0 ");
    equation(out,"f5","10800-$0 ");
    equation(out,"f6","?f5 /10800");
    equation(out,"f7","right/2");
    equation(out,"f8","bottom/2");
    equation(out,"f9","-8050*?f6 ");
    equation(out,"f10","?f9 +?f7 ");
    equation(out,"f11","-3800*?f6 ");
    equation(out,"f12","?f11 +?f8 ");
    equation(out,"f13","-4020*?f6 ");
    equation(out,"f14","?f13 +?f7 ");
    equation(out,"f15","2330*?f6 ");
    equation(out,"f16","?f15 +?f8 ");
    equation(out,"f17","3390*?f6 ");
    equation(out,"f18","?f17 +?f8 ");
    equation(out,"f19","-3100*?f6 ");
    equation(out,"f20","?f19 +?f7 ");
    equation(out,"f21","4230*?f6 ");
    equation(out,"f22","?f21 +?f8 ");
    equation(out,"f23","-1910*?f6 ");
    equation(out,"f24","?f23 +?f7 ");
    equation(out,"f25","1190*?f6 ");
    equation(out,"f26","?f25 +?f7 ");
    equation(out,"f27","2110*?f6 ");
    equation(out,"f28","?f27 +?f7 ");
    equation(out,"f29","4030*?f6 ");
    equation(out,"f30","?f29 +?f7 ");
    equation(out,"f31","-7830*?f6 ");
    equation(out,"f32","?f31 +?f8 ");
    equation(out,"f33","8250*?f6 ");
    equation(out,"f34","?f33 +?f7 ");
    equation(out,"f35","6140*?f6 ");
    equation(out,"f36","?f35 +?f7 ");
    equation(out,"f37","5510*?f6 ");
    equation(out,"f38","?f37 +?f8 ");
    equation(out,"f39","3180*?f6 ");
    equation(out,"f40","?f39 +?f7 ");
    equation(out,"f41","8450*?f6 ");
    equation(out,"f42","?f41 +?f8 ");
    equation(out,"f43","-5090*?f6 ");
    equation(out,"f44","?f43 +?f7 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-maximum", "5400");
    out.xml.addAttribute("draw:handle-switched", "true");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processActionButtonDocument(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 1400);
    out.xml.addAttribute("draw:path-stretchpoint-x", "10800");
    out.xml.addAttribute("draw:path-stretchpoint-y", "10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L 21600 0 21600 21600 0 21600 Z N M 0 0 L 21600 0 ?f3 ?f2 ?f1 ?f2 Z N M 21600 0 L 21600 21600 ?f3 ?f4 ?f3 ?f2 Z N M 21600 21600 L 0 21600 ?f1 ?f4 ?f3 ?f4 Z N M 0 21600 L 0 0 ?f1 ?f2 ?f1 ?f4 Z N M ?f10 ?f12 L ?f14 ?f12 ?f16 ?f18 ?f16 ?f20 ?f10 ?f20 Z N M ?f14 ?f12 L ?f16 ?f18 ?f14 ?f18 Z N");
    out.xml.addAttribute("draw:type", "mso-spt198");
    out.xml.addAttribute("draw:text-areas", "?f1 ?f2 ?f3 ?f4");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","left+$0 ");
    equation(out,"f2","top+$0 ");
    equation(out,"f3","right-$0 ");
    equation(out,"f4","bottom-$0 ");
    equation(out,"f5","10800-$0 ");
    equation(out,"f6","?f5 /10800");
    equation(out,"f7","right/2");
    equation(out,"f8","bottom/2");
    equation(out,"f9","-6350*?f6 ");
    equation(out,"f10","?f9 +?f7 ");
    equation(out,"f11","-7830*?f6 ");
    equation(out,"f12","?f11 +?f8 ");
    equation(out,"f13","1690*?f6 ");
    equation(out,"f14","?f13 +?f7 ");
    equation(out,"f15","6350*?f6 ");
    equation(out,"f16","?f15 +?f7 ");
    equation(out,"f17","-3810*?f6 ");
    equation(out,"f18","?f17 +?f8 ");
    equation(out,"f19","7830*?f6 ");
    equation(out,"f20","?f19 +?f8 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-maximum", "5400");
    out.xml.addAttribute("draw:handle-switched", "true");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processActionButtonSound(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 1400);
    out.xml.addAttribute("draw:path-stretchpoint-x", "10800");
    out.xml.addAttribute("draw:path-stretchpoint-y", "10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L 21600 0 21600 21600 0 21600 Z N M 0 0 L 21600 0 ?f3 ?f2 ?f1 ?f2 Z N M 21600 0 L 21600 21600 ?f3 ?f4 ?f3 ?f2 Z N M 21600 21600 L 0 21600 ?f1 ?f4 ?f3 ?f4 Z N M 0 21600 L 0 0 ?f1 ?f2 ?f1 ?f4 Z N M ?f10 ?f12 L ?f14 ?f12 ?f16 ?f18 ?f16 ?f20 ?f14 ?f22 ?f10 ?f22 Z N M ?f24 ?f8 L ?f26 ?f8 N M ?f24 ?f12 L ?f26 ?f28 N M ?f24 ?f22 L ?f26 ?f30 N");
    out.xml.addAttribute("draw:type", "mso-spt199");
    out.xml.addAttribute("draw:text-areas", "?f1 ?f2 ?f3 ?f4");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","left+$0 ");
    equation(out,"f2","top+$0 ");
    equation(out,"f3","right-$0 ");
    equation(out,"f4","bottom-$0 ");
    equation(out,"f5","10800-$0 ");
    equation(out,"f6","?f5 /10800");
    equation(out,"f7","right/2");
    equation(out,"f8","bottom/2");
    equation(out,"f9","-8050*?f6 ");
    equation(out,"f10","?f9 +?f7 ");
    equation(out,"f11","-2750*?f6 ");
    equation(out,"f12","?f11 +?f8 ");
    equation(out,"f13","-2960*?f6 ");
    equation(out,"f14","?f13 +?f7 ");
    equation(out,"f15","2120*?f6 ");
    equation(out,"f16","?f15 +?f7 ");
    equation(out,"f17","-8050*?f6 ");
    equation(out,"f18","?f17 +?f8 ");
    equation(out,"f19","8050*?f6 ");
    equation(out,"f20","?f19 +?f8 ");
    equation(out,"f21","2750*?f6 ");
    equation(out,"f22","?f21 +?f8 ");
    equation(out,"f23","4020*?f6 ");
    equation(out,"f24","?f23 +?f7 ");
    equation(out,"f25","8050*?f6 ");
    equation(out,"f26","?f25 +?f7 ");
    equation(out,"f27","-5930*?f6 ");
    equation(out,"f28","?f27 +?f8 ");
    equation(out,"f29","5930*?f6 ");
    equation(out,"f30","?f29 +?f8 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-maximum", "5400");
    out.xml.addAttribute("draw:handle-switched", "true");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


void ODrawToOdf::processActionButtonMovie(const MSO::OfficeArtSpContainer& o, Writer& out) {
    out.xml.startElement("draw:custom-shape");
    processStyleAndText(o, out);

    out.xml.startElement("draw:enhanced-geometry");
    processModifiers(o, out, QList<int>() << 1400);
    out.xml.addAttribute("draw:path-stretchpoint-x", "10800");
    out.xml.addAttribute("draw:path-stretchpoint-y", "10800");
    out.xml.addAttribute("svg:viewBox", "0 0 21600 21600");
    out.xml.addAttribute("draw:enhanced-path", "M 0 0 L 21600 0 21600 21600 0 21600 Z N M 0 0 L 21600 0 ?f3 ?f2 ?f1 ?f2 Z N M 21600 0 L 21600 21600 ?f3 ?f4 ?f3 ?f2 Z N M 21600 21600 L 0 21600 ?f1 ?f4 ?f3 ?f4 Z N M 0 21600 L 0 0 ?f1 ?f2 ?f1 ?f4 Z N M ?f10 ?f12 L ?f14 ?f12 ?f16 ?f18 ?f20 ?f18 ?f22 ?f24 ?f22 ?f26 ?f28 ?f26 ?f30 ?f24 ?f32 ?f24 ?f32 ?f34 ?f30 ?f34 ?f28 ?f36 ?f22 ?f36 ?f22 ?f38 ?f42 ?f38 ?f42 ?f40 ?f16 ?f40 ?f14 ?f44 ?f10 ?f44 Z N");
    out.xml.addAttribute("draw:type", "mso-spt200");
    out.xml.addAttribute("draw:text-areas", "?f1 ?f2 ?f3 ?f4");
    setShapeMirroring(o, out);
    equation(out,"f0","$0 ");
    equation(out,"f1","left+$0 ");
    equation(out,"f2","top+$0 ");
    equation(out,"f3","right-$0 ");
    equation(out,"f4","bottom-$0 ");
    equation(out,"f5","10800-$0 ");
    equation(out,"f6","?f5 /10800");
    equation(out,"f7","right/2");
    equation(out,"f8","bottom/2");
    equation(out,"f9","-8050*?f6 ");
    equation(out,"f10","?f9 +?f7 ");
    equation(out,"f11","-4020*?f6 ");
    equation(out,"f12","?f11 +?f8 ");
    equation(out,"f13","-7000*?f6 ");
    equation(out,"f14","?f13 +?f7 ");
    equation(out,"f15","-6560*?f6 ");
    equation(out,"f16","?f15 +?f7 ");
    equation(out,"f17","-3600*?f6 ");
    equation(out,"f18","?f17 +?f8 ");
    equation(out,"f19","4020*?f6 ");
    equation(out,"f20","?f19 +?f7 ");
    equation(out,"f21","4660*?f6 ");
    equation(out,"f22","?f21 +?f7 ");
    equation(out,"f23","-2960*?f6 ");
    equation(out,"f24","?f23 +?f8 ");
    equation(out,"f25","-2330*?f6 ");
    equation(out,"f26","?f25 +?f8 ");
    equation(out,"f27","6780*?f6 ");
    equation(out,"f28","?f27 +?f7 ");
    equation(out,"f29","7200*?f6 ");
    equation(out,"f30","?f29 +?f7 ");
    equation(out,"f31","8050*?f6 ");
    equation(out,"f32","?f31 +?f7 ");
    equation(out,"f33","2960*?f6 ");
    equation(out,"f34","?f33 +?f8 ");
    equation(out,"f35","2330*?f6 ");
    equation(out,"f36","?f35 +?f8 ");
    equation(out,"f37","3800*?f6 ");
    equation(out,"f38","?f37 +?f8 ");
    equation(out,"f39","-1060*?f6 ");
    equation(out,"f40","?f39 +?f8 ");
    equation(out,"f41","-6350*?f6 ");
    equation(out,"f42","?f41 +?f7 ");
    equation(out,"f43","-640*?f6 ");
    equation(out,"f44","?f43 +?f8 ");
    out.xml.startElement("draw:handle");
    out.xml.addAttribute("draw:handle-position", "$0 top");
    out.xml.addAttribute("draw:handle-range-x-maximum", "5400");
    out.xml.addAttribute("draw:handle-switched", "true");
    out.xml.addAttribute("draw:handle-range-x-minimum", "0");
    out.xml.endElement(); // draw:handle
    out.xml.endElement(); // enhanced geometry
    out.xml.endElement(); // custom shape
}


