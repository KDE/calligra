#include "KoOdfForm.h"
#include "KoXmlReader.h"
#include "KoXmlWriter.h"

#include <QDomDocument>
#include <QTextStream>

namespace
{
QString a(const KoXmlElement &e, const char *n)
{
    return e.attribute(QString::fromLatin1(n));
}
}

bool KoOdfForm::Control::loadOdf(const KoXmlElement &e)
{
    if (e.isNull())
        return false;
    m_name = a(e, "form:name");
    m_id = a(e, "form:id");
    m_title = a(e, "form:title");
    m_value = a(e, "form:value");
    m_currentValue = a(e, "form:current-value");
    m_dataField = a(e, "form:data-field");
    m_linkedCell = a(e, "form:linked-cell");
    m_xformsBind = a(e, "xforms:bind");
    m_disabled = a(e, "form:disabled") == QLatin1String("true");
    m_printable = a(e, "form:printable") != QLatin1String("false");
    m_readOnly = a(e, "form:readonly") == QLatin1String("true");
    m_inputRequired = a(e, "form:input-required") == QLatin1String("true");
    m_tabStop = a(e, "form:tab-stop") != QLatin1String("false");
    m_tabIndex = a(e, "form:tab-index").toInt();
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
    if (!m_currentValue.isEmpty())
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
    m_name = a(e, "form:property-name");
    m_value = a(e, "form:value");
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
    m_name = a(e, "form:name");
    m_command = a(e, "form:command");
    m_datasource = a(e, "form:datasource");
    m_method = a(e, "form:method");
    m_xformsSubmission = a(e, "xforms:submission");
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
        }
    }
    return true;
}

bool KoOdfForm::isEmpty() const
{
    return m_name.isEmpty() && m_command.isEmpty() && m_datasource.isEmpty() && m_method.isEmpty() && m_xformsSubmission.isEmpty() && m_texts.isEmpty()
        && m_textareas.isEmpty() && m_formattedTexts.isEmpty() && m_numbers.isEmpty() && m_dates.isEmpty() && m_times.isEmpty() && m_buttons.isEmpty()
        && m_checkboxes.isEmpty() && m_radios.isEmpty() && m_comboboxes.isEmpty() && m_listboxes.isEmpty() && m_models.isEmpty();
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
