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

#include "DatabaseRange.h"

#include <QString>

#include "DatabaseSource.h"
#include "Region.h"

using namespace KSpread;

class Filter;
class Sort;
class SubtotalRules;

class DatabaseRange::Private : public QSharedData
{
public:
    Private()
        : source( 0 )
        , sort( 0 )
        , filter( 0 )
        , subtotalRules( 0 )
        , isSelection( false )
        , onUpdateKeepStyles( false )
        , onUpdateKeepSize( true )
        , hasPersistentData( true )
        , orientation( Row )
        , containsHeader( true )
        , displayFilterButtons( false )
        , refreshDelay( 0 )
    {
    }

    DatabaseSource* source;
    Sort* sort;
    Filter* filter;
    SubtotalRules* subtotalRules;
    QString name;
    bool isSelection                    : 1;
    bool onUpdateKeepStyles             : 1;
    bool onUpdateKeepSize               : 1;
    bool hasPersistentData              : 1;
    enum { Row, Column } orientation    : 1;
    bool containsHeader                 : 1;
    bool displayFilterButtons           : 1;
    Region targetRangeAddress;
    int refreshDelay;
};

DatabaseRange::DatabaseRange()
    : d( 0 )
{
}

DatabaseRange::DatabaseRange( const QString& name )
    : d( new Private )
{
    d->name = name;
}

DatabaseRange::DatabaseRange( const DatabaseRange& other )
    : d( other.d )
{
}

DatabaseRange::~DatabaseRange()
{
}

bool DatabaseRange::isEmpty() const
{
    return !d;
}

bool DatabaseRange::horizontallyOriented() const
{
    return ( d->orientation == Private::Column );
}

bool DatabaseRange::verticallyOriented() const
{
    return ( d->orientation == Private::Row );
}

const Region& DatabaseRange::range() const
{
    return d->targetRangeAddress;
}

void DatabaseRange::setRange( const Region& region )
{
    Q_ASSERT( region.isContiguous() );
    d->targetRangeAddress = region;
}

void DatabaseRange::operator=( const DatabaseRange& other )
{
    d = other.d;
}

bool DatabaseRange::operator==( const DatabaseRange& other ) const
{
    return ( d && other.d ) ? ( d->name == other.d->name ) : ( d == other.d );
}

bool DatabaseRange::operator<( const DatabaseRange& other ) const
{
    return (d && other.d) ? ( d->name < other.d->name ) : (d < other.d);
}
