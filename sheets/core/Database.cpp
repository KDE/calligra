/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Database.h"

#include <QString>

#include "DataFilter.h"
// #include "Map.h"
#include "engine/Region.h"

using namespace Calligra::Sheets;

class Q_DECL_HIDDEN Database::Private : public QSharedData
{
public:
    Private()
        : filter()
        , isSelection(false)
        , onUpdateKeepStyles(false)
        , onUpdateKeepSize(true)
        , hasPersistentData(true)
        , containsHeader(true)
        , displayFilterButtons(false)
        , orientation(Qt::Vertical)
        , refreshDelay(0)
    {
    }

    Private(const Private &other)
        : QSharedData(other)
        , filter(other.filter)
        , isSelection(other.isSelection)
        , onUpdateKeepStyles(other.onUpdateKeepStyles)
        , onUpdateKeepSize(other.onUpdateKeepSize)
        , hasPersistentData(other.hasPersistentData)
        , containsHeader(other.containsHeader)
        , displayFilterButtons(other.displayFilterButtons)
        , orientation(other.orientation)
        , targetRangeAddress(other.targetRangeAddress)
        , refreshDelay(other.refreshDelay)
    {
    }

    virtual ~Private() = default;

    Filter filter;
    QString name;
    bool isSelection : 1;
    bool onUpdateKeepStyles : 1;
    bool onUpdateKeepSize : 1;
    bool hasPersistentData : 1;
    bool containsHeader : 1;
    bool displayFilterButtons : 1;
    Qt::Orientation orientation;
    Region targetRangeAddress;
    int refreshDelay;

private:
    void operator=(const Private &) = delete;
};

Database::Database()
    : d(new Private)
{
}

Database::Database(const QString &name)
    : d(new Private)
{
    d->name = name;
}

Database::Database(const Database &other)

    = default;

Database::~Database() = default;

bool Database::isEmpty() const
{
    return range().isEmpty(); // it may be empty though
}

bool Database::isSelection() const
{
    return d->isSelection;
}

void Database::setIsSelection(bool sel)
{
    d->isSelection = sel;
}

Qt::Orientation Database::orientation() const
{
    return d->orientation;
}

void Database::setOrientation(Qt::Orientation o)
{
    d->orientation = o;
}

bool Database::containsHeader() const
{
    return d->containsHeader;
}

void Database::setContainsHeader(bool enable)
{
    d->containsHeader = enable;
}

bool Database::onUpdateKeepStyles() const
{
    return d->onUpdateKeepStyles;
}

bool Database::onUpdateKeepSize() const
{
    return d->onUpdateKeepSize;
}

bool Database::hasPersistentData() const
{
    return d->hasPersistentData;
}

void Database::setOnUpdateKeepStyles(bool val)
{
    d->onUpdateKeepStyles = val;
}

void Database::setOnUpdateKeepSize(bool val)
{
    d->onUpdateKeepSize = val;
}

void Database::setHasPersistentData(bool val)
{
    d->hasPersistentData = val;
}

int Database::refreshDelay() const
{
    return d->refreshDelay;
}

void Database::setRefreshDelay(int delay)
{
    d->refreshDelay = delay;
}

bool Database::displayFilterButtons() const
{
    return d->displayFilterButtons;
}

void Database::setDisplayFilterButtons(bool enable)
{
    d->displayFilterButtons = enable;
}

const Calligra::Sheets::Region &Database::range() const
{
    return d->targetRangeAddress;
}

void Database::setRange(const Region &region)
{
    Q_ASSERT(region.isContiguous());
    d->targetRangeAddress = region;
}

const Filter &Database::filter() const
{
    return d->filter;
}

void Database::setFilter(const Filter &filter)
{
    if (d->filter == filter)
        return;
    d->filter = filter;
}

void Database::operator=(const Database &other)
{
    d = other.d;
}

bool Database::operator==(const Database &other) const
{
    // NOTE Stefan: Don't compare targetRangeAddress.
    if (d->isSelection != other.d->isSelection)
        return false;
    if (d->onUpdateKeepStyles != other.d->onUpdateKeepStyles)
        return false;
    if (d->onUpdateKeepSize != other.d->onUpdateKeepSize)
        return false;
    if (d->hasPersistentData != other.d->hasPersistentData)
        return false;
    if (d->orientation != other.d->orientation)
        return false;
    if (d->containsHeader != other.d->containsHeader)
        return false;
    if (d->displayFilterButtons != other.d->displayFilterButtons)
        return false;
    if (d->refreshDelay != other.d->refreshDelay)
        return false;
    if (d->filter != other.d->filter)
        return false;
    return true;
}

bool Database::operator<(const Database &other) const
{
    return (d < other.d);
}

void Database::dump() const
{
    d->filter.dump();
}
