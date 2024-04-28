/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007, 2009 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2010 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SVGGRADIENTHELPER_H
#define SVGGRADIENTHELPER_H

#include <FlakeDebug.h>

#include <QTransform>

class QGradient;

class SvgGradientHelper
{
public:
    enum Units { UserSpaceOnUse, ObjectBoundingBox };

    SvgGradientHelper();
    ~SvgGradientHelper();
    /// Copy constructor
    SvgGradientHelper(const SvgGradientHelper &other);

    /// Sets the gradient units type
    void setGradientUnits(Units units);
    /// Returns gradient units type
    Units gradientUnits() const;

    /// Sets the gradient
    void setGradient(QGradient *g);
    /// Retrurns the gradient
    QGradient *gradient();

    /// Copies the given gradient
    void copyGradient(QGradient *g);

    /// Returns fill adjusted to the given bounding box
    QBrush adjustedFill(const QRectF &bound);

    /// Returns the gradient transformation
    QTransform transform() const;
    /// Sets the gradient transformation
    void setTransform(const QTransform &transform);

    /// Assignment operator
    SvgGradientHelper &operator=(const SvgGradientHelper &rhs);

    QGradient *adjustedGradient(const QRectF &bound) const;

    /// Converts a gradient from LogicalMode to ObjectBoundingMode
    static QGradient *convertGradient(const QGradient *originalGradient, const QSizeF &size);

private:
    /// Duplicates the given gradient and applies the given transformation
    static QGradient *duplicateGradient(const QGradient *g, const QTransform &transform);

    QGradient *m_gradient;
    Units m_gradientUnits;
    QTransform m_gradientTransform;
};

#endif // SVGGRADIENTHELPER_H
