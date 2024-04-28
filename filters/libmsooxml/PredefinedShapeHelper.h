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

#ifndef PREDEFINEDSHAPEHELPER_H
#define PREDEFINEDSHAPEHELPER_H

#include <QHash>

// This class can be used to to get draw information of
// predefined shapes in ooxml
class PredefinedShapeHelper
{
public:
    PredefinedShapeHelper();

    // Container for draw:equations
    QHash<QString, QString> equations;

    // Container for draw:enhanced-path attributes for shapes, these should be used in draw:enhanced-geometry
    QHash<QString, QString> attributes;

    // Container for draw:text-areas attribute, should be used in draw:enhanced-geometry
    QHash<QString, QString> textareas;
};

#endif
