/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 2005-2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

// Local
#include "Selection.h"

#include <kdebug.h>

#include <KoCanvasBase.h>

#include "Cell.h"
#include "CellStorage.h"
#include "RowColumnFormat.h"
#include "Sheet.h"

#include "commands/DataManipulators.h"

#include "ui/CellEditor.h"

using namespace KSpread;

// TODO
// - Allow resizing of all ranges in a normal selection; not just the last one.
// - Get rid of anchor and marker. They are the corners of the active element.


/***************************************************************************
  class Selection::Private
****************************************************************************/

class Selection::Private
{
public:
    Private() {
        activeSheet = 0;
        originSheet = 0;
        anchor = QPoint(1, 1);
        cursor = QPoint(1, 1);
        marker = QPoint(1, 1);

        colors.push_back(Qt::red);
        colors.push_back(Qt::blue);
        colors.push_back(Qt::magenta);
        colors.push_back(Qt::darkRed);
        colors.push_back(Qt::darkGreen);
        colors.push_back(Qt::darkMagenta);
        colors.push_back(Qt::darkCyan);
        colors.push_back(Qt::darkYellow);

        multipleOccurences = false;
        selectionMode = MultipleCells;

        activeElement = 1;
        activeSubRegionStart = 0;
        activeSubRegionLength = 1;

        canvasBase = 0;
        referenceMode = false;
    }

    Sheet* activeSheet;
    Sheet* originSheet;
    QPoint anchor;
    QPoint cursor;
    QPoint marker;
    QList<QColor> colors;

    bool multipleOccurences : 1;
    Mode selectionMode : 2;

    // For reference selections this selection represents all references in a
    // formula. The user can place the text cursor at any reference while
    // editing the formula. Such a reference may not just be a contiguous range,
    // but a non-contiguous sub-region.
    // (Even though the text delimiter that separates ranges in a sub-region,
    // ';', is also used as delimiter for function arguments. Functions, that
    // accept two or more adjacent references as arguments cannot cope with
    // non-contiguous references for this reason. In this case it's up to the
    // user to select references, that serve the function's needs.)
    // That's what the next three variables are for.
    // For 'normal' selections these variables are actually superfluous, but may
    // be used in conjunction with the reference selection where appropriate.
    int activeElement; // the active range in a referenced sub-region
    int activeSubRegionStart; // the start of a referenced sub-region
    int activeSubRegionLength; // the length of a referenced sub-region

    KoCanvasBase* canvasBase;
    bool referenceMode : 1;
    Region formerSelection; // for reference selection mode
    Region oldSelection; // for select all
};

/***************************************************************************
  class Selection
****************************************************************************/

Selection::Selection(KoCanvasBase* canvasBase)
        : KoToolSelection(0)
        , Region(1, 1)
        , d(new Private())
{
    d->canvasBase = canvasBase;
}

Selection::Selection(const Selection& selection)
        : KoToolSelection(selection.parent())
        , Region()
        , d(new Private())
{
    d->activeSheet = selection.d->activeSheet;
    d->originSheet = selection.d->originSheet;
    d->activeElement = cells().count();
    d->activeSubRegionStart = 0;
    d->activeSubRegionLength = cells().count();
    d->canvasBase = selection.d->canvasBase;
}

Selection::~Selection()
{
    delete d;
}

KoCanvasBase* Selection::canvas() const
{
    return d->canvasBase;
}

