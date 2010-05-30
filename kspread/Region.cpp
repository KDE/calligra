/* This file is part of the KDE project
   Copyright 2005-2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU Library General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

// Local
#include "Region.h"

#include <QRegExp>
#include <QStringList>

#include <kdebug.h>

#include "Cell.h"
#include "Limits.h"
#include "Map.h"
#include "NamedAreaManager.h"
#include "Sheet.h"
#include "Util.h"

namespace KSpread
{

class Region::Private : public QSharedData
{
public:
    Private()
            : map(0),
            cells(QList<Element*>()) {
    }

    const Map* map;
    mutable QList<Element*> cells;
};


/***************************************************************************
  class Region
****************************************************************************/

Region::Region()
{
    d = new Private();
}

Region::Region(const QString& string, const Map* map, Sheet* fallbackSheet)
{
    d = new Private();
    d->map = map;

    if (string.isEmpty()) {
        return;
    }
    // FIXME Stefan: Does not respect quoted names!
    QStringList substrings = string.split(';');
    QStringList::ConstIterator end = substrings.constEnd();
    for (QStringList::ConstIterator it = substrings.constBegin(); it != end; ++it) {
        QString sRegion = *it;

        // check for a named area first
        const Region namedAreaRegion = map ? map->namedAreaManager()->namedArea(sRegion) : Region();
        if (namedAreaRegion.isValid()) {
            ConstIterator end(namedAreaRegion.d->cells.constEnd());
            for (ConstIterator it = namedAreaRegion.d->cells.constBegin(); it != end; ++it) {
                Element *element = *it;
                if (element->type() == Element::Point) {
                    Point* point = static_cast<Point*>(element);
                    d->cells.append(createPoint(*point));
                } else {
                    Range* range = static_cast<Range*>(element);
                    d->cells.append(createRange(*range));
                }
            }
            continue;
        }

        // Single cell or cell range
        int delimiterPos = sRegion.indexOf(':');
        if (delimiterPos > -1) {
            // range
            QString sUL = sRegion.left(delimiterPos);
            QString sLR = sRegion.mid(delimiterPos + 1);

            Sheet* firstSheet = map ? filterSheetName(sUL) : 0;
            Sheet* lastSheet = map ? filterSheetName(sLR) : 0;
            // TODO: lastSheet is silently ignored if it is different from firstSheet

            // Still has the sheet name separator?
            if (sUL.contains('!') || sLR.contains('!'))
                return;

            if (!firstSheet)
                firstSheet = fallbackSheet;
            if (!lastSheet)
                lastSheet = fallbackSheet;

            Point ul(sUL);
            Point lr(sLR);

            if (ul.isValid() && lr.isValid()) {
                Range* range = createRange(ul, lr);
                if (firstSheet) range->setSheet(firstSheet);
                d->cells.append(range);
            } else if (ul.isValid()) {
                Point* point = createPoint(ul);
                if (firstSheet) point->setSheet(firstSheet);
                d->cells.append(point);
            } else { // lr.isValid()
                Point* point = createPoint(lr);
                if (firstSheet) point->setSheet(firstSheet);
                d->cells.append(point);
            }
        } else {
            // single cell
            Sheet* sheet = map ? filterSheetName(sRegion) : 0;
            // Still has the sheet name separator?
            if (sRegion.contains('!'))
                return;
            if (!sheet)
                sheet = fallbackSheet;
            Point* point = createPoint(sRegion);
            if(sheet) point->setSheet(sheet);
            d->cells.append(point);
        }
    }
}

Region::Region(const QRect& rect, Sheet* sheet)
{
    d = new Private();

    Q_ASSERT(!rect.isNull());
    if (rect.isNull()) {
        kError(36001) << "Region::Region(const QRect&): QRect is empty!" << endl;
        return;
    }
    add(rect, sheet);
}

Region::Region(const QPoint& point, Sheet* sheet)
{
    d = new Private();

    Q_ASSERT(!point.isNull());
    if (point.isNull()) {
        kError(36001) << "Region::Region(const QPoint&): QPoint is empty!" << endl;
        return;
    }
    add(point, sheet);
}

