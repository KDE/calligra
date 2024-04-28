/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>
   SPDX-FileCopyrightText: 2002, 2003 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2003 Nicolas GOUTTE <goutte@kde.org>
   SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOPAGEFORMAT_H
#define KOPAGEFORMAT_H

#include "koodf_export.h"

#include <QPagedPaintDevice>

/// The page formats calligra supports
namespace KoPageFormat
{
/**
 * @brief Represents the paper format a document shall be printed on.
 *
 * For compatibility reasons, and because of screen and custom,
 * this enum doesn't map to QPrinter::PageSize but KoPageFormat::printerPageSize
 * does the conversion.
 */
enum Format : int {
    IsoA3Size,
    IsoA4Size,
    IsoA5Size,
    UsLetterSize,
    UsLegalSize,
    ScreenSize,
    CustomSize,
    IsoB5Size,
    UsExecutiveSize,
    IsoA0Size,
    IsoA1Size,
    IsoA2Size,
    IsoA6Size,
    IsoA7Size,
    IsoA8Size,
    IsoA9Size,
    IsoB0Size,
    IsoB1Size,
    IsoB10Size,
    IsoB2Size,
    IsoB3Size,
    IsoB4Size,
    IsoB6Size,
    IsoC5Size,
    UsComm10Size,
    IsoDLSize,
    UsFolioSize,
    UsLedgerSize,
    UsTabloidSize
};

/**
 *  Represents the orientation of a printed document.
 */
enum Orientation { Portrait, Landscape };

/**
 * @brief Convert a Format into a KPrinter::PageSize.
 *
 * If format is 'screen' it will use A4 landscape.
 * If format is 'custom' it will use A4 portrait.
 * (you may want to take care of those cases separately).
 * Usually passed to QPrinter::setPageSize().
 */
KOODF_EXPORT QPageSize printerPageSize(Format format);

/**
 * Returns the width (in mm) for a given page format and orientation
 * 'Custom' isn't supported by this function, obviously.
 */
KOODF_EXPORT qreal width(Format format, Orientation orientation = Landscape);

/**
 * Returns the height (in mm) for a given page format and orientation
 * 'Custom' isn't supported by this function, obviously.
 */
KOODF_EXPORT qreal height(Format format, Orientation orientation = Landscape);

/**
 * Returns the internal name of the given page format.
 * Use for saving.
 */
KOODF_EXPORT QString formatString(Format format);

/**
 * Convert a format string (internal name) to a page format value.
 * Use for loading.
 */
KOODF_EXPORT Format formatFromString(const QString &string);

/**
 * Returns the default format (based on the KControl settings)
 */
KOODF_EXPORT Format defaultFormat();

/**
 * Returns the translated name of the given page format.
 * Use for showing the user.
 */
KOODF_EXPORT QString name(Format format);

/**
 * Lists the translated names of all the available formats
 */
KOODF_EXPORT QStringList localizedPageFormatNames();

/**
 * Lists the non-translated names of all the available formats
 */
KOODF_EXPORT QStringList pageFormatNames();

/**
 * Try to find the paper format for the given width and height (in mm).
 * Useful to some import filters.
 */
KOODF_EXPORT Format guessFormat(qreal width, qreal height);
}

#endif
