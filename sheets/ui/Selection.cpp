/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>
   SPDX-FileCopyrightText: 2005-2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "Selection.h"

#include <KoCanvasBase.h>
#include <KoCanvasController.h>
#include <KoViewConverter.h>

#include <QWidget>

#include "core/CellStorage.h"
#include "core/ColFormatStorage.h"
#include "core/RowFormatStorage.h"
#include "core/Sheet.h"

using namespace Calligra::Sheets;

// TODO
// - Allow resizing of all ranges in a normal selection; not just the last one.

/***************************************************************************
  class Selection::Private
****************************************************************************/

class Q_DECL_HIDDEN Selection::Private
{
public:
    Private()
    {
        activeSheet = nullptr;
        originSheet = nullptr;
        anchor = QPoint(1, 1);
        cursor = QPoint(1, 1);

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

        canvasBase = nullptr;
        referenceMode = false;
    }

    Sheet *activeSheet;
    Sheet *originSheet;
    QPoint anchor;
    QPoint cursor;
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

    KoCanvasBase *canvasBase;
    bool referenceMode : 1;
    Region formerSelection; // for reference selection mode
    Region oldSelection; // for select all
};

/***************************************************************************
  class Selection
****************************************************************************/

Selection::Selection(KoCanvasBase *canvasBase)
    : KoToolSelection(nullptr)
    , Region(1, 1)
    , d(new Private())
{
    d->canvasBase = canvasBase;
}

Selection::Selection(const Selection &selection)
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

KoCanvasBase *Selection::canvas() const
{
    return d->canvasBase;
}

void Selection::initialize(const QPoint &point, Sheet *sheet)
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

    Region changedRegion = extendRegionToMergedAreas(*this);

    // for the case of a merged cell
    QPoint topLeft(point);
    Cell cell(d->activeSheet, point);
    if (cell.isPartOfMerged()) {
        cell = cell.masterCell();
        topLeft = QPoint(cell.column(), cell.row());
    }

    d->anchor = topLeft;
    d->cursor = point;

    fixSubRegionDimension(); // TODO remove this sanity check
    int index = d->activeSubRegionStart + d->activeSubRegionLength;
    if (insert(index, topLeft, sheet, true, false, false)) {
        // if the point was inserted
        clearSubRegion();
        // Sets:
        // d->activeElement = d->activeSubRegionStart + 1;
        // d->activeSubRegionLength = 0;
    } else {
        warnSheets << "Unable to insert" << topLeft << "in" << sheet->sheetName();
    }
    Element *element = cells()[d->activeSubRegionStart];
    // we end up with one element in the subregion
    d->activeSubRegionLength = 1;
    if (element && element->type() == Element::Point) {
        Point *point = static_cast<Point *>(element);
        point->setColor(d->colors[cells().size() % d->colors.size()]);
    } else if (element && element->type() == Element::Range) {
        Range *range = static_cast<Range *>(element);
        range->setColor(d->colors[cells().size() % d->colors.size()]);
    }

    if (changedRegion == *this) {
        emitChanged(Region(topLeft, sheet));
        return;
    }
    changedRegion.add(topLeft, sheet);

    emitChanged(changedRegion);
}

void Selection::initialize(const QRect &range, Sheet *sheet)
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

    Region changedRegion = extendRegionToMergedAreas(*this);

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

    QPoint origCursor = d->cursor;
    d->anchor = topLeft;
    d->cursor = bottomRight;

    fixSubRegionDimension(); // TODO remove this sanity check
    int index = d->activeSubRegionStart + d->activeSubRegionLength;
    if (insert(index, QRect(topLeft, bottomRight), sheet, true, false, false, false, false)) {
        // if the range was inserted
        clearSubRegion();
        // Sets:
        // d->activeElement = d->activeSubRegionStart + 1;
        // d->activeSubRegionLength = 0;
    } else {
        warnSheets << "Unable to insert" << topLeft << "in" << sheet->sheetName();
    }
    Element *element = cells()[d->activeSubRegionStart];
    // we end up with one element in the subregion
    d->activeSubRegionLength = 1;
    if (element && element->type() == Element::Point) {
        Point *point = static_cast<Point *>(element);
        point->setColor(d->colors[cells().size() % d->colors.size()]);
    } else if (element && element->type() == Element::Range) {
        Range *range = static_cast<Range *>(element);
        range->setColor(d->colors[cells().size() % d->colors.size()]);
    }

    if ((changedRegion == *this) && (origCursor == d->cursor))
        return;

    changedRegion.add(QRect(topLeft, bottomRight), sheet);

    emitChanged(changedRegion);
}

