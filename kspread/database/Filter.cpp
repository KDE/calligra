/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#include "Filter.h"

#include <QList>
#include <QRect>
#include <QString>

#include "CellStorage.h"
#include "Database.h"
#include "Doc.h"
#include "RowColumnFormat.h"
#include "Region.h"
#include "Sheet.h"
#include "Value.h"
#include "ValueConverter.h"

using namespace KSpread;

class AbstractCondition
{
public:
    virtual ~AbstractCondition() {}
    enum Type { And, Or, Condition };
    virtual Type type() const = 0;
    virtual void loadOdf() = 0;
    virtual void saveOdf() = 0;
    virtual bool evaluate(const Database* database, int index) const = 0;
    virtual bool isEmpty() const = 0;
    virtual void removeConditions(int fieldNumber) = 0;
    virtual void dump() const = 0;
};

/**
 * OpenDocument, 8.7.2 Filter And
 */
class Filter::And : public AbstractCondition
{
public:
    And() {}
    And(const And& other);
    virtual ~And() { qDeleteAll(list); }
    virtual Type type() const { return AbstractCondition::And; }
    virtual void loadOdf() {}
    virtual void saveOdf() {}
    virtual bool evaluate(const Database* database, int index) const
    {
        for (int i = 0; i < list.count(); ++i)
        {
            // lazy evaluation, stop on first false
            if (!list[i]->evaluate(database, index))
                return false;
        }
        return true;
    }
    virtual bool isEmpty() const { return list.isEmpty(); }
    virtual void removeConditions(int fieldNumber)
    {
        for (int i = 0; i < list.count(); ++i)
            list[i]->removeConditions(fieldNumber);
        QList<AbstractCondition*> newList;
        for (int i = 0; i < list.count(); ++i)
        {
            if (!list[i]->isEmpty())
                newList.append(list[i]);
        }
        list = newList;
    }
    virtual void dump() const
    {
        for (int i = 0; i < list.count(); ++i)
        {
            if (i)
                kDebug() << "AND" << endl;
            list[i]->dump();
        }
    }

public:
    QList<AbstractCondition*> list; // allowed: Or or Condition
};

/**
 * OpenDocument, 8.7.3 Filter Or
 */
class Filter::Or : public AbstractCondition
{
public:
    Or() {}
    Or(const Or& other);
    virtual ~Or() { qDeleteAll(list); }
    virtual Type type() const { return AbstractCondition::Or; }
    virtual void loadOdf() {}
    virtual void saveOdf() {}
    virtual bool evaluate(const Database* database, int index) const
    {
        for (int i = 0; i < list.count(); ++i)
        {
            // lazy evaluation, stop on first true
            if (list[i]->evaluate(database, index))
                return true;
        }
        return false;
    }
    virtual bool isEmpty() const { return list.isEmpty(); }
    virtual void removeConditions(int fieldNumber)
    {
        for (int i = 0; i < list.count(); ++i)
            list[i]->removeConditions(fieldNumber);
        QList<AbstractCondition*> newList;
        for (int i = 0; i < list.count(); ++i)
        {
            if (!list[i]->isEmpty())
                newList.append(list[i]);
        }
        list = newList;
    }
    virtual void dump() const
    {
        for (int i = 0; i < list.count(); ++i)
        {
            if (i)
                kDebug() << "OR" << endl;
            list[i]->dump();
        }
    }

public:
    QList<AbstractCondition*> list; // allowed: And or Condition
};

/**
 * OpenDocument, 8.7.4 Filter Condition
 */
class Filter::Condition : public AbstractCondition
{
public:
    Condition(int _fieldNumber, Comparison _comparison, const QString& _value,
              Qt::CaseSensitivity _caseSensitivity, Mode _mode)
        : fieldNumber(_fieldNumber)
        , value(_value)
        , operation(_comparison)
        , caseSensitivity(_caseSensitivity)
        , dataType(_mode)
    {
    }
    Condition(const Condition& other)
        : AbstractCondition()
        , fieldNumber(other.fieldNumber)
        , value(other.value)
        , operation(other.operation)
        , caseSensitivity(other.caseSensitivity)
        , dataType(other.dataType)
    {
    }
    virtual ~Condition() {}

    virtual Type type() const { return AbstractCondition::Condition; }
    virtual void loadOdf() {}
    virtual void saveOdf() {}
    virtual bool evaluate(const Database* database, int index) const
    {
        const Sheet* sheet = (*database->range().constBegin())->sheet();
        const QRect range = database->range().lastRange();
        const int start = database->orientation() == Qt::Vertical ? range.left() : range.top();
        kDebug() << "index: " << index << " start: " << start << " fieldNumber: " << fieldNumber << endl;
        const Value value = database->orientation() == Qt::Vertical
                            ? sheet->cellStorage()->value(start + fieldNumber, index)
                            : sheet->cellStorage()->value(index, start + fieldNumber);
        const QString testString = sheet->doc()->converter()->asString(value).asString();
        switch (operation)
        {
            case Match:
            {
                kDebug() << "Match? " << this->value << " " << testString << endl;
                if (QString::compare(this->value, testString, caseSensitivity) == 0)
                    return true;
                break;
            }
            case NotMatch:
            {
                kDebug() << "Not Match? " << this->value << " " << testString << endl;
                if (QString::compare(this->value, testString, caseSensitivity) != 0)
                    return true;
                break;
            }
            default:
                break;
        }
        return false;
    }
    virtual bool isEmpty() const { return fieldNumber == -1; }
    virtual void removeConditions(int fieldNumber)
    {
        if (this->fieldNumber == fieldNumber)
        {
            kDebug() << "removing fieldNumber " << fieldNumber << endl;
            this->fieldNumber = -1;
        }
    }
    virtual void dump() const
    {
        kDebug() << "Condition: fieldNumber: " << fieldNumber << " value: " << value << endl;
    }

public:
    int fieldNumber;
    QString value; // Value?
    Comparison operation;
    Qt::CaseSensitivity caseSensitivity;
    Mode dataType;
};