Region::Region(const Region& list)
{
    d = new Private();
    d->map = list.d->map;

    ConstIterator end(list.d->cells.constEnd());
    for (ConstIterator it = list.d->cells.constBegin(); it != end; ++it) {
        Element *element = *it;
        if (element->type() == Element::Point) {
            Point* point = static_cast<Point*>(element);
            d->cells.append(createPoint(*point));
        } else {
            Range* range = static_cast<Range*>(element);
            d->cells.append(createRange(*range));
        }
    }
}

Region::Region(int x, int y, Sheet* sheet)
{
    d = new Private();

    Q_ASSERT(isValid(QPoint(x, y)));
    if (!isValid(QPoint(x, y))) {
        kError(36001) << "Region::Region(" << x << ", " << y << "): Coordinates are invalid!" << endl;
        return;
    }
    add(QPoint(x, y), sheet);
}

Region::Region(int x, int y, int width, int height, Sheet* sheet)
{
    d = new Private();

    Q_ASSERT(isValid(QRect(x, y, width, height)));
    if (!isValid(QRect(x, y, width, height))) {
        kError(36001) << "Region::Region(" << x << ", " << y << ", " << width << ", " << height << "): Dimensions are invalid!" << endl;
        return;
    }
    add(QRect(x, y, width, height), sheet);
}


Region::~Region()
{
    qDeleteAll(d->cells);
}

QVector<QRect> Region::rects() const
{
    QVector<QRect> cellRects;
    foreach(Element *element, d->cells) {
        cellRects.append(element->rect());
    }
    return cellRects;
}

const Map* Region::map() const
{
    Q_ASSERT(d->map);
    return d->map;
}

void Region::setMap(const Map* map)
{
    d->map = map;
}

bool Region::isValid() const
{
    if (d->cells.isEmpty())
        return false;
    ConstIterator end = d->cells.constEnd();
    for (ConstIterator it = d->cells.constBegin(); it != end; ++it) {
        if (!(*it)->isValid())
            return false;
    }
    return true;
}

bool Region::isSingular() const
{
    if (d->cells.isEmpty() || d->cells.count() > 1 || (*d->cells.constBegin())->type() != Element::Point) {
        return false;
    }
    return true;
}

bool Region::isContiguous() const
{
    if (d->cells.count() != 1 || !isValid()) {
        return false;
    }
    return true;
}

QString Region::name(Sheet* originSheet) const
{
    QStringList names;
    ConstIterator endOfList(d->cells.constEnd());
    for (ConstIterator it = d->cells.constBegin(); it != endOfList; ++it) {
        Element *element = *it;
        names += element->name(originSheet);
    }
    return names.isEmpty() ? "" : names.join(";");
}

Region::Element* Region::add(const QPoint& point, Sheet* sheet)
{
    return insert(d->cells.count(), point, sheet, false);
}

Region::Element* Region::add(const QRect& range, Sheet* sheet)
{
    const QRect normalizedRange = normalized(range);
    if (normalizedRange.width() == 0 || normalizedRange.height() == 0) {
        return 0;
    }
    if (normalizedRange.size() == QSize(1, 1)) {
        return add(normalizedRange.topLeft(), sheet);
    }
    return insert(d->cells.count(), normalizedRange, sheet, false);
}

Region::Element* Region::add(const Region& region, Sheet* sheet)
{
    ConstIterator endOfList(region.d->cells.constEnd());
    for (ConstIterator it = region.d->cells.constBegin(); it != endOfList; ++it) {
        add((*it)->rect(), (*it)->sheet() ? (*it)->sheet() : sheet);
    }
    return d->cells.isEmpty() ? 0 : d->cells.last();
}

void Region::sub(const QPoint& point, Sheet* sheet)
{
    // TODO Stefan: Improve!
    Iterator endOfList(d->cells.end());
    for (Iterator it = d->cells.begin(); it != endOfList; ++it) {
        Element *element = *it;
        if (element->sheet() != sheet) {
            continue;
        }
        if (element->rect() == QRect(point, point)) {
            delete element;
            d->cells.removeAll(element);
            break;
        }
    }
}

