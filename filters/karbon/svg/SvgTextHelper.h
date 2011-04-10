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

typedef QList<qreal> CharOffsets;

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

    /// Pareses character offsets and pushes them to the stack
    void pushOffsets(const KoXmlElement &element, SvgGraphicsContext *gc);

    /// Pops current offsets from the stack
    void popOffsets();

    /// Strips specified number of offsets
    void stripOffsets(int count);

    /// Checks current x-offset type
    OffsetType xOffsetType() const;

    /// Checks current y-offset type
    OffsetType yOffsetType() const;

    /// Returns x-offsets from stack
    CharOffsets xOffsets(int count) const;

    /// Returns y-offsets from stack
    CharOffsets yOffsets(int count) const;

    /// Returns the text position
    QPointF textPosition() const;

private:
    typedef QList<CharOffsets> CharOffsetStack;

    /// Parses offset values from the given string
    void parseList(const QString &listString, CharOffsetStack &stack, SvgGraphicsContext *gc, bool horizontal);

    /// Collects number of specified offset values from the stack
    CharOffsets collectValues(int count, const CharOffsetStack &stack) const;

    CharOffsetStack m_absolutePosX; ///< stack of absolute character x-positions
    CharOffsetStack m_absolutePosY; ///< stack of absolute character y-positions
    CharOffsetStack m_relativePosX; ///< stack of relative character x-positions
    CharOffsetStack m_relativePosY; ///< stack of relative character y-positions
    QPointF m_textPosition;
};

#endif // SVGTEXTHELPER_H
