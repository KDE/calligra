/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoOdfForm.h"

#include "KoXmlNS.h"
#include "KoXmlReader.h"
#include "KoXmlWriter.h"
#include <array>

#include <QDomDocument>
#include <QTextStream>
#include <algorithm>
#include <type_traits>

using namespace Qt::StringLiterals;

namespace
{
using ControlKind = KoOdfForm::ControlKind;
constexpr std::array<std::pair<QLatin1StringView, ControlKind>, 21> controlKinds = {{
    {"text"_L1, ControlKind::Text},
    {"textarea"_L1, ControlKind::Textarea},
    {"formatted-text"_L1, ControlKind::FormattedText},
    {"number"_L1, ControlKind::Number},
    {"date"_L1, ControlKind::Date},
    {"time"_L1, ControlKind::Time},
    {"button"_L1, ControlKind::Button},
    {"checkbox"_L1, ControlKind::Checkbox},
    {"radio"_L1, ControlKind::Radio},
    {"combobox"_L1, ControlKind::Combobox},
    {"listbox"_L1, ControlKind::Listbox},
    {"password"_L1, ControlKind::Password},
    {"hidden"_L1, ControlKind::Hidden},
    {"file"_L1, ControlKind::File},
    {"fixed-text"_L1, ControlKind::FixedText},
    {"value-range"_L1, ControlKind::ValueRange},
    {"image"_L1, ControlKind::Image},
    {"image-frame"_L1, ControlKind::ImageFrame},
    {"frame"_L1, ControlKind::Frame},
    {"grid"_L1, ControlKind::Grid},
    {"generic-control"_L1, ControlKind::GenericControl},
}};
}

bool KoOdfForm::Control::loadOdf(const KoXmlElement &e)
{
    if (e.isNull())
        return false;
    m_name = e.attributeNS(KoXmlNS::form, u"name"_s);
    m_id = e.attributeNS(KoXmlNS::xml, u"id"_s, e.attributeNS(KoXmlNS::form, u"id"_s));
    m_title = e.attributeNS(KoXmlNS::form, u"title"_s);
    m_value = e.attributeNS(KoXmlNS::form, u"value"_s);
    m_currentValue = e.attributeNS(KoXmlNS::form, u"current-value"_s);
    m_dataField = e.attributeNS(KoXmlNS::form, u"data-field"_s);
    m_linkedCell = e.attributeNS(KoXmlNS::form, u"linked-cell"_s);
    m_xformsBind = e.attributeNS(u"http://www.w3.org/2002/xforms"_s, u"bind"_s);
    m_disabled = e.attributeNS(KoXmlNS::form, u"disabled"_s) == QLatin1String("true");
    m_printable = e.attributeNS(KoXmlNS::form, u"printable"_s) != QLatin1String("false");
    m_readOnly = e.attributeNS(KoXmlNS::form, u"readonly"_s) == QLatin1String("true");
    m_inputRequired = e.attributeNS(KoXmlNS::form, u"input-required"_s) == QLatin1String("true");
    m_tabStop = e.attributeNS(KoXmlNS::form, u"tab-stop"_s) != QLatin1String("false");
    m_tabIndex = e.attributeNS(KoXmlNS::form, u"tab-index"_s).toInt();
    m_formAttributes.clear();
    // Keep control-specific attributes and children so rendering and common-property
    // edits do not discard selection state, list contents, or extension properties.
    const QStringList common = {u"name"_s,
                                u"id"_s,
                                u"title"_s,
                                u"value"_s,
                                u"current-value"_s,
                                u"data-field"_s,
                                u"linked-cell"_s,
                                u"disabled"_s,
                                u"printable"_s,
                                u"readonly"_s,
                                u"input-required"_s,
                                u"tab-stop"_s,
                                u"tab-index"_s};
    const auto attributes = e.attributeFullNames();
    for (const auto &attribute : attributes) {
        if (attribute.first == KoXmlNS::form && !common.contains(attribute.second))
            m_formAttributes.insert(attribute.second, e.attributeNS(attribute.first, attribute.second));
    }
    m_entries.clear();
    m_childrenXml.clear();
    QTextStream stream(&m_childrenXml, QIODevice::WriteOnly);
    for (KoXmlNode node = e.firstChild(); !node.isNull(); node = node.nextSibling()) {
        if (!node.isElement())
            continue;
        const auto child = node.toElement();
        QDomDocument childDocument;
        KoXml::asQDomElement(childDocument, child);
        childDocument.documentElement().save(stream, -1);
        if (child.namespaceURI() != KoXmlNS::form)
            continue;
        if (child.localName() == "option"_L1 || child.localName() == "item"_L1 || child.localName() == "column"_L1) {
            Entry entry;
            entry.label = child.attributeNS(KoXmlNS::form, u"label"_s, child.text());
            entry.selected = child.attributeNS(KoXmlNS::form, u"current-selected"_s, child.attributeNS(KoXmlNS::form, u"selected"_s)) == "true"_L1;
            m_entries.append(entry);
        }
    }
    stream.flush();
    return true;
}
void KoOdfForm::Control::saveCommonAttributes(KoXmlWriter &w) const
{
    if (!m_name.isEmpty())
        w.addAttribute("form:name", m_name);
    if (!m_id.isEmpty())
        w.addAttribute("form:id", m_id);
    if (!m_title.isEmpty())
        w.addAttribute("form:title", m_title);
    if (!m_value.isEmpty())
        w.addAttribute("form:value", m_value);
    if (!m_currentValue.isNull())
        w.addAttribute("form:current-value", m_currentValue);
    if (!m_dataField.isEmpty())
        w.addAttribute("form:data-field", m_dataField);
    if (!m_linkedCell.isEmpty())
        w.addAttribute("form:linked-cell", m_linkedCell);
    if (!m_xformsBind.isEmpty())
        w.addAttribute("xforms:bind", m_xformsBind);
    w.addAttribute("form:disabled", m_disabled);
    w.addAttribute("form:printable", m_printable);
    w.addAttribute("form:readonly", m_readOnly);
    w.addAttribute("form:input-required", m_inputRequired);
    w.addAttribute("form:tab-stop", m_tabStop);
    if (m_tabIndex)
        w.addAttribute("form:tab-index", m_tabIndex);
    for (auto it = m_formAttributes.cbegin(); it != m_formAttributes.cend(); ++it)
        w.addAttribute((u"form:"_s + it.key()).toUtf8().constData(), it.value());
}
#define C(N, T)                                                                                                                                                \
    bool KoOdfForm::N::loadOdf(const KoXmlElement &e)                                                                                                          \
    {                                                                                                                                                          \
        return e.localName() == QLatin1String(T) && Control::loadOdf(e);                                                                                       \
    }                                                                                                                                                          \
    void KoOdfForm::N::saveOdf(KoXmlWriter &w) const                                                                                                           \
    {                                                                                                                                                          \
        w.startElement("form:" T);                                                                                                                             \
        saveCommonAttributes(w);                                                                                                                               \
        saveChildren(w);                                                                                                                                       \
        w.endElement();                                                                                                                                        \
    }
