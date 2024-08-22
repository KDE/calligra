/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// These are used by the row/col actions in ui/actions, by ui/dialogs/LayoutDialog and ui/commands/PasteCommand, as well as by the interface routines in part/
// and shape/. Hence keeping them here, not in ui/actions

// Local
#include "RowColumnManipulators.h"

#include <QFontMetricsF>
#include <QWidget>
#include <float.h>

#include "core/Cell.h"
#include "core/CellStorage.h"
#include "core/ColFormatStorage.h"
#include "core/RowFormatStorage.h"
#include "core/Sheet.h"
#include "core/Style.h"
#include "engine/Damages.h"
#include "engine/MapBase.h"

using namespace Calligra::Sheets;

/***************************************************************************
  class ResizeColumnManipulator
****************************************************************************/

ResizeColumnManipulator::ResizeColumnManipulator(KUndo2Command *parent)
    : AbstractRegionCommand(parent)
{
    setText(kundo2_i18n("Resize Column"));
}

ResizeColumnManipulator::~ResizeColumnManipulator() = default;

bool ResizeColumnManipulator::performNonCommandActions()
{
    const QList<Element *> elements = cells();
    for (int i = 0; i < elements.count(); ++i) {
        Element *element = elements[i];
        QRect range = element->rect();
        if (m_firstrun) {
            for (int col = range.right(); col >= range.left(); --col) {
                m_oldSizes[col] = m_sheet->columnFormats()->colWidth(col);
            }
        }
        m_sheet->columnFormats()->setColWidth(range.left(), range.right(), qMax(2.0, m_newSize));

        // TODO: more efficiently update positions of cell-anchored shapes
        for (int col = range.left(); col <= range.right(); ++col) {
            qreal delta = m_newSize - m_oldSizes[col];
            m_sheet->adjustCellAnchoredShapesX(delta, col + 1);
        }
    }

    m_sheet->map()->addDamage(new SheetDamage(m_sheet, SheetDamage::ContentChanged));
    return true;
}

bool ResizeColumnManipulator::undoNonCommandActions()
{
    const QList<Element *> elements = cells();
    for (int i = 0; i < elements.count(); ++i) {
        Element *element = elements[i];
        QRect range = element->rect();
        for (int col = range.right(); col >= range.left(); --col) {
            m_sheet->columnFormats()->setColWidth(col, col, m_oldSizes[col]);
        }
        // TODO: more efficiently update positions of cell-anchored shapes
        for (int col = range.left(); col <= range.right(); ++col) {
            qreal delta = -1 * (m_newSize - m_oldSizes[col]);
            m_sheet->adjustCellAnchoredShapesX(delta, col + 1);
        }
    }

    m_sheet->map()->addDamage(new SheetDamage(m_sheet, SheetDamage::ContentChanged | SheetDamage::ColumnsChanged));
    return true;
}

/***************************************************************************
  class ResizeRowManipulator
****************************************************************************/

ResizeRowManipulator::ResizeRowManipulator(KUndo2Command *parent)
    : AbstractRegionCommand(parent)
{
    setText(kundo2_i18n("Resize Row"));
}

ResizeRowManipulator::~ResizeRowManipulator() = default;

bool ResizeRowManipulator::performNonCommandActions()
{
    const QList<Element *> elements = cells();
    for (int i = 0; i < elements.count(); ++i) {
        Element *element = elements[i];
        QRect range = element->rect();
        if (m_firstrun) {
            for (int row = range.bottom(); row >= range.top(); --row) {
                m_oldSizes[row] = m_sheet->rowFormats()->rowHeight(row);
            }
        }
        m_sheet->rowFormats()->setRowHeight(range.top(), range.bottom(), m_newSize);
        // TODO: more efficiently update positions of cell-anchored shapes
        for (int row = range.top(); row <= range.bottom(); ++row) {
            qreal delta = m_newSize - m_oldSizes[row];
            m_sheet->adjustCellAnchoredShapesY(delta, row + 1);
        }
    }

    m_sheet->map()->addDamage(new SheetDamage(m_sheet, SheetDamage::ContentChanged));
    return true;
}