void Region::sub(const QRect& range, Sheet* sheet)
{
    const QRect normalizedRange = normalized(range);
    // TODO Stefan: Improve!
    Iterator endOfList(d->cells.end());
    for (Iterator it = d->cells.begin(); it != endOfList; ++it) {
        Element *element = *it;
        if (element->sheet() != sheet) {
            continue;
        }
        if (element->rect() == normalizedRange) {
            delete element;
            d->cells.removeAll(element);
            break;
        }
    }
}

void Region::sub(const Region& region)
{
    ConstIterator endOfList(region.constEnd());
    for (ConstIterator it = region.constBegin(); it != endOfList; ++it) {
        Element *element = *it;
        if (element->type() == Element::Point) {
            Point* point = static_cast<Point*>(element);
            sub(Region(point->pos()));
        } else {
            sub(Region(element->rect()));
        }
    }
}

Region Region::intersected(const Region& region)
{
    Region result;
    ConstIterator end(region.constEnd());
    for (ConstIterator it = region.constBegin(); it != end; ++it) {
        Element *element = *it;
        if (element->type() == Element::Point) {
            Point* point = static_cast<Point*>(element);
            if(contains(point->pos(), element->sheet()))
                result.add(point->pos(), element->sheet());
        } else {
            QRect rect = element->rect();
            for(int c = rect.top(); c <= rect.bottom(); ++c) {
                for(int r = rect.left(); r <= rect.right(); ++r) {
                    QPoint p(r,c);
                    if(contains(p, element->sheet()))
                        result.add(p, element->sheet());
                }
            }
        }
    }
    return result;
}

Region::Element* Region::eor(const QPoint& point, Sheet* sheet)
{
    bool containsPoint = false;

    int index = 0;
    while (index < d->cells.count()) {
        if (!d->cells[index]->contains(point)) {
            ++index;
            continue;
        }
        containsPoint = true;
        int x = point.x();
        int y = point.y();
        QRect fullRange = d->cells[index]->rect();
        delete d->cells.takeAt(index);

        // top range
        int left = fullRange.left();
        int top = fullRange.top();
        int width = fullRange.width();
        int height = y - top;
        if (height > 0) {
            insert(index, QRect(left, top, width, height), sheet);
        }
        // left range
        left = fullRange.left();
        top = y;
        width = qMax(0, x - left);
        height = 1;
        if (width > 0) {
            insert(index, QRect(left, top, width, height), sheet);
        }
        // right range
        left = qMin(x + 1, fullRange.right());
        top = y;
        width = qMax(0, fullRange.right() - x);
        height = 1;
        if (width > 0) {
            insert(index, QRect(left, top, width, height), sheet);
        }
        // bottom range
        left = fullRange.left();
        top = y + 1;
        width = fullRange.width();
        height = qMax(0, fullRange.bottom() - y);
        if (height > 0) {
            insert(index, QRect(left, top, width, height), sheet);
        }
        return d->cells[index];
    }

    if (!containsPoint) {
        return add(point, sheet);
    }
    return 0;
}

Region::Element* Region::insert(int pos, const QPoint& point, Sheet* sheet, bool multi)
{
    if (point.x() < 1 || point.y() < 1) {
        return 0;
    }

    bool containsPoint = false;
//   bool adjacentPoint = false;
//   QRect neighbour;

    // we don't have to check for occurrences?
    if (multi) {
        Point* rpoint = createPoint(point);
        rpoint->setSheet(sheet);
        if (pos > d->cells.count())
            pos = d->cells.count();
        d->cells.insert(pos, rpoint);
        return d->cells[pos];
    }

    ConstIterator endOfList(d->cells.constEnd());
    for (ConstIterator it = d->cells.constBegin(); it != endOfList; ++it) {
        Element *element = *it;
        if (sheet && sheet != element->sheet()) {
            continue;
        }
        if (element->contains(point)) {
            containsPoint = true;
            break;
        }
        /*    else
            {
              neighbour = element->rect();
              neighbour.setTopLeft(neighbour.topLeft() - QPoint(1,1));
              neighbour.setBottomRight(neighbour.bottomRight() + QPoint(1,1));
              if (neighbour.contains(point))
              {
                adjacentPoint = true; // TODO Stefan: Implement!
                break;
              }
            }*/
    }
    if (!containsPoint) {
        Point* rpoint = createPoint(point);
        rpoint->setSheet(sheet);
        if (pos > d->cells.count())
            pos = d->cells.count();
        d->cells.insert(pos, rpoint);
        return d->cells[pos];
    }
    return 0;
}

