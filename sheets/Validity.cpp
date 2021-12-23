/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "Validity.h"

// KF5
#include <kmessagebox.h>

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