bool ResizeRowManipulator::undoNonCommandActions()
{
    const QList<Element *> elements = cells();
    for (int i = 0; i < elements.count(); ++i) {
        Element *element = elements[i];
        QRect range = element->rect();
        // TODO: more efficiently store old sizes
        if (m_firstrun) {
            for (int row = range.bottom(); row >= range.top(); --row) {
                m_oldSizes[row] = m_sheet->rowFormats()->rowHeight(row);
            }
        }
        for (int row = range.bottom(); row >= range.top(); --row) {
            m_sheet->rowFormats()->setRowHeight(row, row, m_oldSizes[row]);
        }
        // TODO: more efficiently update positions of cell-anchored shapes
        for (int row = range.top(); row <= range.bottom(); ++row) {
            qreal delta = -1 * (m_newSize - m_oldSizes[row]);
            m_sheet->adjustCellAnchoredShapesY(delta, row + 1);
        }
    }

    m_sheet->map()->addDamage(new SheetDamage(m_sheet, SheetDamage::ContentChanged | SheetDamage::RowsChanged));
    return true;
}

/***************************************************************************
  class HideShowManipulator
****************************************************************************/

HideShowManipulator::HideShowManipulator(KUndo2Command *parent)
    : AbstractRegionCommand(parent)
    , m_manipulateColumns(false)
    , m_manipulateRows(false)
    , m_hide(false)
{
}

HideShowManipulator::~HideShowManipulator() = default;

void HideShowManipulator::setHide(bool hide)
{
    m_hide = hide;
    setText(name(hide));
}

void HideShowManipulator::performActions(bool hide)
{
    if (m_firstrun)
        oldHidden.clear();

    const QList<Element *> elements = cells();
    for (int i = 0; i < elements.count(); ++i) {
        QRect range = elements[i]->rect();

        if (m_manipulateColumns) {
            ColFormatStorage *cf = m_sheet->columnFormats();
            if (m_firstrun)
                for (int i = range.left(); i <= range.right(); ++i)
                    oldHidden[i] = cf->isHidden(i);
            cf->setHidden(range.left(), range.right(), hide);
            qreal delta = cf->totalColWidth(range.left(), range.right());
            if (hide)
                delta = -delta;
            m_sheet->adjustCellAnchoredShapesX(delta, range.left());
        }
        if (m_manipulateRows) {
            RowFormatStorage *rf = m_sheet->rowFormats();
            if (m_firstrun)
                for (int i = range.top(); i <= range.bottom(); ++i)
                    oldHidden[i] = rf->isHidden(i);
            rf->setHidden(range.top(), range.bottom(), hide);
            qreal delta = rf->totalRowHeight(range.top(), range.bottom());
            if (hide)
                delta = -delta;
            m_sheet->adjustCellAnchoredShapesY(delta, range.top());
        }
    }

    // Just repaint everything visible; no need to invalidate the visual cache.
    SheetDamage::Changes changes = SheetDamage::ContentChanged;
    if (m_manipulateColumns)
        changes |= SheetDamage::ColumnsChanged;
    if (m_manipulateRows)
        changes |= SheetDamage::RowsChanged;
    m_sheet->map()->addDamage(new SheetDamage(m_sheet, changes));

    setText(name(hide));
}

void HideShowManipulator::undoActions()
{
    const QList<Element *> elements = cells();
    for (int i = 0; i < elements.count(); ++i) {
        QRect range = elements[i]->rect();

        if (m_manipulateColumns) {
            ColFormatStorage *cf = m_sheet->columnFormats();
            qreal orig = cf->totalColWidth(range.left(), range.right());
            for (int i = range.left(); i <= range.right(); ++i)
                cf->setHidden(i, i, oldHidden[i]);
            qreal neww = cf->totalColWidth(range.left(), range.right());
            m_sheet->adjustCellAnchoredShapesX(neww - orig, range.left());
        }
        if (m_manipulateRows) {
            RowFormatStorage *rf = m_sheet->rowFormats();
            qreal orig = rf->totalRowHeight(range.top(), range.bottom());
            for (int i = range.top(); i <= range.bottom(); ++i)
                rf->setHidden(i, i, oldHidden[i]);
            qreal newh = rf->totalRowHeight(range.top(), range.bottom());
            m_sheet->adjustCellAnchoredShapesY(newh - orig, range.top());
        }
    }

    // Just repaint everything visible; no need to invalidate the visual cache.
    SheetDamage::Changes changes = SheetDamage::ContentChanged;
    if (m_manipulateColumns)
        changes |= SheetDamage::ColumnsChanged;
    if (m_manipulateRows)
        changes |= SheetDamage::RowsChanged;
    m_sheet->map()->addDamage(new SheetDamage(m_sheet, changes));
}

bool HideShowManipulator::performNonCommandActions()
{
    performActions(m_hide);
    return true;
}

bool HideShowManipulator::undoNonCommandActions()
{
    undoActions();
    return true;
}

