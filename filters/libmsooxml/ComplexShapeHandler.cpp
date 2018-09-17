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

#include "ComplexShapeHandler.h"

#include "MsooXmlDebug.h"

QString ComplexShapeHandler::defaultEquations()
{
    QString eqs = QLatin1String(
            "<draw:equation draw:name=\"width\" draw:formula=\"width\"/>"
            "<draw:equation draw:name=\"height\" draw:formula=\"height\"/>"
            "<draw:equation draw:name=\"vc\" draw:formula=\"height/2\"/>"
            "<draw:equation draw:name=\"hc\" draw:formula=\"width/2\"/>"
            "<draw:equation draw:name=\"hd2\" draw:formula=\"height/2\"/>"
    // Note, this is not defined, but is used
            "<draw:equation draw:name=\"hd3\" draw:formula=\"height/3\"/>"
            "<draw:equation draw:name=\"hd4\" draw:formula=\"height/4\"/>"
            "<draw:equation draw:name=\"hd5\" draw:formula=\"height/5\"/>"
            "<draw:equation draw:name=\"hd6\" draw:formula=\"height/6\"/>"
            "<draw:equation draw:name=\"hd8\" draw:formula=\"height/8\"/>"
            "<draw:equation draw:name=\"ss\" draw:formula=\"min(width,height)\"/>"
            "<draw:equation draw:name=\"wd2\" draw:formula=\"width/2\"/>"
            "<draw:equation draw:name=\"wd3\" draw:formula=\"width/3\"/>"
            "<draw:equation draw:name=\"wd4\" draw:formula=\"width/4\"/>"
            "<draw:equation draw:name=\"wd5\" draw:formula=\"width/5\"/>"
            "<draw:equation draw:name=\"wd6\" draw:formula=\"width/6\"/>"
            "<draw:equation draw:name=\"wd8\" draw:formula=\"width/8\"/>"
            "<draw:equation draw:name=\"wd10\" draw:formula=\"width/10\"/>"
            "<draw:equation draw:name=\"ls\" draw:formula=\"max(width,height)\"/>"
            "<draw:equation draw:name=\"ssd2\" draw:formula=\"?ss/2\"/>"
            "<draw:equation draw:name=\"ssd4\" draw:formula=\"?ss/4\"/>"
            "<draw:equation draw:name=\"ssd6\" draw:formula=\"?ss/6\"/>"
            "<draw:equation draw:name=\"ssd8\" draw:formula=\"?ss/8\"/>");

    return eqs;
}

QString ComplexShapeHandler::pathEquationsCreated()
{
    return pathEquations;
}

QString ComplexShapeHandler::getArgument(QString& function, bool equation)
{
    // there can be extra spaces in the beginning/in the end, removing them is necessary
    function = function.trimmed();

    QString argument;
    int separatorIndex = function.indexOf(' ');
    if (separatorIndex > 0) {
        argument = function.left(separatorIndex);
        function.remove(0, separatorIndex + 1);
    }
    else {
        argument = function;
    }
    bool ok;
    argument.toInt(&ok);
    if (ok) {
        return argument;
    } // These values are defined for drawingML
    else if (argument == "h" || argument == "b") {
        if (!equation) {
            return "?height";
        }
        return "height";
    }
    else if (argument == "w" || argument == "r") {
        if (!equation) {
            return "?width";
        }
        return "width";
    }
    else if (argument == "t" || argument == "l") {
        return "0";
    }
    else if (argument == "cd8") {
        return QString("%1").arg(2700000);
    }
    else if (argument == "cd4") {
        return QString("%1").arg(5400000);
    }
    else if (argument == "cd2") {
        return QString("%1").arg(10800000);
    }
    else if (argument == "7cd8") {
        return QString("%1").arg(18900000);
    }
    else if (argument == "5cd8") {
        return QString("%1").arg(13500000);
    }
    else if (argument == "3cd8") {
        return QString("%1").arg(8100000);
    }
    else if (argument == "3cd4") {
        return QString("%1").arg(16200000);
    }
    else {
       return QString("?%1").arg(argument);
    }
}

