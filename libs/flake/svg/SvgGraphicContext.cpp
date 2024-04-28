/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2003, 2005 Rob Buis <buis@kde.org>
 * SPDX-FileCopyrightText: 2007, 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "SvgGraphicContext.h"

SvgGraphicsContext::SvgGraphicsContext()
{
    strokeType = None;
    stroke.setLineStyle(Qt::NoPen, QVector<qreal>()); // default is no stroke
    stroke.setLineWidth(1.0);
    stroke.setCapStyle(Qt::FlatCap);
    stroke.setJoinStyle(Qt::MiterJoin);

    fillType = Solid;
    fillRule = Qt::WindingFill;
    fillColor = QColor(Qt::black); // default is black fill as per svg spec

    opacity = 1.0;

    currentColor = Qt::black;
    forcePercentage = false;

    display = true;

    clipRule = Qt::WindingFill;
    preserveWhitespace = false;

    letterSpacing = 0.0;
    wordSpacing = 0.0;
}
