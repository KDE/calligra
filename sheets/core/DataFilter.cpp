/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DataFilter.h"

#include "engine/CellBaseStorage.h"
#include "engine/Region.h"
#include "engine/SheetBase.h"
#include "engine/Value.h"
#include "engine/ValueConverter.h"

#include "Database.h"

using namespace Calligra::Sheets;

bool AbstractCondition::listsAreEqual(const QList<AbstractCondition *> &a, const QList<AbstractCondition *> &b)
{
    if (a.size() != b.size()) return false;
    for (int i = 0; i < a.size(); i++) {
        if (!Filter::conditionsEquals(a[i], b[i]))
            return false;
    }
    return true;
}
Filter::And::And(const And& other)
        : AbstractCondition()
{
    for (int i = 0; i < other.list.count(); ++i) {
        if (!other.list[i])
            continue;
        else if (other.list[i]->type() == AbstractCondition::And)
            list.append(new Filter::And(*static_cast<Filter::And*>(other.list[i])));
        else if (other.list[i]->type() == AbstractCondition::Or)
            list.append(new Filter::Or(*static_cast<Filter::Or*>(other.list[i])));
        else
            list.append(new Filter::Condition(*static_cast<Filter::Condition*>(other.list[i])));
    }
}

Filter::And::~And() {
    qDeleteAll(list);
}

AbstractCondition::Type Filter::And::type() const {
    return AbstractCondition::And;
}

bool Filter::And::evaluate(const Database& database, int index) const {
    for (int i = 0; i < list.count(); ++i) {
        // lazy evaluation, stop on first false
        if (!list[i]->evaluate(database, index))
            return false;
    }
    return true;
}

bool Filter::And::isEmpty() const {
    return list.isEmpty();
}

QMap<QString, AbstractCondition::Comparison> Filter::And::conditions(int fieldNumber) const {
    QMap<QString, AbstractCondition::Comparison> result;
    for (int i = 0; i < list.count(); ++i)
        result.insert(list[i]->conditions(fieldNumber));
    return result;
}

void Filter::And::removeConditions(int fieldNumber) {
    QList<AbstractCondition*> newList;
    for (int i = 0; i < list.count(); ++i) {
        list[i]->removeConditions(fieldNumber);
        if (!list[i]->isEmpty())
            newList.append(list[i]);
        else
            delete list[i];
    }
    list = newList;
}

bool Filter::And::operator!=(const And& other) const {
    return !listsAreEqual(list, other.list);
}

QString Filter::And::dump() const {
    QString result = "\t";
    for (int i = 0; i < list.count(); ++i) {
        if (i)
            result += "AND\t";
        result += list[i]->dump();
    }
    return result;
}

Filter::Or::Or(const Or& other)
        : AbstractCondition()
{
    for (int i = 0; i < other.list.count(); ++i) {
        if (!other.list[i])
            continue;
        else if (other.list[i]->type() == AbstractCondition::And)
            list.append(new Filter::And(*static_cast<Filter::And*>(other.list[i])));
        else if (other.list[i]->type() == AbstractCondition::Or)
            list.append(new Filter::Or(*static_cast<Filter::Or*>(other.list[i])));
        else
            list.append(new Filter::Condition(*static_cast<Filter::Condition*>(other.list[i])));
    }
}

Filter::Or::~Or() {
    qDeleteAll(list);
}

AbstractCondition::Type Filter::Or::type() const {
    return AbstractCondition::Or;
}

bool Filter::Or::evaluate(const Database& database, int index) const {
    for (int i = 0; i < list.count(); ++i) {
        // lazy evaluation, stop on first true
        if (list[i]->evaluate(database, index))
            return true;
    }
    return false;
}

bool Filter::Or::isEmpty() const {
    return list.isEmpty();
}

QMap<QString, AbstractCondition::Comparison> Filter::Or::conditions(int fieldNumber) const {
    QMap<QString, AbstractCondition::Comparison> result;
    for (int i = 0; i < list.count(); ++i)
        result.insert(list[i]->conditions(fieldNumber));
    return result;
}

void Filter::Or::removeConditions(int fieldNumber) {
    QList<AbstractCondition*> newList;
    for (int i = 0; i < list.count(); ++i) {
        list[i]->removeConditions(fieldNumber);
        if (!list[i]->isEmpty())
            newList.append(list[i]);
        else
            delete list[i];
    }
    list = newList;
}

