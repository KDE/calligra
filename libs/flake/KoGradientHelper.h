/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KO_GRADIENT_HELPER_H
#define KO_GRADIENT_HELPER_H

#include <QBrush>
#include <flake_export.h>

class QGradient;

namespace KoGradientHelper
{
/// creates default gradient
FLAKE_EXPORT QGradient *defaultGradient(QGradient::Type type, QGradient::Spread spread, const QGradientStops &stops);

/// Converts gradient type, preserving as much data as possible
FLAKE_EXPORT QGradient *convertGradient(const QGradient *gradient, QGradient::Type newType);

/// Calculates color at given position from given gradient stops
FLAKE_EXPORT QColor colorAt(qreal position, const QGradientStops &stops);
};

#endif
