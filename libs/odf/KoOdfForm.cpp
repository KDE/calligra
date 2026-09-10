/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoOdfForm.h"
#include "KoXmlNS.h"
#include "KoXmlReader.h"
#include "KoXmlWriter.h"

#include <QDomDocument>
#include <QTextStream>
#include <type_traits>

using namespace Qt::StringLiterals;

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
    m_tabIndex = v;
}
void KoOdfForm::Control::setTabStop(bool v)
{
    m_tabStop = v;
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
    if (id.isEmpty())
        return {};
    const auto matches = [&id](const auto &controls, const QString &kind) {
        for (const auto &control : controls) {
            if (control.id() == id)
                return kind;
        }
        return QString();
    };
    QString kind;
    if (!(kind = matches(m_texts, u"text"_s)).isEmpty())
        return kind;
    if (!(kind = matches(m_textareas, u"textarea"_s)).isEmpty())
        return kind;
    if (!(kind = matches(m_formattedTexts, u"formatted-text"_s)).isEmpty())
        return kind;
    if (!(kind = matches(m_numbers, u"number"_s)).isEmpty())
        return kind;
    if (!(kind = matches(m_dates, u"date"_s)).isEmpty())
        return kind;
    if (!(kind = matches(m_times, u"time"_s)).isEmpty())
        return kind;
    if (!(kind = matches(m_buttons, u"button"_s)).isEmpty())
        return kind;
    if (!(kind = matches(m_checkboxes, u"checkbox"_s)).isEmpty())
        return kind;
    if (!(kind = matches(m_radios, u"radio"_s)).isEmpty())
        return kind;
    if (!(kind = matches(m_comboboxes, u"combobox"_s)).isEmpty())
        return kind;
    if (!(kind = matches(m_listboxes, u"listbox"_s)).isEmpty())
        return kind;
    if (!(kind = matches(m_passwords, u"password"_s)).isEmpty())
        return kind;
    if (!(kind = matches(m_hiddenControls, u"hidden"_s)).isEmpty())
        return kind;
    if (!(kind = matches(m_files, u"file"_s)).isEmpty())
        return kind;
    if (!(kind = matches(m_fixedTexts, u"fixed-text"_s)).isEmpty())
        return kind;
    if (!(kind = matches(m_valueRanges, u"value-range"_s)).isEmpty())
        return kind;
    if (!(kind = matches(m_images, u"image"_s)).isEmpty())
        return kind;
    if (!(kind = matches(m_imageFrames, u"image-frame"_s)).isEmpty())
        return kind;
    if (!(kind = matches(m_frames, u"frame"_s)).isEmpty())
        return kind;
    if (!(kind = matches(m_grids, u"grid"_s)).isEmpty())
        return kind;
    if (!(kind = matches(m_genericControls, u"generic-control"_s)).isEmpty())
        return kind;
    return {};
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

void KoOdfForm::Control::saveChildren(KoXmlWriter &writer) const
{
    if (!m_childrenXml.isEmpty())
        writer.addCompleteElement(m_childrenXml.constData());
}
