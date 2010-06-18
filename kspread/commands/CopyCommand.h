/* This file is part of the KDE project
   Copyright 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_COPY_COMMAND
#define KSPREAD_COPY_COMMAND

#include <QDomDocument>

namespace KSpread
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
     * Saves the cell \p region as HTML.
     */
    QDomDocument saveAsHtml(const Region& region);

    /**
     * Saves the cell \p region as CSV data.
     */
    QString saveAsCSV(const Region& region);
}

} // namespace KSpread

#endif // KSPREAD_COPY_COMMAND
