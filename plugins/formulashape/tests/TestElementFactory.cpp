// SPDX-License-Identifier: LGPL-2.0-or-later

#include "BasicElement.h"
#include "ElementFactory.h"

#include <QTest>

class TestElementFactory : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void createsKnownElements_data();
    void createsKnownElements();
    void createsUnknownElement();
};

void TestElementFactory::createsKnownElements_data()
{
    QTest::addColumn<QString>("tag");
    QTest::addColumn<ElementType>("type");

    QTest::newRow("identifier") << "mi" << Identifier;
    QTest::newRow("number") << "mn" << Number;
    QTest::newRow("operator") << "mo" << Operator;
    QTest::newRow("row") << "mrow" << Row;
    QTest::newRow("fraction") << "mfrac" << Fraction;
    QTest::newRow("root") << "mroot" << Root;
    QTest::newRow("subsup") << "msubsup" << SubSupScript;
    QTest::newRow("underover") << "munderover" << UnderOver;
    QTest::newRow("table") << "mtable" << Table;
    QTest::newRow("multiscript") << "mmultiscripts" << MultiScript;
}

void TestElementFactory::createsKnownElements()
{
    QFETCH(QString, tag);
    QFETCH(ElementType, type);

    auto element = ElementFactory::createElement(tag, nullptr);
    QVERIFY(element);
    QCOMPARE(element->elementType(), type);
}

void TestElementFactory::createsUnknownElement()
{
    auto element = ElementFactory::createElement("not-mathml", nullptr);
    QVERIFY(element);
    QCOMPARE(element->elementType(), Unknown);
}

QTEST_MAIN(TestElementFactory)

#include "TestElementFactory.moc"