C(Text, "text")
C(Textarea, "textarea")
C(FormattedText, "formatted-text")
C(Password, "password")
C(Hidden, "hidden")
C(File, "file")
C(FixedText, "fixed-text")
C(Number, "number")
C(Date, "date")
C(Time, "time")
C(ValueRange, "value-range")
C(Button, "button")
C(Checkbox, "checkbox")
C(Radio, "radio")
C(Combobox, "combobox")
C(Listbox, "listbox")
C(Image, "image")
C(ImageFrame, "image-frame")
C(Frame, "frame")
C(Grid, "grid")
C(GenericControl, "generic-control")
#undef C
#define G(N)                                                                                                                                                   \
    QString KoOdfForm::Control::N() const                                                                                                                      \
    {                                                                                                                                                          \
        return m_##N;                                                                                                                                          \
    }
G(name) G(id) G(title) G(value) G(currentValue) G(dataField) G(linkedCell) G(xformsBind) bool KoOdfForm::Control::disabled() const
{
    return m_disabled;
}
bool KoOdfForm::Control::printable() const
{
    return m_printable;
}
bool KoOdfForm::Control::readOnly() const
{
    return m_readOnly;
}
bool KoOdfForm::Control::inputRequired() const
{
    return m_inputRequired;
}
int KoOdfForm::Control::tabIndex() const
{
    return m_tabIndex;
}
bool KoOdfForm::Control::tabStop() const
{
    return m_tabStop;
}
void KoOdfForm::Control::setName(const QString &v)
{
    m_name = v;
}
void KoOdfForm::Control::setId(const QString &v)
{
    m_id = v;
}
void KoOdfForm::Control::setTitle(const QString &v)
{
    m_title = v;
}
void KoOdfForm::Control::setValue(const QString &v)
{
    m_value = v;
}
void KoOdfForm::Control::setCurrentValue(const QString &v)
{
    m_currentValue = v;
}
void KoOdfForm::Control::setDataField(const QString &v)
{
    m_dataField = v;
}
void KoOdfForm::Control::setLinkedCell(const QString &v)
{
    m_linkedCell = v;
}
void KoOdfForm::Control::setXformsBind(const QString &v)
{
    m_xformsBind = v;
}
void KoOdfForm::Control::setDisabled(bool v)
{
    m_disabled = v;
}
void KoOdfForm::Control::setPrintable(bool v)
{
    m_printable = v;
}
void KoOdfForm::Control::setReadOnly(bool v)
{
    m_readOnly = v;
}
void KoOdfForm::Control::setInputRequired(bool v)
{
    m_inputRequired = v;
}
void KoOdfForm::Control::setTabIndex(int v)
{
    m_tabIndex = qMax(0, v);
}
void KoOdfForm::Control::setTabStop(bool v)
{
    m_tabStop = v;
}

