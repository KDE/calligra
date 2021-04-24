/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2004-2006 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
   SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOODFGRAPHICSTYLES_H
#define KOODFGRAPHICSTYLES_H

#include "koodf_export.h"

#include <QTransform>

class QBrush;
class QPen;
class QString;
class QSizeF;

class KoGenStyle;
class KoGenStyles;
class KoStyleStack;

class KoOdfStylesReader;

namespace KoOdfGraphicStyles
{
    KOODF_EXPORT void saveOdfFillStyle(KoGenStyle &styleFill, KoGenStyles& mainStyles, const QBrush &brush);

    KOODF_EXPORT void saveOdfStrokeStyle(KoGenStyle &styleStroke, KoGenStyles &mainStyles, const QPen &pen);

    KOODF_EXPORT QString saveOdfHatchStyle(KoGenStyles &mainStyles, const QBrush &brush);

    /// Saves gradient style of brush into mainStyles and returns the styles name
    KOODF_EXPORT QString saveOdfGradientStyle(KoGenStyles &mainStyles, const QBrush &brush);

    /// Loads gradient style from style stack and stylesReader adapted to the given size and returns a brush
    KOODF_EXPORT QBrush loadOdfGradientStyle(const KoStyleStack &styleStack, const KoOdfStylesReader &stylesReader, const QSizeF &size);

    /// Loads gradient style with the given name from style stack and stylesReader adapted to the given size and returns a brush
    KOODF_EXPORT QBrush loadOdfGradientStyleByName(const KoOdfStylesReader &stylesReader, const QString &styleName, const QSizeF &size);

    KOODF_EXPORT QBrush loadOdfFillStyle(const KoStyleStack &styleStack, const QString &fill,  const KoOdfStylesReader &stylesReader);

    KOODF_EXPORT QPen loadOdfStrokeStyle(const KoStyleStack &styleStack, const QString &stroke, const KoOdfStylesReader &stylesReader);

    /// Helper function to parse a transformation attribute
    KOODF_EXPORT QTransform loadTransformation(const QString &transformation);

    /// Helper function to create a transformation attribute
    KOODF_EXPORT QString saveTransformation(const QTransform &transformation, bool appendTranslateUnit = true);
}

#endif /* KOODFGRAPHICSTYLES_H */
