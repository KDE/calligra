// SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include <QLoggingCategory>
#include <QObject>
#include <QTest>

#include "OdfRoundTripTestUtils.h"
#include <KoBorder.h>
#include <KoGenStyle.h>
#include <KoXmlNS.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>

namespace
{
// Writes `border`'s properties into a style:paragraph-properties element (via
// KoGenStyle::writeStyleProperties()) and returns that element, reparsed.
KoXmlElement writeBorderPropertiesAndReparse(const KoBorder &border, KoXmlDocument &doc)
{
    KoGenStyle style(KoGenStyle::ParagraphAutoStyle, "paragraph");
    border.saveOdf(style, KoGenStyle::ParagraphType);

    const KoXmlElement wrapper = OdfTestUtils::writeAndReparse(
        "office:automatic-styles",
        {{"office", KoXmlNS::office}, {"style", KoXmlNS::style}, {"fo", KoXmlNS::fo}, {"calligra", KoXmlNS::calligra}},
        [&style](KoXmlWriter &writer) {
            style.writeStyleProperties(&writer, KoGenStyle::ParagraphType);
        },
        doc);
    return wrapper.firstChildElement();
}
}

class TestKoBorder : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();

    void testLoad();
    void testSaveAndLoadRoundTrip_uniform();
    void testSaveAndLoadRoundTrip_perSide();
};

void TestKoBorder::initTestCase()
{
    QLoggingCategory::setFilterRules(
        "*.debug=false\n"
        "calligra.lib.odf=true");
}

void TestKoBorder::testLoad()
{
    KoXmlDocument doc;
    const KoXmlElement root = OdfTestUtils::parseXmlFragment(
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<style:paragraph-properties xmlns:style=\"urn:oasis:names:tc:opendocument:xmlns:style:1.0\""
        " xmlns:fo=\"urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0\""
        " fo:border=\"1.5pt solid #ff0000\"/>",
        doc);
    QVERIFY(!root.isNull());

    KoBorder border;
    QVERIFY(border.loadOdf(root));

    for (KoBorder::BorderSide side : {KoBorder::LeftBorder, KoBorder::TopBorder, KoBorder::RightBorder, KoBorder::BottomBorder}) {
        QCOMPARE(border.borderStyle(side), KoBorder::BorderSolid);
        QCOMPARE(border.borderColor(side), QColor("#ff0000"));
        QCOMPARE(border.borderWidth(side), qreal(1.5));
    }
}

void TestKoBorder::testSaveAndLoadRoundTrip_uniform()
{
    // Same style/color/width on all four sides -- exercises the merged
    // "fo:border" (rather than fo:border-left/-top/-right/-bottom) path.
    KoBorder border;
    for (KoBorder::BorderSide side : {KoBorder::LeftBorder, KoBorder::TopBorder, KoBorder::RightBorder, KoBorder::BottomBorder}) {
        border.setBorderColor(side, QColor("#123456"));
        border.setBorderStyle(side, KoBorder::BorderDashed);
        border.setBorderWidth(side, 2.0);
    }

    KoXmlDocument doc;
    const KoXmlElement root = writeBorderPropertiesAndReparse(border, doc);
    QVERIFY(!root.isNull());

    KoBorder loaded;
    QVERIFY(loaded.loadOdf(root));

    for (KoBorder::BorderSide side : {KoBorder::LeftBorder, KoBorder::TopBorder, KoBorder::RightBorder, KoBorder::BottomBorder}) {
        QCOMPARE(loaded.borderStyle(side), border.borderStyle(side));
        QCOMPARE(loaded.borderColor(side), border.borderColor(side));
        QCOMPARE(loaded.borderWidth(side), border.borderWidth(side));
    }
}

void TestKoBorder::testSaveAndLoadRoundTrip_perSide()
{
    // Different style/color/width per side -- exercises the individual
    // fo:border-left/-top/-right/-bottom path.
    KoBorder border;
    border.setBorderColor(KoBorder::LeftBorder, QColor("#ff0000"));
    border.setBorderStyle(KoBorder::LeftBorder, KoBorder::BorderSolid);
    border.setBorderWidth(KoBorder::LeftBorder, 1.0);

    border.setBorderColor(KoBorder::TopBorder, QColor("#00ff00"));
    border.setBorderStyle(KoBorder::TopBorder, KoBorder::BorderDotted);
    border.setBorderWidth(KoBorder::TopBorder, 2.0);

    border.setBorderColor(KoBorder::RightBorder, QColor("#0000ff"));
    border.setBorderStyle(KoBorder::RightBorder, KoBorder::BorderDashed);
    border.setBorderWidth(KoBorder::RightBorder, 3.0);

    border.setBorderColor(KoBorder::BottomBorder, QColor("#ffff00"));
    border.setBorderStyle(KoBorder::BottomBorder, KoBorder::BorderSolid);
    border.setBorderWidth(KoBorder::BottomBorder, 4.0);

    KoXmlDocument doc;
    const KoXmlElement root = writeBorderPropertiesAndReparse(border, doc);
    QVERIFY(!root.isNull());

    KoBorder loaded;
    QVERIFY(loaded.loadOdf(root));

    for (KoBorder::BorderSide side : {KoBorder::LeftBorder, KoBorder::TopBorder, KoBorder::RightBorder, KoBorder::BottomBorder}) {
        QCOMPARE(loaded.borderStyle(side), border.borderStyle(side));
        QCOMPARE(loaded.borderColor(side), border.borderColor(side));
        QCOMPARE(loaded.borderWidth(side), border.borderWidth(side));
    }
}

QTEST_MAIN(TestKoBorder)
#include "TestKoBorder.moc"