QString ComplexShapeHandler::createEquation(QString& function)
{
    int separatorIndex = function.indexOf(' ');
    QString operation = function.left(separatorIndex);
    function.remove(0, separatorIndex + 1);
    QString first, second, third;

    if (operation == "val") {
        return getArgument(function, true);
    }
    else if (operation == "*/") {
        first = getArgument(function, true);
        second = getArgument(function, true);
        third = getArgument(function, true);
        return QString("(%1*%2)/%3").arg(first).arg(second).arg(third);
    }
    else if (operation == "+-") {
        first = getArgument(function, true);
        second = getArgument(function, true);
        third = getArgument(function, true);
        return QString("(%1+%2)-%3").arg(first).arg(second).arg(third);
    }
    else if (operation == "+/") {
        first = getArgument(function, true);
        second = getArgument(function, true);
        third = getArgument(function, true);
        return QString("(%1+%2)/%3").arg(first).arg(second).arg(third);
    }
    else if (operation == "abs") {
        first = getArgument(function, true);
        return QString("abs(%1)").arg(first);
    }
    else if (operation == "at2") {
        first = getArgument(function, true);
        second = getArgument(function, true);
        // Converting to ooxml units, since sin/cos/tan functions assume that
        // they will get thier inputs in those units
        return QString("atan2(%1,%2)*3437746.771").arg(second).arg(first);
    }
    else if (operation == "cos") {
        first = getArgument(function,true);
        second = getArgument(function, true);
        return QString("%1*cos(%2*0.000000291)").arg(first).arg(second);
    }
    else if (operation == "sin") {
        first = getArgument(function, true);
        second = getArgument(function, true);
        return QString("%1*sin(%2*0.000000291)").arg(first).arg(second);
    }
    else if (operation == "sqrt") {
        first = getArgument(function, true);
        return QString("sqrt(%1)").arg(first);
    }
    else if (operation == "tan") {
        first = getArgument(function, true);
        second = getArgument(function, true);
        return QString("%1*tan(%2*0.000000291)").arg(first).arg(second);
    }
    else if (operation == "min") {
        first = getArgument(function, true);
        second = getArgument(function, true);
        return QString("min(%1,%2)").arg(first).arg(second);
    }
    else if (operation == "max") {
        first = getArgument(function, true);
        second = getArgument(function, true);
        return QString("max(%1,%2)").arg(first).arg(second);
    }
    else if (operation == "?:") {
        first = getArgument(function, true);
        second = getArgument(function, true);
        third = getArgument(function, true);
        return QString("if(max(%1,0),%2,%3)").arg(first).arg(second).arg(third);
    }
    else if (operation == "cat2") {
        first = getArgument(function, true);
        second = getArgument(function, true);
        third = getArgument(function, true);
        return QString("%1*cos(atan2(%2,%3))").arg(first).arg(third).arg(second);
    }
    else if (operation == "sat2") {
        first = getArgument(function, true);
        second = getArgument(function, true);
        third = getArgument(function, true);
        return QString("%1*sin(atan2(%2,%3))").arg(first).arg(third).arg(second);
    }
    else if (operation == "mod") {
        first = getArgument(function, true);
        second = getArgument(function, true);
        third = getArgument(function, true);
        return QString("sqrt(%1*%1+%2*%2+%3*%3)").arg(first).arg(second).arg(third);
    }
    else if (operation == "pin") {
        first = getArgument(function, true);
        second = getArgument(function, true);
        third = getArgument(function, true);
        return QString("if(max(%1-%2,0),%1,if(max(%2-%3,0),%3,%2))").arg(first).arg(second).arg(third);
    }
    else {
        debugMsooXml << "implement UNHANDLED element" << operation;
        return "";
    }
}

QString ComplexShapeHandler::handle_gd(QXmlStreamReader* reader)
{
    QXmlStreamAttributes attrs = reader->attributes();

    QString name = attrs.value("name").toString();
    QString function = attrs.value("fmla").toString();

    QString returnString = QString("<draw:equation draw:name=\"%1\" draw:formula=\"%2\"/>").arg(name).arg(createEquation(function));

    reader->readNext();
    return returnString;
}