void Selection::initialize(const QPoint& point, Sheet* sheet)
{
    if (!isValid(point))
        return;

    if (!d->activeSheet)
        return;

    if (!sheet) {
        if (d->originSheet) {
            sheet = d->originSheet;
        } else {
            sheet = d->activeSheet;
        }
    }

    Region changedRegion(*this);
    changedRegion.add(extendToMergedAreas(QRect(d->anchor, d->marker)));

    // for the case of a merged cell
    QPoint topLeft(point);
    Cell cell(d->activeSheet, point);
    if (cell.isPartOfMerged()) {
        cell = cell.masterCell();
        topLeft = QPoint(cell.column(), cell.row());
    }

    d->anchor = topLeft;
    d->cursor = point;
    d->marker = topLeft;

    fixSubRegionDimension(); // TODO remove this sanity check
    int index = d->activeSubRegionStart + d->activeSubRegionLength;
    if (insert(index, topLeft, sheet/*, true*/)) {
        // if the point was inserted
        clearSubRegion();
        // Sets:
        // d->activeElement = d->activeSubRegionStart + 1;
        // d->activeSubRegionLength = 0;
    } else {
        kWarning() << "Unable to insert" << topLeft << "in" << sheet->sheetName();
    }
    Element* element = cells()[d->activeSubRegionStart];
    // we end up with one element in the subregion
    d->activeSubRegionLength = 1;
    if (element && element->type() == Element::Point) {
        Point* point = static_cast<Point*>(element);
        point->setColor(d->colors[cells().size() % d->colors.size()]);
    } else if (element && element->type() == Element::Range) {
        Range* range = static_cast<Range*>(element);
        range->setColor(d->colors[cells().size() % d->colors.size()]);
    }

    if (changedRegion == *this) {
        emitChanged(Region(topLeft, sheet));
        return;
    }
    changedRegion.add(topLeft, sheet);

    emitChanged(changedRegion);
}

void Selection::initialize(const QRect& range, Sheet* sheet)
{
    if (!isValid(range) || (range == QRect(0, 0, 1, 1)))
        return;

    if (!d->activeSheet)
        return;

    if (d->selectionMode == SingleCell) {
        initialize(range.bottomRight(), sheet);
        return;
    }

    if (!sheet) {
        if (d->originSheet) {
            sheet = d->originSheet;
        } else {
            sheet = d->activeSheet;
        }
    }

    Region changedRegion(*this);
    changedRegion.add(extendToMergedAreas(QRect(d->anchor, d->marker)));

    // for the case of a merged cell
    QPoint topLeft(range.topLeft());
    Cell cell(d->activeSheet, topLeft);
    if (cell.isPartOfMerged()) {
        cell = cell.masterCell();
        topLeft = QPoint(cell.column(), cell.row());
    }

    // for the case of a merged cell
    QPoint bottomRight(range.bottomRight());
    cell = Cell(d->activeSheet, bottomRight);
    if (cell.isPartOfMerged()) {
        cell = cell.masterCell();
        bottomRight = QPoint(cell.column(), cell.row());
    }

    d->anchor = topLeft;
    d->cursor = bottomRight;
    d->marker = bottomRight;

    fixSubRegionDimension(); // TODO remove this sanity check
    int index = d->activeSubRegionStart + d->activeSubRegionLength;
    if (insert(index, QRect(topLeft, bottomRight), sheet/*, true*/)) {
        // if the range was inserted
        clearSubRegion();
        // Sets:
        // d->activeElement = d->activeSubRegionStart + 1;
        // d->activeSubRegionLength = 0;
    } else {
        kWarning() << "Unable to insert" << topLeft << "in" << sheet->sheetName();
    }
    Element* element = cells()[d->activeSubRegionStart];
    // we end up with one element in the subregion
    d->activeSubRegionLength = 1;
    if (element && element->type() == Element::Point) {
        Point* point = static_cast<Point*>(element);
        point->setColor(d->colors[cells().size() % d->colors.size()]);
    } else if (element && element->type() == Element::Range) {
        Range* range = static_cast<Range*>(element);
        range->setColor(d->colors[cells().size() % d->colors.size()]);
    }

    if (changedRegion == *this) {
        return;
    }
    changedRegion.add(QRect(topLeft, bottomRight), sheet);

    emitChanged(changedRegion);
}