Filter::And::And(const And& other)
    : AbstractCondition()
{
    for (int i = 0; i < other.list.count(); ++i)
    {
        if (!other.list[i])
            continue;
        else if (other.list[i]->type() == AbstractCondition::And)
            continue;
        else if (other.list[i]->type() == AbstractCondition::Or)
            list.append(new Filter::Or(*static_cast<Filter::Or*>(other.list[i])));
        else
            list.append(new Filter::Condition(*static_cast<Filter::Condition*>(other.list[i])));
    }
}

Filter::Or::Or(const Or& other)
    : AbstractCondition()
{
    for (int i = 0; i < other.list.count(); ++i)
    {
        if (!other.list[i])
            continue;
        else if (other.list[i]->type() == AbstractCondition::And)
            list.append(new Filter::And(*static_cast<Filter::And*>(other.list[i])));
        else if (other.list[i]->type() == AbstractCondition::Or)
            continue;
        else
            list.append(new Filter::Condition(*static_cast<Filter::Condition*>(other.list[i])));
    }
}


class Filter::Private
{
public:
    Private()
        : condition( 0 )
        , conditionSource(Self)
        , displayDuplicates(true)
    {
    }

    AbstractCondition* condition;
    QString targetRangeAddress; // Region?
    enum { Self, CellRange } conditionSource;
    QString conditionSourceRangeAddress; // Region?
    bool displayDuplicates;
};

Filter::Filter()
    : d(new Private)
{
}

Filter::Filter(const Filter& other)
    : d(new Private)
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
    d->conditionSource = other.d->conditionSource;
    d->conditionSourceRangeAddress = other.d->conditionSourceRangeAddress;
    d->displayDuplicates = other.d->displayDuplicates;
}

Filter::~Filter()
{
    delete d->condition;
    delete d;
}

void Filter::addCondition(Composition composition,
                          int fieldNumber, Comparison comparison, const QString& value,
                          Qt::CaseSensitivity caseSensitivity, Mode mode)
{
    kDebug() << k_funcinfo << endl;
    Condition* condition = new Condition(fieldNumber, comparison, value, caseSensitivity, mode);
    if (!d->condition)
    {
        kDebug() << "no condition yet" << endl;
        d->condition = condition;
    }
    else if (composition == AndComposition)
    {
        kDebug() << "AndComposition" << endl;
        if (d->condition->type() == AbstractCondition::And)
        {
            static_cast<And*>(d->condition)->list.append(condition);
        }
        else
        {
            And* andComposition = new And();
            andComposition->list.append(d->condition);
            andComposition->list.append(condition);
            d->condition = andComposition;
        }
    }
    else // composition == OrComposition
    {
        if (d->condition->type() == AbstractCondition::Or)
        {
            static_cast<Or*>(d->condition)->list.append(condition);
        }
        else
        {
            Or* orComposition = new Or();
            orComposition->list.append(d->condition);
            orComposition->list.append(condition);
            d->condition = orComposition;
        }
    }
}

void Filter::removeConditions(int fieldNumber)
{
    if (fieldNumber == -1)
    {
        kDebug() << "removing all conditions" << endl;
        delete d->condition;
        d->condition = 0;
        return;
    }
    if (!d->condition)
        return;
    kDebug() << "removing condition for field " << fieldNumber << " from " << d->condition <<  endl;
    d->condition->removeConditions(fieldNumber);
    if (d->condition->isEmpty())
    {
        delete d->condition;
        d->condition = 0;
    }
}

bool Filter::isEmpty() const
{
    return d->condition ? d->condition->isEmpty() : true;
}

void Filter::apply(const Database* database) const
{
    Sheet* const sheet = (*database->range().constBegin())->sheet();
    const QRect range = database->range().lastRange();
    const int start = database->orientation() == Qt::Vertical ? range.top() : range.left();
    const int end = database->orientation() == Qt::Vertical ? range.bottom() : range.right();
    for (int i = start + 1; i <= end; ++i)
    {
        kDebug() << endl << "Checking column/row " << i << endl;
        if (database->orientation() == Qt::Vertical)
        {
            sheet->nonDefaultRowFormat(i)->setFiltered(d->condition ? !d->condition->evaluate(database, i) : false);
/*            if (d->condition->evaluate(database, i))
                kDebug() << "showing row " << i << endl;
            else
                kDebug() << "hiding row " << i << endl;*/
            sheet->emitHideRow();
        }
        else // database->orientation() == Qt::Horizontal
        {
            sheet->nonDefaultColumnFormat(i)->setFiltered(d->condition ? !d->condition->evaluate(database, i) : false);
            sheet->emitHideColumn();
        }
    }
}

void Filter::dump() const
{
    if (d->condition)
        d->condition->dump();
    else
        kDebug() << "Condition: 0" << endl;
}
