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

// KF5
#include <kmessagebox.h>

// Calligra
#include <KoXmlNS.h>

// Sheets
#include "CalculationSettings.h"
#include "Cell.h"
#include "Map.h"
#include "Sheet.h"
#include "Value.h"
#include "ValueCalc.h"
#include "ValueConverter.h"
#include "ValueParser.h"

using namespace Calligra::Sheets;

class Q_DECL_HIDDEN Validity::Private : public QSharedData
{
public:
    QString message;
    QString title;
    QString titleInfo;
    QString messageInfo;
    Value minValue;
    Value maxValue;
    Conditional::Type cond;
    Action action;
    Restriction restriction;
    bool displayMessage;
    bool allowEmptyCell;
    bool displayValidationInformation;
    QStringList listValidity;
};

Validity::Validity()
        : d(new Private)
{
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
    ValueParser *const parser = cell->sheet()->map()->parser();
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
            d->minValue = parser->tryParseNumber(param.attribute("valmin"), &ok);
            if (!ok)
                return false;
        }
        if (param.hasAttribute("valmax")) {
            d->maxValue = parser->tryParseNumber(param.attribute("valmax"), &ok);
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
        d->minValue = parser->tryParseTime(timeMinElement.text());
    }
    KoXmlElement timeMaxElement = validityElement.namedItem("timemax").toElement();
    if (!timeMaxElement.isNull()) {
        d->maxValue = parser->tryParseTime(timeMaxElement.text());
    }
    KoXmlElement dateMinElement = validityElement.namedItem("datemin").toElement();
    if (!dateMinElement.isNull()) {
        d->minValue = parser->tryParseTime(dateMinElement.text());
    }
    KoXmlElement dateMaxElement = validityElement.namedItem("datemax").toElement();
    if (!dateMaxElement.isNull()) {
        d->maxValue = parser->tryParseTime(dateMaxElement.text());
    }
    return true;
}

