// SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include <QLoggingCategory>
#include <QObject>
#include <QTest>

#include "OdfRoundTripTestUtils.h"
#include <KoGenStyle.h>
#include <KoGenStyles.h>
#include <KoPageLayout.h>
#include <KoUnit.h>
#include <KoXmlNS.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>

class TestKoPageLayout : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();

    void testLoad();
    void testSaveAndLoadRoundTrip();
};

void TestKoPageLayout::initTestCase()
{
    QLoggingCategory::setFilterRules(
        "*.debug=false\n"
        "calligra.lib.odf=true");
}

void TestKoPageLayout::testLoad()
{
    KoXmlDocument doc;
    const KoXmlElement root = OdfTestUtils::parseXmlFragment(
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<style:page-layout xmlns:style=\"urn:oasis:names:tc:opendocument:xmlns:style:1.0\""
        " xmlns:fo=\"urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0\" style:name=\"PM1\">"
        "<style:page-layout-properties fo:page-width=\"210mm\" fo:page-height=\"297mm\""
        " style:print-orientation=\"landscape\" fo:margin=\"15mm\" fo:padding=\"3mm\"/>"
        "</style:page-layout>",
        doc);
    QVERIFY(!root.isNull());

    KoPageLayout layout;
    layout.loadOdf(root);

    QVERIFY(qFuzzyCompare(layout.width, MM_TO_POINT(210.0)));
    QVERIFY(qFuzzyCompare(layout.height, MM_TO_POINT(297.0)));
    QCOMPARE(layout.orientation, KoPageFormat::Landscape);
    QVERIFY(qFuzzyCompare(layout.leftMargin, MM_TO_POINT(15.0)));
    QVERIFY(qFuzzyCompare(layout.topMargin, MM_TO_POINT(15.0)));
    QVERIFY(qFuzzyCompare(layout.rightMargin, MM_TO_POINT(15.0)));
    QVERIFY(qFuzzyCompare(layout.bottomMargin, MM_TO_POINT(15.0)));
    QVERIFY(qFuzzyCompare(layout.leftPadding, MM_TO_POINT(3.0)));
    QVERIFY(qFuzzyCompare(layout.topPadding, MM_TO_POINT(3.0)));
    QVERIFY(qFuzzyCompare(layout.rightPadding, MM_TO_POINT(3.0)));
    QVERIFY(qFuzzyCompare(layout.bottomPadding, MM_TO_POINT(3.0)));
}

void TestKoPageLayout::testSaveAndLoadRoundTrip()
{
    KoPageLayout layout;
    layout.width = MM_TO_POINT(148.0);
    layout.height = MM_TO_POINT(210.0);
    layout.orientation = KoPageFormat::Portrait;
    layout.leftMargin = MM_TO_POINT(10.0);
    layout.rightMargin = MM_TO_POINT(12.0);
    layout.topMargin = MM_TO_POINT(14.0);
    layout.bottomMargin = MM_TO_POINT(16.0);
    layout.leftPadding = MM_TO_POINT(1.0);
    layout.rightPadding = MM_TO_POINT(2.0);
    layout.topPadding = MM_TO_POINT(3.0);
    layout.bottomPadding = MM_TO_POINT(4.0);
    layout.border.setBorderStyle(KoBorder::LeftBorder, KoBorder::BorderSolid);
    layout.border.setBorderColor(KoBorder::LeftBorder, QColor("#000000"));
    layout.border.setBorderWidth(KoBorder::LeftBorder, 0.5);

    // KoPageLayout::saveOdf() returns a KoGenStyle for a whole style:page-layout
    // element (not just its properties), so it's written with writeStyle()
    // rather than writeStyleProperties() -- an empty KoGenStyles collection is
    // enough since this style has no parent style to resolve.
    KoGenStyle style = layout.saveOdf();
    KoGenStyles emptyStyles;

    KoXmlDocument doc;
    const KoXmlElement wrapper = OdfTestUtils::writeAndReparse(
        "office:automatic-styles",
        {{"office", KoXmlNS::office}, {"style", KoXmlNS::style}, {"fo", KoXmlNS::fo}, {"calligra", KoXmlNS::calligra}},
        [&style, &emptyStyles](KoXmlWriter &writer) {
            style.writeStyle(&writer, emptyStyles, "style:page-layout", "PM1", "style:page-layout-properties");
        },
        doc);
    QVERIFY(!wrapper.isNull());
    const KoXmlElement root = wrapper.firstChildElement();
    QVERIFY(!root.isNull());

    KoPageLayout loaded;
    loaded.loadOdf(root);

    QVERIFY(qFuzzyCompare(loaded.width, layout.width));
    QVERIFY(qFuzzyCompare(loaded.height, layout.height));
    QCOMPARE(loaded.orientation, layout.orientation);
    QVERIFY(qFuzzyCompare(loaded.leftMargin, layout.leftMargin));
    QVERIFY(qFuzzyCompare(loaded.rightMargin, layout.rightMargin));
    QVERIFY(qFuzzyCompare(loaded.topMargin, layout.topMargin));
    QVERIFY(qFuzzyCompare(loaded.bottomMargin, layout.bottomMargin));
    QVERIFY(qFuzzyCompare(loaded.leftPadding, layout.leftPadding));
    QVERIFY(qFuzzyCompare(loaded.rightPadding, layout.rightPadding));
    QVERIFY(qFuzzyCompare(loaded.topPadding, layout.topPadding));
    QVERIFY(qFuzzyCompare(loaded.bottomPadding, layout.bottomPadding));
    QCOMPARE(loaded.border.borderStyle(KoBorder::LeftBorder), layout.border.borderStyle(KoBorder::LeftBorder));
    QCOMPARE(loaded.border.borderColor(KoBorder::LeftBorder), layout.border.borderColor(KoBorder::LeftBorder));
    QCOMPARE(loaded.border.borderWidth(KoBorder::LeftBorder), layout.border.borderWidth(KoBorder::LeftBorder));
}

QTEST_MAIN(TestKoPageLayout)
#include "TestKoPageLayout.moc"