QString KoOdfForm::Text::maxLength() const
{
    return formAttribute(u"max-length"_s);
}

bool KoOdfForm::Text::multiLine() const
{
    return formAttribute(u"multi-line"_s) == "true"_L1;
}

QString KoOdfForm::Text::echoChar() const
{
    return formAttribute(u"echo-char"_s);
}

void KoOdfForm::Text::setMaxLength(const QString &value)
{
    bool valid = false;
    const int length = value.toInt(&valid);
    if (value.isEmpty() || (valid && length >= 0)) {
        setFormAttribute(u"max-length"_s, value);
    }
}

void KoOdfForm::Text::setMultiLine(bool value)
{
    setFormAttribute(u"multi-line"_s, value ? u"true"_s : u"false"_s);
}

void KoOdfForm::Text::setEchoChar(const QString &value)
{
    setFormAttribute(u"echo-char"_s, value);
}

QString KoOdfForm::Number::minValue() const
{
    return formAttribute(u"min-value"_s);
}

QString KoOdfForm::Number::maxValue() const
{
    return formAttribute(u"max-value"_s);
}

QString KoOdfForm::Number::stepSize() const
{
    return formAttribute(u"step-size"_s);
}

void KoOdfForm::Number::setMinValue(const QString &value)
{
    bool valid = false;
    value.toDouble(&valid);
    if (value.isEmpty() || valid) {
        setFormAttribute(u"min-value"_s, value);
    }
}

void KoOdfForm::Number::setMaxValue(const QString &value)
{
    bool valid = false;
    value.toDouble(&valid);
    if (value.isEmpty() || valid) {
        setFormAttribute(u"max-value"_s, value);
    }
}

void KoOdfForm::Number::setStepSize(const QString &value)
{
    bool valid = false;
    const double step = value.toDouble(&valid);
    if (value.isEmpty() || (valid && step > 0)) {
        setFormAttribute(u"step-size"_s, value);
    }
}

bool KoOdfForm::Button::defaultButton() const
{
    return formAttribute(u"default-button"_s) == "true"_L1;
}

bool KoOdfForm::Button::toggle() const
{
    return formAttribute(u"toggle"_s) == "true"_L1;
}

void KoOdfForm::Button::setDefaultButton(bool value)
{
    setFormAttribute(u"default-button"_s, value ? u"true"_s : u"false"_s);
}

void KoOdfForm::Button::setToggle(bool value)
{
    setFormAttribute(u"toggle"_s, value ? u"true"_s : u"false"_s);
}

bool KoOdfForm::Checkbox::selected() const
{
    return formAttribute(u"current-selected"_s) == "true"_L1 || formAttribute(u"selected"_s) == "true"_L1 || formAttribute(u"current-state"_s) == "checked"_L1
        || formAttribute(u"state"_s) == "checked"_L1;
}

bool KoOdfForm::Checkbox::tristate() const
{
    return formAttribute(u"tristate"_s) == "true"_L1;
}

void KoOdfForm::Checkbox::setSelected(bool value)
{
    setFormAttribute(u"selected"_s, value ? u"true"_s : u"false"_s);
    setFormAttribute(u"current-selected"_s, value ? u"true"_s : u"false"_s);
}

void KoOdfForm::Checkbox::setTristate(bool value)
{
    setFormAttribute(u"tristate"_s, value ? u"true"_s : u"false"_s);
}