QString ComplexShapeHandler::handle_avLst(QXmlStreamReader* reader)
{
    QString returnString;

    while (!reader->atEnd()) {
        reader->readNext();
        if (reader->isEndElement() && reader->name() == "avLst") {
            break;
        }
        else if (reader->isStartElement() && reader->name() == "gd") {
            returnString += handle_gd(reader);
        }
    }

    return returnString;
}

QString ComplexShapeHandler::handle_gdLst(QXmlStreamReader* reader)
{
    QString returnString;

    while (!reader->atEnd()) {
        reader->readNext();
        if (reader->isEndElement() && reader->name() == "gdLst") {
            break;
        }
        else if (reader->isStartElement() && reader->name() == "gd") {
            returnString += handle_gd(reader);
        }
    }

    return returnString;
}

QString ComplexShapeHandler::handle_rect(QXmlStreamReader* reader)
{
    QXmlStreamAttributes attrs = reader->attributes();
    while (!reader->atEnd()) {
        reader->readNext();
        if (reader->isEndElement() && reader->name() == "rect") {
            break;
        }
    }

    QString left = attrs.value("l").toString(); left = getArgument(left);
    QString top = attrs.value("t").toString(); top = getArgument(top);
    QString right = attrs.value("r").toString(); right = getArgument(right);
    QString bottom = attrs.value("b").toString(); bottom = getArgument(bottom);

    return QString("%1 %2 %3 %4").arg(left).arg(top).arg(right).arg(bottom).trimmed();
}

QString ComplexShapeHandler::handle_close(QXmlStreamReader* reader)
{
    while (!reader->atEnd()) {
        reader->readNext();
        if (reader->isEndElement() && reader->name() == "close") {
            break;
        }
    }
    return "Z ";
}

