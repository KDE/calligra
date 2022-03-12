/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_COPY_COMMAND
#define CALLIGRA_SHEETS_COPY_COMMAND

class QDomDocument;

namespace Calligra
{
namespace Sheets
{
class Region;

/**
 * \ingroup Commands
 * \brief Command to copy cell data
 */
namespace CopyCommand
{
/**
 * Saves the cell \p region as XML.
 * \param region the cell region to process
 * \param era set this to true if you want to encode relative references
 *            absolutely (they will be switched back to relative
 *            references during decoding) - used for cut to clipboard
 */
QDomDocument saveAsXml(const Region&, bool era = false);

/**
 * Saves the cell \p region as plain text.
 * \param region the cell region to process
 */
QString saveAsPlainText(const Region &region);

/**
 * Saves the cell \p region as HTML.
 */
QDomDocument saveAsHtml(const Region& region);

/**
 * Saves the cell \p region as CSV data.
 */
QString saveAsCSV(const Region& region);
}

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_COPY_COMMAND