bool Filter::Or::operator!=(const Or& other) const {
    return !listsAreEqual(list, other.list);
}

QString Filter::Or::dump() const {
    QString result = "\t";
    for (int i = 0; i < list.count(); ++i) {
        if (i)
            result += "OR\t";
        result += list[i]->dump();
    }
    return result;
}

Filter::Condition::Condition()
        : fieldNumber(-1)
        , operation(Match)
        , caseSensitivity(Qt::CaseInsensitive)
        , dataType(Text) {
}

Filter::Condition::Condition(int _fieldNumber, Comparison _comparison, const QString& _value,
          Qt::CaseSensitivity _caseSensitivity, Mode _mode)
        : fieldNumber(_fieldNumber)
        , value(_value)
        , operation(_comparison)
        , caseSensitivity(_caseSensitivity)
        , dataType(_mode) {
}

Filter::Condition::Condition(const Condition& other)
        : AbstractCondition()
        , fieldNumber(other.fieldNumber)
        , value(other.value)
        , operation(other.operation)
        , caseSensitivity(other.caseSensitivity)
        , dataType(other.dataType) {
}

AbstractCondition::Type Filter::Condition::type() const {
    return AbstractCondition::Condition;
}

bool Filter::Condition::evaluate(const Database& database, int index) const {
    const SheetBase *sheet = database.range().lastSheet();
    const QRect range = database.range().lastRange();
    const int start = database.orientation() == Qt::Vertical ? range.left() : range.top();
//         debugSheets <<"index:" << index <<" start:" << start <<" fieldNumber:" << fieldNumber;
    const Value value = database.orientation() == Qt::Vertical
                        ? sheet->cellStorage()->value(start + fieldNumber, index)
                        : sheet->cellStorage()->value(index, start + fieldNumber);
    const QString testString = sheet->map()->converter()->asString(value).asString();
    switch (operation) {
    case Match: {
        const bool result = QString::compare(this->value, testString, caseSensitivity) == 0;
//                 debugSheets <<"Match" << this->value <<"?" << testString <<"" << result;
        if (result)
            return true;
        break;
    }
    case NotMatch: {
        const bool result = QString::compare(this->value, testString, caseSensitivity) != 0;
//                 debugSheets <<"Not Match" << this->value <<"?" << testString <<"" << result;
        if (result)
            return true;
        break;
    }
    default:
        break;
    }
    return false;
}

bool Filter::Condition::isEmpty() const {
    return fieldNumber == -1;
}

QMap<QString, AbstractCondition::Comparison> Filter::Condition::conditions(int fieldNumber) const {
    QMap<QString, AbstractCondition::Comparison> result;
    if (this->fieldNumber == fieldNumber)
        result.insert(value, operation);
    return result;
}

void Filter::Condition::removeConditions(int fieldNumber) {
    if (this->fieldNumber == fieldNumber) {
//             debugSheets <<"removing condition for fieldNumber" << fieldNumber;
        this->fieldNumber = -1;
    }
}

bool Filter::Condition::operator==(const Condition& other) const {
    if (fieldNumber != other.fieldNumber)
        return false;
    if (value != other.value)
        return false;
    if (operation != other.operation)
        return false;
    if (caseSensitivity != other.caseSensitivity)
        return false;
    if (dataType != other.dataType)
        return false;
    return true;
}

bool Filter::Condition::operator!=(const Condition& other) const {
    return !operator==(other);
}

QString Filter::Condition::dump() const {
    QString result = QString("fieldNumber: %1 ").arg(fieldNumber);
    switch (operation) {
    case Match:     result += "Match"; break;
    case NotMatch:  result += "Not Match"; break;
    default:        break;
    }
    return result + " value: " + value + '\n';
}



class Q_DECL_HIDDEN Filter::Private
{
public:
    Private()
            : condition(0)
            , conditionSourceIsRange(false)
            , displayDuplicates(true) {
    }

    AbstractCondition* condition;
    Region targetRangeAddress;
    bool conditionSourceIsRange;
    Region conditionSourceRangeAddress;
    bool displayDuplicates;
};

Filter::Filter()
        : d(new Private)
{
}

Filter::Filter(const Filter& other)
        : d(new Private)
{
    copyFrom (other);
}

void Filter::operator=(const Filter& other)
{
    delete d->condition;

    copyFrom (other);
}