bool HideShowManipulator::preProcess()
{
    Region region;
    ConstIterator endOfList = cells().constEnd();
    for (ConstIterator it = cells().constBegin(); it != endOfList; ++it) {
        QRect range = (*it)->rect();
        // too big if we want to hide everything
        if (m_manipulateColumns) {
            if (range.right() >= KS_colMax)
                return false;
        } else {
            if (range.bottom() >= KS_rowMax)
                return false;
        }
    }

    return true;
}

KUndo2MagicString HideShowManipulator::name(bool hide) const
{
    if (hide) {
        if (m_manipulateColumns && m_manipulateRows)
            return kundo2_i18n("Hide Rows/Columns");
        else if (m_manipulateColumns)
            return kundo2_i18n("Hide Columns");
        else if (m_manipulateRows)
            return kundo2_i18n("Hide Rows");
    } else {
        if (m_manipulateColumns && m_manipulateRows)
            return kundo2_i18n("Show Rows/Columns");
        else if (m_manipulateColumns)
            return kundo2_i18n("Show Columns");
        else if (m_manipulateRows)
            return kundo2_i18n("Show Rows");
    }

    return kundo2_noi18n("XXX: bug!");
}

/***************************************************************************
  class AdjustColumnRowManipulator
****************************************************************************/

AdjustColumnRowManipulator::AdjustColumnRowManipulator(KUndo2Command *parent)
    : AbstractRegionCommand(parent)
    , m_adjustColumn(false)
    , m_adjustRow(false)
{
}

AdjustColumnRowManipulator::~AdjustColumnRowManipulator() = default;

bool AdjustColumnRowManipulator::performNonCommandActions()
{
    QRect used = m_sheet->usedArea();
    const QList<Element *> elements = cells();
    for (int i = 0; i < elements.count(); ++i) {
        QRect range = elements[i]->rect();
        if (elements[i]->sheet() != m_sheet)
            continue;

        if (m_adjustColumn) {
            int min = qMin(range.left(), used.right());
            int max = qMin(range.right(), used.right());
            if (m_firstrun) {
                for (int col = min; col <= max; ++col) {
                    m_oldWidths[col] = m_sheet->columnFormats()->colWidth(col);
                    m_newWidths[col] = idealColumnWidth(col);
                }
            }
            setWidths(min, max, m_newWidths);
        }

        if (m_adjustRow) {
            int min = qMin(range.top(), used.bottom());
            int max = qMin(range.bottom(), used.bottom());
            if (m_firstrun) {
                for (int row = min; row <= max; ++row) {
                    m_oldHeights[row] = m_sheet->rowFormats()->rowHeight(row);
                    m_newHeights[row] = idealRowHeight(row);
                }
            }
            setHeights(min, max, m_newHeights);
        }
    }

    return true;
}

bool AdjustColumnRowManipulator::undoNonCommandActions()
{
    QRect used = m_sheet->usedArea();
    const QList<Element *> elements = cells();
    for (int i = 0; i < elements.count(); ++i) {
        QRect range = elements[i]->rect();
        if (elements[i]->sheet() != m_sheet)
            continue;

        if (m_adjustColumn) {
            int min = qMin(range.left(), used.right());
            int max = qMin(range.right(), used.right());
            setWidths(min, max, m_oldWidths);
        }
        if (m_adjustRow) {
            int min = qMin(range.top(), used.bottom());
            int max = qMin(range.bottom(), used.bottom());
            setHeights(min, max, m_oldHeights);
        }
    }

    return true;
}

void AdjustColumnRowManipulator::setHeights(int from, int to, QMap<int, double> &heights)
{
    for (int row = from; row <= to; ++row) {
        if (heights.contains(row) && heights[row] > -1.0) {
            m_sheet->rowFormats()->setRowHeight(row, row, heights[row]);
        }
    }

    // Update the column/row header, if necessary.
    m_sheet->map()->addDamage(new SheetDamage(m_sheet, SheetDamage::ContentChanged | SheetDamage::RowsChanged));
}

void AdjustColumnRowManipulator::setWidths(int from, int to, QMap<int, double> &widths)
{
    for (int col = from; col <= to; ++col) {
        if (widths.contains(col) && widths[col] > -1.0) {
            double w = m_sheet->columnFormats()->colWidth(col);
            if (qAbs(w - widths[col]) > DBL_EPSILON) {
                m_sheet->columnFormats()->setColWidth(col, col, qMax(2.0, widths[col]));
            }
        }
    }

    // Update the column/row header, if necessary.
    m_sheet->map()->addDamage(new SheetDamage(m_sheet, SheetDamage::ContentChanged | SheetDamage::ColumnsChanged));
}

