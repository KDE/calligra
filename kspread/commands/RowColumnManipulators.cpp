/* This file is part of the KDE project
   Copyright (C) 2005 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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
#include "RowColumnManipulators.h"

#include <float.h>

#include <QFontMetricsF>
#include <QWidget>
#include <QPen>

#include <klocale.h>

#include "CellStorage.h"
#include "Damages.h"
#include "Map.h"
#include "RowColumnFormat.h"
#include "Sheet.h"
#include "Value.h"

using namespace KSpread;

/***************************************************************************
  class ResizeColumnManipulator
****************************************************************************/

ResizeColumnManipulator::ResizeColumnManipulator(QUndoCommand* parent)
        : AbstractRegionCommand(parent)
{
    setText(i18n("Resize Column"));
}

ResizeColumnManipulator::~ResizeColumnManipulator()
{
}

bool ResizeColumnManipulator::process(Element* element)
{
    QRect range = element->rect();
    for (int col = range.right(); col >= range.left(); --col) {
        ColumnFormat *format = m_sheet->nonDefaultColumnFormat(col);
        if (m_firstrun)
            m_oldSizes[col] = format->width();
        format->setWidth(qMax(2.0, m_reverse ? m_oldSizes[col] : m_newSize));
    }
    // Just repaint everything visible; no need to invalidate the visual cache.
    m_sheet->map()->addDamage(new SheetDamage(m_sheet, SheetDamage::ContentChanged));
    return true;
}



/***************************************************************************
  class ResizeRowManipulator
****************************************************************************/

ResizeRowManipulator::ResizeRowManipulator(QUndoCommand* parent)
        : AbstractRegionCommand(parent)
{
    setText(i18n("Resize Row"));
}

ResizeRowManipulator::~ResizeRowManipulator()
{
}

bool ResizeRowManipulator::process(Element* element)
{
    QRect range = element->rect();
    for (int row = range.bottom(); row >= range.top(); --row) {
        RowFormat* rl = m_sheet->nonDefaultRowFormat(row);
        if (m_firstrun)
            m_oldSizes[row] = rl->height();
        rl->setHeight(qMax(2.0, m_reverse ? m_oldSizes[row] : m_newSize));
    }
    // Just repaint everything visible; no need to invalidate the visual cache.
    m_sheet->map()->addDamage(new SheetDamage(m_sheet, SheetDamage::ContentChanged));
    return true;
}


/***************************************************************************
  class HideShowManipulator
****************************************************************************/

HideShowManipulator::HideShowManipulator()
        : m_manipulateColumns(false),
        m_manipulateRows(false)
{
}

HideShowManipulator::~HideShowManipulator()
{
}

bool HideShowManipulator::process(Element* element)
{
    QRect range = element->rect();
    if (m_manipulateColumns) {
        for (int col = range.left(); col <= range.right(); ++col) {
            ColumnFormat* format = m_sheet->nonDefaultColumnFormat(col);
            format->setHidden(!m_reverse);
        }
    }
    if (m_manipulateRows) {
        for (int row = range.top(); row <= range.bottom(); ++row) {
            RowFormat* format = m_sheet->nonDefaultRowFormat(row);
            format->setHidden(!m_reverse);
        }
    }
    return true;
}

