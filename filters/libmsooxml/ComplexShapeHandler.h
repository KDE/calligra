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

#ifndef COMPLEXSHAPEHANDLER_H
#define COMPLEXSHAPEHANDLER_H

#include "komsooxml_export.h"

#include <QXmlStreamReader>
#include <QString>

// This class is meant to be used as a helper class to understand
// drawingML elements avLst, gdLst and pathLst and to help
// create a custom-shape from them
class KOMSOOXML_EXPORT ComplexShapeHandler {

public:

    // Set of default equations needed in order to support all variables which are possible in drawingML
    QString defaultEquations();

    // pathLst needs to sometimes create extra equations on the fly because arcTo is defined in a way
    // which is not compatible with odf. The equations created are returned by this function.
    QString pathEquationsCreated();

    // Handles avLst items and creates equations out of them
    QString handle_avLst(QXmlStreamReader* reader);

    // Handles gdLst items and creates equations out of them
    QString handle_gdLst(QXmlStreamReader* reader);

    // Handles rect item and creates coordinates for text-areas out of them
    QString handle_rect(QXmlStreamReader* reader);

    // Handles pathLst and creates a value which should be used for enhanced-path attribute
    // Note: remember to check pathEquationsCreated() after using this one
    QString handle_pathLst(QXmlStreamReader* reader);

private:

    QString getArgument(QString& function, bool equation = false);

    QString createEquation(QString& function);

    QString handle_gd(QXmlStreamReader* reader);

    QString handle_lnTo(QXmlStreamReader* reader);

    QString handle_close(QXmlStreamReader* reader);

    QString handle_arcTo(QXmlStreamReader* reader);

    QString handle_quadBezTo(QXmlStreamReader* reader);

    QString handle_cubicBezTo(QXmlStreamReader* reader);

    QString handle_pt(QXmlStreamReader* reader);

    QString handle_path(QXmlStreamReader* reader);

    QString handle_moveTo(QXmlStreamReader* reader);

    // Storing the latest position where we are, this is needed in order to implement arcTo
    QString oldX, oldY;

    int pathWidth, pathHeight;

    int pathEquationIndex;
    QString pathEquations;
};

#endif
