/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SVGFILTERHELPER_H
#define SVGFILTERHELPER_H

#include <KoXmlReader.h>
#include <QPointF>
#include <QSizeF>

class QRectF;

class SvgFilterHelper
{
public:
    enum Units { UserSpaceOnUse, ObjectBoundingBox };

    SvgFilterHelper();
    ~SvgFilterHelper();

    /// Set the filter units type
    void setFilterUnits(Units filterUnits);
    /// Returns the filter units type
    Units filterUnits() const;

    /// Set the filter primitive units type
    void setPrimitiveUnits(Units primitiveUnits);
    /// Returns the filter primitive units type
    Units primitiveUnits() const;

    /// Sets filter position
    void setPosition(const QPointF &position);
    /// Returns filter position (objectBound is used when filterUnits == ObjectBoundingBox)
    QPointF position(const QRectF &objectBound) const;

    /// Sets filter size
    void setSize(const QSizeF &size);
    /// Returns filter size (objectBound is used when filterUnits == ObjectBoundingBox)
    QSizeF size(const QRectF &objectBound) const;

    /// Sets the dom element containing the filter
    void setContent(const KoXmlElement &content);
    /// Return the filer element
    KoXmlElement content() const;

    static QPointF toUserSpace(const QPointF &position, const QRectF &objectBound);
    static QSizeF toUserSpace(const QSizeF &size, const QRectF &objectBound);

private:
    Units m_filterUnits;
    Units m_primitiveUnits;
    QPointF m_position;
    QSizeF m_size;
    KoXmlElement m_filterContent;
};

#endif // SVGFILTERHELPER_H