bool HideShowManipulator::preProcessing()
{
    if (m_firstrun)
        setText(name());
    Region region;
    ConstIterator endOfList = cells().constEnd();
    for (ConstIterator it = cells().constBegin(); it != endOfList; ++it) {
        if (m_reverse) {
            QRect range = (*it)->rect();
            if (m_manipulateColumns) {
                if (range.left() > 1) {
                    int col;
                    for (col = 1; col < range.left(); ++col) {
                        if (!m_sheet->columnFormat(col)->isHidden())
                            break;
                    }
                    if (col == range.left()) {
                        region.add(QRect(1, 1, range.left() - 1, KS_rowMax));
                    }
                }
                for (int col = range.left(); col <= range.right(); ++col) {
                    if (m_sheet->columnFormat(col)->isHidden()) {
                        region.add(QRect(col, 1, 1, KS_rowMax));
                    }
                }
            }
            if (m_manipulateRows) {
                if (range.top() > 1) {
                    int row;
                    for (row = 1; row < range.top(); ++row) {
                        const RowFormat* format = m_sheet->rowFormat(row);
                        if (!format->isHidden()) {
                            break;
                        }
                    }
                    if (row == range.top()) {
                        region.add(QRect(1, 1, KS_colMax, range.top() - 1));
                    }
                }
                for (int row = range.top(); row <= range.bottom(); ++row) {
                    const RowFormat* format = m_sheet->rowFormat(row);
                    if (format->isHidden()) {
                        region.add(QRect(1, row, KS_colMax, 1));
                    }
                }
            }
        }

        if (((*it)->isRow() && m_manipulateColumns) ||
                ((*it)->isColumn() && m_manipulateRows)) {
            /*      KMessageBox::error( this, i18n( "Area is too large." ) );*/
            return false;
        }
    }

    if (m_reverse) {
        clear();
        add(region);
    }

    return AbstractRegionCommand::preProcessing();
}

bool HideShowManipulator::postProcessing()
{
    // Just repaint everything visible; no need to invalidate the visual cache.
    m_sheet->map()->addDamage(new SheetDamage(m_sheet, SheetDamage::ContentChanged));
    return true;
}

QString HideShowManipulator::name() const
{
    QString name;
    if (m_reverse) {
        name = "Show ";
    } else {
        name = "Hide ";
    }
    if (m_manipulateColumns) {
        name += "Columns";
    }
    if (m_manipulateColumns && m_manipulateRows) {
        name += '/';
    }
    if (m_manipulateRows) {
        name += "Rows";
    }
    return name;
}

/***************************************************************************
  class AdjustColumnRowManipulator
****************************************************************************/

AdjustColumnRowManipulator::AdjustColumnRowManipulator(QUndoCommand* parent)
        : AbstractRegionCommand(parent),
        m_adjustColumn(false),
        m_adjustRow(false)
{
}

AdjustColumnRowManipulator::~AdjustColumnRowManipulator()
{
}

bool AdjustColumnRowManipulator::process(Element* element)
{
    Sheet* sheet = m_sheet; // TODO Stefan: element->sheet();
    if (m_sheet && sheet != m_sheet) {
        return true;
    }

    QMap<int, double> heights;
    QMap<int, double> widths;
    if (m_reverse) {
        heights = m_oldHeights;
        widths = m_oldWidths;
    } else {
        heights = m_newHeights;
        widths = m_newWidths;
    }

    QRect range = element->rect();
    if (m_adjustColumn) {
        if (element->isRow()) {
            for (int row = range.top(); row <= range.bottom(); ++row) {
                Cell cell = sheet->cellStorage()->firstInRow(row);
                while (!cell.isNull()) {
                    int col = cell.column();
                    if (!cell.isEmpty() && !cell.isPartOfMerged()) {
                        if (widths.contains(col) && widths[col] != -1.0) {
                            ColumnFormat* format = sheet->nonDefaultColumnFormat(col);
                            if (qAbs(format->width() - widths[col]) > DBL_EPSILON) {
                                format->setWidth(qMax(2.0, widths[col]));
                            }
                        }
                    }
                    cell = sheet->cellStorage()->nextInRow(col, row);
                }
            }
        } else {
            for (int col = range.left(); col <= range.right(); ++col) {
                if (widths.contains(col) && widths[col] != -1.0) {
                    ColumnFormat* format = sheet->nonDefaultColumnFormat(col);
                    if (qAbs(format->width() - widths[col]) > DBL_EPSILON) {
                        format->setWidth(qMax(2.0, widths[col]));
                    }
                }
            }
        }
    }
    if (m_adjustRow) {
        if (element->isColumn()) {
            for (int col = range.left(); col <= range.right(); ++col) {
                Cell cell = sheet->cellStorage()->firstInColumn(col);
                while (!cell.isNull()) {
                    int row = cell.row();
                    if (!cell.isEmpty() && !cell.isPartOfMerged()) {
                        if (heights.contains(row) && heights[row] != -1.0) {
                            RowFormat* format = sheet->nonDefaultRowFormat(row);
                            if (qAbs(format->height() - heights[row]) > DBL_EPSILON) {
                                format->setHeight(qMax(2.0, heights[row]));
                            }
                        }
                    }
                    cell = sheet->cellStorage()->nextInColumn(col, row);
                }
            }
        } else {
            for (int row = range.top(); row <= range.bottom(); ++row) {
                if (heights.contains(row) && heights[row] != -1.0) {
                    RowFormat* format = sheet->nonDefaultRowFormat(row);
                    if (qAbs(format->height() - heights[row]) > DBL_EPSILON) {
                        format->setHeight(qMax(2.0, heights[row]));
                    }
                }
            }
        }
    }
    // The cell width(s) or height(s) changed, which are cached: rebuild them.
    const Region region(m_adjustRow ? 1 : range.left(),
                        m_adjustColumn ? 1 : range.top(),
                        m_adjustRow ? KS_colMax : range.width(),
                        m_adjustColumn ? KS_rowMax : range.height());
    m_sheet->map()->addDamage(new CellDamage(m_sheet, region, CellDamage::Appearance));
    return true;
}

