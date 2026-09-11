/*
 * SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include <KoOdfScript.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>

#include <QBuffer>
#include <QTest>

using namespace Qt::StringLiterals;

class TestKoOdfScript : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void roundTrip()
    {
        const QString source =
            u"<office:document-content xmlns:office='urn:oasis:names:tc:opendocument:xmlns:office:1.0' "
            "xmlns:script='urn:oasis:names:tc:opendocument:xmlns:script:1.0' xmlns:xlink='http://www.w3.org/1999/xlink'>"
            "<office:scripts><script:script script:language='ooo:Basic' script:name='Main'>"
            "print 1"
            "</script:script></office:scripts></office:document-content>"_s;
        KoXmlDocument xml;
        QVERIFY(xml.setContent(source, true));
        const auto scripts = KoOdfScript::loadScripts(xml.documentElement());
        QCOMPARE(scripts.size(), 1);
        QCOMPARE(scripts.first().language, u"ooo:Basic"_s);
        QCOMPARE(scripts.first().name, u"Main"_s);

        QBuffer buffer;
        QVERIFY(buffer.open(QIODevice::WriteOnly));
        KoXmlWriter writer(&buffer);
        writer.startElement("office:document-content");
        writer.addAttribute("xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0");
        writer.addAttribute("xmlns:script", "urn:oasis:names:tc:opendocument:xmlns:script:1.0");
        writer.addAttribute("xmlns:xlink", "http://www.w3.org/1999/xlink");
        KoOdfScript::saveScripts(writer, scripts);
        writer.endElement();

        KoXmlDocument restoredXml;
        QVERIFY(restoredXml.setContent(buffer.data(), true));
        const auto restored = KoOdfScript::loadScripts(restoredXml.documentElement());
        QCOMPARE(restored.size(), 1);
        QCOMPARE(restored.first().language, scripts.first().language);
        QCOMPARE(restored.first().name, scripts.first().name);
        QCOMPARE(restored.first().content, scripts.first().content);
    }
};

QTEST_GUILESS_MAIN(TestKoOdfScript)
#include "TestKoOdfScript.moc"