Region::Element* Region::insert(int pos, const QRect& range, Sheet* sheet, bool multi)
{
    const QRect normalizedRange = normalized(range);
    if (normalizedRange.size() == QSize(1, 1)) {
        return insert(pos, normalizedRange.topLeft(), sheet);
    }

    if (multi) {
        Range* rrange = createRange(normalizedRange);
        rrange->setSheet(sheet);
        if (pos > d->cells.count())
            pos = d->cells.count();
        d->cells.insert(pos, rrange);
        return d->cells[pos];
    }

    bool containsRange = false;

    for (int index = 0; index < d->cells.count(); ++index) {
        if (sheet && sheet != d->cells[index]->sheet()) {
            ++index;
            continue;
        }
        if (d->cells[index]->contains(normalizedRange)) {
            containsRange = true;
        } else if (normalizedRange.contains(d->cells[index]->rect())) {
            delete d->cells.takeAt(index--);
            continue;
        }
    }
    if (!containsRange) {
        Range* rrange = createRange(normalizedRange);
        rrange->setSheet(sheet);
        if (pos > d->cells.count())
            pos = d->cells.count();
        d->cells.insert(pos, rrange);
        return d->cells[pos];
    }
    return 0;
}

QSet<int> Region::columnsSelected() const
{
    QSet<int> result;
    ConstIterator endOfList(d->cells.constEnd());
    for (ConstIterator it = d->cells.constBegin(); it != endOfList; ++it) {
        if ((*it)->isColumn()) {
            const QRect range = (*it)->rect();
            const int right = range.right();
            for (int col = range.left(); col <= right; ++col) {
                result << col;
            }
        }
    }
    return result;
}

QSet<int> Region::rowsSelected() const
{
    QSet<int> result;
    ConstIterator endOfList(d->cells.constEnd());
    for (ConstIterator it = d->cells.constBegin(); it != endOfList; ++it) {
        if ((*it)->isRow()) {
            const QRect range = (*it)->rect();
            const int bottom = range.bottom();
            for (int row = range.top(); row <= bottom; ++row) {
                result << row;
            }
        }
    }
    return result;
}

QSet<int> Region::columnsAffected() const
{
    QSet<int> result;
    ConstIterator endOfList(d->cells.constEnd());
    for (ConstIterator it = d->cells.constBegin(); it != endOfList; ++it) {
        const QRect range = (*it)->rect();
        const int right = range.right();
        for (int col = range.left(); col <= right; ++col) {
            result << col;
        }
    }
    return result;
}

QSet<int> Region::rowsAffected() const
{
    QSet<int> result;
    ConstIterator endOfList(d->cells.constEnd());
    for (ConstIterator it = d->cells.constBegin(); it != endOfList; ++it) {
        const QRect range = (*it)->rect();
        const int bottom = range.bottom();
        for (int row = range.top(); row <= bottom; ++row) {
            result << row;
        }
    }
    return result;
}

bool Region::isColumnSelected(uint col) const
{
    ConstIterator endOfList(d->cells.constEnd());
    for (ConstIterator it = d->cells.constBegin(); it != endOfList; ++it) {
        Element *element = *it;
        QRect region = element->rect();
        if ((col == 0 || ((int)col >= region.left() && (int)col <= region.right())) &&
                region.top() == 1 && region.bottom() == KS_rowMax) {
            return true;
        }
    }
    return false;
}

bool Region::isRowSelected(uint row) const
{
    ConstIterator endOfList(d->cells.constEnd());
    for (ConstIterator it = d->cells.constBegin(); it != endOfList; ++it) {
        Element *element = *it;
        QRect region = element->rect();
        if ((row == 0 || ((int)row >= region.top() && (int)row <= region.bottom())) &&
                region.left() == 1 && region.right() == KS_colMax) {
            return true;
        }
    }
    return false;
}

