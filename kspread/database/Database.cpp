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
#include "Map.h"
#include "Region.h"

using namespace KSpread;

class Sort;
class SubtotalRules;

class Database::Private : public QSharedData
{
public:
    Private()
            : source(0)
            , sort(0)
            , filter(new Filter())
            , subtotalRules(0)
            , isSelection(false)
            , onUpdateKeepStyles(false)
            , onUpdateKeepSize(true)
            , hasPersistentData(true)
            , orientation(Row)
            , containsHeader(true)
            , displayFilterButtons(false)
            , refreshDelay(0) {
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
            , refreshDelay(other.refreshDelay) {
    }

    virtual ~Private() {
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
        : d(new Private)
{
}

Database::Database(const QString& name)
        : d(new Private)
{
    d->name = name;
}

Database::Database(const Database& other)
        : d(other.d)
{
}

Database::~Database()
{
}

bool Database::isEmpty() const
{
    return d->name.isNull(); // it may be empty though
}

const QString& Database::name() const
{
    return d->name;
}

void Database::setName(const QString& name)
{
    d->name = name;
}

Qt::Orientation Database::orientation() const
{
    return d->orientation == Private::Row ? Qt::Vertical : Qt::Horizontal;
}

bool Database::containsHeader() const
{
    return d->containsHeader;
}

void Database::setContainsHeader(bool enable)
{
    d->containsHeader = enable;
}

bool Database::displayFilterButtons() const
{
    return d->displayFilterButtons;
}

void Database::setDisplayFilterButtons(bool enable)
{
    d->displayFilterButtons = enable;
}

const KSpread::Region& Database::range() const
{
    return d->targetRangeAddress;
}

void Database::setRange(const Region& region)
{
    Q_ASSERT(region.isContiguous());
    d->targetRangeAddress = region;
}

void Database::showPopup(QWidget* parent, const Cell& cell, const QRect& cellRect)
{
    FilterPopup* popup = new FilterPopup(parent, cell, this);
    const QPoint position(orientation() == Qt::Vertical ? cellRect.bottomLeft() : cellRect.topRight());
    popup->move(parent->mapToGlobal(position));
    popup->show();
}

const Filter& Database::filter() const
{
    return *d->filter;
}

void Database::setFilter(const Filter& filter)
{
    if (*d->filter == filter)
        return;
    delete d->filter;
    d->filter = new Filter(filter);
}

bool Database::loadOdf(const KoXmlElement& element, const Map* map)
{
    if (element.hasAttributeNS(KoXmlNS::table, "name"))
        d->name = element.attributeNS(KoXmlNS::table, "name", QString());
    if (element.hasAttributeNS(KoXmlNS::table, "is-selection")) {
        if (element.attributeNS(KoXmlNS::table, "is-selection", "false") == "true")
            d->isSelection = true;
        else
            d->isSelection = false;
    }
    if (element.hasAttributeNS(KoXmlNS::table, "on-update-keep-styles")) {
        if (element.attributeNS(KoXmlNS::table, "on-update-keep-styles", "false") == "true")
            d->onUpdateKeepStyles = true;
        else
            d->onUpdateKeepStyles = false;
    }
    if (element.hasAttributeNS(KoXmlNS::table, "on-update-keep-size")) {
        if (element.attributeNS(KoXmlNS::table, "on-update-keep-size", "true") == "false")
            d->onUpdateKeepSize = false;
        else
            d->onUpdateKeepSize = true;
    }
    if (element.hasAttributeNS(KoXmlNS::table, "has-persistent-data")) {
        if (element.attributeNS(KoXmlNS::table, "has-persistent-data", "true") == "false")
            d->hasPersistentData = false;
        else
            d->hasPersistentData = true;
    }
    if (element.hasAttributeNS(KoXmlNS::table, "orientation")) {
        if (element.attributeNS(KoXmlNS::table, "orientation", "row") == "column")
            d->orientation = Private::Column;
        else
            d->orientation = Private::Row;
    }
    if (element.hasAttributeNS(KoXmlNS::table, "contains-header")) {
        if (element.attributeNS(KoXmlNS::table, "contains-header", "true") == "false")
            d->containsHeader = false;
        else
            d->containsHeader = true;
    }
    if (element.hasAttributeNS(KoXmlNS::table, "display-filter-buttons")) {
        if (element.attributeNS(KoXmlNS::table, "display-filter-buttons", "false") == "true")
            d->displayFilterButtons = true;
        else
            d->displayFilterButtons = false;
    }
    if (element.hasAttributeNS(KoXmlNS::table, "target-range-address")) {
        const QString address = element.attributeNS(KoXmlNS::table, "target-range-address", QString());
        // only absolute addresses allowed; no fallback sheet needed
        d->targetRangeAddress = Region(Region::loadOdf(address), map);
        if (!d->targetRangeAddress.isValid())
            return false;
    }
    if (element.hasAttributeNS(KoXmlNS::table, "refresh-delay")) {
        bool ok = false;
        d->refreshDelay = element.attributeNS(KoXmlNS::table, "refresh-delay", QString()).toInt(&ok);
        if (!ok || d->refreshDelay < 0)
            return false;
    }
    KoXmlElement child;
    forEachElement(child, element) {
        if (child.namespaceURI() != KoXmlNS::table)
            continue;
        if (child.localName() == "database-source-sql") {
            // TODO
        } else if (child.localName() == "database-source-table") {
            // TODO
        } else if (child.localName() == "database-source-query") {
            // TODO
        } else if (child.localName() == "sort") {
            // TODO
        } else if (child.localName() == "filter") {
            d->filter = new Filter();
            if (!d->filter->loadOdf(child, map)) {
                delete d->filter;
                d->filter = 0;
                return false;
            }
        } else if (child.localName() == "subtotal-rules") {
            // TODO
        }
    }
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
    xmlWriter.addAttribute("table:target-range-address", d->targetRangeAddress.saveOdf());
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

void Database::operator=(const Database & other)
{
    d = other.d;
}

bool Database::operator==(const Database& other) const
{
    // NOTE Stefan: Don't compare targetRangeAddress.
    if (d->name != other.d->name)
        return false;
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
//     if (*d->source != *other.d->source)
//         return false;
//     if (*d->sort != *other.d->sort)
//         return false;
    if (*d->filter != *other.d->filter)
        return false;
//     if (*d->subtotalRules != *other.d->subtotalRules)
//         return false;
    return true;
}

bool Database::operator<(const Database& other) const
{
    return (d < other.d);
}

void Database::dump() const
{
    if (d->filter) d->filter->dump();
}
