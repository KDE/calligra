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

#include <QList>
#include <QString>

#include "Filter.h"

using namespace KSpread;

class Filter::And
{
public:
    And( Condition*, Condition* )
    {
    }

    And( Or*, Or* )
    {
    }

    union OrOrCondition
    {
        Or* operation;
        Condition* condition;
    };
    QList<OrOrCondition> list;
};

class Filter::Or
{
public:
    Or( Condition*, Condition* )
    {
    }

    Or( And*, And* )
    {
    }

    union AndOrCondition
    {
        And* operation;
        Condition* condition;
    };
    QList<AndOrCondition> list;
};

class Filter::Condition
{
public:
    uint fieldNumber;
    QString value; // Value?
    enum { Match, NotMatch, Equal, NotEqual, Less, Greater, LessOrEqual, GreaterOrEqual } operation;
    bool caseSensitive;
    enum { Text, Number } dataType;
};

class Filter::Private
{
public:
    Private()
        : condition( 0 )
    {
    }

    union
    {
        And* andOperation;
        Or* orOperation;
        Condition* condition;
    };
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