QDomElement Validity::saveXML(QDomDocument& doc, const ValueConverter *converter) const
{
    QDomElement validityElement = doc.createElement("validity");

    QDomElement param = doc.createElement("param");
    param.setAttribute("cond", QString::number((int)d->cond));
    param.setAttribute("action", QString::number((int)d->action));
    param.setAttribute("allow", QString::number((int)d->restriction));
    param.setAttribute("valmin", converter->asString(d->minValue).asString());
    param.setAttribute("valmax", converter->asString(d->maxValue).asString());
    param.setAttribute("displaymessage", QString::number(d->displayMessage));
    param.setAttribute("displayvalidationinformation", QString::number(d->displayValidationInformation));
    param.setAttribute("allowemptycell", QString::number(d->allowEmptyCell));
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
    if (d->restriction == Time) {
        QDomElement timeMinElement = doc.createElement("timemin");
        tmp = converter->asString(d->minValue).asString();
        timeMinElement.appendChild(doc.createTextNode(tmp));
        validityElement.appendChild(timeMinElement);

        if (d->cond == Conditional::Between || d->cond == Conditional::Different) {
            QDomElement timeMaxElement = doc.createElement("timemax");
            tmp = converter->asString(d->maxValue).asString();
            timeMaxElement.appendChild(doc.createTextNode(tmp));
            validityElement.appendChild(timeMaxElement);
        }
    }

    if (d->restriction == Date) {
        QDomElement dateMinElement = doc.createElement("datemin");
        const QDate minDate = d->minValue.asDate(converter->settings());
        QString tmp("%1/%2/%3");
        tmp = tmp.arg(minDate.year()).arg(minDate.month()).arg(minDate.day());
        dateMinElement.appendChild(doc.createTextNode(tmp));
        validityElement.appendChild(dateMinElement);

        if (d->cond == Conditional::Between || d->cond == Conditional::Different) {
            QDomElement dateMaxElement = doc.createElement("datemax");
            const QDate maxDate = d->maxValue.asDate(converter->settings());
            QString tmp("%1/%2/%3");
            tmp = tmp.arg(maxDate.year()).arg(maxDate.month()).arg(maxDate.day());
            dateMaxElement.appendChild(doc.createTextNode(tmp));
            validityElement.appendChild(dateMaxElement);
        }
    }
    return validityElement;
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

const Value &Validity::maximumValue() const
{
    return d->maxValue;
}

const QString& Validity::message() const
{
    return d->message;
}

const Value &Validity::minimumValue() const
{
    return d->minValue;
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

void Validity::setMaximumValue(const Value &value)
{
    d->maxValue = value;
}

void Validity::setMessage(const QString& msg)
{
    d->message = msg;
}

void Validity::setMessageInfo(const QString& info)
{
    d->messageInfo = info;
}

void Validity::setMinimumValue(const Value &value)
{
    d->minValue = value;
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
    bool valid = false;
    if (d->restriction != None) {
        //fixme
        if (d->allowEmptyCell && cell->userInput().isEmpty())
            return true;

        ValueCalc *const calc = cell->sheet()->map()->calc();
        const Qt::CaseSensitivity cs = calc->settings()->caseSensitiveComparisons();

        if ((cell->value().isNumber() &&
                (d->restriction == Number ||
                 (d->restriction == Integer &&
                  numToDouble(cell->value().asFloat()) == ceil(numToDouble(cell->value().asFloat())))))
            || (d->restriction == Time && cell->isTime())
            || (d->restriction == Date && cell->isDate())) {
            switch (d->cond) {
            case Conditional::Equal:
                valid = cell->value().equal(d->minValue, cs);
                break;
            case Conditional::DifferentTo:
                valid = !cell->value().equal(d->minValue, cs);
                break;
            case Conditional::Superior:
                valid = cell->value().greater(d->minValue, cs);
                break;
            case Conditional::Inferior:
                valid = cell->value().less(d->minValue, cs);
                break;
            case Conditional::SuperiorEqual:
                valid = (cell->value().compare(d->minValue, cs)) >= 0;
                break;
            case Conditional::InferiorEqual:
                valid = (cell->value().compare(d->minValue, cs)) <= 0;
                break;
            case Conditional::Between:
                valid = (cell->value().compare(d->minValue, cs) >= 0 &&
                         cell->value().compare(d->maxValue, cs) <= 0);
                break;
            case Conditional::Different:
                valid = (cell->value().compare(d->minValue, cs) < 0 ||
                         cell->value().compare(d->maxValue, cs) > 0);
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
                const int min = d->minValue.asInteger();
                const int max = d->maxValue.asInteger();
                switch (d->cond) {
                case Conditional::Equal:
                    if (len == min)
                        valid = true;
                    break;
                case Conditional::DifferentTo:
                    if (len != min)
                        valid = true;
                    break;
                case Conditional::Superior:
                    if (len > min)
                        valid = true;
                    break;
                case Conditional::Inferior:
                    if (len < min)
                        valid = true;
                    break;
                case Conditional::SuperiorEqual:
                    if (len >= min)
                        valid = true;
                    break;
                case Conditional::InferiorEqual:
                    if (len <= min)
                        valid = true;
                    break;
                case Conditional::Between:
                    if (len >= min && len <= max)
                        valid = true;
                    break;
                case Conditional::Different:
                    if (len < min || len > max)
                        valid = true;
                    break;
                default :
                    break;
                }
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
                if (KMessageBox::warningYesNo((QWidget*)0, d->message, d->title) == KMessageBox::Yes) {
                    valid = true;
                }
                break;
            case Information:
                KMessageBox::information((QWidget*)0, d->message, d->title);
                valid = true;
                break;
            }
        }

        cell->sheet()->showStatusMessage(i18n("Validation for cell %1 failed", cell->fullName()));
    }
    return valid;
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
            d->minValue == other.d->minValue &&
            d->maxValue == other.d->maxValue &&
            d->cond == other.d->cond &&
            d->action == other.d->action &&
            d->restriction == other.d->restriction &&
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
    debugSheets << "validation.isNull?" << validation.isNull();
    if (!validation.isNull()) {
        KoXmlElement element;
        forEachElement(element, validation) {
            if (element.tagName() ==  "content-validation" && element.namespaceURI() == KoXmlNS::table) {
                const QString name = element.attributeNS(KoXmlNS::table, "name", QString());
                validities.insert(name, element);
                debugSheets << " validation found:" << name;
            } else {
                debugSheets << " Tag not recognized:" << element.tagName();
            }
        }
    }
    return validities;
}