void Selection::initialize(const Region &region, Sheet *sheet)
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

    Region changedRegion = extendRegionToMergedAreas(*this);

    // TODO Stefan: handle subregion insertion
    // TODO Stefan: handle obscured cells correctly
    Region::clear(); // all elements; no residuum
    Element *element = add(region);
    if (element && element->type() == Element::Point) {
        Point *point = static_cast<Point *>(element);
        point->setColor(d->colors[cells().size() % d->colors.size()]);
    } else if (element && element->type() == Element::Range) {
        Range *range = static_cast<Range *>(element);
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

    QPoint origCursor = d->cursor;
    d->anchor = topLeft;
    d->cursor = topLeft;

    d->activeElement = cells().count();
    d->activeSubRegionStart = 0;
    d->activeSubRegionLength = cells().count();

    if ((changedRegion == *this) && (origCursor == d->cursor))
        return;

    changedRegion.add(region);

    emitChanged(changedRegion);
}

void Selection::update()
{
    emitChanged(*this);
}

void Selection::update(const QPoint &point)
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

    SheetBase *basesheet = cells()[d->activeElement]->sheet();
    Sheet *sheet = dynamic_cast<Sheet *>(basesheet);
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

    QRect area1 = cells()[d->activeElement]->rect();
    QRect newRange = extendToMergedAreas(QRect(d->anchor, topLeft), d->activeSheet);

    // If the updated range is bigger, it may cover already existing ranges.
    // These get removed, if multiple occurrences are not allowed. Store the old
    // amount of ranges, to figure out how many ranges have been removed later.
    const int count = cells().count();
    // The update may have shrunk the range, which would be contained in
    // the former range. Remove the latter before inserting the new range.
    delete cells().takeAt(d->activeElement);
    insert(d->activeElement, newRange, sheet, d->multipleOccurences, false, false, false, false);
    const int delta = cells().count() - count;
    d->activeSubRegionLength += delta;
    if (atEnd) {
        d->activeElement = d->activeSubRegionStart + d->activeSubRegionLength;
    } else {
        d->activeElement += delta;
    }

    QRect area2 = newRange;
    Region changedRegion;

    bool newLeft = area1.left() != area2.left();
    bool newTop = area1.top() != area2.top();
    bool newRight = area1.right() != area2.right();
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
        changedRegion.add(QRect(QPoint(farLeft, innerTop), QPoint(innerLeft - 1, innerBottom)));
        if (newTop) {
            changedRegion.add(QRect(QPoint(farLeft, farTop), QPoint(innerLeft - 1, innerTop - 1)));
        }
        if (newBottom) {
            changedRegion.add(QRect(QPoint(farLeft, innerBottom + 1), QPoint(innerLeft - 1, farBottom)));
        }
    }

    if (newTop) {
        changedRegion.add(QRect(QPoint(innerLeft, farTop), QPoint(innerRight, innerTop - 1)));
    }

    if (newRight) {
        changedRegion.add(QRect(QPoint(innerRight + 1, innerTop), QPoint(farRight, innerBottom)));
        if (newTop) {
            changedRegion.add(QRect(QPoint(innerRight + 1, farTop), QPoint(farRight, innerTop - 1)));
        }
        if (newBottom) {
            changedRegion.add(QRect(QPoint(innerRight + 1, innerBottom + 1), QPoint(farRight, farBottom)));
        }
    }

    if (newBottom) {
        changedRegion.add(QRect(QPoint(innerLeft, innerBottom + 1), QPoint(innerRight, farBottom)));
    }

    if (point != d->cursor)
        changedRegion.add(QRect(point, d->cursor));

    d->cursor = point;

    emitChanged(changedRegion);
}

