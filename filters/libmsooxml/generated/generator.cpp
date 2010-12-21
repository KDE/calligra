/*
 * This file is part of Office 2007 Filters for Calligra
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
int main()
{
    QFile inputFile("./presetShapeDefinitions.xml");
    inputFile.open(QIODevice::ReadOnly);

    QFile outputFile("./generatedShapes.h");
    outputFile.open(QIODevice::WriteOnly);
    QTextStream outStream(&outputFile);

    QXmlStreamReader xml(&inputFile);

    enum ReadingState {Beginning, ShapeNameNext, InShapeName};
    ReadingState state = Beginning;

    QXmlStreamAttributes attrs;

    QString currentShapeName, shapeDefinition, shapeAttributes;

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
                shapeDefinition = shapeDefinition.replace('"', "\\\"");
                outStream << "equations[\"" << currentShapeName << "\"]=\"" << defaults << shapeDefinition << "\";" << "\n";
                shapeDefinition = "";
                shapeAttributes = shapeAttributes.replace('"', "\\\"");
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
            }
            else if (xml.isStartElement() && xml.name() == "ahLst") {
                xml.skipCurrentElement();
            }
            else if (xml.isStartElement() && xml.name() == "rect") {
                xml.skipCurrentElement();
            }
            else if (xml.isStartElement() && xml.name() == "cxnLst") {
                xml.skipCurrentElement();
            }
            xml.readNext();
            break;
        }
    }
}
