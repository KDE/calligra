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

#include <QDebug>

QString ComplexShapeHandler::defaultEquations()
{
    QString eqs = "<draw:equation draw:name=\"width\" draw:formula=\"width\"/>";
    eqs += "<draw:equation draw:name=\"height\" draw:formula=\"height\"/>";
    eqs += "<draw:equation draw:name=\"vc\" draw:formula=\"height / 2 \"/>";
    eqs += "<draw:equation draw:name=\"hc\" draw:formula=\"width / 2 \"/>";
    eqs += "<draw:equation draw:name=\"hd2\" draw:formula=\"height / 2 \"/>";
    eqs += "<draw:equation draw:name=\"hd4\" draw:formula=\"height / 4 \"/>";
    eqs += "<draw:equation draw:name=\"hd5\" draw:formula=\"height / 5 \"/>";
    eqs += "<draw:equation draw:name=\"hd6\" draw:formula=\"height / 6 \"/>";
    eqs += "<draw:equation draw:name=\"hd8\" draw:formula=\"height / 8 \"/>";
    eqs += "<draw:equation draw:name=\"ss\" draw:formula=\"min (width, height)\"/>";
    eqs += "<draw:equation draw:name=\"wd2\" draw:formula=\"width / 2 \"/>";
    eqs += "<draw:equation draw:name=\"wd4\" draw:formula=\"width / 4 \"/>";
    eqs += "<draw:equation draw:name=\"wd5\" draw:formula=\"width / 5 \"/>";
    eqs += "<draw:equation draw:name=\"wd6\" draw:formula=\"width / 6 \"/>";
    eqs += "<draw:equation draw:name=\"wd8\" draw:formula=\"width / 8 \"/>";
    eqs += "<draw:equation draw:name=\"wd10\" draw:formula=\"width / 10 \"/>";
    eqs += "<draw:equation draw:name=\"ls\" draw:formula=\"max (width, height)\"/>";
    eqs += "<draw:equation draw:name=\"ssd2\" draw:formula=\"?ss / 2 \"/>";
    eqs += "<draw:equation draw:name=\"ssd4\" draw:formula=\"?ss / 4 \"/>";
    eqs += "<draw:equation draw:name=\"ssd6\" draw:formula=\"?ss / 6 \"/>";
    eqs += "<draw:equation draw:name=\"ssd8\" draw:formula=\"?ss / 8 \"/>";

    return eqs;
}

QString ComplexShapeHandler::pathEquationsCreated()
{
    return pathEquations;
}

