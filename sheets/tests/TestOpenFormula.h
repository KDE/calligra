// This file is part of the KDE project
// SPDX-FileCopyrightText: 2007 Ariya Hidayat <ariya@kde.org>
// SPDX-FileCopyrightText: 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
// SPDX-FileCopyrightText: 2005 Tomas Mecir <mecirt@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-only

#ifndef CALLIGRA_SHEETS_TEST_OPENFORMULA
#define CALLIGRA_SHEETS_TEST_OPENFORMULA

#include <QObject>

namespace Calligra
{
namespace Sheets
{
class Value;

class TestOpenFormula : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void testEvaluation();
    //    void testFormulaConversion();
    void testReferenceLoading();
    void testReferenceSaving();

private:
    Value evaluate(const QString &, Value &);
    QString convertToOpenFormula(const QString &expr);
    QString convertFromOpenFormula(const QString &expr);
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_TEST_OPENFORMULA