void Selection::initialize(const Region& region, Sheet* sheet)
{
    if (!region.isValid())
        return;

    if (d->selectionMode == SingleCell) {
        if (!cells().isEmpty())
            initialize(region.firstRange().bottomRight(), sheet);
        return;
    }

    if (!sheet) {
        if (d->originSheet) {
            sheet = d->originSheet;
        } else {
            sheet = d->activeSheet;
        }
    }

    Region changedRegion(*this);
    changedRegion.add(extendToMergedAreas(QRect(d->anchor, d->marker)));

    // TODO Stefan: handle subregion insertion
    // TODO Stefan: handle obscured cells correctly
    Region::clear(); // all elements; no residuum
    Element* element = add(region);
    if (element && element->type() == Element::Point) {
        Point* point = static_cast<Point*>(element);
        point->setColor(d->colors[cells().size() % d->colors.size()]);
    } else if (element && element->type() == Element::Range) {
        Range* range = static_cast<Range*>(element);
        range->setColor(d->colors[cells().size() % d->colors.size()]);
    }

    // for the case of a merged cell
    QPoint topLeft(cells().last()->rect().topLeft());
    Cell cell(d->activeSheet, topLeft);
    if (cell.isPartOfMerged()) {
        cell = cell.masterCell();
        topLeft = QPoint(cell.column(), cell.row());
    }

    // for the case of a merged cell
    QPoint bottomRight(cells().last()->rect().bottomRight());
    cell = Cell(d->activeSheet, bottomRight);
    if (cell.isPartOfMerged()) {
        cell = cell.masterCell();
        bottomRight = QPoint(cell.column(), cell.row());
    }

    d->anchor = topLeft;
    d->cursor = topLeft;
    d->marker = bottomRight;

    d->activeElement = cells().count();
    d->activeSubRegionStart = 0;
    d->activeSubRegionLength = cells().count();

    if (changedRegion == *this) {
        return;
    }
    changedRegion.add(region);

    emitChanged(changedRegion);
}

void Selection::update()
{
    emitChanged(*this);
}

void Selection::update(const QPoint& point)
{
    if (d->selectionMode == SingleCell) {
        initialize(point);
        d->activeElement = 1;
        d->activeSubRegionStart = 0;
        d->activeSubRegionLength = 1;
        return;
    }

    // A length of 0 means inserting at the position d->activeSubRegionStart.
    if (d->activeSubRegionLength == 0) {
        extend(point);
        return;
    }

    if (cells().isEmpty()) {
        initialize(point);
        d->activeElement = 1;
        d->activeSubRegionStart = 0;
        d->activeSubRegionLength = 1;
        return;
    }

    // Take the last range, if pointing beyond the sub-region's end.
    const int subRegionEnd = d->activeSubRegionStart + d->activeSubRegionLength;
    const bool atEnd = d->activeElement >= subRegionEnd;
    if (atEnd) {
        // d->activeSubRegionLength == 0 is already excluded.
        d->activeElement = subRegionEnd - 1;
    }

    Sheet* sheet = cells()[d->activeElement]->sheet();
    if (sheet != d->activeSheet) {
        extend(point);
        d->activeElement = cells().count();
        d->activeSubRegionStart = cells().count() - 1;
        d->activeSubRegionLength = 1;
        return;
    }

    // for the case of a merged cell
    QPoint topLeft(point);
    Cell cell(d->activeSheet, point);
    if (cell.isPartOfMerged()) {
        cell = cell.masterCell();
        topLeft = QPoint(cell.column(), cell.row());
    }

    if (topLeft == d->marker) {
        return;
    }

    QRect area1 = cells()[d->activeElement]->rect();
    QRect newRange = extendToMergedAreas(QRect(d->anchor, topLeft));

    // If the updated range is bigger, it may cover already existing ranges.
    // These get removed, if multiple occurences are not allowed. Store the old
    // amount of ranges, to figure out how many ranges have been removed later.
    const int count = cells().count();
    // The update may have shrunk the range, which would be containend in
    // the former range. Remove the latter before inserting the new range.
    delete cells().takeAt(d->activeElement);
    insert(d->activeElement, newRange, sheet, d->multipleOccurences);
    const int delta = cells().count() - count;
    d->activeSubRegionLength += delta;
    if (atEnd) {
        d->activeElement = d->activeSubRegionStart + d->activeSubRegionLength;
    } else {
        d->activeElement += delta;
    }

    QRect area2 = newRange;
    Region changedRegion;

    bool newLeft   = area1.left() != area2.left();
    bool newTop    = area1.top() != area2.top();
    bool newRight  = area1.right() != area2.right();
    bool newBottom = area1.bottom() != area2.bottom();

    /* first, calculate some numbers that we'll use a few times */
    int farLeft = qMin(area1.left(), area2.left());
    int innerLeft = qMax(area1.left(), area2.left());

    int farTop = qMin(area1.top(), area2.top());
    int innerTop = qMax(area1.top(), area2.top());

    int farRight = qMax(area1.right(), area2.right());
    int innerRight = qMin(area1.right(), area2.right());

    int farBottom = qMax(area1.bottom(), area2.bottom());
    int innerBottom = qMin(area1.bottom(), area2.bottom());

    if (newLeft) {
        changedRegion.add(QRect(QPoint(farLeft, innerTop),
                                QPoint(innerLeft - 1, innerBottom)));
        if (newTop) {
            changedRegion.add(QRect(QPoint(farLeft, farTop),
                                    QPoint(innerLeft - 1, innerTop - 1)));
        }
        if (newBottom) {
            changedRegion.add(QRect(QPoint(farLeft, innerBottom + 1),
                                    QPoint(innerLeft - 1, farBottom)));
        }
    }

    if (newTop) {
        changedRegion.add(QRect(QPoint(innerLeft, farTop),
                                QPoint(innerRight, innerTop - 1)));
    }

    if (newRight) {
        changedRegion.add(QRect(QPoint(innerRight + 1, innerTop),
                                QPoint(farRight, innerBottom)));
        if (newTop) {
            changedRegion.add(QRect(QPoint(innerRight + 1, farTop),
                                    QPoint(farRight, innerTop - 1)));
        }
        if (newBottom) {
            changedRegion.add(QRect(QPoint(innerRight + 1, innerBottom + 1),
                                    QPoint(farRight, farBottom)));
        }
    }

    if (newBottom) {
        changedRegion.add(QRect(QPoint(innerLeft, innerBottom + 1),
                                QPoint(innerRight, farBottom)));
    }

    d->marker = topLeft;
    d->cursor = point;

    emitChanged(changedRegion);
}

