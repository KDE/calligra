/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2004 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "GenValidationStyle.h"

#include "SheetsOdf.h"
#include "SheetsOdfPrivate.h"

// Calligra
#include <KoXmlWriter.h>

// Sheets
#include "engine/Validity.h"

using namespace Calligra::Sheets::Odf;

GenValidationStyles::GenValidationStyles() = default;

GenValidationStyles::~GenValidationStyles() = default;

QString GenValidationStyles::insert(const GenValidationStyle &style)
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

QString GenValidationStyles::makeUniqueName(const QString &base) const
{
    int num = 1;
    QString name;
    do {
        name = base;
        name += QString::number(num++);
    } while (m_names.find(name) != m_names.end());
    return name;
}

void GenValidationStyles::writeStyle(KoXmlWriter &writer) const
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

            const QStringList helpLines = it.key().messageInfo.split('\n', Qt::SkipEmptyParts);
            for (const QString &helpLine : helpLines) {
                writer.startElement("text:p");
                writer.addTextNode(helpLine);
                writer.endElement();
            }
            writer.endElement();

            writer.startElement("table:error-message");
            writer.addAttribute("table:message-type", it.key().messageType);

            writer.addAttribute("table:title", it.key().titleInfo);
            writer.addAttribute("table:display", it.key().displayMessage);

            const QStringList errorLines = it.key().message.split('\n', Qt::SkipEmptyParts);
            for (const QString &errorLine : errorLines) {
                writer.startElement("text:p");
                writer.addTextNode(errorLine);
                writer.endElement();
            }
            writer.endElement();

            writer.endElement();
        }
        writer.endElement(); // close sheet:content-validation
    }
}