QString ComplexShapeHandler::handle_arcTo(QXmlStreamReader* reader)
{
    QXmlStreamAttributes attrs = reader->attributes();
    while (!reader->atEnd()) {
        reader->readNext();
        if (reader->isEndElement() && reader->name() == "arcTo") {
            break;
        }
    }

    QString wR = attrs.value("wR").toString(); // vertical radius
    QString hR = attrs.value("hR").toString(); // horizontal radius
    QString stAng = attrs.value("stAng").toString(); // start angle
    QString swAng = attrs.value("swAng").toString(); // swing angle

    wR = getArgument(wR);
    hR = getArgument(hR);

    if (pathWidth > 0 || pathHeight > 0) {
        bool isNumber = false;
        qreal number = wR.toInt(&isNumber);
        if (pathWidth > 0 && isNumber) {
            int relWidthIndex = pathEquationIndex;
            ++pathEquationIndex;
            pathEquations += QString("<draw:equation draw:name=\"ooxmlArc%1\" draw:formula=\"%2*?width\"/>").
                arg(relWidthIndex).arg(number/pathWidth);
            wR = QString("?ooxmlArc%1 ").arg(relWidthIndex);
        }
        number = hR.toInt(&isNumber);
        if (pathHeight > 0 && isNumber) {
            int relHeightIndex = pathEquationIndex;
            ++pathEquationIndex;
            pathEquations += QString("<draw:equation draw:name=\"ooxmlArc%1\" draw:formula=\"%2*?height\"/>").
                arg(relHeightIndex).arg(number/pathHeight);
            hR = QString("?ooxmlArc%1 ").arg(relHeightIndex);
        }
    }

    stAng = getArgument(stAng);
    swAng = getArgument(swAng);

    // These equations convert ooxml arcTo to a form that is compatible with odf
    // Notice that the angle system used in ooxlm is reverse to normal mathematical handling
    // There seem to be two different ways of conversion, one to W, other one to T
    // both of them need end angle, x1 and y1
    // Most problematic case is when swAng is negative, when A should be used instead of W

    int endAngleIndex = pathEquationIndex;
    ++pathEquationIndex;
    // Converts to end angle
    pathEquations += QString("<draw:equation draw:name=\"ooxmlArc%1\" draw:formula=\"%2+%3\"/>").
         arg(endAngleIndex).arg(stAng).arg(swAng);

    /*
    // Try for T
    int centerXIndex = pathEquationIndex;
    ++pathEquationIndex;
    pathEquations += QString("<draw:equation draw:name=\"ooxmlArc%1\" draw:formula=\"%2 - %3 * cos(%4 * 0.000000291)\"/>").
         arg(centerXIndex).arg(oldX).arg(wR).arg(stAng);

    int centerYIndex = pathEquationIndex;
    ++pathEquationIndex;
    pathEquations += QString("<draw:equation draw:name=\"ooxmlArc%1\" draw:formula=\"%2 - %3 * sin(%4 * 0.000000291)\"/>").
         arg(centerYIndex).arg(oldY).arg(hR).arg(stAng);

    int widthIndex = pathEquationIndex;
    ++pathEquationIndex;
    pathEquations += QString("<draw:equation draw:name=\"ooxmlArc%1\" draw:formula=\"2 * %2\"/>").arg(widthIndex).arg(wR);

    int heightIndex = pathEquationIndex;
    ++pathEquationIndex;
    pathEquations += QString("<draw:equation draw:name=\"ooxmlArc%1\" draw:formula=\"2 * %2\"/>").arg(heightIndex).arg(hR);

    int odfStartAngleIndex = pathEquationIndex;
    ++pathEquationIndex;
    pathEquations += QString("<draw:equation draw:name=\"ooxmlArc%1\" draw:formula=\"360 - %2 / 60000\"/>").
         arg(odfStartAngleIndex).arg(stAng);

    int odfEndAngleIndex = pathEquationIndex;
    ++pathEquationIndex;
    pathEquations += QString("<draw:equation draw:name=\"ooxmlArc%1\" draw:formula=\"360 - ?ooxmlArc%2 / 60000\"/>").
         arg(odfEndAngleIndex).arg(endAngleIndex);

    QString path = QString("T ?ooxmlArc%1 ?ooxmlArc%2 ?ooxmlArc%3 ?ooxmlArc%4 ?ooxmlArc%5 ?ooxmlArc%6 ").arg(centerXIndex).arg(centerYIndex).
        arg(widthIndex).arg(heightIndex).arg(odfStartAngleIndex).arg(odfEndAngleIndex);

    oldX = QString("?ooxmlArc%1 + %2 * cos(?ooxmlArc%3 * 0.000000291)").arg(centerXIndex).arg(wR).arg(endAngleIndex);
    oldY = QString("?ooxmlArc%1 + %2 * sin(?ooxmlArc%3 * 0.000000291)").arg(centerYIndex).arg(hR).arg(endAngleIndex);
    */

    // Try for W/A (commented out atm. T (above) seems to work better
    // x1, y1 marks the upper left corner
    int ellipseX1Index = pathEquationIndex;
    ++pathEquationIndex;
    // x1
    pathEquations += QString("<draw:equation draw:name=\"ooxmlArc%1\" draw:formula=\"%2-%3*cos(%4*0.000000291)-%3\"/>").
         arg(ellipseX1Index).arg(oldX).arg(wR).arg(stAng);

    int ellipseY1Index = pathEquationIndex;
    ++pathEquationIndex;
    // y1
    pathEquations += QString("<draw:equation draw:name=\"ooxmlArc%1\" draw:formula=\"%2-%3*sin(%4*0.000000291)-%3\"/>").
         arg(ellipseY1Index).arg(oldY).arg(hR).arg(stAng);

    // Try for W solution
    // W clockwise- (x1 y1 x2 y2 x3 y3 x y) + The same as the “A” command except, that the arcto arc is drawn clockwise.

    // x2, y2 marks the lower right corner
    int ellipseX2Index = pathEquationIndex;
    ++pathEquationIndex;
    // x2
    pathEquations += QString("<draw:equation draw:name=\"ooxmlArc%1\" draw:formula=\"?ooxmlArc%2+%3*2\"/>").
        arg(ellipseX2Index).arg(ellipseX1Index).arg(wR);

    int ellipseY2Index = pathEquationIndex;
    ++pathEquationIndex;
    // y2
    pathEquations += QString("<draw:equation draw:name=\"ooxmlArc%1\" draw:formula=\"?ooxmlArc%2+%3*2\"/>").
        arg(ellipseY2Index).arg(ellipseY1Index).arg(hR);

    int arcEndX = pathEquationIndex;
    ++pathEquationIndex;
    // x4
    pathEquations += QString("<draw:equation draw:name=\"ooxmlArc%1\" draw:formula=\"?ooxmlArc%2+%3+%3*cos(?ooxmlArc%4*0.000000291)\"/>").
        arg(arcEndX).arg(ellipseX1Index).arg(wR).arg(endAngleIndex);

    int arcEndY = pathEquationIndex;
    ++pathEquationIndex;
    // y4
    pathEquations += QString("<draw:equation draw:name=\"ooxmlArc%1\" draw:formula=\"?ooxmlArc%2+%3+%3*sin(?ooxmlArc%4*0.000000291)\"/>").
        arg(arcEndY).arg(ellipseY1Index).arg(hR).arg(endAngleIndex);

    QString path;
    bool signTest = true;
    int temp = swAng.toInt(&signTest);

    if (signTest && temp < 0) {
        path = QString("A ?ooxmlArc%1 ?ooxmlArc%2 ?ooxmlArc%3 ?ooxmlArc%4 %5 %6 ?ooxmlArc%7 ?ooxmlArc%8 ").
            arg(ellipseX1Index).arg(ellipseY1Index).arg(ellipseX2Index).arg(ellipseY2Index).arg(oldX).arg(oldY).arg(arcEndX).arg(arcEndY);
    }
    else {
        path = QString("W ?ooxmlArc%1 ?ooxmlArc%2 ?ooxmlArc%3 ?ooxmlArc%4 %5 %6 ?ooxmlArc%7 ?ooxmlArc%8 ").
            arg(ellipseX1Index).arg(ellipseY1Index).arg(ellipseX2Index).arg(ellipseY2Index).arg(oldX).arg(oldY).arg(arcEndX).arg(arcEndY);
    }
    oldX = QString("?ooxmlArc%1").arg(arcEndX);
    oldY = QString("?ooxmlArc%1").arg(arcEndY);

    return path;
}

