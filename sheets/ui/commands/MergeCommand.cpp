/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005, 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// This is used by actions, as well as by ui/dialogs/LayoutDialog and ui/strategy/MergeStrategy

// Local
#include "MergeCommand.h"

#include <KLocalizedString>
#include <KMessageBox>

#include "core/Cell.h"
#include "core/Map.h"
#include "core/Sheet.h"
#include "engine/Damages.h"
#include "ui/Selection.h"

using namespace Calligra::Sheets;

MergeCommand::MergeCommand(KUndo2Command *parent)
    : AbstractRegionCommand(parent)
    , m_merge(true)
    , m_mergeHorizontal(false)
    , m_mergeVertical(false)
    , m_selection(nullptr)
{
    m_checkLock = true;
}

MergeCommand::~MergeCommand() = default;

bool MergeCommand::process(Element *element)
{
    if (element->isRow() || element->isColumn()) {
        // TODO Stefan: remove these elements?!
        return true;
    }

    QRect range = element->rect();
    int left = range.left();
    int right = range.right();
    int top = range.top();
    int bottom = range.bottom();
    int height = range.height();
    int width = range.width();

    if (m_merge) {
        if (m_mergeHorizontal) {
            for (int row = top; row <= bottom; ++row) {
                Cell cell = Cell(m_sheet, left, row);
                cell.mergeCells(left, row, width - 1, 0);
            }
        } else if (m_mergeVertical) {
            for (int col = left; col <= right; ++col) {
                Cell cell = Cell(m_sheet, col, top);
                cell.mergeCells(col, top, 0, height - 1);
            }
        } else {
            Cell cell = Cell(m_sheet, left, top);
            cell.mergeCells(left, top, width - 1, height - 1);
        }
    } else {
        Cell cell = Cell(m_sheet, left, top);
        cell.mergeCells(left, top, width - 1, height - 1, true); // dissociate
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

bool MergeCommand::preProcess()
{
    if (isColumnOrRowSelected()) {
        KMessageBox::information(nullptr, i18n("Merging of columns or rows is not supported."));
        return false;
    }

    if (m_firstrun)
        setText(name());

    return true;
}