bool AdjustColumnRowManipulator::preProcessing()
{
    if (m_firstrun)
        setText(name());
    if (m_reverse) {
    } else {
        if (!m_newHeights.isEmpty() || !m_newWidths.isEmpty()) {
            return AbstractRegionCommand::preProcessing();
        }
//     createUndo();

        ConstIterator endOfList(cells().constEnd());
        for (ConstIterator it = cells().constBegin(); it != endOfList; ++it) {
            Element* element = *it;
            QRect range = element->rect();
            if (element->isColumn()) {
                for (int col = range.left(); col <= range.right(); ++col) {
                    Cell cell = m_sheet->cellStorage()->firstInColumn(col);
                    while (!cell.isNull()) {
                        int row = cell.row();
                        if (m_adjustColumn) {
                            if (!m_newWidths.contains(col)) {
                                m_newWidths[col] = -1.0;
                                m_oldWidths[col] = m_sheet->columnFormat(col)->width();
                            }
                            if (!cell.isEmpty() && !cell.isPartOfMerged()) {
                                m_newWidths[col] = qMax(adjustColumnHelper(cell),
                                                        m_newWidths[col]);
                            }
                        }
                        if (m_adjustRow) {
                            if (!m_newHeights.contains(row)) {
                                m_newHeights[row] = -1.0;
                                const RowFormat* format = m_sheet->rowFormat(row);
                                m_oldHeights[row] = format->height();
                            }
                            if (!cell.isEmpty() && !cell.isPartOfMerged()) {
                                m_newHeights[row] = qMax(adjustRowHelper(cell),
                                                         m_newHeights[row]);
                            }
                        }
                        cell = m_sheet->cellStorage()->nextInColumn(col, row);
                    }
                }
            } else if (element->isRow()) {
                for (int row = range.top(); row <= range.bottom(); ++row) {
                    Cell cell = m_sheet->cellStorage()->firstInRow(row);
                    while (!cell.isNull()) {
                        int col = cell.column();
                        if (m_adjustColumn) {
                            if (!m_newWidths.contains(col)) {
                                m_newWidths[col] = -1.0;
                                m_oldWidths[col] = m_sheet->columnFormat(col)->width();
                            }
                            if (!cell.isEmpty() && !cell.isPartOfMerged()) {
                                m_newWidths[col] = qMax(adjustColumnHelper(cell),
                                                        m_newWidths[col]);
                            }
                        }
                        if (m_adjustRow) {
                            if (!m_newHeights.contains(row)) {
                                m_newHeights[row] = -1.0;
                                const RowFormat* format = m_sheet->rowFormat(row);
                                m_oldHeights[row] = format->height();
                            }
                            if (!cell.isEmpty() && !cell.isPartOfMerged()) {
                                m_newHeights[row] = qMax(adjustRowHelper(cell),
                                                         m_newHeights[row]);
                            }
                        }
                        cell = m_sheet->cellStorage()->nextInRow(col, row);
                    }
                }
            } else {
                Cell cell;
                for (int col = range.left(); col <= range.right(); ++col) {
                    for (int row = range.top(); row <= range.bottom(); ++row) {
                        cell = Cell(m_sheet,  col, row);
                        if (m_adjustColumn) {
                            if (!m_newWidths.contains(col)) {
                                m_newWidths[col] = -1.0;
                                m_oldWidths[col] = m_sheet->columnFormat(col)->width();
                            }
                            if (!cell.isEmpty() && !cell.isPartOfMerged()) {
                                m_newWidths[col] = qMax(adjustColumnHelper(cell),
                                                        m_newWidths[col]);
                            }
                        }
                        if (m_adjustRow) {
                            if (!m_newHeights.contains(row)) {
                                m_newHeights[row] = -1.0;
                                const RowFormat* format = m_sheet->rowFormat(row);
                                m_oldHeights[row] = format->height();
                            }
                            if (!cell.isEmpty() && !cell.isPartOfMerged()) {
                                m_newHeights[row] = qMax(adjustRowHelper(cell),
                                                         m_newHeights[row]);
                            }
                        }
                    }
                }
            }
        }
    }
    return AbstractRegionCommand::preProcessing();
}

