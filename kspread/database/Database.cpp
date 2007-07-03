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

#include "Database.h"

#include <QString>

#include <KoXmlNS.h>
#include <KoXmlWriter.h>

#include "DatabaseSource.h"
#include "Filter.h"
#include "FilterPopup.h"
#include "Region.h"
#include "Util.h"

using namespace KSpread;

class Sort;
class SubtotalRules;

class Database::Private : public QSharedData
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

Database::Database()
    : d( new Private )
{
}

Database::Database( const QString& name )
    : d( new Private )
{
    d->name = name;
}

Database::Database(const Database& other)
    : QObject(other.parent())
    , d(other.d)
{
}

Database::~Database()
{
}

bool Database::isEmpty() const
{
    return d->name.isNull(); // it may be empty though
}

Qt::Orientation Database::orientation() const
{
    return d->orientation == Private::Row ? Qt::Vertical : Qt::Horizontal;
}

bool Database::displayFilterButtons() const
{
    return d->displayFilterButtons;
}

void Database::setDisplayFilterButtons( bool enable )
{
    d->displayFilterButtons = enable;
}

const KSpread::Region& Database::range() const
{
    return d->targetRangeAddress;
}

void Database::setRange( const Region& region )
{
    Q_ASSERT( region.isContiguous() );
    d->targetRangeAddress = region;
}

void Database::showPopup(QWidget* parent, const Cell& cell, const QRect& cellRect)
{
    kDebug() << k_funcinfo << endl;
    dump();
    FilterPopup* popup = new FilterPopup(parent, cell, this);
    const QPoint position(orientation() == Qt::Vertical ? cellRect.bottomLeft() : cellRect.topRight());
    popup->move(parent->mapToGlobal(position));
    popup->show();
}

Filter* Database::filter()
{
    return d->filter;
}

bool Database::loadOdf(const KoXmlElement& element, Sheet* const sheet)
{
    // TODO
    return true;
}

void Database::saveOdf(KoXmlWriter& xmlWriter) const
{
    if (d->targetRangeAddress.isEmpty())
        return;
    xmlWriter.startElement("table:database-range");
    if (!d->name.isNull())
        xmlWriter.addAttribute("table:name", d->name);
    if (d->isSelection)
        xmlWriter.addAttribute("table:is-selection", "true");
    if (d->onUpdateKeepStyles)
        xmlWriter.addAttribute("table:on-update-keep-styles", "true");
    if (!d->onUpdateKeepSize)
        xmlWriter.addAttribute("table:on-update-keep-size", "false");
    if (!d->hasPersistentData)
        xmlWriter.addAttribute("table:has-persistent-data", "false");
    if (d->orientation == Private::Column)
        xmlWriter.addAttribute("table:orientation", "column");
    if (!d->containsHeader)
        xmlWriter.addAttribute("table:contains-header", "false");
    if (d->displayFilterButtons)
        xmlWriter.addAttribute("table:display-filter-buttons", "true");
    xmlWriter.addAttribute("table:target-range-address", Oasis::encodeFormula(d->targetRangeAddress.name()));
    if (d->refreshDelay)
        xmlWriter.addAttribute("table:refresh-delay", d->refreshDelay);
    // TODO
//     if (d->source)
//         d->source->saveOdf(xmlWriter);
//     if (d->sort)
//         d->sort->saveOdf(xmlWriter);
    if (d->filter)
        d->filter->saveOdf(xmlWriter);
//     if (d->subtotalRules)
//         d->subtotalRules->saveOdf(xmlWriter);
    xmlWriter.endElement();
}

void Database::operator=( const Database& other )
{
    d = other.d;
}

bool Database::operator==( const Database& other ) const
{
    return (d == other.d);
}

bool Database::operator<( const Database& other ) const
{
    return (d < other.d);
}

void Database::dump() const
{
    if (d->filter) d->filter->dump();
}

#include "Database.moc"