void GenValidationStyle::initVal(Validity *validity, CalculationSettings *cs)
{
    if (validity) {
        allowEmptyCell = (validity->allowEmptyCell() ? "true" : "false");
        condition = createValidationCondition(validity, cs);
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

QString GenValidationStyle::createValidationCondition(Validity *validity, CalculationSettings *cs)
{
    QString result;
    switch (validity->restriction()) {
    case Validity::NoRestriction:
        // nothing
        break;
    case Validity::Text:
        // doesn't exist into oo spec
        result = "cell-content-is-text()";
        break;
    case Validity::Time:
        result = createTimeValidationCondition(validity);
        break;
    case Validity::Date:
        result = createDateValidationCondition(validity, cs);
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

QString GenValidationStyle::createListValidationCondition(Validity *validity)
{
    QString result = "oooc:cell-content-is-in-list(" + validity->validityList().join(";") + ')';
    return result;
}

QString GenValidationStyle::createNumberValidationCondition(Validity *validity)
{
    QString result;
    if (validity->restriction() == Validity::Number)
        result = "oooc:cell-content-is-whole-number() and ";
    else if (validity->restriction() == Validity::Integer)
        result = "oooc:cell-content-is-decimal-number() and ";
    switch (validity->condition()) {
    case Validity::None:
    case Validity::IsTrueFormula:
        // nothing
        break;
    case Validity::Equal:
        result +=
            "cell-content()"
            "="
            + QString::number((double)numToDouble(validity->minimumValue().asFloat()));
        break;
    case Validity::Superior:
        result +=
            "cell-content()"
            ">"
            + QString::number((double)numToDouble(validity->minimumValue().asFloat()));
        break;
    case Validity::Inferior:
        result +=
            "cell-content()"
            "<"
            + QString::number((double)numToDouble(validity->minimumValue().asFloat()));
        break;
    case Validity::SuperiorEqual:
        result +=
            "cell-content()"
            ">="
            + QString::number((double)numToDouble(validity->minimumValue().asFloat()));
        break;
    case Validity::InferiorEqual:
        result +=
            "cell-content()"
            "<="
            + QString::number((double)numToDouble(validity->minimumValue().asFloat()));
        break;
    case Validity::Different:
        result +=
            "cell-content()"
            "!="
            + QString::number((double)numToDouble(validity->minimumValue().asFloat()));
        break;
    case Validity::Between:
        result += "cell-content-is-between(" + QString::number((double)numToDouble(validity->minimumValue().asFloat())) + ','
            + QString::number((double)numToDouble(validity->maximumValue().asFloat())) + ')';
        break;
    case Validity::DifferentTo:
        result += "cell-content-is-not-between(" + QString::number((double)numToDouble(validity->minimumValue().asFloat())) + ','
            + QString::number((double)numToDouble(validity->maximumValue().asFloat())) + ')';
        break;
    }
    return result;
}

QString GenValidationStyle::createTimeValidationCondition(Validity *validity)
{
    QString minval = toSaveString(validity->minimumValue(), Value::fmt_Time);
    QString maxval = toSaveString(validity->maximumValue(), Value::fmt_Time);

    QString result("oooc:cell-content-is-time() and ");
    switch (validity->condition()) {
    case Validity::None:
    case Validity::IsTrueFormula:
        // nothing
        break;
    case Validity::Equal:
        result += "cell-content()=" + minval;
        break;
    case Validity::Superior:
        result += "cell-content()>" + minval;
        break;
    case Validity::Inferior:
        result += "cell-content()<" + minval;
        break;
    case Validity::SuperiorEqual:
        result += "cell-content()>=" + minval;
        break;
    case Validity::InferiorEqual:
        result += "cell-content()<=" + minval;
        break;
    case Validity::Different:
        result += "cell-content()!=" + minval;
        break;
    case Validity::Between:
        result += "cell-content-is-between(" + minval + ',' + maxval + ')';
        break;
    case Validity::DifferentTo:
        result += "cell-content-is-not-between(" + minval + ',' + maxval + ')';
        break;
    }
    return result;
}

QString GenValidationStyle::createDateValidationCondition(Validity *validity, CalculationSettings *cs)
{
    QString minval = toSaveString(validity->minimumValue(), Value::fmt_Date, cs);
    QString maxval = toSaveString(validity->maximumValue(), Value::fmt_Date, cs);

    QString result("oooc:cell-content-is-date() and ");
    switch (validity->condition()) {
    case Validity::None:
    case Validity::IsTrueFormula:
        // nothing
        break;
    case Validity::Equal:
        result += "cell-content()=" + minval;
        break;
    case Validity::Superior:
        result += "cell-content()>" + minval;
        break;
    case Validity::Inferior:
        result += "cell-content()<" + minval;
        break;
    case Validity::SuperiorEqual:
        result += "cell-content()>=" + minval;
        break;
    case Validity::InferiorEqual:
        result += "cell-content()<=" + minval;
        break;
    case Validity::Different:
        result += "cell-content()!=" + minval;
        break;
    case Validity::Between:
        result += "cell-content-is-between(" + minval + ',' + maxval + ')';
        break;
    case Validity::DifferentTo:
        result += "cell-content-is-not-between(" + minval + ',' + maxval + ')';
        break;
    }
    return result;
}

QString GenValidationStyle::createTextValidationCondition(Validity *validity)
{
    QString result;
    switch (validity->condition()) {
    case Validity::None:
    case Validity::IsTrueFormula:
        // nothing
        break;
    case Validity::Equal:
        result +=
            "oooc:cell-content-text-length()"
            "="
            + QString::number((double)numToDouble(validity->minimumValue().asFloat()));
        break;
    case Validity::Superior:
        result +=
            "oooc:cell-content-text-length()"
            ">"
            + QString::number((double)numToDouble(validity->minimumValue().asFloat()));
        break;
    case Validity::Inferior:
        result +=
            "oooc:cell-content-text-length()"
            "<"
            + QString::number((double)numToDouble(validity->minimumValue().asFloat()));
        break;
    case Validity::SuperiorEqual:
        result +=
            "oooc:cell-content-text-length()"
            ">="
            + QString::number((double)numToDouble(validity->minimumValue().asFloat()));
        break;
    case Validity::InferiorEqual:
        result +=
            "oooc:cell-content-text-length()"
            "<="
            + QString::number((double)numToDouble(validity->minimumValue().asFloat()));
        break;
    case Validity::Different:
        result +=
            "oooc:cell-content-text-length()"
            "!="
            + QString::number((double)numToDouble(validity->minimumValue().asFloat()));
        break;
    case Validity::Between:
        result += "oooc:cell-content-text-length-is-between(" + QString::number((double)numToDouble(validity->minimumValue().asFloat())) + ','
            + QString::number((double)numToDouble(validity->maximumValue().asFloat())) + ')';
        break;
    case Validity::DifferentTo:
        result += "oooc:cell-content-text-length-is-not-between(" + QString::number((double)numToDouble(validity->minimumValue().asFloat())) + ','
            + QString::number((double)numToDouble(validity->maximumValue().asFloat())) + ')';
        break;
    }
    return result;
}
