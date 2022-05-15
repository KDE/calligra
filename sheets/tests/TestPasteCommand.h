// This file is part of the KDE project
// SPDX-FileCopyrightText: 2012 Marijn Kruisselbrink <mkruisselbrink@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef CALLIGRA_SHEETS_PASTE_COMMAND_TEST
#define CALLIGRA_SHEETS_PASTE_COMMAND_TEST

#include <QObject>

namespace Calligra
{
namespace Sheets
{
class PasteCommandTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testKSpreadSnippet();
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_PASTE_COMMAND_TEST