class DummyWidget : public QWidget
{
    int metric(PaintDeviceMetric metric) const override
    {
        switch (metric) {
        case QPaintDevice::PdmDpiX:
        case QPaintDevice::PdmDpiY:
        case QPaintDevice::PdmPhysicalDpiX:
        case QPaintDevice::PdmPhysicalDpiY:
            return 72;
        default:
            break;
        }
        return QWidget::metric(metric);
    }
};

QSizeF AdjustColumnRowManipulator::textSize(const QString &text, const Style &style) const
{
    QSizeF size;
    DummyWidget dummyWiget;
    const QFontMetricsF fontMetrics(style.font(), &dummyWiget);

    if (style.verticalText()) {
        // Vertical text.
        qreal width = 0.0;
        for (int i = 0; i < text.length(); i++)
            width = qMax(width, fontMetrics.boundingRect(text.at(i)).width());

        size.setWidth(width);
        size.setHeight((fontMetrics.ascent() + fontMetrics.descent()) * text.length());

        return size;
    }

    size = fontMetrics.size(0, text);

    if ((style.valign() == Style::Bottom) && style.underline()) {
        double offsetFont = fontMetrics.underlinePos() + 1;
        size.setHeight(size.height() + offsetFont);
    }

    if (style.angle()) {
        double angle_rad = style.angle() * M_PI / 180;
        // Rotated text.
        size.setHeight(size.height() * ::cos(angle_rad) + qAbs(size.width() * ::sin(angle_rad)));
        size.setWidth(qAbs(size.height() * ::sin(angle_rad)) + size.width() * ::cos(angle_rad));
    }

    return size;
}

double AdjustColumnRowManipulator::idealColumnWidth(int col)
{
    CellStorage *cs = m_sheet->fullCellStorage();
    double width = -1.0;

    Cell cell = cs->firstInColumn(col);
    while (!cell.isNull()) {
        int row = cell.row();
        if (!cell.isEmpty() && !cell.isPartOfMerged()) {
            width = qMax(width, adjustColumnHelper(cell));
        }
        cell = cs->nextInColumn(col, row);
    }

    return width;
}

double AdjustColumnRowManipulator::idealRowHeight(int row)
{
    CellStorage *cs = m_sheet->fullCellStorage();
    double height = -1.0;

    Cell cell = cs->firstInRow(row);
    while (!cell.isNull()) {
        if (!cell.isEmpty() && !cell.isPartOfMerged()) {
            height = qMax(height, adjustRowHelper(cell));
        }
        cell = cs->nextInRow(cell.column(), row);
    }

    return height;
}

double AdjustColumnRowManipulator::adjustColumnHelper(const Cell &cell)
{
    double long_max = 0.0;
    const Style style = cell.effectiveStyle();
    const QSizeF size = textSize(cell.displayText(), style);
    if (size.width() > long_max) {
        double indent = 0.0;
        Style::HAlign alignment = style.halign();
        if (alignment == Style::HAlignUndefined) {
            if (cell.value().isNumber() || cell.isDate() || cell.isTime())
                alignment = Style::Right;
            else
                alignment = Style::Left;
        }
        if (alignment == Style::Left)
            indent = cell.style().indentation();
        long_max = indent + size.width() + style.leftBorderPen().width() + style.rightBorderPen().width();
        // if this cell has others merged into it, we'll subtract the width of those columns
        // this is not perfect, but at least should work in 90% of the cases
        const int mergedXCount = cell.mergedXCells();
        if (mergedXCount > 0) {
            double cw = cell.fullSheet()->columnFormats()->totalVisibleColWidth(cell.column() + 1, cell.column() + mergedXCount);
            long_max -= cw;
        }
    }
    // add 4 because long_max is the length of the text
    // but column has borders
    if (long_max == 0.0)
        return -1.0;
    else
        return long_max + 4.0;
}

double AdjustColumnRowManipulator::adjustRowHelper(const Cell &cell)
{
    double long_max = 0.0;
    const Style style = cell.effectiveStyle();
    const QSizeF size = textSize(cell.displayText(), style);
    if (size.height() > long_max)
        long_max = size.height() + style.topBorderPen().width() + style.bottomBorderPen().width();
    //  add 1 because long_max is the height of the text
    //  but row has borders
    if (long_max == 0.0)
        return -1.0;
    else
        return long_max + 1.0;
}

KUndo2MagicString AdjustColumnRowManipulator::name() const
{
    if (m_adjustColumn && m_adjustRow) {
        return kundo2_i18n("Adjust Columns/Rows");
    } else if (m_adjustColumn) {
        return kundo2_i18n("Adjust Columns");
    } else {
        return kundo2_i18n("Adjust Rows");
    }
}

