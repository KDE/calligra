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
