// This file is part of the KDE project
// SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
// SPDX-License-Identifier: LGPL-2.0-only

#ifndef CALLIGRA_SHEETS_TEST_STYLESTORAGE
#define CALLIGRA_SHEETS_TEST_STYLESTORAGE

#include <QObject>

namespace Calligra
{
namespace Sheets
{

class TestStyleStorage : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testGarbageCollection();
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_TEST_STYLESTORAGE
