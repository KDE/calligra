// This file is part of the KDE project
// SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-only

#include "TestFods.h"

#include "MockPart.h"

#include <core/Cell.h>
#include <core/CellStorage.h>
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
using namespace Qt::StringLiterals;

static QString valueToString(const Value &v)
{
    if (v.isString())
        return v.asString();
    if (v.isBoolean())
        return v.asBoolean() ? QStringLiteral("TRUE") : QStringLiteral("FALSE");
    if (v.isError())
        return v.errorMessage();
    if (v.isNumber())
        return QString::number(v.asFloat(), 'g', 15);
    return QStringLiteral("<empty>");
}

static bool isSelectedGroup(const QString &name)
{
#if defined(FODS_TEST_GROUP_ARRAY_DATABASE_DATE_TIME)
    return name.startsWith("array/"_L1) || name.startsWith("database/"_L1) || name.startsWith("date_time/"_L1);
#elif defined(FODS_TEST_GROUP_FINANCIAL_INFORMATION_LAMBDA_LOGICAL)
    return name.startsWith("financial/"_L1) || name.startsWith("information/"_L1) || name.startsWith("lambda/"_L1) || name.startsWith("logical/"_L1);
#elif defined(FODS_TEST_GROUP_MATHEMATICAL)
    return name.startsWith("mathematical/"_L1);
#elif defined(FODS_TEST_GROUP_STATISTICAL)
    return name.startsWith("statistical/"_L1);
#else
    return name.startsWith("spreadsheet/"_L1) || name.startsWith("text/"_L1) || name.startsWith("fods/"_L1);
#endif
}

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
        if (!isSelectedGroup(name))
            continue;
        if (name == "financial/pv.fods"_L1 || name == "financial/npv.fods"_L1 || name == "financial/pmt.fods"_L1 || name == "text/fods/bahttext.fods"_L1
            || name == "text/fods/clean.fods"_L1 || name == "text/fods/numbervalue.fods"_L1 || name == "text/fods/lower.fods"_L1
            || name == "date_time/fods/datedif.fods"_L1 || name == "array/fods/frequency.fods"_L1 || name == "spreadsheet/fods/column.fods"
            || name == "spreadsheet/fods/index.fods"_L1 || name == "spreadsheet/fods/lookup.fods"_L1 || name == "spreadsheet/fods/row.fods"_L1
            || name == "spreadsheet/fods/sheets.fods"_L1 || name == "spreadsheet/fods/xlookup.fods"_L1 || name.startsWith("statistical/fods/"_L1)
            || name == "fods/gammaln.precise.fods"_L1 || name == "fods/own_function.fods"_L1 || name == "fods/reference_operators.fods"_L1
            || name == "information/fods/iserr.fods"_L1 || name == "information/fods/iserror.fods"_L1 || name == "information/fods/type.fods"_L1
            || name == "lambda/fods/isomitted.fods"_L1 || name == "lambda/fods/map.fods"_L1 || name == "logical/fods/iferror.fods"_L1) {
            continue; // Known failures tracked separately while implementations are completed.
        }
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
        for (Cell cell = sheet->fullCellStorage()->firstInColumn(3, CellStorage::VisitContent); !cell.isNull();
             cell = sheet->fullCellStorage()->nextInColumn(3, cell.row(), CellStorage::VisitContent)) {
            if (!cell.isFormula())
                continue;
            const int row = cell.row();
            const QString formula = cell.userInput().trimmed();
            if (formula.startsWith(QLatin1String("=AND(")) || formula.startsWith(QLatin1String("=OR(")))
                continue;
            const Value value = cell.value();
            if (value.isBoolean() && !value.asBoolean()) {
                Cell a(sheet, 1, row);
                Cell b(sheet, 2, row);
                failures << QStringLiteral("%1!C%2: %3 (A%2=%4 B%2=%5)")
                                .arg(sheet->sheetName())
                                .arg(row)
                                .arg(formula)
                                .arg(valueToString(a.value()))
                                .arg(valueToString(b.value()));
            }
        }
    }

    QVERIFY2(failures.isEmpty(), qPrintable(QStringLiteral("%1 failing check(s):\n%2").arg(failures.count()).arg(failures.join('\n'))));
}

QTEST_MAIN(TestFods)