void Selection::extend(const QPoint &point, Sheet *sheet)
{
    if (!isValid(point))
        return;

    if (isEmpty() || d->selectionMode == SingleCell) {
        initialize(point, sheet);
        return;
    }

    debugSheets;

    if (!sheet) {
        if (d->originSheet) {
            sheet = d->originSheet;
        } else {
            sheet = d->activeSheet;
        }
    }

    Region changedRegion = extendRegionToMergedAreas(*this);

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
        insert(index, topLeft, sheet, true, false, false);
        ++d->activeSubRegionLength;
        ++d->activeElement;
        d->anchor = topLeft;
    } else {
        // TODO Replace for normal selection and resizing of any range.
        // The new point may split an existing range. Anyway, the new
        // location/range is appended and the last element becomes active.
        const int count = cells().count();
        eor(topLeft, sheet);
        d->activeSubRegionLength += cells().count() - count;
        d->activeElement = cells().count() - 1;
        d->anchor = cells()[d->activeElement]->rect().topLeft();
    }
    d->cursor = point;

    changedRegion.add(topLeft, sheet);
    changedRegion.add(*this);

    emitChanged(changedRegion);
}

void Selection::extend(const QRect &range, Sheet *sheet)
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

    const QRect newRange = extendToMergedAreas(range, sheet);

    Element *element = nullptr;
    if (d->multipleOccurences) {
        const int subRegionEnd = d->activeSubRegionStart + d->activeSubRegionLength;
        const bool prepend = d->activeSubRegionLength == 0;
        const bool atEnd = d->activeElement == subRegionEnd;
        // Insert the new location after the active element, if possible.
        const int index = d->activeElement + ((prepend || atEnd) ? 0 : 1);
        insert(index, newRange, sheet, true, false, false, false, false);
        ++d->activeSubRegionLength;
        ++d->activeElement;
        d->anchor = newRange.topLeft();
        d->cursor = newRange.bottomRight();
    } else {
        const int count = cells().count();
        element = add(newRange, sheet);
        d->activeSubRegionLength += cells().count() - count;
        d->activeElement = cells().count() - 1;
        d->anchor = cells()[d->activeElement]->rect().topLeft();
        d->cursor = cells()[d->activeElement]->rect().bottomRight();
    }

    if (element && element->type() == Element::Point) {
        Point *point = static_cast<Point *>(element);
        point->setColor(d->colors[cells().size() % d->colors.size()]);
    } else if (element && element->type() == Element::Range) {
        Range *range = static_cast<Range *>(element);
        range->setColor(d->colors[cells().size() % d->colors.size()]);
    }

    emitChanged(*this);
}

void Selection::extend(const Region &region)
{
    if (!region.isValid())
        return;

    uint count = cells().count();
    ConstIterator end(region.constEnd());
    for (ConstIterator it = region.constBegin(); it != end; ++it) {
        Element *element = *it;
        if (!element)
            continue;
        Sheet *sheet = dynamic_cast<Sheet *>(element->sheet());
        if (element->type() == Element::Point) {
            Point *point = static_cast<Point *>(element);
            extend(point->pos(), sheet);
        } else {
            extend(element->rect(), sheet);
        }
    }

    d->activeSubRegionLength += cells().count() - count;

    emitChanged(*this);
}

Selection::Element *Selection::eor(const QPoint &point, SheetBase *sheet)
{
    // The selection always has to contain one location/range at least.
    if (isSingular()) {
        return Region::add(point, sheet);
    }
    return Region::eor(point, sheet);
}

const QPoint &Selection::cursor() const
{
    return d->cursor;
}

bool Selection::isSingular() const
{
    return Region::isSingular();
}

QString Selection::name(Sheet *sheet) const
{
    return Region::name(sheet ? sheet : d->originSheet);
}

void Selection::setActiveSheet(Sheet *sheet)
{
    if (d->activeSheet == sheet) {
        return;
    }
    d->activeSheet = sheet;
    Q_EMIT activeSheetChanged(sheet);
}

Sheet *Selection::activeSheet() const
{
    return d->activeSheet;
}

void Selection::setOriginSheet(Sheet *sheet)
{
    d->originSheet = sheet;
}

Sheet *Selection::originSheet() const
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

Calligra::Sheets::Region::Element *Selection::activeElement() const
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
        debugSheets << "start position" << d->activeSubRegionStart << "exceeds list" << cells().count();
        d->activeSubRegionStart = 0;
        d->activeSubRegionLength = cells().count();
        return;
    }
    if (d->activeSubRegionStart + d->activeSubRegionLength > cells().count()) {
        debugSheets << "subregion (" << d->activeSubRegionStart << ".." << d->activeSubRegionStart + d->activeSubRegionLength << ") exceeds list"
                    << cells().count();
        d->activeSubRegionLength = cells().count() - d->activeSubRegionStart;
        return;
    }
}

