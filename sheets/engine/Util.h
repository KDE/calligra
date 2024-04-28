/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006, 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_UTIL
#define CALLIGRA_SHEETS_UTIL

#include <QString>

#include "sheets_engine_export.h"

namespace Calligra
{
namespace Sheets
{

namespace Util
{
/**
 * Call this function to decode the text of a column label to an integer,
 * e.g. 1 for A and 27 for AA.
 * Converted are all characters matching [A-Za-z]+ regular expression, the rest is ignored.
 * 0 is returned if no characters match.
 */
CALLIGRA_SHEETS_ENGINE_EXPORT int decodeColumnLabelText(const QString &labelText);

/**
 * Call this function to decode the text of a row label to an integer,
 * e.g. B7 is translated to 7.
 */
CALLIGRA_SHEETS_ENGINE_EXPORT int decodeRowLabelText(const QString &labelText);

/**
 * Returns true if the given text is a cell-reference.
 *
 * This is an optimized version of QRegExp("^(\\$?)([a-zA-Z]+)(\\$?)([0-9]+)$")
 * to check if the given string is a cell-reference like $A$1 or D17. Note
 * that this will return false for cell-ranges like A1:B2 or cell-references
 * given with sheet-name like Sheet1:A1.
 *
 * @param text The text to check
 * @param startPos The position in the string where we should start to check
 */
CALLIGRA_SHEETS_ENGINE_EXPORT bool isCellReference(const QString &text, int startPos = 0);

/**
 * Generate and return the ODF formula for this cell (\p thisRow, \p thisColumn) based on the formula in the
 * defined cell (\p referencedRow, \p referencedColumn ).
 */
CALLIGRA_SHEETS_ENGINE_EXPORT QString adjustFormulaReference(const QString &formula, int referencedRow, int referencedColumn, int thisRow, int thisColumn);

// Return true when it's a reference to cell from sheet.
CALLIGRA_SHEETS_ENGINE_EXPORT bool localReferenceAnchor(const QString &_ref);

CALLIGRA_SHEETS_ENGINE_EXPORT bool isCellnameCharacter(const QChar &c);

} // namespace Util

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_UTIL
