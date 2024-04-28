/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2011 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 2011 Sebastian Sauer <sebastian.sauer@kdab.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_CELL_TEST
#define CALLIGRA_SHEETS_CELL_TEST

#include <QObject>

class KoXmlDocument;
class QString;

namespace Calligra
{
namespace Sheets
{

class CellTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testRichText();

private:
    KoXmlDocument xmlDocument(const QString &content);
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_CELL_STORAGE_TEST