QString ComplexShapeHandler::getArgument(QString& function, bool equation)
{
    QString argument;
    int separatorIndex = function.indexOf(' ');
    if (separatorIndex > 0) {
        argument = function.left(separatorIndex);
        function = function.mid(separatorIndex + 1);
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
    else if (argument == "cd8") { // Converting to degrees
        return QString("%1").arg(2700000/60000.0);
    }
    else if (argument == "cd4") {
        return QString("%1").arg(5400000/60000.0);
    }
    else if (argument == "cd2") {
        return QString("%1").arg(10800000/60000.0);
    }
    else if (argument == "7cd8") {
        return QString("%1").arg(18900000/60000.0);
    }
    else if (argument == "5cd8") {
        return QString("%1").arg(13500000/60000.0);
    }
    else if (argument == "3cd8") {
        return QString("%1").arg(8100000/60000.0);
    }
    else if (argument == "3cd4") {
        return QString("%1").arg(16200000/60000.0);
    }
    else {
       return QString("?%1").arg(argument);
    }
}

QString ComplexShapeHandler::createEquation(QString& function)
{
    int separatorIndex = function.indexOf(' ');
    QString operation = function.left(separatorIndex);
    function = function.mid(separatorIndex + 1);
    QString first, second, third;

    if (operation == "val") {
        return getArgument(function, true);
    }
    else if (operation == "*/") {
        first = getArgument(function, true);
        second = getArgument(function, true);
        third = getArgument(function, true);
        return QString("%1 * %2 / %3").arg(first).arg(second).arg(third);
    }
    else if (operation == "+-") {
        first = getArgument(function, true);
        second = getArgument(function, true);
        third = getArgument(function, true);
        return QString("%1 + %2 - %3").arg(first).arg(second).arg(third);
    }
    else if (operation == "+/") {
        first = getArgument(function, true);
        second = getArgument(function, true);
        third = getArgument(function, true);
        return QString("%1 + %2 / %3").arg(first).arg(second).arg(third);
    }
    else if (operation == "abs") {
        first = getArgument(function, true);
        return QString("abs(%1)").arg(first);
    }
    else if (operation == "at2") {
        first = getArgument(function, true);
        return QString("atan(%1)").arg(first);
    }
    else if (operation == "abs") {
        first = getArgument(function, true);
        return QString("abs(%1)").arg(first);
    }
    else if (operation == "cos") {
        first = getArgument(function,true);
        return QString("cos(%1)").arg(first);
    }
    else if (operation == "sin") {
        first = getArgument(function, true);
        return QString("sin(%1)").arg(first);
    }
    else if (operation == "sqrt") {
        first = getArgument(function, true);
        return QString("sqrt(%1)").arg(first);
    }
    else if (operation == "tan") {
        first = getArgument(function, true);
        return QString("tan(%1)").arg(first);
    }
    else if (operation == "min") {
        first = getArgument(function, true);
        second = getArgument(function, true);
        return QString("min(%1, %2)").arg(first).arg(second);
    }
    else if (operation == "max") {
        first = getArgument(function, true);
        second = getArgument(function, true);
        third = getArgument(function, true);
        return QString("max(%1, %2)").arg(first).arg(second);
    }
    else if (operation == "?:") {
        first = getArgument(function, true);
        second = getArgument(function, true);
        third = getArgument(function, true);
        return QString("if(%1, %2, %3)").arg(first).arg(second).arg(third);
    }
    else if (operation == "cat2") {
        first = getArgument(function, true);
        second = getArgument(function, true);
        third = getArgument(function, true);
        return QString("%1 * cos ( atan (%2, %3) ) ").arg(first).arg(second).arg(third);
    }
    else if (operation == "sat2") {
        first = getArgument(function, true);
        second = getArgument(function, true);
        third = getArgument(function, true);
        return QString("%1 * sin ( atan (%2, %3) ) ").arg(first).arg(second).arg(third);
    }
    else if (operation == "mod") {
        first = getArgument(function, true);
        second = getArgument(function, true);
        third = getArgument(function, true);
        return QString("sqrt(%1 * %1 + %2 + %2 + %3 + %3 )").arg(first).arg(second).arg(third);
    }
    else if (operation == "pin") {
        first = getArgument(function, true);
        second = getArgument(function, true);
        third = getArgument(function, true);
        return QString("if(%1 - %2, %1, if(%2 - %3, %3, %2))").arg(first).arg(second).arg(third);
    }
    else {
        qDebug() << "implement UNHANDLED element" << operation;
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
    QString wR = attrs.value("wR").toString();
    QString hR = attrs.value("hR").toString();
    QString stAng = attrs.value("stAng").toString();
    QString swAng = attrs.value("swAng").toString();

    // If angles are numbers, they must be converted to degrees
    bool ok;
    int startAngle = stAng.toInt(&ok);
    if (ok) {
        startAngle = startAngle / 60000;
        stAng = QString("%1").arg(startAngle);
    }
    else {
        stAng = getArgument(stAng);
    }
    int swingAngle = swAng.toInt(&ok);
    if (ok) {
        swingAngle = swingAngle / 60000;
        swAng = QString("%1").arg(swingAngle);
    }
    else {
        swAng = getArgument(swAng);
    }

    int index = pathEquationIndex;
    ++pathEquationIndex;
    // Converts ooxml 90 to normal anticlockwise unit circle -90
    pathEquations += QString("<draw:equation draw:name=\"ooxmlArc%1\" draw:formula=\"-1 * %2\"/>").arg(index).arg(stAng);

    int index2 = pathEquationIndex;
    ++pathEquationIndex;
    // Convers ooxml clockwise swing angle to normal ánticlock wise end angle
    pathEquations += QString("<draw:equation draw:name=\"ooxmlArc%1\" draw:formula=\"?ooxmlArc%2 - %3\"/>").arg(index2).arg(index).arg(swAng);

    return QString("T %1 %2 %3 %4 ?ooxmlArc%5 ?ooxmlArc%6 ").arg(getArgument(oldX)).arg(getArgument(oldY)).arg(getArgument(wR)).
                                           arg(getArgument(hR)).arg(index).arg(index2);
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

    return QString("%1 %2 ").arg(getArgument(oldX)).arg(getArgument(oldY));
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

    QXmlStreamAttributes attrs = reader->attributes();

    while (!reader->atEnd()) {
        reader->readNext();
        if (reader->isEndElement() && reader->name() == "path") {
            if (attrs.value("stroke") == "false") {
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
            qDebug() << "UNHANDLED path sub element" << reader->name().toString();
        }
    }

    return returnString;
}

QString ComplexShapeHandler::handle_pathLst(QXmlStreamReader* reader)
{
    QString returnString;

    pathEquationIndex = 0;
    pathEquations = "";

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

