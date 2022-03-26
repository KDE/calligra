/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "PageBreakCommand.h"

#include "engine/Damages.h"
#include "engine/MapBase.h"
#include "core/ColFormatStorage.h"
#include "core/RowFormatStorage.h"
#include "core/Sheet.h"
#include "core/SheetPrint.h"

using namespace Calligra::Sheets;

PageBreakCommand::PageBreakCommand(KUndo2Command *parent)
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
    Sheet *sheet = dynamic_cast<Sheet *>(element->sheet());
    const QRect range = element->rect();
    if (m_mode == BreakBeforeColumn && range.left() > 1) {
        sheet->columnFormats()->setPageBreak(range.left(), range.left(), enable);
    } else if (m_mode == BreakBeforeRow && range.top() > 1) {
        sheet->rowFormats()->setPageBreak(range.top(), range.top(), enable);
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
    if (m_sheet->isShowPageOutline()) {
        m_sheet->map()->addDamage(new SheetDamage(m_sheet, SheetDamage::ContentChanged));
    }
    return AbstractRegionCommand::postProcessing();
}