QString ComplexShapeHandler::handle_pt(QXmlStreamReader* reader)
{
    QXmlStreamAttributes attrs = reader->attributes();
    while (!reader->atEnd()) {
        reader->readNext();
        if (reader->isEndElement() && reader->name() == "pt") {
            break;
        }
    }

    oldX = attrs.value("x").toString();
    oldY = attrs.value("y").toString();
    oldX = getArgument(oldX);
    oldY = getArgument(oldY);

    if (pathWidth > 0 || pathHeight > 0) {
        bool isNumber = false;
        qreal number = oldX.toInt(&isNumber);
        if (pathWidth > 0 && isNumber) {
            int relWidthIndex = pathEquationIndex;
            ++pathEquationIndex;
            pathEquations += QString("<draw:equation draw:name=\"ooxmlArc%1\" draw:formula=\"%2*?width\"/>").
                arg(relWidthIndex).arg(number/pathWidth);
            oldX = QString("?ooxmlArc%1 ").arg(relWidthIndex);
        }
        number = oldY.toInt(&isNumber);
        if (pathHeight > 0 && isNumber) {
            int relHeightIndex = pathEquationIndex;
            ++pathEquationIndex;
            pathEquations += QString("<draw:equation draw:name=\"ooxmlArc%1\" draw:formula=\"%2*?height\"/>").
                arg(relHeightIndex).arg(number/pathHeight);
            oldY = QString("?ooxmlArc%1 ").arg(relHeightIndex);
        }
    }

    return QString("%1 %2 ").arg(oldX).arg(oldY);
}

