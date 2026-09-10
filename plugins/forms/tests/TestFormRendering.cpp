/*
 * SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "KoFormShapeRenderer.h"

#include <KoViewConverter.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>
#include <QApplication>
#include <QBuffer>
#include <QPainter>
#include <QProxyStyle>
#include <QStyleOption>
#include <QTest>

using namespace Qt::StringLiterals;

namespace
{
const QStringList kinds = {u"text"_s,     u"textarea"_s, u"formatted-text"_s, u"password"_s,    u"hidden"_s, u"file"_s,     u"fixed-text"_s,
                           u"number"_s,   u"date"_s,     u"time"_s,           u"value-range"_s, u"button"_s, u"checkbox"_s, u"radio"_s,
                           u"combobox"_s, u"listbox"_s,  u"image"_s,          u"image-frame"_s, u"frame"_s,  u"grid"_s,     u"generic-control"_s};

KoOdfForm loadForm(const QString &kind, const QString &attributes = {}, const QString &children = {})
{
    // Deliberately use an alternate namespace prefix and xml:id without form:id.
    const QString source =
        u"<f:form xmlns:f='urn:oasis:names:tc:opendocument:xmlns:form:1.0'>"
        "<f:%1 xml:id='control1' f:label='A label' %2>%3</f:%1></f:form>"_s.arg(kind, attributes, children);
    KoXmlDocument xml;
    if (!xml.setContent(source, true))
        qFatal("Invalid test XML");
    KoOdfForm form;
    if (!form.loadOdf(xml.documentElement()))
        qFatal("Unable to load test form");
    return form;
}

class RecordingStyle : public QProxyStyle
{
public:
    RecordingStyle()
        : QProxyStyle(u"Fusion"_s)
    {
    }
    mutable QList<ControlElement> controls;
    mutable QList<ComplexControl> complexControls;
    mutable QStringList texts;
    mutable QStyle::State buttonState;
    mutable QRect buttonRect;
    mutable bool validOptionTypes = true;
    mutable int fieldFrameWidth = 0;
    bool zeroFrameMetric = false;

    void clear()
    {
        controls.clear();
        complexControls.clear();
        texts.clear();
        buttonState = {};
        buttonRect = {};
        validOptionTypes = true;
        fieldFrameWidth = 0;
    }

    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override
    {
        controls.append(element);
        if (element == CE_CheckBox || element == CE_RadioButton || element == CE_PushButton) {
            validOptionTypes &= qstyleoption_cast<const QStyleOptionButton *>(option) != nullptr;
            buttonState = option->state;
            buttonRect = option->rect;
        }
        QProxyStyle::drawControl(element, option, painter, widget);
    }

    int pixelMetric(PixelMetric metric, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const override
    {
        if (zeroFrameMetric && metric == PM_DefaultFrameWidth && !widget)
            return 0;
        return QProxyStyle::pixelMetric(metric, option, widget);
    }

    void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override
    {
        if (element == PE_PanelLineEdit || element == PE_Frame) {
            const auto *frame = qstyleoption_cast<const QStyleOptionFrame *>(option);
            validOptionTypes &= frame != nullptr;
            if (frame)
                fieldFrameWidth = frame->lineWidth;
        }
        QProxyStyle::drawPrimitive(element, option, painter, widget);
    }

    void drawComplexControl(ComplexControl element, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget = nullptr) const override
    {
        complexControls.append(element);
        if (element == CC_ComboBox)
            validOptionTypes &= qstyleoption_cast<const QStyleOptionComboBox *>(option) != nullptr;
        if (element == CC_SpinBox)
            validOptionTypes &= qstyleoption_cast<const QStyleOptionSpinBox *>(option) != nullptr;
        if (element == CC_ScrollBar || element == CC_Slider)
            validOptionTypes &= qstyleoption_cast<const QStyleOptionSlider *>(option) != nullptr;
        QProxyStyle::drawComplexControl(element, option, painter, widget);
    }

    void drawItemText(QPainter *painter,
                      const QRect &rect,
                      int flags,
                      const QPalette &palette,
                      bool enabled,
                      const QString &text,
                      QPalette::ColorRole role = QPalette::NoRole) const override
    {
        texts.append(text);
        QProxyStyle::drawItemText(painter, rect, flags, palette, enabled, text, role);
    }
};
}

class TestFormRendering : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase()
    {
        m_style = new RecordingStyle;
        QApplication::setStyle(m_style);
    }

    void allControls_data()
    {
        QTest::addColumn<QString>("kind");
        for (const auto &kind : kinds)
            QTest::newRow(kind.toUtf8().constData()) << kind;
    }

    void allControls()
    {
        QFETCH(QString, kind);
        m_style->clear();
        KoOdfForm form = loadForm(kind, u"f:value='Initial' f:current-value='Current' f:disabled='true'"_s);
        QVERIFY(!form.isEmpty());
        QCOMPARE(form.controlKind(u"control1"_s), kind);
        auto control = form.controlById(u"control1"_s);
        QVERIFY(control);
        QCOMPARE(control->label(), u"A label"_s);
        QVERIFY(control->disabled());
        QBuffer buffer;
        QVERIFY(buffer.open(QIODevice::WriteOnly));
        KoXmlWriter writer(&buffer);
        writer.startElement("office:forms");
        writer.addAttribute("xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0");
        writer.addAttribute("xmlns:form", "urn:oasis:names:tc:opendocument:xmlns:form:1.0");
        form.saveOdf(writer);
        writer.endElement();
        KoXmlDocument xml;
        QVERIFY(xml.setContent(buffer.data(), true));
        KoOdfForm restored;
        QVERIFY(restored.loadOdf(xml.documentElement().firstChildElement()));
        QCOMPARE(restored.controlKind(u"control1"_s), kind);
        auto copy = restored.controlById(u"control1"_s);
        QVERIFY(copy);
        QCOMPARE(copy->label(), control->label());
        QCOMPARE(copy->currentValue(), control->currentValue());
        QCOMPARE(copy->disabled(), control->disabled());

        QImage image(240, 100, QImage::Format_ARGB32_Premultiplied);
        image.fill(Qt::transparent);
        const QImage empty = image;
        QPainter painter(&image);
        const auto transform = painter.transform();
        const auto font = painter.font();
        KoViewConverter converter;
        paintFormControl(painter, converter, QSizeF(200, 80), kind, copy.get(), {});
        QCOMPARE(painter.transform(), transform);
        QCOMPARE(painter.font(), font);
        painter.end();
        QCOMPARE(image == empty, kind == "hidden"_L1);
        QVERIFY(m_style->validOptionTypes);
    }

    void listSelectionRoundTrip()
    {
        KoOdfForm form = loadForm(u"listbox"_s,
                                  u"f:dropdown='true'"_s,
                                  u"<f:option f:label='First' f:selected='true' f:current-selected='false'/>"
                                  "<f:option f:label='Second' f:current-selected='true'/>"_s);
        auto control = form.controlById(u"control1"_s);
        QCOMPARE(control->entries().size(), 2);
        QVERIFY(!control->entries()[0].selected);
        QVERIFY(control->entries()[1].selected);
        control->setName(u"Renamed"_s);
        QVERIFY(form.setControlProperties(u"control1"_s, *control));
        QBuffer buffer;
        QVERIFY(buffer.open(QIODevice::WriteOnly));
        KoXmlWriter writer(&buffer);
        writer.startElement("root");
        writer.addAttribute("xmlns:form", "urn:oasis:names:tc:opendocument:xmlns:form:1.0");
        form.saveOdf(writer);
        writer.endElement();
        KoXmlDocument xml;
        QVERIFY(xml.setContent(buffer.data(), true));
        KoOdfForm restored;
        QVERIFY(restored.loadOdf(xml.documentElement().firstChildElement()));
        auto copy = restored.controlById(u"control1"_s);
        QVERIFY(copy);
        QCOMPARE(copy->name(), u"Renamed"_s);
        QCOMPARE(copy->entries().size(), 2);
        QCOMPARE(copy->entries()[1].label, u"Second"_s);
        QVERIFY(copy->entries()[1].selected);
        render(restored);
        QVERIFY(m_style->complexControls.contains(QStyle::CC_ComboBox));
        QVERIFY(m_style->texts.contains(u"Second"_s));
    }

    void checkStatesAndZoom()
    {
        auto form = loadForm(u"checkbox"_s, u"f:state='unchecked' f:current-state='checked' f:disabled='true'"_s);
        render(form);
        QVERIFY(m_style->controls.contains(QStyle::CE_CheckBox));
        QVERIFY(m_style->buttonState.testFlag(QStyle::State_On));
        QVERIFY(!m_style->buttonState.testFlag(QStyle::State_Enabled));
        QVERIFY(m_style->validOptionTypes);
        const QRect rect = m_style->buttonRect;
        render(form, 2.0);
        QCOMPARE(m_style->buttonRect, rect); // Style metrics stay fixed; the painter scales them.
        render(loadForm(u"checkbox"_s, u"f:current-state='unknown'"_s));
        QVERIFY(m_style->buttonState.testFlag(QStyle::State_NoChange));
        render(loadForm(u"radio"_s, u"f:selected='true' f:current-selected='false'"_s));
        QVERIFY(m_style->controls.contains(QStyle::CE_RadioButton));
        QVERIFY(m_style->buttonState.testFlag(QStyle::State_Off));
    }

    void fieldValues()
    {
        render(loadForm(u"text"_s, u"f:value='Default' f:current-value=''"_s));
        QVERIFY(!m_style->texts.contains(u"Default"_s));
        render(loadForm(u"text"_s, u"f:value='Default'"_s));
        QVERIFY(m_style->texts.contains(u"Default"_s));
        render(loadForm(u"password"_s, u"f:value='secret' f:echo-char='*'"_s));
        QVERIFY(m_style->texts.contains(u"******"_s));
        QVERIFY(!m_style->texts.contains(u"secret"_s));
        render(loadForm(u"number"_s, u"f:spin-button='true' f:value='42'"_s));
        QVERIFY(m_style->complexControls.contains(QStyle::CC_SpinBox));
        QVERIFY(m_style->validOptionTypes);
        QVERIFY(m_style->texts.contains(u"42"_s));
        render(loadForm(u"value-range"_s, u"f:orientation='vertical' f:value='42'"_s));
        QVERIFY(m_style->complexControls.contains(QStyle::CC_ScrollBar));
    }

    void framelessWidgetMetric()
    {
        // Match Breeze's metric for direct painting without a QWidget.
        m_style->zeroFrameMetric = true;
        render(loadForm(u"textarea"_s, u"f:current-value='rere re'"_s));
        const int textFrameWidth = m_style->fieldFrameWidth;
        render(loadForm(u"listbox"_s));
        const int listFrameWidth = m_style->fieldFrameWidth;
        m_style->zeroFrameMetric = false;
        QVERIFY(textFrameWidth > 0);
        QVERIFY(listFrameWidth > 0);
    }

    void gallery()
    {
        const QString output = qEnvironmentVariable("CALLIGRA_FORM_GALLERY");
        if (output.isEmpty())
            return;
        QImage image(900, 1050, QImage::Format_ARGB32_Premultiplied);
        image.fill(Qt::white);
        QPainter painter(&image);
        KoViewConverter converter;
        converter.setZoom(1.5);
        for (int i = 0; i < kinds.size(); ++i) {
            painter.save();
            painter.translate(20 + (i % 3) * 300, 30 + (i / 3) * 150);
            painter.setPen(Qt::black);
            painter.drawText(QPoint(0, 0), kinds[i]);
            painter.translate(0, 12);
            const QString children =
                kinds[i] == "listbox"_L1 ? u"<f:option f:label='First'/><f:option f:label='Second' f:current-selected='true'/>"_s : QString();
            auto form =
                loadForm(kinds[i], u"f:value='42' f:current-state='checked' f:current-selected='true' f:spin-button='true' f:dropdown='true'"_s, children);
            auto control = form.controlById(u"control1"_s);
            const bool tall = kinds[i] == "grid"_L1 || kinds[i] == "frame"_L1 || kinds[i] == "textarea"_L1 || kinds[i] == "image-frame"_L1;
            paintFormControl(painter, converter, QSizeF(175, tall ? 65 : 25), kinds[i], control.get(), {});
            painter.restore();
        }
        painter.end();
        QVERIFY(image.save(output));
    }

private:
    void render(const KoOdfForm &form, qreal zoom = 1.0)
    {
        m_style->clear();
        auto control = form.controlById(u"control1"_s);
        QImage image(500, 160, QImage::Format_ARGB32_Premultiplied);
        image.fill(Qt::transparent);
        QPainter painter(&image);
        KoViewConverter converter;
        converter.setZoom(zoom);
        paintFormControl(painter, converter, QSizeF(200, 60), form.controlKind(u"control1"_s), control.get(), {});
    }
    RecordingStyle *m_style = nullptr;
};

QTEST_MAIN(TestFormRendering)
#include "TestFormRendering.moc"
