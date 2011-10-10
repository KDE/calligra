/*
 * This file is part of Maemo 5 Office UI for Calligra
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Kaushal M <kshlmster@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef FOIMAGESELECTIONWIDGET_H
#define FOIMAGESELECTIONWIDGET_H

#include <QWidget>

class KoShape;
class KoDocumentResourceManager;

/*!
 * This class implements an image selection widget that replaces KoImageSelectionWidget
 * Right now it only shows file dialog to select images. This should be extended to allow for browsing the gallery,
 * and snapping of pictures from the camera
 */
class FoImageSelectionWidget
{
public:
    static KoShape *selectImageShape(KoDocumentResourceManager *documentResource, QWidget *parent = 0);
};

#endif // FOIMAGESELECTIONWIDGET_H