// shared code between the copy constructor and the assignment operator
void Filter::copyFrom (const Filter &other)
{
    if (!other.d->condition)
        d->condition = 0;
    else if (other.d->condition->type() == AbstractCondition::And)
        d->condition = new And(*static_cast<And*>(other.d->condition));
    else if (other.d->condition->type() == AbstractCondition::Or)
        d->condition = new Or(*static_cast<Or*>(other.d->condition));
    else
        d->condition = new Condition(*static_cast<Condition*>(other.d->condition));

    d->targetRangeAddress = other.d->targetRangeAddress;
    d->conditionSourceIsRange = other.d->conditionSourceIsRange;
    d->conditionSourceRangeAddress = other.d->conditionSourceRangeAddress;
    d->displayDuplicates = other.d->displayDuplicates;
}

Filter::~Filter()
{
    delete d->condition;
    delete d;
}

void Filter::addCondition(Composition composition,
                          int fieldNumber, AbstractCondition::Comparison comparison, const QString& value,
                          Qt::CaseSensitivity caseSensitivity, Mode mode)
{
    Condition* condition = new Condition(fieldNumber, comparison, value, caseSensitivity, mode);
    if (!d->condition) {
        d->condition = condition;
    } else if (composition == AndComposition) {
        if (d->condition->type() == AbstractCondition::And) {
            static_cast<And*>(d->condition)->list.append(condition);
        } else {
            And* andComposition = new And();
            andComposition->list.append(d->condition);
            andComposition->list.append(condition);
            d->condition = andComposition;
        }
    } else { // composition == OrComposition
        if (d->condition->type() == AbstractCondition::Or) {
            static_cast<Or*>(d->condition)->list.append(condition);
        } else {
            Or* orComposition = new Or();
            orComposition->list.append(d->condition);
            orComposition->list.append(condition);
            d->condition = orComposition;
        }
    }
}

QList<AbstractCondition*> Filter::copyList(const QList<AbstractCondition*>& list)
{
    QList<AbstractCondition*> out;
    for (AbstractCondition* c : list) {
        if (!c) {
            continue;
        } else if (c->type() == AbstractCondition::And) {
            out.append(new Filter::And(*static_cast<Filter::And*>(c)));
        } else if (c->type() == AbstractCondition::Or) {
            out.append(new Filter::Or(*static_cast<Filter::Or*>(c)));
        } else {
            out.append(new Filter::Condition(*static_cast<Filter::Condition*>(c)));
        }
    }
    return out;
}

void Filter::addSubFilter(Composition composition, const Filter& filter)
{
    if (!d->condition) {
        if (!filter.d->condition)
            d->condition = 0;
        else if (filter.d->condition->type() == AbstractCondition::And)
            d->condition = new And(*static_cast<And*>(filter.d->condition));
        else if (filter.d->condition->type() == AbstractCondition::Or)
            d->condition = new Or(*static_cast<Or*>(filter.d->condition));
        else // if (filter.d->condition->type() == AbstractCondition::Condition)
            d->condition = new Condition(*static_cast<Condition*>(filter.d->condition));
    } else if (composition == AndComposition) {
        if (filter.d->condition && d->condition->type() == AbstractCondition::And) {
            if (filter.d->condition->type() == AbstractCondition::And)
                static_cast<And*>(d->condition)->list += copyList(static_cast<And*>(filter.d->condition)->list);
            else if (filter.d->condition->type() == AbstractCondition::Or)
                static_cast<And*>(d->condition)->list.append(new Or(*static_cast<Or*>(filter.d->condition)));
            else // if (filter.d->condition->type() == AbstractCondition::Condition)
                static_cast<And*>(d->condition)->list.append(new Condition(*static_cast<Condition*>(filter.d->condition)));
        } else if (filter.d->condition) {
            And* andComposition = new And();
            andComposition->list.append(d->condition);
            if (filter.d->condition->type() == AbstractCondition::And)
                andComposition->list += copyList(static_cast<And*>(filter.d->condition)->list);
            else if (filter.d->condition->type() == AbstractCondition::Or)
                andComposition->list.append(new Or(*static_cast<Or*>(filter.d->condition)));
            else // if (filter.d->condition->type() == AbstractCondition::Condition)
                andComposition->list.append(new Condition(*static_cast<Condition*>(filter.d->condition)));
            d->condition = andComposition;
        }
    } else { // composition == OrComposition
        if (filter.d->condition && d->condition->type() == AbstractCondition::Or) {
            if (filter.d->condition->type() == AbstractCondition::And)
                static_cast<Or*>(d->condition)->list.append(new And(*static_cast<And*>(filter.d->condition)));
            else if (filter.d->condition->type() == AbstractCondition::Or)
                static_cast<Or*>(d->condition)->list += copyList(static_cast<Or*>(filter.d->condition)->list);
            else // if (filter.d->condition->type() == AbstractCondition::Condition)
                static_cast<Or*>(d->condition)->list.append(new Condition(*static_cast<Condition*>(filter.d->condition)));
        } else if (filter.d->condition) {
            Or* orComposition = new Or();
            orComposition->list.append(d->condition);
            if (filter.d->condition->type() == AbstractCondition::And)
                orComposition->list.append(new And(*static_cast<And*>(filter.d->condition)));
            else if (filter.d->condition->type() == AbstractCondition::Or)
                orComposition->list += copyList(static_cast<Or*>(filter.d->condition)->list);
            else // if (filter.d->condition->type() == AbstractCondition::Condition)
                orComposition->list.append(new Condition(*static_cast<Condition*>(filter.d->condition)));
            d->condition = orComposition;
        }
    }
}