bool AdjustColumnRowManipulator::postProcessing()
{
    if (!m_adjustColumn && !m_adjustRow) {
        return false;
    }
    // Update the column/row header, if necessary.
    SheetDamage::Changes changes = SheetDamage::None;
    if (m_adjustColumn) {
        changes |= SheetDamage::ColumnsChanged;
    }
    if (m_adjustRow) {
        changes |= SheetDamage::RowsChanged;
    }
    m_sheet->map()->addDamage(new SheetDamage(m_sheet, changes));
    return AbstractRegionCommand::postProcessing();
}

class DummyWidget : public QWidget
{
    int metric(PaintDeviceMetric metric) const {
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

QSizeF AdjustColumnRowManipulator::textSize(const QString& text, const Style& style) const
{
    QSizeF size;
    DummyWidget dummyWiget;
    const QFontMetricsF fontMetrics(style.font(), &dummyWiget);

    // Set size to correct values according to
    // if the text is horizontal, vertical or rotated.
    if (!style.verticalText() && !style.angle()) {
        // Horizontal text.

        size = fontMetrics.size(0, text);
        double offsetFont = 0.0;
        if ((style.valign() == Style::Bottom) && style.underline())
            offsetFont = fontMetrics.underlinePos() + 1;

        size.setHeight((fontMetrics.ascent() + fontMetrics.descent() + offsetFont)
                       *(text.count('\n') + 1));
    } else if (style.angle() != 0) {
        // Rotated text.

        const double height = fontMetrics.ascent() + fontMetrics.descent();
        const double width  = fontMetrics.width(text);
        size.setHeight(height * ::cos(style.angle() * M_PI / 180)
                       + qAbs(width * ::sin(style.angle() * M_PI / 180)));
        size.setWidth(qAbs(height * ::sin(style.angle() * M_PI / 180))
                      + width * ::cos(style.angle() * M_PI / 180));
    } else {
        // Vertical text.

        qreal width = 0.0;
        for (int i = 0; i < text.length(); i++)
            width = qMax(width, fontMetrics.width(text.at(i)));

        size.setWidth(width);
        size.setHeight((fontMetrics.ascent() + fontMetrics.descent())
                       * text.length());
    }
    return size;
}

double AdjustColumnRowManipulator::adjustColumnHelper(const Cell& cell)
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
        long_max = indent + size.width()
                   + style.leftBorderPen().width() + style.rightBorderPen().width();
    }
    // add 4 because long_max is the length of the text
    // but column has borders
    if (long_max == 0.0)
        return -1.0;
    else
        return long_max + 4.0;
}

