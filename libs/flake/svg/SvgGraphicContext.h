/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2003, 2005 Rob Buis <buis@kde.org>
 * SPDX-FileCopyrightText: 2007, 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SVGGRAPHICCONTEXT_H
#define SVGGRAPHICCONTEXT_H

#include "flake_export.h"
#include <KoShapeStroke.h>
#include <QFont>
#include <QTransform>

class FLAKE_EXPORT SvgGraphicsContext
{
public:
    // Fill/stroke styles
    enum StyleType {
        None, ///< no style
        Solid, ///< solid style
        Complex ///< gradient or pattern style
    };

    SvgGraphicsContext();

    StyleType fillType; ///< the current fill type
    Qt::FillRule fillRule; ///< the current fill rule
    QColor fillColor; ///< the current fill color
    QString fillId; ///< the current fill id (used for gradient/pattern fills)

    StyleType strokeType; ///< the current stroke type
    QString strokeId; ///< the current stroke id (used for gradient strokes)
    KoShapeStroke stroke; ///< the current stroke

    QString filterId; ///< the current filter id
    QString clipPathId; ///< the current clip path id
    Qt::FillRule clipRule; ///< the current clip rule
    qreal opacity; ///< the shapes opacity

    QTransform matrix; ///< the current transformation matrix
    QFont font; ///< the current font
    QColor currentColor; ///< the current color
    QString xmlBaseDir; ///< the current base directory (used for loading external content)
    bool preserveWhitespace; ///< preserve whitespace in element text

    QRectF currentBoundbox; ///< the current bound box used for bounding box units
    bool forcePercentage; ///< force parsing coordinates/length as percentages of currentBoundbox
    QTransform viewboxTransform; ///< view box transformation

    qreal letterSpacing; ///< additional spacing between characters of text elements
    qreal wordSpacing; ///< additional spacing between words of text elements
    QString baselineShift; ///< baseline shift mode for text elements

    bool display; ///< controls display of shape
};

#endif // SVGGRAPHICCONTEXT_H
