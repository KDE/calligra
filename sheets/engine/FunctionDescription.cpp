// This file is part of the KDE project
// SPDX-FileCopyrightText: 2003, 2004 Ariya Hidayat <ariya@kde.org>
// SPDX-FileCopyrightText: 2005 Tomas Mecir <mecirt@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-only

// Local
#include "FunctionDescription.h"

#include <QDomElement>
#include <QDomNode>

#include <KLocalizedString>

using namespace Calligra::Sheets;

static ParameterType toType(const QString &type)
{
    if (type == "Boolean")
        return KSpread_Boolean;
    if (type == "Int")
        return KSpread_Int;
    if (type == "String")
        return KSpread_String;
    if (type == "Any")
        return KSpread_Any;
    if (type == "Date")
        return KSpread_Date;

    return KSpread_Float;
}

static QString toString(ParameterType type, bool range = false)
{
    if (!range) {
        switch (type) {
        case KSpread_String:
            return i18n("Text");
        case KSpread_Int:
            return i18n("Whole number (like 1, 132, 2344)");
        case KSpread_Boolean:
            return i18n("A truth value (TRUE or FALSE)");
        case KSpread_Float:
            return i18n("A floating point value (like 1.3, 0.343, 253 )");
        case KSpread_Any:
            return i18n("Any kind of value");
        case KSpread_Date:
            return i18n("A string representing a date (like \"2/22/2012\")");
        }
    } else {
        switch (type) {
        case KSpread_String:
            return i18n("A range of strings");
        case KSpread_Int:
            return i18n("A range of whole numbers (like 1, 132, 2344)");
        case KSpread_Boolean:
            return i18n("A range of truth values (TRUE or FALSE)");
        case KSpread_Float:
            return i18n("A range of floating point values (like 1.3, 0.343, 253 )");
        case KSpread_Any:
            return i18n("A range of any kind of values");
        case KSpread_Date:
            return i18n("A string representing a range of dates (like \"2/22/2012\"-\"5/22/2012\")");
        }
    }

    return QString();
}

FunctionParameter::FunctionParameter()
{
    m_type = KSpread_Float;
    m_range = false;
}

FunctionParameter::FunctionParameter(const FunctionParameter &param)
{
    m_help = param.m_help;
    m_type = param.m_type;
    m_range = param.m_range;
}

FunctionParameter::FunctionParameter(const QDomElement &element)
{
    m_type = KSpread_Float;
    m_range = false;

    QDomNode n = element.firstChild();
    for (; !n.isNull(); n = n.nextSibling())
        if (n.isElement()) {
            QDomElement e = n.toElement();
            if (e.tagName() == "Comment")
                m_help = i18n(e.text().toUtf8());
            else if (e.tagName() == "Type") {
                m_type = toType(e.text());
                if (e.hasAttribute("range")) {
                    if (e.attribute("range").toLower() == "true")
                        m_range = true;
                }
            }
        }
}

FunctionDescription::FunctionDescription()
{
    m_type = KSpread_Float;
}

FunctionDescription::FunctionDescription(const QDomElement &element)
{
    QDomNode n = element.firstChild();
    for (; !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        if (e.tagName() == "Name")
            m_name = e.text();
        else if (e.tagName() == "Type")
            m_type = toType(e.text());
        else if (e.tagName() == "Parameter")
            m_params.append(FunctionParameter(e));
        else if (e.tagName() == "Help") {
            QDomNode n2 = e.firstChild();
            for (; !n2.isNull(); n2 = n2.nextSibling()) {
                if (!n2.isElement())
                    continue;
                QDomElement e2 = n2.toElement();
                if (e2.tagName() == "Text")
                    m_help.append(i18n(e2.text().toUtf8()));
                else if (e2.tagName() == "Syntax")
                    m_syntax.append(i18n(e2.text().toUtf8()));
                else if (e2.tagName() == "Example")
                    m_examples.append(i18n(e2.text().toUtf8()));
                else if (e2.tagName() == "Related")
                    m_related.append(i18n(e2.text().toUtf8()));
            }
        }
    }
}

FunctionDescription::FunctionDescription(const FunctionDescription &desc)
{
    m_examples = desc.m_examples;
    m_related = desc.m_related;
    m_syntax = desc.m_syntax;
    m_help = desc.m_help;
    m_name = desc.m_name;
    m_type = desc.m_type;
}

QString FunctionDescription::toQML() const
{
    QString text("<qt><h1>");
    text += name();
    text += "</h1>";

    if (!m_help.isEmpty()) {
        text += "<p>";
        QStringList::ConstIterator it = m_help.begin();
        for (; it != m_help.end(); ++it) {
            text += *it + "<p>";
        }
        text += "</p>";
    }

    text += i18n("<p><b>Return type:</b> %1</p>", toString(type()));

    if (!m_syntax.isEmpty()) {
        text += "<h2>" + i18n("Syntax") + "</h2><ul>";
        QStringList::ConstIterator it = m_syntax.begin();
        for (; it != m_syntax.end(); ++it) {
            text += "<li>" + *it + "</li>";
        }
        text += "</ul>";
    }

    if (!m_params.isEmpty()) {
        text += "<h2>" + i18n("Parameters") + "</h2><ul>";
        QList<FunctionParameter>::ConstIterator it = m_params.begin();
        for (; it != m_params.end(); ++it) {
            text += i18n("<li><b>Comment:</b> %1", (*it).helpText()) + i18n("<br><b>Type:</b> %1", toString((*it).type(), (*it).hasRange()));
        }
        text += "</ul>";
    }

    if (!m_examples.isEmpty()) {
        text += "<h2>" + i18n("Examples") + "</h2><ul>";
        QStringList::ConstIterator it = m_examples.begin();
        for (; it != m_examples.end(); ++it) {
            text += "<li>" + *it + "</li>";
        }
        text += "</ul>";
    }

    if (!m_related.isEmpty()) {
        text += "<h2>" + i18n("Related Functions") + "</h2><ul>";
        QStringList::ConstIterator it = m_related.begin();
        for (; it != m_related.end(); ++it) {
            text += "<li>"
                    "<a href=\"" + *it + "\">" +
                    *it +
                    "</a>"
                    "</li>";
        }
        text += "</ul>";
    }

    text += "</qt>";
    return text;
}