double AdjustColumnRowManipulator::adjustRowHelper(const Cell& cell)
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

QString AdjustColumnRowManipulator::name() const
{
    if (m_adjustColumn && m_adjustRow) {
        return i18n("Adjust Columns/Rows");
    } else if (m_adjustColumn) {
        return i18n("Adjust Columns");
    } else {
        return i18n("Adjust Rows");
    }
}

/***************************************************************************
  class InsertDeleteColumnManipulator
****************************************************************************/

InsertDeleteColumnManipulator::InsertDeleteColumnManipulator(QUndoCommand *parent)
        : AbstractRegionCommand(parent)
        , m_mode(Insert)
        , m_template(0)
{
    setText(i18n("Insert Columns"));
}

InsertDeleteColumnManipulator::~InsertDeleteColumnManipulator()
{
    delete m_template;
}

void InsertDeleteColumnManipulator::setTemplate(const ColumnFormat &columnFormat)
{
    delete m_template;
    m_template = new ColumnFormat(columnFormat);
}

void InsertDeleteColumnManipulator::setReverse(bool reverse)
{
    m_reverse = reverse;
    m_mode = reverse ? Delete : Insert;
    if (!m_reverse)
        setText(i18n("Insert Columns"));
    else
        setText(i18n("Remove Columns"));
}

bool InsertDeleteColumnManipulator::process(Element* element)
{
    const QRect range = element->rect();
    const int pos = range.left();
    const int num = range.width();
    if (!m_reverse) { // insertion
        // insert rows
        m_sheet->insertColumns(pos, num);
        if (m_template) {
            m_template->setSheet(m_sheet);
            const int end = pos + num - 1;
            for (int col = pos; col <= end; ++col) {
                m_template->setColumn(col);
                m_sheet->insertColumnFormat(m_template);
            }
        }
        m_sheet->cellStorage()->insertColumns(pos, num);

        // undo deletion
        if (m_mode == Delete) {
            QUndoCommand::undo(); // process child commands (from CellStorage)
        }
    } else {
        // delete rows
        m_sheet->removeColumns(pos, num);
        m_sheet->cellStorage()->removeColumns(pos, num);

        // undo insertion
        if (m_mode == Insert) {
            QUndoCommand::undo(); // process child commands (from CellStorage)
        }
    }
    return true;
}

bool elementLeftColumnLessThan(const KSpread::Region::Element *e1, const KSpread::Region::Element *e2)
{
    return e1->rect().left() < e2->rect().left();
}

bool InsertDeleteColumnManipulator::preProcessing()
{
    if (m_firstrun) {
        // If we have an NCS, create a child command for each element.
        if (cells().count() > 1) { // non-contiguous selection
            // Sort the elements by their top row.
            qStableSort(cells().begin(), cells().end(), elementLeftColumnLessThan);
            // Create sub-commands.
            const Region::ConstIterator end(constEnd());
            for (Region::ConstIterator it = constBegin(); it != end; ++it) {
                InsertDeleteColumnManipulator *const command = new InsertDeleteColumnManipulator(this);
                command->setSheet(m_sheet);
                command->add(Region((*it)->rect(), (*it)->sheet()));
                if (m_mode == Delete) {
                    command->setReverse(true);
                }
            }
        } else { // contiguous selection
            m_sheet->cellStorage()->startUndoRecording();
        }
    }
    return AbstractRegionCommand::preProcessing();
}

bool InsertDeleteColumnManipulator::mainProcessing()
{
    if (cells().count() > 1) { // non-contiguous selection
        if ((m_reverse && m_mode == Insert) || (!m_reverse && m_mode == Delete)) {
            QUndoCommand::undo(); // process all sub-commands
        } else {
            QUndoCommand::redo(); // process all sub-commands
        }
        return true;
    }
    return AbstractRegionCommand::mainProcessing(); // calls process(Element*)
}

