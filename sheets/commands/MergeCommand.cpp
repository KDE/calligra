/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005, 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "MergeCommand.h"

#include <KLocalizedString>
#include <kmessagebox.h>

#include "Cell.h"
#include "Damages.h"
#include "Map.h"
#include "ui/Selection.h" // FIXME detach from ui
#include "Sheet.h"

using namespace Calligra::Sheets;

MergeCommand::MergeCommand(KUndo2Command* parent)
        : AbstractRegionCommand(parent),
        m_merge(true),
        m_mergeHorizontal(false),
        m_mergeVertical(false),
        m_unmerger(0),
        m_selection(0)
{
    m_checkLock = true;
}

MergeCommand::~MergeCommand()
{
    delete m_unmerger;
}

bool MergeCommand::process(Element* element)
{
    if (element->type() != Element::Range || element->isRow() || element->isColumn()) {
        // TODO Stefan: remove these elements?!
        return true;
    }

    // sanity check
    if (m_sheet->isProtected() || m_sheet->map()->isProtected()) {
        return false;
    }

    QRect range = element->rect();
    int left   = range.left();
    int right  = range.right();
    int top    = range.top();
    int bottom = range.bottom();
    int height = range.height();
    int width  = range.width();

    bool doMerge = m_reverse ? (!m_merge) : m_merge;

    if (doMerge) {
        if (m_mergeHorizontal) {
            for (int row = top; row <= bottom; ++row) {
                int rows = 0;
                for (int col = left; col <= right; ++col) {
                    Cell cell = Cell(m_sheet, col, row);
                    if (cell.doesMergeCells()) {
                        rows = qMax(rows, cell.mergedYCells());
                        cell.mergeCells(col, row, 0, 0);
                    }
                }
                Cell cell = Cell(m_sheet,  left, row);
                if (!cell.isPartOfMerged()) {
                    cell.mergeCells(left, row, width - 1, rows);
                }
            }
        } else if (m_mergeVertical) {
            for (int col = left; col <= right; ++col) {
                int cols = 0;
                for (int row = top; row <= bottom; ++row) {
                    Cell cell = Cell(m_sheet, col, row);
                    if (cell.doesMergeCells()) {
                        cols = qMax(cols, cell.mergedXCells());
                        cell.mergeCells(col, row, 0, 0);
                    }
                }
                Cell cell = Cell(m_sheet, col, top);
                if (!cell.isPartOfMerged()) {
                    cell.mergeCells(col, top, cols, height - 1);
                }
            }
        } else {
            Cell cell = Cell(m_sheet,  left, top);
            cell.mergeCells(left, top, width - 1, height - 1);
        }
    } else { // dissociate
        for (int col = left; col <= right; ++col) {
            for (int row = top; row <= bottom; ++row) {
                Cell cell = Cell(m_sheet, col, row);
                if (!cell.doesMergeCells()) {
                    continue;
                }
                cell.mergeCells(col, row, 0, 0);
            }
        }
    }

    // adjust selection
    if (m_selection)
        m_selection->isEmpty() ? m_selection->initialize(range, m_sheet) : m_selection->extend(range, m_sheet);

    return true;
}

KUndo2MagicString MergeCommand::name() const
{
    if (m_merge) { // MergeCommand
        if (m_mergeHorizontal) {
            return kundo2_i18n("Merge Cells Horizontally");
        } else if (m_mergeVertical) {
            return kundo2_i18n("Merge Cells Vertically");
        } else {
            return kundo2_i18n("Merge Cells");
        }
    }
    return kundo2_i18n("Dissociate Cells");
}

bool MergeCommand::preProcessing()
{
    if (isColumnOrRowSelected()) {
        KMessageBox::information(0, i18n("Merging of columns or rows is not supported."));
        return false;
    }

    if (m_firstrun) {
        setText(name());

        // reduce the region to the region occupied by merged cells
        Region mergedCells;
        ConstIterator endOfList = constEnd();
        for (ConstIterator it = constBegin(); it != endOfList; ++it) {
            Element* element = *it;
            QRect range = element->rect();
            int right = range.right();
            int bottom = range.bottom();
            for (int row = range.top(); row <= bottom; ++row) {
                for (int col = range.left(); col <= right; ++col) {
                    Cell cell = Cell(m_sheet, col, row);
                    if (cell.doesMergeCells()) {
                        QRect rect(col, row, cell.mergedXCells() + 1, cell.mergedYCells() + 1);
                        mergedCells.add(rect);
                    }
                }
            }
        }

        if (m_merge) { // MergeCommand
            // we're in the manipulator's first execution
            // initialize the undo manipulator
            m_unmerger = new MergeCommand();
            if (!m_mergeHorizontal && !m_mergeVertical) {
                m_unmerger->setReverse(true);
            }
            m_unmerger->setSheet(m_sheet);
            m_unmerger->setRegisterUndo(false);
            m_unmerger->add(mergedCells);
        } else { // DissociateManipulator
            clear();
            add(mergedCells);
        }
    }

    if (m_merge) { // MergeCommand
        if (m_reverse) { // dissociate
        } else { // merge
            // Dissociate cells before merging the whole region.
            // For horizontal/vertical merging the cells stay
            // as they are. E.g. the region contains a merged cell
            // occupying two rows. Then the horizontal merge should
            // keep the height of two rows and extend the merging to the
            // region's width. In this case the unmerging is done while
            // processing each region element.
            if (!m_mergeHorizontal && !m_mergeVertical) {
                m_unmerger->redo();
            }
        }
    }
    // Clear the associated selection, if any. The merge/dissociate process will restore
    // selections. This ensures that the selection isn't broken after merging.
    if (m_selection) m_selection->Region::clear();

    return AbstractRegionCommand::preProcessing();
}

bool MergeCommand::postProcessing()
{
    if (m_merge) { // MergeCommand
        if (m_reverse) { // dissociate
            // restore the old merge status
            if (m_mergeHorizontal || m_mergeVertical) {
                m_unmerger->redo();
            } else {
                m_unmerger->undo();
            }
        }
    }
    m_sheet->map()->addDamage(new CellDamage(m_sheet, *this, CellDamage::Appearance));
    return true;
}
