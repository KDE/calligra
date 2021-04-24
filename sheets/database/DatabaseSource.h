/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_DATABASE_SOURCE
#define CALLIGRA_SHEETS_DATABASE_SOURCE

namespace Calligra
{
namespace Sheets
{

/**
 * OpenDocument, 8.6.2 Database Source SQL
 * OpenDocument, 8.6.3 Database Source Table
 * OpenDocument, 8.6.4 Database Source Query
 */
class DatabaseSource
{
public:
    /**
     * Constructor.
     */
    DatabaseSource();

    /**
     * Destructor.
     */
    virtual ~DatabaseSource();

private:
    class Private;
    Private * const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_DATABASE_SOURCE
