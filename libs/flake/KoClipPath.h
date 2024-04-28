/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOCLIPPATH_H
#define KOCLIPPATH_H

#include "flake_export.h"
#include <QList>
#include <QSharedData>
#include <qnamespace.h>

class KoShape;
class KoPathShape;
class KoViewConverter;
class QPainter;
class QTransform;
class QPainterPath;
class QSizeF;

/// Shared clip path data
class FLAKE_EXPORT KoClipData : public QSharedData
{
public:
    /// Creates clip path data from a single path shape, takes ownership of the path shape
    explicit KoClipData(KoPathShape *clipPathShape);

    /// Creates clip path data from multiple path shapes, takes ownership of the path shapes
    explicit KoClipData(const QList<KoPathShape *> &clipPathShapes);

    /// Destroys the clip path data
    ~KoClipData();

    /// Returns the clip path shapes
    QList<KoPathShape *> clipPathShapes() const;

    /// Gives up ownership of clip path shapes
    void removeClipShapesOwnership();

private:
    class Private;
    Private *const d;
};

/// Clip path used to clip shapes
class FLAKE_EXPORT KoClipPath
{
public:
    /**
     * Create a new shape clipping using the given clip data
     * @param clippedShape the shape to clip
     * @param clipData shared clipping data containing the clip paths
     */
    KoClipPath(KoShape *clippedShape, KoClipData *clipData);

    ~KoClipPath();

    /// Sets the clip rule to be used for the clip path
    void setClipRule(Qt::FillRule clipRule);

    /// Returns the current clip rule
    Qt::FillRule clipRule() const;

    /// Returns the current clip path with coordinates in percent of the clipped shape size
    QPainterPath path() const;

    /// Returns the current clip path scaled to match the specified shape size
    QPainterPath pathForSize(const QSizeF &size) const;

    /// Returns the clip path shapes
    QList<KoPathShape *> clipPathShapes() const;

    /**
     * Returns the transformation from the clip data path shapes to the
     * current document coordinates of the specified clipped shape.
     * If the specified clipped shape is null, the transformation
     * from clip data path shapes to shape coordinates of the clipped shape
     * at the time of creating this clip path is being returned.
     */
    QTransform clipDataTransformation(KoShape *clippedShape) const;

    /// Applies the clipping to the given painter
    static void applyClipping(KoShape *clippedShape, QPainter &painter, const KoViewConverter &converter);

private:
    class Private;
    Private *const d;
};

#endif // KOCLIPPATH_H
