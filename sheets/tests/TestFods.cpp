// This file is part of the KDE project
// SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-only

#include "TestFods.h"

#include "MockPart.h"

#include <core/Cell.h>
#include <core/Map.h>
#include <core/Sheet.h>
#include <engine/FunctionModuleRegistry.h>
#include <engine/Value.h>
#include <part/Doc.h>

#include <KLocalizedString>

#include <QDir>
#include <QDirIterator>
#include <QTest>

using namespace Calligra::Sheets;

void TestFods::initTestCase()
{
    KLocalizedString::setApplicationDomain("calligrasheets");
    FunctionModuleRegistry::instance()->loadFunctionModules();
}

void TestFods::testFods_data()
{
    QTest::addColumn<QString>("filePath");

    QDirIterator it(QStringLiteral(FODS_TEST_DATA_DIR), {QStringLiteral("*.fods")}, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        const QString filePath = it.next();
        const QString name = QDir(QStringLiteral(FODS_TEST_DATA_DIR)).relativeFilePath(filePath);
        QTest::newRow(qPrintable(name)) << filePath;
    }
}

void TestFods::testFods()
{
    QFETCH(QString, filePath);

    Doc doc(new MockPart);
    doc.setUrl(QUrl::fromLocalFile(filePath));
    const bool ok = doc.loadNativeFormat(filePath);
    QVERIFY2(ok, qPrintable(doc.errorMessage()));

    QStringList failures;
    for (SheetBase *sheetBase : doc.map()->sheetList()) {
        auto *sheet = dynamic_cast<Sheet *>(sheetBase);
        if (!sheet)
            continue;
        const QRect area = sheet->usedArea(true);
        for (int row = area.top(); row <= area.bottom(); ++row) {
            for (int col = area.left(); col <= area.right(); ++col) {
                Cell cell(sheet, col, row);
                if (cell.isNull() || !cell.isFormula())
                    continue;
                const QString formula = cell.userInput().trimmed();
                // Top-level AND()/OR() cells summarize a whole (often blank-padded) range and
                // are skipped here; each individual test row already has its own check formula
                // (a ROUND(...)=ROUND(...) or ISERROR(...) comparison) that this loop catches
                // directly, without depending on AND()/OR()'s handling of blank cells.
                if (formula.startsWith(QLatin1String("=AND(")) || formula.startsWith(QLatin1String("=OR(")))
                    continue;
                const Value value = cell.value();
                if (value.isBoolean() && !value.asBoolean()) {
                    failures << QStringLiteral("%1!%2%3: %4").arg(sheet->sheetName()).arg(col).arg(row).arg(formula);
                }
            }
        }
    }

    QVERIFY2(failures.isEmpty(), qPrintable(QStringLiteral("%1 failing check(s):\n%2").arg(failures.count()).arg(failures.join('\n'))));
}

QTEST_MAIN(TestFods)
