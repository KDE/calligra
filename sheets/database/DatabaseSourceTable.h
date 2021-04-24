/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_DATABASE_SOURCE_TABLE
#define CALLIGRA_SHEETS_DATABASE_SOURCE_TABLE

#include "DatabaseSource.h"

namespace Calligra
{
namespace Sheets
{

/**
 * OpenDocument, 8.6.3 Database Source Table
 */
class DatabaseSourceTable : public DatabaseSource
{
public:
    /**
     * Constructor.
     */
    DatabaseSourceTable();

    /**
     * Destructor.
     */
    ~DatabaseSourceTable() override;

private:
    class Private;
    Private * const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_DATABASE_SOURCE_TABLE