void Selection::extend(const QPoint& point, Sheet* sheet)
{
    if (!isValid(point))
        return;

    if (isEmpty() || d->selectionMode == SingleCell) {
        initialize(point, sheet);
        return;
    }

    kDebug() ;

    if (!sheet) {
        if (d->originSheet) {
            sheet = d->originSheet;
        } else {
            sheet = d->activeSheet;
        }
    }

    Region changedRegion = Region(extendToMergedAreas(QRect(d->marker, d->marker)));

    // for the case of a merged cell
    QPoint topLeft(point);
    Cell cell(d->activeSheet, point);
    if (cell.isPartOfMerged()) {
        cell = cell.masterCell();
        topLeft = QPoint(cell.column(), cell.row());
    }

    if (d->multipleOccurences) {
        const int subRegionEnd = d->activeSubRegionStart + d->activeSubRegionLength;
        const bool prepend = d->activeSubRegionLength == 0;
        const bool atEnd = d->activeElement == subRegionEnd;
        // Insert the new location after the active element, if possible.
        const int index = d->activeElement + ((prepend || atEnd) ? 0 : 1);
        insert(index, topLeft, sheet, true);
        ++d->activeSubRegionLength;
        ++d->activeElement;
        d->anchor = topLeft;
        d->marker = topLeft;
    } else {
        // TODO Replace for normal selection and resizing of any range.
        // The new point may split an existing range. Anyway, the new
        // location/range is appended and the last element becomes active.
        const int count = cells().count();
        eor(topLeft, sheet);
        d->activeSubRegionLength += cells().count() - count;
        d->activeElement = cells().count() - 1;
        d->anchor = cells()[d->activeElement]->rect().topLeft();
        d->marker = cells()[d->activeElement]->rect().bottomRight();
    }
    d->cursor = d->marker;

    changedRegion.add(topLeft, sheet);
    changedRegion.add(*this);

    emitChanged(changedRegion);
}