/***************************************************************************
  class InsertDeleteColumnManipulator
****************************************************************************/

InsertDeleteColumnManipulator::InsertDeleteColumnManipulator(KUndo2Command *parent)
    : AbstractRegionCommand(parent)
    , m_mode(Insert)
    , m_template(nullptr)
{
    setText(kundo2_i18n("Insert Columns"));
}

InsertDeleteColumnManipulator::~InsertDeleteColumnManipulator()
{
    delete m_template;
}

void InsertDeleteColumnManipulator::setTemplate(const ColFormat &columnFormat)
{
    delete m_template;
    m_template = new ColFormat(columnFormat);
}

void InsertDeleteColumnManipulator::setDelete(bool deletion)
{
    m_mode = deletion ? Delete : Insert;
    if (!deletion)
        setText(kundo2_i18n("Insert Columns"));
    else
        setText(kundo2_i18n("Remove Columns"));
}

bool InsertDeleteColumnManipulator::process(Element *element)
{
    const QRect range = element->rect();
    const int pos = range.left();
    const int num = range.width();
    if (m_mode == Insert) {
        m_sheet->insertColumns(pos, num);
        if (m_template) {
            const int end = pos + num - 1;
            m_sheet->columnFormats()->setColFormat(pos, end, *m_template);
        }
        m_sheet->cellStorage()->insertColumns(pos, num);
    } else {
        m_sheet->cellStorage()->removeColumns(pos, num);
        m_sheet->removeColumns(pos, num);
    }
    return true;
}

bool InsertDeleteColumnManipulator::undoNonCommandActions()
{
    Mode orig_mode = m_mode;
    // Revert the mode and run the commands. This will get the columns to where they need to be.
    // Then, the parent class will call undo actions to restore the contents.
    m_mode = (m_mode == Insert) ? Delete : Insert;
    performCommands();
    m_mode = orig_mode;
    return true;
}

bool elementLeftColumnLessThan(const Calligra::Sheets::Region::Element *e1, const Calligra::Sheets::Region::Element *e2)
{
    return e1->rect().left() < e2->rect().left();
}

bool InsertDeleteColumnManipulator::preProcess()
{
    // Need these ordered.
    if (m_firstrun)
        std::stable_sort(cells().begin(), cells().end(), elementLeftColumnLessThan);
    return true;
}

/***************************************************************************
  class InsertDeleteRowManipulator
****************************************************************************/

InsertDeleteRowManipulator::InsertDeleteRowManipulator(KUndo2Command *parent)
    : AbstractRegionCommand(parent)
    , m_mode(Insert)
    , m_template(nullptr)
{
    setText(kundo2_i18n("Insert Rows"));
}

InsertDeleteRowManipulator::~InsertDeleteRowManipulator()
{
    delete m_template;
}

void InsertDeleteRowManipulator::setTemplate(const RowFormat &rowFormat)
{
    delete m_template;
    m_template = new RowFormat(rowFormat);
}

void InsertDeleteRowManipulator::setDelete(bool deletion)
{
    m_mode = deletion ? Delete : Insert;
    if (!deletion)
        setText(kundo2_i18n("Insert Rows"));
    else
        setText(kundo2_i18n("Remove Rows"));
}

bool InsertDeleteRowManipulator::process(Element *element)
{
    const QRect range = element->rect();
    const int pos = range.top();
    const int num = range.height();
    if (m_mode == Insert) {
        m_sheet->insertRows(pos, num);
        if (m_template) {
            const int end = pos + num - 1;
            m_sheet->rowFormats()->setRowFormat(pos, end, *m_template);
        }
        m_sheet->cellStorage()->insertRows(pos, num);
    } else {
        // delete rows
        m_sheet->removeRows(pos, num);
        m_sheet->cellStorage()->removeRows(pos, num);
    }
    return true;
}

bool InsertDeleteRowManipulator::undoNonCommandActions()
{
    Mode orig_mode = m_mode;
    // Revert the mode and run the commands. This will get the columns to where they need to be.
    // Then, the parent class will call undo actions to restore the contents.
    m_mode = (m_mode == Insert) ? Delete : Insert;
    performCommands();
    m_mode = orig_mode;
    return true;
}

bool elementTopRowLessThan(const Calligra::Sheets::Region::Element *e1, const Calligra::Sheets::Region::Element *e2)
{
    return e1->rect().top() < e2->rect().top();
}

bool InsertDeleteRowManipulator::preProcess()
{
    // Need these ordered.
    if (m_firstrun)
        std::stable_sort(cells().begin(), cells().end(), elementTopRowLessThan);
    return true;
}
