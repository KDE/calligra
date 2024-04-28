/*
 * This file is part of Office 2007 Filters for Calligra
 *
 * SPDX-FileCopyrightText: 2010 Nokia Corporation and /or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */

#ifndef COMPLEXSHAPEHANDLER_H
#define COMPLEXSHAPEHANDLER_H

#include "komsooxml_export.h"

#include <QString>
#include <QXmlStreamReader>

// This class is meant to be used as a helper class to understand
// drawingML elements avLst, gdLst and pathLst and to help
// create a custom-shape from them
class KOMSOOXML_EXPORT ComplexShapeHandler
{
public:
    // Set of default equations needed in order to support all variables which are possible in drawingML
    QString defaultEquations();

    // pathLst needs to sometimes create extra equations on the fly because arcTo is defined in a way
    // which is not compatible with odf. The equations created are returned by this function.
    QString pathEquationsCreated();

    // Handles avLst items and creates equations out of them
    QString handle_avLst(QXmlStreamReader *reader);

    // Handles gdLst items and creates equations out of them
    QString handle_gdLst(QXmlStreamReader *reader);

    // Handles rect item and creates coordinates for text-areas out of them
    QString handle_rect(QXmlStreamReader *reader);

    // Handles pathLst and creates a value which should be used for enhanced-path attribute
    // Note: remember to check pathEquationsCreated() after using this one
    QString handle_pathLst(QXmlStreamReader *reader);

private:
    QString getArgument(QString &function, bool equation = false);

    QString createEquation(QString &function);

    QString handle_gd(QXmlStreamReader *reader);

    QString handle_lnTo(QXmlStreamReader *reader);

    QString handle_close(QXmlStreamReader *reader);

    QString handle_arcTo(QXmlStreamReader *reader);

    QString handle_quadBezTo(QXmlStreamReader *reader);

    QString handle_cubicBezTo(QXmlStreamReader *reader);

    QString handle_pt(QXmlStreamReader *reader);

    QString handle_path(QXmlStreamReader *reader);

    QString handle_moveTo(QXmlStreamReader *reader);

    // Storing the latest position where we are, this is needed in order to implement arcTo
    QString oldX, oldY;

    int pathWidth, pathHeight;

    int pathEquationIndex;
    QString pathEquations;
};

#endif