bool KoOdfForm::Combobox::autoComplete() const
{
    return formAttribute(u"autocomplete"_s) == "true"_L1;
}

void KoOdfForm::Combobox::setAutoComplete(bool value)
{
    setFormAttribute(u"autocomplete"_s, value ? u"true"_s : u"false"_s);
}

bool KoOdfForm::Listbox::multiple() const
{
    return formAttribute(u"multiple"_s) == "true"_L1;
}

bool KoOdfForm::Listbox::dropdown() const
{
    return formAttribute(u"dropdown"_s) == "true"_L1;
}

QString KoOdfForm::Listbox::listSource() const
{
    return formAttribute(u"list-source"_s);
}

void KoOdfForm::Listbox::setMultiple(bool value)
{
    setFormAttribute(u"multiple"_s, value ? u"true"_s : u"false"_s);
}

void KoOdfForm::Listbox::setDropdown(bool value)
{
    setFormAttribute(u"dropdown"_s, value ? u"true"_s : u"false"_s);
}

void KoOdfForm::Listbox::setListSource(const QString &value)
{
    setFormAttribute(u"list-source"_s, value);
}

QString KoOdfForm::Image::imageData() const
{
    return formAttribute(u"image-data"_s);
}

QString KoOdfForm::Image::imagePosition() const
{
    return formAttribute(u"image-position"_s);
}

QString KoOdfForm::Image::imageAlign() const
{
    return formAttribute(u"image-align"_s);
}

void KoOdfForm::Image::setImageData(const QString &value)
{
    setFormAttribute(u"image-data"_s, value);
}

void KoOdfForm::Image::setImagePosition(const QString &value)
{
    setFormAttribute(u"image-position"_s, value);
}

void KoOdfForm::Image::setImageAlign(const QString &value)
{
    setFormAttribute(u"image-align"_s, value);
}
bool KoOdfForm::Property::loadOdf(const KoXmlElement &e)
{
    if (e.isNull())
        return false;
    m_name = e.attributeNS(KoXmlNS::form, u"property-name"_s);
    m_value = e.attributeNS(KoXmlNS::form, u"value"_s);
    return true;
}
void KoOdfForm::Property::saveOdf(KoXmlWriter &w) const
{
    w.startElement("form:property");
    w.addAttribute("form:property-name", m_name);
    w.addAttribute("form:value", m_value);
    w.endElement();
}
QString KoOdfForm::Property::name() const
{
    return m_name;
}
QString KoOdfForm::Property::value() const
{
    return m_value;
}
void KoOdfForm::Property::setName(const QString &v)
{
    m_name = v;
}
void KoOdfForm::Property::setValue(const QString &v)
{
    m_value = v;
}

bool KoOdfForm::Model::loadOdf(const KoXmlElement &element)
{
    if (element.isNull() || element.localName() != QLatin1String("model"))
        return false;

    QDomDocument document;
    KoXml::asQDomElement(document, element);
    if (document.documentElement().isNull())
        return false;

    QTextStream stream(&m_xml, QIODevice::WriteOnly);
    document.documentElement().save(stream, -1);
    stream.flush();
    return !m_xml.isEmpty();
}

void KoOdfForm::Model::saveOdf(KoXmlWriter &writer) const
{
    if (!m_xml.isEmpty())
        writer.addCompleteElement(m_xml.constData());
}

QByteArray KoOdfForm::Model::xml() const
{
    return m_xml;
}

void KoOdfForm::Model::setXml(const QByteArray &xml)
{
    m_xml = xml;
}