bool Selection::isProtected() const
{
    if (!d->activeSheet->isProtected())
        return false;

    // Unprotected singular cell in a protected sheet?
    // TODO - also allow multi-cell selections consisting solely of protected cells ...

    if (isSingular()) {
        const Cell cell = Cell(d->activeSheet, d->cursor);
        if (!cell.isNull()) {
            const Style style = cell.style();
            if (style.notProtected())
                return false;
        }
    }

    return true;
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

    // Set the cursor according to the active element.
    const int subRegionEnd = d->activeSubRegionStart + d->activeSubRegionLength;
    const bool atEnd = d->activeElement == subRegionEnd;
    const int index = qBound(0, d->activeElement - (atEnd ? 1 : 0), cells().count() - 1);
    const QRect range = cells()[index]->rect();
    d->anchor = range.topLeft();
    d->cursor = range.bottomRight();
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

const QList<QColor> &Selection::colors() const
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
    if (originSheet() != activeSheet()) {
        Q_EMIT visibleSheetRequested(originSheet());
    }
    d->referenceMode = false;
    d->multipleOccurences = false;
    // While entering a formula the choose mode is turned on and off.
    // Clear the choice. Otherwise, cell references will stay highlighted.
    if (!isEmpty()) {
        Q_EMIT changed(*this);
        clear(); // all elements; no residuum
    }
    if (saveChanges) {
        initialize(d->formerSelection);
    }
    d->formerSelection.clear();
    // The normal selection does not support the replacements of sub-regions.
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
    Q_EMIT aboutToModify(*this);
}

void Selection::emitModified()
{
    Q_EMIT modified(*this);
}

void Selection::emitRefreshSheetViews()
{
    Q_EMIT refreshSheetViews();
}

void Selection::emitVisibleSheetRequested(Sheet *sheet)
{
    Q_EMIT visibleSheetRequested(sheet);
}

void Selection::emitCloseEditor(bool saveChanges, bool expandMatrix)
{
    Q_EMIT closeEditor(saveChanges, expandMatrix);
}

void Selection::emitRequestFocusEditor()
{
    Q_EMIT requestFocusEditor();
}