void Selection::extend(const QRect& range, Sheet* sheet)
{
    if (!isValid(range) || (range == QRect(0, 0, 1, 1)))
        return;

    if (isEmpty() || d->selectionMode == SingleCell) {
        initialize(range, sheet);
        return;
    }

    if (!sheet) {
        if (d->originSheet) {
            sheet = d->originSheet;
        } else {
            sheet = d->activeSheet;
        }
    }

    // for the case of a merged cell
    QPoint topLeft(range.topLeft());
    Cell cell(d->activeSheet, topLeft);
    if (cell.isPartOfMerged()) {
        cell = cell.masterCell();
        topLeft = QPoint(cell.column(), cell.row());
    }

    // for the case of a merged cell
    QPoint bottomRight(range.bottomRight());
    cell = Cell(d->activeSheet, bottomRight);
    if (cell.isPartOfMerged()) {
        cell = cell.masterCell();
        bottomRight = QPoint(cell.column(), cell.row());
    }

    const QRect newRange = extendToMergedAreas(QRect(topLeft, bottomRight));

    Element* element = 0;
    if (d->multipleOccurences) {
        const int subRegionEnd = d->activeSubRegionStart + d->activeSubRegionLength;
        const bool prepend = d->activeSubRegionLength == 0;
        const bool atEnd = d->activeElement == subRegionEnd;
        // Insert the new location after the active element, if possible.
        const int index = d->activeElement + ((prepend || atEnd) ? 0 : 1);
        insert(index, newRange, sheet, true);
        ++d->activeSubRegionLength;
        ++d->activeElement;
        d->anchor = newRange.topLeft();
        d->marker = newRange.bottomRight();
    } else {
        const int count = cells().count();
        element = add(newRange, sheet);
        d->activeSubRegionLength += cells().count() - count;
        d->activeElement = cells().count() - 1;
        d->anchor = cells()[d->activeElement]->rect().topLeft();
        d->marker = cells()[d->activeElement]->rect().bottomRight();
    }
    d->cursor = d->marker;

    if (element && element->type() == Element::Point) {
        Point* point = static_cast<Point*>(element);
        point->setColor(d->colors[cells().size() % d->colors.size()]);
    } else if (element && element->type() == Element::Range) {
        Range* range = static_cast<Range*>(element);
        range->setColor(d->colors[cells().size() % d->colors.size()]);
    }

    emitChanged(*this);
}

void Selection::extend(const Region& region)
{
    if (!region.isValid())
        return;

    uint count = cells().count();
    ConstIterator end(region.constEnd());
    for (ConstIterator it = region.constBegin(); it != end; ++it) {
        Element *element = *it;
        if (!element) continue;
        if (element->type() == Element::Point) {
            Point* point = static_cast<Point*>(element);
            extend(point->pos(), element->sheet());
        } else {
            extend(element->rect(), element->sheet());
        }
    }

    d->activeSubRegionLength += cells().count() - count;

    emitChanged(*this);
}

Selection::Element* Selection::eor(const QPoint& point, Sheet* sheet)
{
    // The selection always has to contain one location/range at least.
    if (isSingular()) {
        return Region::add(point, sheet);
    }
    return Region::eor(point, sheet);
}

const QPoint& Selection::anchor() const
{
    return d->anchor;
}

const QPoint& Selection::cursor() const
{
    return d->cursor;
}

const QPoint& Selection::marker() const
{
    return d->marker;
}

bool Selection::isSingular() const
{
    return Region::isSingular();
}

QString Selection::name(Sheet* sheet) const
{
    return Region::name(sheet ? sheet : d->originSheet);
}

void Selection::setActiveSheet(Sheet* sheet)
{
    if (d->activeSheet == sheet) {
        return;
    }
    d->activeSheet = sheet;
    emit activeSheetChanged(sheet);
}

Sheet* Selection::activeSheet() const
{
    return d->activeSheet;
}

void Selection::setOriginSheet(Sheet* sheet)
{
    d->originSheet = sheet;
}

Sheet* Selection::originSheet() const
{
    return d->originSheet;
}