bool KoOdfForm::loadOdf(const KoXmlElement &e)
{
    if (e.isNull() || e.localName() != QLatin1String("form"))
        return false;
    m_name = e.attributeNS(KoXmlNS::form, u"name"_s);
    m_command = e.attributeNS(KoXmlNS::form, u"command"_s);
    m_datasource = e.attributeNS(KoXmlNS::form, u"datasource"_s);
    m_method = e.attributeNS(KoXmlNS::form, u"method"_s);
    m_xformsSubmission = e.attributeNS(u"http://www.w3.org/2002/xforms"_s, u"submission"_s);
    m_texts.clear();
    m_textareas.clear();
    m_formattedTexts.clear();
    m_numbers.clear();
    m_dates.clear();
    m_times.clear();
    m_buttons.clear();
    m_checkboxes.clear();
    m_radios.clear();
    m_comboboxes.clear();
    m_listboxes.clear();
    m_passwords.clear();
    m_hiddenControls.clear();
    m_files.clear();
    m_fixedTexts.clear();
    m_valueRanges.clear();
    m_images.clear();
    m_imageFrames.clear();
    m_frames.clear();
    m_grids.clear();
    m_genericControls.clear();
    m_models.clear();
    for (KoXmlNode n = e.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        const KoXmlElement c = n.toElement();
        const QString t = c.localName();
        if (t == QLatin1String("model") && c.namespaceURI() == QLatin1String("http://www.w3.org/2002/xforms")) {
            Model v;
            if (v.loadOdf(c))
                m_models.append(v);
        } else if (t == QLatin1String("text")) {
            Text v;
            if (v.loadOdf(c))
                m_texts.append(v);
        } else if (t == QLatin1String("textarea")) {
            Textarea v;
            if (v.loadOdf(c))
                m_textareas.append(v);
        } else if (t == QLatin1String("formatted-text")) {
            FormattedText v;
            if (v.loadOdf(c))
                m_formattedTexts.append(v);
        } else if (t == QLatin1String("number")) {
            Number v;
            if (v.loadOdf(c))
                m_numbers.append(v);
        } else if (t == QLatin1String("date")) {
            Date v;
            if (v.loadOdf(c))
                m_dates.append(v);
        } else if (t == QLatin1String("time")) {
            Time v;
            if (v.loadOdf(c))
                m_times.append(v);
        } else if (t == QLatin1String("button")) {
            Button v;
            if (v.loadOdf(c))
                m_buttons.append(v);
        } else if (t == QLatin1String("checkbox")) {
            Checkbox v;
            if (v.loadOdf(c))
                m_checkboxes.append(v);
        } else if (t == QLatin1String("radio")) {
            Radio v;
            if (v.loadOdf(c))
                m_radios.append(v);
        } else if (t == QLatin1String("combobox")) {
            Combobox v;
            if (v.loadOdf(c))
                m_comboboxes.append(v);
        } else if (t == QLatin1String("listbox")) {
            Listbox v;
            if (v.loadOdf(c))
                m_listboxes.append(v);
        } else if (t == "password"_L1) {
            Password v;
            if (v.loadOdf(c))
                m_passwords.append(v);
        } else if (t == "hidden"_L1) {
            Hidden v;
            if (v.loadOdf(c))
                m_hiddenControls.append(v);
        } else if (t == "file"_L1) {
            File v;
            if (v.loadOdf(c))
                m_files.append(v);
        } else if (t == "fixed-text"_L1) {
            FixedText v;
            if (v.loadOdf(c))
                m_fixedTexts.append(v);
        } else if (t == "value-range"_L1) {
            ValueRange v;
            if (v.loadOdf(c))
                m_valueRanges.append(v);
        } else if (t == "image"_L1) {
            Image v;
            if (v.loadOdf(c))
                m_images.append(v);
        } else if (t == "image-frame"_L1) {
            ImageFrame v;
            if (v.loadOdf(c))
                m_imageFrames.append(v);
        } else if (t == "frame"_L1) {
            Frame v;
            if (v.loadOdf(c))
                m_frames.append(v);
        } else if (t == "grid"_L1) {
            Grid v;
            if (v.loadOdf(c))
                m_grids.append(v);
        } else if (t == "generic-control"_L1) {
            GenericControl v;
            if (v.loadOdf(c))
                m_genericControls.append(v);
        }
    }
    return true;
}

bool KoOdfForm::isEmpty() const
{
    return m_name.isEmpty() && m_command.isEmpty() && m_datasource.isEmpty() && m_method.isEmpty() && m_xformsSubmission.isEmpty() && m_texts.isEmpty()
        && m_textareas.isEmpty() && m_formattedTexts.isEmpty() && m_numbers.isEmpty() && m_dates.isEmpty() && m_times.isEmpty() && m_buttons.isEmpty()
        && m_checkboxes.isEmpty() && m_radios.isEmpty() && m_comboboxes.isEmpty() && m_listboxes.isEmpty() && m_passwords.isEmpty()
        && m_hiddenControls.isEmpty() && m_files.isEmpty() && m_fixedTexts.isEmpty() && m_valueRanges.isEmpty() && m_images.isEmpty() && m_imageFrames.isEmpty()
        && m_frames.isEmpty() && m_grids.isEmpty() && m_genericControls.isEmpty() && m_models.isEmpty();
}

