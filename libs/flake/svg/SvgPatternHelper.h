/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SVGPATTERNHELPER_H
#define SVGPATTERNHELPER_H

#include <KoXmlReader.h>
#include <QImage>
#include <QTransform>

class KoShape;

class SvgPatternHelper
{
public:
    enum Units { UserSpaceOnUse, ObjectBoundingBox };

    SvgPatternHelper();
    ~SvgPatternHelper();

    /// Set pattern units type (affects pattern x, y, width and height)
    void setPatternUnits(Units units);
    /// Return pattern units type
    Units patternUnits() const;

    /// Set pattern content units type (affects coordinates/length of pattern child shapes)
    void setPatternContentUnits(Units units);
    /// Returns pattern content units type
    Units patternContentUnits() const;

    /// Sets the pattern transformation found in attribute "patternTransform"
    void setTransform(const QTransform &transform);
    /// Returns the pattern transform
    QTransform transform() const;

    /// Sets pattern tile position
    void setPosition(const QPointF &position);
    /// Returns pattern tile position (objectBound is used when patternUnits == ObjectBoundingBox)
    QPointF position(const QRectF &objectBound) const;

    /// Sets pattern tile size
    void setSize(const QSizeF &size);
    /// Returns pattern tile size (objectBound is used when patternUnits == ObjectBoundingBox)
    QSizeF size(const QRectF &objectBound) const;

    /// Sets the dom element containing the pattern content
    void setContent(const KoXmlElement &content);
    /// Return the pattern content element
    KoXmlElement content() const;

    /// copies the content from the given pattern helper
    void copyContent(const SvgPatternHelper &other);

    /// Sets the pattern view box (the view box content is fitted into the pattern tile)
    void setPatternContentViewbox(const QRectF &viewBox);

    /// generates the pattern image from the given shapes and using the specified bounding box
    QImage generateImage(const QRectF &objectBound, const QList<KoShape *> content);

private:
    Units m_patternUnits;
    Units m_patternContentUnits;
    QTransform m_transform;
    QPointF m_position;
    QSizeF m_size;
    KoXmlElement m_patternContent;
    QRectF m_patternContentViewbox;
};

#endif // SVGPATTERNHELPER_H
