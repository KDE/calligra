/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2011 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
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