QRect Selection::extendToMergedAreas(const QRect &_area, Sheet *sheet) const
{
    if (!sheet)
        return _area;

    QRect area = normalized(_area);
    Cell cell(sheet, area.left(), area.top());

    if (Region::Range(area).isColumn() || Region::Range(area).isRow()) {
        return area;
    } else if (!(cell.isPartOfMerged()) && (cell.mergedXCells() + 1) >= area.width() && (cell.mergedYCells() + 1) >= area.height()) {
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
        // TODO - we only need to check the borders, not inside cells!
        for (int x = area.left(); x <= area.right(); x++)
            for (int y = area.top(); y <= area.bottom(); y++) {
                cell = Cell(sheet, x, y);
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

Region Selection::extendRegionToMergedAreas(const Region &region) const
{
    Region res;
    ConstIterator end(region.constEnd());
    for (ConstIterator it = region.constBegin(); it != end; ++it) {
        Element *element = *it;
        if (!element)
            continue;
        Sheet *sheet = dynamic_cast<Sheet *>(element->sheet());
        const QRect rect = extendToMergedAreas(element->rect(), sheet);
        res.add(rect, element->sheet());
    }
    return res;
}

Calligra::Sheets::Region::Point *Selection::createPoint(const QPoint &point, bool /*fixedColumn*/, bool /*fixedRow*/) const
{
    return new Point(point);
}

Calligra::Sheets::Region::Point *Selection::createPoint(const QString &string) const
{
    return new Point(string);
}

Calligra::Sheets::Region::Point *Selection::createPoint(const Region::Point &point) const
{
    return new Point(point);
}

Calligra::Sheets::Region::Range *
Selection::createRange(const QRect &rect, bool /*fixedTop*/, bool /*fixedLeft*/, bool /*fixedBottom*/, bool /*fixedRight*/) const
{
    return new Range(rect);
}

Calligra::Sheets::Region::Range *Selection::createRange(const Calligra::Sheets::Region::Point &tl, const Calligra::Sheets::Region::Point &br) const
{
    return new Range(tl, br);
}

Calligra::Sheets::Region::Range *Selection::createRange(const QString &string) const
{
    return new Range(string);
}

Calligra::Sheets::Region::Range *Selection::createRange(const Region::Range &range) const
{
    return new Range(range);
}

void Selection::emitChanged(const Region &region)
{
    Sheet *const sheet = d->activeSheet;
    if (!sheet) // no sheet no update needed
        return;
    Region extendedRegion;
    ConstIterator end(region.constEnd());
    for (ConstIterator it = region.constBegin(); it != end; ++it) {
        Element *element = *it;
        QRect area = element->rect();

        // look at if column is hiding.
        // if it's hiding refreshing column+1 (or column -1 )
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
                int lastHidden;
                if (sheet->columnFormats()->isHiddenOrFiltered(right, &lastHidden)) {
                    right = lastHidden;
                } else {
                    break;
                }
            } while (right != KS_colMax);
        }

        if (left > 1) {
            do {
                left--;
                int firstHidden;
                if (sheet->columnFormats()->isHiddenOrFiltered(left, nullptr, &firstHidden)) {
                    left = firstHidden;
                } else {
                    break;
                }
            } while (left != 1);
        }

        if (bottom < KS_rowMax) {
            do {
                bottom++;
                int lastHidden;
                if (sheet->rowFormats()->isHiddenOrFiltered(bottom, &lastHidden)) {
                    bottom = lastHidden;
                } else {
                    break;
                }
            } while (bottom != KS_rowMax);
        }

        if (top > 1) {
            do {
                top--;
                int firstHidden;
                if (sheet->rowFormats()->isHiddenOrFiltered(top, nullptr, &firstHidden)) {
                    top = firstHidden;
                } else {
                    break;
                }
            } while (top != 1);
        }

        area.setLeft(left);
        area.setRight(right);
        area.setTop(top);
        area.setBottom(bottom);

        extendedRegion.add(area, element->sheet());
    }

    const QList<Cell> masterCells = sheet->fullCellStorage()->masterCells(extendedRegion);
    for (int i = 0; i < masterCells.count(); ++i)
        extendedRegion.add(masterCells[i].cellPosition(), sheet);

    Q_EMIT changed(extendedRegion);

    emitModified();
}

void Selection::scrollToCursor()
{
    const QPoint location = cursor();
    Sheet *const sheet = activeSheet();

    // Adjust the maximum accessed column and row for the scrollbars.
    Q_EMIT updateAccessedCellRange(sheet, location);

    // The cell geometry expanded by some pixels in each direction.
    const Cell cell = Cell(sheet, location).masterCell();
    const double xpos = sheet->columnPosition(cell.cellPosition().x());
    const double ypos = sheet->rowPosition(cell.cellPosition().y());
    const double pixelWidth = canvas()->viewConverter()->viewToDocumentX(1);
    const double pixelHeight = canvas()->viewConverter()->viewToDocumentY(1);
    QRectF rect(xpos, ypos, cell.width(), cell.height());
    rect.adjust(-2 * pixelWidth, -2 * pixelHeight, +2 * pixelWidth, +2 * pixelHeight);
    rect = rect & QRectF(QPointF(0.0, 0.0), sheet->documentSize());

    // Scroll to cell.
    canvas()->canvasController()->ensureVisible(canvas()->viewConverter()->documentToView(rect), true);
}

void Selection::dump() const
{
    debugSheets << *this;
    debugSheets << "d->activeElement:" << d->activeElement;
    debugSheets << "d->activeSubRegionStart:" << d->activeSubRegionStart;
    debugSheets << "d->activeSubRegionLength:" << d->activeSubRegionLength;
}

/***************************************************************************
  class Point
****************************************************************************/

Selection::Point::Point(const QPoint &point)
    : Region::Point(point)
    , m_color(Qt::black)
{
}

Selection::Point::Point(const QString &string)
    : Region::Point(string)
    , m_color(Qt::black)
{
}

Selection::Point::Point(const Region::Point &point)
    : Region::Point(point)
    , m_color(Qt::black)
{
}

/***************************************************************************
  class Range
****************************************************************************/

Selection::Range::Range(const QRect &range)
    : Region::Range(range)
    , m_color(Qt::black)
{
}

Selection::Range::Range(const Calligra::Sheets::Region::Point &tl, const Calligra::Sheets::Region::Point &br)
    : Region::Range(tl, br)
    , m_color(Qt::black)
{
}

Selection::Range::Range(const QString &string)
    : Region::Range(string)
    , m_color(Qt::black)
{
}

Selection::Range::Range(const Region::Range &range)
    : Region::Range(range)
    , m_color(Qt::black)
{
}