int Selection::setActiveElement(const Cell &cell)
{
    for (int index = 0; index < cells().count(); ++index) {
        if (cells()[index]->sheet() != cell.sheet()) {
            continue;
        }
        QRect range = cells()[index]->rect();
        const QPoint point = cell.cellPosition();
        if (range.topLeft() == point || range.bottomRight() == point) {
            d->anchor = range.topLeft();
            d->cursor = range.bottomRight();
            d->marker = range.bottomRight();
            d->activeElement = index;
            // Only adjust the sub-region, if index is out of bounds.
            if (index < d->activeSubRegionStart) {
                d->activeSubRegionStart = index;
            }
            if (index > d->activeSubRegionStart + d->activeSubRegionLength) {
                d->activeSubRegionStart = index;
                d->activeSubRegionLength = 1;
            }
            return index;
        }
    }
    return -1;
}

KSpread::Region::Element* Selection::activeElement() const
{
    return (d->activeElement == cells().count()) ? 0 : cells()[d->activeElement];
}

void Selection::clear()
{
    d->activeElement = 0;
    d->activeSubRegionStart = 0;
    d->activeSubRegionLength = 0;
    Region::clear();
    // If this is the normal, not the reference mode, one element must survive.
    if (!referenceSelection()) {
        initialize(QPoint(1, 1), d->activeSheet);
    }
}

void Selection::clearSubRegion()
{
    if (isEmpty()) {
        return;
    }
    for (int index = 0; index < d->activeSubRegionLength; ++index) {
        delete cells().takeAt(d->activeSubRegionStart);
    }
    d->activeSubRegionLength = 0;
    d->activeElement = d->activeSubRegionStart + 1; // point behind the last
}

void Selection::fixSubRegionDimension()
{
    if (d->activeSubRegionStart > cells().count()) {
        kDebug() << "start position" << d->activeSubRegionStart << "exceeds list" << cells().count();
        d->activeSubRegionStart = 0;
        d->activeSubRegionLength = cells().count();
        return;
    }
    if (d->activeSubRegionStart + d->activeSubRegionLength > cells().count()) {
        kDebug() << "subregion (" << d->activeSubRegionStart << ".."
        << d->activeSubRegionStart + d->activeSubRegionLength
        << ") exceeds list" << cells().count();
        d->activeSubRegionLength = cells().count() - d->activeSubRegionStart;
        return;
    }
}

void Selection::setActiveSubRegion(int start, int length, int active)
{
    // Set the active sub-region.
    d->activeSubRegionStart = qBound(0, start, cells().count());
    d->activeSubRegionLength = qBound(0, length, cells().count() - d->activeSubRegionStart);

    // Set the active element.
    d->activeElement = qBound(d->activeSubRegionStart, active, d->activeSubRegionStart + d->activeSubRegionLength);

    if (isEmpty()) {
        return;
    }

    // Set the anchor, marker and cursor according to the active element.
    const int subRegionEnd = d->activeSubRegionStart + d->activeSubRegionLength;
    const bool atEnd = d->activeElement == subRegionEnd;
    const int index = qBound(0, d->activeElement - (atEnd ? 1 : 0), cells().count() - 1);
    const QRect range = cells()[index]->rect();
    d->anchor = range.topLeft();
    d->marker = range.bottomRight();
    d->cursor = d->marker;
}

QString Selection::activeSubRegionName() const
{
    QStringList names;
    int end = d->activeSubRegionStart + d->activeSubRegionLength;
    for (int index = d->activeSubRegionStart; index < end; ++index) {
        names += cells()[index]->name(d->originSheet);
    }
    return names.isEmpty() ? "" : names.join(";");
}

void Selection::setSelectionMode(Mode mode)
{
    d->selectionMode = mode;
}

const QList<QColor>& Selection::colors() const
{
    return d->colors;
}

void Selection::selectAll()
{
    if (!isAllSelected()) {
        d->oldSelection = *this;
        initialize(QRect(QPoint(KS_colMax, KS_rowMax), QPoint(1, 1)));
    } else {
        initialize(d->oldSelection);
        d->oldSelection.clear();
    }
}

void Selection::startReferenceSelection()
{
    // former selection exists - we are in ref mode already, even though it's suspended
    if (!d->formerSelection.isEmpty()) {
        setReferenceSelectionMode(true);
        return;
    }
    d->formerSelection = *this;
    clear(); // all elements; no residuum;
    setOriginSheet(activeSheet());
    // It is important to enable this AFTER we set the rect!
    d->referenceMode = true;
    d->multipleOccurences = true;
    // Visual cue to indicate that the user can drag-select the selection selection
    d->canvasBase->canvasWidget()->setCursor(Qt::CrossCursor);
}

