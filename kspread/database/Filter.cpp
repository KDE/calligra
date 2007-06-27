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
    virtual bool evaluate() const = 0;
};

/**
 * OpenDocument, 8.7.2 Filter And
 */
class Filter::And : public AbstractCondition
{
public:
    virtual void loadOdf() {}
    virtual void saveOdf() {}
    virtual bool evaluate() const
    {
        for (int i = 0; i < list.count(); ++i)
        {
            // lazy evaluation, stop on first false
            if (!list[i]->evaluate())
                return false;
        }
        return true;
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
    virtual bool evaluate() const
    {
        for (int i = 0; i < list.count(); ++i)
        {
            // lazy evaluation, stop on first true
            if (list[i]->evaluate())
                return true;
        }
        return false;
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
    Condition()
        : fieldNumber(0)
        , operation(Match)
        , caseSensitive(Qt::CaseInsensitive)
        , dataType(Text)
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
                if (QString::compare(value, testString, caseSensitive) == 0)
                    return true;
                break;
            }
            default:
                break;
        }
        return false;
    }

public:
    uint fieldNumber;
    QString value; // Value?
    enum { Match, NotMatch, Equal, NotEqual, Less, Greater, LessOrEqual, GreaterOrEqual } operation;
    Qt::CaseSensitivity caseSensitive;
    enum { Text, Number } dataType;
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
