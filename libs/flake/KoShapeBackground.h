/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPEBACKGROUND_H
#define KOSHAPEBACKGROUND_H

#include "flake_export.h"

#include <QtGlobal>

class QSizeF;
class QPainter;
class QPainterPath;
class KoGenStyle;
class KoShapeSavingContext;
class KoOdfLoadingContext;
class KoShapeBackgroundPrivate;
class KoShapePaintingContext;
class KoViewConverter;

/**
 * This is the base class for shape backgrounds.
 * Derived classes are used to paint the background of
 * a shape within a given painter path.
 */
class FLAKE_EXPORT KoShapeBackground
{
public:
    KoShapeBackground();
    virtual ~KoShapeBackground();

    /// Paints the background using the given fill path
    virtual void paint(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &context, const QPainterPath &fillPath) const = 0;

    /// Returns if the background has some transparency.
    virtual bool hasTransparency() const;

    /**
     * Fills the style object
     * @param style object
     * @param context used for saving
     */
    virtual void fillStyle(KoGenStyle &style, KoShapeSavingContext &context) = 0;

    /// load background from odf styles
    virtual bool loadStyle(KoOdfLoadingContext &context, const QSizeF &shapeSize) = 0;

protected:
    KoShapeBackground(KoShapeBackgroundPrivate &);
    KoShapeBackgroundPrivate *d_ptr;

private:
    Q_DECLARE_PRIVATE(KoShapeBackground)
};

#endif // KOSHAPEBACKGROUND_H
