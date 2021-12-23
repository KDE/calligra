/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2021 Tomas Mecir <mecirt@gmail.com>
   SPDX-FileCopyrightText: 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/


#include "SheetsKsp.h"
#include "SheetsKspPrivate.h"

#include "engine/Validity.h"

#include "engine/CellBase.h"
#include "engine/MapBase.h"
#include "engine/SheetBase.h"
#include "engine/ValueConverter.h"
#include "engine/ValueParser.h"


namespace Calligra {
namespace Sheets {

bool Ksp::loadValidity(Validity *v, Cell* const cell, const KoXmlElement& validityElement) {
    ValueParser *const parser = cell->sheet()->map()->parser();
    bool ok = false;
    KoXmlElement param = validityElement.namedItem("param").toElement();
    if (!param.isNull()) {
        if (param.hasAttribute("cond")) {
            v->setCondition((Conditional::Type) param.attribute("cond").toInt(&ok));
            if (!ok)
                return false;
        }
        if (param.hasAttribute("action")) {
            v->setAction((Validity::Action) param.attribute("action").toInt(&ok));
            if (!ok)
                return false;
        }
        if (param.hasAttribute("allow")) {
            v->setRestriction((Validity::Restriction) param.attribute("allow").toInt(&ok));
            if (!ok)
                return false;
        }
        if (param.hasAttribute("valmin")) {
            v->setMinimumValue (parser->tryParseNumber(param.attribute("valmin"), &ok));
            if (!ok)
                return false;
        }
        if (param.hasAttribute("valmax")) {
            v->setMaximumValue (parser->tryParseNumber(param.attribute("valmax"), &ok));
            if (!ok)
                return false;
        }
        if (param.hasAttribute("displaymessage")) {
            v->setDisplayMessage ((bool)param.attribute("displaymessage").toInt());
        }
        if (param.hasAttribute("displayvalidationinformation")) {
            v->setDisplayValidationInformation ((bool)param.attribute("displayvalidationinformation").toInt());
        }
        if (param.hasAttribute("allowemptycell")) {
            v->setAllowEmptyCell ((bool)param.attribute("allowemptycell").toInt());
        }
        if (param.hasAttribute("listvalidity")) {
            v->setValidityList (param.attribute("listvalidity").split(';', QString::SkipEmptyParts));
        }
    }
    KoXmlElement inputTitle = validityElement.namedItem("inputtitle").toElement();
    if (!inputTitle.isNull()) {
        v->setTitleInfo (inputTitle.text());
    }
    KoXmlElement inputMessage = validityElement.namedItem("inputmessage").toElement();
    if (!inputMessage.isNull()) {
        v->setMessageInfo (inputMessage.text());
    }

    KoXmlElement titleElement = validityElement.namedItem("title").toElement();
    if (!titleElement.isNull()) {
        v->setTitle (titleElement.text());
    }
    KoXmlElement messageElement = validityElement.namedItem("message").toElement();
    if (!messageElement.isNull()) {
        v->setMessage (messageElement.text());
    }
    KoXmlElement timeMinElement = validityElement.namedItem("timemin").toElement();
    if (!timeMinElement.isNull()) {
        v->setMinimumValue (parser->tryParseTime(timeMinElement.text()));
    }
    KoXmlElement timeMaxElement = validityElement.namedItem("timemax").toElement();
    if (!timeMaxElement.isNull()) {
        v->setMaximumValue (parser->tryParseTime(timeMaxElement.text()));
    }
    KoXmlElement dateMinElement = validityElement.namedItem("datemin").toElement();
    if (!dateMinElement.isNull()) {
        v->setMinimumValue (parser->tryParseTime(dateMinElement.text()));
    }
    KoXmlElement dateMaxElement = validityElement.namedItem("datemax").toElement();
    if (!dateMaxElement.isNull()) {
        v->setMaximumValue (parser->tryParseTime(dateMaxElement.text()));
    }
    return true;
}

QDomElement Ksp::saveValidity(QDomDocument& doc, Validity *v, const ValueConverter *converter) {
    QDomElement validityElement = doc.createElement("validity");

    QDomElement param = doc.createElement("param");
    param.setAttribute("cond", QString::number((int)v->condition()));
    param.setAttribute("action", QString::number((int)v->action()));
    param.setAttribute("allow", QString::number((int)v->restriction()));
    param.setAttribute("valmin", converter->asString(v->minimumValue()).asString());
    param.setAttribute("valmax", converter->asString(v->maximumValue()).asString());
    param.setAttribute("displaymessage", QString::number(v->displayMessage()));
    param.setAttribute("displayvalidationinformation", QString::number(v->displayValidationInformation()));
    param.setAttribute("allowemptycell", QString::number(v->allowEmptyCell()));
    if (!v->validityList().isEmpty())
        param.setAttribute("listvalidity", v->validityList().join(";"));
    validityElement.appendChild(param);
    QDomElement titleElement = doc.createElement("title");
    titleElement.appendChild(doc.createTextNode(v->title()));
    validityElement.appendChild(titleElement);
    QDomElement messageElement = doc.createElement("message");
    messageElement.appendChild(doc.createCDATASection(v->message()));
    validityElement.appendChild(messageElement);

    QDomElement inputTitle = doc.createElement("inputtitle");
    inputTitle.appendChild(doc.createTextNode(v->titleInfo()));
    validityElement.appendChild(inputTitle);

    QDomElement inputMessage = doc.createElement("inputmessage");
    inputMessage.appendChild(doc.createTextNode(v->messageInfo()));
    validityElement.appendChild(inputMessage);


    Validity::Restriction rr = v->restriction();
    QString tmp;
    if (rr == Time) {
        QDomElement timeMinElement = doc.createElement("timemin");
        tmp = converter->asString(v->minimumValue()).asString();
        timeMinElement.appendChild(doc.createTextNode(tmp));
        validityElement.appendChild(timeMinElement);

        if (v->condition() == Conditional::Between || v->condition() == Conditional::Different) {
            QDomElement timeMaxElement = doc.createElement("timemax");
            tmp = converter->asString(v->maximumValue()).asString();
            timeMaxElement.appendChild(doc.createTextNode(tmp));
            validityElement.appendChild(timeMaxElement);
        }
    }

    if (rr == Date) {
        QDomElement dateMinElement = doc.createElement("datemin");
        const QDate minDate = v->minimumValue().asDate(converter->settings());
        QString tmp("%1/%2/%3");
        tmp = tmp.arg(minDate.year()).arg(minDate.month()).arg(minDate.day());
        dateMinElement.appendChild(doc.createTextNode(tmp));
        validityElement.appendChild(dateMinElement);

        if (v->condition() == Conditional::Between || v->condition() == Conditional::Different) {
            QDomElement dateMaxElement = doc.createElement("datemax");
            const QDate maxDate = v->maximumValue().asDate(converter->settings());
            QString tmp("%1/%2/%3");
            tmp = tmp.arg(maxDate.year()).arg(maxDate.month()).arg(maxDate.day());
            dateMaxElement.appendChild(doc.createTextNode(tmp));
            validityElement.appendChild(dateMaxElement);
        }
    }
    return validityElement;
}


}  // Sheets
}  // Calligra
