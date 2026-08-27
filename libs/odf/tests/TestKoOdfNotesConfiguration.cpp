// SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include <QLoggingCategory>
#include <QObject>
#include <QTest>

#include "OdfRoundTripTestUtils.h"
#include <KoOdfNotesConfiguration.h>
#include <KoOdfNumberDefinition.h>
#include <KoXmlNS.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>

class TestKoOdfNotesConfiguration : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();

    void testLoad();
    void testSaveAndLoadRoundTrip();
};

void TestKoOdfNotesConfiguration::initTestCase()
{
    QLoggingCategory::setFilterRules(
        "*.debug=false\n"
        "calligra.lib.odf=true");
}

void TestKoOdfNotesConfiguration::testLoad()
{
    KoXmlDocument doc;
    const KoXmlElement root = OdfTestUtils::parseXmlFragment(
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<text:notes-configuration xmlns:text=\"urn:oasis:names:tc:opendocument:xmlns:text:1.0\""
        " xmlns:style=\"urn:oasis:names:tc:opendocument:xmlns:style:1.0\""
        " text:note-class=\"footnote\" text:citation-style-name=\"MyCitation\""
        " text:citation-body-style-name=\"MyCitationBody\" text:default-style-name=\"MyFootnote\""
        " text:master-page-name=\"Footer\" text:start-value=\"3\" style:num-format=\"1\""
        " text:start-numbering-at=\"page\" text:footnotes-position=\"document\">"
        "<text:note-continuation-notice-forward>continued...</text:note-continuation-notice-forward>"
        "<text:note-continuation-notice-backward>...continued</text:note-continuation-notice-backward>"
        "</text:notes-configuration>",
        doc);
    QVERIFY(!root.isNull());

    KoOdfNotesConfiguration config(KoOdfNotesConfiguration::Footnote);
    config.loadOdf(root);

    QCOMPARE(config.citationTextStyleName(), QString("MyCitation"));
    QCOMPARE(config.citationBodyTextStyleName(), QString("MyCitationBody"));
    QCOMPARE(config.defaultNoteParagraphStyleName(), QString("MyFootnote"));
    QCOMPARE(config.masterPage(), QString("Footer"));
    QCOMPARE(config.startValue(), 3);
    QCOMPARE(config.numberFormat().formatSpecification(), KoOdfNumberDefinition::Numeric);
    QCOMPARE(config.numberingScheme(), KoOdfNotesConfiguration::BeginAtPage);
    QCOMPARE(config.footnotesPosition(), KoOdfNotesConfiguration::Document);
    QCOMPARE(config.footnoteContinuationForward(), QString("continued..."));
    QCOMPARE(config.footnoteContinuationBackward(), QString("...continued"));
}

void TestKoOdfNotesConfiguration::testSaveAndLoadRoundTrip()
{
    KoOdfNotesConfiguration config(KoOdfNotesConfiguration::Endnote);
    config.setCitationTextStyle(nullptr);
    config.setMasterPage("Endnotes");
    config.setStartValue(5);
    KoOdfNumberDefinition numberFormat;
    numberFormat.setFormatSpecification(KoOdfNumberDefinition::RomanLowerCase);
    config.setNumberFormat(numberFormat);
    config.setNumberingScheme(KoOdfNotesConfiguration::BeginAtChapter);
    config.setFootnotesPosition(KoOdfNotesConfiguration::Section);
    config.setFootnoteContinuationForward("(cont.)");
    config.setFootnoteContinuationBackward("(cont. from prev.)");

    // KoOdfNotesConfiguration::saveOdf() writes its own complete
    // text:notes-configuration element, so it's written as the child of a
    // throwaway root that only exists to declare the namespaces it uses.
    KoXmlDocument doc;
    const KoXmlElement wrapper = OdfTestUtils::writeAndReparse(
        "office:styles",
        {{"office", KoXmlNS::office}, {"text", KoXmlNS::text}, {"style", KoXmlNS::style}},
        [&config](KoXmlWriter &writer) {
            config.saveOdf(&writer);
        },
        doc);
    QVERIFY(!wrapper.isNull());
    const KoXmlElement root = wrapper.firstChildElement();
    QVERIFY(!root.isNull());

    KoOdfNotesConfiguration loaded(KoOdfNotesConfiguration::Endnote);
    loaded.loadOdf(root);

    QCOMPARE(loaded.citationTextStyleName(), config.citationTextStyleName());
    QCOMPARE(loaded.citationBodyTextStyleName(), config.citationBodyTextStyleName());
    QCOMPARE(loaded.defaultNoteParagraphStyleName(), config.defaultNoteParagraphStyleName());
    QCOMPARE(loaded.masterPage(), config.masterPage());
    QCOMPARE(loaded.startValue(), config.startValue());
    QCOMPARE(loaded.numberFormat().formatSpecification(), config.numberFormat().formatSpecification());
    QCOMPARE(loaded.numberingScheme(), config.numberingScheme());
    QCOMPARE(loaded.footnotesPosition(), config.footnotesPosition());
    QCOMPARE(loaded.footnoteContinuationForward(), config.footnoteContinuationForward());
    QCOMPARE(loaded.footnoteContinuationBackward(), config.footnoteContinuationBackward());
}

QTEST_MAIN(TestKoOdfNotesConfiguration)
#include "TestKoOdfNotesConfiguration.moc"