void Selection::endReferenceSelection(bool saveChanges)
{
    // The reference selection may be temporarily disabled.
    // The stored selection reliably indicates the reference selection mode.
    if (d->formerSelection.isEmpty()) {
        return;
    }
    d->referenceMode = false;
    d->multipleOccurences = false;
    if (originSheet() != activeSheet()) {
        emit visibleSheetRequested(originSheet());
    }
    // While entering a formula the choose mode is turned on and off.
    // Clear the choice. Otherwise, cell references will stay highlighted.
    if (!isEmpty()) {
        emit changed(*this);
        clear(); // all elements; no residuum
    }
    if (saveChanges) {
        initialize(d->formerSelection);
    }
    d->formerSelection.clear();
    // The normal selection does not support the replacments of sub-regions.
    // Reset the active sub-region to the whole region.
    // TODO Why not allow that? Would make resizing of all ranges in a
    // non-contiguous selection possible!
    setActiveSubRegion(0, cells().count());
    d->canvasBase->canvasWidget()->setCursor(Qt::ArrowCursor);
}

void Selection::setReferenceSelectionMode(bool enable)
{
    d->referenceMode = enable;
    d->multipleOccurences = enable;
    d->canvasBase->canvasWidget()->setCursor(enable ? Qt::CrossCursor : Qt::ArrowCursor);
}

bool Selection::referenceSelectionMode() const
{
    return d->referenceMode;
}

bool Selection::referenceSelection() const
{
    return (!d->formerSelection.isEmpty());
}

void Selection::emitAboutToModify()
{
    emit aboutToModify(*this);
}

void Selection::emitModified()
{
    emit modified(*this);
}

void Selection::emitRefreshSheetViews()
{
    emit refreshSheetViews();
}

void Selection::emitVisibleSheetRequested(Sheet* sheet)
{
    emit visibleSheetRequested(sheet);
}

void Selection::emitCloseEditor(bool saveChanges, bool expandMatrix)
{
    emit closeEditor(saveChanges, expandMatrix);
}

void Selection::emitRequestFocusEditor()
{
    emit requestFocusEditor();
}

QRect Selection::extendToMergedAreas(const QRect& _area) const
{
    if (!d->activeSheet)
        return _area;

    QRect area = normalized(_area);
    Cell cell(d->activeSheet, area.left(), area.top());

    if (Region::Range(area).isColumn() || Region::Range(area).isRow()) {
        return area;
    } else if (!(cell.isPartOfMerged()) &&
               (cell.mergedXCells() + 1) >= area.width() &&
               (cell.mergedYCells() + 1) >= area.height()) {
        /* if just a single cell is selected, we need to merge even when
        the obscuring isn't forced.  But only if this is the cell that
        is doing the obscuring -- we still want to be able to click on a cell
        that is being obscured.
        */
        area.setWidth(cell.mergedXCells() + 1);
        area.setHeight(cell.mergedYCells() + 1);
    } else {
        int top = area.top();
        int left = area.left();
        int bottom = area.bottom();
        int right = area.right();
        for (int x = area.left(); x <= area.right(); x++)
            for (int y = area.top(); y <= area.bottom(); y++) {
                cell = Cell(d->activeSheet, x, y);
                if (cell.doesMergeCells()) {
                    right = qMax(right, cell.mergedXCells() + x);
                    bottom = qMax(bottom, cell.mergedYCells() + y);
                } else if (cell.isPartOfMerged()) {
                    cell = cell.masterCell();
                    left = qMin(left, cell.column());
                    top = qMin(top, cell.row());
                    bottom = qMax(bottom, cell.row() + cell.mergedYCells());
                    right = qMax(right, cell.column() + cell.mergedXCells());
                }
            }

        area.setCoords(left, top, right, bottom);
    }
    return area;
}

KSpread::Region::Point* Selection::createPoint(const QPoint& point) const
{
    return new Point(point);
}