bool Region::isColumnOrRowSelected() const
{
    ConstIterator endOfList(d->cells.constEnd());
    for (ConstIterator it = d->cells.constBegin(); it != endOfList; ++it) {
        Element *element = *it;
        QRect region = element->rect();
        if ((region.top() == 1 && region.bottom() == KS_rowMax) ||
                (region.left() == 1 && region.right() == KS_colMax)) {
            return true;
        }
    }
    return false;
}

bool Region::isAllSelected() const
{
    if (d->cells.count() != 1)
        return false;
    Q_ASSERT(d->cells.first());
    return d->cells.first()->isAll();
}

bool Region::contains(const QPoint& point, Sheet* sheet) const
{
    if (d->cells.isEmpty()) {
        return false;
    }
    ConstIterator endOfList(d->cells.constEnd());
    for (ConstIterator it = d->cells.constBegin(); it != endOfList; ++it) {
        Element *element = *it;
        if (element->contains(point)) {
            if (sheet && element->sheet() != sheet) {
                return false;
            }
            return true;
        }
    }
    return false;
}

bool Region::isEmpty() const
{
    return d->cells.isEmpty();
}

void Region::clear()
{
    for (int i = 0; i < d->cells.count(); ++i) {
        delete d->cells.takeAt(0);
    }
}

QRect Region::firstRange() const
{
    if (!isValid())
        return QRect();
    return d->cells.value(0)->rect();
}

QRect Region::lastRange() const
{
    if (!isValid())
        return QRect();
    return d->cells.value(d->cells.count() - 1)->rect();
}

Sheet* Region::firstSheet() const
{
    if (!isValid())
        return 0;
    return d->cells.value(0)->sheet();
}

Sheet* Region::lastSheet() const
{
    if (!isValid())
        return 0;
    return d->cells.value(d->cells.count() - 1)->sheet();
}

QRect Region::boundingRect() const
{
    int left   = KS_colMax;
    int right  = 1;
    int top    = KS_rowMax;
    int bottom = 1;
    Region::ConstIterator endOfList = cells().constEnd();
    for (Region::ConstIterator it = cells().constBegin(); it != endOfList; ++it) {
        QRect range = (*it)->rect();
        if (range.left() < left) {
            left = range.left();
        }
        if (range.right() > right) {
            right = range.right();
        }
        if (range.top() < top) {
            top = range.top();
        }
        if (range.bottom() > bottom) {
            bottom = range.bottom();
        }
    }
    return QRect(left, top, right - left + 1, bottom - top + 1);
}

QRect Region::normalized(const QRect& rect)
{
    QRect normalizedRect(rect);
    if (rect.left() > rect.right()) {
        normalizedRect.setLeft(rect.right());
        normalizedRect.setRight(rect.left());
    }
    if (rect.top() > rect.bottom()) {
        normalizedRect.setTop(rect.bottom());
        normalizedRect.setBottom(rect.top());
    }
    if (rect.right() > KS_colMax) {
        normalizedRect.setRight(KS_colMax);
    }
    if (rect.bottom() > KS_rowMax) {
        normalizedRect.setBottom(KS_rowMax);
    }
    return normalizedRect;
}

Region::ConstIterator Region::constBegin() const
{
    return d->cells.constBegin();
}

Region::ConstIterator Region::constEnd() const
{
    return d->cells.constEnd();
}

bool Region::isValid(const QPoint& point)
{
    if (point.x() < 1 || point.y() < 1 ||
            point.x() > KS_colMax ||  point.y() > KS_rowMax)
        return false;
    else
        return true;
}

bool Region::isValid(const QRect& rect)
{
    if (!isValid(rect.topLeft()) || !isValid(rect.bottomRight()) ||
            rect.width() == 0 || rect.height() == 0)
        return false;
    else
        return true;
}

