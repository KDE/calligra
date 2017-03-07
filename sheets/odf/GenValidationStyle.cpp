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

// Calligra
#include <KoXmlWriter.h>

// Sheets
#include "Validity.h"
#include "ValueConverter.h"

using namespace Calligra::Sheets;

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

void GenValidationStyles::writeStyle(KoXmlWriter& writer) const
{
    if (m_styles.count() > 0) {
        writer.startElement("table:content-validations");
        StyleMap::ConstIterator it;
        for (it = m_styles.begin(); it != m_styles.end(); ++it) {
            writer.startElement("table:content-validation");
            writer.addAttribute("table:name", it.value());
            writer.addAttribute("table:allow-empty-cell", it.key().allowEmptyCell);
            writer.addAttribute("table:condition", it.key().condition);

            writer.startElement("table:help-message");
            writer.addAttribute("table:title", it.key().title);
            writer.addAttribute("table:display", it.key().displayValidationInformation);

            const QStringList helpLines = it.key().messageInfo.split('\n', QString::SkipEmptyParts);
            foreach(const QString& helpLine, helpLines) {
                writer.startElement("text:p");
                writer.addTextNode(helpLine);
                writer.endElement();
            }
            writer.endElement();

            writer.startElement("table:error-message");
            writer.addAttribute("table:message-type", it.key().messageType);

            writer.addAttribute("table:title", it.key().titleInfo);
            writer.addAttribute("table:display", it.key().displayMessage);

            const QStringList errorLines = it.key().message.split('\n', QString::SkipEmptyParts);
            foreach(const QString& errorLine, errorLines) {
                writer.startElement("text:p");
                writer.addTextNode(errorLine);
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
    QString result =
        "oooc:cell-content-is-in-list(" +
        validity->validityList().join(";") +
        ')';
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
    case Conditional::IsTrueFormula:
        //nothing
        break;
    case Conditional::Equal:
        result += "cell-content()"
                  "=" +
                  QString::number((double)numToDouble(validity->minimumValue().asFloat()));
        break;
    case Conditional::Superior:
        result += "cell-content()"
                  ">" +
                  QString::number((double)numToDouble(validity->minimumValue().asFloat()));
        break;
    case Conditional::Inferior:
        result += "cell-content()"
                  "<" +
                  QString::number((double)numToDouble(validity->minimumValue().asFloat()));
        break;
    case Conditional::SuperiorEqual:
        result += "cell-content()"
                  ">=" +
                  QString::number((double)numToDouble(validity->minimumValue().asFloat()));
        break;
    case Conditional::InferiorEqual:
        result += "cell-content()"
                  "<=" +
                  QString::number((double)numToDouble(validity->minimumValue().asFloat()));
        break;
    case Conditional::Different:
        result += "cell-content()"
                  "!=" +
                  QString::number((double)numToDouble(validity->minimumValue().asFloat()));
        break;
    case Conditional::Between:
        result += "cell-content-is-between(" +
                  QString::number((double)numToDouble(validity->minimumValue().asFloat())) +
                  ',' +
                  QString::number((double)numToDouble(validity->maximumValue().asFloat())) +
                  ')';
        break;
    case Conditional::DifferentTo:
        result += "cell-content-is-not-between(" +
                  QString::number((double)numToDouble(validity->minimumValue().asFloat())) +
                  ',' +
                  QString::number((double)numToDouble(validity->maximumValue().asFloat())) +
                  ')';
        break;
    }
    return result;
}


QString GenValidationStyle::createTimeValidationCondition(Validity* validity, const ValueConverter *converter)
{
    QString result("oooc:cell-content-is-time() and ");
    switch (validity->condition()) {
    case Conditional::None:
    case Conditional::IsTrueFormula:
        //nothing
        break;
    case Conditional::Equal:
        result += "cell-content()"
                  "=" +
                  converter->asString(validity->minimumValue()).asString();
        break;
    case Conditional::Superior:
        result += "cell-content()"
                  ">" +
                  converter->asString(validity->minimumValue()).asString();
        break;
    case Conditional::Inferior:
        result += "cell-content()"
                  "<" +
                  converter->asString(validity->minimumValue()).asString();
        break;
    case Conditional::SuperiorEqual:
        result += "cell-content()"
                  ">=" +
                  converter->asString(validity->minimumValue()).asString();
        break;
    case Conditional::InferiorEqual:
        result += "cell-content()"
                  "<=" +
                  converter->asString(validity->minimumValue()).asString();
        break;
    case Conditional::Different:
        result += "cell-content()"
                  "!=" +
                  converter->asString(validity->minimumValue()).asString();
        break;
    case Conditional::Between:
        result += "cell-content-is-between(" +
                  converter->asString(validity->minimumValue()).asString() +
                  ',' +
                  converter->asString(validity->maximumValue()).asString() +
                  ')';
        break;
    case Conditional::DifferentTo:
        result += "cell-content-is-not-between(" +
                  converter->asString(validity->minimumValue()).asString() +
                  ',' +
                  converter->asString(validity->maximumValue()).asString() +
                  ')';
        break;
    }
    return result;
}

QString GenValidationStyle::createDateValidationCondition(Validity* validity, const ValueConverter *converter)
{
    QString result("oooc:cell-content-is-date() and ");
    switch (validity->condition()) {
    case Conditional::None:
    case Conditional::IsTrueFormula:
        //nothing
        break;
    case Conditional::Equal:
        result += "cell-content()"
                  "=" +
                  converter->asString(validity->minimumValue()).asString();
        break;
    case Conditional::Superior:
        result += "cell-content()"
                  ">" +
                  converter->asString(validity->minimumValue()).asString();
        break;
    case Conditional::Inferior:
        result += "cell-content()"
                  "<" +
                  converter->asString(validity->minimumValue()).asString();
        break;
    case Conditional::SuperiorEqual:
        result += "cell-content()"
                  ">=" +
                  converter->asString(validity->minimumValue()).asString();
        break;
    case Conditional::InferiorEqual:
        result += "cell-content()"
                  "<=" +
                  converter->asString(validity->minimumValue()).asString();
        break;
    case Conditional::Different:
        result += "cell-content()"
                  "!=" +
                  converter->asString(validity->minimumValue()).asString();
        break;
    case Conditional::Between:
        result += "cell-content-is-between(" +
                  converter->asString(validity->minimumValue()).asString() +
                  ',' +
                  converter->asString(validity->maximumValue()).asString() +
                  ')';
        break;
    case Conditional::DifferentTo:
        result += "cell-content-is-not-between(" +
                  converter->asString(validity->minimumValue()).asString() +
                  ',' +
                  converter->asString(validity->maximumValue()).asString() +
                  ')';
        break;
    }
    return result;
}

QString GenValidationStyle::createTextValidationCondition(Validity* validity)
{
    QString result;
    switch (validity->condition()) {
    case Conditional::None:
    case Conditional::IsTrueFormula:
        //nothing
        break;
    case Conditional::Equal:
        result += "oooc:cell-content-text-length()"
                  "=" +
                  QString::number((double)numToDouble(validity->minimumValue().asFloat()));
        break;
    case Conditional::Superior:
        result += "oooc:cell-content-text-length()"
                  ">" +
                  QString::number((double)numToDouble(validity->minimumValue().asFloat()));
        break;
    case Conditional::Inferior:
        result += "oooc:cell-content-text-length()"
                  "<" +
                  QString::number((double)numToDouble(validity->minimumValue().asFloat()));
        break;
    case Conditional::SuperiorEqual:
        result += "oooc:cell-content-text-length()"
                  ">=" +
                  QString::number((double)numToDouble(validity->minimumValue().asFloat()));
        break;
    case Conditional::InferiorEqual:
        result += "oooc:cell-content-text-length()"
                  "<=" +
                  QString::number((double)numToDouble(validity->minimumValue().asFloat()));
        break;
    case Conditional::Different:
        result += "oooc:cell-content-text-length()"
                  "!=" +
                  QString::number((double)numToDouble(validity->minimumValue().asFloat()));
        break;
    case Conditional::Between:
        result += "oooc:cell-content-text-length-is-between(" +
                  QString::number((double)numToDouble(validity->minimumValue().asFloat())) +
                  ',' +
                  QString::number((double)numToDouble(validity->maximumValue().asFloat())) +
                  ')';
        break;
    case Conditional::DifferentTo:
        result += "oooc:cell-content-text-length-is-not-between(" +
                  QString::number((double)numToDouble(validity->minimumValue().asFloat())) +
                  ',' +
                  QString::number((double)numToDouble(validity->maximumValue().asFloat())) +
                  ')';
        break;
    }
    return result;
}