KSpread::Region::Point* Selection::createPoint(const QString& string) const
{
    return new Point(string);
}

KSpread::Region::Point* Selection::createPoint(const Region::Point& point) const
{
    return new Point(point);
}

KSpread::Region::Range* Selection::createRange(const QRect& rect) const
{
    return new Range(rect);
}

KSpread::Region::Range* Selection::createRange(const KSpread::Region::Point& tl, const KSpread::Region::Point& br) const
{
    return new Range(tl, br);
}

KSpread::Region::Range* Selection::createRange(const QString& string) const
{
    return new Range(string);
}

KSpread::Region::Range* Selection::createRange(const Region::Range& range) const
{
    return new Range(range);
}

void Selection::emitChanged(const Region& region)
{
    Sheet * const sheet = d->activeSheet;
    if(!sheet) // no sheet no update needed
        return;
    Region extendedRegion;
    ConstIterator end(region.constEnd());
    for (ConstIterator it = region.constBegin(); it != end; ++it) {
        Element* element = *it;
        QRect area = element->rect();

        const ColumnFormat *col;
        const RowFormat *rl;
        //look at if column is hiding.
        //if it's hiding refreshing column+1 (or column -1 )
        int left = area.left();
        int right = area.right();
        int top = area.top();
        int bottom = area.bottom();

        // a merged cells is selected
        if (element->type() == Region::Element::Point) {
            Cell cell(sheet, left, top);
            if (cell.doesMergeCells()) {
                // extend to the merged region
                // prevents artefacts of the selection rectangle
                right += cell.mergedXCells();
                bottom += cell.mergedYCells();
            }
        }

        if (right < KS_colMax) {
            do {
                right++;
                col = sheet->columnFormat(right);
            } while (col->isHiddenOrFiltered() && right != KS_colMax);
        }
        if (left > 1) {
            do {
                left--;
                col = sheet->columnFormat(left);
            } while (col->isHiddenOrFiltered() && left != 1);
        }

        if (bottom < KS_rowMax) {
            do {
                bottom++;
                rl = sheet->rowFormat(bottom);
            } while (rl->isHiddenOrFiltered() && bottom != KS_rowMax);
        }

        if (top > 1) {
            do {
                top--;
                rl = sheet->rowFormat(top);
            } while (rl->isHiddenOrFiltered() && top != 1);
        }

        area.setLeft(left);
        area.setRight(right);
        area.setTop(top);
        area.setBottom(bottom);

        extendedRegion.add(area, element->sheet());
    }

    const QList<Cell> masterCells = sheet->cellStorage()->masterCells(extendedRegion);
    for (int i = 0; i < masterCells.count(); ++i)
        extendedRegion.add(masterCells[i].cellPosition(), sheet);

    emit changed(extendedRegion);
}

void Selection::dump() const
{
    kDebug() << *this;
    kDebug() << "d->activeElement:" << d->activeElement;
    kDebug() << "d->activeSubRegionStart:" << d->activeSubRegionStart;
    kDebug() << "d->activeSubRegionLength:" << d->activeSubRegionLength;
}

/***************************************************************************
  class Point
****************************************************************************/

Selection::Point::Point(const QPoint& point)
        : Region::Point(point),
        m_color(Qt::black)
{
}

Selection::Point::Point(const QString& string)
        : Region::Point(string),
        m_color(Qt::black)
{
}

Selection::Point::Point(const Region::Point& point)
        : Region::Point(point),
        m_color(Qt::black)
{
}

/***************************************************************************
  class Range
****************************************************************************/

Selection::Range::Range(const QRect& range)
        : Region::Range(range),
        m_color(Qt::black)
{
}

Selection::Range::Range(const KSpread::Region::Point& tl, const KSpread::Region::Point& br)
        : Region::Range(tl, br),
        m_color(Qt::black)
{
}

Selection::Range::Range(const QString& string)
        : Region::Range(string),
        m_color(Qt::black)
{
}

Selection::Range::Range(const Region::Range& range)
        : Region::Range(range),
        m_color(Qt::black)
{
}

#include "Selection.moc"
