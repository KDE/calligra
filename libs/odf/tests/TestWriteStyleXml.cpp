/* This file is part of the KDE project
 * Copyright (C) 2017 Jos van den Oever <jos@vandenoever.info>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include <KoXmlWriter.h>
#include <writeodf/writeodf.h>
#include <writeodf/writeodfoffice.h>
#include <writeodf/writeodfstyle.h>
#include <writeodf/writeodftext.h>
#include <writeodf/writeodfofficestyle.h>

#include <QString>
#include <QBuffer>
#include <QTest>
#include <QLoggingCategory>

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
    QLoggingCategory::setFilterRules("*.debug=false\n"
        "calligra.lib.odf=true\ncalligra.lib.store=true");
}

void TestWriteStyleXml::testWriteRegionLeft()
{
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    {
        KoXmlWriter writer(&buffer);
        writer.startDocument(0);
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
    const QString e = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<office:document-styles office:version=\"1.2\" xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\" xmlns:style=\"urn:oasis:names:tc:opendocument:xmlns:style:1.0\" xmlns:text=\"urn:oasis:names:tc:opendocument:xmlns:text:1.0\">\n"
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