bool InsertDeleteColumnManipulator::postProcessing()
{
    if (cells().count() > 1) { // non-contiguous selection
        return true;
    }
    if (m_firstrun) {
        m_sheet->cellStorage()->stopUndoRecording(this);
    }
    const QRect rect(QPoint(boundingRect().left(), 1), QPoint(KS_colMax, KS_rowMax));
    m_sheet->map()->addDamage(new CellDamage(m_sheet, Region(rect, m_sheet), CellDamage::Appearance));
    return true;
}

/***************************************************************************
  class InsertDeleteRowManipulator
****************************************************************************/

InsertDeleteRowManipulator::InsertDeleteRowManipulator(QUndoCommand *parent)
        : AbstractRegionCommand(parent)
        , m_mode(Insert)
        , m_template(0)
{
    setText(i18n("Insert Rows"));
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

void InsertDeleteRowManipulator::setReverse(bool reverse)
{
    m_reverse = reverse;
    m_mode = reverse ? Delete : Insert;
    if (!m_reverse)
        setText(i18n("Insert Rows"));
    else
        setText(i18n("Remove Rows"));
}

bool InsertDeleteRowManipulator::process(Element* element)
{
    const QRect range = element->rect();
    const int pos = range.top();
    const int num = range.height();
    if (!m_reverse) { // insertion
        // insert rows
        m_sheet->insertRows(pos, num);
        if (m_template) {
            m_template->setSheet(m_sheet);
            const int end = pos + num - 1;
            for (int row = pos; row <= end; ++row) {
                m_template->setRow(row);
                m_sheet->insertRowFormat(m_template);
            }
        }
        m_sheet->cellStorage()->insertRows(pos, num);

        // undo deletion
        if (m_mode == Delete) {
            QUndoCommand::undo(); // process child commands (from CellStorage)
        }
    } else {
        // delete rows
        m_sheet->removeRows(pos, num);
        m_sheet->cellStorage()->removeRows(pos, num);

        // undo insertion
        if (m_mode == Insert) {
            QUndoCommand::undo(); // process child commands (from CellStorage)
        }
    }
    return true;
}

bool elementTopRowLessThan(const KSpread::Region::Element *e1, const KSpread::Region::Element *e2)
{
    return e1->rect().top() < e2->rect().top();
}

bool InsertDeleteRowManipulator::preProcessing()
{
    if (m_firstrun) {
        // If we have an NCS, create a child command for each element.
        if (cells().count() > 1) { // non-contiguous selection
            // Sort the elements by their top row.
            qStableSort(cells().begin(), cells().end(), elementTopRowLessThan);
            // Create sub-commands.
            const Region::ConstIterator end(constEnd());
            for (Region::ConstIterator it = constBegin(); it != end; ++it) {
                InsertDeleteRowManipulator *const command = new InsertDeleteRowManipulator(this);
                command->setSheet(m_sheet);
                command->add(Region((*it)->rect(), (*it)->sheet()));
                if (m_mode == Delete) {
                    command->setReverse(true);
                }
            }
        } else { // contiguous selection
            m_sheet->cellStorage()->startUndoRecording();
        }
    }
    return AbstractRegionCommand::preProcessing();
}

bool InsertDeleteRowManipulator::mainProcessing()
{
    if (cells().count() > 1) { // non-contiguous selection
        if ((m_reverse && m_mode == Insert) || (!m_reverse && m_mode == Delete)) {
            QUndoCommand::undo(); // process all sub-commands
        } else {
            QUndoCommand::redo(); // process all sub-commands
        }
        return true;
    }
    return AbstractRegionCommand::mainProcessing(); // calls process(Element*)
}

bool InsertDeleteRowManipulator::postProcessing()
{
    if (cells().count() > 1) { // non-contiguous selection
        return true;
    }
    if (m_firstrun) {
        m_sheet->cellStorage()->stopUndoRecording(this);
    }
    const QRect rect(QPoint(1, boundingRect().top()), QPoint(KS_colMax, KS_rowMax));
    m_sheet->map()->addDamage(new CellDamage(m_sheet, Region(rect, m_sheet), CellDamage::Appearance));
    return true;
}
