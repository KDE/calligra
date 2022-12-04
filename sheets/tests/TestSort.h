/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2011 Juan Aquino <utcl95@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef CALLIGRA_SHEETS_TEST_SORT
#define CALLIGRA_SHEETS_TEST_SORT

#include <QObject>

namespace Calligra
{
namespace Sheets
{

class Map;

class TestSort : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void AscendingOrder();
    void DescendingOrder();

private:
    Map *m_map;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_TEST_SORT