QString KoOdfForm::controlKind(const QString &id) const
{
    return controlKindName(controlKindEnum(id));
}

KoOdfForm::ControlKind KoOdfForm::controlKindEnum(const QString &id) const
{
    if (id.isEmpty()) {
        return ControlKind::Unknown;
    }
    const auto matches = [&id](const auto &controls, ControlKind kind) {
        for (const auto &control : controls) {
            if (control.id() == id) {
                return kind;
            }
        }
        return ControlKind::Unknown;
    };
    ControlKind kind;
    if ((kind = matches(m_texts, ControlKind::Text)) != ControlKind::Unknown)
        return kind;
    if ((kind = matches(m_textareas, ControlKind::Textarea)) != ControlKind::Unknown)
        return kind;
    if ((kind = matches(m_formattedTexts, ControlKind::FormattedText)) != ControlKind::Unknown)
        return kind;
    if ((kind = matches(m_numbers, ControlKind::Number)) != ControlKind::Unknown)
        return kind;
    if ((kind = matches(m_dates, ControlKind::Date)) != ControlKind::Unknown)
        return kind;
    if ((kind = matches(m_times, ControlKind::Time)) != ControlKind::Unknown)
        return kind;
    if ((kind = matches(m_buttons, ControlKind::Button)) != ControlKind::Unknown)
        return kind;
    if ((kind = matches(m_checkboxes, ControlKind::Checkbox)) != ControlKind::Unknown)
        return kind;
    if ((kind = matches(m_radios, ControlKind::Radio)) != ControlKind::Unknown)
        return kind;
    if ((kind = matches(m_comboboxes, ControlKind::Combobox)) != ControlKind::Unknown)
        return kind;
    if ((kind = matches(m_listboxes, ControlKind::Listbox)) != ControlKind::Unknown)
        return kind;
    if ((kind = matches(m_passwords, ControlKind::Password)) != ControlKind::Unknown)
        return kind;
    if ((kind = matches(m_hiddenControls, ControlKind::Hidden)) != ControlKind::Unknown)
        return kind;
    if ((kind = matches(m_files, ControlKind::File)) != ControlKind::Unknown)
        return kind;
    if ((kind = matches(m_fixedTexts, ControlKind::FixedText)) != ControlKind::Unknown)
        return kind;
    if ((kind = matches(m_valueRanges, ControlKind::ValueRange)) != ControlKind::Unknown)
        return kind;
    if ((kind = matches(m_images, ControlKind::Image)) != ControlKind::Unknown)
        return kind;
    if ((kind = matches(m_imageFrames, ControlKind::ImageFrame)) != ControlKind::Unknown)
        return kind;
    if ((kind = matches(m_frames, ControlKind::Frame)) != ControlKind::Unknown)
        return kind;
    if ((kind = matches(m_grids, ControlKind::Grid)) != ControlKind::Unknown)
        return kind;
    if ((kind = matches(m_genericControls, ControlKind::GenericControl)) != ControlKind::Unknown)
        return kind;
    return ControlKind::Unknown;
}

KoOdfForm::ControlKind KoOdfForm::controlKindFromString(const QString &kind)
{
    for (const auto &[name, value] : controlKinds) {
        if (kind == name) {
            return value;
        }
    }
    return ControlKind::Unknown;
}

QString KoOdfForm::controlKindName(ControlKind kind)
{
    for (const auto &[name, value] : controlKinds) {
        if (kind == value) {
            return name.toString();
        }
    }
    return {};
}

QString KoOdfForm::addControl(const QString &kind)
{
    return addControl(controlKindFromString(kind));
}

