/* This file is part of the KDE project
 * Copyright (C) 2011 Jan Hambrecht <jaham@gmx.net>
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

#ifndef SVGTEXTHELPER_H
#define SVGTEXTHELPER_H

#include <QtCore/QVector>
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QPointF>

typedef QList<qreal> CharTransforms;

class KoXmlElement;
class SvgGraphicsContext;

class SvgTextHelper
{
public:
    enum OffsetType {
        None,
        Absolute,
        Relative
    };

    SvgTextHelper();

    static QString simplifyText(const QString &text, bool preserveWhiteSpace = false);

    /// Parses character transforms (x,y,dx,dy,rotate) and pushes them to the stack
    void pushCharacterTransforms(const KoXmlElement &element, SvgGraphicsContext *gc);

    /// Pops current character trasnforms from the stack
    void popCharacterTransforms();

    /// Strips specified number of character transformations
    void stripCharacterTransforms(int count);

    /// Checks current x-offset type
    OffsetType xOffsetType() const;

    /// Checks current y-offset type
    OffsetType yOffsetType() const;

    /// Returns x-offsets from stack
    CharTransforms xOffsets(int count) const;

    /// Returns y-offsets from stack
    CharTransforms yOffsets(int count) const;

    /// Returns rotations from stack
    CharTransforms rotations(int count) const;

    /// Returns the text position
    QPointF textPosition() const;

private:
    typedef QList<CharTransforms> CharTransformStack;

    enum ValueType {
        Number,
        XLength,
        YLength
    };

    /// Parses offset values from the given string
    void parseList(const QString &listString, CharTransformStack &stack, SvgGraphicsContext *gc, ValueType type);

    /// Collects number of specified transforms values from the stack
    CharTransforms collectValues(int count, const CharTransformStack &stack) const;

    CharTransformStack m_absolutePosX; ///< stack of absolute character x-positions
    CharTransformStack m_absolutePosY; ///< stack of absolute character y-positions
    CharTransformStack m_relativePosX; ///< stack of relative character x-positions
    CharTransformStack m_relativePosY; ///< stack of relative character y-positions
    CharTransformStack m_rotations;    ///< stack of character rotations

    QPointF m_textPosition;
};

#endif // SVGTEXTHELPER_H
