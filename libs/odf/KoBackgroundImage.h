/* This file is part of the KDE project
 *
 * Copyright (C) 2013 Inge Wallin <inge@lysator.liu.se>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KOBACKGROUNDIMAGE_H
#define KOBACKGROUNDIMAGE_H

// Qt
#include <QColor>
#include <QVector3D>
#include <QString>
#include <QList>

class KoXmlWriter;
class KoXmlElement;


/** A scene in which to show 3d objects.
 *
 * The scene parameters include camera parameters (origin, direction
 * and up direction), the projection to be used and a shadow
 * slant. All these are attributes of the element.
 *
 * The scene can also have a number of light sources as child
 * elements.  These are picked up from the XML element but others are
 * ignored and have to be loaded by code that handles the actual
 * element.
 *
 * In ODF 1.2, a scene description can be part of a dr3d:scene or
 * chart:plot-area if the chart also has 3D mode set.
 */


#include "koodf_export.h"


class KOODF_EXPORT KoBackgroundImage
{
public:
    enum Position {
        Left,
        Center,
        Right,
        Top,
        Bottom
    };

    enum Repeat {
        NoRepeat,
        DoRepeat,
        Stretch
    };

    KoBackgroundImage();
    ~KoBackgroundImage();

    bool loadOdf(const KoXmlElement &element);
    void saveOdf(KoXmlWriter &writer) const;

    // getters
    qint8 opacity() const;     // 0-100, -1 means not defined
    Position verticalPosition() const;
    Position horizontalPosition() const;
    Repeat repeat() const;

    // setters
    void setOpacity(quint8 opacity);
    void setVerticalPosition(Position position);
    void setHorizontalPosition(Position position);
    void setRepeat(Repeat repeat);

private:
    class Private;
    Private * const d;
};


#endif // KOBACKGROUNDIMAGE_H