// static
QString Region::loadOdf(const QString& expression)
{
    QString result;
    QString temp;
    bool isRange = false;
    enum { Start, InQuotes } state = Start;
    int i = 0;
    // NOTE Stefan: As long as KSpread does not support fixed sheets eat the dollar sign.
    if (expression[i] == '$')
        ++i;
    while (i < expression.count()) {
        switch (state) {
        case Start: {
            if (expression[i] == '\'') { // quoted sheet name or named area
                temp.append(expression[i]);
                state = InQuotes;
            } else if (expression[i] == '.') { // sheet name separator
                // was there already a sheet name?
                if (!temp.isEmpty() && !isRange) {
                    result.append(temp);
                    result.append('!');
                }
                temp.clear();
            } else if (expression[i] == ':') { // cell separator
                isRange = true;
                result.append(temp);
                result.append(':');
                temp.clear();
                // NOTE Stefan: As long as KSpread does not support fixed sheets eat the dollar sign.
                if (i + 2 < expression.count() && expression[i+1] == '$' && expression[i+2] != '.')
                    ++i;
            } else if (expression[i] == ' ') { // range separator
                result.append(temp);
                result.append(';');
                temp.clear();
            } else
                temp.append(expression[i]);
            ++i;
            break;
        }
        case InQuotes: {
            temp.append(expression[i]);
            if (expression[i] == '\'') {
                // an escaped apostrophe?
                if (i + 1 < expression.count() && expression[i+1] == '\'')
                    ++i; // eat it
                else // the end
                    state = Start;
            }
            ++i;
            break;
        }
        }
    }
    return result + temp;
}

// static
QString Region::saveOdf(const QString& expression)
{
    QString result;
    QString sheetName;
    QString temp;
    enum { Start, InQuotes } state = Start;
    int i = 0;
    while (i < expression.count()) {
        switch (state) {
        case Start: {
            if (expression[i] == '\'') {
                temp.append(expression[i]);
                state = InQuotes;
            } else if (expression[i] == '!') { // sheet name separator
                // There has to be a sheet name.
                if (temp.isEmpty())
                    return expression; // error
                if (temp.count() > 2 && (temp[0] != '\'' && temp[temp.count()-1] != '\'')) {
                    temp.replace('\'', "''");
                    if (temp.contains(' ') || temp.contains('.') ||
                            temp.contains(';') || temp.contains('!') ||
                            temp.contains('$') || temp.contains(']'))
                        temp = '\'' + temp + '\'';
                }
                sheetName = temp;
                result.append(temp);
                result.append('.');
                temp.clear();
            } else if (expression[i] == ':') { // cell separator
                if (result.isEmpty())
                    result = '.';
                result.append(temp);
                result.append(':');
                result.append(sheetName);
                result.append('.');
                temp.clear();
            } else if (expression[i] == ';') { // range separator
                result.append(temp);
                result.append(' ');
                temp.clear();
            } else
                temp.append(expression[i]);
            ++i;
            break;
        }
        case InQuotes: {
            temp.append(expression[i]);
            if (expression[i] == '\'') {
                // an escaped apostrophe?
                if (i + 1 < expression.count() && expression[i+1] == '\'')
                    ++i; // eat it
                else // the end
                    state = Start;
            }
            ++i;
            break;
        }
        }
    }
    if (result.isEmpty())
        result = '.';
    return result + temp;
}

QString Region::saveOdf() const
{
    return saveOdf(Region::name());
}

QList<Region::Element*>& Region::cells() const
{
    return d->cells;
}

bool Region::operator==(const Region& other) const
{
    if (d->cells.count() != other.d->cells.count())
        return false;
    ConstIterator endOfList(d->cells.constEnd());
    ConstIterator endOfOtherList(other.d->cells.constEnd());
    ConstIterator it = d->cells.constBegin();
    ConstIterator it2 = other.d->cells.constBegin();
    while (it != endOfList && it2 != endOfOtherList) {
        if ((*it)->sheet() != (*it2)->sheet())
            return false;
        if ((*it++)->rect() != (*it2++)->rect())
            return false;
    }
    return true;
}

void Region::operator=(const Region& other)
{
    d->map = other.d->map;
    clear();
    ConstIterator end(other.d->cells.constEnd());
    for (ConstIterator it = other.d->cells.constBegin(); it != end; ++it) {
        Element *element = *it;
        if (element->type() == Element::Point) {
            Point* point = static_cast<Point*>(element);
            d->cells.append(createPoint(*point));
        } else {
            Range* range = static_cast<Range*>(element);
            d->cells.append(createRange(*range));
        }
    }
}