QMap<QString, AbstractCondition::Comparison> Filter::conditions(int fieldNumber) const
{
    return d->condition ? d->condition->conditions(fieldNumber) : QMap<QString, AbstractCondition::Comparison>();
}

void Filter::removeConditions(int fieldNumber)
{
    if (fieldNumber == -1) {
//         debugSheets <<"removing all conditions";
        delete d->condition;
        d->condition = 0;
        return;
    }
    if (!d->condition)
        return;
    d->condition->removeConditions(fieldNumber);
    if (d->condition->isEmpty()) {
        delete d->condition;
        d->condition = 0;
    }
}

bool Filter::isEmpty() const
{
    return d->condition ? d->condition->isEmpty() : true;
}

Region Filter::targetRangeAddress() const
{
    return d->targetRangeAddress;
}

void Filter::setTargetRangeAddress(const Region &address)
{
    d->targetRangeAddress = address;
}

Region Filter::sourceRangeAddress() const
{
    return d->conditionSourceRangeAddress;
}

void Filter::setSourceRangeAddress(const Region &address)
{
    d->conditionSourceRangeAddress = address;
}

bool Filter::displayDuplicates() const
{
    return d->displayDuplicates;
}

void Filter::setDisplayDuplicates(bool val)
{
    d->displayDuplicates = val;
}

bool Filter::conditionSourceIsRange() const
{
    return d->conditionSourceIsRange;
}

void Filter::setConditionSourceIsRange(bool val)
{
    d->conditionSourceIsRange = val;
}

AbstractCondition *Filter::rootCondition() const {
    return d->condition;
}

void Filter::setRootCondition(AbstractCondition *cond) {
    if (!cond) return;
    if (d->condition) delete d->condition;
    d->condition = cond;
}

bool Filter::evaluate(const Database& database, int index) const
{
    return d->condition ? d->condition->evaluate(database, index) : true;
}

bool Filter::conditionsEquals(AbstractCondition* a, AbstractCondition* b)
{
    if (!a || !b)
        return a == b;
    if (a->type() != b->type())
        return false;
    if (a->type() == AbstractCondition::And && *static_cast<And*>(a) != *static_cast<And*>(b))
        return false;
    if (a->type() == AbstractCondition::Or && *static_cast<Or*>(a) != *static_cast<Or*>(b))
        return false;
    if (a->type() == AbstractCondition::Condition && *static_cast<Condition*>(a) != *static_cast<Condition*>(b))
        return false;
    return true;
}

bool Filter::operator==(const Filter& other) const
{
    if (d->targetRangeAddress != other.d->targetRangeAddress)
        return false;
    if (d->conditionSourceIsRange != other.d->conditionSourceIsRange)
        return false;
    if (d->conditionSourceRangeAddress != other.d->conditionSourceRangeAddress)
        return false;
    if (d->displayDuplicates != other.d->displayDuplicates)
        return false;
    return conditionsEquals(d->condition, other.d->condition);
}

void Filter::dump() const
{
    if (d->condition)
        debugSheets << "Condition:" + d->condition->dump();
    else
        debugSheets << "Condition: 0";
}



