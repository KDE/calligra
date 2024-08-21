/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2017 Jos van den Oever <jos@vandenoever.info>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include <KoXmlWriter.h>
#include <writeodf/writeodf.h>
#include <writeodf/writeodfoffice.h>
#include <writeodf/writeodfofficestyle.h>
#include <writeodf/writeodfstyle.h>
#include <writeodf/writeodftext.h>

#include <QBuffer>
#include <QLoggingCategory>
#include <QString>
#include <QTest>

using namespace writeodf;

class TestWriteStyleXml : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void testWriteRegionLeft();
};

void TestWriteStyleXml::initTestCase()
{
    QLoggingCategory::setFilterRules(
        "*.debug=false\n"
        "calligra.lib.odf=true\ncalligra.lib.store=true");
}

void TestWriteStyleXml::testWriteRegionLeft()
{
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    {
        KoXmlWriter writer(&buffer);
        writer.startDocument(nullptr);
        office_document_styles styles(&writer);
        styles.addAttribute("xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0");
        styles.addAttribute("xmlns:style", "urn:oasis:names:tc:opendocument:xmlns:style:1.0");
        styles.addAttribute("xmlns:text", "urn:oasis:names:tc:opendocument:xmlns:text:1.0");
        office_master_styles master_styles(styles.add_office_master_styles());
        style_master_page master_page(master_styles.add_style_master_page("Standard", "Layout"));
        style_header header(master_page.add_style_header());
        style_region_left left(header.add_style_region_left());
        text_p p(left.add_text_p());
        p.addTextNode("left");
    }
    const QString r = buffer.buffer();
    const QString e =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<office:document-styles office:version=\"1.2\" xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\" "
        "xmlns:style=\"urn:oasis:names:tc:opendocument:xmlns:style:1.0\" xmlns:text=\"urn:oasis:names:tc:opendocument:xmlns:text:1.0\">\n"
        " <office:master-styles>\n"
        "  <style:master-page style:name=\"Standard\" style:page-layout-name=\"Layout\">\n"
        "   <style:header>\n"
        "    <style:region-left>\n"
        "     <text:p>left</text:p>\n"
        "    </style:region-left>\n"
        "   </style:header>\n"
        "  </style:master-page>\n"
        " </office:master-styles>\n"
        "</office:document-styles>";
    QCOMPARE(r, e);
}

QTEST_GUILESS_MAIN(TestWriteStyleXml)
#include <TestWriteStyleXml.moc>