QString ComplexShapeHandler::handle_lnTo(QXmlStreamReader* reader)
{
    QString returnString;

    while (!reader->atEnd()) {
        reader->readNext();
        if (reader->isEndElement() && reader->name() == "lnTo") {
            break;
        }
        else if (reader->isStartElement() && reader->name() == "pt") {
            returnString += handle_pt(reader);
        }
    }
    return QString("L %1 ").arg(returnString);
}

QString ComplexShapeHandler::handle_moveTo(QXmlStreamReader* reader)
{
    QString returnString;

    while (!reader->atEnd()) {
        reader->readNext();
        if (reader->isEndElement() && reader->name() == "moveTo") {
            break;
        }
        else if (reader->isStartElement() && reader->name() == "pt") {
            returnString += handle_pt(reader);
        }
    }
    return QString("M %1").arg(returnString);
}

QString ComplexShapeHandler::handle_quadBezTo(QXmlStreamReader* reader)
{
    QString returnString;

    while (!reader->atEnd()) {
        reader->readNext();
        if (reader->isEndElement() && reader->name() == "quadBezTo") {
            break;
        }
        else if (reader->isStartElement() && reader->name() == "pt") {
            returnString += handle_pt(reader);
        }
    }
    return QString("Q %1").arg(returnString);
}

QString ComplexShapeHandler::handle_cubicBezTo(QXmlStreamReader* reader)
{
    QString returnString;

    while (!reader->atEnd()) {
        reader->readNext();
        if (reader->isEndElement() && reader->name() == "cubicBezTo") {
            break;
        }
        else if (reader->isStartElement() && reader->name() == "pt") {
            returnString += handle_pt(reader);
        }
    }
    return QString("C %1").arg(returnString);
}

QString ComplexShapeHandler::handle_path(QXmlStreamReader* reader)
{
    QString returnString;
    pathWidth = 0;
    pathHeight = 0;

    QXmlStreamAttributes attrs = reader->attributes();

    QString width = attrs.value("w").toString();
    QString height = attrs.value("h").toString();

    if (!width.isEmpty()) {
        pathWidth = width.toInt();
    }
    if (!height.isEmpty()) {
        pathHeight = height.toInt();
    }

    while (!reader->atEnd()) {
        reader->readNext();
        if (reader->isEndElement() && reader->name() == "path") {
            if (attrs.value("stroke") == "false" || attrs.value("stroke") == "0" ) {
                returnString += "S ";
            }
            if (attrs.value("fill") == "none") {
                returnString += "F ";
            }
            break;
        }
        else if (reader->isStartElement() && reader->name() == "moveTo") {
            returnString += handle_moveTo(reader);
        }
        else if (reader->isStartElement() && reader->name() == "close") {
            returnString += handle_close(reader);
        }
        else if (reader->isStartElement() && reader->name() == "lnTo") {
            returnString += handle_lnTo(reader);
        }
        else if (reader->isStartElement() && reader->name() == "cubicBezTo") {
            returnString += handle_cubicBezTo(reader);
        }
        else if (reader->isStartElement() && reader->name() == "quadBezTo") {
            returnString += handle_quadBezTo(reader);
        }
        else if (reader->isStartElement() && reader->name() == "arcTo") {
            returnString += handle_arcTo(reader);
        }
	else if (reader->isStartElement()) {
            debugMsooXml << "UNHANDLED path sub element" << reader->name().toString();
        }
    }

    return returnString;
}

QString ComplexShapeHandler::handle_pathLst(QXmlStreamReader* reader)
{
    QString returnString;

    pathEquationIndex = 0;
    pathEquations.clear();

    while (!reader->atEnd()) {
        reader->readNext();
        if (reader->isEndElement() && reader->name() == "pathLst") {
            break;
        }
        else if (reader->isStartElement() && reader->name() == "path") {
            returnString += handle_path(reader);
        }
    }

    return returnString;
}
