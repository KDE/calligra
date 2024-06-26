/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2016 Tomas Mecir <mecirt@gmail.com>
   SPDX-FileCopyrightText: 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "SheetsOdf.h"
#include "SheetsOdfPrivate.h"

#include "engine/CS_Time.h"
#include "engine/CellBase.h"
#include "engine/MapBase.h"
#include "engine/SheetBase.h"
#include "engine/Validity.h"

#include <KoXmlNS.h>

namespace Calligra
{
namespace Sheets
{

namespace Odf
{
void loadValidationCondition(Validity *validity, QString &valExpression, CalculationSettings *cs);
void loadValidationValue(Validity *validity, const QStringList &listVal, CalculationSettings *cs);
}

void Odf::loadValidation(Validity *validity, CellBase *const cell, const QString &validationName, OdfLoadingContext &tableContext)
{
    KoXmlElement element = tableContext.validities.value(validationName);
    CalculationSettings *cs = cell->sheet()->map()->calculationSettings();
    if (element.hasAttributeNS(KoXmlNS::table, "condition")) {
        QString valExpression = element.attributeNS(KoXmlNS::table, "condition", QString());
        debugSheetsODF << " element.attribute( table:condition )" << valExpression;
        // Condition ::= ExtendedTrueCondition | TrueFunction 'and' TrueCondition
        // TrueFunction ::= cell-content-is-whole-number() | cell-content-is-decimal-number() | cell-content-is-date() | cell-content-is-time()
        // ExtendedTrueCondition ::= ExtendedGetFunction | cell-content-text-length() Operator Value
        // TrueCondition ::= GetFunction | cell-content() Operator Value
        // GetFunction ::= cell-content-is-between(Value, Value) | cell-content-is-not-between(Value, Value)
        // ExtendedGetFunction ::= cell-content-text-length-is-between(Value, Value) | cell-content-text-length-is-not-between(Value, Value)
        // Operator ::= '<' | '>' | '<=' | '>=' | '=' | '!='
        // Value ::= NumberValue | String | Formula
        // A Formula is a formula without an equals (=) sign at the beginning. See section 8.1.3 for more information.
        // A String comprises one or more characters surrounded by quotation marks.
        // A NumberValue is a whole or decimal number. It must not contain comma separators for numbers of 1000 or greater.

        // ExtendedTrueCondition
        if (valExpression.contains("cell-content-text-length()")) {
            //"cell-content-text-length()>45"
            valExpression = valExpression.remove("oooc:cell-content-text-length()");
            debugSheetsODF << " valExpression = :" << valExpression;
            validity->setRestriction(Validity::TextLength);

            loadValidationCondition(validity, valExpression, cs);
        } else if (valExpression.contains("cell-content-is-text()")) {
            validity->setRestriction(Validity::Text);
        }
        // cell-content-text-length-is-between(Value, Value) | cell-content-text-length-is-not-between(Value, Value) | cell-content-is-in-list( StringList )
        else if (valExpression.contains("cell-content-text-length-is-between")) {
            validity->setRestriction(Validity::TextLength);
            validity->setCondition(Validity::Between);
            valExpression.remove("oooc:cell-content-text-length-is-between(");
            debugSheetsODF << " valExpression :" << valExpression;
            valExpression.remove(')');
            QStringList listVal = valExpression.split(',', Qt::SkipEmptyParts);
            loadValidationValue(validity, listVal, cs);
        } else if (valExpression.contains("cell-content-text-length-is-not-between")) {
            validity->setRestriction(Validity::TextLength);
            validity->setCondition(Validity::Different);
            valExpression.remove("oooc:cell-content-text-length-is-not-between(");
            debugSheetsODF << " valExpression :" << valExpression;
            valExpression.remove(')');
            debugSheetsODF << " valExpression :" << valExpression;
            QStringList listVal = valExpression.split(',', Qt::SkipEmptyParts);
            loadValidationValue(validity, listVal, cs);
        } else if (valExpression.contains("cell-content-is-in-list(")) {
            validity->setRestriction(Validity::List);
            valExpression.remove("oooc:cell-content-is-in-list(");
            debugSheetsODF << " valExpression :" << valExpression;
            valExpression.remove(')');
            validity->setValidityList(valExpression.split(';', Qt::SkipEmptyParts));
        }
        // TrueFunction ::= cell-content-is-whole-number() | cell-content-is-decimal-number() | cell-content-is-date() | cell-content-is-time()
        else {
            if (valExpression.contains("cell-content-is-whole-number()")) {
                validity->setRestriction(Validity::Number);
                valExpression.remove("oooc:cell-content-is-whole-number() and ");
            } else if (valExpression.contains("cell-content-is-decimal-number()")) {
                validity->setRestriction(Validity::Integer);
                valExpression.remove("oooc:cell-content-is-decimal-number() and ");
            } else if (valExpression.contains("cell-content-is-date()")) {
                validity->setRestriction(Validity::Date);
                valExpression.remove("oooc:cell-content-is-date() and ");
            } else if (valExpression.contains("cell-content-is-time()")) {
                validity->setRestriction(Validity::Time);
                valExpression.remove("oooc:cell-content-is-time() and ");
            }
            debugSheetsODF << "valExpression :" << valExpression;

            if (valExpression.contains("cell-content()")) {
                valExpression.remove("cell-content()");
                loadValidationCondition(validity, valExpression, cs);
            }
            // GetFunction ::= cell-content-is-between(Value, Value) | cell-content-is-not-between(Value, Value)
            // for the moment we support just int/double value, not text/date/time :(
            if (valExpression.contains("cell-content-is-between(")) {
                valExpression.remove("cell-content-is-between(");
                valExpression.remove(')');
                QStringList listVal = valExpression.split(',', Qt::SkipEmptyParts);
                loadValidationValue(validity, listVal, cs);
                validity->setCondition(Validity::Between);
            }
            if (valExpression.contains("cell-content-is-not-between(")) {
                valExpression.remove("cell-content-is-not-between(");
                valExpression.remove(')');
                QStringList listVal = valExpression.split(',', Qt::SkipEmptyParts);
                loadValidationValue(validity, listVal, cs);
                validity->setCondition(Validity::Different);
            }
        }
    }
    if (element.hasAttributeNS(KoXmlNS::table, "allow-empty-cell")) {
        debugSheetsODF << " element.hasAttribute( table:allow-empty-cell ) :" << element.hasAttributeNS(KoXmlNS::table, "allow-empty-cell");
        validity->setAllowEmptyCell(((element.attributeNS(KoXmlNS::table, "allow-empty-cell", QString()) == "true") ? true : false));
    }
    if (element.hasAttributeNS(KoXmlNS::table, "base-cell-address")) {
        // todo what is it ?
    }

    KoXmlElement help = KoXml::namedItemNS(element, KoXmlNS::table, "help-message");
    if (!help.isNull()) {
        if (help.hasAttributeNS(KoXmlNS::table, "title")) {
            debugSheetsODF << "help.attribute( table:title ) :" << help.attributeNS(KoXmlNS::table, "title", QString());
            validity->setTitleInfo(help.attributeNS(KoXmlNS::table, "title", QString()));
        }
        if (help.hasAttributeNS(KoXmlNS::table, "display")) {
            debugSheetsODF << "help.attribute( table:display ) :" << help.attributeNS(KoXmlNS::table, "display", QString());
            validity->setDisplayValidationInformation(((help.attributeNS(KoXmlNS::table, "display", QString()) == "true") ? true : false));
        }
        KoXmlElement attrText = KoXml::namedItemNS(help, KoXmlNS::text, "p");
        if (!attrText.isNull()) {
            debugSheetsODF << "help text :" << attrText.text();
            validity->setMessageInfo(attrText.text());
        }
    }

    KoXmlElement error = KoXml::namedItemNS(element, KoXmlNS::table, "error-message");
    if (!error.isNull()) {
        if (error.hasAttributeNS(KoXmlNS::table, "title"))
            validity->setTitle(error.attributeNS(KoXmlNS::table, "title", QString()));
        if (error.hasAttributeNS(KoXmlNS::table, "message-type")) {
            QString str = error.attributeNS(KoXmlNS::table, "message-type", QString());
            if (str == "warning")
                validity->setAction(Validity::Warning);
            else if (str == "information")
                validity->setAction(Validity::Information);
            else if (str == "stop")
                validity->setAction(Validity::Stop);
            else
                debugSheetsODF << "validation : message type unknown  :" << str;
        }

        if (error.hasAttributeNS(KoXmlNS::table, "display")) {
            debugSheetsODF << " display message :" << error.attributeNS(KoXmlNS::table, "display", QString());
            validity->setDisplayMessage((error.attributeNS(KoXmlNS::table, "display", QString()) == "true"));
        }
        KoXmlElement attrText = KoXml::namedItemNS(error, KoXmlNS::text, "p");
        if (!attrText.isNull())
            validity->setMessage(attrText.text());
    }
}

void Odf::loadValidationValue(Validity *validity, const QStringList &listVal, CalculationSettings *cs)
{
    bool ok = false;
    debugSheetsODF << " listVal[0] :" << listVal[0] << " listVal[1] :" << listVal[1];

    if (validity->restriction() == Validity::Date) {
        validity->setMinimumValue(Value(QDate::fromString(listVal[0], Qt::ISODate), cs));
        validity->setMaximumValue(Value(QDate::fromString(listVal[1], Qt::ISODate), cs));
    } else if (validity->restriction() == Validity::Time) {
        validity->setMinimumValue(Value(Time(QTime::fromString(listVal[0], Qt::ISODate))));
        validity->setMaximumValue(Value(Time(QTime::fromString(listVal[1], Qt::ISODate))));
    } else {
        validity->setMinimumValue(Value(listVal[0].toDouble(&ok)));
        if (!ok) {
            validity->setMinimumValue(Value(listVal[0].toInt(&ok)));
            if (!ok)
                debugSheetsODF << " Try to parse this value :" << listVal[0];

#if 0
            if (!ok)
                validity->setMinimumValue(listVal[0]);
#endif
        }
        ok = false;
        validity->setMaximumValue(Value(listVal[1].toDouble(&ok)));
        if (!ok) {
            validity->setMaximumValue(Value(listVal[1].toInt(&ok)));
            if (!ok)
                debugSheetsODF << " Try to parse this value :" << listVal[1];

#if 0
            if (!ok)
                validity->setMaximumValue(listVal[1]);
#endif
        }
    }
}

void Odf::loadValidationCondition(Validity *validity, QString &valExpression, CalculationSettings *cs)
{
    if (validity->isEmpty())
        return;
    QString value;
    if (valExpression.indexOf("<=") == 0) {
        value = valExpression.remove(0, 2);
        validity->setCondition(Validity::InferiorEqual);
    } else if (valExpression.indexOf(">=") == 0) {
        value = valExpression.remove(0, 2);
        validity->setCondition(Validity::SuperiorEqual);
    } else if (valExpression.indexOf("!=") == 0) {
        // add Differentto attribute
        value = valExpression.remove(0, 2);
        validity->setCondition(Validity::DifferentTo);
    } else if (valExpression.indexOf('<') == 0) {
        value = valExpression.remove(0, 1);
        validity->setCondition(Validity::Inferior);
    } else if (valExpression.indexOf('>') == 0) {
        value = valExpression.remove(0, 1);
        validity->setCondition(Validity::Superior);
    } else if (valExpression.indexOf('=') == 0) {
        value = valExpression.remove(0, 1);
        validity->setCondition(Validity::Equal);
    } else
        debugSheetsODF << " I don't know how to parse it :" << valExpression;
    if (validity->restriction() == Validity::Date) {
        validity->setMinimumValue(Value(QDate::fromString(value, Qt::ISODate), cs));
    } else if (validity->restriction() == Validity::Time) {
        validity->setMinimumValue(Value(Time(QTime::fromString(value, Qt::ISODate))));
    } else {
        bool ok = false;
        validity->setMinimumValue(Value(value.toDouble(&ok)));
        if (!ok) {
            validity->setMinimumValue(Value(value.toInt(&ok)));
            if (!ok)
                debugSheetsODF << " Try to parse this value :" << value;

#if 0
            if (!ok)
                validity->setMinimumValue(value);
#endif
        }
    }
}

} // Sheets
} // Calligra
