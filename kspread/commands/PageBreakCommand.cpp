/* This file is part of the KDE project
   Copyright 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#include "PageBreakCommand.h"

#include "RowColumnFormat.h"
#include "Sheet.h"
#include "SheetPrint.h"

using namespace KSpread;

PageBreakCommand::PageBreakCommand(QUndoCommand *parent)
    : AbstractRegionCommand(parent)
    , m_mode(BreakBeforeColumn)
{
}

PageBreakCommand::~PageBreakCommand()
{
}

void PageBreakCommand::setMode(Mode mode)
{
    m_mode = mode;
}

bool PageBreakCommand::process(Element *element)
{
    // No reverse means setting; reverse means unsetting.
    const bool enable = !m_reverse;
    Sheet *const sheet = element->sheet();
    const QRect range = element->rect();
    if (m_mode == BreakBeforeColumn && range.left() > 1) {
        sheet->nonDefaultColumnFormat(range.left())->setPageBreak(enable);
    } else if (m_mode == BreakBeforeRow && range.top() > 1) {
        sheet->nonDefaultRowFormat(range.top())->setPageBreak(enable);
    }
    return true;
}

bool PageBreakCommand::postProcessing()
{
    const QRect range = boundingRect();
    if (m_mode == BreakBeforeColumn && range.left() > 1) {
        m_sheet->print()->updateHorizontalPageParameters(range.left() - 1);
    } else if (m_mode == BreakBeforeRow && range.top() > 1) {
        m_sheet->print()->updateVerticalPageParameters(range.top() - 1);
    }
    if (m_sheet->isShowPageBorders()) {
        m_sheet->updateView();
    }
    return AbstractRegionCommand::postProcessing();
}

#include "PageBreakCommand.moc"