QString KoOdfForm::addControl(ControlKind kindValue, const QString &requestedId)
{
    QString id;
    if (!requestedId.isEmpty() && controlKindEnum(requestedId) == ControlKind::Unknown) {
        id = requestedId;
    } else {
        for (int index = 1;; ++index) {
            id = u"control%1"_s.arg(index);
            if (controlKindEnum(id) == ControlKind::Unknown) {
                break;
            }
        }
    }
    const QString kind = controlKindName(kindValue);
    const auto setDefaults = [&id, &kind](auto &control) {
        control.setId(id);
        control.setName(kind);
    };
    const auto append = [&setDefaults]<typename T>(QVector<T> &controls) {
        T control;
        setDefaults(control);
        controls.append(control);
    };
    switch (kindValue) {
    case ControlKind::Text:
        append(m_texts);
        break;
    case ControlKind::Textarea:
        append(m_textareas);
        break;
    case ControlKind::FormattedText:
        append(m_formattedTexts);
        break;
    case ControlKind::Number:
        append(m_numbers);
        break;
    case ControlKind::Date:
        append(m_dates);
        break;
    case ControlKind::Time:
        append(m_times);
        break;
    case ControlKind::Button:
        append(m_buttons);
        break;
    case ControlKind::Checkbox:
        append(m_checkboxes);
        break;
    case ControlKind::Radio:
        append(m_radios);
        break;
    case ControlKind::Combobox:
        append(m_comboboxes);
        break;
    case ControlKind::Listbox:
        append(m_listboxes);
        break;
    case ControlKind::Password:
        append(m_passwords);
        break;
    case ControlKind::Hidden:
        append(m_hiddenControls);
        break;
    case ControlKind::File:
        append(m_files);
        break;
    case ControlKind::FixedText:
        append(m_fixedTexts);
        break;
    case ControlKind::ValueRange:
        append(m_valueRanges);
        break;
    case ControlKind::Image:
        append(m_images);
        break;
    case ControlKind::ImageFrame:
        append(m_imageFrames);
        break;
    case ControlKind::Frame:
        append(m_frames);
        break;
    case ControlKind::Grid:
        append(m_grids);
        break;
    case ControlKind::GenericControl:
    case ControlKind::Unknown:
        append(m_genericControls);
        break;
    }
    return id;
}

bool KoOdfForm::removeControl(const QString &id)
{
    if (id.isEmpty()) {
        return false;
    }
    const auto remove = [&id](auto &controls) {
        const auto it = std::find_if(controls.begin(), controls.end(), [&id](const auto &control) {
            return control.id() == id;
        });
        if (it == controls.end()) {
            return false;
        }
        controls.erase(it);
        return true;
    };
    return remove(m_texts) || remove(m_textareas) || remove(m_formattedTexts) || remove(m_numbers) || remove(m_dates) || remove(m_times) || remove(m_buttons)
        || remove(m_checkboxes) || remove(m_radios) || remove(m_comboboxes) || remove(m_listboxes) || remove(m_passwords) || remove(m_hiddenControls)
        || remove(m_files) || remove(m_fixedTexts) || remove(m_valueRanges) || remove(m_images) || remove(m_imageFrames) || remove(m_frames) || remove(m_grids)
        || remove(m_genericControls);
}
std::unique_ptr<KoOdfForm::Control> KoOdfForm::controlById(const QString &id) const
{
    if (id.isEmpty())
        return nullptr;
    const auto findControl = [&id](const auto &controls) -> std::unique_ptr<Control> {
        for (const auto &control : controls) {
            if (control.id() == id)
                return std::make_unique<std::decay_t<decltype(control)>>(control);
        }
        return nullptr;
    };
    std::unique_ptr<Control> control;
    if ((control = findControl(m_texts)))
        return control;
    if ((control = findControl(m_textareas)))
        return control;
    if ((control = findControl(m_formattedTexts)))
        return control;
    if ((control = findControl(m_numbers)))
        return control;
    if ((control = findControl(m_dates)))
        return control;
    if ((control = findControl(m_times)))
        return control;
    if ((control = findControl(m_buttons)))
        return control;
    if ((control = findControl(m_checkboxes)))
        return control;
    if ((control = findControl(m_radios)))
        return control;
    if ((control = findControl(m_comboboxes)))
        return control;
    if ((control = findControl(m_listboxes)))
        return control;
    if ((control = findControl(m_passwords)))
        return control;
    if ((control = findControl(m_hiddenControls)))
        return control;
    if ((control = findControl(m_files)))
        return control;
    if ((control = findControl(m_fixedTexts)))
        return control;
    if ((control = findControl(m_valueRanges)))
        return control;
    if ((control = findControl(m_images)))
        return control;
    if ((control = findControl(m_imageFrames)))
        return control;
    if ((control = findControl(m_frames)))
        return control;
    if ((control = findControl(m_grids)))
        return control;
    if ((control = findControl(m_genericControls)))
        return control;
    return nullptr;
}

