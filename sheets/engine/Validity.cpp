/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "Validity.h"

// Sheets
#include "CalculationSettings.h"
#include "CellBase.h"
#include "MapBase.h"
#include "SheetBase.h"
#include "ValueCalc.h"

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
    Validity::Type cond;
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
    d->cond = Validity::None;
    d->action = Stop;
    d->restriction = NoRestriction;
    d->displayMessage = true;
    d->allowEmptyCell = false;
    d->displayValidationInformation = false;
}

Validity::Validity(const Validity &other)

    = default;

Validity::~Validity() = default;

bool Validity::isEmpty() const
{
    return d->restriction == NoRestriction;
}

Validity::Action Validity::action() const
{
    return d->action;
}

bool Validity::allowEmptyCell() const
{
    return d->allowEmptyCell;
}

Validity::Type Validity::condition() const
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

const QString &Validity::messageInfo() const
{
    return d->messageInfo;
}

const Value &Validity::maximumValue() const
{
    return d->maxValue;
}

const QString &Validity::message() const
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

const QString &Validity::title() const
{
    return d->title;
}

const QString &Validity::titleInfo() const
{
    return d->titleInfo;
}

const QStringList &Validity::validityList() const
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

void Validity::setCondition(Validity::Type condition)
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

void Validity::setMessage(const QString &msg)
{
    d->message = msg;
}

void Validity::setMessageInfo(const QString &info)
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

void Validity::setTitle(const QString &t)
{
    d->title = t;
}

void Validity::setTitleInfo(const QString &info)
{
    d->titleInfo = info;
}

void Validity::setValidityList(const QStringList &list)
{
    d->listValidity = list;
}

bool Validity::testValidity(const CellBase *cell) const
{
    if (d->restriction == NoRestriction)
        return true;

    bool valid = false;

    // fixme
    if (d->allowEmptyCell && cell->userInput().isEmpty())
        return true;

    ValueCalc *const calc = cell->sheet()->map()->calc();
    const Qt::CaseSensitivity cs = calc->settings()->caseSensitiveComparisons();
    Value val = cell->value();

    if ((val.isNumber() && (d->restriction == Number || (d->restriction == Integer && numToDouble(val.asFloat()) == ceil(numToDouble(val.asFloat())))))
        || (d->restriction == Time && (val.format() == Value::fmt_Time)) || (d->restriction == Date && (val.format() == Value::fmt_Date))) {
        switch (d->cond) {
        case Validity::Equal:
            valid = calc->naturalEqual(val, d->minValue, cs);
            break;
        case Validity::DifferentTo:
            valid = !calc->naturalEqual(val, d->minValue, cs);
            break;
        case Validity::Superior:
            valid = calc->naturalGreater(val, d->minValue, cs);
            break;
        case Validity::Inferior:
            valid = calc->naturalLower(val, d->minValue, cs);
            break;
        case Validity::SuperiorEqual:
            valid = calc->naturalGequal(val, d->minValue, cs);
            break;
        case Validity::InferiorEqual:
            valid = calc->naturalLequal(val, d->minValue, cs);
            break;
        case Validity::Between:
            valid = (calc->naturalGequal(val, d->minValue, cs) && calc->naturalLequal(val, d->maxValue, cs));
            break;
        case Validity::Different:
            valid = (calc->naturalLower(val, d->minValue, cs) || calc->naturalGreater(val, d->maxValue, cs));
            break;
        default:
            break;
        }
    } else if (d->restriction == Text) {
        valid = val.isString();
    } else if (d->restriction == List) {
        // test int value
        if (val.isString() && d->listValidity.contains(val.asString()))
            valid = true;
    } else if (d->restriction == TextLength) {
        if (val.isString()) {
            int len = val.asString().length();
            const int min = d->minValue.asInteger();
            const int max = d->maxValue.asInteger();
            switch (d->cond) {
            case Validity::Equal:
                if (len == min)
                    valid = true;
                break;
            case Validity::DifferentTo:
                if (len != min)
                    valid = true;
                break;
            case Validity::Superior:
                if (len > min)
                    valid = true;
                break;
            case Validity::Inferior:
                if (len < min)
                    valid = true;
                break;
            case Validity::SuperiorEqual:
                if (len >= min)
                    valid = true;
                break;
            case Validity::InferiorEqual:
                if (len <= min)
                    valid = true;
                break;
            case Validity::Between:
                if (len >= min && len <= max)
                    valid = true;
                break;
            case Validity::Different:
                if (len < min || len > max)
                    valid = true;
                break;
            default:
                break;
            }
        }
    }

    if (valid)
        return true;

    if (d->displayMessage) {
        valid = cell->sheet()->onValidationFailed(d->action, cell, d->message, d->title);
    }

    return false;
}

void Validity::operator=(const Validity &other)
{
    d = other.d;
}

bool Validity::operator==(const Validity &other) const
{
    if (d->message == other.d->message && d->title == other.d->title && d->titleInfo == other.d->titleInfo && d->messageInfo == other.d->messageInfo
        && d->minValue == other.d->minValue && d->maxValue == other.d->maxValue && d->cond == other.d->cond && d->action == other.d->action
        && d->restriction == other.d->restriction && d->displayMessage == other.d->displayMessage && d->allowEmptyCell == other.d->allowEmptyCell
        && d->displayValidationInformation == other.d->displayValidationInformation && d->listValidity == other.d->listValidity) {
        return true;
    }
    return false;
}
