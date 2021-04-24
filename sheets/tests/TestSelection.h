/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_TEST_SELECTION
#define CALLIGRA_SHEETS_TEST_SELECTION

#include <QObject>

namespace Calligra
{
namespace Sheets
{

class TestSelection : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initialize();
    void update();
    void extend();
    void activeElement();
    void referenceSelectionMode();
    void covering();
    void splitting();
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_TEST_SELECTION