Sheet* Region::filterSheetName(QString& sRegion)
{
    Sheet* sheet = 0;
    int delimiterPos = sRegion.lastIndexOf('!');
    if (delimiterPos < 0)
        delimiterPos = sRegion.lastIndexOf('.');
    if (delimiterPos > -1) {
        const QString sheetName = sRegion.left(delimiterPos);
        sheet = d->map->findSheet(sheetName);
        // try again without apostrophes
        if (!sheet && sheetName.count() > 2 && sheetName[0] == '\'' && sheetName[sheetName.count()-1] == '\'')
            sheet = d->map->findSheet(sheetName.mid(1, sheetName.count() - 2));
        // remove the sheet name, incl. '!', from the string
        if (sheet)
            sRegion = sRegion.right(sRegion.length() - delimiterPos - 1);
    }
    return sheet;
}

Region::Point* Region::createPoint(const QPoint& point) const
{
    return new Point(point);
}

Region::Point* Region::createPoint(const QString& string) const
{
    return new Point(string);
}

Region::Point* Region::createPoint(const Point& point) const
{
    return new Point(point);
}

Region::Range* Region::createRange(const QRect& rect) const
{
    return new Range(rect);
}

Region::Range* Region::createRange(const Point& tl, const Point& br) const
{
    return new Range(tl, br);
}

Region::Range* Region::createRange(const QString& string) const
{
    return new Range(string);
}

Region::Range* Region::createRange(const Range& range) const
{
    return new Range(range);
}

/***************************************************************************
  class Element
****************************************************************************/

Region::Element::Element()
        : m_sheet(0)
{
}

Region::Element::~Element()
{
}


/***************************************************************************
  class Point
****************************************************************************/

Region::Point::Point(const QPoint& point)
        : Region::Element()
        , m_point(point)
        , m_fixedColumn(false)
        , m_fixedRow(false)
{
    if (m_point.x() > KS_colMax)
        m_point.setX(KS_colMax);
    if (m_point.y() > KS_rowMax)
        m_point.setY(KS_rowMax);
}

Region::Point::Point(const QString& string)
        : Region::Element()
        , m_fixedColumn(false)
        , m_fixedRow(false)
{
    const uint length = string.length();
    if (length == 0)
        return;

    uint p = 0;

    // Fixed ?
    if (string[0] == '$') {
        m_fixedColumn = true;
        p++;
    }

    // Malformed ?
    if (p == length)
        return;

    if ((string[p] < 'A' || string[p] > 'Z') && (string[p] < 'a' || string[p] > 'z'))
        return;

    //default is error
    int x = -1;
    //search for the first character != text
    int result = string.indexOf(QRegExp("[^A-Za-z]+"), p);

    //get the column number for the character between actual position and the first non text charakter
    if (result != -1)
        x = Util::decodeColumnLabelText(string.mid(p, result - p));     // x is defined now
    else  // If there isn't any, then this is not a point -> return
        return;
    p = result;

    //limit the x-value
    //Q_ASSERT(x >= 1 && x <= KS_colMax);
    if (x < 1)
        return;
    if (x > KS_colMax)
        x = KS_colMax;

    // Malformed ?
    if (p == length)
        return;

    if (string[p] == '$') {
        m_fixedRow = true;
        p++;
    }

    // Malformed ?
    if (p == length)
        return;

    uint p2 = p;
    while (p < length) {
        if (!string[p++].isDigit())
            return;
    }

    bool ok;
    int y = string.mid(p2, p - p2).toInt(&ok);

    //limit the y-value
    //Q_ASSERT(y >= 1 && y <= KS_rowMax);
    if (!ok || y < 1)
        return;
    if (y > KS_rowMax)
        y = KS_rowMax;

    m_point = QPoint(x, y);
}

Region::Point::~Point()
{
}

