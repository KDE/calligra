/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef CALLIGRA_SHEETS_SHEET_TEST
#define CALLIGRA_SHEETS_SHEET_TEST

#include <QObject>

namespace Calligra
{
namespace Sheets
{

class Map;
class Sheet;

class SheetTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void init();
    void cleanup();

    void testRemoveRows_data();
    void testRemoveRows();
    void testRemoveColumns_data();
    void testRemoveColumns();

    void testDocumentToCellCoordinates_data();
    void testDocumentToCellCoordinates();

    //    void testCompareRows();

private:
    Map *m_map;
    Sheet *m_sheet;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_SHEET_TEST
