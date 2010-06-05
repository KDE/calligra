/* This file is part of the KDE project
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

// Local
#include "Validity.h"

#include <float.h>

// Qt
#include <QTimer>

// KDE
#include <kmessagebox.h>

// KOffice
#include <KoXmlReader.h>
#include <KoXmlNS.h>

// KSpread
#include "CalculationSettings.h"
#include "Cell.h"
#include "Map.h"
#include "OdfLoadingContext.h"
#include "Sheet.h"
#include "Value.h"

using namespace KSpread;

class Validity::Private : public QSharedData
{
public:
    QString message;
    QString title;
    QString titleInfo;
    QString messageInfo;
    double valMin;
    double valMax;
    Conditional::Type cond;
    Action action;
    Restriction restriction;
    QTime  timeMin;
    QTime  timeMax;
    QDate  dateMin;
    QDate  dateMax;
    bool displayMessage;
    bool allowEmptyCell;
    bool displayValidationInformation;
    QStringList listValidity;
};

Validity::Validity()
        : d(new Private)
{
    d->valMin = 0.0;
    d->valMax = 0.0;
    d->cond = Conditional::None;
    d->action = Stop;
    d->restriction = None;
    d->displayMessage = true;
    d->allowEmptyCell = false;
    d->displayValidationInformation = false;
}

Validity::Validity(const Validity& other)
        : d(other.d)
{
}

Validity::~Validity()
{
}

bool Validity::isEmpty() const
{
    return d->restriction == None;
}

bool Validity::loadXML(Cell* const cell, const KoXmlElement& validityElement)
{
    bool ok = false;
    KoXmlElement param = validityElement.namedItem("param").toElement();
    if (!param.isNull()) {
        if (param.hasAttribute("cond")) {
            d->cond = (Conditional::Type) param.attribute("cond").toInt(&ok);
            if (!ok)
                return false;
        }
        if (param.hasAttribute("action")) {
            d->action = (Action) param.attribute("action").toInt(&ok);
            if (!ok)
                return false;
        }
        if (param.hasAttribute("allow")) {
            d->restriction = (Restriction) param.attribute("allow").toInt(&ok);
            if (!ok)
                return false;
        }
        if (param.hasAttribute("valmin")) {
            d->valMin = param.attribute("valmin").toDouble(&ok);
            if (!ok)
                return false;
        }
        if (param.hasAttribute("valmax")) {
            d->valMax = param.attribute("valmax").toDouble(&ok);
            if (!ok)
                return false;
        }
        if (param.hasAttribute("displaymessage")) {
            d->displayMessage = (bool)param.attribute("displaymessage").toInt();
        }
        if (param.hasAttribute("displayvalidationinformation")) {
            d->displayValidationInformation = (bool)param.attribute("displayvalidationinformation").toInt();
        }
        if (param.hasAttribute("allowemptycell")) {
            d->allowEmptyCell = (bool)param.attribute("allowemptycell").toInt();
        }
        if (param.hasAttribute("listvalidity")) {
            d->listValidity = param.attribute("listvalidity").split(';', QString::SkipEmptyParts);
        }
    }
    KoXmlElement inputTitle = validityElement.namedItem("inputtitle").toElement();
    if (!inputTitle.isNull()) {
        d->titleInfo = inputTitle.text();
    }
    KoXmlElement inputMessage = validityElement.namedItem("inputmessage").toElement();
    if (!inputMessage.isNull()) {
        d->messageInfo = inputMessage.text();
    }

    KoXmlElement titleElement = validityElement.namedItem("title").toElement();
    if (!titleElement.isNull()) {
        d->title = titleElement.text();
    }
    KoXmlElement messageElement = validityElement.namedItem("message").toElement();
    if (!messageElement.isNull()) {
        d->message = messageElement.text();
    }
    KoXmlElement timeMinElement = validityElement.namedItem("timemin").toElement();
    if (!timeMinElement.isNull()) {
        d->timeMin = cell->toTime(timeMinElement);
    }
    KoXmlElement timeMaxElement = validityElement.namedItem("timemax").toElement();
    if (!timeMaxElement.isNull()) {
        d->timeMax = cell->toTime(timeMaxElement);
    }
    KoXmlElement dateMinElement = validityElement.namedItem("datemin").toElement();
    if (!dateMinElement.isNull()) {
        d->dateMin = cell->toDate(dateMinElement);
    }
    KoXmlElement dateMaxElement = validityElement.namedItem("datemax").toElement();
    if (!dateMaxElement.isNull()) {
        d->dateMax = cell->toDate(dateMaxElement);
    }
    return true;
}

QDomElement Validity::saveXML(QDomDocument& doc) const
{
    QDomElement validityElement = doc.createElement("validity");

    QDomElement param = doc.createElement("param");
    param.setAttribute("cond", (int)d->cond);
    param.setAttribute("action", (int)d->action);
    param.setAttribute("allow", (int)d->restriction);
    param.setAttribute("valmin", d->valMin);
    param.setAttribute("valmax", d->valMax);
    param.setAttribute("displaymessage", d->displayMessage);
    param.setAttribute("displayvalidationinformation", d->displayValidationInformation);
    param.setAttribute("allowemptycell", d->allowEmptyCell);
    if (!d->listValidity.isEmpty())
        param.setAttribute("listvalidity", d->listValidity.join(";"));
    validityElement.appendChild(param);
    QDomElement titleElement = doc.createElement("title");
    titleElement.appendChild(doc.createTextNode(d->title));
    validityElement.appendChild(titleElement);
    QDomElement messageElement = doc.createElement("message");
    messageElement.appendChild(doc.createCDATASection(d->message));
    validityElement.appendChild(messageElement);

    QDomElement inputTitle = doc.createElement("inputtitle");
    inputTitle.appendChild(doc.createTextNode(d->titleInfo));
    validityElement.appendChild(inputTitle);

    QDomElement inputMessage = doc.createElement("inputmessage");
    inputMessage.appendChild(doc.createTextNode(d->messageInfo));
    validityElement.appendChild(inputMessage);



    QString tmp;
    if (d->timeMin.isValid()) {
        QDomElement timeMinElement = doc.createElement("timemin");
        tmp = d->timeMin.toString();
        timeMinElement.appendChild(doc.createTextNode(tmp));
        validityElement.appendChild(timeMinElement);
    }
    if (d->timeMax.isValid()) {
        QDomElement timeMaxElement = doc.createElement("timemax");
        tmp = d->timeMax.toString();
        timeMaxElement.appendChild(doc.createTextNode(tmp));
        validityElement.appendChild(timeMaxElement);
    }

    if (d->dateMin.isValid()) {
        QDomElement dateMinElement = doc.createElement("datemin");
        QString tmp("%1/%2/%3");
        tmp = tmp.arg(d->dateMin.year()).arg(d->dateMin.month()).arg(d->dateMin.day());
        dateMinElement.appendChild(doc.createTextNode(tmp));
        validityElement.appendChild(dateMinElement);
    }
    if (d->dateMax.isValid()) {
        QDomElement dateMaxElement = doc.createElement("datemax");
        QString tmp("%1/%2/%3");
        tmp = tmp.arg(d->dateMax.year()).arg(d->dateMax.month()).arg(d->dateMax.day());
        dateMaxElement.appendChild(doc.createTextNode(tmp));
        validityElement.appendChild(dateMaxElement);
    }
    return validityElement;
}


void Validity::loadOdfValidation(Cell* const cell, const QString& validationName,
                                 OdfLoadingContext& tableContext)
{
    KoXmlElement element = tableContext.validities.value(validationName);
    Validity validity;
    if (element.hasAttributeNS(KoXmlNS::table, "condition")) {
        QString valExpression = element.attributeNS(KoXmlNS::table, "condition", QString());
        kDebug(36003) << " element.attribute( table:condition )" << valExpression;
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
            kDebug(36003) << " valExpression = :" << valExpression;
            setRestriction(Validity::TextLength);

            loadOdfValidationCondition(valExpression);
        } else if (valExpression.contains("cell-content-is-text()")) {
            setRestriction(Validity::Text);
        }
        //cell-content-text-length-is-between(Value, Value) | cell-content-text-length-is-not-between(Value, Value) | cell-content-is-in-list( StringList )
        else if (valExpression.contains("cell-content-text-length-is-between")) {
            setRestriction(Validity::TextLength);
            setCondition(Conditional::Between);
            valExpression = valExpression.remove("oooc:cell-content-text-length-is-between(");
            kDebug(36003) << " valExpression :" << valExpression;
            valExpression = valExpression.remove(')');
            QStringList listVal = valExpression.split(',', QString::SkipEmptyParts);
            loadOdfValidationValue(listVal);
        } else if (valExpression.contains("cell-content-text-length-is-not-between")) {
            setRestriction(Validity::TextLength);
            setCondition(Conditional::Different);
            valExpression = valExpression.remove("oooc:cell-content-text-length-is-not-between(");
            kDebug(36003) << " valExpression :" << valExpression;
            valExpression = valExpression.remove(')');
            kDebug(36003) << " valExpression :" << valExpression;
            QStringList listVal = valExpression.split(',', QString::SkipEmptyParts);
            loadOdfValidationValue(listVal);
        } else if (valExpression.contains("cell-content-is-in-list(")) {
            setRestriction(Validity::List);
            valExpression = valExpression.remove("oooc:cell-content-is-in-list(");
            kDebug(36003) << " valExpression :" << valExpression;
            valExpression = valExpression.remove(')');
            setValidityList(valExpression.split(';',  QString::SkipEmptyParts));

        }
        //TrueFunction ::= cell-content-is-whole-number() | cell-content-is-decimal-number() | cell-content-is-date() | cell-content-is-time()
        else {
            if (valExpression.contains("cell-content-is-whole-number()")) {
                setRestriction(Validity::Number);
                valExpression = valExpression.remove("oooc:cell-content-is-whole-number() and ");
            } else if (valExpression.contains("cell-content-is-decimal-number()")) {
                setRestriction(Validity::Integer);
                valExpression = valExpression.remove("oooc:cell-content-is-decimal-number() and ");
            } else if (valExpression.contains("cell-content-is-date()")) {
                setRestriction(Validity::Date);
                valExpression = valExpression.remove("oooc:cell-content-is-date() and ");
            } else if (valExpression.contains("cell-content-is-time()")) {
                setRestriction(Validity::Time);
                valExpression = valExpression.remove("oooc:cell-content-is-time() and ");
            }
            kDebug(36003) << "valExpression :" << valExpression;

            if (valExpression.contains("cell-content()")) {
                valExpression = valExpression.remove("cell-content()");
                loadOdfValidationCondition(valExpression);
            }
            //GetFunction ::= cell-content-is-between(Value, Value) | cell-content-is-not-between(Value, Value)
            //for the moment we support just int/double value, not text/date/time :(
            if (valExpression.contains("cell-content-is-between(")) {
                valExpression = valExpression.remove("cell-content-is-between(");
                valExpression = valExpression.remove(')');
                QStringList listVal = valExpression.split(',', QString::SkipEmptyParts);
                loadOdfValidationValue(listVal);
                setCondition(Conditional::Between);
            }
            if (valExpression.contains("cell-content-is-not-between(")) {
                valExpression = valExpression.remove("cell-content-is-not-between(");
                valExpression = valExpression.remove(')');
                QStringList listVal = valExpression.split(',', QString::SkipEmptyParts);
                loadOdfValidationValue(listVal);
                setCondition(Conditional::Different);
            }
        }
    }
    if (element.hasAttributeNS(KoXmlNS::table, "allow-empty-cell")) {
        kDebug(36003) << " element.hasAttribute( table:allow-empty-cell ) :" << element.hasAttributeNS(KoXmlNS::table, "allow-empty-cell");
        setAllowEmptyCell(((element.attributeNS(KoXmlNS::table, "allow-empty-cell", QString()) == "true") ? true : false));
    }
    if (element.hasAttributeNS(KoXmlNS::table, "base-cell-address")) {
        //todo what is it ?
    }

    KoXmlElement help = KoXml::namedItemNS(element, KoXmlNS::table, "help-message");
    if (!help.isNull()) {
        if (help.hasAttributeNS(KoXmlNS::table, "title")) {
            kDebug(36003) << "help.attribute( table:title ) :" << help.attributeNS(KoXmlNS::table, "title", QString());
            setTitleInfo(help.attributeNS(KoXmlNS::table, "title", QString()));
        }
        if (help.hasAttributeNS(KoXmlNS::table, "display")) {
            kDebug(36003) << "help.attribute( table:display ) :" << help.attributeNS(KoXmlNS::table, "display", QString());
            setDisplayValidationInformation(((help.attributeNS(KoXmlNS::table, "display", QString()) == "true") ? true : false));
        }
        KoXmlElement attrText = KoXml::namedItemNS(help, KoXmlNS::text, "p");
        if (!attrText.isNull()) {
            kDebug(36003) << "help text :" << attrText.text();
            setMessageInfo(attrText.text());
        }
    }

    KoXmlElement error = KoXml::namedItemNS(element, KoXmlNS::table, "error-message");
    if (!error.isNull()) {
        if (error.hasAttributeNS(KoXmlNS::table, "title"))
            setTitle(error.attributeNS(KoXmlNS::table, "title", QString()));
        if (error.hasAttributeNS(KoXmlNS::table, "message-type")) {
            QString str = error.attributeNS(KoXmlNS::table, "message-type", QString());
            if (str == "warning")
                setAction(Validity::Warning);
            else if (str == "information")
                setAction(Validity::Information);
            else if (str == "stop")
                setAction(Validity::Stop);
            else
                kDebug(36003) << "validation : message type unknown  :" << str;
        }

        if (error.hasAttributeNS(KoXmlNS::table, "display")) {
            kDebug(36003) << " display message :" << error.attributeNS(KoXmlNS::table, "display", QString());
            setDisplayMessage((error.attributeNS(KoXmlNS::table, "display", QString()) == "true"));
        }
        KoXmlElement attrText = KoXml::namedItemNS(error, KoXmlNS::text, "p");
        if (!attrText.isNull())
            setMessage(attrText.text());
    }
    cell->setValidity(validity);
}

void Validity::loadOdfValidationValue(const QStringList &listVal)
{
    bool ok = false;
    kDebug(36003) << " listVal[0] :" << listVal[0] << " listVal[1] :" << listVal[1];

    if (restriction() == Validity::Date) {
        setMinimumDate(QDate::fromString(listVal[0]));
        setMaximumDate(QDate::fromString(listVal[1]));
    } else if (restriction() == Validity::Time) {
        setMinimumTime(QTime::fromString(listVal[0]));
        setMaximumTime(QTime::fromString(listVal[1]));
    } else {
        setMinimumValue(listVal[0].toDouble(&ok));
        if (!ok) {
            setMinimumValue(listVal[0].toInt(&ok));
            if (!ok)
                kDebug(36003) << " Try to parse this value :" << listVal[0];

#if 0
            if (!ok)
                setMinimumValue(listVal[0]);
#endif
        }
        ok = false;
        setMaximumValue(listVal[1].toDouble(&ok));
        if (!ok) {
            setMaximumValue(listVal[1].toInt(&ok));
            if (!ok)
                kDebug(36003) << " Try to parse this value :" << listVal[1];

#if 0
            if (!ok)
                setMaximumValue(listVal[1]);
#endif
        }
    }
}

void Validity::loadOdfValidationCondition(QString &valExpression)
{
    if (isEmpty()) return;
    QString value;
    if (valExpression.indexOf("<=") == 0) {
        value = valExpression.remove(0, 2);
        setCondition(Conditional::InferiorEqual);
    } else if (valExpression.indexOf(">=") == 0) {
        value = valExpression.remove(0, 2);
        setCondition(Conditional::SuperiorEqual);
    } else if (valExpression.indexOf("!=") == 0) {
        //add Differentto attribute
        value = valExpression.remove(0, 2);
        setCondition(Conditional::DifferentTo);
    } else if (valExpression.indexOf('<') == 0) {
        value = valExpression.remove(0, 1);
        setCondition(Conditional::Inferior);
    } else if (valExpression.indexOf('>') == 0) {
        value = valExpression.remove(0, 1);
        setCondition(Conditional::Superior);
    } else if (valExpression.indexOf('=') == 0) {
        value = valExpression.remove(0, 1);
        setCondition(Conditional::Equal);
    } else
        kDebug(36003) << " I don't know how to parse it :" << valExpression;
    if (restriction() == Validity::Date) {
        setMinimumDate(QDate::fromString(value));
    } else if (restriction() == Validity::Date) {
        setMinimumTime(QTime::fromString(value));
    } else {
        bool ok = false;
        setMinimumValue(value.toDouble(&ok));
        if (!ok) {
            setMinimumValue(value.toInt(&ok));
            if (!ok)
                kDebug(36003) << " Try to parse this value :" << value;

#if 0
            if (!ok)
                setMinimumValue(value);
#endif
        }
    }
}

Validity::Action Validity::action() const
{
    return d->action;
}

bool Validity::allowEmptyCell() const
{
    return d->allowEmptyCell;
}

Conditional::Type Validity::condition() const
{
    return d->cond;
}

bool Validity::displayMessage() const
{
    return d->displayMessage;
}

bool Validity::displayValidationInformation() const
{
    return d->displayValidationInformation;
}

const QString& Validity::messageInfo() const
{
    return d->messageInfo;
}

const QDate& Validity::maximumDate() const
{
    return d->dateMax;
}

const QTime& Validity::maximumTime() const
{
    return d->timeMax;
}

double Validity::maximumValue() const
{
    return d->valMax;
}

const QString& Validity::message() const
{
    return d->message;
}

const QDate& Validity::minimumDate() const
{
    return d->dateMin;
}

const QTime& Validity::minimumTime() const
{
    return d->timeMin;
}

double Validity::minimumValue() const
{
    return d->valMin;
}

Validity::Restriction Validity::restriction() const
{
    return d->restriction;
}

const QString& Validity::title() const
{
    return d->title;
}

const QString& Validity::titleInfo() const
{
    return d->titleInfo;
}

const QStringList& Validity::validityList() const
{
    return d->listValidity;
}

void Validity::setAction(Action action)
{
    d->action = action;
}

void Validity::setAllowEmptyCell(bool allow)
{
    d->allowEmptyCell = allow;
}

void Validity::setCondition(Conditional::Type condition)
{
    d->cond = condition;
}

void Validity::setDisplayMessage(bool display)
{
    d->displayMessage = display;
}

void Validity::setDisplayValidationInformation(bool display)
{
    d->displayValidationInformation = display;
}

void Validity::setMaximumDate(const QDate& date)
{
    d->dateMax = date;
}

void Validity::setMaximumTime(const QTime& time)
{
    d->timeMax = time;
}

void Validity::setMaximumValue(double value)
{
    d->valMax = value;
}

void Validity::setMessage(const QString& msg)
{
    d->message = msg;
}

void Validity::setMessageInfo(const QString& info)
{
    d->messageInfo = info;
}

void Validity::setMinimumDate(const QDate& date)
{
    d->dateMin = date;
}

void Validity::setMinimumTime(const QTime& time)
{
    d->timeMin = time;
}

void Validity::setMinimumValue(double value)
{
    d->valMin = value;
}

void Validity::setRestriction(Restriction restriction)
{
    d->restriction = restriction;
}

void Validity::setTitle(const QString& t)
{
    d->title = t;
}

void Validity::setTitleInfo(const QString& info)
{
    d->titleInfo = info;
}

void Validity::setValidityList(const QStringList& list)
{
    d->listValidity = list;
}

bool Validity::testValidity(const Cell* cell) const
{
    const CalculationSettings* settings = cell->sheet()->map()->calculationSettings();
    bool valid = false;
    if (d->restriction != None) {
        //fixme
        if (d->allowEmptyCell && cell->userInput().isEmpty())
            return true;

        if (cell->value().isNumber() &&
                (d->restriction == Number ||
                 (d->restriction == Integer &&
                  numToDouble(cell->value().asFloat()) == ceil(numToDouble(cell->value().asFloat()))))) {
            switch (d->cond) {
            case Conditional::Equal:
                valid = (cell->value().asFloat() - d->valMin < DBL_EPSILON
                         && cell->value().asFloat() - d->valMin >
                         (0.0 - DBL_EPSILON));
                break;
            case Conditional::DifferentTo:
                valid = !((cell->value().asFloat() - d->valMin < DBL_EPSILON
                           && cell->value().asFloat() - d->valMin >
                           (0.0 - DBL_EPSILON)));
                break;
            case Conditional::Superior:
                valid = (cell->value().asFloat() > d->valMin);
                break;
            case Conditional::Inferior:
                valid = (cell->value().asFloat()  < d->valMin);
                break;
            case Conditional::SuperiorEqual:
                valid = (cell->value().asFloat() >= d->valMin);
                break;
            case Conditional::InferiorEqual:
                valid = (cell->value().asFloat() <= d->valMin);
                break;
            case Conditional::Between:
                valid = (cell->value().asFloat() >= d->valMin &&
                         cell->value().asFloat() <= d->valMax);
                break;
            case Conditional::Different:
                valid = (cell->value().asFloat() < d->valMin ||
                         cell->value().asFloat() > d->valMax);
                break;
            default :
                break;
            }
        } else if (d->restriction == Text) {
            valid = cell->value().isString();
        } else if (d->restriction == List) {
            //test int value
            if (cell->value().isString() && d->listValidity.contains(cell->value().asString()))
                valid = true;
        } else if (d->restriction == TextLength) {
            if (cell->value().isString()) {
                int len = cell->displayText().length();
                switch (d->cond) {
                case Conditional::Equal:
                    if (len == d->valMin)
                        valid = true;
                    break;
                case Conditional::DifferentTo:
                    if (len != d->valMin)
                        valid = true;
                    break;
                case Conditional::Superior:
                    if (len > d->valMin)
                        valid = true;
                    break;
                case Conditional::Inferior:
                    if (len < d->valMin)
                        valid = true;
                    break;
                case Conditional::SuperiorEqual:
                    if (len >= d->valMin)
                        valid = true;
                    break;
                case Conditional::InferiorEqual:
                    if (len <= d->valMin)
                        valid = true;
                    break;
                case Conditional::Between:
                    if (len >= d->valMin && len <= d->valMax)
                        valid = true;
                    break;
                case Conditional::Different:
                    if (len < d->valMin || len > d->valMax)
                        valid = true;
                    break;
                default :
                    break;
                }
            }
        } else if (d->restriction == Time && cell->isTime()) {
            switch (d->cond) {
            case Conditional::Equal:
                valid = (cell->value().asTime(settings) == d->timeMin);
                break;
            case Conditional::DifferentTo:
                valid = (cell->value().asTime(settings) != d->timeMin);
                break;
            case Conditional::Superior:
                valid = (cell->value().asTime(settings) > d->timeMin);
                break;
            case Conditional::Inferior:
                valid = (cell->value().asTime(settings) < d->timeMin);
                break;
            case Conditional::SuperiorEqual:
                valid = (cell->value().asTime(settings) >= d->timeMin);
                break;
            case Conditional::InferiorEqual:
                valid = (cell->value().asTime(settings) <= d->timeMin);
                break;
            case Conditional::Between:
                valid = (cell->value().asTime(settings) >= d->timeMin &&
                         cell->value().asTime(settings) <= d->timeMax);
                break;
            case Conditional::Different:
                valid = (cell->value().asTime(settings) < d->timeMin ||
                         cell->value().asTime(settings) > d->timeMax);
                break;
            default :
                break;

            }
        } else if (d->restriction == Date && cell->isDate()) {
            switch (d->cond) {
            case Conditional::Equal:
                valid = (cell->value().asDate(settings) == d->dateMin);
                break;
            case Conditional::DifferentTo:
                valid = (cell->value().asDate(settings) != d->dateMin);
                break;
            case Conditional::Superior:
                valid = (cell->value().asDate(settings) > d->dateMin);
                break;
            case Conditional::Inferior:
                valid = (cell->value().asDate(settings) < d->dateMin);
                break;
            case Conditional::SuperiorEqual:
                valid = (cell->value().asDate(settings) >= d->dateMin);
                break;
            case Conditional::InferiorEqual:
                valid = (cell->value().asDate(settings) <= d->dateMin);
                break;
            case Conditional::Between:
                valid = (cell->value().asDate(settings) >= d->dateMin &&
                         cell->value().asDate(settings) <= d->dateMax);
                break;
            case Conditional::Different:
                valid = (cell->value().asDate(settings) < d->dateMin ||
                         cell->value().asDate(settings) > d->dateMax);
                break;
            default :
                break;

            }
        }
    } else {
        valid = true;
    }

    if (!valid) {
        if (d->displayMessage) {
            switch (d->action) {
            case Stop:
                KMessageBox::error((QWidget*)0, d->message, d->title);
                break;
            case Warning:
                KMessageBox::warningYesNo((QWidget*)0, d->message, d->title);
                break;
            case Information:
                KMessageBox::information((QWidget*)0, d->message, d->title);
                break;
            }
        }

        cell->sheet()->showStatusMessage(i18n("Validation for cell %1 failed", cell->fullName()));
    }
    return (valid || d->action != Stop);
}

void Validity::operator=(const Validity & other)
{
    d = other.d;
}

bool Validity::operator==(const Validity& other) const
{
    if (d->message == other.d->message &&
            d->title == other.d->title &&
            d->titleInfo == other.d->titleInfo &&
            d->messageInfo == other.d->messageInfo &&
            d->valMin == other.d->valMin &&
            d->valMax == other.d->valMax &&
            d->cond == other.d->cond &&
            d->action == other.d->action &&
            d->restriction == other.d->restriction &&
            d->timeMin == other.d->timeMin &&
            d->timeMax == other.d->timeMax &&
            d->dateMin == other.d->dateMin &&
            d->dateMax == other.d->dateMax &&
            d->displayMessage == other.d->displayMessage &&
            d->allowEmptyCell == other.d->allowEmptyCell &&
            d->displayValidationInformation == other.d->displayValidationInformation &&
            d->listValidity == other.d->listValidity) {
        return true;
    }
    return false;
}

// static
QHash<QString, KoXmlElement> Validity::preloadValidities(const KoXmlElement& body)
{
    QHash<QString, KoXmlElement> validities;
    KoXmlNode validation = KoXml::namedItemNS(body, KoXmlNS::table, "content-validations");
    kDebug() << "validation.isNull?" << validation.isNull();
    if (!validation.isNull()) {
        KoXmlElement element;
        forEachElement(element, validation) {
            if (element.tagName() ==  "content-validation" && element.namespaceURI() == KoXmlNS::table) {
                const QString name = element.attributeNS(KoXmlNS::table, "name", QString());
                validities.insert(name, element);
                kDebug() << " validation found:" << name;
            } else {
                kDebug() << " Tag not recognized:" << element.tagName();
            }
        }
    }
    return validities;
}
