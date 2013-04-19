/*
 * This file is part of Office 2007 Filters for Calligra
 *
 * Copyright (C) 2010-2011 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include <QXmlStreamReader>
#include <QFile>
#include <QDebug>

#include "../ComplexShapeHandler.h"

// This small helper program is meant to be used for parsing ooxml 'presetShapeDefinitions.xml'
// and to create a header file from it, header can then be used eg. in a constructor of a class to
// have support for shapes defined in the xml

// Note, presetShapeDefinitions has at least the following errors, which should be corrected manually
// before running this program
// 1. leftArrow <gd name="y1" fmla="+- vc 0 dy1"/>
// 2. 1nd upDownArrow should be replaced with upArrow, which could look something like
//<upArrow>
//    <avLst xmlns="http://schemas.openxmlformats.org/drawingml/2006/main">
//      <gd name="adj1" fmla="val 50000"/>
//      <gd name="adj2" fmla="val 50000"/>
//    </avLst>
//    <gdLst xmlns="http://schemas.openxmlformats.org/drawingml/2006/main">
//      <gd name="maxAdj2" fmla="*/ 100000 h ss"/>
//      <gd name="a1" fmla="pin 0 adj1 100000"/>
//      <gd name="a2" fmla="pin 0 adj2 maxAdj2"/>
//      <gd name="dy1" fmla="*/ ss a2 100000"/>
//      <gd name="y1" fmla="+- t dy1 0"/>
//      <gd name="dx1" fmla="*/ w a1 200000"/>
//      <gd name="x1" fmla="+- hc 0 dx1"/>
//      <gd name="x2" fmla="+- hc dx1 0"/>
//      <gd name="dy2" fmla="*/ x1 dy1 wd2"/>
//      <gd name="y2" fmla="+- y1 dy2 0"/>
//    </gdLst>
//    <ahLst xmlns="http://schemas.openxmlformats.org/drawingml/2006/main">
//      <ahXY gdRefX="adj1" minX="0" maxX="100000">
//        <pos x="x1" y="t"/>
//      </ahXY>
//      <ahXY gdRefY="adj2" minY="0" maxY="maxAdj2">
//        <pos x="l" y="y1"/>
//      </ahXY>
//    </ahLst>
//    <cxnLst xmlns="http://schemas.openxmlformats.org/drawingml/2006/main">
//      <cxn ang="3cd4">
//        <pos x="hc" y="t"/>
//      </cxn>
//      <cxn ang="cd2">
//        <pos x="l" y="y1"/>
//      </cxn>
//      <cxn ang="cd4">
//        <pos x="hc" y="b"/>
//      </cxn>
//      <cxn ang="0">
//        <pos x="r" y="y1"/>
//      </cxn>
//    </cxnLst>
//    <rect xmlns="http://schemas.openxmlformats.org/drawingml/2006/main" l="x1" t="t" r="x2" b="y2"/>
//    <pathLst xmlns="http://schemas.openxmlformats.org/drawingml/2006/main">
//      <path>
//        <moveTo>
//         <pt x="l" y="y1"/>
//        </moveTo>
//        <lnTo>
//          <pt x="x1" y="y1"/>
//        </lnTo>
//        <lnTo>
//          <pt x="x1" y="b"/>
//        </lnTo>
//        <lnTo>
//          <pt x="x2" y="b"/>
//        </lnTo>
//        <lnTo>
//          <pt x="x2" y="y1"/>
//        </lnTo>
//        <lnTo>
//          <pt x="r" y="y1"/>
//        </lnTo>
//        <lnTo>
//          <pt x="hc" y="t"/>
//        </lnTo>
//        <close/>
//      </path>
//    </pathLst>
//  </upArrow>

const QString license = "/*\n\
 * This file is part of Office 2007 Filters for Calligra\n\
 *\n\
 * Copyright (C) 2010-2011 Nokia Corporation and/or its subsidiary(-ies).\n\
 *\n\
 * Contact: Suresh Chande suresh.chande@nokia.com\n\
 *\n\
 * This library is free software; you can redistribute it and/or\n\
 * modify it under the terms of the GNU Lesser General Public License\n\
 * version 2.1 as published by the Free Software Foundation.\n\
 *\n\
 * This library is distributed in the hope that it will be useful, but\n\
 * WITHOUT ANY WARRANTY; without even the implied warranty of\n\
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU\n\
 * Lesser General Public License for more details.\n\
 *\n\
 * You should have received a copy of the GNU Lesser General Public\n\
 * License along with this library; if not, write to the Free Software\n\
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA\n\
 * 02110-1301 USA\n\
 *\n\
 */\n";

int main()
{
    QFile inputFile("./presetShapeDefinitions.xml");
    inputFile.open(QIODevice::ReadOnly);

    QFile outputFile("./generatedShapes.h");
    outputFile.open(QIODevice::WriteOnly);
    QTextStream outStream(&outputFile);

    outStream << license;

    QXmlStreamReader xml(&inputFile);

    enum ReadingState {Beginning, ShapeNameNext, InShapeName};
    ReadingState state = Beginning;

    QXmlStreamAttributes attrs;

    QString currentShapeName, shapeDefinition, shapeAttributes, pathEquations, textareas;

    ComplexShapeHandler handler;

    while (!xml.atEnd()) {
        xml.readNext();
        switch (state) {
        case Beginning:
            if (xml.isStartElement() && xml.name() == "presetShapeDefinitons") {
                xml.readNext();
                state = ShapeNameNext;
            }
            break;
        case ShapeNameNext:
            currentShapeName = xml.name().toString();
            qDebug() << "Shape name is " << currentShapeName;
            xml.readNext();
            state = InShapeName;
            break;
        case InShapeName:
            if (xml.isEndElement() && xml.name() == currentShapeName) {
                state = ShapeNameNext;
                QString defaults = handler.defaultEquations().replace('"', "\\\"");
                pathEquations.replace('"', "\\\"");
                shapeDefinition.replace('"', "\\\"");
                QString output = "equations[\"" + currentShapeName + "\"]=\"" + defaults + shapeDefinition + pathEquations + "\";" + "\n";
                // Cutting the output to sequence of 1000 chars
                int index = 1000;
                while (index < output.length()) {
                    output.insert(index, "\"\\\n\"");
                    index = index + 1000;
                }
                outStream << output;
                shapeDefinition.clear();
                pathEquations.clear();
                shapeAttributes.replace('"', "\\\"");
                outStream << "attributes[\"" << currentShapeName << "\"]=\"" << shapeAttributes << "\";" << "\n";
                shapeAttributes = "";
            }
            else if (xml.isStartElement() && xml.name() == "avLst") {
                shapeDefinition += handler.handle_avLst(&xml);
            }
            else if (xml.isStartElement() && xml.name() == "gdLst") {
                shapeDefinition += handler.handle_gdLst(&xml);
            }
            else if (xml.isStartElement() && xml.name() == "pathLst") {
                shapeAttributes += handler.handle_pathLst(&xml);
                pathEquations += handler.pathEquationsCreated();
            }
            else if (xml.isStartElement() && xml.name() == "ahLst") {
                xml.skipCurrentElement();
            }
            else if (xml.isStartElement() && xml.name() == "rect") {
                // draw:text-areas
                textareas = handler.handle_rect(&xml);
                outStream << "textareas[\"" << currentShapeName << "\"]=\"" << textareas << "\";" << "\n";
            }
            else if (xml.isStartElement() && xml.name() == "cxnLst") {
                xml.skipCurrentElement();
            }
            xml.readNext();
            break;
        }
    }
}
