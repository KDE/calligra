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
#include "Filter.h"
#include "FilterPopup.h"
#include "Region.h"

#include "commands/ApplyFilterCommand.h"

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
        , filter(new Filter())
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

    Private(const Private& other)
        : QSharedData(other)
        , source(/*other.source ? new DatabaseSource(*other.source) : */0)
        , sort(/*other.sort ? new Sort(*other.sort) : */0)
        , filter(other.filter ? new Filter(*other.filter) : 0)
        , subtotalRules(/*other.subtotalRules ? new SubtotalRules(*other.subtotalRules) : */0)
        , name(other.name)
        , isSelection(other.isSelection)
        , onUpdateKeepStyles(other.onUpdateKeepStyles)
        , onUpdateKeepSize(other.onUpdateKeepSize)
        , hasPersistentData(other.hasPersistentData)
        , orientation(other.orientation)
        , containsHeader(other.containsHeader)
        , displayFilterButtons(other.displayFilterButtons)
        , targetRangeAddress(other.targetRangeAddress)
        , refreshDelay(other.refreshDelay)
    {
    }

    virtual ~Private()
    {
//         delete source;
//         delete sort;
        delete filter;
//         delete subtotalRules;
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

private:
    void operator=(const Private&);
};

DatabaseRange::DatabaseRange()
    : d( new Private )
{
}

DatabaseRange::DatabaseRange( const QString& name )
    : d( new Private )
{
    d->name = name;
}

DatabaseRange::DatabaseRange(const DatabaseRange& other)
    : QObject(other.parent())
    , d(other.d)
{
}

DatabaseRange::~DatabaseRange()
{
}

bool DatabaseRange::isEmpty() const
{
    return d->name.isNull(); // it may be empty though
}

Qt::Orientation DatabaseRange::orientation() const
{
    return d->orientation == Private::Row ? Qt::Vertical : Qt::Horizontal;
}

bool DatabaseRange::displayFilterButtons() const
{
    return d->displayFilterButtons;
}

void DatabaseRange::setDisplayFilterButtons( bool enable )
{
    d->displayFilterButtons = enable;
}

const KSpread::Region& DatabaseRange::range() const
{
    return d->targetRangeAddress;
}

void DatabaseRange::setRange( const Region& region )
{
    Q_ASSERT( region.isContiguous() );
    d->targetRangeAddress = region;
}

void DatabaseRange::showPopup(QWidget* parent, const Cell& cell, const QRect& cellRect)
{
    FilterPopup* popup = new FilterPopup(parent, cell, *this);
    const QPoint position(orientation() == Qt::Vertical ? cellRect.bottomLeft() : cellRect.topRight());
    popup->move(parent->mapToGlobal(position));
    popup->resize(100, 20);
    popup->show();
}

void DatabaseRange::applyFilter() const
{
    d->filter->apply(*this);
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

void DatabaseRange::updateSubFilter(FilterPopup* popup)
{
    popup->updateFilter(d->filter);
    // TODO Stefan: Create and execute apply filter command.
    ApplyFilterCommand* command = new ApplyFilterCommand();
    kDebug() << "DatabaseRange::updateSubFilter: " << d->targetRangeAddress << endl;
    command->setSheet((*d->targetRangeAddress.constBegin())->sheet());
    command->add(d->targetRangeAddress);
    command->setDatabase(*this); // FIXME Stefan: Really needed?
    command->execute();
}

#include "DatabaseRange.moc"
