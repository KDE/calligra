/* This file is part of the KDE project
   Copyright 2016 Tomas Mecir <mecirt@gmail.com>
   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#include "SheetsOdf.h"
#include "SheetsOdfPrivate.h"

#include "Cell.h"
#include "Map.h"
#include "Sheet.h"
#include "Validity.h"
#include "ValueParser.h"

#include <KoXmlReader.h>
#include <KoXmlNS.h>

#include "OdfLoadingContext.h"

namespace Calligra {
namespace Sheets {

namespace Odf {
    void loadValidationCondition(Validity *validity, QString &valExpression, const ValueParser *parser);
    void loadValidationValue(Validity *validity, const QStringList &listVal, const ValueParser *parser);
}

void Odf::loadValidation(Validity *validity, Cell* const cell, const QString& validationName,
                                 OdfLoadingContext& tableContext)
{
    KoXmlElement element = tableContext.validities.value(validationName);
    if (element.hasAttributeNS(KoXmlNS::table, "condition")) {
        QString valExpression = element.attributeNS(KoXmlNS::table, "condition", QString());
        debugSheetsODF << " element.attribute( table:condition )" << valExpression;
        //Condition ::= ExtendedTrueCondition | TrueFunction 'and' TrueCondition
        //TrueFunction ::= cell-content-is-whole-number() | cell-content-is-decimal-number() | cell-content-is-date() | cell-content-is-time()
        //ExtendedTrueCondition ::= ExtendedGetFunction | cell-content-text-length() Operator Value
        //TrueCondition ::= GetFunction | cell-content() Operator Value
        //GetFunction ::= cell-content-is-between(Value, Value) | cell-content-is-not-between(Value, Value)
        //ExtendedGetFunction ::= cell-content-text-length-is-between(Value, Value) | cell-content-text-length-is-not-between(Value, Value)
        //Operator ::= '<' | '>' | '<=' | '>=' | '=' | '!='
        //Value ::= NumberValue | String | Formula
        //A Formula is a formula without an equals (=) sign at the beginning. See section 8.1.3 for more information.
        //A String comprises one or more characters surrounded by quotation marks.
        //A NumberValue is a whole or decimal number. It must not contain comma separators for numbers of 1000 or greater.

        //ExtendedTrueCondition
        if (valExpression.contains("cell-content-text-length()")) {
            //"cell-content-text-length()>45"
            valExpression = valExpression.remove("oooc:cell-content-text-length()");
            debugSheetsODF << " valExpression = :" << valExpression;
            validity->setRestriction(Validity::TextLength);

            loadValidationCondition(validity, valExpression, cell->sheet()->map()->parser());
        } else if (valExpression.contains("cell-content-is-text()")) {
            validity->setRestriction(Validity::Text);
        }
        //cell-content-text-length-is-between(Value, Value) | cell-content-text-length-is-not-between(Value, Value) | cell-content-is-in-list( StringList )
        else if (valExpression.contains("cell-content-text-length-is-between")) {
            validity->setRestriction(Validity::TextLength);
            validity->setCondition(Conditional::Between);
            valExpression.remove("oooc:cell-content-text-length-is-between(");
            debugSheetsODF << " valExpression :" << valExpression;
            valExpression.remove(')');
            QStringList listVal = valExpression.split(',', QString::SkipEmptyParts);
            loadValidationValue(validity, listVal, cell->sheet()->map()->parser());
        } else if (valExpression.contains("cell-content-text-length-is-not-between")) {
            validity->setRestriction(Validity::TextLength);
            validity->setCondition(Conditional::Different);
            valExpression.remove("oooc:cell-content-text-length-is-not-between(");
            debugSheetsODF << " valExpression :" << valExpression;
            valExpression.remove(')');
            debugSheetsODF << " valExpression :" << valExpression;
            QStringList listVal = valExpression.split(',', QString::SkipEmptyParts);
            loadValidationValue(validity, listVal, cell->sheet()->map()->parser());
        } else if (valExpression.contains("cell-content-is-in-list(")) {
            validity->setRestriction(Validity::List);
            valExpression.remove("oooc:cell-content-is-in-list(");
            debugSheetsODF << " valExpression :" << valExpression;
            valExpression.remove(')');
            validity->setValidityList(valExpression.split(';',  QString::SkipEmptyParts));
        }
        //TrueFunction ::= cell-content-is-whole-number() | cell-content-is-decimal-number() | cell-content-is-date() | cell-content-is-time()
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
                loadValidationCondition(validity, valExpression, cell->sheet()->map()->parser());
            }
            //GetFunction ::= cell-content-is-between(Value, Value) | cell-content-is-not-between(Value, Value)
            //for the moment we support just int/double value, not text/date/time :(
            if (valExpression.contains("cell-content-is-between(")) {
                valExpression.remove("cell-content-is-between(");
                valExpression.remove(')');
                QStringList listVal = valExpression.split(',', QString::SkipEmptyParts);
                loadValidationValue(validity, listVal, cell->sheet()->map()->parser());
                validity->setCondition(Conditional::Between);
            }
            if (valExpression.contains("cell-content-is-not-between(")) {
                valExpression.remove("cell-content-is-not-between(");
                valExpression.remove(')');
                QStringList listVal = valExpression.split(',', QString::SkipEmptyParts);
                loadValidationValue(validity, listVal, cell->sheet()->map()->parser());
                validity->setCondition(Conditional::Different);
            }
        }
    }
    if (element.hasAttributeNS(KoXmlNS::table, "allow-empty-cell")) {
        debugSheetsODF << " element.hasAttribute( table:allow-empty-cell ) :" << element.hasAttributeNS(KoXmlNS::table, "allow-empty-cell");
        validity->setAllowEmptyCell(((element.attributeNS(KoXmlNS::table, "allow-empty-cell", QString()) == "true") ? true : false));
    }
    if (element.hasAttributeNS(KoXmlNS::table, "base-cell-address")) {
        //todo what is it ?
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

void Odf::loadValidationValue(Validity *validity, const QStringList &listVal, const ValueParser *parser)
{
    bool ok = false;
    debugSheetsODF << " listVal[0] :" << listVal[0] << " listVal[1] :" << listVal[1];

    if (validity->restriction() == Validity::Date) {
        validity->setMinimumValue(parser->tryParseDate(listVal[0]));
        validity->setMaximumValue(parser->tryParseDate(listVal[1]));
    } else if (validity->restriction() == Validity::Time) {
        validity->setMinimumValue(parser->tryParseTime(listVal[0]));
        validity->setMaximumValue(parser->tryParseTime(listVal[1]));
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

void Odf::loadValidationCondition(Validity *validity, QString &valExpression, const ValueParser *parser)
{
    if (validity->isEmpty()) return;
    QString value;
    if (valExpression.indexOf("<=") == 0) {
        value = valExpression.remove(0, 2);
        validity->setCondition(Conditional::InferiorEqual);
    } else if (valExpression.indexOf(">=") == 0) {
        value = valExpression.remove(0, 2);
        validity->setCondition(Conditional::SuperiorEqual);
    } else if (valExpression.indexOf("!=") == 0) {
        //add Differentto attribute
        value = valExpression.remove(0, 2);
        validity->setCondition(Conditional::DifferentTo);
    } else if (valExpression.indexOf('<') == 0) {
        value = valExpression.remove(0, 1);
        validity->setCondition(Conditional::Inferior);
    } else if (valExpression.indexOf('>') == 0) {
        value = valExpression.remove(0, 1);
        validity->setCondition(Conditional::Superior);
    } else if (valExpression.indexOf('=') == 0) {
        value = valExpression.remove(0, 1);
        validity->setCondition(Conditional::Equal);
    } else
        debugSheetsODF << " I don't know how to parse it :" << valExpression;
    if (validity->restriction() == Validity::Date) {
        validity->setMinimumValue(parser->tryParseDate(value));
    } else if (validity->restriction() == Validity::Date) {
        validity->setMinimumValue(parser->tryParseTime(value));
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


}  // Sheets
}  // Calligra
