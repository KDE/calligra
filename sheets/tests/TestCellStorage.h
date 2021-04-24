/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2011 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_CELL_STORAGE_TEST
#define CALLIGRA_SHEETS_CELL_STORAGE_TEST

#include <QObject>

namespace Calligra
{
namespace Sheets
{

class CellStorageTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testMergedCellsInsertRowBug();
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_CELL_STORAGE_TEST
