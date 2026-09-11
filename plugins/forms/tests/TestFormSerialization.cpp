/*
 * SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include <KoOdfForm.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>

#include <QBuffer>
#include <QTest>

using namespace Qt::StringLiterals;

namespace
{
KoOdfForm loadForm(const QString &kind, const QString &attributes = {}, const QString &children = {})
{
    const QString source =
        u"<f:form xmlns:f='urn:oasis:names:tc:opendocument:xmlns:form:1.0'>"
        "<f:%1 xml:id='control1' f:label='A label' %2>%3</f:%1></f:form>"_s.arg(kind, attributes, children);
    KoXmlDocument xml;
    if (!xml.setContent(source, true)) {
        qFatal("Invalid test XML");
    }
    KoOdfForm form;
    if (!form.loadOdf(xml.documentElement())) {
        qFatal("Unable to load test form");
    }
    return form;
}

KoOdfForm roundTrip(const KoOdfForm &form)
{
    QBuffer buffer;
    if (!buffer.open(QIODevice::WriteOnly)) {
        qFatal("Unable to open test buffer");
    }
    KoXmlWriter writer(&buffer);
    writer.startElement("root");
    writer.addAttribute("xmlns:form", "urn:oasis:names:tc:opendocument:xmlns:form:1.0");
    form.saveOdf(writer);
    writer.endElement();

    KoXmlDocument xml;
    if (!xml.setContent(buffer.data(), true)) {
        qFatal("Invalid serialized form XML");
    }
    KoOdfForm restored;
    if (!restored.loadOdf(xml.documentElement().firstChildElement())) {
        qFatal("Unable to reload serialized form");
    }
    return restored;
}
}

class TestFormSerialization : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void listSelectionRoundTrip()
    {
        const KoOdfForm form = loadForm(u"listbox"_s,
                                        u"f:dropdown='true'"_s,
                                        u"<f:option f:label='First' f:selected='true' f:current-selected='false'/>"
                                        "<f:option f:label='Second' f:current-selected='true'/>"_s);
        auto control = form.controlById(u"control1"_s);
        QCOMPARE(control->entries().size(), 2);
        QVERIFY(!control->entries()[0].selected);
        QVERIFY(control->entries()[1].selected);

        control->setName(u"Renamed"_s);
        KoOdfForm edited = form;
        QVERIFY(edited.setControlProperties(u"control1"_s, *control));
        const auto restored = roundTrip(edited);
        const auto copy = restored.controlById(u"control1"_s);
        QVERIFY(copy);
        QCOMPARE(copy->name(), u"Renamed"_s);
        QCOMPARE(copy->entries().size(), 2);
        QCOMPARE(copy->entries()[1].label, u"Second"_s);
        QVERIFY(copy->entries()[1].selected);
    }

    void addRemoveControlRoundTrip()
    {
        KoOdfForm form;
        const QString id = form.addControl(u"checkbox"_s);
        QVERIFY(!id.isEmpty());
        QVERIFY(form.controlById(id));
        QVERIFY(form.removeControl(id));
        QVERIFY(!form.controlById(id));
        QVERIFY(!form.removeControl(id));

        const QString first = form.addControl(u"text"_s);
        const QString second = form.addControl(u"radio"_s);
        QVERIFY(first != second);
        const auto restored = roundTrip(form);
        QCOMPARE(restored.controlKind(first), u"text"_s);
        QCOMPARE(restored.controlKind(second), u"radio"_s);
    }

    void relationshipsRoundTrip()
    {
        KoOdfForm form = loadForm(u"radio"_s, u"f:name='choices' f:value='yes' f:for='label1'"_s);
        auto control = form.controlById(u"control1"_s);
        QVERIFY(control);
        QCOMPARE(control->name(), u"choices"_s);
        QCOMPARE(control->formAttribute(u"for"_s), u"label1"_s);
        control->setFormAttribute(u"for"_s, u"label2"_s);
        control->setEventHandler(u"click"_s, u"macro://click"_s);
        QVERIFY(form.setControlProperties(u"control1"_s, *control));

        const auto restored = roundTrip(form);
        const auto copy = restored.controlById(u"control1"_s);
        QVERIFY(copy);
        QCOMPARE(copy->name(), u"choices"_s);
        QCOMPARE(copy->formAttribute(u"for"_s), u"label2"_s);
        QCOMPARE(copy->eventHandler(u"click"_s), u"macro://click"_s);
        QCOMPARE(copy->eventHandlers().value(u"click"_s), u"macro://click"_s);
    }

    void formEventsRoundTrip()
    {
        KoOdfForm form;
        form.setEventHandler(u"load"_s, u"macro://load"_s);
        form.setEventHandler(u"unload"_s, u"macro://unload"_s);
        QVERIFY(!form.isEmpty());
        QCOMPARE(form.eventHandler(u"load"_s), u"macro://load"_s);
        QCOMPARE(form.eventHandlers().size(), 2);

        const auto restored = roundTrip(form);
        QCOMPARE(restored.eventHandler(u"load"_s), u"macro://load"_s);
        QCOMPARE(restored.eventHandler(u"unload"_s), u"macro://unload"_s);
        QCOMPARE(restored.eventHandlers(), form.eventHandlers());

        KoOdfForm edited = restored;
        edited.setEventHandler(u"load"_s, {});
        QVERIFY(!edited.eventHandlers().contains(u"load"_s));
    }
};

QTEST_GUILESS_MAIN(TestFormSerialization)
#include "TestFormSerialization.moc"
