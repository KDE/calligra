/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_COPY_COMMAND
#define CALLIGRA_SHEETS_COPY_COMMAND

#include "../sheets_ui_export.h"
#include <QString>

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
 * Saves the cell \p region as a text with coordinates - used for copy/pasting.
 * \param region the cell region to process
 */
QString CALLIGRA_SHEETS_UI_EXPORT saveAsSnippet(const Region &);

/**
 * Saves the cell \p region as plain text.
 * \param region the cell region to process
 */
QString CALLIGRA_SHEETS_UI_EXPORT saveAsPlainText(const Region &region);

} // namespace CopyCommand

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_COPY_COMMAND
