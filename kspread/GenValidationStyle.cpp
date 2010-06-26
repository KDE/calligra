/* This file is part of the KDE project

   Copyright 2004 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// Local
#include "GenValidationStyle.h"

// KOffice
#include <KoXmlWriter.h>

// KSpread
#include "Validity.h"
#include "ValueConverter.h"

using namespace KSpread;

GenValidationStyles::GenValidationStyles()
{

}

GenValidationStyles::~GenValidationStyles()
{

}

QString GenValidationStyles::insert(const GenValidationStyle& style)
{
    StyleMap::iterator it = m_styles.find(style);
    if (it == m_styles.end()) {

        QString styleName("val");
        styleName = makeUniqueName(styleName);
        m_names.insert(styleName, true);
        it = m_styles.insert(style, styleName);
    }
    return it.value();
}

QString GenValidationStyles::makeUniqueName(const QString& base) const
{
    int num = 1;
    QString name;
    do {
        name = base;
        name += QString::number(num++);
    } while (m_names.find(name) != m_names.end());
    return name;
}

void GenValidationStyles::writeStyle(KoXmlWriter& writer)
{
    if (m_styles.count() > 0) {
        writer.startElement("table:content-validations");
        StyleMap::Iterator it;
        for (it = m_styles.begin(); it != m_styles.end(); ++it) {
            writer.startElement("table:content-validation");
            writer.addAttribute("table:name", it.value());
            writer.addAttribute("table:allow-empty-cell", it.key().allowEmptyCell);
            writer.addAttribute("table:condition", it.key().condition);

            writer.startElement("table:help-message");
            writer.addAttribute("table:title", it.key().title);
            writer.addAttribute("table:display", it.key().displayValidationInformation);

            QStringList text = it.key().messageInfo.split('\n', QString::SkipEmptyParts);
            for (QStringList::Iterator it2 = text.begin(); it2 != text.end(); ++it2) {
                writer.startElement("text:p");
                writer.addTextNode(*it2);
                writer.endElement();
            }
            writer.endElement();

            writer.startElement("table:error-message");
            writer.addAttribute("table:message-type", it.key().messageType);

            writer.addAttribute("table:title", it.key().titleInfo);
            writer.addAttribute("table:display", it.key().displayMessage);
            text = it.key().message.split("\n", QString::SkipEmptyParts);
            for (QStringList::Iterator it3 = text.begin(); it3 != text.end(); ++it3) {
                writer.startElement("text:p");
                writer.addTextNode(*it3);
                writer.endElement();
            }
            writer.endElement();

            writer.endElement();
        }
        writer.endElement();//close sheet:content-validation
    }
}

void GenValidationStyle::initVal(Validity *validity, const ValueConverter *converter)
{
    if (validity) {
        allowEmptyCell = (validity->allowEmptyCell() ? "true" : "false");
        condition = createValidationCondition(validity, converter);
        title = validity->title();
        displayValidationInformation = (validity->displayValidationInformation() ? "true" : "false");
        messageInfo = validity->messageInfo();

        switch (validity->action()) {
        case Validity::Warning:
            messageType = "warning";
            break;
        case Validity::Information:
            messageType = "information";
            break;
        case Validity::Stop:
            messageType = "stop";
            break;
        }

        titleInfo = validity->titleInfo();
        displayMessage = (validity->displayMessage() ? "true" : "false");
        message = validity->message();
    }
}

QString GenValidationStyle::createValidationCondition(Validity* validity, const ValueConverter *converter)
{
    QString result;
    switch (validity->restriction()) {
    case Validity::None:
        //nothing
        break;
    case Validity::Text:
        //doesn't exist into oo spec
        result = "cell-content-is-text()";
        break;
    case Validity::Time:
        result = createTimeValidationCondition(validity, converter);
        break;
    case Validity::Date:
        result = createDateValidationCondition(validity, converter);
        break;
    case Validity::Integer:
    case Validity::Number:
        result = createNumberValidationCondition(validity);
        break;
    case Validity::TextLength:
        result = createTextValidationCondition(validity);
        break;
    case Validity::List:
        result = createListValidationCondition(validity);
        break;
    }
    return result;
}

QString GenValidationStyle::createListValidationCondition(Validity* validity)
{
    QString result = "oooc:cell-content-is-in-list(";
    result = validity->validityList().join(";");
    result += ')';
    return result;
}

QString GenValidationStyle::createNumberValidationCondition(Validity* validity)
{
    QString result;
    if (validity->restriction() == Validity::Number)
        result = "oooc:cell-content-is-whole-number() and ";
    else if (validity->restriction() == Validity::Integer)
        result = "oooc:cell-content-is-decimal-number() and ";
    switch (validity->condition()) {
    case Conditional::None:
        //nothing
        break;
    case Conditional::Equal:
        result += "cell-content()";
        result += '=';
        result += QString::number((double)numToDouble(validity->minimumValue().asFloat()));
        break;
    case Conditional::Superior:
        result += "cell-content()";
        result += '>';
        result += QString::number((double)numToDouble(validity->minimumValue().asFloat()));
        break;
    case Conditional::Inferior:
        result += "cell-content()";
        result += '<';
        result += QString::number((double)numToDouble(validity->minimumValue().asFloat()));
        break;
    case Conditional::SuperiorEqual:
        result += "cell-content()";
        result += ">=";
        result += QString::number((double)numToDouble(validity->minimumValue().asFloat()));
        break;
    case Conditional::InferiorEqual:
        result += "cell-content()";
        result += "<=";
        result += QString::number((double)numToDouble(validity->minimumValue().asFloat()));
        break;
    case Conditional::Different:
        result += "cell-content()";
        result += "!=";
        result += QString::number((double)numToDouble(validity->minimumValue().asFloat()));
        break;
    case Conditional::Between:
        result += "cell-content-is-between(";
        result += QString::number((double)numToDouble(validity->minimumValue().asFloat()));
        result += ',';
        result += QString::number((double)numToDouble(validity->maximumValue().asFloat()));
        result += ')';
        break;
    case Conditional::DifferentTo:
        result += "cell-content-is-not-between(";
        result += QString::number((double)numToDouble(validity->minimumValue().asFloat()));
        result += ',';
        result += QString::number((double)numToDouble(validity->maximumValue().asFloat()));
        result += ')';
        break;
    }
    return result;
}


QString GenValidationStyle::createTimeValidationCondition(Validity* validity, const ValueConverter *converter)
{
    QString result("oooc:cell-content-is-time() and ");
    switch (validity->condition()) {
    case Conditional::None:
        //nothing
        break;
    case Conditional::Equal:
        result += "cell-content()";
        result += '=';
        result += converter->asString(validity->minimumValue()).asString();
        break;
    case Conditional::Superior:
        result += "cell-content()";
        result += '>';
        result += converter->asString(validity->minimumValue()).asString();
        break;
    case Conditional::Inferior:
        result += "cell-content()";
        result += '<';
        result += converter->asString(validity->minimumValue()).asString();
        break;
    case Conditional::SuperiorEqual:
        result += "cell-content()";
        result += ">=";
        result += converter->asString(validity->minimumValue()).asString();
        break;
    case Conditional::InferiorEqual:
        result += "cell-content()";
        result += "<=";
        result += converter->asString(validity->minimumValue()).asString();
        break;
    case Conditional::Different:
        result += "cell-content()";
        result += "!=";
        result += converter->asString(validity->minimumValue()).asString();
        break;
    case Conditional::Between:
        result += "cell-content-is-between(";
        result += converter->asString(validity->minimumValue()).asString();
        result += ',';
        result += converter->asString(validity->maximumValue()).asString();
        result += ')';
        break;
    case Conditional::DifferentTo:
        result += "cell-content-is-not-between(";
        result += converter->asString(validity->minimumValue()).asString();
        result += ',';
        result += converter->asString(validity->maximumValue()).asString();
        result += ')';
        break;
    }
    return result;
}

QString GenValidationStyle::createDateValidationCondition(Validity* validity, const ValueConverter *converter)
{
    QString result("oooc:cell-content-is-date() and ");
    switch (validity->condition()) {
    case Conditional::None:
        //nothing
        break;
    case Conditional::Equal:
        result += "cell-content()";
        result += '=';
        result += converter->asString(validity->minimumValue()).asString();
        break;
    case Conditional::Superior:
        result += "cell-content()";
        result += '>';
        result += converter->asString(validity->minimumValue()).asString();
        break;
    case Conditional::Inferior:
        result += "cell-content()";
        result += '<';
        result += converter->asString(validity->minimumValue()).asString();
        break;
    case Conditional::SuperiorEqual:
        result += "cell-content()";
        result += ">=";
        result += converter->asString(validity->minimumValue()).asString();
        break;
    case Conditional::InferiorEqual:
        result += "cell-content()";
        result += "<=";
        result += converter->asString(validity->minimumValue()).asString();
        break;
    case Conditional::Different:
        result += "cell-content()";
        result += "!=";
        result += converter->asString(validity->minimumValue()).asString();
        break;
    case Conditional::Between:
        result += "cell-content-is-between(";
        result += converter->asString(validity->minimumValue()).asString();
        result += ',';
        result += converter->asString(validity->maximumValue()).asString();
        result += ')';
        break;
    case Conditional::DifferentTo:
        result += "cell-content-is-not-between(";
        result += converter->asString(validity->minimumValue()).asString();
        result += ',';
        result += converter->asString(validity->maximumValue()).asString();
        result += ')';
        break;
    }
    return result;
}

QString GenValidationStyle::createTextValidationCondition(Validity* validity)
{
    QString result;
    switch (validity->condition()) {
    case Conditional::None:
        //nothing
        break;
    case Conditional::Equal:
        result += "oooc:cell-content-text-length()";
        result += '=';
        result += QString::number((double)numToDouble(validity->minimumValue().asFloat()));
        break;
    case Conditional::Superior:
        result += "oooc:cell-content-text-length()";
        result += '>';
        result += QString::number((double)numToDouble(validity->minimumValue().asFloat()));
        break;
    case Conditional::Inferior:
        result += "oooc:cell-content-text-length()";
        result += '<';
        result += QString::number((double)numToDouble(validity->minimumValue().asFloat()));
        break;
    case Conditional::SuperiorEqual:
        result += "oooc:cell-content-text-length()";
        result += ">=";
        result += QString::number((double)numToDouble(validity->minimumValue().asFloat()));
        break;
    case Conditional::InferiorEqual:
        result += "oooc:cell-content-text-length()";
        result += "<=";
        result += QString::number((double)numToDouble(validity->minimumValue().asFloat()));
        break;
    case Conditional::Different:
        result += "oooc:cell-content-text-length()";
        result += "!=";
        result += QString::number((double)numToDouble(validity->minimumValue().asFloat()));
        break;
    case Conditional::Between:
        result += "oooc:cell-content-text-length-is-between(";
        result += QString::number((double)numToDouble(validity->minimumValue().asFloat()));
        result += ',';
        result += QString::number((double)numToDouble(validity->maximumValue().asFloat()));
        result += ')';
        break;
    case Conditional::DifferentTo:
        result += "oooc:cell-content-text-length-is-not-between(";
        result += QString::number((double)numToDouble(validity->minimumValue().asFloat()));
        result += ',';
        result += QString::number((double)numToDouble(validity->maximumValue().asFloat()));
        result += ')';
        break;
    }
    return result;
}
