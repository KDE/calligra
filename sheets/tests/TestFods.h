// This file is part of the KDE project
// SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-only

#pragma once

#include <QObject>

namespace Calligra
{
namespace Sheets
{

// Loads flat-ODF (.fods) spreadsheets copied from other office suites' test suites (e.g.
// LibreOffice) through Calligra Sheets' real ODF import + recalculation, then checks that
// every self-check formula in the file (a boolean-valued formula cell, other than a top-level
// AND()/OR() summary) evaluates to true.
class TestFods : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void testFods_data();
    void testFods();
};

} // namespace Sheets
} // namespace Calligra
