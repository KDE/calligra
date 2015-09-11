/* This file is part of the KDE project
   Copyright 2011 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   Copyright 2011 Sebastian Sauer <sebastian.sauer@kdab.com>

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
