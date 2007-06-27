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
#include <QString>

using namespace KSpread;

class AbstractCondition
{
public:
    virtual ~AbstractCondition() {}
    virtual void loadOdf() = 0;
    virtual void saveOdf() = 0;
    virtual bool evaluate(const QString& testString) const = 0;
    virtual bool isEmpty() const = 0;
    virtual void removeConditions(int fieldNumber) = 0;
};

/**
 * OpenDocument, 8.7.2 Filter And
 */
class Filter::And : public AbstractCondition
{
public:
    virtual void loadOdf() {}
    virtual void saveOdf() {}
    virtual bool evaluate(const QString& testString) const
    {
        for (int i = 0; i < list.count(); ++i)
        {
            // lazy evaluation, stop on first false
            if (!list[i]->evaluate(testString))
                return false;
        }
        return true;
    }
    virtual bool isEmpty() const { return list.isEmpty(); }
    virtual void removeConditions(int fieldNumber)
    {
        for (int i = 0; i < list.count(); ++i)
            list[i]->removeConditions(fieldNumber);
        QList<AbstractCondition*> list;
        for (int i = 0; i < list.count(); ++i)
        {
            if (!list[i]->isEmpty())
                list.append(this->list[i]);
        }
        this->list = list;
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
    virtual void loadOdf() {}
    virtual void saveOdf() {}
    virtual bool evaluate(const QString& testString) const
    {
        for (int i = 0; i < list.count(); ++i)
        {
            // lazy evaluation, stop on first true
            if (list[i]->evaluate(testString))
                return true;
        }
        return false;
    }
    virtual bool isEmpty() const { return list.isEmpty(); }
    virtual void removeConditions(int fieldNumber)
    {
        for (int i = 0; i < list.count(); ++i)
            list[i]->removeConditions(fieldNumber);
        QList<AbstractCondition*> list;
        for (int i = 0; i < list.count(); ++i)
        {
            if (!list[i]->isEmpty())
                list.append(this->list[i]);
        }
        this->list = list;
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
    Condition(int fieldNumber, Comparison comparison, const QString& value,
              Qt::CaseSensitivity caseSensitivity, Mode mode)
        : fieldNumber(fieldNumber)
        , value(value)
        , operation(comparison)
        , caseSensitivity(caseSensitivity)
        , dataType(mode)
    {
    }

    virtual void loadOdf() {}
    virtual void saveOdf() {}
    virtual bool evaluate(const QString& testString) const
    {
        switch (operation)
        {
            case Match:
            {
                if (QString::compare(value, testString, caseSensitivity) == 0)
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
            this->fieldNumber = -1;
    }

public:
    int fieldNumber;
    QString value; // Value?
    Comparison operation;
    Qt::CaseSensitivity caseSensitivity;
    Mode dataType;
};


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
    : d( new Private )
{
}

Filter::~Filter()
{
    delete d;
}

void Filter::addCondition(Composition composition,
                          int fieldNumber, Comparison comparison, const QString& value,
                          Qt::CaseSensitivity caseSensitivity, Mode mode)
{
    Condition* condition = new Condition(fieldNumber, comparison, value, caseSensitivity, mode);
    if (!d->condition)
    {
        d->condition = condition;
    }
    else if (composition == AndComposition)
    {
        And* andComposition = new And();
        andComposition->list.append(d->condition);
        andComposition->list.append(condition);
        d->condition = andComposition;
    }
    else // composition == OrComposition
    {
        Or* orComposition = new Or();
        orComposition->list.append(d->condition);
        orComposition->list.append(condition);
        d->condition = orComposition;
    }
}

void Filter::removeConditions(int fieldNumber)
{
    if (fieldNumber == -1)
    {
        delete d->condition;
        d->condition = 0;
        return;
    }
    d->condition->removeConditions(fieldNumber);
    if (d->condition->isEmpty())
    {
        delete d->condition;
        d->condition = 0;
    }
}