QString Region::Point::name(Sheet* originSheet) const
{
    QString name;
    if (m_sheet && m_sheet != originSheet) {
        name.append(m_sheet->sheetName());
        name.replace('\'', "''");
        if (name.contains('!') || name.contains(' ') || name.contains(';') || name.contains('$'))
            name = '\'' + name + '\'';
        name.append('!');
    }
    if (m_fixedColumn)
        name.append('$');
    name.append(Cell::columnName(m_point.x()));
    if (m_fixedRow)
        name.append('$');
    name.append(QString::number(m_point.y()));
    return name;
}

bool Region::Point::contains(const QPoint& point) const
{
    return (m_point == point);
}

bool Region::Point::contains(const QRect& range) const
{
    return (range.width() == 1) && (range.height() == 1) && (range.topLeft() == m_point);
}

Cell Region::Point::cell() const
{
    return Cell(m_sheet, m_point);
}

/***************************************************************************
  class Range
****************************************************************************/

Region::Range::Range(const QRect& rect)
        : Region::Element()
        , m_range(rect)
        , m_fixedTop(false)
        , m_fixedLeft(false)
        , m_fixedBottom(false)
        , m_fixedRight(false)
{
    if (m_range.right() > KS_colMax)
        m_range.setRight(KS_colMax);
    if (m_range.bottom() > KS_rowMax)
        m_range.setBottom(KS_rowMax);
}

Region::Range::Range(const KSpread::Region::Point& ul, const KSpread::Region::Point& lr)
        : Region::Element()
        , m_fixedTop(false)
        , m_fixedLeft(false)
        , m_fixedBottom(false)
        , m_fixedRight(false)
{
    if (!ul.isValid() || !lr.isValid())
        return;
    m_range = QRect(ul.pos(), lr.pos());
    m_fixedTop    = ul.isRowFixed();
    m_fixedLeft   = ul.isColumnFixed();
    m_fixedBottom = lr.isRowFixed();
    m_fixedRight  = lr.isColumnFixed();
}

Region::Range::Range(const QString& sRange)
        : Region::Element()
        , m_fixedTop(false)
        , m_fixedLeft(false)
        , m_fixedBottom(false)
        , m_fixedRight(false)
{
    int delimiterPos = sRange.indexOf(':');
    if (delimiterPos == -1)
        return;

    Region::Point ul(sRange.left(delimiterPos));
    Region::Point lr(sRange.mid(delimiterPos + 1));

    if (!ul.isValid() || !lr.isValid())
        return;
    m_range = QRect(ul.pos(), lr.pos());
    m_fixedTop    = ul.isRowFixed();
    m_fixedLeft   = ul.isColumnFixed();
    m_fixedBottom = lr.isRowFixed();
    m_fixedRight  = lr.isColumnFixed();
}

Region::Range::~Range()
{
}

bool Region::Range::isColumn() const
{
    return (m_range.top() == 1 && m_range.bottom() == KS_rowMax);
}

bool Region::Range::isRow() const
{
    return (m_range.left() == 1 && m_range.right() == KS_colMax);
}

bool Region::Range::isAll() const
{
    return (m_range == QRect(1, 1, KS_colMax, KS_rowMax));
}

bool Region::Range::contains(const QPoint& point) const
{
    return m_range.contains(point);
}

bool Region::Range::contains(const QRect& range) const
{
    return m_range.contains(normalized(range));
}

QString Region::Range::name(Sheet* originSheet) const
{
    QString name;
    if (m_sheet && m_sheet != originSheet) {
        name.append(m_sheet->sheetName());
        name.replace('\'', "''");
        if (name.contains('!') || name.contains(' ') || name.contains(';') || name.contains('$'))
            name = '\'' + name + '\'';
        name.append('!');
    }
    if (m_fixedLeft)
        name.append('$');
    name.append(Cell::columnName(m_range.left()));
    if (m_fixedTop)
        name.append('$');
    name.append(QString::number(m_range.top()));
    name.append(':');
    if (m_fixedRight)
        name.append('$');
    name.append(Cell::columnName(m_range.right()));
    if (m_fixedBottom)
        name.append('$');
    name.append(QString::number(m_range.bottom()));
    return name;
}

} // namespace KSpread
