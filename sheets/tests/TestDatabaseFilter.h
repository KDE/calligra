/* This file is part of the KDE project
   Copyright 2011 Marijn Kruisselbrink <mkruisselbrink@kde.org>

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

#ifndef CALLIGRA_SHEETS_DATABASE_FILTER_TEST
#define CALLIGRA_SHEETS_DATABASE_FILTER_TEST

#include <QObject>

namespace Calligra
{
namespace Sheets
{

class DatabaseFilterTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testIsEmpty();
    void testEmptyEquals();
    void testSimpleEquals();
    void testNotEquals1();
    void testNotEquals2();
    void testAndEquals();
    void testOrEquals();
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_DATABASE_FILTER_TEST