bool KoOdfForm::setControlProperties(const QString &id, const Control &properties)
{
    if (id.isEmpty())
        return false;
    const auto updateControl = [&id, &properties](auto &controls) {
        for (auto &control : controls) {
            if (control.id() == id) {
                static_cast<Control &>(control) = properties;
                control.setId(id);
                return true;
            }
        }
        return false;
    };
    return updateControl(m_texts) || updateControl(m_textareas) || updateControl(m_formattedTexts) || updateControl(m_numbers) || updateControl(m_dates)
        || updateControl(m_times) || updateControl(m_buttons) || updateControl(m_checkboxes) || updateControl(m_radios) || updateControl(m_comboboxes)
        || updateControl(m_listboxes) || updateControl(m_passwords) || updateControl(m_hiddenControls) || updateControl(m_files) || updateControl(m_fixedTexts)
        || updateControl(m_valueRanges) || updateControl(m_images) || updateControl(m_imageFrames) || updateControl(m_frames) || updateControl(m_grids)
        || updateControl(m_genericControls);
}

void KoOdfForm::saveOdf(KoXmlWriter &w) const
{
    w.startElement("form:form");
    if (!m_name.isEmpty())
        w.addAttribute("form:name", m_name);
    if (!m_command.isEmpty())
        w.addAttribute("form:command", m_command);
    if (!m_datasource.isEmpty())
        w.addAttribute("form:datasource", m_datasource);
    if (!m_method.isEmpty())
        w.addAttribute("form:method", m_method);
    if (!m_xformsSubmission.isEmpty())
        w.addAttribute("xforms:submission", m_xformsSubmission);
    for (const Text &v : m_texts)
        v.saveOdf(w);
    for (const Textarea &v : m_textareas)
        v.saveOdf(w);
    for (const FormattedText &v : m_formattedTexts)
        v.saveOdf(w);
    for (const Number &v : m_numbers)
        v.saveOdf(w);
    for (const Date &v : m_dates)
        v.saveOdf(w);
    for (const Time &v : m_times)
        v.saveOdf(w);
    for (const Button &v : m_buttons)
        v.saveOdf(w);
    for (const Checkbox &v : m_checkboxes)
        v.saveOdf(w);
    for (const Radio &v : m_radios)
        v.saveOdf(w);
    for (const Combobox &v : m_comboboxes)
        v.saveOdf(w);
    for (const Listbox &v : m_listboxes)
        v.saveOdf(w);
    for (const Password &v : m_passwords)
        v.saveOdf(w);
    for (const Hidden &v : m_hiddenControls)
        v.saveOdf(w);
    for (const File &v : m_files)
        v.saveOdf(w);
    for (const FixedText &v : m_fixedTexts)
        v.saveOdf(w);
    for (const ValueRange &v : m_valueRanges)
        v.saveOdf(w);
    for (const Image &v : m_images)
        v.saveOdf(w);
    for (const ImageFrame &v : m_imageFrames)
        v.saveOdf(w);
    for (const Frame &v : m_frames)
        v.saveOdf(w);
    for (const Grid &v : m_grids)
        v.saveOdf(w);
    for (const GenericControl &v : m_genericControls)
        v.saveOdf(w);
    for (const Model &v : m_models)
        v.saveOdf(w);
    w.endElement();
}
QString KoOdfForm::name() const
{
    return m_name;
}
QString KoOdfForm::command() const
{
    return m_command;
}
QString KoOdfForm::datasource() const
{
    return m_datasource;
}
QString KoOdfForm::method() const
{
    return m_method;
}
QString KoOdfForm::xformsSubmission() const
{
    return m_xformsSubmission;
}

QVector<KoOdfForm::Model> KoOdfForm::models() const
{
    return m_models;
}

QString KoOdfForm::Control::label() const
{
    return formAttribute(u"label"_s);
}

void KoOdfForm::Control::setLabel(const QString &label)
{
    setFormAttribute(u"label"_s, label);
}

QString KoOdfForm::Control::formAttribute(const QString &name) const
{
    return m_formAttributes.value(name);
}

void KoOdfForm::Control::setFormAttribute(const QString &name, const QString &value)
{
    m_formAttributes.insert(name, value);
}

QVector<KoOdfForm::Control::Entry> KoOdfForm::Control::entries() const
{
    return m_entries;
}

void KoOdfForm::Control::setEntries(const QVector<Entry> &entries)
{
    m_entries = entries;
}

void KoOdfForm::Control::saveChildren(KoXmlWriter &writer) const
{
    if (!m_childrenXml.isEmpty())
        writer.addCompleteElement(m_childrenXml.constData());
}
