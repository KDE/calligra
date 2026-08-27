// SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include <QLoggingCategory>
#include <QObject>
#include <QTest>

#include "OdfRoundTripTestUtils.h"
#include <KoColumns.h>
#include <KoGenStyle.h>
#include <KoXmlNS.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>

namespace
{
// KoColumns::saveOdf() registers a whole "style:columns" child element on a
// KoGenStyle (under SectionType, since columns are commonly a section
// property); this writes that out as <style:section-properties> and returns
// it, reparsed -- the element loadOdf() expects (it looks for a
// style:columns child of the element it's given).
KoXmlElement writeColumnsPropertiesAndReparse(const KoColumns &columns, KoXmlDocument &doc)
{
    KoGenStyle style(KoGenStyle::SectionAutoStyle, "section");
    columns.saveOdf(style);

    const KoXmlElement wrapper = OdfTestUtils::writeAndReparse(
        "office:automatic-styles",
        {{"office", KoXmlNS::office}, {"style", KoXmlNS::style}, {"fo", KoXmlNS::fo}},
        [&style](KoXmlWriter &writer) {
            style.writeStyleProperties(&writer, KoGenStyle::SectionType);
        },
        doc);
    return wrapper.firstChildElement();
}
}

class TestKoColumns : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();

    void testLoad();
    void testSaveAndLoadRoundTrip_uniformColumns();
    void testSaveAndLoadRoundTrip_explicitColumnData();
};

void TestKoColumns::initTestCase()
{
    QLoggingCategory::setFilterRules(
        "*.debug=false\n"
        "calligra.lib.odf=true");
}

void TestKoColumns::testLoad()
{
    KoXmlDocument doc;
    const KoXmlElement root = OdfTestUtils::parseXmlFragment(
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<style:section-properties xmlns:style=\"urn:oasis:names:tc:opendocument:xmlns:style:1.0\""
        " xmlns:fo=\"urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0\">"
        "<style:columns fo:column-count=\"3\" fo:column-gap=\"12pt\">"
        "<style:column-sep style:style=\"dashed\" style:width=\"1pt\" style:height=\"80%\""
        " style:color=\"#ff0000\" style:vertical-align=\"middle\"/>"
        "</style:columns>"
        "</style:section-properties>",
        doc);
    QVERIFY(!root.isNull());

    KoColumns columns;
    columns.loadOdf(root);

    QCOMPARE(columns.count, 3);
    QCOMPARE(columns.gapWidth, qreal(12.0));
    QCOMPARE(columns.separatorStyle, KoColumns::Dashed);
    QCOMPARE(columns.separatorWidth, qreal(1.0));
    QCOMPARE(columns.separatorHeight, 80);
    QCOMPARE(columns.separatorColor, QColor("#ff0000"));
    QCOMPARE(columns.separatorVerticalAlignment, KoColumns::AlignVCenter);
    QVERIFY(columns.columnData.isEmpty());
}

void TestKoColumns::testSaveAndLoadRoundTrip_uniformColumns()
{
    KoColumns columns;
    columns.count = 2;
    columns.gapWidth = 10.0;
    columns.separatorStyle = KoColumns::Solid;
    columns.separatorColor = QColor("#00ff00");
    columns.separatorVerticalAlignment = KoColumns::AlignBottom;
    columns.separatorWidth = 0.5;
    columns.separatorHeight = 90;

    KoXmlDocument doc;
    const KoXmlElement root = writeColumnsPropertiesAndReparse(columns, doc);
    QVERIFY(!root.isNull());

    KoColumns loaded;
    loaded.loadOdf(root);

    QCOMPARE(loaded.count, columns.count);
    QCOMPARE(loaded.gapWidth, columns.gapWidth);
    QCOMPARE(loaded.separatorStyle, columns.separatorStyle);
    QCOMPARE(loaded.separatorColor, columns.separatorColor);
    QCOMPARE(loaded.separatorVerticalAlignment, columns.separatorVerticalAlignment);
    QCOMPARE(loaded.separatorWidth, columns.separatorWidth);
    QCOMPARE(loaded.separatorHeight, columns.separatorHeight);
    QVERIFY(loaded.columnData.isEmpty());
}

void TestKoColumns::testSaveAndLoadRoundTrip_explicitColumnData()
{
    // With explicit per-column data, count must match columnData.size() or
    // loadOdf() discards it -- see the warnOdf check in KoColumns::loadOdf().
    KoColumns columns;
    columns.count = 2;
    columns.separatorStyle = KoColumns::None;
    columns.columnData.append(KoColumns::ColumnDatum(1.0, 2.0, 3.0, 4.0, 1));
    columns.columnData.append(KoColumns::ColumnDatum(5.0, 6.0, 7.0, 8.0, 2));

    KoXmlDocument doc;
    const KoXmlElement root = writeColumnsPropertiesAndReparse(columns, doc);
    QVERIFY(!root.isNull());

    KoColumns loaded;
    loaded.loadOdf(root);

    QCOMPARE(loaded.count, columns.count);
    QCOMPARE(loaded.columnData, columns.columnData);
}

QTEST_MAIN(TestKoColumns)
#include "TestKoColumns.moc"
